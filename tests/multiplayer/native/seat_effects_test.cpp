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
    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&,u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind,const Matrix4&) override {}
    void triangles(u32,const void*,u32) override {}
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
int __attribute__((weak)) main(){return run_all("MP per-seat player effects");}
