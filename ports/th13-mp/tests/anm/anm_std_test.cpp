#include "anm_test_fixtures.hpp"
#include "game/StageResource.hpp"
#include "game/StageScript.hpp"

#include <cstdio>

using namespace th13;

namespace {

th13test::TestFileReader& reader() {
    static th13test::TestFileReader instance;
    return instance;
}

bool open_st01(StageResource& resource) {
    std::vector<th13::u8> bytes;
    if (!reader().read("artifacts/reference/raw/st01.std", bytes)) return false;
    return resource.open(bytes.data(), th13::u32(bytes.size()));
}

TEST(std_golden_header_and_objects) {
    StageResource resource;
    CHECK(open_st01(resource));
    CHECK(resource.error.empty());
    CHECK_EQ(resource.animation_name, std::string("st01wl.anm"));
    CHECK_EQ(resource.objects.size(), 3u);
    CHECK_EQ(resource.animation_count, 24u);
    CHECK_EQ(resource.objects[0].layer, 1);
    CHECK_EQ(resource.objects[0].flags, 0);
    CHECK_EQ(resource.objects[0].position.x, 136.0f);
    CHECK_EQ(resource.objects[0].position.y, 91.0f);
    CHECK_EQ(resource.objects[0].position.z, -180.0f);
    CHECK_EQ(resource.objects[0].size.x, 280.0f);
    CHECK_NEAR(resource.objects[0].size.y, 429.325f, 0.01f);
    CHECK_EQ(resource.objects[0].primitives.size(), 20u);
    const auto& first = resource.objects[0].primitives[0];
    CHECK_EQ(first.script, 22);
    CHECK_EQ(first.position.x, 8.0f);
    CHECK_EQ(first.position.y, 91.0f);
    CHECK_EQ(first.position.z, -168.0f);
    CHECK_EQ(first.size.x, 24.0f);
    CHECK_NEAR(first.size.y, 429.325f, 0.01f);
    CHECK_EQ(resource.objects[1].layer, 0);
    CHECK_EQ(resource.objects[1].position.x, -96.0f);
    CHECK_EQ(resource.objects[1].primitives.size(), 1u);
    CHECK_EQ(resource.objects[1].primitives[0].script, 16);
    CHECK_EQ(resource.objects[2].layer, 2);
    CHECK_EQ(resource.objects[2].primitives.size(), 3u);
    CHECK_EQ(resource.instances.size(), 40u);
    CHECK_EQ(resource.instances[0].object, 0);
    CHECK_NEAR(resource.instances[0].position.y, -320.0f, 0.01f);
    CHECK_EQ(resource.instances[1].object, 1);
    CHECK_EQ(resource.instances[3].object, 0);
}

TEST(std_golden_camera_script) {
    StageResource resource;
    CHECK(open_st01(resource));
    CHECK_EQ(resource.instructions.size(), 19u);
    const auto& instructions = resource.instructions;
    CHECK_EQ(instructions[0].time, 0);
    CHECK_EQ(instructions[0].opcode, 6);
    CHECK_EQ(instructions[1].opcode, 7);
    CHECK_NEAR(instructions[1].argument<float>(0), 0.62831855f, 1e-6f);
    CHECK_EQ(instructions[2].opcode, 8);
    CHECK_EQ(instructions[2].argument<u32>(0), 0xff000000u);
    CHECK_EQ(instructions[2].argument<float>(1), 300.0f);
    CHECK_EQ(instructions[2].argument<float>(2), 1000.0f);
    CHECK_EQ(instructions[5].opcode, 2);
    CHECK_EQ(instructions[5].argument<float>(0), -80.0f);
    CHECK_EQ(instructions[5].argument<float>(2), 900.0f);
    CHECK_EQ(instructions[7].time, 800);
    CHECK_EQ(instructions[7].opcode, 9);
    CHECK_EQ(instructions[7].argument<u32>(2), 0xff100010u);
    CHECK_EQ(instructions[10].time, 1568);
    CHECK_EQ(instructions[10].opcode, 1);
    CHECK_EQ(instructions[10].argument<u32>(0), 176u);
    CHECK_EQ(instructions[10].argument<i32>(1), 800);
    CHECK_EQ(instructions.back().time, -1);
    CHECK_EQ(instructions.back().opcode, -1);
    const auto* target = resource.instruction(176);
    CHECK(target != nullptr);
    CHECK_EQ(target->time, 800);
    CHECK_EQ(target->opcode, 2);
}

TEST(std_camera_script_runtime) {
    StageResource resource;
    CHECK(open_st01(resource));
    StageScript script(resource);
    CHECK(script.step());
    CHECK(script.error.empty());
    CHECK_EQ(script.state.camera.up.x, 0.0f);
    CHECK_EQ(script.state.camera.up.y, 1.0f);
    CHECK_NEAR(script.state.camera.fov, 0.62831855f, 1e-6f);
    CHECK_EQ(script.state.camera.fog.color, 0xff000000u);
    CHECK_EQ(script.state.camera.fog.near_distance, 300.0f);
    CHECK_EQ(script.state.camera.position.x, -80.0f);
    CHECK_NEAR(script.state.camera.position.z, 900.0f - 1100.0f / 800.0f, 0.01f);
    CHECK_EQ(script.state.camera.direction.z, -400.0f);
    CHECK_EQ(script.state.position.duration, 800);
    for (int frame = 0; frame < 799; frame++)
        CHECK(script.step());
    CHECK_EQ(script.state.timer.current, 800);
    CHECK_EQ(script.state.camera.position.y, 400.0f);
    CHECK_EQ(script.state.camera.position.z, -200.0f);
    CHECK_EQ(script.state.camera.fog.color, 0xff000000u);
    for (int frame = 0; frame < 60; frame++)
        CHECK(script.step());
    CHECK_EQ(script.state.camera.fog.color, 0xff100010u);
    CHECK_EQ(script.state.camera.fog.near_distance, 800.0f);
    CHECK_EQ(script.state.camera.fog.far_distance, 1600.0f);
    CHECK_EQ(script.state.position.duration, 768);
    for (int frame = 0; frame < 1568 - 800 - 60; frame++)
        CHECK(script.step());
    CHECK_EQ(script.state.timer.current, 1568);
    CHECK(script.step());
    CHECK_EQ(script.state.timer.current, 801);
    CHECK_EQ(script.state.camera.up.y, 1.0f);
    CHECK_EQ(script.state.camera.fog.color, 0xff100010u);
    CHECK_EQ(script.state.position.duration, 768);
    CHECK_NEAR(script.state.camera.position.y, 400.0f + 384.0f / 768.0f, 0.01f);
}

TEST(std_rejects_truncated_and_invalid) {
    StageResource resource;
    std::vector<th13::u8> bytes;
    CHECK(reader().read("artifacts/reference/raw/st01.std", bytes));
    StageResource cut;
    CHECK(!cut.open(bytes.data(), 0x40));
    CHECK(!cut.error.empty());
    StageResource broken;
    std::vector<th13::u8> mutated = bytes;
    mutated[4] = 0xff;
    mutated[5] = 0xff;
    mutated[6] = 0xff;
    mutated[7] = 0xff;
    CHECK(!broken.open(mutated.data(), th13::u32(mutated.size())));
}

}

int main() { return th13test::run_all("STD 舞台背景"); }
