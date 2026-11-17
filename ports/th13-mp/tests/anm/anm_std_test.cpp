// STD 舞台背景单测：
// 1) golden：真实 st01.std 逐字段断言（头 3/24/0x39c/0x62c、"st01wl.anm"、
//    对象层/位置/尺寸、图元脚本位、实例数、脚本指令数）；
// 2) 相机脚本闭环：t=0 起步（上向量/视场角/黑雾 300-1000/位置(-80,0,900)）、
//    t=800 雾插值与位置插值推进、t=1568 ins_1(176,800) 跳转回 800 的循环；
// 3) 非法输入拒绝（截断头/越界偏移）。
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
    CHECK_EQ(resource.animation_count, 24u);  // 图元数 = 头部第二字段
    // 对象 0：层 1 / 位置 (136,91,-180) / 尺寸 (280,429.325,360)（独立 Python
    // 逐字节复核：id=0/layer=1/flags=0）。
    CHECK_EQ(resource.objects[0].layer, 1);
    CHECK_EQ(resource.objects[0].flags, 0);
    CHECK_EQ(resource.objects[0].position.x, 136.0f);
    CHECK_EQ(resource.objects[0].position.y, 91.0f);
    CHECK_EQ(resource.objects[0].position.z, -180.0f);
    CHECK_EQ(resource.objects[0].size.x, 280.0f);
    CHECK_NEAR(resource.objects[0].size.y, 429.325f, 0.01f);
    CHECK_EQ(resource.objects[0].primitives.size(), 20u);
    // 首图元：脚本 22、位置 (8,91,-168)、宽 24、高 429.325。
    const auto& first = resource.objects[0].primitives[0];
    CHECK_EQ(first.script, 22);
    CHECK_EQ(first.position.x, 8.0f);
    CHECK_EQ(first.position.y, 91.0f);
    CHECK_EQ(first.position.z, -168.0f);
    CHECK_EQ(first.size.x, 24.0f);
    CHECK_NEAR(first.size.y, 429.325f, 0.01f);
    // 对象 1（层 0、900 宽大墙）与对象 2（层 2、3 图元）。
    CHECK_EQ(resource.objects[1].layer, 0);
    CHECK_EQ(resource.objects[1].position.x, -96.0f);
    CHECK_EQ(resource.objects[1].primitives.size(), 1u);
    CHECK_EQ(resource.objects[1].primitives[0].script, 16);
    CHECK_EQ(resource.objects[2].layer, 2);
    CHECK_EQ(resource.objects[2].primitives.size(), 3u);
    // 实例（FACE）：40 条，对象号循环 0,1,2（独立字节复核）。
    CHECK_EQ(resource.instances.size(), 40u);
    CHECK_EQ(resource.instances[0].object, 0);
    CHECK_NEAR(resource.instances[0].position.y, -320.0f, 0.01f);
    CHECK_EQ(resource.instances[1].object, 1);
    CHECK_EQ(resource.instances[3].object, 0);
}

TEST(std_golden_camera_script) {
    StageResource resource;
    CHECK(open_st01(resource));
    // 指令序列：18 条 + 终止符（0x62c 起逐条走查）。
    CHECK_EQ(resource.instructions.size(), 19u);
    const auto& instructions = resource.instructions;
    CHECK_EQ(instructions[0].time, 0);
    CHECK_EQ(instructions[0].opcode, 6);  // up=(0,1,0)
    CHECK_EQ(instructions[1].opcode, 7);  // fov=0.62831855
    CHECK_NEAR(instructions[1].argument<float>(0), 0.62831855f, 1e-6f);
    CHECK_EQ(instructions[2].opcode, 8);  // fog #000000 300 1000
    CHECK_EQ(instructions[2].argument<u32>(0), 0xff000000u);
    CHECK_EQ(instructions[2].argument<float>(1), 300.0f);
    CHECK_EQ(instructions[2].argument<float>(2), 1000.0f);
    CHECK_EQ(instructions[5].opcode, 2);  // pos=(-80,0,900)
    CHECK_EQ(instructions[5].argument<float>(0), -80.0f);
    CHECK_EQ(instructions[5].argument<float>(2), 900.0f);
    CHECK_EQ(instructions[7].time, 800);
    CHECK_EQ(instructions[7].opcode, 9);  // 雾插值 → (#100010, 800, 1600)
    CHECK_EQ(instructions[7].argument<u32>(2), 0xff100010u);
    CHECK_EQ(instructions[10].time, 1568);
    CHECK_EQ(instructions[10].opcode, 1);  // 跳转 offset=176/time=800
    CHECK_EQ(instructions[10].argument<u32>(0), 176u);
    CHECK_EQ(instructions[10].argument<i32>(1), 800);
    // 终止符。
    CHECK_EQ(instructions.back().time, -1);
    CHECK_EQ(instructions.back().opcode, -1);
    // 跳转目标 = 脚本区 +176 的指令（t=800 的 ins_2）。
    const auto* target = resource.instruction(176);
    CHECK(target != nullptr);
    CHECK_EQ(target->time, 800);
    CHECK_EQ(target->opcode, 2);
}

