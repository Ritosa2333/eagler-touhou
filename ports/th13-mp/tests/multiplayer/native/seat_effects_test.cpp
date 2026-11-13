// Per-seat player presentation ownership. Uses the real player ANM scripts;
// no combat rules, resources, or production animation lifetime are replaced.
#include "../../ecl/ecl_test_fixtures.hpp"
#include "multiplayer/WorldState.hpp"
#include "game/StagePresentation.hpp"
#include <fstream>
#include <iterator>
#include <set>

using namespace th13;
using namespace th13test;
namespace {
struct NullGraphics final : ZunGraphics {
    touhou::graphics::PipelineState state;
    std::vector<AnmGeometryVertex> submitted;
    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&,u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind,const Matrix4&) override {}
    void triangles(u32 count,const void* data,u32 stride) override {
        if(stride==sizeof(AnmGeometryVertex)) {
            const auto* first=static_cast<const AnmGeometryVertex*>(data);
            submitted.insert(submitted.end(),first,first+count*3);
        }
    }
    void primitives(touhou::graphics::Topology,u32,const void*,u32) override {}
    bool select_target(const AnmResource*,u32) override { return true; }
    bool clear_target(u32,const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}
};
std::vector<u8> asset(const char* name) {
    const char* root=std::getenv("TH13_NATIVE_ASSET_ROOT");
    std::ifstream file(std::string(root?root:"/work/artifacts/reference/raw")+"/"+name,std::ios::binary);
    return {std::istreambuf_iterator<char>(file),{}};
}
struct Host final : StageBattleServices {
    bool life_hud(i32,i32) override { return true; }
    bool bomb_hud(i32,i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};
struct Provider final : EclResourceProvider {
    std::vector<u8> ecl;
    std::vector<std::string>* names = nullptr;
    Provider() {
        std::vector<u8> main;
        emit(main,0,23,0,0xff,0,{100000});
        emit(main,0,10,0,0xff,0,{});
        ecl=build_scpt({{"main",main}},{"enemy.anm","st01enm.anm"});
    }
    bool read(const std::string& name,std::vector<u8>& out) override {
        out=name=="seats.ecl"?ecl:asset(name.c_str()); return !out.empty();
    }
    bool animation(u32,const std::string& name) override { if(names) names->push_back(name); return true; }
};
struct Fixture {
    GameRandom random{123}; AnmEnvironment environment;
    AnmManager animations{random,environment}; NullGraphics graphics;
    AnmRenderer renderer{graphics}; StageBattle battle;
    StagePresentation::Banks banks{4,5,6,8,25,3,{7,9}};
    StagePresentation presentation{animations,renderer,banks,0};
    Host host; Provider provider; SessionState progress; HudPlayerState hud; HudScoreState score;
    bool boot(std::array<i32,3> characters={0,0,0},i32 count=2) {
        provider.names=&battle.sim.anim_list;
        progress.character=characters[0];
        if(!battle.load(provider,"seats.ecl",2,host,progress,hud,score) ||
           !battle.enable_seats(provider,count,characters)) { std::printf("battle boot: %s\n",battle.error().c_str()); return false; }
        battle.mp_registry_hook=&animations.registry;
        battle.reimu_bomb_orb_birth=[this](const StageBattle& b,i32 index) {
            return presentation.reimu_bomb_orb_birth(b,index);
        };
        battle.youmu_bomb_update=[this](StageBattle& b){return presentation.sync_youmu_slash(b);};
        const bool ok=presentation.initialize_player_effects(battle);
        if(!ok) std::printf("presentation boot: %s\n",presentation.error().c_str());
        return ok;
    }
    bool frame(int number, bool simultaneous = false) {
        // Legal separate pressed edges, with one overlap and a later Bomb for
        // each seat. No resource grants or combat state changes during play.
        const u32 p0=(number==120 || number==620)?2:0;
        const u32 p1=(number==(simultaneous?120:80) || number==(simultaneous?620:580))?2:0;
        battle.seat_inputs[1].update(p1);
        GameInput input=battle.seat_inputs[0]; input.update(p0);
        const bool ok=battle.step(input) && presentation.initialize_player_effects(battle) &&
               animations.update(false) && battle.error().empty() && presentation.error().empty();
        if(!ok) std::printf("frame %d: battle=%s presentation=%s animations=%s\n",number,
            battle.error().c_str(),presentation.error().c_str(),animations.error.c_str());
        return ok;
    }
    bool load(const char* name="pl00.anm") {
        const auto ascii=asset("ascii.anm");
        const auto bullet=asset("bullet.anm");
        const auto effect=asset("effect.anm");
        if(ascii.empty() || !animations.load(2,ascii.data(),ascii.size())) return false;
        environment.fallback_sprite_resource=animations.resource(2);
        if(bullet.empty() || effect.empty() || !animations.load(5,bullet.data(),bullet.size()) ||
           !animations.load(8,effect.data(),effect.size())) {
            std::printf("common banks: %s\n",animations.error.c_str()); return false;
        }
        const auto bytes=asset(name);
        return !bytes.empty() && animations.load(6,bytes.data(),bytes.size()) &&
            animations.load(7,bytes.data(),bytes.size()) && animations.load(9,bytes.data(),bytes.size());
    }
};
}
TEST(reimu_bomb_orb_slots_are_owned_by_each_seat) {
    Fixture f; CHECK(f.load()); f.battle.seat_count=3;
    std::set<u32> handles;
    for(int seat=0;seat<3;++seat) {
        f.battle.active_seat=seat;
        for(int orb=0;orb<8;++orb) CHECK(f.presentation.reimu_bomb_orb_birth(f.battle,orb));
    }
    for(u32 i=0;i<f.animations.registry.ordered_count(false);++i) {
        const auto handle=f.animations.registry.ordered_handle(false,i);
        const auto* vm=f.animations.registry.find(handle);
        if(vm && vm->source_script==18) {
            handles.insert(handle);
            CHECK(vm->shot_effect_owner>=1 && vm->shot_effect_owner<=3);
            const i32 bank=vm->shot_effect_owner==1?6:vm->shot_effect_owner==2?7:9;
            CHECK(vm->resource==f.animations.resource(bank));
        }
    }
    CHECK_EQ(handles.size(),size_t(24));
}
TEST(overlapping_and_repeat_reimu_bombs_restore_exactly) {
    Fixture f; CHECK(f.load()); CHECK(f.boot()); if(!f.battle.error().empty() || !f.presentation.error().empty()) return;
    for(int frame=0;frame<100;++frame) if(!f.frame(frame)) { CHECK(false); return; }
    multiplayer::WorldState saved; CHECK(saved.capture(f.battle,f.random,nullptr,&f.presentation));
    std::vector<u64> continuous;
    for(int frame=100;frame<160;++frame) {
        if(!f.frame(frame)) { CHECK(false); return; } multiplayer::WorldState state;
        CHECK(state.capture(f.battle,f.random,nullptr,&f.presentation)); continuous.push_back(state.digest());
    }
    CHECK(saved.restore(f.battle,f.random,nullptr,&f.presentation));
    for(int frame=100;frame<160;++frame) {
        if(!f.frame(frame)) { CHECK(false); return; } multiplayer::WorldState state;
        CHECK(state.capture(f.battle,f.random,nullptr,&f.presentation));
        CHECK_EQ(state.digest(),continuous[size_t(frame-100)]);
    }
    for(int frame=160;frame<1000;++frame) if(!f.frame(frame)) { CHECK(false); return; }
    CHECK_EQ(f.battle.seats[0].bomb_generation,2u);
    CHECK_EQ(f.battle.seats[1].bomb_generation,2u);
    CHECK_EQ(f.battle.seat_huds[0].bombs,0);
    CHECK_EQ(f.battle.seat_huds[1].bombs,0);
}
TEST(all_four_duplicate_characters_keep_separate_bomb_families) {
    const char* names[]={"pl00.anm","pl01.anm","pl02.anm","pl03.anm"};
    for(i32 character=0;character<4;++character) {
        Fixture f; const bool loaded=f.load(names[character]); CHECK(loaded); if(!loaded)return;
        const bool booted=f.boot({character,character,0}); CHECK(booted); if(!booted)return;
        for(int frame=0;frame<1000;++frame) {
            if(!f.frame(frame,true)) { CHECK(false); return; }
            if(frame==130) {
                StagePresentation::SnapshotData state; CHECK(f.presentation.capture_presentation(state));
                const auto& teammate=state.teammate_effects[0];
                CHECK(state.bomb_background!=0 && teammate.bomb_background!=0);
                CHECK(state.bomb_background!=teammate.bomb_background);
                if(character==1) CHECK(state.bomb_beam!=0 && teammate.bomb_beam!=0 && state.bomb_beam!=teammate.bomb_beam);
                if(character==2) for(size_t ray=0;ray<5;++ray)
                    CHECK(state.bomb_ray_handles[ray]!=0 && teammate.bomb_ray_handles[ray]!=0 && state.bomb_ray_handles[ray]!=teammate.bomb_ray_handles[ray]);
                if(character==3) CHECK(state.youmu_slash_handles[0]!=0 && teammate.youmu_slash_handles[0]!=0 && state.youmu_slash_handles[0]!=teammate.youmu_slash_handles[0]);
                for(u32 i=0;i<f.animations.registry.ordered_count(false);++i) {
                    const auto* vm=f.animations.registry.find(f.animations.registry.ordered_handle(false,i));
                    CHECK(vm && (vm->shot_effect_owner==1 || vm->shot_effect_owner==2));
                    if(vm) CHECK(vm->resource==f.animations.resource(vm->shot_effect_owner==1?6:7));
                }
            }
        }
        CHECK_EQ(f.battle.seats[0].bomb_generation,2u);
        CHECK_EQ(f.battle.seats[1].bomb_generation,2u);
        CHECK_EQ(f.battle.seat_huds[0].bombs,0);
        CHECK_EQ(f.battle.seat_huds[1].bombs,0);
    }
}
TEST(mixed_character_options_focus_and_death_stay_with_their_seat) {
    Fixture f; CHECK(f.load());
    for(const auto& bank:std::array<std::pair<i32,const char*>,2>{{{7,"pl01.anm"},{9,"pl03.anm"}}}) {
        f.animations.unload(bank.first);const auto bytes=asset(bank.second);
        CHECK(f.animations.load(bank.first,bytes.data(),bytes.size()));
    }
    const bool booted=f.boot({0,1,3},3); CHECK(booted); if(!booted)return;
    for(int frame=-100;frame<0;++frame) if(!f.frame(frame)) { CHECK(false); return; }
    for(i32 seat=0;seat<3;++seat) {
        f.battle.seats[size_t(seat)].options[0].x=float(-80+seat*70);
        f.battle.seats[size_t(seat)].options[0].y=320;
    }
    f.battle.seats[1].focus=true;
    CHECK(f.presentation.initialize_player_effects(f.battle));
    StagePresentation::SnapshotData before; CHECK(f.presentation.capture_presentation(before));
    const std::array<u32,3> roots{before.option_handles[0],before.teammate_effects[0].option_handles[0],before.teammate_effects[1].option_handles[0]};
    const std::array<i32,3> scripts{16,18,15};
    CHECK(roots[0]!=roots[1] && roots[1]!=roots[2] && roots[0]!=roots[2]);
    for(i32 seat=0;seat<3;++seat) {
        const auto* vm=f.animations.registry.find(roots[size_t(seat)]);CHECK(vm);
        if(!vm)return;
        CHECK_EQ(vm->shot_effect_owner,u8(seat+1));CHECK_EQ(vm->source_script,scripts[size_t(seat)]);
        CHECK(vm->resource==f.animations.resource(seat==0?6:seat==1?7:9));
        CHECK_EQ(vm->visual.translation.x,float(-80+seat*70));
    }
    CHECK_EQ(before.focus_handle,0u);CHECK(before.teammate_effects[0].focus_handle!=0);CHECK_EQ(before.teammate_effects[1].focus_handle,0u);
    auto& dying=f.battle.seats[1];dying.phase=StageBattle::PlayerPhase::Dying;dying.death_option_rebuild=true;
    CHECK(f.presentation.initialize_player_effects(f.battle));
    StagePresentation::SnapshotData after; CHECK(f.presentation.capture_presentation(after));
    CHECK(!dying.death_option_rebuild);CHECK(after.teammate_effects[0].death_options_queued);
    CHECK(!after.death_options_queued && !after.teammate_effects[1].death_options_queued);
    CHECK(after.option_handles == before.option_handles);
    CHECK(after.teammate_effects[1].option_handles == before.teammate_effects[1].option_handles);
    CHECK(f.presentation.hide_player_options());CHECK(f.presentation.interrupt_player_options());
    CHECK(f.presentation.capture_presentation(after));
    CHECK(after.options_hidden && after.teammate_effects[0].options_hidden && after.teammate_effects[1].options_hidden);
    CHECK(after.option_rebuild_pending && after.teammate_effects[0].option_rebuild_pending && after.teammate_effects[1].option_rebuild_pending);
    CHECK(f.presentation.activate_player_effects(f.battle));CHECK(f.presentation.capture_presentation(after));
    CHECK(!after.options_hidden && !after.teammate_effects[0].options_hidden && !after.teammate_effects[1].options_hidden);
    CHECK(!after.option_rebuild_pending && !after.teammate_effects[0].option_rebuild_pending && !after.teammate_effects[1].option_rebuild_pending);
}
TEST(reimu_slow_time_zero_age_births_once_and_replays_exactly) {
    // Both rates repeat integral age zero. Count the real synchronous births,
    // retain their registry handles and replay across the fractional timer.
    for(float rate : {0.5f,0.25f}) {
        Fixture f;const bool loaded=f.load();CHECK(loaded);if(!loaded)return;
        const bool booted=f.boot();CHECK(booted);if(!booted)return;
        for(int frame=-100;frame<0;++frame) if(!f.frame(frame)) {CHECK(false);return;}
        std::array<u32,3> births{};
        f.battle.reimu_bomb_orb_birth=[&](const StageBattle& battle,i32 orb) {
            ++births[size_t(battle.active_seat)];
            return f.presentation.reimu_bomb_orb_birth(battle,orb);
        };
        f.battle.sim.set_time_rate(rate);
        if(!f.frame(80)) {CHECK(false);return;}  // P2 presses Bomb.
        CHECK_EQ(f.battle.seats[1].bomb_age,0);
        CHECK_EQ(births[0],0u);CHECK_EQ(births[1],8u);
        StagePresentation::SnapshotData first;CHECK(f.presentation.capture_presentation(first));
        const auto handles=first.teammate_effects[0].bomb_orb_handles;
        multiplayer::WorldState saved;CHECK(saved.capture(f.battle,f.random,nullptr,&f.presentation));
        std::array<u64,12> digests{};
        for(int frame=0;frame<12;++frame) {
            const bool ok=f.frame(81+frame);CHECK(ok);if(!ok)return;
            CHECK_EQ(births[1],8u);
            CHECK_EQ(f.battle.seats[1].bomb_age,i32(float(frame+1)*rate));
            StagePresentation::SnapshotData state;CHECK(f.presentation.capture_presentation(state));
            CHECK(state.teammate_effects[0].bomb_orb_handles==handles);
            multiplayer::WorldState world;CHECK(world.capture(f.battle,f.random,nullptr,&f.presentation));
            digests[size_t(frame)]=world.digest();
        }
        CHECK(saved.restore(f.battle,f.random,nullptr,&f.presentation));
        for(int frame=0;frame<12;++frame) {
            const bool ok=f.frame(81+frame);CHECK(ok);if(!ok)return;
            CHECK_EQ(births[1],8u);
            multiplayer::WorldState world;CHECK(world.capture(f.battle,f.random,nullptr,&f.presentation));
            CHECK_EQ(world.digest(),digests[size_t(frame)]);
        }
    }
}
TEST(teammate_presentation_fields_are_captured_and_hashed) {
    Fixture f; CHECK(f.load()); CHECK(f.boot()); if(!f.battle.error().empty() || !f.presentation.error().empty()) return;
    StagePresentation::SnapshotData original; CHECK(f.presentation.capture_presentation(original));
    multiplayer::WorldState world; CHECK(world.capture(f.battle,f.random,nullptr,&f.presentation));
    const auto base=world.digest();
    const auto check_mutation=[&](auto mutate) {
        auto changed=original; mutate(changed.teammate_effects[0]);
        CHECK(f.presentation.restore_presentation(changed));
        CHECK(world.capture(f.battle,f.random,nullptr,&f.presentation)); CHECK(world.digest()!=base);
        CHECK(f.presentation.restore_presentation(original));
        CHECK(world.capture(f.battle,f.random,nullptr,&f.presentation)); CHECK_EQ(world.digest(),base);
    };
    check_mutation([](auto& v){v.youmu_slash_handles[0]++;});
    check_mutation([](auto& v){v.youmu_slash_extra.push_back(1);});
    check_mutation([](auto& v){v.option_handles[0]++;});
    check_mutation([](auto& v){v.full_option_handles[0]++;});
    check_mutation([](auto& v){v.bomb_orb_handles[0]++;});
    check_mutation([](auto& v){v.bomb_orb_exploding[0]=!v.bomb_orb_exploding[0];});
    check_mutation([](auto& v){v.bomb_ray_handles[0]++;});
    check_mutation([](auto& v){v.focus_handle++;});
    check_mutation([](auto& v){v.option_character++;});
    check_mutation([](auto& v){v.option_count++;});
    check_mutation([](auto& v){v.option_anchor_deferred=!v.option_anchor_deferred;});
    check_mutation([](auto& v){v.option_rebuild_pending=!v.option_rebuild_pending;});
    check_mutation([](auto& v){v.options_hidden=!v.options_hidden;});
    check_mutation([](auto& v){v.death_options_queued=!v.death_options_queued;});
    check_mutation([](auto& v){v.full_options=!v.full_options;});
    check_mutation([](auto& v){v.bomb_generation++;});
    check_mutation([](auto& v){v.bomb_background++;});
    check_mutation([](auto& v){v.bomb_beam++;});
}

