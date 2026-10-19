// Session assembly and lifecycle exports for the TH_SDL3 build. The open
// sequence mirrors recovered_win_main's initialization order (minus the Win32
// window, settings dialog, IME/cursor/screen-saver and message pump), so the
// game boots through the same recovered call chain. The browser frame loop
// lives in ApplicationHost.cpp.
#include <cstdio>
#include "../../../src/program_entry/program_entry.hpp"
#include "../../../src/program_entry/unrecovered_dependencies.hpp"
#include "../../../src/program_entry/text_constants.hpp"
#include "../../../src/platform_services/services.hpp"
#include "../../../src/platform_window/platform_window.hpp"
#include "../../../src/title_system/title.hpp"
#include "../../../src/gameplay/gameplay.hpp"
#include "../../../src/player_entity/owner.hpp"
#include "../../../src/pause_system/pause.hpp"
#include "../../../src/stone_menu/stone.hpp"
#include "../platform/Files.hpp"
#include "../platform/Time.hpp"
#include "../platform/Audio.hpp"
#include "../platform/Graphics.hpp"
#include "HitboxDisplay.hpp"
#include <cstring>
#include <exception>
#include <memory>
#include <algorithm>

namespace th20::source::platform_window {
// Title language follows the launcher configure language, not the OS locale.
bool is_japanese_user_locale() { return web::files::save_root().find("/jp") != std::string::npos; }
}

namespace th20::source::input {
void initialize_input_host();
void shutdown_input_host();
}

namespace pe = th20::source::program_entry;
namespace u = th20::source::program_entry::unrecovered;

using th20::source::program_entry::FunctionController;
using th20::source::program_entry::SpriteController;

namespace {
struct Session {
    pe::unrecovered::RuntimeListenerStorage listener;
    void* allocation = nullptr;
    bool open = false;
    ~Session();
};
std::unique_ptr<Session> session;
bool japanese = true;

void teardown(bool quit) {
    auto& g = pe::graphics_state;
    auto& config = g.configuration;
    config.saved_display_mode = pe::display_mode(pe::window_state);
    u::fn_004dd490(g);
    u::free_function_controller(pe::allocations, pe::function_controller);
    pe::function_controller = nullptr;
    u::fn_00426170(pe::thread_registry);
    u::free_sprite_controller(pe::allocations, pe::sprite_controller);
    pe::sprite_controller = nullptr;
    g.resource_019c = nullptr;
    g.resource_01a0 = nullptr;
    g.resource_01a4 = nullptr;
    pe::release_device(g);
    u::save_configuration(config);
    u::log_flush(pe::log_buffer);
    (void)quit;
}
}
Session::~Session() {
    th20::web::clear_hitbox_display();
    if (!open) return;
    open = false;
    teardown(true);
    th20::source::input::shutdown_input_host();
    th20::web::audio::shutdown();
    if (allocation) u::destroy_allocations(pe::allocations, 1);
    pe::allocations = nullptr;
    u::lock_registry_disable(pe::lock_registry);
    u::destroy_listener(listener);
}

