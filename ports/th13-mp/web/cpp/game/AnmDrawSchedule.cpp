#include "AnmDrawSchedule.hpp"
#include <unordered_map>
namespace th13 {
namespace {
using GhostIcons = std::unordered_map<u32, u8>;
std::unordered_map<AnmManager*, GhostIcons> hud_ghost_icons;
}
void AnmDrawSchedule::set_hud_ghost_icons(AnmManager& manager, const std::array<std::array<u32, 21>, 3>& icons,
                                          u8 ghost) {
    auto& selected = hud_ghost_icons[&manager];
    selected.clear();
    for (size_t seat = 0; seat < icons.size(); ++seat)
        if ((ghost >> seat) & 1)
            for (u32 handle : icons[seat])
                if (handle) selected[handle] = 140;
}
const std::array<AnmDrawSchedule::Entry, 31> AnmDrawSchedule::entries{
    {{0, 5},
     {1, 7},
     {2, 9},
     {4, 11},
     {3, 10, 3, true, true, false, false, true},
     {5, 13},
     {6, 16},
     {7, 18},
     {8, 19},
     {9, 20},
     {10, 21},
     {11, 23},
     {12, 26},
     {13, 27},
     {14, 30},
     {15, 31},
     {16, 33},
     {17, 35},
     {18, 39},
     {19, 43},
     {20, 44, 3, false, true, true, true, false, true},
     {21, 54},
     {22, 55},
     {23, 57},
     {26, 69},
     {28, 74},
     {25, 68},
     {24, 56, 2, false, true, true, true, false},
     {27, 61, 0, false, false, false, false, true},
     {32, 70, 0, false, true, true, true, true},
     {31, 71, 2, false, true, true, true, false}}};
AnmDrawSchedule::AnmDrawSchedule(FrameScheduler& f, AnmManager& a, AnmRenderer& r, ZunGraphics& g, AnmDrawServices& s)
    : scheduler(f), animations(a), renderer(r), graphics(g), services(s) {
    for (u32 i = 0; i < callbacks.size(); i++) {
        auto& c = callbacks[i];
        c.owner = this;
        c.index = i;
        c.callback.owner = &c;
        c.callback.enabled = true;
        c.callback.run = [](void* p) {
            auto& c = *static_cast<Callback*>(p);
            return c.owner->draw(c.index) ? i32(FrameAction::Continue) : i32(FrameAction::Error);
        };
        if (scheduler.add(c.callback, FramePass::Draw, entries[i].priority) < 0)
            error = "Animation draw callback registration failed";
    }
}
AnmDrawSchedule::~AnmDrawSchedule() {
    for (auto& c : callbacks)
        scheduler.remove(c.callback);
    hud_ghost_icons.erase(&animations);
}
bool AnmDrawSchedule::draw(u32 index) {
    if (!error.empty()) return false;
    const auto& e = entries[index];
    if (e.camera >= 0) {
        renderer.invalidate();
        if (!services.camera(DrawCamera(e.camera), e.refresh)) {
            error = "Animation camera selection failed";
            return false;
        }
        if (e.disable_fog) graphics.set_fog(false);
        if (e.disable_depth_write) graphics.set_depth_mask(false);
        if (e.disable_fog || e.disable_depth_write) graphics.set_depth_compare(touhou::graphics::Compare::Always);
        if (e.zero_offset) renderer.offset = {};
        if (e.clear_shake) animations.scene_environment().screen_shake_offset = {};
    }
    if (e.layer >= 4 && e.layer <= 20) {
        const auto& env = animations.scene_environment();
        renderer.clip = {u32(truncate_int(float(float(env.screen_width) - 408.f) * .5f)),
                         u32(truncate_int(float(float(env.screen_height) - 476.f) * .5f)),
                         408,
                         476,
                         0,
                         1};
        renderer.clip_set = true;
    } else
        renderer.clip_set = false;
    for (auto* vm : animations.registry.layer(e.layer)) {
        const auto group = hud_ghost_icons.find(&animations);
        const auto handle = animations.registry.handle(*vm);
        const auto opacity = group == hud_ghost_icons.end() ? GhostIcons::const_iterator{} : group->second.find(handle);
        const int result = group != hud_ghost_icons.end() && opacity != group->second.end()
                               ? renderer.draw_with_opacity(*vm, opacity->second)
                               : renderer.draw(*vm);
        if (result == -2) {
            error = renderer.error;
            return false;
        }
    }
    if (e.restore) {
        renderer.invalidate();
        if (!services.camera(DrawCamera::Fullscreen, false)) {
            error = "Animation full-screen camera restoration failed";
            return false;
        }
    }
    return true;
}
}
