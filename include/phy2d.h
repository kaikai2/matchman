#ifndef PHY2d_H
#define PHY2d_H

#include "_vector2.h"
#include "_matrix33.h"
#include "bbox.h"
#include <vector>

namespace Phy2d
{
    using namespace std;
    ///
    /*
    Geom ����������ֱ�߶Ρ�Բ���߶�
    Space �����ڿ�������Geom
    Geom ֧��ƽ�ơ���֧����ת
    �������ʵ��circle��ֱ�߶Ρ�Բ���߶ε���ײ
    */
    struct CollisionInfo
    {
        CollisionInfo() : depth(0.0f), force(0)
        {

        }
        // ��ײ���Ľ��
        vector2 pos;    // ������ײ�ĽӴ�λ��
        vector2 normal; // Զ��Ӵ�λ�õķ��򣨻������õ�2������õ���normal�෴��
        float depth;    // �Ӵ���� 0��ʾ������>0��ʾ����һ�������α䣬<0�������
        // ��̬ƽ��Ľ��
        float force;    // ʵ���ṩ��֧����
    };
    class RigidBody;
    class Geom
    {
    public:
        enum GeomType
        {
            LineSeg,
            Arc,
            Wall,
            Space,
        };
        virtual GeomType GetType() const = 0;
        Geom() : dirty(true)
        {
        }
        virtual bool CanGrab() const
        {
            return false;
        }
        virtual float GetDistance(const vector2 &point, vector2 &shadow) const
        {
            return 0;
        }
        /// move from 'from' to 'to', but may collide at the collide position
        virtual bool CollisionRay(const vector2 &from, const vector2 &to, vector<CollisionInfo> &collideinfo)
        {
            return false;
        }
        // collideinfo��һ��vector����ʾ���ܷ��������ײ
        virtual bool CollisionCircle(float radius, const vector2 &from, const vector2 &to, vector<CollisionInfo> &collideinfo)
        {
            return false;
        }
        const bbox2& GetBBox() const
        {
            return boundingBox;
        }
        bool GetDirty()
        {
            if (dirty)
            {
                dirty = false;
                return true;
            }
            return false;
        }
        void SetData(void *data)
        {
            this->data = data;
        }
        void *GetData() const
        {
            return this->data;
        }
        virtual const vector2 &GetVector2(size_t index) const
        {
            return vector2::zero;
        }
        virtual float GetFloat(size_t index) const
        {
            return 0;
        }
    protected:
        bbox2 boundingBox;

        // transform
        vector2 toParent;
        vector2 toWorld;

        RigidBody *body;
        bool dirty; // �����ƶ�

        void *data;
    };
    typedef Geom* GeomPtr;
    // �߶�
    class LineSegmentGeom : public Geom
    {
    public:
        LineSegmentGeom()
        {
        }
        virtual GeomType GetType() const
        {
            return Geom::LineSeg;
        }
        void SetLineSegment(const vector2 &from, const vector2 &to)
        {
            a = from;
            b = to;
            boundingBox.begin_extend();
            boundingBox.extend(a);
            boundingBox.extend(b);
            boundingBox.end_extend();
            dirty = true;
        }
        virtual const vector2 &GetVector2(size_t index) const
        {
            return index == 0 ? a : b;
        }

        virtual float GetDistance(const vector2 &point, vector2 &shadow) const;
        virtual bool CollisionRay(const vector2 &from, const vector2 &to, vector<CollisionInfo> &collideinfo);
        virtual bool CollisionCircle(float radius, const vector2 &from, const vector2 &to, vector<CollisionInfo> &collideinfo);
        vector2 a, b;
    };
    // ����
    class ArcGeom : public Geom
    {
    public:
        virtual GeomType GetType() const
        {
            return Geom::Arc;
        }
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
            dirty = true;
        }
        virtual const vector2 &GetVector2(size_t index) const
        {
            return index == 0 ? center : arc;
        }
        virtual float GetFloat(size_t index) const
        {
            return radian;
        }
        virtual float GetDistance(const vector2 &point, vector2 &shadow) const;
        virtual bool CollisionRay(const vector2 &from, const vector2 &to, vector<CollisionInfo> &collideinfo);
        virtual bool CollisionCircle(float radius, const vector2 &from, const vector2 &to, vector<CollisionInfo> &collideinfo);
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
#if 0
    class WallGeom : public Geom
    {
    public:
        virtual GeomType GetType() const
        {
            return Geom::Wall;
        }

