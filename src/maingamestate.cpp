#include <cmath>
#include <cassert>

#include "maingamestate.h"
#include "hgefont.h"
#include "hgesprite.h"

#if 0
Phy2d::ArcGeom ga;
vector2 mousepos;
#endif
void MainGameState::OnEnter()
{
    assert(!fnt);
    srand(GetTickCount());
    HGE *hge = hgeCreate(HGE_VERSION);
    assert(hge);
    fnt = new hgeFont("font1.fnt");
    player.Load();
    hge->Release();
    player.font = fnt;
    world.AddGeom(CreateLineSegment(vector2(0, 500), vector2(800, 500)));
    world.AddGeom(CreateLineSegment(vector2(10, 0), vector2(10, 500)));
    world.AddGeom(CreateLineSegment(vector2(790, 0), vector2(790, 500)));

    world.AddGeom(CreateLineSegment(vector2(550, 0), vector2(550, 400)));
    world.AddGeom(CreateLineSegment(vector2(600, 0), vector2(600, 400)));

    world.AddGeom(CreateLineSegment(vector2(20, 400), vector2(100, 450)));
    world.AddGeom(CreateLineSegment(vector2(100, 400), vector2(20, 450)));

    const float Pi = acos(-1.0f);
    float x, y, len, dHeight;
    for (int i = 0; i < 5; i++)
    {
        x = float(rand() % 780);
        y = float(rand() % 500);
        len = float(rand() % 200 + 10);
        dHeight = float(rand() % 100 - 50);
        world.AddGeom(CreateLineSegment(vector2(x - len, y), vector2(x + len, y + dHeight)));
    }
    for (int i = 0; i < 5; i++)
    {
        x = float(rand() % 780 + 10);
        y = float(rand() % 500);
        world.AddGeom(CreateArc(vector2(x, y), vector2(x + rand()%100 - 50, y + rand()%100 - 50), rand()%10 * 0.1f * Pi));
    }
    player.SetMapQuery(&this->map);
    player.font = fnt;
#if 0
    x = rand() % 500 + 200;
    y = rand() % 500;
    ga.SetArc(vector2(x, y), vector2(x + rand()%10 * 10 - 50, y + rand()%100 - 50), rand()%100 * 0.01f * Pi);
#endif
}

Phy2d::LineSegmentGeom* MainGameState::CreateLineSegment(const vector2 &a, const vector2 &b)
{
    Phy2d::LineSegmentGeom *lsg = new Phy2d::LineSegmentGeom;
    lsg->SetLineSegment(a, b);
    geoms.push_back(lsg);
    return lsg;
}

Phy2d::ArcGeom* MainGameState::CreateArc(const vector2 &center, const vector2 &arc, float radian)
{
    Phy2d::ArcGeom *ag = new Phy2d::ArcGeom;
    ag->SetArc(center, arc, radian);
    geoms.push_back(ag);
    return ag;
}
void MainGameState::OnLeave()
{
    world.Clear();
    for (vector<Phy2d::GeomPtr>::iterator g = geoms.begin(); g != geoms.end(); ++g)
        delete *g;
    geoms.clear();

    delete fnt;
    fnt = 0;
}    
#pragma comment(lib, "winmm")
DWORD t1, t2, t3;
void MainGameState::OnFrame()
{
    HGE *hge = hgeCreate(HGE_VERSION);
    assert(hge);

    float delta = hge->Timer_GetDelta();

    // If ESCAPE was pressed, tell the GUI to finish
    if (hge->Input_GetKeyState(HGEK_ESCAPE))
    {
        RequestState("mainmenu");
    }
    
    t1 = timeGetTime();

    while(world.Update())
        ;
#if 0
    hge->Input_GetMousePos(&mousepos.x, &mousepos.y);
#endif
    t2 = timeGetTime();
    player.OnFrame(delta);
    t3 = timeGetTime();

    //playerdata.vy = playerdata.vy * 0.9;
    hge->Release();
}

void MainGameState::OnRender()
{
    HGE *hge = hgeCreate(HGE_VERSION);
    assert(hge);
    hge->Gfx_BeginScene();
    hge->Gfx_Clear(0);
    fnt->SetColor(0xFFFFFFFF);
    fnt->printf(5, 5, HGETEXT_LEFT, "dt:%.3f\nFPS:%d", hge->Timer_GetDelta(), hge->Timer_GetFPS());
    // world.RenderDebug();
    this->map.Render();
    player.Render();
    char buf[100];
#if 0
    vector2 col, normal;
    hge->Gfx_RenderLine(400, 300, mousepos.x, mousepos.y);
    if (ga.CollisionRay(mousepos, vector2(400, 300), col, normal))
    {
        map.RenderGeom(&ga, 0xffff0000);
        hge->Gfx_RenderLine(400, 300, col.x, col.y);

        hge->Gfx_RenderLine(col.x, col.y, col.x + normal.x * 10, col.y + normal.y * 10, 0xff00ff00);
    }
    else
    {
        map.RenderGeom(&ga, 0xffffffff);
    }
#endif

    sprintf(buf, "%d %d", t3 - t2, t2 - t1);
    fnt->Render(0, 100, HGETEXT_LEFT, buf);
    hge->Gfx_EndScene();
    hge->Release();
}
