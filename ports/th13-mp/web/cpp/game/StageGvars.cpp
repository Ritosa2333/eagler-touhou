#include "AnmVm.hpp"
#include "SessionState.hpp"
#include "StageSim.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace th13 {

namespace {
StageEnemy* primary_boss(StageSim& sim) {
    if (sim.boss_slot[0] < 0) return nullptr;
    for (auto& enemy : sim.enemies)
        if (!enemy->dead && enemy->script_id == sim.boss_slot[0]) return enemy.get();
    return nullptr;
}

float aim(const StageSim& sim, const StageEnemy* enemy, const float* source) {
    if (!enemy) return 0.0f;
    const float dy = sim.player_y - source[1];
    const float dx = sim.player_x - source[0];
    if (dy == 0.0f && dx == 0.0f) return 1.5707964f;
    return float(std::atan2(double(dy), double(dx)));
}

float motion_angle(const StageEnemy* enemy) {
    if (!enemy) return 0.0f;
    return float(std::atan2(double(enemy->motion_delta[1]), double(enemy->motion_delta[0])));
}

float distance_to_enemy(const StageSim& sim, const StageEnemy* enemy) {
    const double dx = double(sim.player_x) - double(enemy ? enemy->position[0] : 0.0f);
    const double dy = double(sim.player_y) - double(enemy ? enemy->position[1] : 0.0f);
    const float squared = float(dx * dx + dy * dy);
    return float(std::sqrt(double(squared)));
}

i32 active_enemy_count(const StageSim& sim) {
    i32 count = 0;
    for (const auto& enemy : sim.enemies)
        if ((enemy->flags & 0x31u) == 0 && enemy->invulnerable < 1) ++count;
    return count;
}

float float_script(i32 script) {
    if (script >= 0) return float(script);
    return float(double(script) + 4294967296.0);
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
    case -9998:
        value = 0;
        return true;
    case -9997:
    case -9977:
        value = e ? truncate_int(e->position[0]) : 0;
        return true;
    case -9996:
    case -9976:
        value = e ? truncate_int(e->position[1]) : 0;
        return true;
    case -9995:
    case -9975:
        value = e ? truncate_int(e->vel.pos[0]) : 0;
        return true;
    case -9994:
    case -9974:
        value = e ? truncate_int(e->vel.pos[1]) : 0;
        return true;
    case -9993:
    case -9973:
        value = e ? truncate_int(e->pos.pos[0]) : 0;
        return true;
    case -9992:
    case -9972:
        value = e ? truncate_int(e->pos.pos[1]) : 0;
        return true;
    case -9991:
    case -9965:
        value = truncate_int(sim.player_x);
        return true;
    case -9990:
    case -9964:
        value = truncate_int(sim.player_y);
        return true;
    case -9989:
        value = 0;
        return true;
    case -9988:
        value = e ? truncate_int(e->age) : 0;
        return true;
    case -9987:
        GameRandom::tag(2);
        value = truncate_int(sim.random.signed_unit());
        return true;
    case -9986:
        value = e && e->phase_timed_out ? 1 : 0;
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
        value = e ? truncate_int(e->f_slots[id + 9981]) : 0;
        return true;
    case -9971:
        value = e ? truncate_int(e->vel.angle) : 0;
        return true;
    case -9970:
        value = e ? truncate_int(e->pos.angle) : 0;
        return true;
    case -9969:
        value = e ? truncate_int(e->vel.speed) : 0;
        return true;
    case -9968:
        value = e ? truncate_int(e->pos.speed) : 0;
        return true;
    case -9967:
        value = e ? truncate_int(e->vel.radius) : 0;
        return true;
    case -9966:
        value = e ? truncate_int(e->pos.radius) : 0;
        return true;
    case -9963: {
        StageEnemy* boss = primary_boss(sim);
        value = boss ? truncate_int(boss->position[0]) : 0;
        return true;
    }
    case -9962: {
        StageEnemy* boss = primary_boss(sim);
        value = boss ? truncate_int(boss->position[1]) : 0;
        return true;
    }
    case -9961: {
        AnmVm* vm = e && sim.enemy_animation ? sim.enemy_animation(*e, 0) : nullptr;
        value = vm ? vm->source_script : 0;
        return true;
    }
    case -9960:
        value = sim.rank;
        return true;
    case -9959:
        value = sim.diff;
        return true;
    case -9958:
        value = truncate_int(motion_angle(e));
        return true;
    case -9957:
        value = 1;
        return true;
    case -9956:
        value = truncate_int(aim(sim, e, e ? e->vel.pos : nullptr));
        return true;
    case -9955:
        value = truncate_int(aim(sim, e, e ? e->pos.pos : nullptr));
        return true;
    case -9954:
        value = e ? truncate_int(e->life) : 0;
        return true;
    case -9953:
    case -9952:
    case -9951:
    case -9950:
        value = sim.diff == id + 9953 ? 1 : 0;
        return true;
    case -9949:
    case -9948:
    case -9947: {
        const i32* slot = integer_destination(id);
        value = slot ? *slot : 0;
        return true;
    }
    case -9946:
        value = i32(sim.enemies.size());
        return true;
    case -9945:
        value = (sim.session ? sim.session->stage : 1) + sim.diff;
        return true;
    case -9944:
        value = truncate_int(distance_to_enemy(sim, e));
        return true;
    case -9943:
    case -9942:
    case -9941:
    case -9940: {
        StageEnemy* boss = primary_boss(sim);
        value = boss ? boss->i_slots[id + 9943] : 0;
        return true;
    }
    case -9939:
    case -9938:
    case -9937:
    case -9936: {
        StageEnemy* boss = primary_boss(sim);
        value = boss ? truncate_int(boss->f_slots[id + 9939]) : 0;
        return true;
    }
    case -9935:
    case -9934:
    case -9933:
    case -9932:
        value = e ? truncate_int(e->f_slots[id + 9939]) : 0;
        return true;
    case -9931:
        value = sim.last_created_enemy_id;
        return true;
    case -9930:
        value = sim.player_lives;
        return true;
    case -9914:
        value = e ? e->script_id : 0;
        return true;
    case -9911:
        value = truncate_int(motion_angle(primary_boss(sim)));
        return true;
    case -9910: {
        StageEnemy* boss = primary_boss(sim);
        value = boss ? truncate_int(boss->vel.speed) : 0;
        return true;
    }
    case -9909:
        value = e ? e->trace_id : 0;
        return true;
    case -9908:
        value = active_enemy_count(sim);
        return true;
    case -9907:
        value = sim.session ? sim.session->selected_spell_id : -1;
        return true;
    default:
        value = 0;
        return true;
    }
}

