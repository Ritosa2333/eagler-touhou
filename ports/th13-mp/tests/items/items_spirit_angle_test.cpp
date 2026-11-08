// 0x443f60 精度回归：dx/dy 各自 fsub 后 fstp dword 落 f32（0x443f6e-0x443f76 /
// 0x443f7a-0x443f83），再 fld 回 x87 调双精度 CRT atan2（0x443fbb call 0x4977aa），
// 结果一次 fstp dword（0x443fc0）。
//   · S3 f8187（serial 1052）证据例：wasm/musl 的 atan2f 在
//     atan2(33.80281066894531, 13.255630493164062) 取上邻 0x3f9939e4，
//     双精度路径落 0x3f9939e3，经 0.01 量化放大成 134.31→134.30 翻转。
//   · 宿主 glibc 的 atan2f 对该例恰好与双精度一致，故另设一对宿主可判别
//     的输入（glibc atan2f 与 double atan2 差 1 ULP），钉死重载选择。
#include "../harness/test_framework.hpp"
#include "game/ItemManager.hpp"
#include <cstring>
using namespace th13;

static u32 bits_of(float value) {
    u32 bits = 0;
    std::memcpy(&bits, &value, sizeof bits);
    return bits;
}

static float tracking_angle(float player_x, float player_y, float spirit_x,
                            float spirit_y) {
    SessionState session;
    session.rate = 1.f;
    ItemCollector collector;
    collector.progress = &session;
    collector.x = player_x;
    collector.y = player_y;
    collector.state = 1;
    GameRandom random{123};
    ItemManager manager{&random};
    manager.spawn_value_item(0, 0, 0, 0, 0.f);
    auto& spirit = manager.spirits()[0];
    spirit.state = 2;  // 追踪态：0x438730 case2 走 0x443f60
    spirit.x = spirit_x;
    spirit.y = spirit_y;
    spirit.speed = 1.f;
    spirit.move_speed = 1.f;
    manager.update_spirits(collector);
    return spirit.angle;
}

TEST(spirit_tracking_angle_matches_native_s3_f8187_bits) {
    // dx = 13.255630493164062、dy = 33.80281066894531（均为 f32）；
    // 双精度 atan2 = 1.1970790883735427 → 最近 f32 = 1.197079062461853
    // （0x3f9939e3，原生 0x443fc0 落点，S3 f8187 原生角度）。
    const float angle = tracking_angle(145.265625f, 260.6328125f,
                                       132.00999450683594f, 226.8300018310547f);
    CHECK_EQ(bits_of(angle), 0x3f9939e3u);
}

TEST(spirit_tracking_angle_selects_double_atan2_overload) {
    // dx = 0x1.1aff78p+4、dy = 0x1.7b861p+4：宿主 glibc
    // atan2f = 0x3f6e1961，双精度 atan2 落 f32 = 0x3f6e1960（差 1 ULP）。
    // 断言 0x3f6e1960 即证明调用的是 double 重载（与 0x4977aa 一致）。
    const float angle = tracking_angle(0x1.1aff78p+4f, 0x1.7b861p+4f, 0.f, 0.f);
    CHECK_EQ(bits_of(angle), 0x3f6e1960u);
}

int main() { return th13test::run_all("items-spirit-angle"); }
