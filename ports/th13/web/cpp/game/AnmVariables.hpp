// ANM mask 变量表：id 与原版 VM 偏移一一对应（th13 实测，Ghidra 证据）：
//   读整数 0x4621b0 / 读浮点 0x461e80 / 写侧取址 0x462380(整) 0x4622c0(浮)。
//   10000-10003 i32 工作变量   vm+0x4AC/0x4B0/0x4B4/0x4B8
//   10004-10007 f32 工作变量   vm+0x4BC/0x4C0/0x4C4/0x4C8
//   10008/10009  附加 i32      vm+0x4CC/0x4D0
//   10010-10012  随机浮点（游戏 RNG 0x472e10/0x472de0 × 幅度/角度）
//   10013-10015  脚本位置（op400 直写，00462450.c case 0x190） vm+0x3C/0x40/0x44
//   10016-10021  场景全局（镜头原点+屏幕平移 / 参考点）——解释器经环境解析
//   10022        随机 u32（0x472c90 取模上界）
//   10023-10025  脚本旋转（op401 直写，case 0x191；create 的 anmArg 落 z 槽
//                vm+0x50，0046f820.c:50） vm+0x48/0x4C/0x50
//   10026        父链旋转 z（一层父，00461e80.c case 0x272a）——解释器解析
//   10027-10029  随机幅度/角度/上界 vm+0x4D4/0x4D8/0x4DC
//   10030-10032  与 10010-10012 同式（0x272e-0x2730）
// th15 另有的 10033-10035（vector）在 th13 两张读表均无 case，不设。
#pragma once
#include "Types.hpp"
#include "GameRandom.hpp"
#include "Timer.hpp"
namespace th13 {
struct AnmVariables {
    i32 integers[4]{};float floats[4]{};i32 extra_integers[2]{};
    float random_scale=1,random_angle=3.1415927410125732421875f;u32 random_bound=65536;
    Vec3 position{},rotation{};
    i32 integer(i32 value,GameRandom& rng)noexcept;
    float floating(float value,GameRandom& rng)noexcept;
    i32* integer_destination(i32* argument)noexcept;
    float* float_destination(float* argument)noexcept;
};
}
