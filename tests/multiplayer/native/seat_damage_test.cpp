// R-G: real enemy_hit -> damage registration collision -> enemy HP. The
// fixture supplies registered projectiles, never a replacement damage formula.
#include "../../ecl/ecl_test_fixtures.hpp"
#include "multiplayer/WorldState.hpp"
#include <fstream>
#include <iterator>
using namespace th13;
using namespace th13test;
namespace {
struct Host final : StageBattleServices {
    bool life_hud(i32,i32) override { return true; }
    bool bomb_hud(i32,i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool hud_sound(i32,float) override { return true; }
};
struct Provider final : EclResourceProvider {
    std::vector<u8> ecl;
    std::vector<std::string>* names = nullptr;
    Provider() {
        std::vector<u8> main;
        emit(main,0,23,0,255,0,{100000}); emit(main,0,10,0,255,0,{});
        ecl=build_scpt({{"main",main}},{"enemy.anm","st01enm.anm"});
    }
    bool read(const std::string& name,std::vector<u8>& out) override {
        if(name=="damage.ecl") { out=ecl;return true; }
        const char* root=std::getenv("TH13_NATIVE_ASSET_ROOT");
        std::ifstream file(std::string(root?root:"/work/artifacts/reference/raw")+"/"+name,std::ios::binary);
        out={std::istreambuf_iterator<char>(file),{}};return !out.empty();
    }
    bool animation(u32,const std::string& name) override { names->push_back(name);return true; }
};
struct Fixture {
    Host host;Provider provider;SessionState progress;HudPlayerState hud;HudScoreState score;
    StageBattle battle;GameRandom visual{123};
    bool boot(i32 count=2) {
        provider.names=&battle.sim.anim_list;
        if(!battle.load(provider,"damage.ecl",2,host,progress,hud,score) ||
           !battle.enable_seats(provider,count,{0,0,0})) return false;
        auto& enemy=*battle.sim.enemies.front();enemy.life=10000;enemy.flags=0;
        enemy.invulnerable=0;enemy.position[0]=0;enemy.position[1]=100;
        for(i32 seat=0;seat<count;++seat) {
            auto& state=battle.seats[size_t(seat)];state.phase=StagePlayerState::Phase::Active;
            state.phase_clock.current=1;state.phase_clock.previous=0;state.invulnerable=0;
        }
        return true;
    }
    StageEnemy& enemy() { return *battle.sim.enemies.front(); }
    void arm(i32 seat,float amount,bool area=false,u32 flags=1) {
        auto& state=battle.seats[size_t(seat)];state.shots.clear();state.damage_registrations={};
        PlayerShot shot;shot.alive=true;shot.x=0;shot.y=100;shot.damage=amount;
        shot.generation=1;shot.owner_seat=seat;shot.registration_slot=0;state.shots.push_back(shot);
        auto& reg=state.damage_registrations[0];reg.active=true;reg.flags=flags;
        reg.owner=area?DamageRegistration::Owner::Area:DamageRegistration::Owner::Shot;
        reg.center_x=0;reg.center_y=100;reg.width=8;reg.height=8;
        reg.damage=amount;reg.generation=1;reg.owner_seat=seat;
    }
    float collide() {
        const float before=enemy().life;battle.sim.enemy_hit(enemy());return before-enemy().life;
    }
};
}
TEST(second_seat_registered_shot_hits_ordinary_enemy) {
    Fixture f;CHECK(f.boot());f.arm(1,8);
    CHECK_EQ(f.collide(),8.f);
    CHECK_EQ(f.battle.seats[1].shots[0].state,2);
    CHECK(!f.battle.seats[1].damage_registrations[0].active);
    CHECK_EQ(f.battle.active_seat,0);
    CHECK(f.battle.shots.empty());CHECK(f.battle.error().empty());
}
TEST(second_seat_registered_shot_hits_boss_at_cooperative_rate) {
    Fixture f;CHECK(f.boot());f.enemy().flags|=0x1000000u;f.arm(1,8);
    CHECK_EQ(f.collide(),6.f);
    CHECK_EQ(f.enemy().spirit_accum,6.f);
    CHECK_EQ(f.battle.seats[1].shots[0].state,2);
}
TEST(all_seats_sum_hits_once_and_keep_their_registration_side_effects) {
    for(i32 count:{2,3}) for(bool boss:{false,true}) {
        Fixture f;CHECK(f.boot(count));if(boss)f.enemy().flags|=0x1000000u;
        for(i32 seat=0;seat<count;++seat)f.arm(seat,12);
        CHECK_EQ(f.collide(),float(count*(boss?(count==2?9:8):12)));
        CHECK_EQ(f.score.score,count);
        for(i32 seat=0;seat<count;++seat) {
            CHECK_EQ(f.battle.seats[size_t(seat)].shots[0].state,2);
            CHECK(!f.battle.seats[size_t(seat)].damage_registrations[0].active);
            CHECK_EQ(f.battle.seats[size_t(seat)].damage_registrations[0].accumulated_damage,12);
        }
        CHECK_EQ(f.battle.active_seat,0);CHECK_EQ(f.collide(),0.f);
    }
}
TEST(owner_phase_reductions_and_ghosts_do_not_follow_observer_window) {
    Fixture f;CHECK(f.boot(3));
    f.battle.seats[0].phase=StagePlayerState::Phase::Dying;
    f.battle.seats[2].ghost=true;
    for(i32 seat=0;seat<3;++seat)f.arm(seat,25);
    CHECK_EQ(f.collide(),30.f); // P1 25/5 + P2 25; ghost contributes nothing.
    CHECK_EQ(f.battle.seats[2].shots[0].state,1);
    CHECK_EQ(f.score.score,2);
    CHECK_EQ(f.battle.active_seat,0);
}
TEST(boss_fractional_damage_survives_dynamic_operable_counts) {
    for(i32 count:{2,3}) {
        Fixture f;CHECK(f.boot(count));f.enemy().flags|=0x1000000u;
        const std::array<float,4> expected=count==2?std::array<float,4>{0,1,1,1}
                                                  :std::array<float,4>{0,1,1,0};
        const i32 period=count==2?4:3;
        for(i32 hit=0;hit<period;++hit) { f.arm(1,1);CHECK_EQ(f.collide(),expected[size_t(hit)]); }
        CHECK_EQ(f.enemy().mp_boss_damage_remainder,0);
        CHECK_EQ(f.enemy().spirit_accum,float(count==2?3:2));
    }
    Fixture f;CHECK(f.boot(3));f.enemy().flags|=0x1000000u;
    f.arm(0,1);CHECK_EQ(f.collide(),0.f);CHECK_EQ(f.enemy().mp_boss_damage_remainder,8);
    f.battle.seats[2].ghost=true;
    f.arm(0,1);CHECK_EQ(f.collide(),1.f);CHECK_EQ(f.enemy().mp_boss_damage_remainder,5);
    f.battle.seats[1].ghost=true;
    f.arm(0,1);CHECK_EQ(f.collide(),1.f);CHECK_EQ(f.enemy().mp_boss_damage_remainder,5);
    f.battle.seats[1].ghost=false;
    f.arm(0,1);CHECK_EQ(f.collide(),1.f);CHECK_EQ(f.enemy().mp_boss_damage_remainder,2);
    // A pilot on their final death stops counting before the later ghost
    // transition; a pilot with remaining lives still counts while respawning.
    Fixture final;CHECK(final.boot());final.enemy().flags|=0x1000000u;
    final.battle.seat_huds[1].extra_lives=-1;
    final.battle.seats[1].phase=StagePlayerState::Phase::Dying;
    final.arm(0,12);CHECK_EQ(final.collide(),12.f);
}
TEST(boss_remainder_is_authoritative_and_restores_before_replay) {
    Fixture f;CHECK(f.boot());f.enemy().flags|=0x1000000u;
    f.arm(1,1);CHECK_EQ(f.collide(),0.f);
    multiplayer::WorldState saved;CHECK(saved.capture(f.battle,f.visual));
    const u64 digest=saved.digest();
    f.enemy().mp_boss_damage_remainder=0;
    multiplayer::WorldState changed;CHECK(changed.capture(f.battle,f.visual));CHECK(changed.digest()!=digest);
    CHECK(saved.restore(f.battle,f.visual));CHECK_EQ(f.enemy().mp_boss_damage_remainder,9);
    std::array<u64,12> expected{};
    for(i32 frame=0;frame<12;++frame) {
        f.arm(1,float(frame%4+1));f.collide();
        multiplayer::WorldState state;CHECK(state.capture(f.battle,f.visual));expected[size_t(frame)]=state.digest();
    }
    CHECK(saved.restore(f.battle,f.visual));
    for(i32 frame=0;frame<12;++frame) {
        f.arm(1,float(frame%4+1));f.collide();
        multiplayer::WorldState state;CHECK(state.capture(f.battle,f.visual));CHECK_EQ(state.digest(),expected[size_t(frame)]);
    }
}
TEST(second_seat_bomb_cancels_bomb_sensitive_enemy) {
    Fixture f;CHECK(f.boot());f.enemy().flags|=0x800u;
    f.arm(1,8,true,1u|4u);f.collide();
    CHECK(f.enemy().dead);CHECK(f.battle.error().empty());
}
TEST(ship_callback_forwards_summed_damage_to_boss_once) {
    Fixture f;CHECK(f.boot());
    auto* boss=f.battle.sim.spawn_enemy("main",0,100,10000,nullptr,false);
    CHECK(boss);if(!boss)return;
    boss->flags=0x1000000u;boss->invulnerable=0;
    f.battle.sim.boss_slot[0]=boss->script_id;
    f.enemy().damage_function=1;
    f.arm(0,4);f.arm(1,8);
    CHECK_EQ(f.collide(),24.f);
    CHECK_EQ(boss->pending_damage,12);
    const float before=boss->life;f.battle.sim.enemy_hit(*boss);
    CHECK_EQ(before-boss->life,9.f);CHECK_EQ(boss->pending_damage,0);
    f.battle.sim.enemy_hit(*boss);CHECK_EQ(before-boss->life,9.f);
}
TEST(hit_spirit_distance_uses_nearest_eligible_seat) {
    Fixture f;CHECK(f.boot());f.arm(1,12);f.enemy().spirit_threshold=0;
    auto& sim=f.battle.sim;sim.seat_count=2;
    sim.seat_x={200,0,0};sim.seat_y={400,100,0};sim.seat_eligible={true,true,false};
    CHECK_EQ(f.collide(),12.f);CHECK_EQ(f.enemy().spirit_threshold,200);
    f.enemy().spirit_threshold=0;f.arm(1,12);sim.seat_eligible[1]=false;
    CHECK_EQ(f.collide(),12.f);CHECK_EQ(f.enemy().spirit_threshold,1000);
}
TEST(owner_protection_and_spell_damage_gates_precede_boss_cooperation) {
    for(bool protected_owner:{false,true}) {
        Fixture f;CHECK(f.boot());f.enemy().flags|=0x1000000u;f.enemy().life=1000;
        SpellSnapshot spell;spell.id=102;spell.flags=kSpellActive;f.battle.sim.spell.restore(spell);
        // P1 is protected, but its observation window must not reduce P2.
        f.battle.seats[0].invulnerable=120;
        f.battle.seats[1].invulnerable=protected_owner?120:0;
        f.arm(1,20);CHECK_EQ(f.collide(),protected_owner?3.f:15.f);
    }
    Fixture reduced;CHECK(reduced.boot());reduced.enemy().flags|=0x1000000u;
    SpellSnapshot spell;spell.id=1;spell.flags=kSpellActive|kSpellBombProtection;
    reduced.battle.sim.spell.restore(spell);
    reduced.arm(0,60);reduced.arm(1,60);
    CHECK_EQ(reduced.collide(),3.f);CHECK_EQ(reduced.enemy().spirit_accum,3.f);
    Fixture immune;CHECK(immune.boot());immune.enemy().flags|=0x1000000u;
    spell.id=109;spell.flags=kSpellActive;immune.battle.sim.spell.restore(spell);
    immune.battle.sim.bomb_active=true;immune.arm(1,60);
    CHECK_EQ(immune.collide(),0.f);CHECK_EQ(immune.enemy().mp_boss_damage_remainder,0);
    CHECK_EQ(immune.enemy().spirit_accum,0.f);CHECK_EQ(immune.battle.seats[1].shots[0].state,2);
}
int __attribute__((weak)) main() { return run_all("MP all-seat collision and Boss cooperation"); }