        virtual bool CanGrab() const
        {
            return true;
        }
        virtual const vector2 &GetVector2(size_t index) const
        {
            return center;
        }
        virtual float GetFloat(size_t index) const
        {
            return height;
        }
        virtual bool Collision(const vector2 &from, const vector2 &to, vector2 &collide)
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
#endif
    class Space;
    typedef Space* SpacePtr;

    class Space : public Geom
    {
    public:
        Space() : topSpace(this)
        {
        }
        virtual GeomType GetType() const
        {
            return Geom::Space;
        }

        virtual void AddGeom(GeomPtr geom)
        {
            newgeoms.push_back(geom);
        }
        /// move from 'from' to 'to', but may collide at the collide position
        virtual bool CollisionRay(const vector2 &from, const vector2 &to, vector<CollisionInfo> &collideinfo)
        {
            for (vector<GeomPtr>::const_iterator g = geoms.begin(); g != geoms.end(); ++g)
            {
                vector<CollisionInfo> ci;
                if ((*g)->CollisionRay(from, to, ci))
                {
                    collideinfo.insert(collideinfo.end(), ci.begin(), ci.end());
                    lastCollision = *g;
                }
            }
            return lastCollision != 0;
        }
        virtual bool CollisionCircle(float radius, const vector2 &from, const vector2 &to, vector<CollisionInfo> &collideinfo)
        {
            lastCollision = 0;
            for (vector<GeomPtr>::const_iterator g = geoms.begin(); g != geoms.end(); ++g)
            {
                vector<CollisionInfo> ci;
                if ((*g)->CollisionCircle(radius, from, to, ci))
                {
                    collideinfo.insert(collideinfo.end(), ci.begin(), ci.end());
                    lastCollision = *g;
                }
            }
            return lastCollision != 0;
        }
        virtual GeomPtr GetLastCollision() const 
        {
            return this->lastCollision;
        }
        // ����geom��λ�ã��������ת�ƣ�����true�����򷵻�false
        virtual bool Update()
        {
            bool flag = false;
            geoms.insert(geoms.end(), newgeoms.begin(), newgeoms.end());
            newgeoms.clear();
            for (vector<GeomPtr>::iterator g = geoms.begin(); g != geoms.end();)
            {
                if ((*g)->GetDirty() && topSpace != this) // remove dirty flag, but do nothing
                {
                    topSpace->AddGeom(*g);
                    *g = geoms.back();
                    geoms.pop_back();
                    flag = true;
                }
                else
                    ++g;
            }
            return flag;
        }
        const vector<GeomPtr> &GetGeoms() const
        {
            return geoms;
        }

        void Clear()
        {
            newgeoms.clear();
            geoms.clear();
            lastCollision = 0;
        }
    protected:
        vector<GeomPtr> newgeoms;
        vector<GeomPtr> geoms;

        GeomPtr lastCollision;

        SpacePtr topSpace; // ������dirtyʱ��ת�͵���spaceȥ
    };
#if 0
    class QuadSplitSpace : public Space
    {
    public:
        QuadSplitSpace()
        {

        }
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
            LeftBottom,
            RightBottom,

            NumSubSpaces,
        };

