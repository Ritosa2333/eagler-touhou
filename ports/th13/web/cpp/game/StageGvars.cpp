// stage1 全局变量供给（0x420380 int 视图 / 0x420d00 float 视图的行为子集）。
// 语义依据与【待校准】清单见 docs/plans/event-contract.md；事件字段是 oracle 合同。
#include "StageSim.hpp"
#include "SessionState.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace th13 {

namespace {
const StageEnemy* primary_boss(const StageSim& sim) {
    if (sim.boss_slot[0] < 0) return nullptr;
    for (const auto& enemy : sim.enemies)
        if (!enemy->dead && enemy->script_id == sim.boss_slot[0]) return enemy.get();
    return nullptr;
}
}

// ---------------- 全局变量（0x420380 int 视图 / 0x420d00 float 视图） ----------------

bool StageGvars::integer(i32 id, i32& value) {
    StageSim& sim = *this->sim;
    if(id>=-9926&&id<=-9923){value=sim.shared_integers[id+9926];return true;}
    if(id>=-9922&&id<=-9915){value=truncate_int(sim.shared_floats[id+9922]);return true;}
    StageEnemy* e = sim.current;
    switch (id) {
    case -10000:  // RAND（0x472c90 & 0x7fffffff）
        value = i32(sim.random.next() & 0x7fffffffu);
        return true;
    case -9999:  // RANDF 的 int 视图：ftol 截断（消耗一次抽取）
        value = i32(sim.random.unit());
        return true;
    case -9988:  // TIME int 视图（+0x14A8）
        value = i32(e ? e->age : 0);
        return true;
    case -9987:  // RANDF2 int 视图：0x472e10 → ftol
        value = i32(sim.random.signed_unit());
        return true;
    case -9986:  // 00420380: enemy+0x521c bit25, set by timeout interruption
        value = e && e->phase_timed_out ? 1 : 0;
        return true;
    case -9998:  // int getter 无对应 case，走 default 0（不消耗 RNG）
        value = 0;
        return true;
    case -9985:
    case -9984:
    case -9983:
    case -9982:
        value = e ? e->i_slots[id + 9985] : 0;
        return true;
    case -9981:
    case -9980:
    case -9979:
    case -9978:  // F0-F3 int 视图（ftol）
        value = e ? i32(e->f_slots[id + 9981]) : 0;
        return true;
    case -9935: case -9934: case -9933: case -9932:  // F4-F7, 00420380.
        value = e ? truncate_int(e->f_slots[id + 9939]) : 0;
        return true;
    case -9965:
        value = i32(sim.player_x);
        return true;
    case -9964:
        value = i32(sim.player_y);
        return true;
    case -9944: {  // 00420669: player distance, rounded to f32 before ftol.
        float distance = 0.0f;
        floating(id, distance);
        value = truncate_int(distance);
        return true;
    }
    case -9943: case -9942: case -9941: case -9940: {
        const auto* boss = primary_boss(sim);
        value = boss ? boss->i_slots[id + 9943] : 0;
        return true;
    }
    case -9939: case -9938: case -9937: case -9936: {
        const auto* boss = primary_boss(sim);
        value = boss ? i32(boss->f_slots[id + 9939]) : 0;
        return true;
    }
    case -9963: {
        StageEnemy* boss = sim.boss();
        value = boss ? i32(boss->position[0]) : 0;
        return true;
    }
    case -9962: {
        StageEnemy* boss = sim.boss();
        value = boss ? i32(boss->position[1]) : 0;
        return true;
    }
    case -9960:  // RANK
        value = sim.rank;
        return true;
    case -9959:  // DIFF
        value = sim.diff;
        return true;
    case -9958:  // 420380: atan2(inner+0x7c, inner+0x78), then ftol.
        value = e ? i32(std::atan2(double(e->motion_delta[1]), double(e->motion_delta[0]))) : 0;
        return true;
    case -9954:  // LIFE (+0x5138 in the enemy ECL manager)
        value = e ? i32(e->life) : 0;
        return true;
    case -9953:
    case -9952:
    case -9951:
    case -9950:  // EASY/NORMAL/HARD/LUNATIC flags, distinct from RANK
        value = sim.diff == id + 9953;
        return true;
    case -9957:
        value = 1;
        return true;
    case -9946:
        // ENM_CNT_REAL（mgr+0xB8）：0x420638 直读计数器，计数在链表摘除时
        // 才减——遍历中途读取时，已标 dead 但未被本趟游标到达的敌机仍在列。
        // 移植侧 vector 摘除同拍（step_frame 游标到达时擦除），故计数 =
        // 当前 vector 大小（含 root；勿用 !dead 过滤——会把已标未摘者漏计，
        // 回放实测 frame 3252 由此少 1 导致 Phantom01 生成门 [-9946]<11 误开）。
        value = i32(sim.enemies.size());
        return true;
    case -9909:  // 0x420380 case 0xffffd94b：+0x11E8（trace_id，0x4183f0:61 的
        // param2[0x14]）。oracle 实测 Phantom01（MBoss1_at 子线程创建）求值
        // 得 75 = MBoss id（433 的 moveEnm 目标）。
        value = e ? e->trace_id : 0;
        return true;
    case -9908:  // 00417e90: count eligible nodes still linked to the manager.
        value = 0;
        for (const auto& enemy : sim.enemies)
            if ((enemy->flags & 0x31u) == 0 && enemy->invulnerable < 1) ++value;
        return true;
    case -9907:  // SPELL_ID
        value = sim.session ? sim.session->selected_spell_id : -1;
        return true;
    case -9914:  // ID（+0x11E4）
        value = e ? e->script_id : 0;
        return true;
    case -9931:  // 420380: enemy manager+0xc0, last constructor-assigned ID.
        value = sim.last_created_enemy_id;
        return true;
    default:
        for (const auto& pair : writable_ints_)
            if (pair.first == id) {
                value = pair.second;
                return true;
            }
        value = 0;
        return true;
    }
}

