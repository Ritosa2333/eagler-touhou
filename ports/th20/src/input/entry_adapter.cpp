#include "input.hpp"
#include "../platform_window/platform_window.hpp"
#include "../program_entry/unrecovered_dependencies.hpp"
#include <optional>
#include <stdexcept>

namespace th20::source::input {
Controller* create_game_controller() {
    namespace pe=program_entry;
    if(!pe::function_controller) throw std::logic_error("Input construction requires the recovered scheduler");
    if(controller) throw std::logic_error("Input controller already exists");
    // Rebuilt at each creation to capture the actual current HWND/HINSTANCE;
    // all mutable service/configuration storage is shared by reference.
    static std::optional<ControllerContext> context;
    context.emplace(ControllerContext{pe::window_state.instance,pe::window_state.window,pe::window_state.active,
        pe::graphics_state.configuration,pe::graphics_state.input_device_caps,pe::log_buffer,
        shared_state(),win32_host(),*pe::function_controller,pe::scheduler_environment});
    return create_controller(*context);
}
void sample_game_frame() {
    if(!controller) throw std::logic_error("Input frame requires the recovered controller");
    controller->sample_frame();
}
void destroy_game_controller() { destroy_controller(controller); }
}

namespace th20::source::platform_window {
InputPrefix*& input=th20::source::input::button_slot_storage(2);
namespace unrecovered {
th20::source::input::Controller*& scheduler_object_005b8898=th20::source::input::controller;
void retire_scheduler_object(runtime::CallbackOwner* owner) { runtime::retire_callback_owner(owner); }
void sample_input() {
    th20::source::input::sample_startup_input(th20::source::input::shared_state(),th20::source::input::win32_host());
}
}
}
namespace th20::source::program_entry::unrecovered {
void fn_00420f80() {
    input::probe_legacy_devices(input::shared_state(),input::win32_host(),graphics_event_flags);
}
void fn_00421040() { input::clear_keyboard_high_bits(input::win32_host()); }
}
