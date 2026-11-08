#include "PlayfieldTargets.hpp"

namespace th13 {

PlayfieldTargets::PlayfieldTargets(FrameScheduler& f, AnmManager& a, AnmRenderer& r,
                                 ZunGraphics& g, ScreenViews& v)
    : scheduler(f), animations(a), renderer(r), graphics(g), views(v) {}

PlayfieldTargets::~PlayfieldTargets() {
    scheduler.remove(updating);
    for (auto& callback : drawing) scheduler.remove(callback.frame);
    renderer.invalidate();
    graphics.select_target(nullptr, 0);
}

bool PlayfieldTargets::fail(const std::string& why) {
    if (error.empty()) error = why.empty() ? "Playfield render target operation failed" : why;
    return false;
}

bool PlayfieldTargets::initialize(i32 bank) {
    if (resource) return fail("Playfield render targets already initialized");
    resource = animations.resource(bank);
    if (!resource) return fail("Playfield text animation bank unavailable");
    constexpr i32 scripts[] = {82, 83, 82, 84};
    // 原版四个监督者 VM（43a040 分配的 dc85c/dc860/dc864/dc868）的层字段
    // 依次为 29/30/30/20——82 第二实例与 script84 的层是监督者覆写（脚本默认
    // 82→29、84→30；battle_oracle r4/oracle-replay replay121 全窗层标实测：
    // f0 [L29,L30,L30,L20]、f1+ [L29,…,L30,L30,…,L20] 恒定）。
    constexpr i32 supervisor_layers[] = {29, 30, 30, 20};
    for (u32 i = 0; i < compositors.size(); ++i) {
        auto& vm = compositors[i];
        if (!animations.bind_template(vm, bank, scripts[i]) ||
            animations.tick_instance(vm) < 0)
            return fail(animations.error);
        vm.visual.layer = supervisor_layers[i];
        // These four original supervisor-owned VMs are not registry-layer
        // objects. They must not also be drawn by layers29/30.
        if (!vm.sprite || vm.sprite->texture >= resource->textures.size() ||
            resource->textures[vm.sprite->texture].kind != AnmTexture::Kind::RenderTarget)
            return fail("Playfield compositor does not reference an actual render target");
    }
    targets = {compositors[0].sprite->texture, compositors[1].sprite->texture};
    if (targets[0] == targets[1] || compositors[2].sprite->texture != targets[0] ||
        compositors[3].sprite->texture != targets[1])
        return fail("Playfield compositor ping-pong surfaces do not match");

    // 0043a440: 43c4b0 / 43c600 / 43c870 / 43c6a0 / 43c920 /
    // 43c740 / 43c9d0 / 43c7e0 / 43ca50 respectively.
    constexpr i32 priorities[] = {1, 12, 14, 38, 40, 41, 42, 52, 53};
    for (u32 i = 0; i < drawing.size(); ++i) {
        auto& callback = drawing[i];
        callback.owner = this;
        callback.index = i;
        callback.frame.owner = &callback;
        callback.frame.enabled = true;
        callback.frame.run = [](void* value) -> i32 {
            auto& callback = *static_cast<Callback*>(value);
            return callback.owner->draw(callback.index) ? i32(FrameAction::Continue)
                                                         : i32(FrameAction::Error);
        };
        if (scheduler.add(callback.frame, FramePass::Draw, priorities[i]) < 0)
            return fail("Playfield target draw callback registration failed");
    }
    updating.owner = this;
    updating.enabled = true;
    updating.run = [](void* value) -> i32 {
        return static_cast<PlayfieldTargets*>(value)->update() ? i32(FrameAction::Continue)
                                                              : i32(FrameAction::Error);
    };
    if (scheduler.add(updating, FramePass::Update, 1) < 0)
        return fail("Playfield compositor update callback registration failed");
    return true;
}

bool PlayfieldTargets::update() {
    // 层字段为监督者所有（原版两实例同脚本异层实证脚本不覆写；tick 后保持钉住，
    // 与 dc85c/dc860/dc864/dc868 的常驻层一致）。
    constexpr i32 supervisor_layers[] = {29, 30, 30, 20};
    for (u32 i = 0; i < compositors.size(); ++i) {
        if (animations.tick_instance(compositors[i]) < 0) return fail(animations.error);
        compositors[i].visual.layer = supervisor_layers[i];
    }
    return true;
}

bool PlayfieldTargets::select(u32 target, bool clear) {
    renderer.invalidate();
    if (!graphics.select_target(target < targets.size() ? resource : nullptr,
                                target < targets.size() ? targets[target] : 0))
        return fail("Playfield target selection failed");
    if (!views.camera(DrawCamera::Fullscreen, false))
        return fail("Playfield target full-screen camera unavailable");
    if (clear) {
        // 0043bbc0's third viewport bounds the intermediate game surfaces,
        // including the 12px effect margin around the central384x448 crop.
        const auto& region = views.view(DrawCamera::Playfield).viewport;
        if (!graphics.clear_target(0xff000000, target < targets.size() ? &region : nullptr))
            return fail("Playfield target clear failed");
    }
    return true;
}

bool PlayfieldTargets::compose(u32 index) {
    renderer.invalidate();
    if (!views.camera(DrawCamera::Fullscreen, false))
        return fail("Playfield compositor full-screen camera unavailable");
    graphics.set_fog(false);
    graphics.set_depth_test(false);
    graphics.set_depth_mask(false);
    // The unmodified ANM scripts select texture, UV crop, anchor, destination
    // and blend mode. The draw and its resource reads use the normal production
    // renderer/submission path, equally with tracing on and off.
    if (renderer.draw(compositors[index]) == -2) return fail(renderer.error);
    // 0043c870/0043c920 draw the registered distortion strips immediately
    // after their compositor, while the opposite @R surface is still bound.
    if (index < 2 && !renderer.draw_layer(animations.registry.layer(29 + index)))
        return fail(renderer.error);
    renderer.flush();
    return true;
}

bool PlayfieldTargets::draw(u32 index) {
    if (!error.empty()) return false;
    switch (index) {
    case 0:
        if (!select(2, false) || !graphics.clear_target(0xffffffff, nullptr))
            return fail("Playfield screen initial clear failed");
        return select(0, true);
    case 7:
        // 0043c7e0（优先级 52）：切回后台缓冲后 Clear(count=0, rect=NULL,
        // flags=3, 0xff000000) —— 整屏全清。{116,2,408,476} 只是 43c600/
        // 43c6a0/43c740 对中间 @R 的清除区，不适用于最终屏幕；区域清屏会把
        // 116..524 之外的屏幕内容（如先绘制的层）留成未定义底色。
        if (!select(2, false) || !graphics.clear_target(0xff000000, nullptr))
            return fail("Playfield screen final clear failed");
        return true;
    case 1: return select(1, true);
    case 2: return compose(0);
    case 3: return select(0, true);
    case 4: return compose(1);
    case 5: return select(1, true);
    case 6: return compose(2);
    case 8: return compose(3);
    default: return fail("Playfield target pass outside schedule");
    }
}

}  // namespace th13
