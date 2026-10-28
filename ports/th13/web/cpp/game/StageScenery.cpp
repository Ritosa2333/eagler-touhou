// STD 舞台背景实现。绘制语义（th15 StageScene 同族，锚点见文件头）：
//   - 实例平移 = 图元位置 + 实例位置；缩放 = STD 尺寸 / 精灵尺寸（flags|=8
//     触发变换重算）；雾仅 mode 8/24；深度写由 flags 0x2000 取反决定；
//   - 帧内先把上一帧视线归一化再执行脚本（原版快照序）；
//   - 相机快照落 AnmEnvironment（camera_origin/screen_translation/
//     reference_position —— ANM 变量 10016-10021 的数据源）。
#include "StageScenery.hpp"
#include "AnmTransforms.hpp"
#include "SceneMatrices.hpp"
namespace th13 {
namespace {
// 对象包围盒 16 顶点投影裁剪（视口矩形 + 8px 余量；th15 StageVisibility
// 同族——顶点集含面心与边中点，覆盖薄壁/细长对象；th13 3D 取景为全屏）。
bool stage_visible(const StageObject& object, const Vec3& instance, const AnmCamera& camera,
                   const GraphicsViewport& viewport, Vec2 origin, Vec2 extent,
                   float distance_squared) noexcept {
    const Vec3 delta{float(float(object.position.x + instance.x) - camera.eye.x),
                     float(float(object.position.y + instance.y) - camera.eye.y),
                     float(float(object.position.z + instance.z) - camera.eye.z)};
    const float distance =
        float(float(float(delta.y * delta.y) + float(delta.x * delta.x)) + float(delta.z * delta.z));
    if (distance > distance_squared) return false;
    const float hx = float(object.size.x * .5f), hy = float(object.size.y * .5f),
               hz = float(object.size.z * .5f);
    const float x0 = float(object.position.x - hx), x1 = float(object.position.x + hx),
               y0 = float(object.position.y - hy), y1 = float(object.position.y + hy),
               z0 = float(object.position.z - hz), z1 = float(object.position.z + hz),
               x = object.position.x, z = object.position.z;
    const Vec3 boxes[16] = {{x1, y1, z1},           {x1, y1, z0},           {x1, y0, z1},
                            {x1, y0, z0},           {x0, y1, z1},           {x0, y1, z0},
                            {x0, y0, z1},           {x0, y0, z0},           {x, y0, z0},
                            {x, y1, z0},            {x, y0, z1},            {x, y1, z1},
                            {x, y0, z},             {x, y1, z},             {x, y0, float(z - float(hz * .5f))},
                            {x, y1, float(z + float(hz * .5f))}};
    Matrix4 world;
    world.identity();
    world.m[12] = instance.x;
    world.m[13] = instance.y;
    world.m[14] = instance.z;
    Matrix4 screen;
    screen.m[0] = float(double(viewport.width) * .5);
    screen.m[5] = float(double(viewport.height) * -.5);
    screen.m[10] = float(double(viewport.far_depth) - viewport.near_depth);
    screen.m[12] = float(double(viewport.x) + double(viewport.width) * .5);
    screen.m[13] = float(double(viewport.y) + double(viewport.height) * .5);
    screen.m[14] = viewport.near_depth;
    screen.m[15] = 1;
    const auto m = matrix_product(matrix_product(matrix_product(world, camera.view), camera.projection), screen);
    float min_x = float(float(origin.x + extent.x) + 8), max_x = float(origin.x - 8),
          min_y = float(float(origin.y + extent.y) + 8), max_y = float(origin.y - 8);
    for (const auto& box : boxes) {
        float q[4];
        for (u32 c = 0; c < 4; ++c)
            q[c] = float(float(float(float(box.x * m.m[c]) + float(box.y * m.m[4 + c])) +
                               float(box.z * m.m[8 + c])) +
                         m.m[12 + c]);
        const float estimate = float(1.f / q[3]),
                   w = float(float(estimate + estimate) - float(float(estimate * q[3]) * estimate));
        const Vec3 point{float(q[0] * w), float(q[1] * w), float(q[2] * w)};
        if (!(point.z >= 0 && point.z <= 1)) continue;
        if (point.x < min_x) min_x = point.x;
        if (point.x > max_x) max_x = point.x;
        if (point.y < min_y) min_y = point.y;
        if (point.y > max_y) max_y = point.y;
    }
    return max_x >= origin.x && min_x <= float(origin.x + extent.x) && max_y >= origin.y &&
           min_y <= float(origin.y + extent.y);
}
}  // namespace
void StageScenery::begin_exit() noexcept {
    lifecycle_flags = (lifecycle_flags | 2u | 1u) & ~8u;
    lifecycle_timer.set(30);
}
void StageScenery::begin_enter() noexcept {
    lifecycle_flags = (lifecycle_flags | 4u | 1u) & ~8u;
    lifecycle_timer.set(60);
    fade_in_pending = false;
}
bool StageScenery::draw_camera_enabled() const noexcept {
    if (retired()) return false;
    return (lifecycle_flags & 8u) == 0 &&
           ((lifecycle_flags & 4u) == 0 || lifecycle_timer.current < 60);
}
u32 StageScenery::draw_clear_color() const noexcept {
    if ((lifecycle_flags & 4u) != 0 && frames < 34) return 0;
    return script.state.camera.fog.color;
}
u32 StageScenery::take_draw_tint() noexcept {
    const u32 color = transition_color;
    transition_color &= 0x00ffffffu;
    return color;
}
bool StageScenery::finish_draw_pass(i32 pass) noexcept {
    if (retired()) return false;
    if (pass == 2 && (lifecycle_flags & 4u)) {
        if (lifecycle_timer.current < 30) {
            fade_in_pending = true;
            lifecycle_flags |= 1u;
            lifecycle_timer.set(1);
        } else {
            lifecycle_flags &= ~1u;
            transition_color &= 0x00ffffffu;
        }
    }
    if (pass != 5) return true;
    if ((lifecycle_flags & 4u) && lifecycle_timer.current >= 30)
        transition_color &= 0x00ffffffu;
    if (lifecycle_timer.current > 0) {
        lifecycle_timer.decrement(&rate);
        if (lifecycle_timer.current < 1) {
            if (lifecycle_flags & 2u) lifecycle_flags |= 8u;
            lifecycle_flags &= ~6u;
            transition_color = 0x00ffffffu;
        }
    }
    return !retired();
}
bool StageScenery::bind(AnmVm& vm, AnmManager& animations, i32 bank, i32 script_index) {
    if (!animations.bind_template(vm, bank, script_index)) return fail(animations.error);
    vm.rotation_parent = vm.creation_parent = nullptr;
    if (animations.tick_instance(vm) < 0) return fail(animations.error);
    return true;
}
bool StageScenery::initialize(AnmManager& animations, i32 bank) {
    if (initialized) return fail("Stage scenery already initialized");
    if (!file.error.empty()) return fail(file.error);
    if (!animations.resource(bank)) return fail("Stage scenery animation bank unavailable");
    script.state.timer.set(0);
    script.state.culling_distance_squared = 9999999.f;
    script.background_command = [this, &animations, bank](i32 slot, i32 template_index) {
        auto& vm = background_slots[u32(slot)];
        if (template_index < 0) {
            vm.visual.flags &= ~1u;
            vm.visible = false;
            return true;
        }
        return bind(vm, animations, bank, template_index);
    };
    primitives.resize(file.animation_count);
    object_flags.assign(file.objects.size(), 1);
    instance_flags.resize(file.instances.size());
    for (u32 i = 0; i < file.objects.size(); ++i) {
        for (const auto& primitive : file.objects[i].primitives) {
            if (primitive.animation >= primitives.size())
                return fail("Stage scenery primitive index outside pool");
            if (!bind(primitives[primitive.animation], animations, bank, primitive.script))
                return false;
        }
    }
    initialized = true;
    lifecycle_flags = 1;
    lifecycle_timer.set(0);
    return true;
}
bool StageScenery::update(AnmManager& animations, AnmEnvironment& environment, float step_rate) {
    if (!error.empty()) return false;
    if (!initialized) return fail("Stage scenery not initialized");
    if (retired()) return true;
    rate = step_rate;
    animations.rate = rate;
    if ((lifecycle_flags & 4u) && lifecycle_timer.current >= 60) return true;
    transition_color = (transition_color & 0xff000000u) | 0x00808080u;
    script.state.camera.animation_delta = {};
    // 先归一化上一帧视线（含 target_offset）再执行脚本。
    const auto& direction = script.state.camera.direction;
    const auto& offset = script.state.camera.target_offset;
    view_direction = normalize_scene_vector(
        {float(direction.x + offset.x), float(direction.y + offset.y), float(direction.z + offset.z)});
    if ((lifecycle_flags & 4u) && lifecycle_timer.current >= 30) { ++frames; return true; }
    for (u32 i = 0; i < file.objects.size(); ++i) {
        if (!(object_flags[i] & 1)) continue;
        u32 active = 0;
        for (const auto& primitive : file.objects[i].primitives) {
            auto& vm = primitives[primitive.animation];
            if (animations.tick_instance(vm) < 0) return fail(animations.error);
            if (vm.instruction_offset >= 0) active++;
        }
        if (!active) object_flags[i] &= u8(~1u);
    }
    if (!script.step(rate)) return fail(error.empty() ? script.error : error);
    for (auto& vm : background_slots)
        if (vm.resource && animations.tick_instance(vm) < 0) return fail(animations.error);
    environment.camera_origin = script.state.camera.eye_offset;
    environment.screen_translation = script.state.camera.position;
    environment.reference_position = view_direction;
    environment.stage_direction=script.state.camera.direction;
    environment.background_delta=script.state.camera.animation_delta;
    frames++;
    return true;
}
bool StageScenery::interrupt_objects(i32 label) {
    for (auto& vm : primitives) vm.pending_interrupt = label;
    return true;
}
bool StageScenery::draw(AnmManager& animations, AnmRenderer& renderer, ZunGraphics& graphics,
                        i32 layer_first, i32 layer_last, const GraphicsViewport& viewport,
                        Vec2 cull_extent, Vec2 screen_shake) {
    (void)animations;
    if (!error.empty()) return false;
    if (!initialized) return fail("Stage scenery not initialized");
    if (retired() || !draw_geometry_enabled()) return true;
    if (layer_first == 0) {
        bool has_background = false;
        for (const auto& vm : background_slots) has_background |= vm.sprite != nullptr;
        if (has_background) {
            renderer.set_viewport(viewport);
            AnmCamera overlay_camera;
            const float fov = script.state.camera.fov;
            const float distance = float(240.0 / std::tan(double(float(fov * .5f))));
            overlay_camera.eye = {320, 240, distance};
            overlay_camera.view = scene_look_at(overlay_camera.eye, {320, 240, 0}, {0, -1, 0});
            overlay_camera.projection = scene_perspective(fov, 1.3333333730697632f, 1.f, 10000.f);
            renderer.set_camera(overlay_camera);
            renderer.offset = screen_shake;
            renderer.flush();
            graphics.set_fog(false);
            graphics.set_depth_mask(false);
            for (auto& vm : background_slots)
                if (vm.sprite && renderer.draw(vm) == -2) return fail(renderer.error);
            renderer.flush();
            graphics.set_depth_mask(true);
        }
    }
    auto camera_state = script.state.camera;
    camera_state.direction = view_direction;
    const auto camera = stage_camera(camera_state, viewport);
    renderer.scenery_camera=camera;renderer.scenery_viewport=viewport;renderer.scenery_camera_ready=true;
    renderer.set_viewport(viewport);
    renderer.set_camera(camera);
    graphics.presentation_camera(true);
    // 406d90/407310 copy4dcbe8/ec into camera+f4/f8;43ba20 then
    // sets ANM manager+d0/d4. This is a projected/screen-quad offset,
    // not a displacement of the eye, world geometry, or culling volume.
    renderer.offset = screen_shake;
    for (u32 i = 0; i < file.instances.size(); ++i) {
        renderer.presentation_instance = i + 1;
        const auto& instance = file.instances[i];
        const auto& object = file.objects[u32(instance.object)];
        const i32 layer = i32(object.layer);
        if (layer < layer_first || layer > layer_last) continue;
        if (!stage_visible(object, instance.position, camera, viewport, {}, cull_extent,
                           script.state.culling_distance_squared)) {
            object_flags[u32(instance.object)] |= 2;
            instance_flags[i] = i16(instance.flags & ~1);
            continue;
        }
        object_flags[u32(instance.object)] |= 2;
        for (const auto& primitive : object.primitives) {
            if (primitive.type != 0) continue;
            auto& vm = primitives[primitive.animation];
            auto& visual = vm.visual;
            const u32 mode = visual.draw_mode();
            if (mode >= 4) {
                visual.translation = {float(primitive.position.x + instance.position.x),
                                      float(primitive.position.y + instance.position.y),
                                      float(primitive.position.z + instance.position.z)};
                // STD 尺寸 → 精灵缩放（延迟的 ins_300 可能尚未选精灵：仅非零
                // 尺寸查表，与原版 40f99c 行为一致）。
                if (primitive.size.x != 0 || primitive.size.y != 0) {
                    const auto* selected = vm.texture_sprite();
                    if (!selected)
                        return fail("Stage scenery primitive sprite unavailable");
                    const auto& sprite = *selected;
                    if (primitive.size.x != 0) {
                        visual.scale.x = float(primitive.size.x / sprite.width);
                        visual.flags |= 8;
                    }
                    if (primitive.size.y != 0) {
                        visual.scale.y = float(primitive.size.y / sprite.height);
                        visual.flags |= 8;
                    }
                }
            }
            const bool fog = mode == 8 || mode == 24, depth_write = (visual.flags & 0x2000) == 0;
            if (graphics.pipeline().fog != fog || graphics.pipeline().depthWrite != depth_write)
                renderer.flush();
            graphics.set_depth_mask(depth_write);
            graphics.set_fog(fog);
            graphics.set_fog_color(camera.fog_color);
            graphics.set_fog_range(camera.fog_near, camera.fog_far);
            if (renderer.draw(vm) == -2) return fail(renderer.error);
        }
        instance_flags[i] = i16(instance.flags | 1);
    }
    renderer.presentation_instance = 0;
    renderer.flush();
    graphics.set_depth_mask(false);
    return true;
}
}  // namespace th13