TEST(std_camera_script_runtime) {
    StageResource resource;
    CHECK(open_st01(resource));
    StageScript script(resource);
    CHECK(script.step());  // t=0：全部 time<=0 指令即刻生效
    CHECK(script.error.empty());
    CHECK_EQ(script.state.camera.up.x, 0.0f);
    CHECK_EQ(script.state.camera.up.y, 1.0f);
    CHECK_NEAR(script.state.camera.fov, 0.62831855f, 1e-6f);
    CHECK_EQ(script.state.camera.fog.color, 0xff000000u);
    CHECK_EQ(script.state.camera.fog.near_distance, 300.0f);
    CHECK_EQ(script.state.camera.position.x, -80.0f);
    CHECK_NEAR(script.state.camera.position.z, 900.0f - 1100.0f / 800.0f, 0.01f);
    CHECK_EQ(script.state.camera.direction.z, -400.0f);
    CHECK_EQ(script.state.position.duration, 800);  // (-80,0,900)→(-80,400,-200)
    // 推进到 t=800：首段插值恰完成（位置到端点），t=800 块尚未执行。
    for (int frame = 0; frame < 799; frame++) CHECK(script.step());
    CHECK_EQ(script.state.timer.current, 800);
    CHECK_EQ(script.state.camera.position.y, 400.0f);
    CHECK_EQ(script.state.camera.position.z, -200.0f);
    CHECK_EQ(script.state.camera.fog.color, 0xff000000u);  // 雾插值未开始
    // t=800 块：雾 60 帧插值 → (#100010, 800, 1600)；位置插值重启（768 帧）。
    for (int frame = 0; frame < 60; frame++) CHECK(script.step());
    CHECK_EQ(script.state.camera.fog.color, 0xff100010u);
    CHECK_EQ(script.state.camera.fog.near_distance, 800.0f);
    CHECK_EQ(script.state.camera.fog.far_distance, 1600.0f);
    CHECK_EQ(script.state.position.duration, 768);
    // 推进到 t=1568：ins_1(176,800) 为该时刻首指令——跳转先于同组 cut 指令
    // 生效（t=1568 的换景组为不可达数据）：时钟回 800，位置插值重启。
    for (int frame = 0; frame < 1568 - 800 - 60; frame++) CHECK(script.step());
    CHECK_EQ(script.state.timer.current, 1568);
    CHECK(script.step());  // 触发跳转
    CHECK_EQ(script.state.timer.current, 801);  // set(800) 后 tick
    CHECK_EQ(script.state.camera.up.y, 1.0f);   // cut 未执行（上向量未变）
    CHECK_EQ(script.state.camera.fog.color, 0xff100010u);
    CHECK_EQ(script.state.position.duration, 768);  // 跳转后 ins_2/ins_3 重挂
    CHECK_NEAR(script.state.camera.position.y, 400.0f + 384.0f / 768.0f, 0.01f);
}

TEST(std_rejects_truncated_and_invalid) {
    StageResource resource;
    std::vector<th13::u8> bytes;
    CHECK(reader().read("artifacts/reference/raw/st01.std", bytes));
    StageResource cut;
    CHECK(!cut.open(bytes.data(), 0x40));  // 截断头
    CHECK(!cut.error.empty());
    // 对象目录越界：instance_offset < 目录末端。
    StageResource broken;
    std::vector<th13::u8> mutated = bytes;
    mutated[4] = 0xff;
    mutated[5] = 0xff;
    mutated[6] = 0xff;
    mutated[7] = 0xff;
    CHECK(!broken.open(mutated.data(), th13::u32(mutated.size())));
}

}  // namespace

int main() { return th13test::run_all("STD 舞台背景"); }
