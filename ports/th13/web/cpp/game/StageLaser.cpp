#include "StageSim.hpp"
#include "StageMath.hpp"

#include <algorithm>

namespace th13 {
namespace {
bool on_screen(const Vec3& p, float margin) {
    return double(p.x) + margin >= -192 && double(p.x) - margin < 192 &&
           double(p.y) + margin >= 0 && double(p.y) - margin < 448;
}
}

float StageLaser::collision_width() const {
    if(kind==Kind::Curve)return width*.5f; // 435a07: every segment, all widths
    // 4336cd/4336d8: narrowed full width passed to 444260.
    return width < 32.0f ? width * 0.5f : kind == Kind::Line
        ? float(double(width) - (double(width) + 16.0) * 0.5)
        : float(double(width) - (double(width) + 16.0) / 3.0);
}

i32 StageLaser::player_contact(float x, float y, float half_x, float half_y) const {
    if (kind == Kind::Beam) return 0; // 438000: no collision.
    float ox = origin[0], oy = origin[1], span = length;
    if (kind == Kind::Line) {
        float dx, dy;
        polar_xy(angle, float(double(length) / 10), dx, dy);
        ox += dx; oy += dy;
        span = float(double(length) * 4 / 5);
    }
    const float dx = x - ox, dy = y - oy;
    const float sa = float(std::sin(double(-angle))), ca = float(std::cos(double(-angle)));
    const float along = float(double(ca) * dx - double(sa) * dy);
    const float across = float(double(ca) * dy + double(dx) * sa);
    const double half_width = double(collision_width()) * .5;
    const auto overlaps = [&](float hx, float hy) {
        return double(along) - hx <= span && double(along) + hx >= 0 &&
               double(across) - hy <= half_width && double(across) + hy >= -half_width;
    };
    if (!overlaps(half_x * 16, half_y * 16)) return 0;
    return overlaps(half_x, half_y) ? 1 : 2;
}

bool StageLaser::update(const float* first_boss_position, float rate) {
    if (kind == Kind::Beam) return true; // 437ff0: no-op update.
    // Split children have an empty line descriptor queue (433870/4342e0).
    if (kind == Kind::Line) {
        if (target_length <= length) {
            start_offset = float(double(length_speed)*rate+start_offset);
            for (int n = 0; n < 3; ++n) origin[n] += float(velocity[n]*rate);
            if (line_limit > 0 && double(line_limit) < double(length) + start_offset) {
                length = line_limit - start_offset;
                target_length = length;
                if (length <= 0) return false;
            }
        } else length = std::min(target_length, float(double(length_speed)*rate+length));
        if (cull_timer < 1) {
            float dx, dy;
            polar_xy(angle, length, dx, dy);
            if (!on_screen({origin[0], origin[1], origin[2]}, width) &&
                !on_screen({origin[0] + dx, origin[1] + dy, origin[2]}, width)) return false;
        } else --cull_timer;
        anm_flags |= 8;
        return true;
    }
    // 434cc0: infinite lasers have their own queue interpreter. In particular
    // 0x100000 changes ANM blend bits, not player collision. Unknown queue ops
    // advance as in the retail function, without ordinary-bullet semantics.
    while (ex_cursor < 18) {
        const auto& entry = ex[ex_cursor];
        if (!entry.op || (!entry.flags && behavior_flags)) break;
        if (entry.op == 0x80) cancel_protection = i32(entry.args[4]);
        else if (entry.op == 0x400) phase = 3;
        else if (entry.op == 0x100000) {
            anm_flags = i32(entry.args[4]) == 0 ? anm_flags & 0xfffffe1fu
                                             : (anm_flags & 0xfffffe3fu) | 0x20u;
        }
        ++ex_cursor;
    }
    if (length < target_length) {
        length = float(double(length_speed)*rate + double(length));
        if (target_length < length) length = target_length;
    }
    angle = wrap_angle(float(double(angle) + double(float(rotation*rate))));
    if ((flags & 1) && first_boss_position)
        std::copy_n(first_boss_position, 3, origin);
    for (int n = 0; n < 3; ++n) origin[n] += float(velocity[n]*rate);

    const auto reset_time = [&] { previous_time = -1; time = 0; time_fraction = 0; };
    if (phase == 3) {
        if (time >= timing[0]) { reset_time(); phase = 4; }
        // This transition does not fall through to opening's width formula.
    } else {
        if (phase == 4) {
            if (time >= timing[1]) { reset_time(); width = target_width; phase = 2; }
            else width = float(double(target_width) * double(time_fraction) / double(timing[1]));
        }
        if (phase == 2 && time >= timing[2]) { reset_time(); phase = 5; }
        if (phase == 5) {
            if (time >= timing[3]) return false;
            width = float(double(target_width) -
                          double(time_fraction) * double(target_width) / double(timing[3]));
        }
    }
    anm_flags |= 8; // width/length scale is refreshed before the body's ANM tick.
    return true;
}

StageLaser* StageSim::find_laser(i32 id) {
    if (!id) return nullptr;
    for (auto& laser : lasers)
        if (laser.active && laser.id == id) return &laser;
    return nullptr;
}

void StageSim::emit_infinite_laser(const StageEt& et, const StageEnemy& enemy, i32 id) {
    // 42fed0 caps the entire live list, including nodes pending retirement.
    if (lasers.size() >= 256) return;
    StageLaser laser;
    laser.id = id;
    if (et.absolute[2] <= 0.9f) {
        for (int n = 0; n < 3; ++n) laser.origin[n] = et.offset[n] + enemy.position[n];
    } else {
        laser.origin[0] = et.absolute[0] + et.offset[0];
        laser.origin[1] = et.absolute[1] + et.offset[1];
        laser.origin[2] = 0;
    }
    laser.angle = wrap_angle(et.angle1);
    if (et.spin != 0) {
        float dx, dy;
        polar_xy(laser.angle, et.spin, dx, dy);
        laser.origin[0] += dx;
        laser.origin[1] += dy;
    }
    laser.length = et.spawn[0];
    laser.target_length = et.spawn[1];
    laser.length_speed = et.speed1;
    laser.target_width = et.laser_width;
    std::copy_n(et.laser_timing, 4, laser.timing);
    laser.flags = u32(et.laser_timing[4]) | 2u;
    laser.sprite = et.sprite;
    laser.subtype = et.subtype;
    laser.sound = et.sound;
    laser.transform_sound = et.transform_sound;
    std::copy_n(et.ex, 18, laser.ex);
    append_laser(laser);
}

void StageSim::append_laser(StageLaser laser) {
    if (lasers.size() >= 256) return;
    laser_serial = u32(wrapping_add(signed_bits(laser_serial), 1));
    if (signed_bits(laser_serial) < 0x10000) laser_serial = 0x10000;
    laser.generation = laser_serial;
    if (laser.kind != StageLaser::Kind::Infinite && laser.kind != StageLaser::Kind::Beam)
        laser.id = signed_bits(laser_serial);
    lasers.push_back(laser); // 42fed0 appends, including duplicate IDs.
    if (laser_birth && !laser_birth(lasers.back())) {
        error_ = "Infinite laser animation birth failed";
        return;
    }
    // 4333c1 passes a literal zero pan to 460c50.
    if (laser.kind != StageLaser::Kind::Beam && laser.sound >= 0 && laser_sound && !laser_sound(laser.sound))
        error_ = "Infinite laser birth sound failed";
}

void StageSim::emit_beam_laser(const StageEt& et,const StageEnemy& enemy,i32 id) {
    if(lasers.size()>=256)return;
    StageLaser laser; laser.kind=StageLaser::Kind::Beam; laser.id=id;
    for(int n=0;n<3;++n)laser.origin[n]=et.offset[n]+enemy.position[n];
    if(et.absolute[2]>.9f) {
        laser.origin[0]=et.offset[0]+et.absolute[0];
        laser.origin[1]=et.offset[1]+et.absolute[1];laser.origin[2]=0;
    }
    // 437d70 copies descriptor +18 angle and +20 width; 713 writes
    // emitter speed1 to +20, and its explicit ID to +28.
    laser.angle=wrap_angle(et.angle1);laser.width=et.speed1;
    laser.sprite=et.sprite;laser.subtype=et.subtype;
    std::copy_n(et.ex,18,laser.ex);
    append_laser(laser);
}

void StageSim::emit_line_laser(const StageEt& et,const StageEnemy& enemy) {
    if(lasers.size()>=256)return;
    StageLaser laser;laser.kind=StageLaser::Kind::Line;laser.phase=2;
    for(int n=0;n<3;++n)laser.origin[n]=et.offset[n]+enemy.position[n];
    if(et.absolute[2]>.9f){
        laser.origin[0]=et.offset[0]+et.absolute[0];
        laser.origin[1]=et.offset[1]+et.absolute[1];laser.origin[2]=0;
    }
    laser.angle=laser.line_descriptor_angle=wrap_angle(et.angle1);
    if(et.spin!=0){float x,y;polar_xy(laser.angle,et.spin,x,y);laser.origin[0]+=x;laser.origin[1]+=y;}
    laser.length=laser.line_initial_length=et.spawn[0];
    laser.target_length=et.spawn[1];laser.line_limit=et.spawn[2];
    laser.width=laser.target_width=et.laser_width;laser.length_speed=et.speed1;
    laser.start_offset=laser.target_length<laser.length?.01f:0.f;
    polar_xy(laser.angle,laser.length_speed,laser.velocity[0],laser.velocity[1]);
    laser.flags=1;laser.sprite=et.sprite;laser.subtype=et.subtype;
    laser.sound=et.sound;laser.transform_sound=et.transform_sound;
    laser.cull_timer=30;std::copy_n(et.ex,18,laser.ex);
    append_laser(laser);
}

void StageSim::emit_curve_laser(const StageEt& et,const StageEnemy& enemy) {
    if(lasers.size()>=256)return;
    StageLaser laser;laser.kind=StageLaser::Kind::Curve;laser.phase=2;
    laser.origin[0]=et.offset[0]+enemy.position[0];laser.origin[1]=et.offset[1]+enemy.position[1];laser.origin[2]=et.offset[2]+enemy.position[2];
    if(et.absolute[2]>.9f) {
        laser.origin[0]=et.offset[0]+et.absolute[0];
        laser.origin[1]=et.offset[1]+et.absolute[1];laser.origin[2]=0;
    }
    laser.angle=wrap_angle(et.angle1);laser.width=laser.target_width=et.laser_width;
    if(et.spin!=0) {
        float x,y;polar_xy(laser.angle,et.spin,x,y);
        laser.origin[0]+=x;laser.origin[1]+=y;
    }
    // 41ec71 -> descriptor+20 ->434f10(+5e0): number of history
    // samples, taken from701 arg1, independent of700's length fields.
    if(et.laser_timing[0]<1) {error_="Curve laser sample count must be positive";return;}
    laser.length_speed=et.speed1;laser.flags=1;
    laser.sprite=et.sprite;laser.subtype=et.subtype;laser.sound=et.sound;laser.transform_sound=et.transform_sound;
    laser.curve_path.reset({laser.origin[0],laser.origin[1],laser.origin[2]},laser.angle,et.speed1);
    std::copy_n(et.ex,18,laser.ex);laser.curve_samples.resize(size_t(et.laser_timing[0]));laser.cull_timer=30;
    laser.curve_path.sample(0,laser.curve_samples);
    polar_xy(laser.angle,et.speed1,laser.curve_samples[0].delta.x,laser.curve_samples[0].delta.y);
    append_laser(laser);
}

bool StageSim::advance_line_behaviors(StageLaser& laser) {
    const float rate=time_rate();
    const float tick=rate>.99f&&rate<1.01f?1.f:rate;
    // 4306e0 -> 4315d0; completion re-enters the descriptor queue in the
    // SAME update, before length growth or movement. Line op4 differs from
    // bullet op4: scalar speed is accumulated, never replaced by hypot.
    u32 finished=0;
    do {
        while(laser.ex_cursor<18) {
            const auto& entry=laser.ex[laser.ex_cursor];
            if(!entry.op||(!entry.flags&&laser.behavior_flags))break;
            const i32 value=i32(entry.args[4]);
            switch(entry.op) {
            case 1:case 8: laser.behavior_flags|=entry.op;break; // native virtual handlers are42f690/42f6b0 stubs
            case 4: {
                laser.behavior_flags|=4;laser.acceleration_time=0;laser.acceleration_fraction=0;
                laser.acceleration=entry.args[0];laser.acceleration_duration=value;
                float angle=laser.angle;
                if(entry.args[1]>-990.f) {
                    angle=entry.args[1];
                    if(angle>=990.f){const float dx=player_x-laser.origin[0],dy=player_y-laser.origin[1];
                        angle=dx==0&&dy==0?1.5707964f:float(std::atan2(double(dy),double(dx)));}
                }
                polar_xy(angle,laser.acceleration,laser.acceleration_velocity[0],laser.acceleration_velocity[1]);
                if(laser.ex_cursor&&laser.transform_sound>=0&&laser_sound&&!laser_sound(laser.transform_sound)){
                    error_="Line laser transform sound failed";return false;
                }
                break;
            }
            case 0x80:laser.cancel_protection=value;break;
            case 0x400:laser.phase=3;break;
            case 0x800:
                if(laser_sound&&!laser_sound(value)){error_="Line laser queue sound failed";return false;}break;
            case 0x8000:laser.id=value;break;
            case 0x10000:laser.ex_cursor=value;continue;
            case 0x100000:laser.anm_flags=value==0?laser.anm_flags&0xfffffe1fu:(laser.anm_flags&0xfffffe3fu)|0x20u;break;
            case 0x80000000:laser.behavior_flags|=0x80000000u;laser.wait_remaining=value;laser.wait_fraction=float(value);break;
            case 0x10:case 0x40:case 0x200:case 0x1000:case 0x2000:
                error_="Unimplemented line laser queue operation "+std::to_string(entry.op);return false;
            default:break; // retail4306e0 default advances unrecognized entries
            }
            ++laser.ex_cursor;
        }
        const u32 previous=laser.behavior_flags;
        if(!previous)break;
        if(previous&4) {
            if(laser.acceleration_time<laser.acceleration_duration) {
                laser.length_speed=float(double(laser.acceleration)*rate+laser.length_speed);
                for(int n=0;n<3;++n)laser.velocity[n]+=float(laser.acceleration_velocity[n]*rate);
                if(std::abs(laser.velocity[0])>.0001f||std::abs(laser.velocity[1])>.0001f)
                    laser.angle=float(std::atan2(double(laser.velocity[1]),double(laser.velocity[0])));
                laser.acceleration_fraction+=tick;
                laser.acceleration_time=truncate_int(laser.acceleration_fraction);
            } else laser.behavior_flags&=~4u;
        }
        if(previous&0x80000000u){if(laser.wait_remaining<1)laser.behavior_flags&=~0x80000000u;else {
            laser.wait_fraction-=tick;laser.wait_remaining=truncate_int(laser.wait_fraction);
        }}
        if(laser.cancel_protection) --laser.cancel_protection;
        finished=previous&~laser.behavior_flags;
    }while(finished);
    return true;
}

void StageSim::advance_lasers() {
    const float rate=time_rate();
    const float tick=rate>.99f&&rate<1.01f?1.f:rate;
    // Only boss slot0 is followed (43350e); do not substitute slot1/slot2 or
    // the enemy that originally emitted this laser.
    const float* first_boss = nullptr;
    for (const auto& enemy : enemies)
        if (!enemy->dead && enemy->script_id == boss_slot[0]) {
            first_boss = enemy->position;
            break;
        }
    for (size_t index = 0; index < lasers.size();) {
        // 42fcd0 saves next BEFORE the callback. New children of the old tail
        // wait a frame; children appended earlier may be reached this frame.
        const bool had_next = index + 1 < lasers.size();
        if(lasers[index].active&&lasers[index].kind==StageLaser::Kind::Curve&&!lasers[index].pending_remove&&lasers[index].phase!=1) {
            auto& curve=lasers[index];
            while(curve.ex_cursor<18) {
                const auto& ex=curve.ex[curve.ex_cursor];if(!ex.op)break;
                if(!ex.flags&&curve.behavior_flags)break;
                if(ex.op&curve.behavior_flags)break;
                if(ex.op==4||ex.op==8)curve.curve_path.append(ex.op==4?1:2,float(ex.args[5]),i32(ex.args[4]),ex.args[0],ex.args[1]);
                else if(ex.op==0x80)curve.cancel_protection=i32(ex.args[4]);
                else if(ex.op==0x8000)curve.id=i32(ex.args[4]);
                else if(ex.op==0x100000)curve.anm_flags=ex.args[4]==0?curve.anm_flags&~0x1e0u:(curve.anm_flags&~0x3e0u)|0x20u;
                ++curve.ex_cursor;
            }
            //4355f0 samples path at float timer+44 minus segment index;
            // quantizing to the integer timer makes slowed curves jump.
            curve.curve_path.sample(curve.time_fraction,curve.curve_samples);
            // 4355f0 retires once all samples leave the playfield, after
            // the thirty-update grace period. A curve is not an immortal chord.
            if(curve.cull_timer<1 && !(curve.behavior_flags&0x100u) &&
               std::none_of(curve.curve_samples.begin(),curve.curve_samples.end(),
                   [&](const CurveLaserSample& p){return on_screen(p.position,curve.width);})) {
                if(laser_retire)laser_retire(curve.generation);
                lasers.erase(lasers.begin()+index);
                if(!had_next)break;
                continue;
            }
            if(curve.cull_timer>0)--curve.cull_timer;
            // 4355f0 updates the sample array, not the base origin/angle.
            // Drawing consumes that array directly; do not replace the curve
            // with a head-to-tail chord or overwrite its emitter state.
            // 435996: accumulate signed segment speeds, query from16px inclusive,
            // query at sample+half-speed. A hit splits the original curve
            // immediately; subsequent iterations observe its new sample count.
            float distance=0;
            for(size_t sample=0;sample+1<lasers[index].curve_samples.size();++sample) {
                const auto point=lasers[index].curve_samples[sample];
                distance+=point.speed;
                if(!(distance>=16)||!laser_collision)continue;
                StageLaser segment;segment.kind=StageLaser::Kind::Curve;segment.phase=2;
                float dx,dy;polar_xy(point.angle,point.speed*.5f,dx,dy);
                segment.origin[0]=point.position.x+dx;segment.origin[1]=point.position.y+dy;
                segment.origin[2]=point.position.z;segment.angle=point.angle;
                segment.length=point.speed;segment.width=lasers[index].width;
                segment.generation=lasers[index].generation;segment.id=lasers[index].id;
                segment.graze_timer=lasers[index].graze_timer;
                if(laser_collision(segment)==1)
                    split_curve_laser(index,{player_x,player_y,0},{32,32,0},0,false,false,true);
                if(!error_.empty())return;
            }
            // Splitting can append nodes and reallocate the vector.
            auto& updated=lasers[index];
            if(laser_animation&&!laser_animation(updated)){error_="Curve laser animation update failed";return;}
            ++updated.graze_timer;
            updated.previous_time=updated.time;updated.time_fraction+=tick;
            updated.time=truncate_int(updated.time_fraction);updated.has_updated=true;
            ++index;if(!had_next)break;continue;
        }
        if(lasers[index].active&&!lasers[index].pending_remove&&lasers[index].phase!=1&&
           lasers[index].kind==StageLaser::Kind::Line&&!advance_line_behaviors(lasers[index]))return;
        if (!lasers[index].active || lasers[index].pending_remove ||
            lasers[index].phase == 1 || !lasers[index].update(first_boss,rate)) {
            if (laser_retire) laser_retire(lasers[index].generation);
            lasers.erase(lasers.begin() + index);
            if (!had_next) break;
            continue;
        }
        if (laser_collision && lasers[index].collision_enabled()) {
            const i32 result = laser_collision(lasers[index]);
            if (result == 1)
                split_laser(index, {player_x, player_y, 0}, {32, 32, 0}, 0, false, false, true);
            else if (result == 2 && lasers[index].kind == StageLaser::Kind::Line)
                ++lasers[index].graze_timer;
        }
        auto& laser = lasers[index];
        if (laser_animation && !laser_animation(laser)) {
            error_ = "Infinite laser animation update failed";
            return;
        }
        laser.previous_time = laser.time;
        laser.time_fraction += tick;
        laser.time = truncate_int(laser.time_fraction);
        laser.has_updated = true;
        ++index;
        if (!had_next) break;
    }
}

bool StageSim::laser_effect(const StageLaser& laser, const Vec3& pos, bool rectangle) {
    i32 base = -1;
    if (laser.kind==StageLaser::Kind::Curve || laser.sprite < 18 || laser.sprite == 33 || laser.sprite == 37) base = 163;
    else if (laser.sprite < (rectangle && laser.kind == StageLaser::Kind::Line ? 30u : 31u)) base = 195;
    else if (laser.sprite < 33) base = 211;
    if (base < 0) return true;
    const i32 script = base + i32(laser.subtype) * 2;
    if (laser_cancel_effect) {
        if (!laser_cancel_effect(script, pos)) {
            error_ = "Laser cancellation animation failed";
            return false;
        }
    } else anm_effects.push_back({0, script, pos.x, pos.y, pos.z});
    return true;
}

void StageSim::split_laser(size_t index, const Vec3& center, const Vec3& size,
                           float angle, bool circle, bool spawn_items, bool respect_protection) {
    if(lasers[index].kind==StageLaser::Kind::Beam)return; // 438010/438020: no-op
    if(lasers[index].kind==StageLaser::Kind::Curve) {
        split_curve_laser(index,center,size,angle,circle,spawn_items,respect_protection);
        return;
    }
    // 433870/4342e0 and 431bf0/4327c0 sample segment centers every16px.
    const StageLaser source = lasers[index];
    if (respect_protection && source.cancel_protection) return;
    const bool line = source.kind == StageLaser::Kind::Line;
    const bool inclusive = !circle || line;
    if (inclusive ? source.length < 16 : source.length <= 16) return;
    float dx, dy;
    polar_xy(source.angle, 8, dx, dy);
    Vec3 pos{source.origin[0] + dx, source.origin[1] + dy, source.origin[2]};
    dx += dx; dy += dy;
    const float sa = float(std::sin(double(-angle))), ca = float(std::cos(double(-angle)));
    const float ox = source.origin[0] - center.x, oy = source.origin[1] - center.y;
    float tx = float(double(ox) * ca - double(sa) * oy);
    float ty = float(double(ox) * sa + double(oy) * ca);
    float sx, sy;
    polar_xy(wrap_angle(source.angle - angle), 8, sx, sy);
    tx += sx; ty += sy; sx += sx; sy += sy;
    std::vector<bool> removed;
    bool any = false;
    float distance = 8;
    do {
        const double cx = double(center.x) - pos.x, cy = double(center.y) - pos.y;
        const float distance2 = float(cx * cx + cy * cy);
        const bool hit = circle ? double(distance2) <= double(size.x) * size.x
            : -double(size.x) * .5 <= tx && tx <= double(size.x) * .5 &&
              -double(size.y) * .5 <= ty && ty <= double(size.y) * .5;
        removed.push_back(hit);
        if (hit) {
            any = true;
            if (!circle && spawn_items && items && on_screen(pos, 32))
                items->spawn_item(pos.x, pos.y, pos.z, 9, -1.5707964f, 2.2f);
            if ((!circle || line || on_screen(pos, 32)) && !laser_effect(source, pos, !circle)) return;
        }
        pos.x += dx; pos.y += dy; tx += sx; ty += sy;
        distance += 16;
    } while (inclusive ? double(distance) + 8 <= source.length : double(distance) + 8 < source.length);
    if (!any) return;
    auto& parent = lasers[index];
    size_t first = 0;
    while (first < removed.size() && removed[first]) ++first;
    if (line) {
        if (first == removed.size()) { parent.pending_remove = true; return; }
        if (first) {
            parent.origin[0] += float(first) * dx;
            parent.origin[1] += float(first) * dy;
            parent.length -= float(first) * 16;
            if (parent.length <= 24) { parent.pending_remove = true; return; }
            parent.target_length = parent.length;
            parent.start_offset = float(first) * 16;
        }
    }
    size_t end = first;
    while (end < removed.size() && !removed[end]) ++end;
    if (!line && first) end = first; // the infinite parent must retain its original origin
    if (end == removed.size() && (line || !first)) return;
    const float retained = float(end - first) * 16;
    if (line) {
        parent.target_length = float(double(parent.target_length) - (double(parent.length) - retained));
        if (retained < 24) parent.pending_remove = true;
    }
    parent.length = retained;
    // Parent updates are committed before appending; vector growth cannot
    // invalidate references used by the manager or the presentation callback.
    for (size_t begin = end; begin < removed.size();) {
        if (removed[begin]) { ++begin; continue; }
        size_t finish = begin;
        while (finish < removed.size() && !removed[finish]) ++finish;
        const float length = float(finish - begin) * 16;
        Vec3 start{source.origin[0] + float(begin) * dx,
                   source.origin[1] + float(begin) * dy, source.origin[2]};
        if ((!line || length > 24) && (!circle || line || on_screen(start, 32))) {
            StageLaser child;
            child.kind = StageLaser::Kind::Line;
            child.phase = 2;
            child.origin[0] = start.x; child.origin[1] = start.y; child.origin[2] = start.z;
            child.angle = source.angle;
            child.length = child.target_length = length;
            child.width = child.target_width = source.width;
            child.length_speed = line ? source.length_speed : 8;
            child.line_limit = line ? source.line_limit : source.target_length - float(begin) * 16;
            child.sprite = source.sprite; child.subtype = source.subtype;
            child.flags = line ? source.flags : circle ? 0 : (source.flags >> 1) & 1;
            child.sound = line ? source.sound : circle ? 0 : -1;
            child.transform_sound = line ? source.transform_sound : circle ? 0 : -1;
            child.cull_timer = 30;
            polar_xy(child.angle, child.length_speed, child.velocity[0], child.velocity[1]);
            append_laser(child);
        }
        begin = finish;
    }
}

void StageSim::split_curve_laser(size_t index,const Vec3& center,const Vec3& size,
                                 float angle,bool circle,bool spawn_items,bool protection) {
    // 4371b0 rectangle /4378d0 circle: cancellation tests sample points,
    // not the rendered ribbon width and not fixed16px line segments.
    const StageLaser source=lasers[index];
    if(protection&&source.cancel_protection)return;
    const size_t count=source.curve_samples.size();
    std::vector<bool> removed(count,false);size_t hits=0;
    const float sine=float(std::sin(double(-angle))),cosine=float(std::cos(double(-angle)));
    for(size_t i=0;i<count;++i) {
        const auto& p=source.curve_samples[i].position;
        const float dx=p.x-center.x,dy=p.y-center.y;
        const float x=float(double(dx)*cosine-double(dy)*sine);
        const float y=float(double(sine)*dx+double(cosine)*dy);
        const float distance2=float((double(center.x)-p.x)*(double(center.x)-p.x)+
                                    (double(center.y)-p.y)*(double(center.y)-p.y));
        removed[i]=circle?double(distance2)<=double(size.x)*size.x:
            std::abs(x)<=double(size.x)*.5 && std::abs(y)<=double(size.y)*.5;
        if(!removed[i])continue;
        ++hits;
        if(i%3==0) {
            if(!circle&&spawn_items&&items&&on_screen(p,32))
                items->spawn_item(p.x,p.y,p.z,9,-1.5707964f,2.2f);
            if(!laser_effect(source,p,!circle))return;
        }
    }
    if(!hits)return;
    if(hits==count){lasers[index].pending_remove=true;return;}
    size_t first=0;while(first<count&&removed[first])++first;
    if(circle) {
        // 4378d0 does not create children or rewind the path timer. A leading
        // hit run shifts sample data; an interior run discards the prefix
        // through that run if a surviving tail exists, otherwise truncates.
        size_t shift=first,keep=count-first;
        if(!first) {
            size_t hit=0;while(hit<count&&!removed[hit])++hit;
            size_t tail=hit;while(tail<count&&removed[tail])++tail;
            if(tail<count){shift=tail;keep=count-tail;}
            else {shift=0;keep=hit;}
        }
        auto& samples=lasers[index].curve_samples;
        if(shift)for(size_t i=0;i<keep;++i)samples[i]=source.curve_samples[i+shift];
        samples.resize(keep);return;
    }
    auto& parent=lasers[index];
    if(first) {
        parent.previous_time=parent.time;
        parent.time-=i32(first);parent.time_fraction-=float(first);
        parent.curve_samples.resize(count-first); // native leaves the array unshifted
        if(count-first<=3){parent.pending_remove=true;return;}
    }
    size_t end=first;while(end<count&&!removed[end])++end;
    const size_t prefix=end-first;
    // 4371b0 copies the original descriptor/path into each surviving run
    // longer than3. The parent retains the first run with its current array.
    for(size_t at=end;at<count;) {
        while(at<count&&removed[at])++at;
        const size_t begin=at;while(at<count&&!removed[at])++at;
        if(at-begin<=3)continue;
        StageLaser child;child.kind=StageLaser::Kind::Curve;child.phase=2;
        std::copy_n(source.origin,3,child.origin);
        child.angle=source.curve_path.segments.front().angle;
        child.width=child.target_width=source.target_width;
        child.length_speed=source.curve_path.segments.front().speed;
        child.sprite=source.sprite;child.subtype=source.subtype;child.flags=source.flags;
        child.sound=-1;child.transform_sound=source.transform_sound;
        child.curve_path=source.curve_path;std::copy_n(source.ex,18,child.ex);
        child.ex_cursor=99;child.cull_timer=30;
        child.time=source.time-i32(begin);child.time_fraction=source.time_fraction-float(begin);
        child.curve_samples.resize(at-begin);
        child.curve_path.sample(0,child.curve_samples);
        polar_xy(child.angle,child.length_speed,child.curve_samples[0].delta.x,child.curve_samples[0].delta.y);
        child.curve_path.sample(child.time_fraction,child.curve_samples);
        append_laser(child);if(!error_.empty())return;
    }
    if(prefix>3)lasers[index].curve_samples.resize(prefix);
    else lasers[index].pending_remove=true;
}

void StageSim::cancel_lasers_radius(const Vec3& pos, float radius, bool protection) {
    for (size_t i = 0; i < lasers.size();) {
        const bool had_next = i + 1 < lasers.size();
        if (lasers[i].phase != 1) split_laser(i, pos, {radius, 0, 0}, 0, true, false, protection);
        if (!had_next) break;
        ++i;
    }
}

void StageSim::cancel_lasers_box(const Vec3& pos, const Vec3& size, float angle,
                                bool spawn_items, bool protection) {
    for (size_t i = 0; i < lasers.size();) {
        const bool had_next = i + 1 < lasers.size();
        if (lasers[i].phase != 1 && lasers[i].has_updated)
            split_laser(i, pos, size, angle, false, spawn_items, protection);
        if (!had_next) break;
        ++i;
    }
}

void StageSim::cancel_lasers(bool reset_protection) {
    // 430220 clears protection before invoking every infinite laser's
    // full cancellation method434940. No item spawn occurs in this method.
    for (auto& laser : lasers) {
        if (!reset_protection && laser.phase == 1) continue; // 430300 skip gate
        if (reset_protection) { laser.cancel_protection = 0; laser.cull_timer = 0; }
        if (!cancel_laser(laser)) return;
    }
}

bool StageSim::cancel_laser(StageLaser& laser) {
        if(laser.kind==StageLaser::Kind::Beam) {laser.pending_remove=true;return true;} // 438050(mode 0)
        if(laser.kind==StageLaser::Kind::Curve) {
            // 437b80: no screen cull; sample0,3,6... even on a short curve.
            for(size_t i=0;i<laser.curve_samples.size();i+=3)
                if(!laser_effect(laser,laser.curve_samples[i].position))return false;
            laser.phase=1;return true;
        }
        // 432e00 / 434940, mode arguments both zero: bypass protection.
        // Infinite cancellation culls particles to the playfield; line does not.
        float dx, dy;
        polar_xy(laser.angle, 8.0f, dx, dy);
        Vec3 pos{laser.origin[0] + dx, laser.origin[1] + dy, laser.origin[2]};
        dx += dx; dy += dy;
        if (laser.length > 16.0f) {
            float distance = 8;
            do {
                if ((laser.kind == StageLaser::Kind::Line || on_screen(pos, 16)) &&
                    !laser_effect(laser, pos)) return false;
                pos.x += dx; pos.y += dy;
                distance += 16;
            } while (double(distance) + 8 < double(laser.length));
        }
        laser.phase = 1; // manager removes it on its next traversal
        return true;
}

} // namespace th13
