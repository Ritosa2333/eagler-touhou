#pragma once
#ifndef TH_MULTIPLAYER_FIXTURES
#error World journal diagnostics must not enter production
#endif
#include "../../th08_web/cpp/multiplayer/WorldJournal.hpp"
#include "../../th08_web/cpp/platform/BrowserRuntime.hpp"
#include <cstdio>

namespace th08::multiplayer::fixture {
inline const u32* world_journal_probe(BrowserRuntime& runtime,u32 mode=0){
    static u32 result[64]{};std::fill(result,result+64,0);result[0]=1;
    WorldJournal journal;
    const auto fail=[&](u32 step){result[2]=step;result[36]=runtime.status(4);result[37]=journal.OwnerFaults();
        std::printf("world-journal step=%u nativeFaults=%u ownerFaults=%u reason=%s\n",step,result[36],result[37],journal.Error());
        auto& e=runtime.app.game.globals;auto& p=runtime.app.game.presentation;
        const auto info=[](AnmLoaded* a){return a?u32(a->spriteCount|(a->scriptCount<<16)):0u;};
        result[40]=info(e.spell_human_face);result[41]=info(e.spell_youkai_face);
        result[42]=info(e.spell_banners);result[43]=info(p.context.text);
        for(u32 slot=0;slot<7;++slot){const auto& v=e.spell_vms[slot];result[44+slot]=u32(v.scriptIndex)|(u32(v.anmFile!=nullptr)<<16)|(u32(v.loadedSprite!=nullptr)<<17)|(u32(v.visible)<<18);}
        return result;};
    auto& app=runtime.app;auto& game=app.game;
    if(!app.in_game()||app.invalid()||app.session.netplay.Configured()||mode>3)return fail(1);
    if(!journal.Bind(runtime))return fail(2);
    const auto before=journal.AuditHash();const auto blocks_before=journal.BlockHashes();
    std::array<std::array<u32,WorldJournal::GroupCount>,6> expected{};
    std::array<std::vector<u32>,6> expectedBlocks;
    u8 keyboard[256]{};std::memcpy(keyboard,runtime.keyboard_state(),sizeof(keyboard));
    struct RestoreKeys {BrowserRuntime& r;const u8* saved;~RestoreKeys(){std::memcpy(r.keyboard_state(),saved,256);}} restore{runtime,keyboard};
    std::memset(runtime.keyboard_state(),0,256);
    const auto execute=[&](u32 frame){
        float bombs[3]{};for(u32 seat=0;seat<app.session.player_count;++seat)bombs[seat]=app.session.pilot_resources[seat].bombs;
        u16 buttons[3]{};
        for(u32 seat=0;seat<app.session.player_count;++seat)
            buttons[seat]=u16(InputButton::Shoot|((seat+frame)%2?InputButton::Left:InputButton::Right)|
                (mode!=3&&frame<3?InputButton::Focus:0));
        if((mode==1||mode==3)&&frame==0)for(u32 seat=0;seat<app.session.player_count;++seat)buttons[seat]|=InputButton::Bomb;
        if(mode==2&&frame==0)game.pilot(app.session.player_count-1).die();
        if(!game.commit_inputs(buttons,app.session.player_count)||!runtime.step(true))return false;
        if((mode==1||mode==3)&&frame==0){
            for(u32 seat=0;seat<app.session.player_count;++seat){
                if(!game.pilot(seat).status().bomb.active||app.session.pilot_resources[seat].bombs>=bombs[seat])return false;
                result[51+seat]=1;
            }
        }
        return true;
    };
    for(u32 frame=0;frame<6;++frame){
        if(!journal.BeginFrame(frame))return fail(10+frame);
        if(!execute(frame))return fail(20+frame);
        if(!journal.EndFrame())return fail(30+frame);
        expected[frame]=journal.AuditHash();result[3]=std::max(result[3],u32(journal.BytesForFrame(frame)));
        expectedBlocks[frame]=journal.BlockHashes();
    }
    if(!journal.UndoTo(0))return fail(40);
    const auto restored=journal.AuditHash();
    if(restored!=before){
        for(u32 i=0;i<WorldJournal::GroupCount;++i){result[8+i]=before[i];result[20+i]=restored[i];}
        const auto blocks=journal.BlockHashes();
        for(u32 i=0;i<blocks.size();++i)if(blocks[i]!=blocks_before[i]){result[4]=i+1;result[5]=blocks_before[i];result[6]=blocks[i];break;}
        return fail(41);
    }
    result[32]=1;
    for(u32 frame=0;frame<6;++frame){
        if(!journal.BeginFrame(frame)||!execute(frame)||!journal.EndFrame())return fail(50+frame);
        const auto actual=journal.AuditHash();
        if(actual!=expected[frame]){
            for(u32 i=0;i<WorldJournal::GroupCount;++i){result[8+i]=expected[frame][i];result[20+i]=actual[i];}
            const auto blocks=journal.BlockHashes();
            for(u32 i=0;i<blocks.size();++i)if(blocks[i]!=expectedBlocks[frame][i]){
                result[4]=i+1;result[5]=expectedBlocks[frame][i];result[6]=blocks[i];
                std::printf("first divergent block at frame %u: %s\n",frame,journal.BlockName(i));break;}
            return fail(60+frame);
        }
    }
    result[33]=1;
    journal.DiscardBefore(4);
    if(journal.UndoTo(0)||!journal.UndoTo(4))return fail(70);
    result[34]=1;
    // A transition changes the authoritative scene before resource teardown.
    // It may be restored, but must not execute destructors with live history.
    if(!journal.BeginFrame(4))return fail(71);
    app.supervisor.state.target=i32(th08::Scene::NextStage);
    if(!journal.EndFrame()||journal.CanAdvance()||!journal.UndoTo(4)||!journal.CanAdvance())return fail(72);
    result[35]=1;
    journal.Clear();result[1]=1;return result;
}
}
