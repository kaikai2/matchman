#ifndef MOVE_OBJECT_H
#define MOVE_OBJECT_H


#include "phy2d.h"
//////////////////////////////////////////////////////////////////////////
/**
假设MoveObject是一个圆
圆心在pos, 半径为radius
*/
class MapQuery;

class MoveObject
{
public:
    MoveObject() : map(0), radius(1.0), collisonDepth(0)
    {
    }
    void SetMapQuery(MapQuery *map)
    {
        this->map = map;
    }
    float GetRadius() const
    {
        return radius;
    }
    vector<Phy2d::CollisionInfo>& GetCollisionInfo()
    {
        return collisionInfos;
    }
    float &GetCollisionDepth()
    {
        return collisonDepth;
    }
    /// called by physics controller, once per frame, handle input here
    virtual void OnFrame(float delta) = 0;
    /// called by physics simulator, once every collision, this could happen more than one per frame, or none.
    virtual void OnCollide() = 0;
protected:
    vector2 velocity;
    vector2 pos;
    float radius; 
    MapQuery *map;

    vector<Phy2d::CollisionInfo> collisionInfos;// 最近一次发生碰撞的接触点法向量
    float collisonDepth;
};

#endif//MOVE_OBJECT_H
