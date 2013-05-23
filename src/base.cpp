#include "_vector2.h"
#include "bbox.h"
#include <vector>

namespace Phy2d
{
    using namespace std;
    ///
    /*
    Geom 包括基本的直线段、圆弧线段、和可以攀爬的竖线段。
    Space 包含内可以容纳Geom
    Geom 支持平移、不支持旋转
    */

    class RigidBody;
    class Geom
    {
    public:
        virtual bool CanGrab() const
        {
            return false;
        }
        /// move from 'from' to 'to', but may collide at the collide position
        virtual bool Collision(const vector2 &from, const vector2 &to, vector2 &collide) const
        {
            return false;
        }
        const bbox2& GetBBox() const
        {
            return boundingBox;
        }
    protected:
        bbox2 boundingBox;
        RigidBody *body;
    };
    typedef Geom* GeomPtr;
    // 线段
    class LineSegmentGeom : public Geom
    {
    public:
        LineSegmentGeom()
        {
        }
        void SetLineSegment(const vector2 &from, const vector2 &to)
        {
            a = from;
            b = to;
            boundingBox.begin_extend();
            boundingBox.extend(a);
            boundingBox.extend(b);
            boundingBox.end_extend();
        }
        virtual bool Collision(const vector2 &from, const vector2 &to, vector2 &collide) const
        {
            // FIXME: implement me
            // boundingbox test
            {
                bbox2 b;
                b.begin_extend();
                b.extend(from);
                b.extend(to);
                b.end_extend();

                if (b.intersect(boundingBox) == bbox2::OUTSIDE)
                    return false;
            }

            if (cross_product(from - a, b - a) * cross_product(b - a, to - a) > 0 &&
                cross_product(a - to, from - to) * cross_product(from - to, b - to) > 0)
            {
                /*
                p = from + k * (to - from)
                */
                float k = (cross_product(a, from) + cross_product(b, a) + cross_product(from, b)/* + cross_product(a, a)*/) /
                    (cross_product(to, a) + cross_product(a, from) + cross_product(b, to) + cross_product(from, b));
                collide.lerp(from, to, k);
                return true;
            }

            return false;
        }
    protected:
        vector2 a, b;
    };
    // 弧线
    class ArcGeom : public Geom
    {
    public:
        void SetArc(const vector2 &center, const vector2 &arc, float radian)
        {
            this->center = center;
            this->arc = arc;
            this->radian = radian;
            boundingBox.begin_extend();
            boundingBox.extend(arc);
            vector2 oa(arc - center);
            vector2 oa1(oa), oa2(oa);
            oa1.rotate(radian);
            oa2.rotate(-radian);
            boundingBox.extend(center + oa1);
            boundingBox.extend(center + oa2);
            oa.norm();
            if (acos(dot_product(vector2(0, 1), oa)) < radian)
            {
                boundingBox.extend(center + vector2(0, radian));
            }
            if (acos(dot_product(vector2(0, -1), oa)) < radian)
            {
                boundingBox.extend(center + vector2(0, -radian));
            }
            if (acos(dot_product(vector2(1, 0), oa)) < radian)
            {
                boundingBox.extend(center + vector2(radian, 0));
            }
            if (acos(dot_product(vector2(-1, 0), oa)) < radian)
            {
                boundingBox.extend(center + vector2(-radian, 0));
            }
            boundingBox.end_extend();
        }
        virtual bool Collision(const vector2 &from, const vector2 &to, vector2 &collide) const
        {
            // boundingbox test
            bbox2 b;
            b.begin_extend();
            b.extend(from);
            b.extend(to);
            b.end_extend();

            if (b.intersect(boundingBox) == bbox2::OUTSIDE)
                return false;

            /*
            |from + k * (to - from) - center| = radius
            a = from - center
            b = to - from
            |a + k * b| = radius
            |b|^2 * k^2 + 2(a dot b)k + |a|^2 - r^2 = 0

            k=
            */
            {
                vector2 a(from - center);
                vector2 b(to - from);
                vector2 ta(arc - center);
                float alenS = a.x * a.x + a.y * a.y;
                float blenS = b.x * b.x + b.y * b.y;
                float adotb2 = 2 * dot_product(a, b);
                float c = alenS - ta.x * ta.x - ta.y * ta.y;
                float delta = sqrt(adotb2 * adotb2 - 4 * blenS * c);
                float k1 = (- adotb2 + delta) / (2 * blenS);
                float k2 = (- adotb2 - delta) / (2 * blenS);
                if (k1 > k2)
                    swap(k1, k2);

                ta.norm();
                if (k1 >= 0)
                {
                    a.lerp(from, to, k1);
                    vector2 t(a - center);
                    t.norm();
                    if (acos(dot_product(ta, t)) < radian)
                    {
                        collide = a;
                        return true;
                    }
                }
                if (k2 >= 0)
                {
                    b.lerp(from, to, k2);
                    vector2 t(b - center);
                    t.norm();
                    if (acos(dot_product(ta, t)) < radian)
                    {
                        collide = b;
                        return true;
                    }
                }
            }
            return false;
        }
        vector2 center;
        vector2 arc;
        float radian;
        /*
        center at point 'o'
        arc at point 'a'
        radius is length between 'o' and 'a'
        radian is (0, PI], at both side of a, size of cornor c,o,a
          c-_ 
             \
              a
           r   |
        o      |
        
        */
    };
    class WallGeom : public Geom
    {
    public:
        virtual bool CanGrab() const
        {
            return true;
        }
        virtual bool Collision(const vector2 &from, const vector2 &to, vector2 &collide) const
        {
            // FIXME: implement me
            return false;
        }
        vector2 center;
        float height;
        /*
        center is at point 'o'
        height is the total height of the wall
           |
           |
           o
           |
           |
        */
    };
    class RigidBody
    {
    public:
        vector<GeomPtr> geoms;
        vector2 pos;
        vector2 vel;
    };