TEST(equal_generation_shot_fades_are_independent_for_every_seat) {
    Fixture f; CHECK(f.load()); CHECK(f.boot({0,0,0},3));
    const auto stage=asset("st01enm.anm");
    CHECK(!stage.empty() && f.animations.load(4,stage.data(),stage.size()));
    f.presentation.enemies_from_ecl=true;
    f.battle.defer_item_side=true;
    // Real pl00 script10 carries the hit interrupt. Equal generations are
    // legal because every seat owns its own projectile generation counter.
    for(i32 seat=0;seat<3;++seat) {
        PlayerShot shot; shot.alive=true; shot.state=2; shot.script=10;
        shot.generation=7; shot.owner_seat=seat; shot.x=float(seat*40); shot.y=250;
        f.battle.seats[size_t(seat)].shots.push_back(shot);
    }
    CHECK(f.presentation.sync(f.battle,0));
    StagePresentation::SnapshotData snapshot;
    CHECK(f.presentation.capture_presentation(snapshot));
    CHECK_EQ(snapshot.shot_faded_generations.size(),size_t(3));
    std::set<u32> handles;
    for(size_t seat=0;seat<3;++seat) {
        const u32 handle=snapshot.shot_handles[seat]; handles.insert(handle);
        const auto* vm=f.animations.registry.find(handle); CHECK(vm);
        if(!vm)return;
        CHECK_EQ(vm->pending_interrupt,1);
        CHECK_EQ(vm->shot_effect_owner,u8(seat+1));
        CHECK(vm->resource==f.animations.resource(seat==0?6:seat==1?7:9));
    }
    CHECK_EQ(handles.size(),size_t(3));
    // Repeating sync must not enqueue another fade for the same seat/generation.
    CHECK(f.animations.update(false));
    CHECK(f.presentation.sync(f.battle,0));
    CHECK(f.presentation.capture_presentation(snapshot));
    CHECK_EQ(snapshot.shot_faded_generations.size(),size_t(3));
    for(size_t seat=0;seat<3;++seat) {
        const auto* vm=f.animations.registry.find(snapshot.shot_handles[seat]); CHECK(vm);
        if(vm) CHECK_EQ(vm->pending_interrupt,0);
    }
}

