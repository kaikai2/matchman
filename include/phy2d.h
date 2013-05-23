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
    Geom 包括基本的直线段、圆弧线段
    Space 包含内可以容纳Geom
    Geom 支持平移、不支持旋转
    这里仅仅实现circle与直线段、圆弧线段的碰撞
    */
    struct CollisionInfo
    {
        CollisionInfo() : depth(0.0f), force(0)
        {

        }
        // 碰撞检测的结果
        vector2 pos;    // 发生碰撞的接触位置
        vector2 normal; // 远离接触位置的方向（互相作用的2个物体得到的normal相反）
        float depth;    // 接触深度 0表示紧贴，>0表示产生一定量的形变，<0不会出现
        // 动态平衡的结果
        float force;    // 实际提供的支持力
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
        // collideinfo是一个vector，表示可能发生多个碰撞
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
        bool dirty; // 发生移动

        void *data;
    };
    typedef Geom* GeomPtr;
    // 线段
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
    // 弧线
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
        // 更新geom的位置，如果发生转移，返回true，否则返回false
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

        SpacePtr topSpace; // 当发现dirty时，转送到该space去
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
                if ((*g)->GetDirty()) // 移动过
                {
                    topSpace->AddGeom(*g); // 需要重新分配位置
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