    class Space : public Geom
    {
    public:
        virtual void AddGeom(GeomPtr geom)
        {
            geoms.push_back(geom);
        }
        /// move from 'from' to 'to', but may collide at the collide position
        virtual bool Collision(const vector2 &from, const vector2 &to, vector2 &collide) const
        {
            bool flag = false;
            vector2 newCollidePosition;
            collide = to;
            for (vector<GeomPtr>::const_iterator g = geoms.begin(); g != geoms.end(); ++g)
            {
                if ((*g)->Collision(from, collide, newCollidePosition))
                {
                    collide = newCollidePosition;
                    flag = true;
                }
            }
            return flag;
        }
        vector<GeomPtr> geoms;
    };
    typedef Space* SpacePtr;
    
    class QuadSplitSpace : public Space
    {
    public:
        /*
             ^ y
         LT  |  RT
        -----+------> x
         LB  |  RB
        */
        enum SubSpaceIndex
        {
            LeftTop,
            RightTop,
            LeftBottm,
            RightBottom,

            NumSubSpaces,
        };

        /// move from 'from' to 'to', but may collide at the collide position
        virtual bool Collision(const vector2 &from, const vector2 &to, vector2 &collide) const
        {
            bool flag = false;
            vector2 newCollidePosition;
            collide = to;
            for (vector<GeomPtr>::const_iterator g = geoms.begin(); g != geoms.end(); ++g)
            {
                if ((*g)->Collision(from, collide, newCollidePosition))
                {
                    collide = newCollidePosition;
                    flag = true;
                }
            }

            // check childs
            for (SubSpaceIndex subSpaceIndex = SubSpaceIndex(0); subSpaceIndex < NumSubSpaces; subSpaceIndex = SubSpaceIndex(subSpaceIndex + 1))
            {
                if (PointInSubSpace(from, subSpaceIndex) || PointInSubSpace(collide, subSpaceIndex) ||
                    LineCrossSubSpace(from, collide, subSpaceIndex))
                {
                    if (child[subSpaceIndex]->Collision(from, collide, newCollidePosition))
                    {
                        collide = newCollidePosition;
                        flag = true;
                    }
                }
            }

            return flag;
        }

        bool PointInSubSpace(const vector2 &p, SubSpaceIndex index) const
        {
            switch(index)
            {
            case LeftTop:
                return p.x < splitCenter.x && p.y > splitCenter.y;
            case RightTop:
                return p.x > splitCenter.x && p.y > splitCenter.y;
            case LeftBottm:
                return p.x < splitCenter.x && p.y < splitCenter.y;
            case RightBottom:
                return p.x > splitCenter.x && p.y < splitCenter.y;
            }
            assert(0);
            return false;
        }
        bool LineCrossSubSpace(const vector2 &from, const vector2 &to, SubSpaceIndex index) const
        {
            vector2 axis_pos; // all zeros
            vector2 off(to - from);
            if (abs(off.x) < TINY)
            {
                if (from.y > 0 && to.y < 0 || from.y < 0 && to.y > 0)
                    axis_pos.x = off.x;
            }
            else if (abs(off.y) < TINY)
            {
                if (from.x > 0 && to.x < 0 || from.x < 0 && to.x > 0)
                    axis_pos.y = off.y;
            }
            else
            {
                if (from.y > 0 && to.y < 0 || from.y < 0 && to.y > 0)
                    axis_pos.x = from.x + off.x * from.y / off.y;

                if (from.x > 0 && to.x < 0 || from.x < 0 && to.x > 0)
                    axis_pos.y = from.y + off.y * from.x / off.x;
            }
            switch(index)
            {
            case LeftTop:
                return axis_pos.x < -TINY || axis_pos.y > TINY;
            case RightTop:
                return axis_pos.x > TINY || axis_pos.y > TINY;
            case LeftBottm:
                return axis_pos.x < -TINY || axis_pos.y < -TINY;
            case RightBottom:
                return axis_pos.x > TINY || axis_pos.y < -TINY;
            }
            assert(0);
            return false;
        }
    
        virtual void AddGeom(GeomPtr geom)
        {
            newgeoms.push_back(geom);
        }

        void Update()
        {
            if (!newgeoms.empty())
            {
                for (vector<GeomPtr>::const_iterator g = newgeoms.begin(); g != newgeoms.end(); ++g)
                {
                    const bbox2 &box= (*g)->GetBBox();
                    //box.vmin.x
                }
                newgeoms.clear();
            }
        }

        vector2 splitCenter;

        // all geoms in child must stay in its sub-area and not cross splitCenter-defined cross lines
        SpacePtr child[NumSubSpaces];

        vector<GeomPtr> newgeoms;
    };
}