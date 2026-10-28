#include "MsgPresentation.hpp"

#include <cstring>

namespace th13 {

// EXE .data 0x4bb4f0 行实读（games/th13/th13.exe 只读 dump；-1 = 原版表值
// 0xffffffff = 缺席）。Stage1 名牌 st01enm script15、右立绘 script10 与
// p58-dialogue 静态分析一致。
const MsgPresentation::PortraitRow MsgPresentation::kStageRows[7] = {
    /*stage1*/ {-1, -1, 3, 15, -1, -1, 3, 10},
    /*stage2*/ {-1, -1, 3, 16, -1, -1, 3, 11},
    /*stage3*/ {4, 16, 3, 16, 4, 11, 3, 11},
    /*stage4*/ {-1, -1, 3, 16, 4, 10, 3, 11},
    /*stage5*/ {-1, -1, 3, 16, -1, -1, 3, 12},
    /*stage6*/ {-1, -1, 3, 24, -1, -1, 3, 19},
    /*stage7*/ {4, 11, 3, 27, 4, 10, 3, 23},
};

namespace {
// ANM entry 链脚本计数（AnmResource.cpp parse_entries 同布局：u32 版本@0、
// u16 sprite 数@4、u16 脚本数@6、u32 stride@36，stride 0 为末 chunk）。
bool count_anm_scripts(const std::vector<u8>& bytes, u32& scripts) {
    u32 base = 0, total = 0;
    for (;;) {
        if (base + 64 > bytes.size()) return false;
        u32 version = 0, stride = 0;
        u16 ns = 0, nc = 0;
        std::memcpy(&version, bytes.data() + base, 4);
        std::memcpy(&ns, bytes.data() + base + 4, 2);
        std::memcpy(&nc, bytes.data() + base + 6, 2);
        std::memcpy(&stride, bytes.data() + base + 36, 4);
        if (version != 8) return false;
        total += nc;
        if (!stride) break;
        base += stride;
    }
    scripts = total;
    return true;
}
}  // namespace

bool MsgPresentation::bind_stage_scripts(i32 stage, const std::vector<std::string>& anim_list,
                                         const ReadFn& read) {
    if (stage < 1 || stage > 7) return fail("MsgPresentation stage row out of range");
    if (!read) return fail("MsgPresentation stage read channel unavailable");
    stage_ = stage;
    stage_script_base_.clear();
    stage_script_base_.reserve(anim_list.size());
    u32 base = 0;
    for (const auto& name : anim_list) {
        stage_script_base_.push_back(i32(base));
        std::vector<u8> bytes;
        if (!read(name, bytes)) return fail("MsgPresentation stage ANM missing: " + name);
        u32 scripts = 0;
        if (!count_anm_scripts(bytes, scripts))
            return fail("MsgPresentation stage ANM invalid: " + name);
        base += scripts;
    }
    return true;
}

i32 MsgPresentation::stage_script(i32 manager_bank, i32 script) {
    // ECL 文件槽 = 2 + 名单序（StageSim 装载核验同源注释）；port 合并表全局
    // 脚本号 = 名单序基号 + 文件内脚本号。
    const i32 index = manager_bank - 2;
    if (index < 0 || u32(index) >= stage_script_base_.size()) {
        fail("MsgPresentation stage bank out of ANIM list: " + std::to_string(manager_bank));
        return -1;
    }
    return stage_script_base_[u32(index)] + script;
}

bool MsgPresentation::create(bool player_bank, i32 manager_bank, i32 script, u32& handle) {
    // 玩家 bank 未合并（plXX.anm 单文件），脚本号原样；stage bank 经名单基号
    // 换算全局脚本号。
    const i32 global = player_bank ? script : stage_script(manager_bank, script);
    if (global < 0) return false;
    // 0x429285 原版序：位置清零 + flags 0x200（create 内建）+ 立即 tick。
    // layer 不传（-1）：立绘脚本经 ins_304 自带层（AnmInterpreter.cpp:121）。
    const u32 created =
        animations.create(player_bank ? banks.player : banks.stage, global, -1, 2, Vec3{});
    if (!created) return fail("MsgPresentation VM create: " + animations.error);
    handle = created;
    return true;
}

bool MsgPresentation::player_portrait(i32 character) {
    if (player_) return true;  // 已在（原版 +0x40 非零即不重建）
    return create(true, banks.player, kPlayerPortraitScripts[character & 3], player_);
}

bool MsgPresentation::side_portrait(i32 entry) {
    if (side_) return true;
    if (stage_ < 1 || stage_ > 7) return fail("MsgPresentation stage row not bound");
    const PortraitRow& row = kStageRows[stage_ - 1];
    // opcode 2（0x429335）：+0x64==-1 或 entry<2 → 中Boss 对（+0x6c/+0x70），
    // 否则 Boss 对（+0x64/+0x68）。
    const bool boss = row.boss_bank != -1 && entry >= 2;
    // create 第 2 参 = stage 表 bank（ECL 文件槽）；AnmManager 槽由 player_bank
    // 分支内建（false → banks.stage），无需重复传。
    return create(false, boss ? row.boss_bank : row.midboss_bank,
                  boss ? row.boss_script : row.midboss_script, side_);
}

bool MsgPresentation::boss_side_portrait() {
    if (side_) return true;
    if (stage_ < 1 || stage_ > 7) return fail("MsgPresentation stage row not bound");
    // opcode 31（0x42948c）：Boss 对无条件。
    const PortraitRow& row = kStageRows[stage_ - 1];
    if (row.boss_bank == -1) return fail("MsgPresentation boss portrait row absent");
    return create(false, row.boss_bank, row.boss_script, side_);
}

bool MsgPresentation::nameplate() {
    if (nameplate_) return true;
    if (stage_ < 1 || stage_ > 7) return fail("MsgPresentation stage row not bound");
    const PortraitRow& row = kStageRows[stage_ - 1];
    // opcode 20（0x429a94）：be824<5 恒真（难度 0..4），[stage+0x54]!=-1 取
    // A 对，否则 B 对。
    const bool use_a = row.nameplate_a_bank != -1;
    return create(false, use_a ? row.nameplate_a_bank : row.nameplate_b_bank,
                  use_a ? row.nameplate_a_script : row.nameplate_b_script, nameplate_);
}

bool MsgPresentation::interrupt_player(i32 label) {
    if (!player_) return true;  // 槽空：0x46fca0 对 0 句柄空转
    if (!animations.interrupt(player_, label, true)) return fail(animations.error);
    return true;
}

bool MsgPresentation::interrupt_side(i32 label) {
    if (!side_) return true;
    if (!animations.interrupt(side_, label, true)) return fail(animations.error);
    return true;
}

void MsgPresentation::release_player() {
    if (player_ && !animations.retire(player_)) fail(animations.error);
    player_ = 0;
}

void MsgPresentation::release_side() {
    if (side_ && !animations.retire(side_)) fail(animations.error);
    side_ = 0;
    // opcode 5（0x4295e9）：右立绘随名牌槽 +0x5c 一并释放。
    if (nameplate_ && !animations.retire(nameplate_)) fail(animations.error);
    nameplate_ = 0;
}

void MsgPresentation::release_nameplates() {
    if (nameplate_ && !animations.retire(nameplate_)) fail(animations.error);
    nameplate_ = 0;
}

void MsgPresentation::release_all() {
    release_player();
    release_side();
    release_nameplates();
}

}  // namespace th13