bool StageGvars::floating(i32 id, float& value) {
    StageSim& sim = *this->sim;
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
    case -9977:
        value = e ? e->position[0] : 0.0f;
        return true;
    case -9996:
    case -9976:
        value = e ? e->position[1] : 0.0f;
        return true;
    case -9995:
    case -9975:
        value = e ? e->vel.pos[0] : 0.0f;
        return true;
    case -9994:
    case -9974:
        value = e ? e->vel.pos[1] : 0.0f;
        return true;
    case -9993:
    case -9973:
        value = e ? e->pos.pos[0] : 0.0f;
        return true;
    case -9992:
    case -9972:
        value = e ? e->pos.pos[1] : 0.0f;
        return true;
    case -9991:
    case -9965:
        value = sim.player_x;
        return true;
    case -9990:
    case -9964:
        value = sim.player_y;
        return true;
    case -9989:
        value = aim(sim, e, e ? e->position : nullptr);
        return true;
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
    case -9971:
        value = e ? e->vel.angle : 0.0f;
        return true;
    case -9970:
        value = e ? e->pos.angle : 0.0f;
        return true;
    case -9969:
        value = e ? e->vel.speed : 0.0f;
        return true;
    case -9968:
        value = e ? e->pos.speed : 0.0f;
        return true;
    case -9967:
        value = e ? e->vel.radius : 0.0f;
        return true;
    case -9966:
        value = e ? e->pos.radius : 0.0f;
        return true;
    case -9963: {
        StageEnemy* boss = primary_boss(sim);
        value = boss ? boss->position[0] : 0.0f;
        return true;
    }
    case -9962: {
        StageEnemy* boss = primary_boss(sim);
        value = boss ? boss->position[1] : 0.0f;
        return true;
    }
    case -9961:
        value = 0.0f;
        return true;
    case -9960:
        value = float(sim.rank);
        return true;
    case -9959:
        value = float(sim.diff);
        return true;
    case -9958:
        value = motion_angle(e);
        return true;
    case -9957:
        value = 1.0f;
        return true;
    case -9956:
        value = aim(sim, e, e ? e->vel.pos : nullptr);
        return true;
    case -9955:
        value = aim(sim, e, e ? e->pos.pos : nullptr);
        return true;
    case -9954:
        value = e ? e->life : 0.0f;
        return true;
    case -9953:
    case -9952:
    case -9951:
    case -9950:
        value = sim.diff == id + 9953 ? 1.0f : 0.0f;
        return true;
    case -9949:
    case -9948:
    case -9947: {
        const i32* slot = integer_destination(id);
        value = slot ? float(*slot) : 0.0f;
        return true;
    }
    case -9946:
        value = float(sim.enemies.size());
        return true;
    case -9945:
        value = float((sim.session ? sim.session->stage : 1) + sim.diff);
        return true;
    case -9944:
        value = distance_to_enemy(sim, e);
        return true;
    case -9943:
    case -9942:
    case -9941:
    case -9940: {
        StageEnemy* boss = primary_boss(sim);
        value = boss ? float(boss->i_slots[id + 9943]) : 0.0f;
        return true;
    }
    case -9939:
    case -9938:
    case -9937:
    case -9936: {
        StageEnemy* boss = primary_boss(sim);
        value = boss ? boss->f_slots[id + 9939] : 0.0f;
        return true;
    }
    case -9935:
    case -9934:
    case -9933:
    case -9932:
        value = e ? e->f_slots[id + 9939] : 0.0f;
        return true;
    case -9931:
        value = float(sim.last_created_enemy_id);
        return true;
    case -9930:
        value = float(sim.player_lives);
        return true;
    case -9914:
        value = e ? float_script(e->script_id) : 0.0f;
        return true;
    case -9911:
        value = motion_angle(primary_boss(sim));
        return true;
    case -9910: {
        StageEnemy* boss = primary_boss(sim);
        value = boss ? boss->vel.speed : 0.0f;
        return true;
    }
    case -9909:
        value = e ? float_script(e->trace_id) : 0.0f;
        return true;
    case -9908:
        value = float(active_enemy_count(sim));
        return true;
    case -9907:
        value = sim.session ? float(sim.session->selected_spell_id) : -1.0f;
        return true;
    default:
        value = 0.0f;
        return true;
    }
}

