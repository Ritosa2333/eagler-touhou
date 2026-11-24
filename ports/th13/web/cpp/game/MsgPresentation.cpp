#include "MsgPresentation.hpp"

#include <cstring>

namespace th13 {

const MsgPresentation::PortraitRow MsgPresentation::kStageRows[7] = {
    {-1, -1, 3, 15, -1, -1, 3, 10}, {-1, -1, 3, 16, -1, -1, 3, 11}, {4, 16, 3, 16, 4, 11, 3, 11},
    {-1, -1, 3, 16, 4, 10, 3, 11},  {-1, -1, 3, 16, -1, -1, 3, 12}, {-1, -1, 3, 24, -1, -1, 3, 19},
    {4, 11, 3, 27, 4, 10, 3, 23},
};

namespace {
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
}

bool MsgPresentation::bind_stage_scripts(i32 stage, const std::vector<std::string>& anim_list, const ReadFn& read) {
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
        if (!count_anm_scripts(bytes, scripts)) return fail("MsgPresentation stage ANM invalid: " + name);
        base += scripts;
    }
    return true;
}

i32 MsgPresentation::stage_script(i32 manager_bank, i32 script) {
    const i32 index = manager_bank - 2;
    if (index < 0 || u32(index) >= stage_script_base_.size()) {
        fail("MsgPresentation stage bank out of ANIM list: " + std::to_string(manager_bank));
        return -1;
    }
    return stage_script_base_[u32(index)] + script;
}

bool MsgPresentation::create(bool player_bank, i32 manager_bank, i32 script, u32& handle) {
    const i32 global = player_bank ? script : stage_script(manager_bank, script);
    if (global < 0) return false;
    const u32 created = animations.create(player_bank ? banks.player : banks.stage, global, -1, 0, Vec3{});
    if (!created) return fail("MsgPresentation VM create: " + animations.error);
    handle = created;
    return true;
}

bool MsgPresentation::player_portrait(i32 character) {
    if (player_) return true;
    return create(true, banks.player, kPlayerPortraitScripts[character & 3], player_);
}

bool MsgPresentation::side_portrait(i32 entry) {
    if (stage_ < 1 || stage_ > 7) return fail("MsgPresentation stage row not bound");
    const PortraitRow& row = kStageRows[stage_ - 1];
    if (side_) return true;
    const bool boss = row.boss_bank != -1 && entry >= 2;
    return create(false, boss ? row.boss_bank : row.midboss_bank, boss ? row.boss_script : row.midboss_script, side_);
}

bool MsgPresentation::boss_side_portrait() {
    if (stage_ < 1 || stage_ > 7) return fail("MsgPresentation stage row not bound");
    if (side_) return true;
    const PortraitRow& row = kStageRows[stage_ - 1];
    if (row.boss_bank == -1) return fail("MsgPresentation boss portrait row absent");
    return create(false, row.boss_bank, row.boss_script, side_);
}

bool MsgPresentation::nameplate(i32 chapter) {
    if (nameplate_) return true;
    if (stage_ < 1 || stage_ > 7) return fail("MsgPresentation stage row not bound");
    const PortraitRow& row = kStageRows[stage_ - 1];
    const bool use_a = chapter < 5 && row.nameplate_a_bank != -1;
    return create(false, use_a ? row.nameplate_a_bank : row.nameplate_b_bank,
                  use_a ? row.nameplate_a_script : row.nameplate_b_script, nameplate_);
}

bool MsgPresentation::interrupt_player(i32 label) {
    if (!player_) return true;
    if (!animations.interrupt(player_, label, true)) return fail(animations.error);
    return true;
}

bool MsgPresentation::interrupt_side(i32 label) {
    if (!side_) return true;
    if (!animations.interrupt(side_, label, true)) return fail(animations.error);
    return true;
}

bool MsgPresentation::fade_player(i32 label) {
    if (!player_) return true;
    if (!animations.interrupt(player_, label, false)) return fail(animations.error);
    return true;
}

bool MsgPresentation::fade_side(i32 label) {
    if (!side_) return true;
    if (!animations.interrupt(side_, label, false)) return fail(animations.error);
    return true;
}

bool MsgPresentation::set_visible(bool visible) {
    for (u32 handle : {player_, side_, nameplate_})
        if (handle && !animations.pause(handle, !visible)) return fail(animations.error);
    return true;
}

void MsgPresentation::release_player() {
    if (player_ && !animations.interrupt(player_, 1, false)) fail(animations.error);
    player_ = 0;
}

void MsgPresentation::release_side() {
    if (side_ && !animations.interrupt(side_, 1, false)) fail(animations.error);
    side_ = 0;
    if (nameplate_ && !animations.interrupt(nameplate_, 1, false)) fail(animations.error);
}

void MsgPresentation::release_nameplates() {
    if (nameplate_ && !animations.retire(nameplate_)) fail(animations.error);
    nameplate_ = 0;
}

void MsgPresentation::release_all() {
    if (player_ && !animations.retire(player_)) fail(animations.error);
    player_ = 0;
    if (side_ && !animations.retire(side_)) fail(animations.error);
    side_ = 0;
    if (nameplate_ && !animations.retire(nameplate_)) fail(animations.error);
    nameplate_ = 0;
}

}
