#include <cmath>
#include <cstdlib>
#include "phy2d.h"

namespace Phy2d
{
const float Pi = acos(-1.0f);
float LineSegmentGeom::GetDistance(const vector2 &point, vector2 &shadow) const
{
    vector2 ab = b - a;
    vector2 ap = point - a;
    float f = dot_product(ab, ap);
    if (f < 0)
    {
        shadow = a;
        return ap.len();
    }
    float d = dot_product(ab, ab);
    if (f > d)
    {
        shadow = b;
        return (b - point).len();
    }
    f = f / d;
    shadow.lerp(a, b, f);
    return (shadow - point).len();
}
bool LineSegmentGeom::CollisionRay(const vector2 &from, const vector2 &to, vector<CollisionInfo> &collideinfo)
{
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
        CollisionInfo ci;
        ci.pos.lerp(from, to, k);
        ci.normal = b - a;
        ci.normal.norm();
        swap(ci.normal.x, ci.normal.y);
        if (dot_product(ci.normal, from - ci.pos) < 0)
            ci.normal.x = -ci.normal.x, ci.normal.y = -ci.normal.y;
        collideinfo.push_back(ci);
        return true;
    }

    return false;
}

bool LineSegmentGeom::CollisionCircle(float radius, const vector2 &from, const vector2 &to, vector<CollisionInfo> &collideinfo)
{
    assert(radius > 0);
    vector2 shadow;
    if (GetDistance(from, shadow) <= radius + 0.005f && (to - shadow).len() < (from - shadow).len())
    {
        CollisionInfo ci;
        ci.normal = from - shadow;
        ci.depth = max(0.0f, radius - ci.normal.len());
        ci.normal.norm();
        ci.pos = shadow + ci.normal * (radius + 0.0025f);
        collideinfo.push_back(ci);
        return true;
    }
    return false;

    bool flag = false;
    vector2 tmpcollide;

    vector2 normalRadius(b.y - a.y, a.x - b.x);
    normalRadius.norm();
    normalRadius *= radius;

    LineSegmentGeom tls;
    tls.SetLineSegment(a + normalRadius, b + normalRadius);
    vector<CollisionInfo> ci;
    ci.reserve(1);
    if (tls.CollisionRay(from, to, ci))
    {
        assert(ci.size() == 1);
        CollisionInfo &cr = ci[0];
        if ((tmpcollide - from).len() < (cr.pos - from).len())
        {
            collideinfo.push_back(cr);
            flag = true;
        }
        ci.clear();
    }
    tls.SetLineSegment(a - normalRadius, b - normalRadius);
    if (tls.CollisionRay(from, to, ci))
    {
        assert(ci.size() == 1);
        CollisionInfo &cr = ci[0];
        if ((tmpcollide - from).len() < (cr.pos - from).len())
        {
            collideinfo.push_back(cr);
            flag = true;
        }
        ci.clear();
    }

    vector2 tangentRadius = b - a;
    tangentRadius.norm();
    tangentRadius *= radius;

    ArcGeom ta;
    ta.SetArc(a, a - tangentRadius, Pi * 0.5f);
    if (ta.CollisionRay(from, to, ci))
    {
        assert(ci.size() == 1);
        CollisionInfo &cr = ci[0];
        if ((tmpcollide - from).len() < (cr.pos - from).len())
        {
            collideinfo.push_back(cr);
            flag = true;
        }
        ci.clear();
    }
    ta.SetArc(b, b + tangentRadius, Pi * 0.5f);
    if (ta.CollisionRay(from, to, ci))
    {
        assert(ci.size() == 1);
        CollisionInfo &cr = ci[0];
        if ((tmpcollide - from).len() < (cr.pos - from).len())
        {
            collideinfo.push_back(cr);
            flag = true;
        }
        ci.clear();
    }
    //if (flag)
    //    collisionDepth = 0;
    return flag;
}

