#ifndef N_BBOX_H
#define N_BBOX_H
//------------------------------------------------------------------------------
/**
    @class bbox2
    @ingroup NebulaMathDataTypes

    A non-oriented bounding box class.

    (C) 2004 RadonLabs GmbH
*/
#include <cassert>
#include "_vector2.h"
//#include "mathlib/matrix.h"
//#include "mathlib/line.h"
//#include "mathlib/plane.h"
//#include "util/narray.h"

//------------------------------------------------------------------------------
//  bbox2
//------------------------------------------------------------------------------
class bbox2
{
public:
    /// clip codes
    enum
    {
        ClipLeft   = (1<<0),
        ClipRight  = (1<<1),
        ClipBottom = (1<<2),
        ClipTop    = (1<<3),
        ClipNear   = (1<<4),
        ClipFar    = (1<<5),
    };
    /// clip status
    enum ClipStatus
    {
        Outside,
        Inside,
        Clipped,
    };

    enum {
        OUTSIDE     = 0,
        ISEQUAL     = (1<<0),
        ISCONTAINED = (1<<1),
        CONTAINS    = (1<<2),
        CLIPS       = (1<<3),
    };

    /// constructor 1
    bbox2();
    /// constructor 3
    bbox2(const vector2& center, const vector2& extents);
    /// construct bounding box from matrix44
    //bbox2(const matrix44& m);
    /// get center point
    vector2 center() const;
    /// get extents of box
    vector2 extents() const;
    /// get size of box
    vector2 size() const;
    /// get diagonal size of box
    float diagonal_size() const;
    /// set from matrix44
    //void set(const matrix44& m);
    /// set from center point and extents
    void set(const vector2& center, const vector2& extents);
    /// begin extending the box
    void begin_extend();
    /// extend the box
    void extend(const vector2& v);
    /// this resets the bounding box size to zero if no extend() method was called after begin_extend()
    void end_extend();
    /// extend the box
    void extend(float x, float y);
    /// extend the box
    void extend(const bbox2& box);
    /// transform axis aligned bounding box
    //void transform(const matrix44& m);
    /// transform bounding box with divide by w
    //void transform_divw(const matrix44& m);
    /// check for intersection with axis aligned bounding box
    bool intersects(const bbox2& box) const;
    /// check if this box completely contains the parameter box
    bool contains(const bbox2& box) const;
    /// return true if this box contains the position
    bool contains(const vector2& pos) const;
    /// check for intersection with other bounding box
    ClipStatus clipstatus(const bbox2& other) const;
    /// check for intersection with projection volume
    //ClipStatus clipstatus(const matrix44& viewProjection) const;
    /// create a matrix which transforms an unit cube to this bounding box
    //matrix44 to_matrix44() const;
    /// return one of the 8 corner points
    vector2 corner_point(int index) const;
    /// return side planes in clip space
    //void get_clipplanes(const matrix44& viewProjection, nArray<plane>& outPlanes) const;

    int line_test(float v0, float v1, float w0, float w1);
    int intersect(const bbox2& box);
    //bool intersect(const line3& line, vector2& ipos) const;

#if 0
    // get point of intersection of 3d line with planes
    // on const x,y,z
    bool isect_const_x(const float x, const line3& l, vector2& out) const
    {
        if (l.m.x != 0.0f)
        {
            float t = (x - l.b.x) / l.m.x;
            if ((t >= 0.0f) && (t <= 1.0f))
            {
                // point of intersection...
                out = l.ipol(t);
                return true;
            }
        }
        return false;
    }
    bool isect_const_y(const float y, const line3& l, vector2& out) const
    {
        if (l.m.y != 0.0f)
        {
            float t = (y - l.b.y) / l.m.y;
            if ((t >= 0.0f) && (t <= 1.0f))
            {
                // point of intersection...
                out = l.ipol(t);
                return true;
            }
        }
        return false;
    }
    bool isect_const_z(const float z, const line3& l, vector2& out) const
    {
        if (l.m.z != 0.0f)
        {
            float t = (z - l.b.z) / l.m.z;
            if ((t >= 0.0f) && (t <= 1.0f))
            {
                // point of intersection...
                out = l.ipol(t);
                return true;
            }
        }
        return false;
    }

