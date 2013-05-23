/*
** Haaf's Game Engine 1.6
** Copyright (C) 2003-2006, Relish Games
** hge.relishgames.com
**
** hge_tut06 - Creating menus
*/


// Copy the files "menu.wav", "font1.fnt", "font1.png",
// "bg.png" and "cursor.png" from the folder "precompiled"
// to the folder with executable file. Also copy hge.dll
// and bass.dll to the same folder.


#include "hge.h"

#include "phy2d.h"

#include "maingamestate.h"
#include "mainmenustate.h"

#include <cmath>
#include <cassert>


// Pointer to the HGE interface.
// Helper classes require this to work.
HGE *hge=0;



// Pointers to the HGE objects we will use

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    hge = hgeCreate(HGE_VERSION);

    hge->System_SetState(HGE_LOGFILE, "hge_tut06.log");
    hge->System_SetState(hgeIntState(14), 0xFACE0FF);
    hge->System_SetState(HGE_TITLE, "HGE Tutorial 06 - Creating menus");
    hge->System_SetState(HGE_WINDOWED, true);
    hge->System_SetState(HGE_SCREENWIDTH, 800);
    hge->System_SetState(HGE_SCREENHEIGHT, 600);
    hge->System_SetState(HGE_SCREENBPP, 32);

    MainMenuState mms;
    mms.SetName("mainmenu");
    GameStateManager::Instance()->RegisterState(&mms);
    MainGameState mgs;
    mgs.SetName("maingame");
    GameStateManager::Instance()->RegisterState(&mgs);
    GameStateManager::Instance()->RequestState("mainmenu");

    if(hge->System_Initiate())
    {
        // Let's rock now!
        hge->System_Start();
    }

    // Clean up and shutdown
    hge->System_Shutdown();
    hge->Release();
    return 0;
}
