#include "LaserPresentation.hpp"
#include "StageMath.hpp"
#include "CurveLaserGeometry.hpp"

namespace th13 {
LaserPresentation::LaserPresentation(AnmManager& a, AnmRenderer& r, StageSim& s, i32 b)
    : animations(a), renderer(r), sim(s), bank(b) {
    sim.laser_birth = [this](StageLaser& laser) { return birth(laser); };
    sim.laser_animation = [this](StageLaser& laser) { return update(laser); };
    sim.laser_retire = [this](u32 generation) { retire(generation); };
    sim.laser_cancel_effect = [this](i32 script, const Vec3& position) {
        return animations.create(bank, script, -1, 0, position, 0, false) != 0 ||
               fail(animations.error);
    };
}
LaserPresentation::~LaserPresentation() {
    sim.laser_birth = {}; sim.laser_animation = {}; sim.laser_retire = {};
    sim.laser_cancel_effect = {};
    while (!visuals.empty()) retire(visuals.begin()->first);
}
bool LaserPresentation::fail(const std::string& message) {
    if (error.empty()) error = message;
    return false;
}
bool LaserPresentation::tick(AnmVm& vm) {
    return vm.tick(animations.random_source(), 1) >= 0 || fail(vm.error);
}
bool LaserPresentation::birth(StageLaser& laser) {
    if (laser.kind == StageLaser::Kind::Beam) return true; // 437d70 binds no script.
    const auto* appearance = bullet_appearance(i32(laser.sprite));
    auto* resource = animations.resource(bank);
    if (!resource || !appearance || laser.subtype >= 16)
        return fail("Infinite laser bullet animation unavailable");
    auto visual = std::make_unique<Visual>();
    visual->source.type = i32(laser.sprite);
    visual->source.color = i32(laser.subtype);
    visual->source.curve = laser.kind==StageLaser::Kind::Curve;
    auto& body = visual->body;
    body.environment = &animations.scene_environment();
    body.object_host = &animations;
    // 433260 -> 405010 + 461cb0: fresh VM/script bind, source callback
    // installed BEFORE its first tick, then immediate interrupt2 tick.
    const i32 body_script=visual->source.curve?113+i32(laser.sprite):appearance->script;
    if (!body.bind(*resource, u32(body_script))) return fail(body.error);
    body.sprite_source = &visual->source;
    if (!tick(body)) return false;
    body.pending_interrupt = 2;
    if (!tick(body)) return false;
    body.visual.flags = (body.visual.flags & 0xfffffe3fu) | 0x20u;
    body.visual.render_flags |= 0x40000;
    body.visual.flags = (body.visual.flags & 0xc31fffffu) | 0x03000000u;
    auto& orb = visual->orb;
    // 403800 copies the already prepared bank template, unlike the body.
    if (!animations.bind_template(orb, bank, i32(laser.subtype) + 56))
        return fail(animations.error);
    if (!tick(orb)) return false;
    orb.pending_interrupt = 2;
    if (!tick(orb)) return false;
    orb.visual.flags = (orb.visual.flags & 0xfffffe3fu) | 0x20u;
    orb.visual.render_flags |= 0x40000;
    orb.visual.flags = (orb.visual.flags & 0xc3ffffffu) | 0x02000000u;
    if (laser.kind == StageLaser::Kind::Line) {
        // 4304d9: the moving end has its own template, color+82, one tick.
        if (!animations.bind_template(visual->tip, bank, i32(laser.subtype) + 82))
            return fail(animations.error);
        if (!tick(visual->tip)) return false;
        visual->tip.visual.render_flags |= 0x40000;
    }
    laser.anm_flags = body.visual.flags;
    visuals.emplace(laser.generation, std::move(visual));
    return true;
}
bool LaserPresentation::update(StageLaser& laser) {
    if (laser.kind == StageLaser::Kind::Beam) return true;
    const auto found = visuals.find(laser.generation);
    if (found == visuals.end()) return fail("Infinite laser animation birth was not observed");
    auto& visual = *found->second;
    auto& body = visual.body;
    if(laser.kind==StageLaser::Kind::Curve) {
        // 4355f0 only advances the orb. The body supplies the strip's
        // texture/material; it is not a stretched bullet quad.
        body.visual.flags=laser.anm_flags;
        return tick(visual.orb);
    }
    if (!body.sprite || body.sprite->height == 0 || body.sprite->width == 0)
        return fail("Infinite laser sprite dimensions unavailable");
    // 43374c: read pre-tick sprite dimensions, set scales, then tick. ANM
    // is allowed to modify those scales; drawing consumes that resulting VM.
    body.visual.flags = laser.anm_flags;
    // Sprite metadata +38 is width, +34 is height (461a60 copies those
    // into VM+80/+84). Do not confuse metadata offsets with x/y order.
    body.visual.scale.x = laser.width / body.sprite->width;
    body.visual.scale.y = laser.length / body.sprite->height;
    if (!tick(body)) return false;
    laser.anm_flags = body.visual.flags;
    if (laser.start_offset == 0 && !tick(visual.orb)) return false;
    if (laser.kind == StageLaser::Kind::Line && !tick(visual.tip)) return false;
    return true;
}
void LaserPresentation::retire(u32 generation) {
    const auto found = visuals.find(generation);
    if (found == visuals.end()) return;
    animations.release_tree(found->second->body);
    animations.release_tree(found->second->orb);
    animations.release_tree(found->second->tip);
    visuals.erase(found);
}
bool LaserPresentation::draw() {
    if (!error.empty()) return false;
    for (const auto& laser : sim.lasers) {
        if (!laser.active) continue;
        if (laser.kind == StageLaser::Kind::Beam) continue; // 438010: no-op draw.
        const auto found = visuals.find(laser.generation);
        if (found == visuals.end()) return fail("Infinite laser draw has no animation");
        auto& visual = *found->second;
        auto& body = visual.body;
        if(laser.kind==StageLaser::Kind::Curve) {
            auto& vertices=body.geometry.screen_vertices;
            curve_laser_strip(laser.curve_samples,laser.target_width,
                              body.visual.uv[0].y,body.visual.uv[2].y,vertices);
            if(renderer.draw_screen_strip(body,vertices.data(),u32(vertices.size()))==-2)
                return fail(renderer.error);
            if(laser.time<=i32(laser.curve_samples.size())&&!laser.curve_samples.empty()) {
                visual.orb.variables.position=laser.curve_samples.back().position;
                if(renderer.draw(visual.orb)==-2)return fail(renderer.error);
            }
            continue;
        }
        // 4337c0 writes VM+3c (position), not the +574 translation field.
        body.variables.position = {laser.origin[0], laser.origin[1], laser.origin[2]};
        body.variables.rotation.z = wrap_angle(float(double(laser.angle) + double(1.5707964f)));
        body.visual.flags |= 4;
        if (renderer.draw(body) == -2) return fail(renderer.error);
        if (laser.kind == StageLaser::Kind::Line) {
            float dx, dy;
            polar_xy(laser.angle, laser.length, dx, dy);
            visual.tip.variables.position = {laser.origin[0] + dx, laser.origin[1] + dy, 0};
            if (renderer.draw(visual.tip) == -2) return fail(renderer.error);
        }
        if (laser.start_offset == 0) {
            visual.orb.variables.position = body.variables.position;
            if (renderer.draw(visual.orb) == -2) return fail(renderer.error);
        }
    }
    return true;
}
const AnmVm* LaserPresentation::body(u32 generation) const {
    const auto it = visuals.find(generation);
    return it == visuals.end() ? nullptr : &it->second->body;
}
const AnmVm* LaserPresentation::orb(u32 generation) const {
    const auto it = visuals.find(generation);
    return it == visuals.end() ? nullptr : &it->second->orb;
}
const AnmVm* LaserPresentation::tip(u32 generation) const {
    const auto it = visuals.find(generation);
    return it == visuals.end() ? nullptr : &it->second->tip;
}
}
