#ifndef MAP_QUERY_H
#define MAP_QUERY_H

#include "_vector2.h"
class MoveObject;

class MapQuery
{
public:
    enum CollisionType
    {
        CT_None,    // no collision
        CT_Ground,
        CT_Wall,
    };
    /// an object is trying to move from from to to, it will move to position suggest if return value is true
    /// @param in from, to
    /// @param out suggest
    /// @return collision type
    virtual CollisionType QueryMove(MoveObject *object, const vector2 &from, const vector2 &to, vector2 &suggest)
    {
        suggest = to; // move as wish
        return CT_None;
    }
};
#endif
