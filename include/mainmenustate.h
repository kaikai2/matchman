#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include "gamestate.h"

#include "hgefont.h"
#include "hgegui.h"

class MainMenuState : public GameState
{
public:
    MainMenuState() : gui(0), fnt(0), spr(0)
    {

    }
    virtual void OnEnter();
    virtual void OnLeave();
    virtual void OnFrame();
    virtual void OnRender();

protected:


    hgeGUI *gui;
    hgeFont *fnt;
    hgeSprite *spr;
    // Some resource handles
    HEFFECT snd;
    HTEXTURE tex;
    hgeQuad quad;
};

#endif