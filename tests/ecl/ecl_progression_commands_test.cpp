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
TEST(shared_position_interpolation_rounds_each_stage_to_f32) {
    // 0x422f80 共享权重路径（0x423200 起）三处 f32 落点：
    //   0x423218-0x423230 (target-start) fstp dword [ebp-0x4c/-0x48/-0x44]；
    //   0x423233-0x42324d (delta*w)      fstp dword [ebp-0x40/-0x3c/-0x38]；
    //   0x423250-0x423273 (+start)       fstp dword [ebp-0x4c/-0x48/-0x44]。
    // 观测点 vel.pos 在 accumulate（0x473780）尾部还要过一次 round001
    // （StageMath.hpp:58，floor(x*100)/100），所以用例必须落在 0.01 边界
    // 才能区分 f32 三段落点与 double 单次落点；旧的 [30,1,80,0] 在
    // 44.4444 处两条链都被 round001 抹成 44.44，断言的是不可观测量。
    // 现用例：op401 [12,1,-109.91,0]，start x=-74、elapsed=8 →
    //   w=f32((8/12)²)=0x3ee38e3a；delta=0xc20fa3d8(-35.910004)；
    //   prod=0xc17f5c2c → out=0xc2b3eb86(-89.96001)；
    //   round001(-8996.001)=floor→-8997 → 0xc2b3f0a4(-89.97)。
    //   double 单次落点 out=0xc2b3eb85(-89.96) → round001=-89.96
    //   (0xc2b3eb85)，与 f32 链差 0.01 ——S4 实体面 f80 serial 3 的
    //   0.01 翻转即该族（r23f 0xc2ad0aec vs 原生/f32 0xc2ad05cd；
    //   S4 全窗实体面 43+1142 → 0）。
    Rig r;
    r.enemy.vel.pos[0] = -74.0f;
    r.enemy.vel.pos[1] = 0.0f;
    r.enemy.vel.pos[2] = 0.0f;
    CHECK_EQ(r.op(401, {12, 1, float_to_bits(-109.91f), float_to_bits(0.0f)}), 0);
    for (int n = 0; n < 8; ++n) r.step();
    CHECK_EQ(float_to_bits(r.enemy.vel.pos[0]), 0xc2b3f0a4u);
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
