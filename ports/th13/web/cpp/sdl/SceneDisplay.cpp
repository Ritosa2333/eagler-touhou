#include "SceneDisplay.hpp"

namespace th13::sdl {

SceneDisplay::SceneDisplay(FrameScheduler& f, AnmManager& a, AnmEnvironment& e, GraphicsDevice& g, AnmRenderer& r,
                           ScreenViews& v, AsciiText& t, i32 ascii)
    : scheduler(f), animations(a), environment(e), graphics(g), renderer(r), views(v), captions(t), ascii_bank(ascii) {}

SceneDisplay::~SceneDisplay() {
    detach();
    caption_drawing.reset();
    animation_drawing.reset();
}

bool SceneDisplay::fail(const std::string& why) {
    if (error.empty()) error = why.empty() ? "Scene display failed" : why;
    return false;
}

bool SceneDisplay::initialize() {
    if (animation_drawing) return fail("Scene display already initialized");
    auto* file = animations.resource(ascii_bank);
    if (!file || !graphics.preload(*file))
        return fail(graphics.error.empty() ? "Common scene animation unavailable" : graphics.error);
    views.configure(environment);
    if (!captions.initialize(ascii_bank)) return fail(captions.error);
    animation_drawing = std::make_unique<AnmDrawSchedule>(scheduler, animations, renderer, graphics, views);
    caption_drawing = std::make_unique<AsciiFrame>(scheduler, captions, renderer, views);
    return animation_drawing->error.empty() || fail("Common scene draw callbacks unavailable");
}

void SceneDisplay::detach() noexcept {
    playfield_active = false;
    renderer.flush();
    graphics.select_target(nullptr, 0);
}

bool SceneDisplay::prepare() {
    if (!error.empty() || !animation_drawing) return false;
    graphics.presentation.begin();
    if (!graphics.select_target(nullptr, 0)) return fail(graphics.error);
    if (!playfield_active) {
        if (!graphics.clear_target(0xff000000, nullptr)) return fail(graphics.error);
        renderer.invalidate();
        renderer.offset = {};
        if (!views.camera(DrawCamera::Fullscreen, false)) return fail("Scene display full-screen camera unavailable");
    }
    if (scheduler.draw() < 0) {
        if (!animation_drawing->error.empty()) return fail(animation_drawing->error);
        if (!captions.error.empty()) return fail(captions.error);
        return fail("Scene draw callbacks failed");
    }
    renderer.flush();
    if (before_present && !before_present()) return fail("Scene present hook failed");
    graphics.presentation.finish();
    return graphics.error.empty() || fail(graphics.error);
}

bool SceneDisplay::present() {
    graphics.present();
    return graphics.error.empty() || fail(graphics.error);
}

bool SceneDisplay::draw() { return prepare() && present(); }

}