        /// move from 'from' to 'to', but may collide at the collide position
        virtual bool CollisionRay(const vector2 &from, const vector2 &to, vector2 &collide, vector2 &normal)
        {
            bool flag = false;
            vector2 newCollidePosition;
            vector2 newNormal;
            collide = to;
            for (vector<GeomPtr>::const_iterator g = geoms.begin(); g != geoms.end(); ++g)
            {
                if ((*g)->CollisionRay(from, collide, newCollidePosition, newNormal))
                {
                    collide = newCollidePosition;
                    normal = newNormal;
                    flag = true;
                }
            }

            // check childs
            for (SubSpaceIndex subSpaceIndex = SubSpaceIndex(0); subSpaceIndex < NumSubSpaces; subSpaceIndex = SubSpaceIndex(subSpaceIndex + 1))
            {
                if (PointInSubSpace(from, subSpaceIndex) || PointInSubSpace(collide, subSpaceIndex) ||
                    LineCrossSubSpace(from, collide, subSpaceIndex))
                {
                    if (child[subSpaceIndex]->CollisionRay(from, collide, newCollidePosition, newNormal))
                    {
                        collide = newCollidePosition;
                        normal = newNormal;
                        flag = true;
                    }
                }
            }

            return flag;
        }

        /// move from 'from' to 'to', but may collide at the collide position
        virtual bool CollisionCircle(float radius, const vector2 &from, const vector2 &to, vector2 &collide, vector2 &normal, float &collisionDepth)
        {
            bool flag = false;
            vector2 newCollidePosition;
            vector2 newNormal;
            float newCollisionDepth;
            collide = to;
            for (vector<GeomPtr>::const_iterator g = geoms.begin(); g != geoms.end(); ++g)
            {
                if ((*g)->CollisionCircle(radius, from, collide, newCollidePosition, newNormal, newCollisionDepth))
                {
                    collide = newCollidePosition;
                    normal = newNormal;
                    collisionDepth = newCollisionDepth;
                    flag = true;
                }
            }

            // check childs
            for (SubSpaceIndex subSpaceIndex = SubSpaceIndex(0); subSpaceIndex < NumSubSpaces; subSpaceIndex = SubSpaceIndex(subSpaceIndex + 1))
            {
                if (PointInSubSpace(from, subSpaceIndex) || PointInSubSpace(collide, subSpaceIndex) ||
                    LineCrossSubSpace(from, collide, subSpaceIndex))
                {
                    if (child[subSpaceIndex]->CollisionCircle(radius, from, collide, newCollidePosition, newNormal, newCollisionDepth))
                    {
                        collide = newCollidePosition;
                        normal = newNormal;
                        collisionDepth = newCollisionDepth;
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
            case LeftBottom:
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
            case LeftBottom:
                return axis_pos.x < -TINY || axis_pos.y < -TINY;
            case RightBottom:
                return axis_pos.x > TINY || axis_pos.y < -TINY;
            }
            assert(0);
            return false;
        }
    
        virtual bool Update()
        {
            bool flag = false;
            for (vector<GeomPtr>::iterator g = newgeoms.begin(); g != newgeoms.end();)
            {
                if ((*g)->GetDirty()) // �ƶ���
                {
                    topSpace->AddGeom(*g); // ��Ҫ���·���λ��
                    *g = newgeoms.back();
                    newgeoms.pop_back();
                    flag = true;
                }
                else
                    ++g;
            }
            if (!newgeoms.empty())
            {
                for (vector<GeomPtr>::const_iterator g = newgeoms.begin(); g != newgeoms.end(); ++g)
                {
                    const bbox2 &box= (*g)->GetBBox();
                    if (box.vmin.x > splitCenter.x)
                    {
                        if (box.vmin.y > splitCenter.y)
                            child[RightTop]->AddGeom(*g);
                        else if (box.vmax.y < splitCenter.y)
                            child[RightBottom]->AddGeom(*g);
                        else
                            geoms.push_back(*g);
                    }
                    else if (box.vmax.x < splitCenter.x)
                    {
                        if (box.vmin.y > splitCenter.y)
                            child[LeftTop]->AddGeom(*g);
                        else if (box.vmax.y < splitCenter.y)
                            child[LeftBottom]->AddGeom(*g);
                        else
                            geoms.push_back(*g);
                    }
                    else
                        geoms.push_back(*g);
                }
                newgeoms.clear();
            }
            return flag;
        }

        vector2 splitCenter;

        // all geoms in child must stay in its sub-area and not cross splitCenter-defined cross lines
        SpacePtr child[NumSubSpaces];
    };
#endif
}

#endif