float ArcGeom::GetDistance(const vector2 &point, vector2 &shadow) const
{
    vector2 off1(arc - center);
    vector2 off2(off1);
    off1.rotate(radian);
    off2.rotate(-radian);
    float len;
    
    shadow = off1 + center;
    len = (point - shadow).len();

    vector2 tmp = off2 + center;
    float len2 = (point - tmp).len();
    if (len2 < len)
    {
        shadow = tmp;
        len = len2;
    }

    vector2 off(arc - center);
    off.norm();
    vector2 d(point - center);
    d.norm();
    if (dot_product(d, off) > cos(radian))
    {
        len2 = abs((point - center).len() - off1.len());
        if (len2 < len)
        {
            shadow = center + d * off1.len();
            len = len2;
        }
    }
    return len;
}
bool ArcGeom::CollisionRay(const vector2 &from, const vector2 &to, vector<CollisionInfo> &collideinfo)
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
        float cosRadian = cos(radian);
        if (k1 >= 0 && k1 <= 1)
        {
            a.lerp(from, to, k1);
            vector2 t(a - center);
            t.norm();
            if (dot_product(ta, t) > cosRadian)
            {
                CollisionInfo ci;
                ci.pos = a;
                ci.normal = t;
                ci.normal.norm();
                collideinfo.push_back(ci);
                return true;
            }
        }
        if (k2 >= 0 && k1 <= 1)
        {
            b.lerp(from, to, k2);
            vector2 t(b - center);
            t.norm();
            if (dot_product(ta, t) > cosRadian)
            {
                CollisionInfo ci;
                ci.pos = b;
                ci.normal = t;
                ci.normal.norm();
                collideinfo.push_back(ci);
                return true;
            }
        }
    }
    return false;
}

bool ArcGeom::CollisionCircle(float radius, const vector2 &from, const vector2 &to, vector<CollisionInfo> &collideinfo)
{
    assert(radius > 0);
    vector2 shadow;
    if (GetDistance(from, shadow) <= radius + 0.005f && (to - shadow).len() < (from - shadow).len())
    {
        CollisionInfo ci;
        ci.normal = from - shadow;
        ci.depth = max(0.0f, radius - ci.normal.len());
        ci.normal.norm();
        ci.pos = shadow + ci.normal * (radius + 0.0025f);
        collideinfo.push_back(ci);
        return true;
    }
    return false;

    bool flag = false;
    vector2 tmpcollide;

    ArcGeom ta;
    vector2 ca = arc - center;
    vector2 normalRadius = ca;
    normalRadius.norm();
    normalRadius *= radius;

    vector<CollisionInfo> ci;
    ta.SetArc(center, arc + normalRadius, radian);
    if (ta.CollisionRay(from, to, ci))
    {
        assert(ci.size() == 1);
        CollisionInfo &cr = ci[0];
        if ((tmpcollide - from).len() < (cr.pos - from).len())
        {
            collideinfo.push_back(cr);
            flag = true;
        }
        ci.clear();
    }
    if (radius < (arc - center).len())
    {
        ta.SetArc(center, arc - normalRadius, radian);
        if (ta.CollisionRay(from, to, ci))
        {
            assert(ci.size() == 1);
            CollisionInfo &cr = ci[0];
            if ((tmpcollide - from).len() < (cr.pos - from).len())
            {
                collideinfo.push_back(cr);
                flag = true;
            }
            ci.clear();
        }
    }

    vector2 nr1 = ca, nr2 = ca;
    nr1.rotate(radian);
    nr2.rotate(-radian);
    vector2 c1 = center + nr1, c2 = center + nr2;
    nr1.rotate(Pi / 2);
    nr2.rotate(-Pi / 2);
    nr1.norm();
    nr2.norm();
    vector2 ca1 = nr1 * radius;
    vector2 ca2 = nr2 * radius;
    ta.SetArc(c1, c1 + ca1, Pi * 0.5f);
    if (ta.CollisionRay(from, to, ci))
    {
        assert(ci.size() == 1);
        CollisionInfo &cr = ci[0];
        if ((tmpcollide - from).len() < (cr.pos - from).len())
        {
            collideinfo.push_back(cr);
            flag = true;
        }
        ci.clear();
    }

    ta.SetArc(c2, c2 + ca2, Pi * 0.5f);
    if (ta.CollisionRay(from, to, ci))
    {
        assert(ci.size() == 1);
        CollisionInfo &cr = ci[0];
        if ((tmpcollide - from).len() < (cr.pos - from).len())
        {
            collideinfo.push_back(cr);
            flag = true;
        }
        ci.clear();
    }
/*
    if (flag && GetDistance(collide, shadow) <= radius + 0.005f)
    {
        CollisionInfo ci;
        ci.normal = from - shadow;
        ci.depth = max(0.0f, radius - ci.normal.len());
        ci.normal.norm();
        ci.pos = shadow + ci.normal * (radius + 0.0025f);
        collideinfo.push_back(ci);
        return true;
    }
    */
    if (!flag && GetDistance(to, shadow) <= radius + 0.005f)
    {
        CollisionInfo ci;
        ci.normal = from - shadow;
        ci.depth = max(0.0f, radius - ci.normal.len());
        ci.normal.norm();
        ci.pos = shadow + ci.normal * (radius + 0.0025f);
        collideinfo.push_back(ci);
        return true;
    }
    //if (flag)
    //    collisionDepth = 0;

    return flag;
}

}