    // point in polygon check for sides with constant x,y and z
    bool pip_const_x(const vector2& p) const
    {
        if ((p.y >= vmin.y) && (p.y <= vmax.y) && (p.z >= vmin.z) && (p.z <= vmax.z)) return true;
        return false;
    }
    bool pip_const_y(const vector2& p) const
    {
        if ((p.x >= vmin.x) && (p.x <= vmax.x) && (p.z >= vmin.z) && (p.z <= vmax.z)) return true;
        return false;
    }
    bool pip_const_z(const vector2& p) const
    {
        if ((p.x >= vmin.x) && (p.x <= vmax.x) && (p.y >= vmin.y) && (p.y <= vmax.y)) return true;
        return false;
    }
#endif
    vector2 vmin;
    vector2 vmax;
};

//------------------------------------------------------------------------------
/**
*/
inline
bbox2::bbox2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bbox2::bbox2(const vector2& center, const vector2& extents)
{
    vmin = center - extents;
    vmax = center + extents;
}
#if 0
//------------------------------------------------------------------------------
/**
    Construct a bounding box around a 4x4 matrix. The translational part
    defines the center point, and the x,y,z vectors of the matrix
    define the extents.
*/
inline
void
bbox2::set(const matrix44& m)
{
    // get extents
    float xExtent = n_max(n_max(n_abs(m.M11), n_abs(m.M21)), n_abs(m.M31));
    float yExtent = n_max(n_max(n_abs(m.M12), n_abs(m.M22)), n_abs(m.M32));
    float zExtent = n_max(n_max(n_abs(m.M13), n_abs(m.M23)), n_abs(m.M33));
    vector2 extent(xExtent, yExtent, zExtent);

    vector2 center = m.pos_component();
    this->vmin = center - extent;
    this->vmax = center + extent;
}

//------------------------------------------------------------------------------
/**
*/
inline
bbox2::bbox2(const matrix44& m)
{
    this->set(m);
}
#endif
//------------------------------------------------------------------------------
/**
*/
inline
vector2
bbox2::center() const
{
    return vector2((vmin + vmax) * 0.5f);
}

//------------------------------------------------------------------------------
/**
*/
inline
vector2
bbox2::extents() const
{
    return vector2((vmax - vmin) * 0.5f);
}