TEST(shot_fade_snapshot_order_and_restore_boundaries) {
    Fixture f;
    StagePresentation::SnapshotData state;
    CHECK(f.presentation.capture_presentation(state));
    CHECK(state.shot_faded_generations.empty());
    // Simulated accumulated generations, including equal generations across
    // seats and the full 32-bit generation boundary. Deliberately unordered
    // input and duplicates exercise restore semantics rather than assuming a
    // pre-sorted capture.
    const std::vector<u64> input = {
        (u64(2)<<32)|0xffffffffu, 7, (u64(1)<<32)|7,
        0, (u64(2)<<32)|7, (u64(1)<<32)|7, 0xffffffffu,
        (u64(1)<<32), (u64(2)<<32)|0xffffffffu
    };
    state.shot_faded_generations=input;
    CHECK(f.presentation.restore_presentation(state));
    StagePresentation::SnapshotData captured;
    CHECK(f.presentation.capture_presentation(captured));
    const std::set<u64> expected(input.begin(),input.end());
    CHECK(captured.shot_faded_generations==std::vector<u64>(expected.begin(),expected.end()));
    CHECK_EQ(captured.shot_faded_generations.size(),size_t(7));
    // Capture remains a deep value even after replacement and a second restore.
    const auto retained=captured.shot_faded_generations;
    state.shot_faded_generations={(u64(2)<<32)|9, 8, 8};
    CHECK(f.presentation.restore_presentation(state));
    CHECK(f.presentation.capture_presentation(captured));
    CHECK(captured.shot_faded_generations==std::vector<u64>({8,(u64(2)<<32)|9}));
    CHECK_EQ(retained.size(),size_t(7));
    f.presentation.reset_snapshot_caches();
    CHECK(f.presentation.capture_presentation(captured));
    CHECK(captured.shot_faded_generations.empty());
}

