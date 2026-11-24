#include "SessionState.hpp"
#include "StageSim.hpp"

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

bool StageGvars::integer(i32 id, i32& value) {
    StageSim& sim = *this->sim;
    if (id >= -9926 && id <= -9923) {
        value = sim.shared_integers[id + 9926];
        return true;
    }
    if (id >= -9922 && id <= -9915) {
        value = truncate_int(sim.shared_floats[id + 9922]);
        return true;
    }
    StageEnemy* e = sim.current;
    switch (id) {
    case -10000:
        GameRandom::tag(0);
        value = i32(sim.random.next() & 0x7fffffffu);
        return true;
    case -9999:
        GameRandom::tag(1);
        value = i32(sim.random.unit());
        return true;
    case -9988:
        value = i32(e ? e->age : 0);
        return true;
    case -9987:
        GameRandom::tag(2);
        value = i32(sim.random.signed_unit());
        return true;
    case -9986:
        value = e && e->phase_timed_out ? 1 : 0;
        return true;
    case -9998:
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
    case -9978:
        value = e ? i32(e->f_slots[id + 9981]) : 0;
        return true;
    case -9935:
    case -9934:
    case -9933:
    case -9932:
        value = e ? truncate_int(e->f_slots[id + 9939]) : 0;
        return true;
    case -9965:
        value = i32(sim.player_x);
        return true;
    case -9964:
        value = i32(sim.player_y);
        return true;
    case -9944: {
        float distance = 0.0f;
        floating(id, distance);
        value = truncate_int(distance);
        return true;
    }
    case -9943:
    case -9942:
    case -9941:
    case -9940: {
        const auto* boss = primary_boss(sim);
        value = boss ? boss->i_slots[id + 9943] : 0;
        return true;
    }
    case -9939:
    case -9938:
    case -9937:
    case -9936: {
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
    case -9960:
        value = sim.rank;
        return true;
    case -9959:
        value = sim.diff;
        return true;
    case -9958:
        value = e ? i32(std::atan2(double(e->motion_delta[1]), double(e->motion_delta[0]))) : 0;
        return true;
    case -9954:
        value = e ? i32(e->life) : 0;
        return true;
    case -9953:
    case -9952:
    case -9951:
    case -9950:
        value = sim.diff == id + 9953;
        return true;
    case -9957:
        value = 1;
        return true;
    case -9946:
        value = i32(sim.enemies.size());
        return true;
    case -9909:
        value = e ? e->trace_id : 0;
        return true;
    case -9908:
        value = 0;
        for (const auto& enemy : sim.enemies)
            if ((enemy->flags & 0x31u) == 0 && enemy->invulnerable < 1) ++value;
        return true;
    case -9907:
        value = sim.session ? sim.session->selected_spell_id : -1;
        return true;
    case -9914:
        value = e ? e->script_id : 0;
        return true;
    case -9931:
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
    if (id == -9931) {
        value = float(sim.last_created_enemy_id);
        return true;
    }
    if (id >= -9926 && id <= -9923) {
        value = float(sim.shared_integers[id + 9926]);
        return true;
    }
    if (id >= -9922 && id <= -9915) {
        value = sim.shared_floats[id + 9922];
        return true;
    }
    StageEnemy* e = sim.current;
    switch (id) {
    case -10000:
        GameRandom::tag(3);
        value = float(sim.random.next() & 0x7fffffffu);
        return true;
    case -9999:
        GameRandom::tag(4);
        value = sim.random.unit();
        return true;
    case -9998:
        GameRandom::tag(5);
        value = float(sim.random.signed_unit() * 3.1415927410125732);
        return true;
    case -9997:
    case -9996:
        value = e ? e->position[id == -9997 ? 0 : 1] : 0.0f;
        return true;
    case -9995:
    case -9994:
        value = e ? e->vel.pos[id == -9995 ? 0 : 1] : 0.0f;
        return true;
    case -9993:
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
    case -9989: {
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
        GameRandom::tag(6);
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
    case -9935:
    case -9934:
    case -9933:
    case -9932:
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
    case -9958:
        value = e ? float(std::atan2(double(e->motion_delta[1]), double(e->motion_delta[0]))) : 0.f;
        return true;
    case -9944: {
        const double dx = double(e ? e->position[0] : 0.0f) - double(sim.player_x);
        const double dy = double(e ? e->position[1] : 0.0f) - double(sim.player_y);
        const float squared = float(dx * dx + dy * dy);
        value = float(std::sqrt(double(squared)));
        return true;
    }
    case -9943:
    case -9942:
    case -9941:
    case -9940: {
        const auto* boss = primary_boss(sim);
        value = boss ? float(boss->i_slots[id + 9943]) : 0.f;
        return true;
    }
    case -9939:
    case -9938:
    case -9937:
    case -9936: {
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
    case -9908: {
        i32 count = 0;
        integer(id, count);
        value = float(count);
        return true;
    }
    case -9907:
        value = float(sim.session ? sim.session->selected_spell_id : -1);
        return true;
    default:
        value = 0.0f;
        return true;
    }
}

i32* StageGvars::integer_destination(i32 id) {
    if (id >= -9926 && id <= -9923) return &sim->shared_integers[id + 9926];
    StageEnemy* e = sim->current;
    if (e && id >= -9985 && id <= -9982) return &e->i_slots[id + 9985];
    if (id == -9914 && e) return &e->script_id;
    for (auto& pair : writable_ints_)
        if (pair.first == id) return &pair.second;
    if (id == -9949 || id == -9948 || id == -9947) {
        writable_ints_.emplace_back(id, 0);
        return &writable_ints_.back().second;
    }
    return nullptr;
}

float* StageGvars::float_destination(i32 id) {
    if (id >= -9922 && id <= -9915) return &sim->shared_floats[id + 9922];
    StageEnemy* e = sim->current;
    if (e && id >= -9981 && id <= -9978) return &e->f_slots[id + 9981];
    if (e && id >= -9935 && id <= -9932) return &e->f_slots[id + 9939];
    return nullptr;
}

}