//------------------------------------------------------------------------------
/**
*/
inline
vector2
bbox2::size() const
{
    return vector2(vmax - vmin);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
bbox2::set(const vector2& center, const vector2& extents)
{
    vmin = center - extents;
    vmax = center + extents;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
bbox2::begin_extend()
{
    vmin.set( 1000000.0f,  1000000.0f);
    vmax.set(-1000000.0f, -1000000.0f);
}

//------------------------------------------------------------------------------
/**
    This just checks whether the extend() method has actually been called after
    begin_extend() and just sets vmin and vmax to the null vector if it hasn't.
*/
inline
void
bbox2::end_extend()
{
    if (vmin.isequal(vector2( 1000000.0f,  1000000.0f), TINY) &&
        vmax.isequal(vector2(-1000000.0f, -1000000.0f), TINY))
    {
        vmin.set(0.0f, 0.0f);
        vmax.set(0.0f, 0.0f);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
bbox2::extend(const vector2& v)
{
    if (v.x < vmin.x) vmin.x = v.x;
    if (v.x > vmax.x) vmax.x = v.x;
    if (v.y < vmin.y) vmin.y = v.y;
    if (v.y > vmax.y) vmax.y = v.y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
bbox2::extend(float x, float y)
{
    if (x < vmin.x) vmin.x = x;
    if (x > vmax.x) vmax.x = x;
    if (y < vmin.y) vmin.y = y;
    if (y > vmax.y) vmax.y = y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
bbox2::extend(const bbox2& box)
{
    if (box.vmin.x < vmin.x) vmin.x = box.vmin.x;
    if (box.vmin.y < vmin.y) vmin.y = box.vmin.y;
    if (box.vmax.x > vmax.x) vmax.x = box.vmax.x;
    if (box.vmax.y > vmax.y) vmax.y = box.vmax.y;
}

//------------------------------------------------------------------------------
/**
    Returns one of the 8 corners of the bounding box.
*/
inline
vector2
bbox2::corner_point(int index) const
{
    assert((index >= 0) && (index < 4));
    switch (index)
    {
        case 0:     return this->vmin;
        case 1:     return vector2(this->vmin.x, this->vmax.y);
        case 2:     return vector2(this->vmax.x, this->vmin.y);
        case 3:     return this->vmax;
    }
}
#if 0
//------------------------------------------------------------------------------
/**
    Get the bounding box's side planes in clip space.
*/
inline
void
bbox2::get_clipplanes(const matrix44& viewProj, nArray<plane>& outPlanes) const
{
    matrix44 inv = viewProj;
    inv.invert();
    inv.transpose();

    vector4 planes[6];

    planes[0].set(-1, 0, 0, +this->vmax.x);
    planes[1].set(+1, 0, 0, -this->vmin.x);
    planes[2].set(0, -1, 0, +this->vmax.y);
    planes[3].set(0, +1, 0, -this->vmin.y);
    planes[4].set(0, 0, -1, +this->vmax.z);
    planes[5].set(0, 0, +1, -this->vmin.z);

    for (int i = 0; i < 6; ++i)
    {
        vector4 v = inv * planes[i];
        outPlanes.Append(plane(v.x, v.y, v.z, v.w));
    }
}

//------------------------------------------------------------------------------
/**
    Transforms this axis aligned bounding by the 4x4 matrix. This bounding
    box must be axis aligned with the matrix, the resulting bounding
    will be axis aligned in the matrix' "destination" space.

    E.g. if you have a bounding box in model space 'modelBox', and a
    'modelView' matrix, the operation

    modelBox.transform(modelView)

    would transform the bounding box into view space.
*/
inline
void
bbox2::transform(const matrix44& m)
{
    /*  ?? BUG ??
    // get own extents vector
    vector2 extents = this->extents();
    vector2 center  = this->center();

    // Extent the matrix' (x,y,z) components by our own extent
    // vector.
    matrix44 extentMatrix(
        m.M11 * extents.x, m.M12 * extents.x, m.M13 * extents.x, 0.0f,
        m.M21 * extents.y, m.M22 * extents.y, m.M23 * extents.y, 0.0f,
        m.M31 * extents.z, m.M32 * extents.z, m.M33 * extents.z, 0.0f,
        m.M41 + center.x,  m.M42 + center.y,  m.M43 + center.z,  1.0f);

    this->set(extentMatrix);
    */

    vector2 temp, min, max, corners[8];
    bool first = true;
    int i;

    corners[0]   = this->vmin;
    corners[1].x = this->vmin.x; corners[1].y = this->vmax.y; corners[1].z = this->vmin.z;
    corners[2].x = this->vmax.x; corners[2].y = this->vmax.y; corners[2].z = this->vmin.z;
    corners[3].x = this->vmax.x; corners[3].y = this->vmin.y; corners[3].z = this->vmin.z;
    corners[4]   = this->vmax;
    corners[5].x = this->vmin.x; corners[5].y = this->vmax.y; corners[5].z = this->vmax.z;
    corners[6].x = this->vmin.x; corners[6].y = this->vmin.y; corners[6].z = this->vmax.z;
    corners[7].x = this->vmax.x; corners[7].y = this->vmin.y; corners[7].z = this->vmax.z;

    for (i = 0; i < 8; ++i)
    {
        // Transform and check extents
        temp = m * corners[i];
        if (first || temp.x > max.x)   max.x = temp.x;
        if (first || temp.y > max.y)   max.y = temp.y;
        if (first || temp.z > max.z)   max.z = temp.z;
        if (first || temp.x < min.x)   min.x = temp.x;
        if (first || temp.y < min.y)   min.y = temp.y;
        if (first || temp.z < min.z)   min.z = temp.z;
        first = false;
    }

    this->vmin = min;
    this->vmax = max;
}

//------------------------------------------------------------------------------
/**
    Same as transform() but does a div-by-w on the way (useful for transforming
    to screen space).
*/
inline
void
bbox2::transform_divw(const matrix44& m)
{
    vector2 temp, min, max, corners[8];
    bool first = true;
    int i;

    corners[0]   = this->vmin;
    corners[1].x = this->vmin.x; corners[1].y = this->vmax.y; corners[1].z = this->vmin.z;
    corners[2].x = this->vmax.x; corners[2].y = this->vmax.y; corners[2].z = this->vmin.z;
    corners[3].x = this->vmax.x; corners[3].y = this->vmin.y; corners[3].z = this->vmin.z;
    corners[4]   = this->vmax;
    corners[5].x = this->vmin.x; corners[5].y = this->vmax.y; corners[5].z = this->vmax.z;
    corners[6].x = this->vmin.x; corners[6].y = this->vmin.y; corners[6].z = this->vmax.z;
    corners[7].x = this->vmax.x; corners[7].y = this->vmin.y; corners[7].z = this->vmax.z;

    for (i = 0; i < 8; ++i)
    {
        // Transform and check extents
        temp = m.mult_divw(corners[i]);
        if (first || temp.x > max.x)   max.x = temp.x;
        if (first || temp.y > max.y)   max.y = temp.y;
        if (first || temp.z > max.z)   max.z = temp.z;
        if (first || temp.x < min.x)   min.x = temp.x;
        if (first || temp.y < min.y)   min.y = temp.y;
        if (first || temp.z < min.z)   min.z = temp.z;
        first = false;
    }

    this->vmin = min;
    this->vmax = max;
}
//------------------------------------------------------------------------------
/**
    Check for intersection of 2 axis aligned bounding boxes. The
    bounding boxes must live in the same coordinate space.
*/
inline
bool
bbox2::intersects(const bbox2& box) const
{
    if ((this->vmax.x < box.vmin.x) || (this->vmin.x > box.vmax.x) ||
        (this->vmax.y < box.vmin.y) || (this->vmin.y > box.vmax.y))
    {
        return false;
    }
    return true;
}

#endif
//------------------------------------------------------------------------------
/**
    Check if the parameter bounding box is completely contained in this
    bounding box.
*/
inline
bool
bbox2::contains(const bbox2& box) const
{
    if ((this->vmin.x < box.vmin.x) && (this->vmax.x >= box.vmax.x) &&
        (this->vmin.y < box.vmin.y) && (this->vmax.y >= box.vmax.y))
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Check if position is inside bounding box.
*/
inline
bool
bbox2::contains(const vector2& v) const
{
    if ((this->vmin.x < v.x) && (this->vmax.x >= v.x) &&
        (this->vmin.y < v.y) && (this->vmax.y >= v.y))
    {
        return true;
    }
    return false;
}
#if 0
//------------------------------------------------------------------------------
/**
    Return box/box clip status.
*/
inline
bbox2::ClipStatus
bbox2::clipstatus(const bbox2& other) const
{
    if (this->contains(other))
    {
        return Inside;
    }
    if (this->intersects(other))
    {
        return Clipped;
    }
    return Outside;
}
//------------------------------------------------------------------------------
/**
    Check for intersection with a view volume defined by a view-projection
    matrix.
*/
inline
bbox2::ClipStatus
bbox2::clipstatus(const matrix44& viewProjection) const
{
    int andFlags = 0xffff;
    int orFlags  = 0;
    int i;
    static vector4 v0;
    static vector4 v1;
    for (i = 0; i < 8; i++)
    {
        int clip = 0;
        v0.w = 1.0f;
        if (i & 1) v0.x = this->vmin.x;
        else       v0.x = this->vmax.x;
        if (i & 2) v0.y = this->vmin.y;
        else       v0.y = this->vmax.y;
        if (i & 4) v0.z = this->vmin.z;
        else       v0.z = this->vmax.z;

        v1 = viewProjection * v0;

        if (v1.x < -v1.w)       clip |= ClipLeft;
        else if (v1.x > v1.w)   clip |= ClipRight;
        if (v1.y < -v1.w)       clip |= ClipBottom;
        else if (v1.y > v1.w)   clip |= ClipTop;
        if (v1.z < -v1.w)       clip |= ClipFar;
        else if (v1.z > v1.w)   clip |= ClipNear;
        andFlags &= clip;
        orFlags  |= clip;
    }
    if (0 == orFlags) return Inside;
    if (0 != andFlags) return Outside;
    return Clipped;
}

//------------------------------------------------------------------------------
/**
    Create a transform matrix which would transform an unit cube to this
    bounding box.
*/
inline
matrix44
bbox2::to_matrix44() const
{
    matrix44 m;
    m.scale(this->size());
    m.translate(this->center());
    return m;
}

//------------------------------------------------------------------------------
/**
    @brief Gets closest intersection with AABB.
    If the line starts inside the box,  start point is returned in ipos.
    @param line the pick ray
    @param ipos closest point of intersection if successful, trash otherwise
    @return true if an intersection occurs
*/
inline bool bbox2::intersect(const line3& line, vector2& ipos) const
{
    // Handle special case for start point inside box
    if (line.b.x >= vmin.x && line.b.y >= vmin.y &&
        line.b.x <= vmax.x && line.b.y <= vmax.y)
    {
        ipos = line.b;
        return true;
    }

    // Order planes to check, closest three only
    int plane[2];
    if (line.m.x > 0) plane[0] = 0;
    else              plane[0] = 1;
    if (line.m.y > 0) plane[1] = 2;
    else              plane[1] = 3;

    for (int i = 0; i < 3; ++i)
    {
        switch (plane[i])
        {
            case 0:
                if (isect_const_x(vmin.x,line,ipos) && pip_const_x(ipos)) return true;
                break;
            case 1:
                if (isect_const_x(vmax.x,line,ipos) && pip_const_x(ipos)) return true;
                break;
            case 2:
                if (isect_const_y(vmin.y,line,ipos) && pip_const_y(ipos)) return true;
                break;
            case 3:
                if (isect_const_y(vmax.y,line,ipos) && pip_const_y(ipos)) return true;
                break;
        }
    }

    return false;
}
#endif
//------------------------------------------------------------------------------
/**
*/
inline
int bbox2::line_test(float v0, float v1, float w0, float w1)
{
    // quick rejection test
    if ((v1 < w0) || (v0 > w1)) return OUTSIDE;
    if ((v0 == w0) && (v1 == w1)) return ISEQUAL;
    if ((v0 >= w0) && (v1 <= w1)) return ISCONTAINED;
    if ((v0 <= w0) && (v1 >= w1)) return CONTAINS;
    return CLIPS;
}
//------------------------------------------------------------------------------
/**
    Check if box intersects, contains or is contained in other box
    by doing 3 projection tests for each dimension, if all 3 test
    return true, then the 2 boxes intersect.
*/
inline
int bbox2::intersect(const bbox2& box)
{
    int and_code = 0xffff;
    int or_code = 0;
    int cx = line_test(vmin.x, vmax.x, box.vmin.x, box.vmax.x);
    and_code &= cx;
    or_code |= cx;
    int cy = line_test(vmin.y, vmax.y, box.vmin.y, box.vmax.y);
    and_code &= cy;
    or_code |= cy;
    if (or_code == 0) return OUTSIDE;
    if (and_code != 0) return and_code;
    // only if all test produced a non-outside result,
    // an intersection has occurred
    if (cx && cy) return CLIPS;
    return OUTSIDE;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
bbox2::diagonal_size() const
{
    return (this->vmin - this->vmax).len();
}
//------------------------------------------------------------------------------
#endif
