// Real Reimu block-10 shot path (R-T2, R-G): a synthetic stationary Boss,
// but production SHT, input, projectile registration and ANM lifecycle.
#include "../../ecl/ecl_test_fixtures.hpp"
#include "multiplayer/WorldState.hpp"
#include "game/StagePresentation.hpp"
#include <fstream>
#include <iterator>
#include <cmath>

using namespace th13;
using namespace th13test;
namespace {
std::vector<u8> asset(const char* name) {
    const char* root=std::getenv("TH13_NATIVE_ASSET_ROOT");
    std::ifstream file(std::string(root?root:"/work/artifacts/reference/raw")+"/"+name,std::ios::binary);
    return {std::istreambuf_iterator<char>(file),{}};
}
struct Graphics final : ZunGraphics {
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
struct Host final : StageBattleServices {
    bool life_hud(i32,i32) override { return true; }
    bool bomb_hud(i32,i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool hud_sound(i32,float) override { return true; }
};
struct Provider final : EclResourceProvider {
    std::vector<u8> ecl;
    std::vector<std::string>* names=nullptr;
    Provider() {
        std::vector<u8> main;
        emit(main,0,23,0,255,0,{100000});
        emit(main,0,10,0,255,0,{});
        ecl=build_scpt({{"main",main}},{"enemy.anm","st01enm.anm"});
    }
    bool read(const std::string& name,std::vector<u8>& out) override {
        out=name=="reimu-path.ecl"?ecl:asset(name.c_str());
        return !out.empty();
    }
    bool animation(u32,const std::string& name) override {
        if(names) names->push_back(name);
        return true;
    }
};
struct Fixture {
    GameRandom random{123}; AnmEnvironment environment;
    AnmManager animations{random,environment}; Graphics graphics;
    AnmRenderer renderer{graphics}; StageBattle battle;
    StagePresentation::Banks banks{4,5,6,8,25,3,{7,9}};
    StagePresentation presentation{animations,renderer,banks,0};
    Host host; Provider provider; SessionState progress; HudPlayerState player; HudScoreState score;
    bool load_bank(i32 slot,const char* name) {
        const auto bytes=asset(name);
        return !bytes.empty() && animations.load(slot,bytes.data(),u32(bytes.size()));
    }
    u32 boss_serial=0;
    bool init(i32 count=3) {
        if(!load_bank(2,"ascii.anm") || !animations.sprite_fallback(2) ||
           !load_bank(5,"bullet.anm") || !load_bank(8,"effect.anm") || !load_bank(25,"astral.anm") ||
           !load_bank(4,"st01enm.anm") || !load_bank(6,"pl00.anm") ||
           !load_bank(7,"pl00.anm") || !load_bank(9,"pl00.anm")) return false;
        provider.names=&battle.sim.anim_list;
        if(!battle.load(provider,"reimu-path.ecl",2,host,progress,player,score) ||
           !battle.enable_seats(provider,count,{0,0,0})) return false;
        battle.mp_registry_hook=&animations.registry;
        battle.defer_item_side=true;
        presentation.enemies_from_ecl=true;
        battle.player_shot_birth=[this](PlayerShot& shot,u32 index) {
            return presentation.initialize_player_shot(shot,index,battle.active_seat);
        };
        if(battle.sim.enemies.empty() || !battle.sim.enemies.front()) return false;
        auto& e=*battle.sim.enemies.front(); boss_serial=e.serial;
        e.life=10000; e.flags=0x1000000u; e.invulnerable=0;
        e.position[0]=10000; e.position[1]=10000;
        return presentation.sync(battle,0) && animations.update(false);
    }
    StageEnemy* find_boss() {
        for(auto& e:battle.sim.enemies) if(e && e->serial==boss_serial) return e.get();
        return nullptr;
    }
    StageEnemy& boss() { auto* e=find_boss(); if(!e) std::abort(); return *e; }
    bool frame(const std::array<u32,3>& input) {
        GameInput local; local.update(input[0]);
        for (int seat=1;seat<3;++seat) battle.seat_inputs[size_t(seat)].update(input[size_t(seat)]);
        const bool ok=battle.step(local) && presentation.sync(battle,input[0]) && animations.update(false);
        if (!ok) std::printf("# frame failure: battle=%s presentation=%s animation=%s\n",
            battle.error().c_str(), presentation.error().c_str(), animations.error.c_str());
        return ok;
    }
};
}

TEST(reimu_trance_shot_real_sht_to_boss_and_rollback) {
    for(i32 shooter=0;shooter<3;++shooter) {
        Fixture f; const bool initialized=f.init(); CHECK(initialized);
        if(!initialized) continue;
        CHECK(f.find_boss()); if(!f.find_boss()) continue;
        // Normal C edge, after the actual 30-frame activation gate.
        f.progress.ending_points=600;
        for(int frame=0;frame<32;++frame) CHECK(f.frame({0,0,0}));
        std::array<u32,3> buttons{};
        buttons[size_t(shooter)]=0x800;
        CHECK(f.frame(buttons));
        CHECK(f.battle.team_trance_active());
        CHECK_EQ(f.battle.trance_owner_seat,shooter);
        buttons[size_t(shooter)]=1;
        for(int n=0;n<4 && f.battle.seats[size_t(shooter)].shots.empty();++n)
            CHECK(f.frame(buttons));
        CHECK(f.find_boss()); if(!f.find_boss()) continue;
        auto& shots=f.battle.seats[size_t(shooter)].shots;
        CHECK_EQ(shots.size(),size_t(2));
        if(shots.size()!=2) continue;
        std::array<u32,2> generation{}, handle{};
        std::array<i32,2> registration{};
        StagePresentation::SnapshotData visual;
        CHECK(f.presentation.capture_presentation(visual));
        for(size_t i=0;i<2;++i) {
            const auto& shot=shots[i];
            CHECK_EQ(shot.owner_seat,shooter);
            CHECK_EQ(shot.damage,22.f); CHECK_EQ(shot.script,14);
            CHECK_EQ(shot.hit_behavior,0); CHECK_EQ(shot.behavior,0);
            CHECK(shot.alive && shot.state==1 && shot.registration_slot>=0);
            generation[i]=shot.generation; registration[i]=shot.registration_slot;
            if(registration[i]<0) continue;
            const auto& reg=f.battle.seats[size_t(shooter)].damage_registrations[size_t(registration[i])];
            CHECK(reg.active && (reg.flags&1u)); CHECK_EQ(reg.owner_seat,shooter);
            CHECK_EQ(reg.generation,generation[i]); CHECK_EQ(reg.damage,22.f);
            CHECK_EQ(reg.width,shot.width); CHECK_EQ(reg.height,shot.height);
            const size_t slot=i*3+size_t(shooter);
            CHECK(slot<visual.shot_handles.size());
            if(slot>=visual.shot_handles.size()) continue;
            handle[i]=visual.shot_handles[slot];
            const auto* vm=f.animations.registry.find(handle[i]);
            CHECK(vm); if(vm) { CHECK_EQ(vm->source_script,14); CHECK_EQ(vm->shot_effect_owner,u8(shooter+1)); }
            if(vm) CHECK(!vm->hide_hit_projectile);
        }
        if(registration[0]<0 || registration[1]<0 || !handle[0] || !handle[1]) continue;
        // Geometry negative control: distant boss cannot consume the actual registrations.
        const float hp=f.boss().life;
        f.battle.sim.enemy_hit(f.boss());
        CHECK_EQ(f.boss().life,hp);
        for(i32 slot:registration) CHECK(f.battle.seats[size_t(shooter)].damage_registrations[size_t(slot)].active);
        // Move only the synthetic Boss, not either projectile or its registration.
        const auto& a=f.battle.seats[size_t(shooter)].damage_registrations[size_t(registration[0])];
        const auto& b=f.battle.seats[size_t(shooter)].damage_registrations[size_t(registration[1])];
        f.boss().position[0]=(a.center_x+b.center_x)*.5f;
        f.boss().position[1]=(a.center_y+b.center_y)*.5f;
        f.boss().hurtbox[0]=100; f.boss().hurtbox[1]=100;
        multiplayer::WorldState saved;
        CHECK(saved.capture(f.battle,f.random,nullptr,&f.presentation));
        const u64 saved_digest=saved.digest();
        // EX spell Bomb immunity consumes the same real shots but blocks HP.
        SpellSnapshot immune; immune.id=109; immune.flags=kSpellActive;
        f.battle.sim.spell.restore(immune); f.battle.sim.bomb_active=true;
        f.battle.sim.enemy_hit(f.boss());
        CHECK_EQ(f.boss().life,hp);
        for(i32 slot:registration)
            CHECK(!f.battle.seats[size_t(shooter)].damage_registrations[size_t(slot)].active);
        CHECK(saved.restore(f.battle,f.random,nullptr,&f.presentation));
        CHECK(f.find_boss()); if(!f.find_boss()) continue;
        auto collide_and_fade=[&](std::vector<u64>& digests) {
            f.battle.sim.enemy_hit(f.boss());
            CHECK_EQ(hp-f.boss().life,29.f); // 44 * 2/3, remainder 4/12
            CHECK_EQ(f.boss().mp_boss_damage_remainder,4);
            for(size_t i=0;i<2;++i) {
                const auto& reg=f.battle.seats[size_t(shooter)].damage_registrations[size_t(registration[i])];
                CHECK(!reg.active && !(reg.flags&1u)); CHECK_EQ(reg.accumulated_damage,22);
                const auto& shot=f.battle.seats[size_t(shooter)].shots[i];
                CHECK_EQ(shot.generation,generation[i]); CHECK_EQ(shot.owner_seat,shooter);
                CHECK_EQ(shot.state,2); CHECK_EQ(shot.registration_slot,-1); CHECK(shot.hit_fade);
            }
            CHECK(f.presentation.sync(f.battle,0));
            for(u32 h:handle) {
                auto* vm=f.animations.registry.find(h);
                CHECK(vm);
                if(!vm) continue;
                CHECK_EQ(vm->pending_interrupt,1);
                CHECK(vm->hide_hit_projectile);
                CHECK_EQ(f.renderer.draw(*vm),-1);
            }
            CHECK(f.animations.update(false));
            // pl00.anm script14 label1 is op403(192), then op409(30,0,0).
            // Mode 0 is linear: f32(frame/30), x87 192-192*weight,
            // then one integer truncation. The first tick advances to frame 1.
            const auto expected_alpha=[](int tick) {
                const float weight=float(tick)/30.f;
                return u32(int(192.0-192.0*double(weight)));
            };
            std::array<u32,2> initial_alpha{};
            for(size_t i=0;i<handle.size();++i) {
                const auto* vm=f.animations.registry.find(handle[i]); CHECK(vm);
                if(vm) {
                    CHECK_EQ(vm->pending_interrupt,0);
                    CHECK(vm->hide_hit_projectile);
                    initial_alpha[i]=vm->visual.color>>24;
                    CHECK_EQ(initial_alpha[i],expected_alpha(1));
                }
            }
            bool disappeared=false;
            std::array<bool,2> faded{};
            for(int frame=0;frame<80;++frame) {
                const bool stepped=f.frame({0,0,0}); CHECK(stepped); if(!stepped) break;
                for(size_t i=0;i<handle.size();++i) if(const auto* vm=f.animations.registry.find(handle[i])) {
                    CHECK_EQ(vm->visual.color >> 24,expected_alpha(frame+2));
                    if ((vm->visual.color >> 24) < initial_alpha[i]) faded[i]=true;
                }
                multiplayer::WorldState state;
                CHECK(state.capture(f.battle,f.random,nullptr,&f.presentation));
                digests.push_back(state.digest());
                if(!f.animations.registry.find(handle[0]) && !f.animations.registry.find(handle[1])) {
                    disappeared=true; break;
                }
            }
            CHECK(faded[0] && faded[1]);
            CHECK(disappeared);
            CHECK(f.frame({0,0,0}));
            for(const auto& shot:f.battle.seats[size_t(shooter)].shots)
                for(u32 gen:generation) CHECK(shot.generation!=gen || !shot.alive);
        };
        std::vector<u64> first, replay;
        collide_and_fade(first);
        CHECK(saved.restore(f.battle,f.random,nullptr,&f.presentation));
        CHECK(f.find_boss()); if(!f.find_boss()) continue;
        multiplayer::WorldState restored;
        CHECK(restored.capture(f.battle,f.random,nullptr,&f.presentation));
        CHECK_EQ(restored.digest(),saved_digest);
        collide_and_fade(replay);
        CHECK_EQ(first.size(),replay.size());
        for(size_t i=0;i<std::min(first.size(),replay.size());++i) CHECK_EQ(first[i],replay[i]);
    }
}
TEST(reimu_team_trance_nonowner_natural_boss_and_geometry_boundaries) {
    for(i32 count:{2,3}) for(i32 shooter=0;shooter<count;++shooter) {
        Fixture f; const bool initialized=f.init(count); CHECK(initialized); if(!initialized)continue;
        f.progress.ending_points=600;
        for(int frame=0;frame<32;++frame) CHECK(f.frame({0,0,0}));
        const i32 owner=(shooter+1)%count;
        std::array<u32,3> input{}; input[size_t(owner)]=0x800;
        CHECK(f.frame(input)); CHECK(f.battle.team_trance_active());
        CHECK_EQ(f.battle.trance_owner_seat,owner); CHECK(owner!=shooter);
        input={};input[size_t(shooter)]=1;
        for(int n=0;n<4 && f.battle.seats[size_t(shooter)].shots.empty();++n) CHECK(f.frame(input));
        const auto& shots=f.battle.seats[size_t(shooter)].shots;
        CHECK_EQ(shots.size(),size_t(2)); if(shots.size()!=2)continue;
        for(const auto& shot:shots) {
            CHECK_EQ(shot.owner_seat,shooter); CHECK_EQ(shot.script,14);
            CHECK_EQ(shot.damage,22.f); CHECK_EQ(shot.hit_behavior,0);
            CHECK(shot.registration_slot>=0);
        }
        CHECK(f.find_boss()); if(!f.find_boss())continue;
        const auto& shot=shots[0];
        const auto& reg=f.battle.seats[size_t(shooter)].damage_registrations[size_t(shot.registration_slot)];
        // After the next normal player update, shot14 moves 24 pixels at
        // -pi/2. A synthetic axis-aligned enemy whose right edge is one pixel
        // beyond or exactly at the shot's left edge gives a geometric control.
        const float next_x=reg.center_x, next_y=reg.center_y-shot.speed;
        const float half_x=reg.height*.5f, half_y=reg.width*.5f;
        auto prepare=[&](float separation) {
            auto& boss=f.boss(); boss.flags=0x1000000u; boss.invulnerable=0;
            boss.dead=false; boss.life=10000; boss.spell_active=false;
            boss.hurtbox[0]=boss.hurtbox[1]=2;
            const float x=next_x-half_x-1.f-separation;
            boss.position[0]=x;boss.position[1]=next_y;
            boss.pos.pos[0]=boss.pos.pos[1]=0;
            boss.vel.pos[0]=x;boss.vel.pos[1]=next_y;
            boss.tick_gate=false;
        };
        // Inspect both registrations and exclude the other shot from the
        // tiny box; inputs are released before the natural enemy update.
        CHECK(std::fabs(shots[1].x-shot.x)>2.f*half_x+2.f);
        multiplayer::WorldState before;CHECK(before.capture(f.battle,f.random,nullptr,&f.presentation));
        prepare(1.f);
        CHECK(f.frame({0,0,0})); CHECK(f.find_boss());if(!f.find_boss())continue;
        CHECK_EQ(f.boss().life,10000.f);
        CHECK(f.battle.seats[size_t(shooter)].damage_registrations[size_t(shot.registration_slot)].active);
        CHECK(before.restore(f.battle,f.random,nullptr,&f.presentation));
        CHECK(f.find_boss());if(!f.find_boss())continue;
        prepare(0.f);
        CHECK(f.frame({0,0,0})); CHECK(f.find_boss());if(!f.find_boss())continue;
        CHECK_EQ(10000.f-f.boss().life,count==2?16.f:14.f);
        CHECK_EQ(f.boss().mp_boss_damage_remainder,count==2?6:8);
        CHECK_EQ(f.battle.seats[size_t(shooter)].shots[0].state,2);
        (void)half_y;
    }
}
int __attribute__((weak)) main() { return run_all("Reimu trance real shot to Boss"); }