TEST(shot_fade_history_preserves_unknown_high_words_and_future_reuse) {
    for(i32 seat=0;seat<3;++seat) for(bool consumed : {false,true}) {
        Fixture f; CHECK(f.load()); CHECK(f.boot({0,0,0},3));
        const auto stage=asset("st01enm.anm");
        CHECK(!stage.empty() && f.animations.load(4,stage.data(),stage.size()));
        f.presentation.enemies_from_ecl=true; f.battle.defer_item_side=true;
        StagePresentation::SnapshotData history;
        CHECK(f.presentation.capture_presentation(history));
        // All u64 values accepted by restore remain observable, even when the
        // high word is outside the three current seats. No live shot or fade
        // owns these historical keys. The same generation can recur after
        // the seat's counter wraps; it must retain the old consumption result.
        history.shot_faded_generations={~u64(0),u64(0xffffffffu)<<32,
            (u64(3)<<32)|7,0xffffffffu,0,~u64(0)};
        const u64 key=(u64(u32(seat))<<32)|7;
        if(consumed) history.shot_faded_generations.push_back(key);
        const std::set<u64> restored(history.shot_faded_generations.begin(),
                                     history.shot_faded_generations.end());
        CHECK(f.presentation.restore_presentation(history));
        StagePresentation::SnapshotData captured;
        CHECK(f.presentation.capture_presentation(captured));
        CHECK(captured.shot_faded_generations==std::vector<u64>(restored.begin(),restored.end()));
        PlayerShot shot; shot.alive=true; shot.state=2; shot.script=10;
        shot.generation=7; shot.owner_seat=seat; shot.x=0; shot.y=250;
        f.battle.seats[size_t(seat)].shots.push_back(shot);
        CHECK(f.presentation.sync(f.battle,0));
        CHECK(f.presentation.capture_presentation(captured));
        CHECK(captured.shot_handles.size()>size_t(seat));
        if(captured.shot_handles.size()<=size_t(seat)) return;
        const auto* vm=f.animations.registry.find(captured.shot_handles[size_t(seat)]);
        CHECK(vm);
        if(vm) CHECK_EQ(vm->pending_interrupt,consumed ? 0 : 1);
        auto expected=restored; expected.insert(key);
        CHECK(captured.shot_faded_generations==std::vector<u64>(expected.begin(),expected.end()));
        // Restoring the older snapshot must replace the later branch deeply.
        CHECK(f.presentation.restore_presentation(history));
        CHECK(f.presentation.capture_presentation(captured));
        CHECK(captured.shot_faded_generations==std::vector<u64>(restored.begin(),restored.end()));
    }
}