i32* StageGvars::integer_destination(i32 id) {
    StageSim& sim = *this->sim;
    if (id >= -9926 && id <= -9923) return &sim.shared_integers[id + 9926];
    StageEnemy* e = sim.current;
    if (e && id >= -9985 && id <= -9982) return &e->i_slots[id + 9985];
    if (id >= -9943 && id <= -9940) {
        StageEnemy* boss = primary_boss(sim);
        return boss ? &boss->i_slots[id + 9943] : e ? &e->i_slots[id + 9943] : nullptr;
    }
    if (id >= -9949 && id <= -9947) {
        for (auto& pair : writable_ints_)
            if (pair.first == id) return &pair.second;
        writable_ints_.emplace_back(id, 0);
        return &writable_ints_.back().second;
    }
    return nullptr;
}

float* StageGvars::float_destination(i32 id) {
    StageSim& sim = *this->sim;
    if (id >= -9922 && id <= -9915) return &sim.shared_floats[id + 9922];
    StageEnemy* e = sim.current;
    if (e && id >= -9995 && id <= -9994) return &e->vel.pos[id + 9995];
    if (e && id >= -9993 && id <= -9992) return &e->pos.pos[id + 9993];
    if (e && id >= -9981 && id <= -9978) return &e->f_slots[id + 9981];
    if (e && id >= -9935 && id <= -9932) return &e->f_slots[id + 9939];
    if (id >= -9939 && id <= -9936) {
        StageEnemy* boss = primary_boss(sim);
        return boss ? &boss->f_slots[id + 9939] : e ? &e->f_slots[id + 9939] : nullptr;
    }
    return nullptr;
}

}
