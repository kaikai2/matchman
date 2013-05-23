#ifndef vector2_H
#define vector2_H
//------------------------------------------------------------------------------
/**
    @class vector2
    @ingroup NebulaMathDataTypes

    Generic vector2 class.

    (C) 2002 RadonLabs GmbH
*/
#include <cmath>
#include <float.h>
#define TINY 1e-6

//------------------------------------------------------------------------------
class vector2
{
public:
    static const vector2 zero;

public:
    /// constructor 1
    vector2();
    /// constructor 2
    vector2(const float _x, const float _y);
    /// constructor 3
    vector2(const vector2& vec);
    /// constructor 4
    vector2(const float* p);
    /// set elements 1
    void set(const float _x, const float _y);
    /// set elements 2
    void set(const vector2& vec);
    /// set elements 3
    void set(const float* p);
    /// return length
    float len() const;
    /// normalize
    void norm();
    /// in place add
    void operator+=(const vector2& v0);
    /// in place sub
    void operator-=(const vector2& v0);
    /// in place scalar mul
    void operator*=(const float s);
    /// in place scalar div
    void operator/=(const float s);
    /// true if all elements are equal
    bool operator ==(const vector2& v0);
    /// true if any of the elements is not equal
    bool operator !=(const vector2& v0);
    /// fuzzy compare operator
    bool isequal(const vector2& v, const float tol) const;
    /// fuzzy compare, returns -1, 0, +1
    int compare(const vector2& v, float tol) const;
    /// rotate around P(0,0)
    void rotate(float angle);
    /// inplace linear interpolation
    void lerp(const vector2& v0, float lerpVal);
    /// linear interpolation between v0 and v1
    void lerp(const vector2& v0, const vector2& v1, float lerpVal);

    friend float dot_product(const vector2& v0, const vector2& v1);
    friend float cross_product(const vector2& v0, const vector2& v1);
    float x, y;
};

//------------------------------------------------------------------------------
/**
*/
inline
vector2::vector2() :
    x(0.0f),
    y(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
vector2::vector2(const float _x, const float _y) :
    x(_x),
    y(_y)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
vector2::vector2(const vector2& vec) :
    x(vec.x),
    y(vec.y)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
vector2::vector2(const float* p) :
    x(p[0]),
    y(p[1])
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector2::set(const float _x, const float _y)
{
    x = _x;
    y = _y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector2::set(const vector2& v)
{
    x = v.x;
    y = v.y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector2::set(const float* p)
{
    x = p[0];
    y = p[1];
}

//------------------------------------------------------------------------------
/**
*/
inline
float
vector2::len() const
{
    return (float) sqrt(x * x + y * y);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector2::norm()
{
    float l = len();
    if (l > TINY)
    {
        x /= l;
        y /= l;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector2::operator +=(const vector2& v0)
{
    x += v0.x;
    y += v0.y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector2::operator -=(const vector2& v0)
{
    x -= v0.x;
    y -= v0.y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector2::operator *=(const float s)
{
    x *= s;
    y *= s;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector2::operator /=(const float s)
{
    x /= s;
    y /= s;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
vector2::operator ==(const vector2& rhs)
{
    if ((this->x == rhs.x) && (this->y == rhs.y))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
vector2::operator !=(const vector2& rhs)
{
    if ((this->x != rhs.x) || (this->y != rhs.y))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
vector2::isequal(const vector2& v, const float tol) const
{
    if (fabs(v.x - x) > tol)      return false;
    else if (fabs(v.y - y) > tol) return false;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
vector2::compare(const vector2& v, float tol) const
{
    if (fabs(v.x - x) > tol)      return (v.x > x) ? +1 : -1;
    else if (fabs(v.y - y) > tol) return (v.y > y) ? +1 : -1;
    else                          return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector2::rotate(float angle)
{
    // rotates this one around P(0,0).
    float sa, ca;

    sa = (float) sin(angle);
    ca = (float) cos(angle);

    // "handmade" multiplication
    vector2 help(ca * this->x - sa * this->y,
                  sa * this->x + ca * this->y);

    *this = help;
}

//------------------------------------------------------------------------------
/**
*/
static
inline
vector2 operator +(const vector2& v0, const vector2& v1)
{
    return vector2(v0.x + v1.x, v0.y + v1.y);
}

//------------------------------------------------------------------------------
/**
*/
static
inline
vector2 operator -(const vector2& v0, const vector2& v1)
{
    return vector2(v0.x - v1.x, v0.y - v1.y);
}

//------------------------------------------------------------------------------
/**
*/
static
inline
vector2 operator *(const vector2& v0, const float s)
{
    return vector2(v0.x * s, v0.y * s);
}

//------------------------------------------------------------------------------
/**
*/
static
inline
vector2 operator -(const vector2& v)
{
    return vector2(-v.x, -v.y);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector2::lerp(const vector2& v0, float lerpVal)
{
    x = v0.x + ((x - v0.x) * lerpVal);
    y = v0.y + ((y - v0.y) * lerpVal);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
vector2::lerp(const vector2& v0, const vector2& v1, float lerpVal)
{
    x = v0.x + ((v1.x - v0.x) * lerpVal);
    y = v0.y + ((v1.y - v0.y) * lerpVal);
}

//------------------------------------------------------------------------------
/**
*/
inline
float 
dot_product(const vector2& v0, const vector2& v1)
{
    return v0.x * v1.x + v0.y * v1.y;
}

//------------------------------------------------------------------------------
/**
*/
inline
float 
cross_product(const vector2& v0, const vector2& v1)
{
    return v0.x * v1.y - v0.y * v1.x;
}

//------------------------------------------------------------------------------
#endif