bool StageGvars::floating(i32 id, float& value) {
    StageSim& sim = *this->sim;
    if (id == -9931) { value = float(sim.last_created_enemy_id); return true; }
    if(id>=-9926&&id<=-9923){value=float(sim.shared_integers[id+9926]);return true;}
    if(id>=-9922&&id<=-9915){value=sim.shared_floats[id+9922];return true;}
    StageEnemy* e = sim.current;
    switch (id) {
    case -10000:
        value = float(sim.random.next() & 0x7fffffffu);
        return true;
    case -9999:  // RANDF（0x472de0）
        value = sim.random.unit();
        return true;
    case -9998:  // RANDRAD（0x472e10 × π）
        value = float(sim.random.signed_unit() * 3.1415927410125732);
        return true;
    case -9997:  // FINAL_X/Y（主位置 +0x1230）
    case -9996:
        value = e ? e->position[id == -9997 ? 0 : 1] : 0.0f;
        return true;
    case -9995:  // ABS_X/Y（+0x1274，ctor 写出生点后未见解更新——窗口内无人读）
    case -9994:
        value = e ? e->vel.pos[id == -9995 ? 0 : 1] : 0.0f;
        return true;
    case -9993:  // REL_X/Y（+0x12B8，恒 0）
    case -9992:
        value = 0.0f;
        return true;
    case -9965:
    case -9991:
        value = sim.player_x;
        return true;
    case -9964:
    case -9990:
        value = sim.player_y;
        return true;
    case -9989: {  // ANGLE_PLAYER（0x443f60：自机位置指向玩家）
        const float dy = sim.player_y - (e ? e->position[1] : 0.0f);
        const float dx = sim.player_x - (e ? e->position[0] : 0.0f);
        if (dy == 0.0f && dx == 0.0f) {
            value = 1.5707964f;
        } else {
            value = float(std::atan2(static_cast<double>(dy), static_cast<double>(dx)));
        }
        return true;
    }
    case -9988:
        value = e ? e->age : 0.0f;
        return true;
    case -9987:
        value = sim.random.signed_unit();
        return true;
    case -9986:
        value = e && e->phase_timed_out ? 1.0f : 0.0f;
        return true;
    case -9985:
    case -9984:
    case -9983:
    case -9982:
        value = float(e ? e->i_slots[id + 9985] : 0);
        return true;
    case -9981:
    case -9980:
    case -9979:
    case -9978:
        value = e ? e->f_slots[id + 9981] : 0.0f;
        return true;
    case -9935: case -9934: case -9933: case -9932:  // +0x1494..0x14a0.
        value = e ? e->f_slots[id + 9939] : 0.0f;
        return true;
    case -9963: {
        StageEnemy* boss = sim.boss();
        value = boss ? boss->position[0] : 0.0f;
        return true;
    }
    case -9962: {
        StageEnemy* boss = sim.boss();
        value = boss ? boss->position[1] : 128.0f;
        return true;
    }
    case -9960:
        value = float(sim.rank);
        return true;
    case -9958:  // 421107 reads the pre-quantization movement delta, not velocity.
        value = e ? float(std::atan2(double(e->motion_delta[1]), double(e->motion_delta[0]))) : 0.f;
        return true;
    case -9944: {  // 00421367 -> 00423e20: current enemy to player distance.
        const double dx = double(e ? e->position[0] : 0.0f) - double(sim.player_x);
        const double dy = double(e ? e->position[1] : 0.0f) - double(sim.player_y);
        // x87 keeps the differences/products wide, then stores the squared
        // distance as f32 before sqrt; the returned distance is also f32.
        const float squared = float(dx * dx + dy * dy);
        value = float(std::sqrt(double(squared)));
        return true;
    }
    case -9943: case -9942: case -9941: case -9940: {
        const auto* boss = primary_boss(sim);
        value = boss ? float(boss->i_slots[id + 9943]) : 0.f;
        return true;
    }
    case -9939: case -9938: case -9937: case -9936: {
        const auto* boss = primary_boss(sim);
        value = boss ? boss->f_slots[id + 9939] : 0.f;
        return true;
    }
    case -9959:
        value = float(sim.diff);
        return true;
    case -9954:
        value = e ? e->life : 0.0f;
        return true;
    case -9953:
    case -9952:
    case -9951:
    case -9950:
        value = float(sim.diff == id + 9953);
        return true;
    case -9957:
        value = 1.0f;
        return true;
    case -9908: { // 00420d00 calls the same 00417e90 as the integer getter.
        i32 count = 0;
        integer(id, count);
        value = float(count);
        return true;
    }
    case -9907:  // 00420d00: the float view of DAT_004be7d4, including -1.
        value = float(sim.session ? sim.session->selected_spell_id : -1);
        return true;
    default:
        value = 0.0f;
        return true;
    }
}

i32* StageGvars::integer_destination(i32 id) {
    if(id>=-9926&&id<=-9923)return &sim->shared_integers[id+9926];
    StageEnemy* e = sim->current;
    if (e && id >= -9985 && id <= -9982) return &e->i_slots[id + 9985];
    if (id == -9914 && e) return &e->script_id;
    for (auto& pair : writable_ints_)
        if (pair.first == id) return &pair.second;
    if (id == -9949 || id == -9948 || id == -9947) {  // MISS/BOMB/CAPTURE 计数（MBoss 族写入）
        writable_ints_.emplace_back(id, 0);
        return &writable_ints_.back().second;
    }
    return nullptr;
}

float* StageGvars::float_destination(i32 id) {
    if(id>=-9922&&id<=-9915)return &sim->shared_floats[id+9922];
    StageEnemy* e = sim->current;
    if (e && id >= -9981 && id <= -9978) return &e->f_slots[id + 9981];
    if (e && id >= -9935 && id <= -9932) return &e->f_slots[id + 9939];
    return nullptr;
}

}  // namespace th13
