#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
#include "game/SessionState.hpp"
using namespace th13;
namespace {
struct Rig {
    StageSim sim;StageEnemy& enemy;EclContext ctx;
    StageEnemy* spawn() {
        std::vector<u8> root;th13test::emit(root,0,23,0,255,0,{600});
        th13test::emit(root,0,10,0,255,0,{});
        auto data=th13test::build_scpt({{"main",root}},{});
        sim.program.attach(data.data(),u32(data.size()));
        sim.commands.sim=&sim;sim.gvars.sim=&sim;
        return sim.spawn_enemy("main",12,45,10,nullptr,false);
    }
    Rig():enemy(*spawn()){sim.current=&enemy;enemy.tick_gate=false;}
    void step(){CHECK(sim.step_frame());}
    int op(u16 id,std::initializer_list<u32> args){std::vector<u8> data;th13test::emit(data,0,id,0,255,0,args);ctx.instruction=(const EclInstruction*)data.data();return sim.commands.execute(ctx,id);}
};
TEST(curve_uses_independent_axis_weights_and_relative_mirroring) {
    Rig r;CHECK_EQ(r.op(434,{5,0,4,float_to_bits(96),float_to_bits(-20)}),0);
    CHECK(r.enemy.vel.postime.per_axis);CHECK_EQ(r.enemy.vel.postime.axis_mode[0],0);CHECK_EQ(r.enemy.vel.postime.axis_mode[1],4);
    r.step();CHECK_EQ(r.enemy.vel.postime.elapsed,1);
    CHECK_EQ(r.enemy.vel.pos[0],28.8f);CHECK_EQ(r.enemy.vel.pos[1],21.59f);
    Rig mirrored;mirrored.enemy.flags|=0x100000;
    CHECK_EQ(mirrored.op(438,{5,1,6,float_to_bits(96),float_to_bits(-20)}),0);
    for(int n=0;n<5;++n)mirrored.step();
    CHECK_EQ(mirrored.enemy.vel.pos[0],-84.0f);CHECK_EQ(mirrored.enemy.vel.pos[1],25.0f);
    CHECK_EQ(mirrored.enemy.vel.postime.target[0],-84.0f);
    CHECK_EQ(mirrored.enemy.vel.postime.target[1],25.0f);
}
TEST(curve_followed_by_plain_position_interpolation_clears_axis_mode) {
    Rig r;CHECK_EQ(r.op(434,{30,4,6,float_to_bits(96),float_to_bits(10)}),0);
    CHECK_EQ(r.op(401,{10,0,float_to_bits(22),float_to_bits(55)}),0);
    CHECK(!r.enemy.vel.postime.per_axis);CHECK_EQ(r.enemy.vel.postime.mode,0);
    r.step();CHECK_EQ(r.enemy.vel.pos[0],13.0f);CHECK_EQ(r.enemy.vel.pos[1],46.0f);
}
TEST(radial_emitter_offset_uses_two_float_arguments_and_preserves_z) {
    Rig r;r.enemy.et[2].offset[2]=7;
    CHECK_EQ(r.op(626,{2,float_to_bits(0),float_to_bits(48)}),0);
    CHECK_EQ(r.enemy.et[2].offset[0],48.0f);CHECK_EQ(r.enemy.et[2].offset[1],0.0f);
    CHECK_EQ(r.enemy.et[2].offset[2],7.0f);CHECK_EQ(r.enemy.et[2].spin,0.0f);
    CHECK_EQ(r.op(626,{2,float_to_bits(1.5707964f),float_to_bits(48)}),0);
    CHECK_EQ(r.enemy.et[2].offset[1],48.0f);
}
struct Provider final:EclResourceProvider {
    std::vector<u8> data;std::vector<std::string>* names=nullptr;
    Provider(){std::vector<u8> root,death;
        th13test::emit(root,0,556,0,255,0,{8,0x68746165,0x00000044}); // string corrected below
        const char name[]="Death";std::memcpy(root.data()+20,name,sizeof name);
        th13test::emit(root,0,23,0,255,0,{600});th13test::emit(root,0,10,0,255,0,{});
        th13test::emit(death,0,501,0,255,0,{float_to_bits(31),float_to_bits(47)});
        th13test::emit(death,0,10,0,255,0,{});
        data=th13test::build_scpt({{"Death",death},{"main",root}},{"enemy.anm","st03enm.anm"});
    }
    bool read(const std::string&,std::vector<u8>& out)override{out=data;return true;}
    bool animation(u32,const std::string& name)override{names->push_back(name);return true;}
};
TEST(death_subroutine_executes_once_before_enemy_retirement) {
    StageSim sim;Provider p;p.names=&sim.anim_list;
    CHECK(sim.begin(p,"stage.ecl",8));CHECK(sim.start());
    auto* enemy=sim.enemies.back().get();CHECK_EQ(enemy->death_routine,std::string("Death"));
    enemy->life=10;enemy->invulnerable=0;sim.damage(*enemy,20);
    CHECK(enemy->dead);CHECK(enemy->death_routine.empty());
    CHECK_EQ(enemy->hitbox[0],31.0f);CHECK_EQ(enemy->hitbox[1],47.0f);
}
}
TEST(chapter_changes_reset_only_the_chapter_clock) {
    StageSim sim; SessionState state; StageEnemy enemy; EclContext ctx;
    sim.commands.sim=&sim;sim.current=&enemy;sim.session=&state;
    for(i32 next:{0,4,4,6}) {
        const i32 previous=state.chapter;state.chapter_frame=99;state.stage_frame=500;
        std::vector<u8> code;th13test::emit(code,0,524,0,255,0,{u32(next)});
        ctx.instruction=reinterpret_cast<const EclInstruction*>(code.data());
        CHECK_EQ(sim.commands.execute(ctx,524),0);CHECK_EQ(state.chapter,next);
        CHECK_EQ(state.chapter_frame,previous==next?99:0);CHECK_EQ(state.stage_frame,500);
    }
}
int main(){return th13test::run_all("progression-commands");}
