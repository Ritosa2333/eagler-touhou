// Independent Bomb protection audit: real X input and whole source lifetime.
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
    std::vector<u8> ecl; std::vector<std::string>* names=nullptr;
    Provider() {
        std::vector<u8> main; emit(main,0,23,0,255,0,{100000}); emit(main,0,10,0,255,0,{});
        ecl=build_scpt({{"main",main}},{"enemy.anm","st01enm.anm"});
    }
    bool read(const std::string& name,std::vector<u8>& out) override {
        if(name=="bomb-window.ecl") { out=ecl; return true; }
        const char* root=std::getenv("TH13_NATIVE_ASSET_ROOT");
        std::ifstream file(std::string(root?root:"/work/artifacts/reference/raw")+"/"+name,std::ios::binary);
        out={std::istreambuf_iterator<char>(file),{}};return !out.empty();
    }
    bool animation(u32,const std::string& name) override { if(names)names->push_back(name);return true; }
};
struct Fixture {
    Host host; Provider provider; SessionState progress; HudPlayerState player; HudScoreState score;
    StageBattle battle; GameRandom visual{7};
    bool init(i32 character,i32 owner) {
        provider.names=&battle.sim.anim_list;
        if(!battle.load(provider,"bomb-window.ecl",2,host,progress,player,score)) return false;
        std::array<i32,3> chars{0,0,0}; chars[size_t(owner)]=character;
        if(!battle.enable_seats(provider,3,chars)) return false;
        battle.defer_item_side=true;
        for(int frame=0;frame<130;++frame) if(!step({0,0,0})) return false;
        for(int seat=0;seat<3;++seat) {
            auto& p=battle.seats[size_t(seat)];
            p.x=float((seat-1)*120); p.y=400; p.fixed_x=i32(p.x*128); p.fixed_y=51200;
        }
        return true;
    }
    bool step(std::array<u32,3> keys) {
        GameInput local;local.update(keys[0]);
        for(int seat=1;seat<3;++seat) battle.seat_inputs[size_t(seat)].update(keys[size_t(seat)]);
        return battle.step(local);
    }
    void collision(int target,int kind) {
        const auto& p=battle.seats[size_t(target)];
        if(kind==0) {
            StageBullet hit;hit.alive=true;hit.collision_ready=true;hit.flags=0x12;
            hit.cancel_protection=1;hit.hit_size[0]=hit.hit_size[1]=8;hit.x=p.x;hit.y=p.y;
            battle.sim.bullet_player_collision(hit);
        } else if(kind==1) {
            StageLaser laser;laser.origin[0]=p.x-48;laser.origin[1]=p.y;
            laser.length=laser.target_length=96;laser.width=20;laser.phase=2;
            battle.sim.laser_collision(laser);
        } else {
            StageEnemy enemy;enemy.position[0]=p.x;enemy.position[1]=p.y;
            enemy.hitbox[0]=enemy.hitbox[1]=24;battle.sim.enemy_body_collision(enemy);
        }
    }
};
}
TEST(bomb_window_all_roles_all_seats_each_frame) {
    for(int character=0;character<4;++character) for(int owner=0;owner<3;++owner) {
        Fixture f; const bool initialized=f.init(character,owner); CHECK(initialized);if(!initialized)continue;
        const int target=(owner+1)%3, long_target=(owner+2)%3;
        f.battle.seats[size_t(long_target)].invulnerable=1000;
        std::array<u32,3> keys{}; keys[size_t(owner)]=2;
        CHECK(f.step(keys));
        CHECK_EQ(f.battle.seat_huds[size_t(owner)].bombs,1);
        CHECK(f.battle.seats[size_t(owner)].bomb_protection>0);
        multiplayer::WorldState mid, tail;
        bool saved=false, tail_saved=false;int saved_frame=-1;
        std::vector<u64> forward;
        bool done=false;
        for(int frame=0;frame<500;++frame) {
            const auto& source=f.battle.seats[size_t(owner)];
            const auto& receiver=f.battle.seats[size_t(target)];
            CHECK_EQ(receiver.phase,StagePlayerState::Phase::Active);
            CHECK_EQ(receiver.invulnerable,source.bomb_protection);
            CHECK_EQ(f.battle.seats[size_t(long_target)].invulnerable,999-frame);
            // The source is an independent counter: compare protection truth
            // against the owner's actual player collision gate each frame.
            CHECK_EQ(source.bomb_protection>0,source.invulnerable>0);
            if(source.bomb_protection<=0) { done=true;break; }
            for(int kind=0;kind<3;++kind) {
                f.collision(target,kind);
                CHECK_EQ(f.battle.seats[size_t(target)].phase,StagePlayerState::Phase::Active);
            }
            if(frame==180) { CHECK(mid.capture(f.battle,f.visual));saved=true;saved_frame=frame; }
            if(source.bomb_protection==1) { CHECK(tail.capture(f.battle,f.visual));tail_saved=true; }
            CHECK(f.step({0,0,0}));
            if(saved) { multiplayer::WorldState state; CHECK(state.capture(f.battle,f.visual)); forward.push_back(state.digest()); }
        }
        CHECK(done); CHECK(saved); CHECK(tail_saved); CHECK_EQ(saved_frame,180);
        // Actual unprotected collision proves each geometry is lethal. Restore
        // the same post-tail snapshot before each independent negative control.
        multiplayer::WorldState end;CHECK(end.capture(f.battle,f.visual));
        for(int kind=0;kind<3;++kind) {
            CHECK(end.restore(f.battle,f.visual));f.collision(target,kind);
            CHECK_EQ(f.battle.seats[size_t(target)].phase,StagePlayerState::Phase::Deathbomb);
        }
        if(tail_saved) {
            CHECK(tail.restore(f.battle,f.visual)); CHECK(f.step({0,0,0}));
            multiplayer::WorldState tail_replayed; CHECK(tail_replayed.capture(f.battle,f.visual));
            CHECK_EQ(tail_replayed.digest(),end.digest());
        }
        CHECK(mid.restore(f.battle,f.visual));
        for(size_t frame=0;frame<forward.size();++frame) {
            CHECK(f.step({0,0,0}));
            multiplayer::WorldState state;CHECK(state.capture(f.battle,f.visual));CHECK_EQ(state.digest(),forward[frame]);
        }
    }
}
TEST(bomb_window_overlap_ghost_and_death) {
    for(bool staggered : {false,true}) {
        Fixture f; const bool initialized=f.init(0,0); CHECK(initialized);if(!initialized)continue;
        f.battle.seats[2].ghost=true;
        f.battle.seats[2].invulnerable=7;
        CHECK(f.step(staggered?std::array<u32,3>{2,0,0}:std::array<u32,3>{2,2,0}));
        CHECK_EQ(f.battle.seats[0].bomb_protection,40);
        CHECK_EQ(f.battle.seats[1].invulnerable,40);
        CHECK_EQ(f.battle.seats[2].invulnerable,7);
        if(staggered) {
            for(int frame=0;frame<100;++frame) CHECK(f.step({0,0,0}));
            CHECK(f.step({0,2,0}));
        }
        CHECK(f.battle.seats[0].bomb_protection>0);
        CHECK(f.battle.seats[1].bomb_protection>0);
        multiplayer::WorldState checkpoint;CHECK(checkpoint.capture(f.battle,f.visual));
        // Force owner 0 to expire its last life; its pre-existing protection
        // source continues independently, and the other Bomb is not erased.
        auto& dying=f.battle.seats[0];
        f.battle.seat_huds[0].extra_lives=0;
        dying.phase=StagePlayerState::Phase::Deathbomb;dying.phase_frame=16;
        CHECK(f.step({0,0,0}));
        CHECK(f.battle.seats[0].final_death);
        CHECK_EQ(f.battle.seats[0].bomb_timer,0);
        CHECK(f.battle.seats[1].bomb_protection>0);
        CHECK(f.battle.seats[1].invulnerable>0);
        CHECK(f.battle.seats[2].ghost); CHECK_EQ(f.battle.seats[2].invulnerable,7);
        CHECK(checkpoint.restore(f.battle,f.visual));
        CHECK(!f.battle.seats[0].final_death);
        CHECK(f.battle.seats[0].bomb_protection>0);
        CHECK(f.battle.seats[1].bomb_protection>0);
    }
}
TEST(bomb_tail_spell_damage_uses_teammate_protection) {
    Fixture f;const bool initialized=f.init(0,1);CHECK(initialized);if(!initialized)return;
    CHECK(f.step({0,2,0}));
    while(f.battle.seats[1].bomb_timer>0) CHECK(f.step({0,0,0}));
    CHECK(f.battle.seats[1].bomb_protection>0);
    CHECK_EQ(f.battle.seats[0].invulnerable,f.battle.seats[1].bomb_protection);
    CHECK(!f.battle.sim.bomb_active);
    // The initial ECL root may already have been killed by the real Bomb.
    // Create this diagnostic target through the normal entity constructor.
    auto* target=f.battle.sim.spawn_enemy("main",0,100,1000,nullptr,false);
    CHECK(target!=nullptr); if(!target)return;
    auto& enemy=*target;
    const u32 target_serial=enemy.serial;
    enemy.flags=0x1000000u;enemy.invulnerable=0;enemy.life=1000;
    enemy.spell_active=true;enemy.spell_life=7000;enemy.phase_life_threshold=0;
    enemy.position[0]=0;enemy.position[1]=100;
    enemy.hurtbox[0]=enemy.hurtbox[1]=32;
    SpellSnapshot spell;spell.id=102;spell.flags=kSpellActive;f.battle.sim.spell.restore(spell);
    auto& player=f.battle.seats[0];player.shots.clear();player.damage_registrations={};
    PlayerShot shot;shot.alive=true;shot.x=0;shot.y=100;shot.damage=20;shot.generation=1;
    shot.owner_seat=0;shot.registration_slot=0;player.shots.push_back(shot);
    auto& reg=player.damage_registrations[0];reg.active=true;reg.flags=1;
    reg.owner=DamageRegistration::Owner::Shot;reg.center_x=0;reg.center_y=100;
    reg.width=8;reg.height=8;reg.damage=20;reg.generation=1;reg.owner_seat=0;
    multiplayer::WorldState saved;CHECK(saved.capture(f.battle,f.visual));
    f.battle.sim.enemy_hit(enemy);
    std::printf("# spell probe: dead=%d flags=%x hp=%g spellhp=%d phase=%d clock=%d/%d reg=%d regclock=%d/%d\n",
        enemy.dead,enemy.flags,double(enemy.life),enemy.spell_life,int(player.phase),
        player.phase_clock.current,player.phase_clock.previous,reg.active,reg.clock.current,reg.clock.previous);
    CHECK_EQ(7000-enemy.spell_life,2); // owner protection: floor(20/5)*2/3
    CHECK(saved.restore(f.battle,f.visual));
    f.battle.seats[0].invulnerable=0; // negative control for this consumer
    StageEnemy* restored_target=nullptr;
    for(auto& candidate:f.battle.sim.enemies)
        if(candidate && candidate->serial==target_serial)restored_target=candidate.get();
    CHECK(restored_target!=nullptr);if(!restored_target)return;
    f.battle.sim.enemy_hit(*restored_target);
    CHECK_EQ(7000-restored_target->spell_life,13); // floor(20*2/3)
}
TEST(bomb_start_frame_real_world_collisions) {
    for(int kind=0;kind<3;++kind) for(bool bombing : {false,true}) {
        Fixture f; const bool initialized=f.init(0,2); CHECK(initialized);if(!initialized)continue;
        // High seat starts X while low seat already overlaps a real hazard.
        // The manager callbacks below observe normal sim.step_frame traversal.
        const auto& player=f.battle.seats[0];
        int observed=0;
        int observed_protection=-1, observed_source=-1;
        const auto observe_protection=[&] {
            ++observed;
            observed_protection=f.battle.seats[0].invulnerable;
            observed_source=f.battle.seats[2].bomb_protection;
        };
        if(kind==0) {
            StageBullet hit;hit.alive=true;hit.state=1;hit.flags=0x12;
            hit.cancel_protection=1000;hit.hit_size[0]=hit.hit_size[1]=8;
            hit.x=player.x;hit.y=player.y;hit.generation=9001;
            f.battle.sim.bullets.push_back(hit);
            const auto original=f.battle.sim.bullet_player_collision;
            f.battle.sim.bullet_player_collision=[&,original](StageBullet& bullet) {
                if(bullet.generation==9001)observe_protection(); original(bullet);
            };
        } else if(kind==1) {
            StageLaser laser;laser.origin[0]=player.x-48;laser.origin[1]=player.y;
            laser.length=laser.target_length=96;laser.width=laser.target_width=20;
            laser.phase=2;laser.timing[2]=1000;laser.cancel_protection=1000;laser.generation=9002;
            f.battle.sim.lasers.push_back(laser);
            const auto original=f.battle.sim.laser_collision;
            f.battle.sim.laser_collision=[&,original](const StageLaser& value) {
                if(value.generation==9002)observe_protection(); return original(value);
            };
        } else {
            auto enemy=std::make_unique<StageEnemy>();
            enemy->serial=9003;enemy->life=1000;enemy->flags=1; // no shot damage, live body
            enemy->position[0]=player.x;enemy->position[1]=player.y;
            enemy->vel.pos[0]=player.x;enemy->vel.pos[1]=player.y;
            enemy->hitbox[0]=enemy->hitbox[1]=24;
            f.battle.sim.enemies.push_back(std::move(enemy));
            const auto original=f.battle.sim.enemy_body_collision;
            f.battle.sim.enemy_body_collision=[&,original](StageEnemy& value) {
                if(value.serial==9003)observe_protection();original(value);
            };
        }
        CHECK(f.step({0,0,bombing?2u:0u}));
        std::printf("# start collision kind=%d bombing=%d observed=%d phase=%d inv=%d source=%d\n",
            kind,bombing,observed,int(f.battle.seats[0].phase),f.battle.seats[0].invulnerable,
            f.battle.seats[2].bomb_protection);
        CHECK_EQ(observed,1);
        CHECK_EQ(observed_protection,observed_source);
        CHECK_EQ(observed_protection>0,bombing);
        CHECK_EQ(f.battle.seat_huds[2].bombs,bombing?1:2);
        CHECK_EQ(f.battle.seats[0].phase,bombing?StagePlayerState::Phase::Active:
                 StagePlayerState::Phase::Deathbomb);
        if(bombing) CHECK_EQ(f.battle.seats[0].invulnerable,f.battle.seats[2].bomb_protection);
    }
}
namespace {
bool transition(Fixture& f, std::array<u32, 3> keys) {
    GameInput local;
    local.update(keys[0]);
    for (int seat = 1; seat < 3; ++seat)
        f.battle.seat_inputs[size_t(seat)].update(keys[size_t(seat)]);
    return f.battle.step_transition(local);
}

void verify_transition_window(Fixture& f, int owner, bool ghost_case, int frames) {
    const int receiver = (owner + 1) % 3, other = (owner + 2) % 3;
    const auto& b = f.battle;
    const int source = b.seats[size_t(owner)].bomb_protection;
    // Keep provenance on the caster: sharing must not create new sources.
    CHECK_EQ(b.seats[size_t(receiver)].bomb_protection, 0);
    CHECK_EQ(b.seats[size_t(other)].bomb_protection, 0);
    CHECK_EQ(b.seats[size_t(receiver)].invulnerable, source);
    if (ghost_case) CHECK_EQ(b.seats[size_t(other)].invulnerable, 7);
    else CHECK_EQ(b.seats[size_t(other)].invulnerable, 999 - frames);
}

void transition_window(int character, int owner, bool ghost_case, bool starts_in_transition) {
    Fixture f;
    const bool initialized = f.init(character, owner);
    CHECK(initialized); if (!initialized) return;
    const int other = (owner + 2) % 3;
    if (ghost_case) {
        f.battle.seats[size_t(other)].ghost = true;
        f.battle.seats[size_t(other)].invulnerable = 7;
    } else f.battle.seats[size_t(other)].invulnerable = 1000;
    std::array<u32, 3> keys{}; keys[size_t(owner)] = 2;
    CHECK(starts_in_transition ? transition(f, keys) : f.step(keys));
    CHECK_EQ(f.battle.seat_huds[size_t(owner)].bombs, 1);
    CHECK(f.battle.seats[size_t(owner)].bomb_protection > 0);
    multiplayer::WorldState checkpoint;
    CHECK(checkpoint.capture(f.battle, f.visual));
    std::vector<u64> digests;
    int frames = 0;
    while (f.battle.seats[size_t(owner)].bomb_protection > 0 && frames < 130) {
        verify_transition_window(f, owner, ghost_case, frames);
        CHECK(transition(f, {0, 0, 0}));
        multiplayer::WorldState state; CHECK(state.capture(f.battle, f.visual));
        digests.push_back(state.digest()); ++frames;
    }
    CHECK(frames > 0 && frames <= 120);
    CHECK_EQ(f.battle.seats[size_t(owner)].bomb_protection, 0);
    verify_transition_window(f, owner, ghost_case, frames);
    CHECK(checkpoint.restore(f.battle, f.visual));
    for (u64 digest : digests) {
        CHECK(transition(f, {0, 0, 0}));
        multiplayer::WorldState state; CHECK(state.capture(f.battle, f.visual));
        CHECK_EQ(state.digest(), digest);
    }
}
}

// Transition pauses enemy/bullet/laser managers. These cases verify the actual
// player X path, complete protection state and replay, without claiming that
// an enemy collision naturally runs during the transition.
TEST(transition_real_x_team_window_and_restore) {
    for (int character = 0; character < 4; ++character)
        for (int owner = 0; owner < 3; ++owner)
            for (bool ghost_case : {false, true})
                transition_window(character, owner, ghost_case, true);
}
TEST(transition_existing_bomb_team_window_and_restore) {
    for (int character = 0; character < 4; ++character)
        for (int owner = 0; owner < 3; ++owner)
            for (bool ghost_case : {false, true})
                transition_window(character, owner, ghost_case, false);
}
int __attribute__((weak)) main(){ return run_all("Bomb protection window audit"); }
