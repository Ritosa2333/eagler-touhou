// 妖梦 Bomb 斩击池——战斗侧专用模块（非通用框架）。
// 原版证据（th13.exe v1.00c，Ghidra 函数体 + .data dump + objdump 交叉验证，
// 详见 artifacts/verification/r4/bomb/youmu-bomb-evidence.md）：
//   BombYoumuInf vtable @ 0x4aa610：slot0 start=0x40c910（cursor 复位、主 ANM
//   script 25、屏震任务），slot1 update=0x40cb50（本模块语义来源）。
//   0x40cb50 每帧（age<0x105=261）：生成支路恰好一条斩击（ANM script 19 偶/
//   20 奇，位置 x=0、y=lane 表 0x4bba54[cursor%11] + signed_unit×48），随后
//   60 槽循环（ctx+0x610 anm id 区，槽 0..59）：VM 死亡则清 id（scale 冻结）；
//   存活则先注册旋转矩形（0x4457e0：全长 96 × scale×128，伤害 30，寿命 0=
//   当帧一次性，flag|4），再用同矩形消弹（0x4119c0，弹还需在场内
//   384×448@中心(0,224)），最后槽定时器 tick（scale += 1.0f，0x4016c0 布局）。
//   cursor 无回绕：>=60 的生成写越界到久死槽字段，无战斗效果（视觉 ANM 仍在）
//   ——战斗矩形只来自前 60 条斩击（帧 0..59 生成，每条活 16 帧后 VM 自灭）。
// 随机归属：生成支路每帧抽 1 次 0x472e10（signed [-1,1)）作 lane 抖动（本模块
// 经注入的 GameRandom 抽取，不自种）；斩击角度随机在 pl03 VM script 19/20 的
// ins_119（var10030=signed_unit×π → fmod 8 → −π/2 → ins_401 写 rotation.z）由
// ANM 侧抽取，本模块每槽接收真实 VM 的 rotation.z 回填。
#pragma once

#include "Rng.hpp"

#include <array>
#include <cstdint>
#include <optional>

namespace th13 {

class YoumuBomb {
public:
    static constexpr int kSlots = 60;                 // ctx+0x610 anm id 槽数
    static constexpr int kActiveFrames = 261;         // 0x40cb50: age < 0x105
    static constexpr float kRectLength = 96.0f;       // 0x4457e0 param_2（全长）
    static constexpr float kRectGrowUnit = 128.0f;    // 高度 = scale×128（全长）
    static constexpr float kDamagePerRect = 30.0f;    // 0x4457e0 param_6
    static constexpr float kCenterOffset = 256.0f;    // 0x4aebe8
    static constexpr float kLaneJitter = 48.0f;       // 0x472e10 ×48（有符号）
    // 0x4bba54（.data dump）：11 条 Y lane，cursor%11 取用
    static constexpr float kLanes[11] = {224.0f, 192.0f, 320.0f, 224.0f, 64.0f,
                                         80.0f,  384.0f, 320.0f, 96.0f, 128.0f, 224.0f};

    struct Spawn {
        int slot;        // -1 = cursor 已过 60 槽（原版越界写，无战斗效果，仅视觉）
        int script;      // 19（cursor 偶）/ 20（cursor 奇）
        float y;         // lane + signed_unit×48（float32 存储）
    };

    struct Rect {
        float cx, cy;    // 中心
        float w, h;      // 全长：96 × scale×128
        float angle;     // VM rotation.z（弧度）
    };

    // bomb 开始（0x40c910 的 cursor 复位 + 槽清零语义）。激活期间每帧：
    //   step() → 逐槽 set_vm()（Combat 解析真实 VM）→ 命中/消弹查询。
    void reset();

    // 每帧一步。age = bomb_age（0..260 生成期；>=261 无生成、不抽随机）。
    // rng = 游戏共享 GameRandom：生成帧恰抽一次 signed_unit（lane 抖动）。
    std::optional<Spawn> step(int age, GameRandom& rng);

    // 每帧对每槽回填真实 pl03 VM 状态（step 之后、命中查询之前）。
    // alive=false：清槽（anm id 置 0；scale/counter 冻结——原版死槽不 tick）。
    // alive=true：以当前 scale 生成本帧矩形参数，随后 tick（scale += 1.0f）。
    void set_vm(int slot, bool alive, float rotation_z);

    // —— 只读视图（供 Combat 接线/对拍） ——
    bool active(int slot) const;
    bool has_anm(int slot) const;
    float scale(int slot) const;            // 槽定时器 float（tick 后）
    float rect_scale(int slot) const;       // 本帧矩形用的 tick 前值
    float lane_y(int slot) const;
    float angle(int slot) const;
    int script(int slot) const;
    int cursor() const;

    // 槽矩形（0x40cb50 注册参数；中心 = lane 点沿 angle+π/2 垂线偏移 ±256，偶−/奇+）。
    Rect rect(int slot) const;

    // 0x474340 旋转矩形 vs 点(半径 r)：局部坐标 + 中段/四角圆测试，原样分支。
    static bool rect_covers(const Rect& rc, float px, float py, float r);

    // 覆盖该点的活跃斩数：伤害 = kDamagePerRect × count（0x4457e0 当帧一次性，
    // 每矩形每帧至多计一次；多斩重叠按斩数叠加——原版逐槽独立注册）。
    int cover_count(float px, float py, float r) const;

    // 消弹判定（0x4119c0：斩矩形命中 && 弹在场内 384×448@中心(0,224)，含弹半径）。
    bool clears_bullet(float px, float py, float r) const;

private:
    struct Slot {
        bool has_anm = false;   // anm id != 0
        bool alive = false;     // 本帧 VM 存活且已回填
        bool odd = false;       // 符号位：奇 +192 / 偶 −192（0x4aea58/0x4aea70，只取符号）
        int script = 0;
        int counter = 0;        // 槽定时器 max（int 镜像，非战斗量）
        float y = 0.0f;
        float angle = 0.0f;
        float scale = 0.0f;       // 槽定时器 float（tick 后）
        float rect_scale = 0.0f;  // 本帧矩形 tick 前值
    };
    std::array<Slot, kSlots> slots_{};
    int cursor_ = 0;
};

}  // namespace th13
