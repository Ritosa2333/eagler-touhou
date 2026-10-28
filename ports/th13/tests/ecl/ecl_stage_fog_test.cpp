#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
#include "game/StageScript.hpp"

using namespace th13;
namespace {
struct Variables final : EclVariables {
    std::vector<i32> reads;
    bool integer(i32 id, i32& value) override {
        reads.push_back(id);
        value = id == -1000 ? 60 : id == -1001 ? 0 : i32(0xff500050u);
        return true;
    }
    bool floating(i32 id, float& value) override {
        reads.push_back(id);
        value = id == -1003 ? 600.f : 1800.f;
        return true;
    }
    i32* integer_destination(i32) override { return nullptr; }
    float* float_destination(i32) override { return nullptr; }
};
TEST(stage_fog_resolves_native_argument_order_and_reaches_camera_interpolator) {
    StageSim sim;
    StageEnemy enemy;
    sim.commands.sim = &sim;
    sim.current = &enemy;
    StageResource resource;
    StageInstruction stop;
    stop.opcode = 0;
    stop.length = 8;
    resource.instructions.push_back(stop);
    StageScript script(resource);
    script.state.camera.fog.set(0xff100010,800,1600);
    bool called = false;
    sim.stage_fog = [&](i32 frames,i32 mode,u32 color,float near_distance,float far_distance) {
        called = true;
        CHECK_EQ(frames,60);
        CHECK_EQ(mode,0);
        CHECK_EQ(color,0xff500050u);
        CHECK_EQ(near_distance,600.f);
        CHECK_EQ(far_distance,1800.f);
        auto& s = script.state;
        s.fog.start = s.camera.fog;
        s.fog.end.set(color,near_distance,far_distance);
        s.fog.duration = frames;
        s.fog.mode = mode;
        s.fog.timer.set(0);
    };
    std::vector<u8> code;
    th13test::emit(code,0,557,31,255,0,
        {u32(-1000),u32(-1001),u32(-1002),float_to_bits(-1003.f),float_to_bits(-1004.f)});
    Variables vars;
    EclContext context;
    context.variables = &vars;
    context.instruction = reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(context,557),0);
    CHECK(called);
    // 0041f40b resolves duration,mode,color,far,near before00405f00.
    CHECK((vars.reads == std::vector<i32>{-1000,-1001,-1002,-1004,-1003}));
    for (i32 i=0;i<30;++i) CHECK(script.step(1));
    CHECK_EQ(script.state.camera.fog.color,0xff300030u);
    CHECK_EQ(script.state.camera.fog.near_distance,700.f);
    CHECK_EQ(script.state.camera.fog.far_distance,1700.f);
    for (i32 i=0;i<30;++i) CHECK(script.step(1));
    CHECK_EQ(script.state.camera.fog.color,0xff500050u);
    CHECK_EQ(script.state.camera.fog.near_distance,600.f);
    CHECK_EQ(script.state.camera.fog.far_distance,1800.f);
    CHECK_EQ(script.state.fog.duration,0);
}
TEST(stage_fog_headless_still_resolves_operands_without_presentation_host) {
    StageSim sim;
    StageEnemy enemy;
    sim.commands.sim = &sim;
    sim.current = &enemy;
    std::vector<u8> code;
    th13test::emit(code,0,557,31,255,0,
        {u32(-1000),u32(-1001),u32(-1002),float_to_bits(-1003.f),float_to_bits(-1004.f)});
    Variables vars;
    EclContext context;
    context.variables=&vars;
    context.instruction=reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(context,557),0);
    CHECK((vars.reads == std::vector<i32>{-1000,-1001,-1002,-1004,-1003}));
}
}
int main() { return th13test::run_all("ecl-stage-fog"); }
