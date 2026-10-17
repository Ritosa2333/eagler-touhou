#pragma once
#ifndef TH_MULTIPLAYER_FIXTURES
#error World journal diagnostics must not enter production
#endif
#include "../../th08_web/cpp/multiplayer/WorldJournal.hpp"
#include "../../th08_web/cpp/platform/BrowserRuntime.hpp"
#include "../../th08_web/cpp/game/PlayerCollision.hpp"
#include <cstdio>

namespace th08::multiplayer::fixture {
inline const u32* world_journal_probe(BrowserRuntime& runtime,u32 mode=0,bool optimization_oracle=false){
    static u32 result[64]{};std::fill(result,result+64,0);result[0]=1;
    WorldJournal journal;
    journal.AuditBullets(true);
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
    // This diagnostic starts and ends with the production defaults. Compare
    // legacy forward execution against optimized replay of the same world.
    struct ResetOptimizations {bool active;~ResetOptimizations(){if(active){
        fixture_bullet_target_filter(true);fixture_collision_broadphase(true);fixture_barrier_cache(true);fixture_barrier_cache_audit(false);
    }}} reset_optimizations{optimization_oracle};
    if(optimization_oracle){
        fixture_bullet_target_filter(false);fixture_collision_broadphase(false);fixture_barrier_cache(false);
        // Force an aiming extra to become active inside normal(), rather than
        // only testing ordinary straight bullets. Both policies see this seed.
        auto* bullet=game.projectile_pool.next_slot;
        if(bullet->state!=0)return fail(3);
        BulletEmission shot;shot.sprite=0;shot.color=2;shot.position={190,100,.1f};
        shot.speed=.1f;shot.ending_speed=.1f;shot.count=1;shot.layers=1;
        game.bullets.emit(shot);
        if(game.bullets.invalid()||bullet->state!=1)return fail(4);
        bullet->flags|=0x80;bullet->extra_flags=0;bullet->current_extra=0;
        bullet->extras[0]={0,2,0,1,0x80,0};
    }
    if(!journal.Bind(runtime))return fail(2);
    {
        std::vector<u32> order(journal.DiagnosticBlockCount());
        for(u32 i=0;i<order.size();++i)order[i]=i;
        std::sort(order.begin(),order.end(),[&](u32 a,u32 b){return journal.DiagnosticBlockBytes(a)>journal.DiagnosticBlockBytes(b);});
        for(u32 rank=0;rank<std::min<u32>(12,order.size());++rank){
            const u32 i=order[rank];
            std::printf("world-journal block %u: %zu bytes %s\n",rank,journal.DiagnosticBlockBytes(i),journal.BlockName(i));
        }
    }
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
        // Match smoke.commit(): this offline fixture still samples P1 through
        // the physical keyboard path before publishing the shared input.
        constexpr u16 masks[]{1,2,4,16,32,64,128};
        constexpr u8 keys[]{90,88,16,38,40,37,39};
        for(u32 i=0;i<7;++i)runtime.keyboard_state()[keys[i]]=(buttons[0]&masks[i])?128:0;
        if(!game.commit_inputs(buttons,app.session.player_count)||!runtime.step(true))return false;
        if(optimization_oracle&&fixture_barrier_cache_mismatches())return false;
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
        const auto bytes=journal.DiagnosticBytesForFrame(frame);
        for(u32 i=0;i<bytes.size();++i)result[54+i]=std::max(result[54+i],u32(bytes[i]));
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
    if(optimization_oracle){fixture_bullet_target_filter(true);fixture_collision_broadphase(true);fixture_barrier_cache(true);fixture_barrier_cache_audit(true);}
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
    // Exercise every undo position with a nonzero ring head, including live
    // records that straddle the last physical slot. Keep an independent full
    // hash before each capture; mutate the real Replay writer under ownership.
    const auto ring_before=journal.AuditHash();
    const auto recording_before=game.recording;
    const auto random_before=app.session.random;
    for(u32 round=0;round<8;++round){
        const u32 base=100+round*16;
        std::array<std::array<u32,WorldJournal::GroupCount>,11> checkpoints{};
        const auto capture=[&](u32 offset){
            checkpoints[offset]=journal.AuditHash();
            if(!journal.BeginFrame(base+offset))return false;
            game.recording.input.flags=4;
            game.recording.input.physical=u16(37*(base+offset));
            game.recording.input.timing_level=u8(40+offset);
            game.recording.update();
            app.session.random.calls+=1;
            return journal.EndFrame();
        };
        for(u32 offset=0;offset<8;++offset)if(!capture(offset))return fail(80);
        journal.DiscardBefore(base+3);
        for(u32 offset=8;offset<11;++offset)if(!capture(offset))return fail(81);
        const u32 target=3+round;
        if(journal.CheckpointStart(base+2)!=Netplay::INVALID_FRAME||
           !journal.UndoTo(base+target)||journal.AuditHash()!=checkpoints[target]||
           journal.CheckpointStart(base+target)!=Netplay::INVALID_FRAME)return fail(82);
        ++result[59];
        // Continue from the undone frame before clearing the remaining prefix.
        if(!journal.BeginFrame(base+target)||!journal.EndFrame()||
           !journal.UndoTo(base+target)||journal.AuditHash()!=checkpoints[target])return fail(83);
        journal.DiscardBefore(base+16);
    }
    game.recording=recording_before;app.session.random=random_before;
    if(journal.HasHistory()||journal.AuditHash()!=ring_before)return fail(84);
    // Extend admission and restored Replay bytes remain identical.
    if(!journal.BeginFrame(300))return fail(85);
    game.recording.input.flags=4;game.recording.update();
    if(!journal.EndFrame()||!journal.BeginFrame(301,true))return fail(86);
    game.recording.update();
    if(!journal.EndFrame()||journal.CheckpointStart(301)!=300||
       !journal.UndoTo(300)||journal.AuditHash()!=ring_before)return fail(87);
    result[60]=1;
    journal.Clear();
    if(!journal.Bind(runtime))return fail(88);
    for(u32 offset=0;offset<WorldJournal::History;++offset)
        if(!journal.BeginFrame(400+offset)||!journal.EndFrame())return fail(89);
    if(journal.BeginFrame(400+WorldJournal::History)||!journal.Failed())return fail(90);
    result[61]=1; // still fails closed instead of overwriting a needed record
    journal.Clear();
    // Exercise the dormant transition bytes independently of animation
    // liveness. These synthetic VM bytes are never executed or rendered.
    auto& display=game.display;auto& flow=game.gui_flow;
    struct RestoreGui {
        GuiImplState& display;GuiLoadContext& context;
        std::vector<u8> bytes;GuiLoadContext saved_context;
        RestoreGui(GuiImplState& d,GuiLoadContext& c):display(d),context(c),
            bytes(reinterpret_cast<const u8*>(&d),reinterpret_cast<const u8*>(&d)+sizeof(d)),saved_context(c){}
        ~RestoreGui(){std::memcpy(&display,bytes.data(),bytes.size());context=saved_context;}
    } restore_gui{display,flow.context};
    if(!journal.Bind(runtime)||flow.transition_touch_context!=&journal||!flow.transition_touch)return fail(91);
    if(!flow.transition_touch(flow.transition_touch_context))return fail(92); // no retained history
    display.transition_count=0;std::memset(display.transition,0xa5,sizeof(display.transition));
    auto gui_hash=journal.AuditHash();
    if(!journal.BeginFrame(500))return fail(93);
    const auto dormant_bytes=journal.DiagnosticBytesForFrame(500)[0];
    if(!journal.EndFrame()||!journal.UndoTo(500)||journal.AuditHash()!=gui_hash)return fail(94);
    ++result[62];
    // count == 1 still executes the entire array, including its last slot.
    display.transition_count=1;gui_hash=journal.AuditHash();
    if(!journal.BeginFrame(501))return fail(95);
    const auto active_bytes=journal.DiagnosticBytesForFrame(501)[0];
    if(active_bytes!=dormant_bytes+sizeof(display.transition))return fail(96);
    result[63]=u32(active_bytes-dormant_bytes);
    std::memset(display.transition,0x5a,sizeof(display.transition));
    if(!journal.EndFrame()||!journal.UndoTo(501)||journal.AuditHash()!=gui_hash)return fail(97);
    ++result[62];
    display.transition_count=0;gui_hash=journal.AuditHash();
    if(!journal.BeginFrame(502)||!flow.transition_touch(flow.transition_touch_context)||
       journal.DiagnosticBytesForFrame(502)[0]!=active_bytes)return fail(98);
    std::memset(display.transition,0x3c,sizeof(display.transition));display.transition_count=168;
    if(!flow.transition_touch(flow.transition_touch_context)||
       journal.DiagnosticBytesForFrame(502)[0]!=active_bytes||
       !journal.EndFrame()||!journal.UndoTo(502)||journal.AuditHash()!=gui_hash)return fail(99);
    ++result[62]; // first write and repeated touch preserve the original bytes
    if(!journal.BeginFrame(503)||!journal.EndFrame()||!journal.BeginFrame(504,true)||
       !flow.transition_touch(flow.transition_touch_context))return fail(100);
    std::memset(display.transition,0x69,sizeof(display.transition));display.transition_count=1;
    if(!journal.EndFrame()||journal.CheckpointStart(504)!=503||
       !journal.UndoTo(503)||journal.AuditHash()!=gui_hash)return fail(101);
    ++result[62];
    display.transition_count=1;gui_hash=journal.AuditHash();
    if(!journal.BeginFrame(505))return fail(102);
    game.gui.reset_clear();
    if(!flow.transition_touch(flow.transition_touch_context))return fail(103);
    std::memset(display.transition,0x96,sizeof(display.transition));display.transition_count=168;
    if(journal.DiagnosticBytesForFrame(505)[0]!=active_bytes||
       !journal.EndFrame()||!journal.UndoTo(505)||journal.AuditHash()!=gui_hash)return fail(104);
    ++result[62];
    journal.Clear();
    // Call the real setup entry point for both branches with closed retained
    // history. The owner must reject before memset, reset_clear or resource IO.
    std::memcpy(&display,restore_gui.bytes.data(),restore_gui.bytes.size());
    for(bool initial:{true,false}){
        flow.context.initial=initial;
        if(!journal.Bind(runtime)||!journal.BeginFrame(600)||!journal.EndFrame())return fail(105);
        gui_hash=journal.AuditHash();
        if(flow.setup()||!journal.Failed()||journal.AuditHash()!=gui_hash)return fail(106);
        ++result[62];
        journal.Clear();
        if(flow.transition_touch||flow.transition_touch_context)return fail(107);
    }
    result[1]=1;return result;
}
}
