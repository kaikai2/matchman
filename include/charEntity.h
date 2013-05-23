#ifndef CHAR_ENTITY_H
#define CHAR_ENTITY_H

#include <cstdio>
#include "hge.h"
#include "hgefont.h"
#include "hgesprite.h"
#include "hgefont.h"
#include "mapQuery.h"
#include "moveObject.h"
#include "_vector2.h"

class CharEntity : public MoveObject
{
public:

    void Load()
    {
        HGE *hge = hgeCreate(HGE_VERSION);
        assert(hge);

        HTEXTURE tex = hge->Texture_Load("zazaka.png");
        sprite = new hgeSprite(tex, 0, 0, (float)hge->Texture_GetWidth(tex), (float)hge->Texture_GetHeight(tex));
        pos.set(hge->System_GetState(HGE_SCREENWIDTH) * 0.5f,
                hge->System_GetState(HGE_SCREENHEIGHT) * 0.5f);
        velocity.set(0, 0);
        bGround = true;
        bJumphold = false;
        bGrabWall = false;
        font = 0;
        radius = 20.0f;
        gravity.set(0, 980);
        hge->Release();
    }
    virtual void OnFrame(float delta)
    {
        HGE *hge = hgeCreate(HGE_VERSION);
        assert(hge);
        vector2 force;

        if (hge->Input_GetKeyState(HGEK_LEFT))
        {
            force.x -= 200.0f;
        }
        if (hge->Input_GetKeyState(HGEK_RIGHT))
        {
            force.x += 200.0f;
        }
        if (bGround && !bJumphold && hge->Input_GetKeyState(HGEK_Z))
        {
            vector2 g = gravity;
            g.norm();
            force += -g * (gravity.len() + 100.0f);
            velocity += vector2(0, -300);
            // velocity = velocity - collisionNormal * dot_product(collisionNormal, velocity) + collisionNormal * 100 + vector2(0, -250);
            bGround = false;
            bJumphold = true;
        }

        if (bJumphold)
        {
            if (hge->Input_GetKeyState(HGEK_Z))
            {
                if (!bGround)
                {
                    force -= gravity * 0.5f;
                    // velocity -= gravity * (0.5f * delta);
                }
            }
            else
                bJumphold = false;
        }
        force += gravity;
        vector2 v1 = velocity + force * delta;
        vector2 dest;
        switch(map->QueryMove(this, pos, pos + v1 * delta, dest))
        {
        case MapQuery::CT_None:

            bGround = false;
            break;
        case MapQuery::CT_Ground:
            // apply friction
            // velocity -= collisionNormal * dot_product(velocity, collisionNormal);
            //dest.y -= 1;
            bGround = true;
            break;
        case MapQuery::CT_Wall:
            velocity.x = 0;
            velocity.y = 0;
            bGround = true;
            break;
        }
        // pos = dest;
        if (bGround)
        {
            vector2 tempForce = force;
            vector2 friction;
            float totalForce = 0;
            vector2 ttforce;
            float frictionMu = 0.1f;
            do
            {
                ttforce = tempForce;
                for (vector<Phy2d::CollisionInfo>::iterator ci = this->collisionInfos.begin();
                    ci != collisionInfos.end(); ++ci)
                {
                    // apply N, support force from collision point
                    float f = -dot_product(ci->normal, tempForce);
                    if (f > 0)
                    {
                        ci->force += f;
                        totalForce += f;
                        tempForce += ci->normal * f;
                        //friction += 
                        vector2 friction =  -velocity + ci->normal * dot_product(velocity, ci->normal);
                        friction.norm();
                        friction *= f * frictionMu;
                        tempForce += friction;
                    }
                }
            } while(ttforce != tempForce);
            for (vector<Phy2d::CollisionInfo>::iterator ci = this->collisionInfos.begin();
                ci != collisionInfos.end(); ++ci)
            {
                float depth = ci->depth - 0.005;
                if (depth > 0)
                {
                    if (totalForce > 0 && ci->force < totalForce)
                        pos += ci->normal * (depth * ci->force / totalForce);
                    else
                        pos += ci->normal * depth;
                }

                velocity = velocity - ci->normal * dot_product(ci->normal, velocity);
            }
            force = tempForce;

            //pos += dest - pos - collisionNormal * dot_product(collisionNormal, dest - pos);
            //pos += collisionNormal * this->radius;
            // pos = dest;

            // apply Friction
           
        }

        velocity += force * delta;
        pos += velocity * delta;

        hge->Release();
    }
    virtual void OnCollide()
    {
    }
    void RenderCircle()
    {
        HGE *hge = hgeCreate(HGE_VERSION);
        assert(hge);
        const float Pi = acos(-1.0f);
        for (float r = 0; r < Pi; r += .1f)
        {
            vector2 p(radius, 0), p2(radius, 0);
            p.rotate(r);
            p2.rotate(r + .1f);
            hge->Gfx_RenderLine(pos.x + p.x, pos.y + p.y, pos.x + p2.x, pos.y + p2.y);
            hge->Gfx_RenderLine(pos.x - p.x, pos.y - p.y, pos.x - p2.x, pos.y - p2.y);
            hge->Gfx_RenderLine(pos.x - p.x, pos.y + p.y, pos.x - p2.x, pos.y + p2.y);
            hge->Gfx_RenderLine(pos.x + p.x, pos.y - p.y, pos.x + p2.x, pos.y - p2.y);
        
        }
        hge->Release();
    }
    void RenderStatus()
    {
        if (!font)
            return;
        
        HGE *hge = hgeCreate(HGE_VERSION);
        assert(hge);
        float s = font->GetScale();
        DWORD c = font->GetColor();
        font->SetScale(0.5f);
        font->SetColor(0xffff8080);

        char buf[128];
        sprintf(buf, "%s %s %s", bGround?"Ground":"", bJumphold?"JumpHold":"",bGrabWall?"GrabWall":"");
        font->Render(pos.x - 100, pos.y - 100, HGETEXT_LEFT, buf);
        sprintf(buf, "Vel %.2f:%.2f", velocity.x, velocity.y);
        font->Render(pos.x - 100, pos.y - 80, HGETEXT_LEFT, buf);

        font->SetScale(s);
        font->SetColor(c);
        if (bGround)
        {
            for (vector<Phy2d::CollisionInfo>::iterator ci = this->collisionInfos.begin();
                ci != collisionInfos.end(); ++ci)
            {
                vector2 n = pos + ci->normal * (ci->force * 0.1f);
                hge->Gfx_RenderLine(pos.x, pos.y, n.x, n.y, 0xffff0000);
            }
            
        }
        vector2 g = pos + gravity * 0.1f;
        hge->Gfx_RenderLine(pos.x, pos.y, g.x, g.y, 0xff00ff00);
        hge->Gfx_RenderLine(pos.x, pos.y, pos.x + velocity.x, pos.y + velocity.y, 0xff0000ff);
        hge->Release();
    }

    void Render()
    {
        sprite->Render(pos.x - sprite->GetWidth() / 2, pos.y - sprite->GetHeight() / 2);
        RenderCircle();
        RenderStatus();
    }

    bool bGround;
    bool bJumphold;
    bool bGrabWall;
    hgeSprite *sprite;
    hgeFont *font;

    vector2 gravity;
};

#endif //CHAR_ENTITY_H
