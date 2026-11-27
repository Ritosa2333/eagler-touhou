// th16 画面合成实现——地址见 ScreenCompositor.hpp。
#include "ScreenCompositor.hpp"

#include <cstring>

namespace th16 {

using namespace touhou::graphics;

void ScreenCompositor::init_cameras(float window_scale) {
    // 0x43cb10：UI 相机先按全窗口建好，其余三台由它复制后改视口与 kind
    ui_camera = Camera{};
    ui_camera.viewport = {0, 0, 640, 480};
    ui_camera.kind = Camera::kUi;
    ui_camera.reset_to_viewport_center();

    field_camera = ui_camera;
    field_camera.kind = Camera::kField;
    field_camera.viewport = {i32(window_scale * 32.0f), i32(window_scale * 16.0f), i32(window_scale * 384.0f), i32(window_scale * 448.0f)};
    field_camera.reset_to_viewport_center();

    wide_field_camera = field_camera;
    wide_field_camera.kind = Camera::kWideField;
    wide_field_camera.viewport = {i32(window_scale * 128.0f), i32(window_scale * 16.0f), i32(window_scale * 384.0f), i32(window_scale * 448.0f)};
    wide_field_camera.reset_to_viewport_center();

    *render_target_camera = field_camera;
    render_target_camera->kind = Camera::kRenderTarget;
    render_target_camera->viewport = {i32((640.0f - 408.0f) * 0.5f), i32((480.0f - 472.0f) * 0.5f), 0x198, 0x1d8};
    render_target_camera->reset_to_viewport_center();
}

void ScreenCompositor::create(AnmManager& manager, AnmFile& text) {
    // 0x43d970（640 宽）：RT1→RT2 复制 0x3b、RT2→RT1 复制 0x41、RT1→RT2 二次复制 0x3e、RT2→屏幕 0x44
    static constexpr int kScripts640[kCompositeVmCount] = {0x3b, 0x41, 0x3e, 0x44};
    for (u32 i = 0; i < kCompositeVmCount; ++i) {
        AnmVm& vm = composite_vms_[i];
        AnmManager::reset_vm(vm);
        manager.init_from_template(text, vm, kScripts640[i]);
        vm.parent = nullptr;
        vm.root = nullptr;
        manager.run(vm);
    }
    render_targets_ready = true;
}

void ScreenCompositor::tick(AnmManager& manager) {
    if (!render_targets_ready) return;
    for (AnmVm& vm : composite_vms_) manager.run(vm);
}

void ScreenCompositor::begin_loading(AnmManager& manager) {
    if (!render_targets_ready) return;
    AnmVm& vm = composite_vms_[0];
    if (vm.interrupt_callback != 0 && manager.hooks != nullptr) manager.hooks->interrupted(vm, 2);
    vm.pending_interrupt = 2;
    manager.run(vm);
}

void ScreenCompositor::request_plain_copy() {
    if (!render_targets_ready) return;
    for (AnmVm& vm : composite_vms_) vm.pending_interrupt = 2;   // 0x4173c0（中断回调表在 th16 中为空）
}

void ScreenCompositor::set_color(AnmVm& vm, u32 argb) {
    std::memcpy(vm.color, &argb, 4);
}

ScreenRect ScreenCompositor::viewport_rect(const Camera& camera) const {
    const Viewport& vp = camera.viewport;
    return {vp.x, vp.y, vp.x + vp.width, vp.y + vp.height};
}

void ScreenCompositor::register_draw_chains(ChainManager& chains, AnmRenderer& renderer) {
    struct Node { i32 priority; ChainResult (ScreenCompositor::*pass)(AnmRenderer&); };
    static constexpr Node kNodes[] = {
        {0x01, &ScreenCompositor::pass_begin_frame},
        {0x0e, &ScreenCompositor::pass_select_rt2_clear_depth},
        {0x0f, &ScreenCompositor::pass_copy_rt1_to_rt2},
        {0x19, &ScreenCompositor::pass_select_rt1_clear_depth},
        {0x1a, &ScreenCompositor::pass_copy_rt2_to_rt1},
        {0x2b, &ScreenCompositor::pass_select_rt2_clear_field_depth},
        {0x2c, &ScreenCompositor::pass_copy_rt1_to_rt2_again},
        {0x38, &ScreenCompositor::pass_select_backbuffer},
        {0x39, &ScreenCompositor::pass_present_field},
        {0x55, &ScreenCompositor::pass_end_frame},
    };
    for (const Node& node : kNodes) {
        ChainEntry* entry = chains.create([this, &renderer, pass = node.pass]() { return (this->*pass)(renderer); },
                                          node.priority, false);
        chains.enable(entry);
    }
}

// ---------------------------------------------------------------------------------------------
// 各优先级的合成步骤
// ---------------------------------------------------------------------------------------------

ChainResult ScreenCompositor::pass_begin_frame(AnmRenderer& renderer) {
    RenderDevice* device = renderer.device;
    if (device != nullptr) {
        if (!render_targets_ready) {
            device->clear(kClearColor | kClearDepth, background_color, nullptr);
        } else {
            device->clear(kClearColor | kClearDepth, 0xffffffffu, nullptr);
            device->select_target({kTextSlot, kRenderTarget1Entry});
            const ScreenRect field = viewport_rect(*render_target_camera);
            device->clear(kClearColor | kClearDepth, background_color, &field);
        }
    }
    // DAT_004d9d44/40 = 384/448、DAT_004d9d50/54 = (w/2, (h-448)/2)：场地偏移常量，AnmGeometry 已持同值
    renderer.reset_state_cache();
    renderer.use_camera(ui_camera);
    return ChainResult::Continue;
}

ChainResult ScreenCompositor::pass_select_rt2_clear_depth(AnmRenderer& renderer) {
    if (!render_targets_ready || renderer.device == nullptr) return ChainResult::Continue;
    renderer.flush();
    renderer.device->select_target({kTextSlot, kRenderTarget2Entry});
    const ScreenRect field = viewport_rect(*render_target_camera);
    renderer.device->clear(kClearDepth, background_color, &field);
    return ChainResult::Continue;
}

void ScreenCompositor::copy_with_vm(AnmRenderer& renderer, AnmVm& vm, bool use_render_target_camera, int extra_layer) {
    RenderDevice& device = *renderer.device;
    // 直接下发、不走缓存：BLENDOP ADD、关 alpha test
    device.pipeline().blendEquation = BlendEquation::Add;
    device.pipeline().alphaTest = false;
    renderer.set_depth_write(false);
    if (use_render_target_camera) renderer.use_camera(*render_target_camera);
    renderer.draw_vm(vm);
    set_color(vm, 0xffffffffu);
    if (extra_layer >= 0) renderer.draw_layer(extra_layer);
    renderer.flush();
    device.pipeline().alphaTest = true;
}

ChainResult ScreenCompositor::pass_copy_rt1_to_rt2(AnmRenderer& renderer) {
    // DAT_004a6eec（自定义合成回调）在 th16 中始终为 0
    if (!render_targets_ready || renderer.device == nullptr) return ChainResult::Continue;
    copy_with_vm(renderer, composite_vms_[0], true, 0x22);
    return ChainResult::Continue;
}

ChainResult ScreenCompositor::pass_select_rt1_clear_depth(AnmRenderer& renderer) {
    if (!render_targets_ready || renderer.device == nullptr) return ChainResult::Continue;
    renderer.flush();
    renderer.device->select_target({kTextSlot, kRenderTarget1Entry});
    const ScreenRect field = viewport_rect(*render_target_camera);
    renderer.device->clear(kClearDepth, background_color, &field);
    return ChainResult::Continue;
}

ChainResult ScreenCompositor::pass_copy_rt2_to_rt1(AnmRenderer& renderer) {
    if (!render_targets_ready || renderer.device == nullptr) return ChainResult::Continue;
    copy_with_vm(renderer, composite_vms_[1], true, -1);
    return ChainResult::Continue;
}

ChainResult ScreenCompositor::pass_select_rt2_clear_field_depth(AnmRenderer& renderer) {
    if (!render_targets_ready || renderer.device == nullptr) return ChainResult::Continue;
    renderer.flush();
    renderer.device->select_target({kTextSlot, kRenderTarget2Entry});
    const ScreenRect field = viewport_rect(wide_field_camera);
    renderer.device->clear(kClearDepth, background_color, &field);
    return ChainResult::Continue;
}

ChainResult ScreenCompositor::pass_copy_rt1_to_rt2_again(AnmRenderer& renderer) {
    if (!render_targets_ready || renderer.device == nullptr) return ChainResult::Continue;
    copy_with_vm(renderer, composite_vms_[2], false, -1);
    return ChainResult::Continue;
}

ChainResult ScreenCompositor::pass_select_backbuffer(AnmRenderer& renderer) {
    if (renderer.device == nullptr) return ChainResult::Continue;
    renderer.flush();
    renderer.device->select_target(TextureRef::backbuffer());
    renderer.device->clear(kClearColor | kClearDepth, 0xff000000u, nullptr);
    return ChainResult::Continue;
}

ChainResult ScreenCompositor::pass_present_field(AnmRenderer& renderer) {
    if (!render_targets_ready || renderer.device == nullptr) return ChainResult::Continue;
    RenderDevice& device = *renderer.device;
    device.pipeline().alphaTest = false;
    renderer.draw_vm(composite_vms_[3]);
    set_color(composite_vms_[3], 0xffffffffu);
    renderer.flush();
    device.pipeline().alphaTest = true;
    return ChainResult::Continue;
}

ChainResult ScreenCompositor::pass_end_frame(AnmRenderer& renderer) {
    renderer.flush();
    // offset = DAT_004c1788/8c（关卡相机抖动），offset_scaled = DAT_004c1530/34（宽场地相机 +0xfc/+0x100）
    if (screen_shake != nullptr) *screen_shake = ScreenShakeOffset{};
    return ChainResult::Continue;
}

}  // namespace th16