extern "C" {
void sdl_loop_start();
void sdl_loop_stop();
void sdl_loop_pause(std::uint32_t);

namespace {
std::uint32_t game_open_impl(std::uint32_t chinese) {
    if (session) return 0;
    japanese = !chinese;
    th20::web::files::set_language(chinese ? "chs" : "jp");
    auto owned = std::make_unique<Session>();
    auto& w = pe::window_state;
    auto& g = pe::graphics_state;
    w.instance = nullptr;
    w.startup_status = 0;
    u::set_rounding_mode(0);
    u::lock_registry_enable(pe::lock_registry);
    owned->allocation = u::allocate(8);
    pe::allocations = owned->allocation ? u::construct_allocations(owned->allocation) : nullptr;
    u::construct_listener(owned->listener);
    u::install_listener(&owned->listener);
    u::log_append(pe::log_buffer, pe::text_constants::start);
    // No single-instance mutex, startup settings dialog or Win32 window on the
    // browser runtime; configuration arrives via the launcher configure step.
    u::fn_004117a0(g, nullptr);
    if (u::fn_0041b4f0(w) != 0) { u::log_error(pe::log_buffer, pe::text_constants::cannot_save); std::fprintf(stderr,"sdl_game_open: save-directory setup failed\n"); return 0; }
    if (u::load_configuration(g, "th20.cfg") != 0) { std::fprintf(stderr,"sdl_game_open: load_configuration failed\n"); return 0; }
    th20::source::input::initialize_input_host();
    u::fn_00420f80();
    u::fn_00421040();
    w.display_mode = g.configuration.saved_display_mode;
    u::fn_00416d20();
    if (u::fn_0041c320() != 0) { std::fprintf(stderr,"sdl_game_open: create_direct3d failed\n"); return 0; }
    if (u::create_window(w, nullptr) != 0) { std::fprintf(stderr,"sdl_game_open: create_window failed\n"); return 0; }
    if (u::fn_0041c3e0(w) != 0) { std::fprintf(stderr,"sdl_game_open: prepare_presentation failed\n"); return 0; }
    pe::function_controller = u::make_function_controller(pe::allocations, pe::text_constants::function_allocation_site);
    pe::sprite_controller = u::make_sprite_controller(pe::allocations, pe::text_constants::sprite_allocation_site);
    w.clock_offset = 0.0;
    w.next_update_time = u::read_clock(w);
    w.current_time = w.next_update_time;
    w.previous_time = w.current_time;
    w.current_draw_time = u::read_clock(w);
    w.previous_draw_time = w.current_draw_time;
    const int loop_status = u::fn_004de1f0();        // register_graphics_callbacks
    if (loop_status != 0) { std::fprintf(stderr,"sdl_game_open: register_graphics_callbacks failed %d\n",loop_status); return 0; }
    pe::set_draw_counter(w, 0xfc);
    w.flags |= 1u;
    owned->open = true;
    session = std::move(owned);
    return 1;
}
} // namespace

__attribute__((export_name("sdl_game_open"))) std::uint32_t sdl_game_open(std::uint32_t chinese, std::uint32_t /*seed*/) {
    try {
        return game_open_impl(chinese);
    } catch (const std::exception& e) {
        std::fprintf(stderr, "sdl_game_open: exception: %s\n", e.what());
    } catch (...) {
        std::fprintf(stderr, "sdl_game_open: unknown exception\n");
    }
    return 0;
}
__attribute__((export_name("sdl_game_close"))) void sdl_game_close() { th20::web::clear_hitbox_display();session.reset(); }
__attribute__((export_name("sdl_game_language"))) std::uint32_t sdl_game_language() { return japanese ? 0 : 1; }

__attribute__((export_name("application_save"))) void application_save(std::uint32_t) {
    if (!session) return;
    u::save_configuration(pe::graphics_state.configuration);
}
__attribute__((export_name("application_error"))) std::uint32_t application_error(std::uint32_t) {
    // No fatal-error channel exists in the recovered runtime; the log carries
    // diagnostics. Zero means "no fatal error" here, not a hidden failure.
    return 0;
}
__attribute__((export_name("graphics_allocate"))) void* graphics_allocate(std::uint32_t bytes) {
    return std::malloc(bytes ? bytes : 1);
}
__attribute__((export_name("graphics_free"))) void graphics_free(void* p) { std::free(p); }
__attribute__((export_name("sdl_game_status"))) const std::int32_t* sdl_game_status() {
    static std::int32_t out[10]{};
    out[0] = pe::graphics_state.field_0b0c;              // scene
    out[1] = 0;                                          // stage (session-owned; read-only consumers tolerate 0)
    out[2] = 0;                                          // error channel (see application_error)
    // Slots 3..8 were previously constant zero. They now report the title
    // menu's read-only navigation position so a browser regression check can
    // drive the menu deterministically instead of guessing from the picture.
    const auto* title = th20::source::title::controller();
    out[3] = title ? title->state : -1;                   // title page/state
    out[4] = title ? title->phase : -1;                   // title phase within the page
    out[5] = out[6] = 0;                                  // touch context (InputHost owns gestures)
    out[7] = title ? title->cursor.current : -1;          // title list cursor
    out[8] = title ? title->cursor108.current : -1;       // title page cursor
    const auto* buttons = th20::source::input::button_slot(0);
    out[9] = buttons ? std::int32_t(buttons->current) : -1;
    return out;
}
__attribute__((export_name("sdl_resource_stats"))) const std::uint32_t* sdl_resource_stats() {
    static std::uint32_t out[4]{};
    return out; // file layer keeps no decoded cache; counters are honestly zero
}
__attribute__((export_name("th20_ui_observation_v1"))) const std::int32_t* th20_ui_observation_v1() {
    using namespace th20::source;
    // i32 layout: version, validMask, pauseState, pauseSubstate, pauseCursor,
    // retryConfirmInputReady, stoneState, stoneVisible, stoneCategory.
    // validMask bit 0 covers words 2..5; bit 1 covers words 6..8.
    static std::int32_t out[9];
    std::fill(out,out+9,-1);
    out[0]=1;out[1]=0;
    if(const auto* paused=pause::controller()){
        out[1]|=1;out[2]=paused->state;out[3]=paused->substate;out[4]=paused->cursor.current;
        out[5]=paused->state==1&&paused->substate==7&&paused->age.current>=30.f?1:0;
    }
    if(const auto* stones=stone_menu::controller){
        out[1]|=2;out[6]=stones->state;out[7]=stones->visible;out[8]=stones->category.current;
    }
    return out;
}
__attribute__((export_name("th20_native_observation_v1"))) const std::uint32_t* th20_native_observation_v1() {
    using namespace th20::source;
    static std::uint32_t out[12];std::fill(out,out+12,~0u);
    out[0]=1;out[1]=1;out[2]=title::controller()?0:6;out[8]=out[9]=0x7fc00000u;
    if(const auto* buttons=input::button_slot(0)){out[1]|=2;out[3]=buttons->current;}
    if(const auto* game=gameplay::controller){
        const auto* paused=pause::controller();const int state=paused?paused->state:0;
        out[1]|=0x20;out[6]=state==1;out[2]=(game->game_flags&4u)?7:out[6]?2:state>=2?3:1;
        if(game->frame_timer.current>=0){out[1]|=0x18;out[4]=out[5]=static_cast<std::uint32_t>(game->frame_timer.current);}
        if(const auto* player=static_cast<const player_entity::Player*>(game_session::context(0).objects_04[0])){
            out[1]|=4;std::memcpy(out+8,&player->position_614.x,4);std::memcpy(out+9,&player->position_614.y,4);
        }
        out[1]|=0x100;out[11]=0;
    }return out;
}
}