TEST(ghost_ship_bodies_remain_visible_with_draw_only_opacity) {
    for(i32 ghost=0;ghost<3;++ghost) {
        Fixture f; CHECK(f.load()); CHECK(f.boot({0,0,0},3));
        const auto stage=asset("st01enm.anm");
        CHECK(!stage.empty() && f.animations.load(4,stage.data(),stage.size()));
        f.presentation.enemies_from_ecl=true; f.battle.defer_item_side=true;
        // Production centered playfield offsets; a zeroed environment would
        // incorrectly cull the negative-x ship in this draw-only fixture.
        f.environment.screen_offsets={320,240,320,16};
        GameInput idle;
        for(i32 frame=0;frame<32;++frame) CHECK(f.battle.step(idle));
        for(i32 seat=0;seat<3;++seat) {
            auto& player=f.battle.seats[size_t(seat)];
            player.x=float((seat-1)*80); player.fixed_x=i32(player.x*128);
        }
        f.battle.mp_local_seat=ghost;
        auto& state=f.battle.seats[size_t(ghost)];
        state.ghost=true; state.dead=true; state.phase=StagePlayerState::Phase::Dying;
        CHECK(f.battle.step(idle));
        CHECK(f.presentation.sync(f.battle,0));
        f.presentation.prepare_player_opacity();
        CHECK_EQ(f.renderer.player_opacity[size_t(ghost+1)],140);
        StagePresentation::SnapshotData snapshot;
        CHECK(f.presentation.capture_presentation(snapshot));
        CHECK(snapshot.has_player);
        CHECK(snapshot.teammate_players[0].has_value());
        CHECK(snapshot.teammate_players[1].has_value());
        CHECK(snapshot.player.visible);
        CHECK(snapshot.teammate_players[0]->visible);
        CHECK(snapshot.teammate_players[1]->visible);
        multiplayer::WorldState before, after;
        CHECK(before.capture(f.battle,f.random,nullptr,&f.presentation));
        for(i32 draw=0;draw<2;++draw) {
            f.graphics.submitted.clear();
            CHECK(f.presentation.draw(1)); f.renderer.flush();
            // Two remote ships and the local ghost: actual production quad
            // submission, including alpha, rather than the opacity setting.
            CHECK_EQ(f.graphics.submitted.size(),size_t(18));
            for(size_t vertex=0;vertex<f.graphics.submitted.size();++vertex)
                CHECK_EQ(f.graphics.submitted[vertex].color>>24,vertex<12?160u:140u);
        }
        CHECK(after.capture(f.battle,f.random,nullptr,&f.presentation));
        CHECK_EQ(before.digest(),after.digest());
    }
}
TEST(focus_exit_tree_does_not_create_an_extra_bullet_collision) {
    // Diagnostic fixture: production input and ANM lifecycle, then inject one
    // stationary enemy bullet into each of the two separated positions.
    Fixture f; const bool loaded=f.load(); CHECK(loaded); if(!loaded)return;
    const bool booted=f.boot(); CHECK(booted); if(!booted)return;
    GameInput input;
    const auto step=[&](u32 held) {
        input.update(held);
        f.battle.seat_inputs[1].update(0);
        return f.battle.step(input) && f.presentation.initialize_player_effects(f.battle) &&
               f.animations.update(false);
    };
    // Wait out the normal entry invulnerability before testing contact.
    for(int i=0;i<110;++i) { const bool ok=step(0); CHECK(ok); if(!ok)return; }
    // Keep the second seat away from both test locations.
    auto& other=f.battle.seats[1]; other.x=120; other.fixed_x=120*128;
    for(int i=0;i<24;++i) { const bool ok=step(8); CHECK(ok); if(!ok)return; }
    StagePresentation::SnapshotData before;
    CHECK(f.presentation.capture_presentation(before));
    const u32 old_handle=before.focus_handle;
    const auto* old=f.animations.registry.find(old_handle);
    CHECK(old); if(!old)return;
    const float old_x=old->visual.translation.x, old_y=old->visual.translation.y;
    for(int i=0;i<4;++i) { const bool ok=step(128); CHECK(ok); if(!ok)return; }
    { const bool ok=step(128|8); CHECK(ok); if(!ok)return; }
    StagePresentation::SnapshotData after;
    CHECK(f.presentation.capture_presentation(after));
    const auto* old_after=f.animations.registry.find(old_handle);
    const auto* current=f.animations.registry.find(after.focus_handle);
    CHECK(old_after && current && after.focus_handle!=old_handle);
    if(!old_after || !current || after.focus_handle==old_handle)return;
    CHECK_EQ(old_after->source_script,22); CHECK_EQ(current->source_script,22);
    CHECK_EQ(old_after->shot_effect_owner,u8(1)); CHECK_EQ(current->shot_effect_owner,u8(1));
    CHECK_EQ(old_after->visual.translation.x,old_x); CHECK_EQ(old_after->visual.translation.y,old_y);
    const auto& player=f.battle.seats[0];
    CHECK(player.focus); CHECK_EQ(current->visual.translation.x,player.x);
    CHECK_EQ(current->visual.translation.y,player.y);
    CHECK(player.x-old_x>16.f); CHECK_EQ(player.phase,StagePlayerState::Phase::Active);
    CHECK_EQ(player.invulnerable,0);
    StageBullet old_bullet; old_bullet.x=old_x; old_bullet.y=old_y;
    old_bullet.flags=0x12u; old_bullet.hit_size[0]=old_bullet.hit_size[1]=8.f;
    f.battle.sim.bullet_player_collision(old_bullet);
    CHECK_EQ(f.battle.seats[0].phase,StagePlayerState::Phase::Active);
    CHECK_EQ(f.battle.seats[1].phase,StagePlayerState::Phase::Active);
    CHECK(old_bullet.alive);
    CHECK(f.animations.registry.find(old_handle));
    StageBullet current_bullet; current_bullet.x=player.x; current_bullet.y=player.y;
    current_bullet.flags=0x12u; current_bullet.hit_size[0]=current_bullet.hit_size[1]=8.f;
    f.battle.sim.bullet_player_collision(current_bullet);
    CHECK_EQ(f.battle.seats[0].phase,StagePlayerState::Phase::Deathbomb);
    CHECK_EQ(f.battle.seats[1].phase,StagePlayerState::Phase::Active);
    CHECK(f.battle.error().empty());
}
int __attribute__((weak)) main(){return run_all("MP per-seat player effects");}
