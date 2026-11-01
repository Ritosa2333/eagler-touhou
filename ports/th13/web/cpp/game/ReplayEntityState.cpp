#include "ReplayEntityState.hpp"
#include <cstring>

namespace th13 {
namespace {
u32 bits(float value) { u32 out = 0; std::memcpy(&out, &value, sizeof out); return out; }
void comma(std::string& out, bool& first) { if (!first) out += ','; first = false; }
void str(std::string& out, u32 value) { out += std::to_string(value); }
void field(std::string& out, const char* name, u32 value) { out += '"'; out += name; out += "\":"; str(out, value); }
}

ReplayEntityState sample_replay_entities(const StageBattle& battle) {
    ReplayEntityState s;
    s.frame = battle.sim.frame;
    s.player_phase = i32(battle.player_phase); s.player_phase_frame = battle.player_phase_frame;
    s.player_x_bits = bits(battle.player_x); s.player_y_bits = bits(battle.player_y);
    s.bomb_timer = battle.bomb_timer; s.boss_timer_seconds = battle.sim.boss_timer_seconds;
    s.boss_timer_centiseconds = battle.sim.boss_timer_centiseconds;
    for (const auto& enemy_ptr : battle.sim.enemies) if (enemy_ptr) {
        const auto& e = *enemy_ptr;
        s.enemies.push_back({e.serial, u32(e.trace_id), e.script_id, bits(e.position[0]), bits(e.position[1]),
                             bits(e.life), e.dead,
                             e.hit_anim_timer, bits(e.spirit_accum), bits(e.spirit_threshold)});
    }
    for (u32 slot = 0; slot < battle.sim.bullets.size(); ++slot) {
        const auto& b = battle.sim.bullets[slot];
        if (!b.alive) continue;
        s.bullets.push_back({slot, b.generation, b.source_enemy, b.state,
                             bits(b.x), bits(b.y), bits(b.angle), b.alive,
                             bits(b.z), bits(b.vel[0]), bits(b.vel[1]), bits(b.vel[2]),
                             bits(b.speed), b.sprite, b.subtype, b.behavior_flags, b.pool_slot});
    }
    for (u32 slot = 0; slot < battle.sim.lasers.size(); ++slot) {
        const auto& l = battle.sim.lasers[slot];
        if (!l.active) continue;
        s.lasers.push_back({slot, l.generation, l.id, l.phase, l.time,
                            bits(l.origin[0]), bits(l.origin[1]), bits(l.length), l.active});
    }
    for (u32 slot = 0; slot < battle.shots.size(); ++slot) {
        const auto& p = battle.shots[slot];
        if (!p.alive) continue;
        s.shots.push_back({slot, p.generation, p.age, p.state, bits(p.x), bits(p.y), bits(p.damage), p.alive,
                           bits(p.angle), bits(p.speed), bits(p.width), bits(p.height),
                           bits(p.damage_x), bits(p.damage_y), p.behavior, p.type, p.anchor});
    }
    const auto& items = const_cast<ItemManager&>(battle.item_manager).items();
    for (u32 slot = 0; slot < items.size(); ++slot) {
        const auto& i = items[slot];
        if (!i.alive) continue;
        s.items.push_back({slot, 0, i.state, i.type, bits(i.x), bits(i.y), i.alive});
    }
    const auto& spirits = const_cast<ItemManager&>(battle.item_manager).spirits();
    for (u32 slot = 0; slot < spirits.size(); ++slot) {
        const auto& i = spirits[slot];
        if (!i.alive) continue;
        s.spirits.push_back({slot, i.serial, i.state, i.kind, bits(i.x), bits(i.y), i.alive});
    }
    for (u32 slot = 0; slot < battle.damage_registrations.size(); ++slot) {
        const auto& r = battle.damage_registrations[slot];
        if (!r.active || !(r.flags & 1u)) continue;
        s.damage_registrations.push_back({slot, r.generation, r.flags,
            u32(r.owner), bits(r.radius), bits(r.radius_growth),
            bits(r.angle), bits(r.width), bits(r.height),
            bits(r.center_x), bits(r.center_y), bits(r.center_z),
            r.clock.previous, r.clock.current, bits(r.clock.fractional), bits(r.damage),
            r.accumulated_damage, r.threshold, r.interval});
    }
    return s;
}

std::string ReplayEntityState::json() const {
    std::string out = "{\"schema\":\"th13-entity-state-v1\",\"frame\":";
    out += std::to_string(frame); out += ",\"player\":{\"phase\":";
    out += std::to_string(player_phase); out += ",\"phase_frame\":";
    out += std::to_string(player_phase_frame); out += ",\"x_bits\":";
    str(out, player_x_bits); out += ",\"y_bits\":"; str(out, player_y_bits);
    out += "},\"timers\":{\"bomb\":";
    out += std::to_string(bomb_timer); out += ",\"boss_seconds\":"; out += std::to_string(boss_timer_seconds);
    out += ",\"boss_centiseconds\":"; out += std::to_string(boss_timer_centiseconds); out += "}";
    auto arrays = [&](const char* name, auto& values, auto emit) {
        out += ",\""; out += name; out += "\":["; bool first = true;
        for (const auto& value : values) { comma(out, first); out += '{'; emit(value, out); out += '}'; }
        out += ']';
    };
    arrays("enemies", enemies, [](const Enemy& e, std::string& o) { field(o,"serial",e.serial);o+=",\"trace_id\":";str(o,e.trace_id);o+=",\"script_id\":";o+=std::to_string(e.script_id);o+=",\"x_bits\":";str(o,e.x_bits);o+=",\"y_bits\":";str(o,e.y_bits);o+=",\"life_bits\":";str(o,e.life_bits);o+=",\"dead\":";o+=e.dead?"true":"false";o+=",\"hit_anim_timer\":";o+=std::to_string(e.hit_anim_timer);o+=",\"spirit_accum_bits\":";str(o,e.spirit_accum_bits);o+=",\"spirit_threshold_bits\":";str(o,e.spirit_threshold_bits); });
    arrays("bullets", bullets, [](const Bullet& b, std::string& o) { field(o,"index",b.index);o+=",\"generation\":";str(o,b.generation);o+=",\"source_enemy\":";str(o,b.source_enemy);o+=",\"state\":";str(o,b.state);o+=",\"x_bits\":";str(o,b.x_bits);o+=",\"y_bits\":";str(o,b.y_bits);o+=",\"angle_bits\":";str(o,b.angle_bits);o+=",\"z_bits\":";str(o,b.z_bits);o+=",\"velocity_x_bits\":";str(o,b.velocity_x_bits);o+=",\"velocity_y_bits\":";str(o,b.velocity_y_bits);o+=",\"velocity_z_bits\":";str(o,b.velocity_z_bits);o+=",\"speed_bits\":";str(o,b.speed_bits);o+=",\"sprite\":";str(o,b.sprite);o+=",\"subtype\":";str(o,b.subtype);o+=",\"behavior_flags\":";str(o,b.behavior_flags);o+=",\"pool_slot\":";o+=std::to_string(b.pool_slot);o+=",\"alive\":";o+=b.alive?"true":"false"; });
    arrays("lasers", lasers, [](const Laser& l, std::string& o) { field(o,"index",l.index);o+=",\"generation\":";str(o,l.generation);o+=",\"id\":";o+=std::to_string(l.id);o+=",\"phase\":";o+=std::to_string(l.phase);o+=",\"time\":";o+=std::to_string(l.time);o+=",\"origin_x_bits\":";str(o,l.origin_x_bits);o+=",\"origin_y_bits\":";str(o,l.origin_y_bits);o+=",\"length_bits\":";str(o,l.length_bits);o+=",\"active\":";o+=l.active?"true":"false"; });
    arrays("shots", shots, [](const Shot& p, std::string& o) { field(o,"index",p.index);o+=",\"generation\":";str(o,p.generation);o+=",\"age\":";o+=std::to_string(p.age);o+=",\"state\":";o+=std::to_string(p.state);o+=",\"x_bits\":";str(o,p.x_bits);o+=",\"y_bits\":";str(o,p.y_bits);o+=",\"damage_bits\":";str(o,p.damage_bits);o+=",\"angle_bits\":";str(o,p.angle_bits);o+=",\"speed_bits\":";str(o,p.speed_bits);o+=",\"width_bits\":";str(o,p.width_bits);o+=",\"height_bits\":";str(o,p.height_bits);o+=",\"damage_x_bits\":";str(o,p.damage_x_bits);o+=",\"damage_y_bits\":";str(o,p.damage_y_bits);o+=",\"behavior\":";o+=std::to_string(p.behavior);o+=",\"type\":";o+=std::to_string(p.type);o+=",\"anchor\":";o+=std::to_string(p.anchor);o+=",\"alive\":";o+=p.alive?"true":"false"; });
    arrays("items", items, [](const Item& i, std::string& o) { field(o,"slot",i.slot);o+=",\"state\":";o+=std::to_string(i.state);o+=",\"kind\":";o+=std::to_string(i.kind);o+=",\"x_bits\":";str(o,i.x_bits);o+=",\"y_bits\":";str(o,i.y_bits);o+=",\"alive\":";o+=i.alive?"true":"false"; });
    arrays("spirits", spirits, [](const Item& i, std::string& o) { field(o,"slot",i.slot);o+=",\"serial\":";str(o,i.serial);o+=",\"state\":";o+=std::to_string(i.state);o+=",\"kind\":";o+=std::to_string(i.kind);o+=",\"x_bits\":";str(o,i.x_bits);o+=",\"y_bits\":";str(o,i.y_bits);o+=",\"alive\":";o+=i.alive?"true":"false"; });
    arrays("damage_registrations", damage_registrations, [](const Registration& r, std::string& o) {
        field(o, "slot", r.slot);
        const auto raw = [&](const char* name, u32 value) { o += ','; field(o, name, value); };
        const auto integer = [&](const char* name, i32 value) {
            o += ",\""; o += name; o += "\":"; o += std::to_string(value);
        };
        raw("generation", r.generation); raw("flags", r.flags);
        raw("owner", r.owner); raw("radius_bits", r.radius_bits);
        raw("radius_growth_bits", r.radius_growth_bits);
        raw("angle_bits", r.angle_bits); raw("width_bits", r.width_bits); raw("height_bits", r.height_bits);
        raw("center_x_bits", r.center_x_bits); raw("center_y_bits", r.center_y_bits); raw("center_z_bits", r.center_z_bits);
        integer("previous", r.previous); integer("current", r.current);
        raw("current_f_bits", r.current_f_bits); raw("damage_bits", r.damage_bits);
        integer("accumulated_damage", r.accumulated_damage);
        integer("threshold", r.threshold); integer("interval", r.interval);
    });
    out += '}'; return out;
}
}  // namespace th13
