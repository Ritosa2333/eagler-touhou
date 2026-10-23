#include "ItemManager.hpp"
#include "GameManager.hpp"
#include "Player.hpp"
#include "Multiplayer.hpp"
#include "multiplayer/GameplaySession.hpp"
#include <emscripten.h>
#include <cmath>
#include <cstdio>
#include <cstring>

// Linked into a separate browser diagnostic, after native stage resources load.
extern "C" EMSCRIPTEN_KEEPALIVE const char* th07_item_collection_fixture(int scenario) {
    static char report[4096];
    const bool ready=MultiplayerGameplay::IsMultiplayer() && g_GameManager.globals &&
        g_Players[0].shooterData && g_Players[1].shooterData;
    EM_ASM({globalThis.__itemFixtureReady=!!$0;},ready);
    if(!ready)return "{\"ready\":false}";
    if(scenario<0)return "{\"ready\":true}";
    std::memset(g_ItemManager.items,0,sizeof(g_ItemManager.items));
    g_ItemManager.nextIndex=0;
    const unsigned count=MultiplayerGameplay::GetPlayerCount();
    for (unsigned s=0;s<count;++s) {
        auto& p=g_Players[s];
        p.playerState=PLAYER_STATE_ALIVE;p.hasBorder=0;
        p.pos=ZunVec3(s==0?100.f:s==1?280.f:360.f,scenario==0&&s==0?80.f:360.f,0);
        p.grabItemTopLeft=p.pos-ZunVec3(4,4,0);p.grabItemBottomRight=p.pos+ZunVec3(4,4,0);
        SetPlayerPower(s,scenario==2?128:(s==0?128:s==1?64:1));
    }
    ZunVec3 origin=scenario==0?ZunVec3(100,220,0):g_Players[0].pos;
    if(scenario==3)SetPlayerPower(0,127);
    if(scenario==4)SetPlayerPower(0,100);
    Item* item=g_ItemManager.SpawnSingleItem(&origin,scenario==5?ITEM_POWER_BIG:ITEM_POWER_SMALL,0);
    const int initialType=item->itemType;
    if(scenario==3) {
        // The first pickup reaches max and converts an existing field item.
        ZunVec3 second=g_Players[1].pos;
        item=g_ItemManager.SpawnSingleItem(&second,ITEM_POWER_SMALL,0);
    }
    bool moved=false,finite=true;
    const auto start=item->pos;
    int frames=0;
    while(item->isInUse && frames<180) {
        g_ItemManager.OnUpdate();++frames;
        moved|=item->pos.x!=start.x||item->pos.y!=start.y;
        finite&=std::isfinite(item->pos.x)&&std::isfinite(item->pos.y);
    }
    std::snprintf(report,sizeof(report),"{\"ready\":true,\"scenario\":%d,\"collected\":%s,\"moved\":%s,\"finite\":%s,\"frames\":%d,\"initialType\":%d,\"type\":%d,\"state\":%d,\"marker\":%d,\"x\":%.3f,\"y\":%.3f,\"power\":[%d,%d,%d],\"slots\":[%d,%d,%d],\"ids\":[%d,%d,%d]}",
        scenario,item->isInUse?"false":"true",moved?"true":"false",finite?"true":"false",frames,initialType,int(item->itemType),int(item->state),int(item->autoCollect),item->pos.x,item->pos.y,GetPlayerPower(0),GetPlayerPower(1),GetPlayerPower(2),int(IsPlayerSlotActive(0)),int(IsPlayerSlotActive(1)),int(IsPlayerSlotActive(2)),g_Players[0].initParam,g_Players[1].initParam,g_Players[2].initParam);
    EM_ASM({globalThis.__itemFixtureResult=JSON.parse(UTF8ToString($0));},report);
    return report;
}
