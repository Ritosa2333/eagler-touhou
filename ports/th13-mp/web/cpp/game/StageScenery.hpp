#pragma once
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "StageResource.hpp"
#include "StageScript.hpp"
#include "Timer.hpp"
#include <vector>
namespace th13 {
class StageScenery {
public:
    StageResource file;
    StageScript script{file};
    std::string error;
    u32 frames = 0;
    bool ready() const noexcept { return initialized; }
    void begin_exit() noexcept;
    void begin_enter() noexcept;
    bool retired() const noexcept { return (lifecycle_flags & 8u) != 0; }
    bool draw_camera_enabled() const noexcept;
    bool draw_geometry_enabled() const noexcept { return !retired() && (lifecycle_flags & 1u) != 0; }
    u32 draw_clear_color() const noexcept;
    bool finish_draw_pass(i32 pass) noexcept;
    u32 take_draw_tint() noexcept;
    bool fade_in_requested() const noexcept { return fade_in_pending; }
    void clear_fade_in_request() noexcept { fade_in_pending = false; }
    const StageCamera& camera() const noexcept { return script.state.camera; }
    const Vec3& camera_direction() const noexcept { return view_direction; }
    bool initialize(AnmManager& animations, i32 bank);
    bool update(AnmManager& animations, AnmEnvironment& environment, float rate = 1);
    bool draw(AnmManager& animations, AnmRenderer& renderer, ZunGraphics& graphics, i32 layer_first, i32 layer_last,
              const GraphicsViewport& viewport, Vec2 cull_extent, Vec2 screen_shake = {});
    bool interrupt_objects(i32 label);
    struct ExtraSnapshot {
        std::vector<AnmVm> primitives;
        std::array<AnmVm, 8> background_slots{};
        std::vector<u8> object_flags;
        std::vector<i16> instance_flags;
        u32 lifecycle_flags = 1;
        Timer lifecycle_timer{0, 0, 0, 0, 0};
        bool fade_in_pending = false;
        u32 transition_color = 0x00808080u;
        float rate = 1;
        Vec3 view_direction{};
        u32 frames = 0;
    };
    ExtraSnapshot capture_extra() const;
    void restore_extra(const ExtraSnapshot& source);

private:
    std::vector<AnmVm> primitives;
    std::array<AnmVm, 8> background_slots{};
    std::vector<u8> object_flags;
    std::vector<i16> instance_flags;
    bool initialized = false;
    u32 lifecycle_flags = 1;
    Timer lifecycle_timer{0, 0, 0, 0, 0};
    bool fade_in_pending = false;
    u32 transition_color = 0x00808080u;
    float rate = 1;
    Vec3 view_direction{};
    bool bind(AnmVm&, AnmManager&, i32 bank, i32 script_index);
    bool fail(const std::string& message) {
        if (error.empty()) error = message;
        return false;
    }
};
}
