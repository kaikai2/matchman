#ifndef MAINGAMESTATE_H
#define MAINGAMESTATE_H

#include "gamestate.h"
#include "charEntity.h"
#include "mapQuery.h"
//#include "flatland/flatland.hpp"
#include "phy2d.h"
class hgeFont;
class hgeSprite;
const float Pi = acos(-1.0f);

class Map : public MapQuery
{
public:
    Map(Phy2d::Space *_world) : world(_world)
    {
    }
    virtual CollisionType QueryMove(MoveObject *object, const vector2 &from, const vector2 &to, vector2 &suggest)
    {
        vector<Phy2d::CollisionInfo> &ci = object->GetCollisionInfo();
        ci.clear();
        if (world->CollisionCircle(object->GetRadius(), from, to, ci))
        {
            Phy2d::GeomPtr g = world->GetLastCollision();
            if (g)
            {
                g->GetData();
            }
            return MapQuery::CT_Ground;
        }
        return MapQuery::CT_None;
    }
    void RenderGeom(Phy2d::GeomPtr g, DWORD color, int iteration, float radius) const
    {
        HGE *hge = hgeCreate(HGE_VERSION);
        assert(hge);

        switch(g->GetType())
        {
        case Phy2d::Geom::LineSeg:
            {
                const Phy2d::LineSegmentGeom &l = *(const Phy2d::LineSegmentGeom *)g;
                hge->Gfx_RenderLine(l.GetVector2(0).x, l.GetVector2(0).y, l.GetVector2(1).x, l.GetVector2(1).y, color);

                if (iteration > 0)
                {
                    const vector2 &a = l.GetVector2(0);
                    const vector2 &b = l.GetVector2(1);

                    vector2 tmpcollide, tmpnormal;
                    vector2 normalRadius(b.y - a.y, a.x - b.x);
                    normalRadius.norm();
                    normalRadius *= radius;

                    Phy2d::LineSegmentGeom tls;
                    tls.SetLineSegment(a + normalRadius, b + normalRadius);
                    RenderGeom(&tls, 0xff80ff80, iteration - 1, radius);
                    tls.SetLineSegment(a - normalRadius, b - normalRadius);
                    RenderGeom(&tls, 0xff80ff80, iteration - 1, radius);

                    vector2 tangentRadius = b - a;
                    tangentRadius.norm();
                    tangentRadius *= radius;

                    Phy2d::ArcGeom ta;
                    ta.SetArc(a, a - tangentRadius, Pi * 0.5f);
                    RenderGeom(&ta, 0xff80ff80, iteration - 1, radius);
                    ta.SetArc(b, b + tangentRadius, Pi * 0.5f);
                    RenderGeom(&ta, 0xff80ff80, iteration - 1, radius);
                }
            }
            break;
        case Phy2d::Geom::Arc:
            {
                const Phy2d::ArcGeom &a = *(const Phy2d::ArcGeom *)g;
                const vector2 &center = a.GetVector2(0);
                const vector2 &arc = a.GetVector2(1);
                float radian = a.GetFloat(0);
                vector2 off = arc - center;
                off.rotate(-radian);
                vector2 p, p2;
                p = center + off;
                float d;
                for (d = 0; d < radian * 2; d += 0.1f)
                {
                    off.rotate(0.1f);
                    p2 = center + off;
                    hge->Gfx_RenderLine(p.x, p.y, p2.x, p2.y, color);
                    p = p2;
                }
                if (d != radian * 2)
                {
                    off = arc - center;
                    off.rotate(radian);
                    p2 = center + off;
                    hge->Gfx_RenderLine(p.x, p.y, p2.x, p2.y, color);
                }
                if (iteration > 0)
                {
                    vector2 tmpcollide, tmpnormal;

                    Phy2d::ArcGeom ta;
                    vector2 ca = arc - center;
                    vector2 normalRadius = ca;
                    normalRadius.norm();
                    normalRadius *= radius;

                    ta.SetArc(center, arc + normalRadius, radian);
                    RenderGeom(&ta, 0xff80ff80, iteration - 1, radius);

                    if (radius < (arc - center).len())
                    {
                        ta.SetArc(center, arc - normalRadius, radian);
                        RenderGeom(&ta, 0xff80ff80, iteration - 1, radius);
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
                    RenderGeom(&ta, 0xff80ff80, iteration - 1, radius);
                    ta.SetArc(c2, c2 + ca2, Pi * 0.5f);
                    RenderGeom(&ta, 0xff80ff80, iteration - 1, radius);
                }
            }
            break;
        case Phy2d::Geom::Wall:
            break;
        case Phy2d::Geom::Space:
        default:
            break;
        }
        hge->Release();
    }
    void Render() const
    {
        if (!world)
            return;
        float radius = 20;

        HGE *hge = hgeCreate(HGE_VERSION);
        assert(hge);
        const vector<Phy2d::GeomPtr> &geoms = world->GetGeoms();
        for (vector<Phy2d::GeomPtr>::const_iterator g = geoms.begin(); g != geoms.end(); ++g)
        {
            RenderGeom(*g, 0xffffffff, 1, radius);
        }
        hge->Release();
    }
    Phy2d::Space *world;
};


class MainGameState : public GameState
{
public:
    MainGameState() : fnt(0), map(&world)
    {
    }
    virtual void OnEnter();
    virtual void OnLeave();
    virtual void OnFrame();
    virtual void OnRender();
protected:
    hgeFont *fnt;

    Phy2d::LineSegmentGeom* MainGameState::CreateLineSegment(const vector2 &a, const vector2 &b);
    Phy2d::ArcGeom* MainGameState::CreateArc(const vector2 &center, const vector2 &arc, float radian);

    CharEntity player;
    float land;

    Phy2d::Space world;
    Map map;
    vector<Phy2d::GeomPtr> geoms;
   // Flatland::Static<Flatland::Terrain> terrain;
};

#endif
