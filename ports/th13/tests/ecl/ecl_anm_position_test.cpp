#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
#include "game/AnmVm.hpp"
using namespace th13;
namespace {
struct Args final : EclVariables {
    std::vector<i32> reads;
    bool integer(i32 id,i32& out) override {
        reads.push_back(id);
        out=id==-10000?3:id==-10001?320:9;
        return true;
    }
    bool floating(i32 id,float& out) override {
        reads.push_back(id);out=id==-10003?4.f:8.f;return true;
    }
    i32* integer_destination(i32) override {return nullptr;}
    float* float_destination(i32) override {return nullptr;}
};
}
TEST(ecl333_uses_fresh_source_translation_and_original_argument_order) {
    StageSim sim;StageEnemy enemy;AnmVm destination,source;EclContext ctx;Args args;
    sim.current=&enemy;sim.commands.sim=&sim;ctx.variables=&args;
    destination.visual.translation={1,2,9};source.visual.translation={80,40,3};
    i32 lookups=0;
    sim.enemy_animation=[&](StageEnemy&,i32 slot){CHECK_EQ(slot,3);return lookups++?&source:&destination;};
    std::vector<u8> code;
    th13test::emit(code,0,333,31,255,0,
        {u32(-10000),u32(-10001),u32(-10002),th13test::bits_of(-10003.f),th13test::bits_of(-10004.f)});
    ctx.instruction=reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(ctx,333),0);
    CHECK_EQ(lookups,2);
    const std::vector<i32> order={-10000,-10004,-10003,-10000,-10002,-10001};
    CHECK(args.reads==order);
    const auto& p=destination.interpolators.position;
    CHECK((p.start==std::array<float,3>{80,40,3}));
    CHECK((p.end==std::array<float,3>{4,8,0}));
    CHECK((p.control1==std::array<float,3>{0,0,0}));
    CHECK((p.control2==std::array<float,3>{0,0,0}));
    CHECK_EQ(p.mode,9);CHECK_EQ(p.duration,320);CHECK_EQ(p.timer.current,0);
}
TEST(ecl333_missing_destination_skips_all_other_argument_reads) {
    StageSim sim;StageEnemy enemy;EclContext ctx;Args args;
    sim.current=&enemy;sim.commands.sim=&sim;ctx.variables=&args;
    sim.enemy_animation=[](StageEnemy&,i32)->AnmVm*{return nullptr;};
    std::vector<u8> code;th13test::emit(code,0,333,31,255,0,
        {u32(-10000),u32(-10001),u32(-10002),th13test::bits_of(-10003.f),th13test::bits_of(-10004.f)});
    ctx.instruction=reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(ctx,333),0);
    CHECK((args.reads==std::vector<i32>{-10000}));
}
int main(){return th13test::run_all("ecl-animation-position");}
