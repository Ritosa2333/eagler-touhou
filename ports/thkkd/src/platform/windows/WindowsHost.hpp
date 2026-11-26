#pragma once
#include "GamePlatformServices.hpp"
#include "DisplaySettings.hpp"
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <set>

namespace kaikeidou {
struct DisplayState;
struct DesktopOptions {
    std::filesystem::path assets, userdata, screenshot;
    unsigned frame_limit = 0;
    unsigned close_frame = 0;
    int window_mode_override = -1;
    std::set<unsigned> toggle_frames;
    std::set<unsigned> capture_frames;
    bool muted = false, fast = false, secondary = false, trace = false;
    std::map<unsigned, std::vector<unsigned>> scripted_keys;
};

class WindowsHost final : public GamePlatformServices, public ApplicationStartupServices {
    struct File { std::fstream stream; };
    DesktopOptions options_;
    std::fstream save_file_;
    std::map<U, File> files_;
    std::map<U, std::fstream> config_files_;
    std::map<int, bool> graph_alpha_;
    U next_handle_ = 0x70000000, next_allocation_ = 0x2000000;
    std::set<U> unsupported_;
    bool initialized_ = false;
    bool rendering_snapshot_ = false;
    std::unique_ptr<DisplayState> display_;
    DisplaySettings settings_;
    bool windowed_=true,software_rendering_=false,hotkey_down_=false,close_posted_=false,closing_=false;
    int output_width_=640,output_height_=480,color_depth_=32,canvas_=-1;
    int blend_mode_=0;
    double window_scale_=1.0;
    std::array<int,4> draw_area_{{0,0,640,480}};
    void toggle_fullscreen();
    int process_messages();
    int present_screen();
    void shutdown() noexcept;
    void draw_scene(U entry);
    std::filesystem::path data_path(std::string_view path) const;
    std::filesystem::path asset_path(std::string_view path) const;
    std::string memory_string(U address) const;
    int legacy_transparency(int graph, int requested) const;
public:
    unsigned frames = 0;
    bool suppress_drawing = false;
    std::function<void(U)> render_component;
    WindowsHost(GameState& state, unsigned precision, bool variant, DesktopOptions options);
    ~WindowsHost();
    using GamePlatformServices::invoke;
    MenuScenePlatformResult invoke(const MenuSceneRequest& request, MenuReplayFormatMemory* local = nullptr) override;
    ApplicationStartupPlatformResult invoke(const ApplicationStartupRequest& request) override;
    CombinedGameHostPlatform combined_platform() { return {*this,*this,pause_platform(),*this,*this,*this}; }
    bool open(std::string_view path, bool writing) override;
    std::size_t read(std::uint8_t* data, std::size_t size) override;
    void write(const std::uint8_t* data, std::size_t size) override;
    void close() override;
    void initialize_window();
    void screenshot(const std::filesystem::path& path);
    void present_gameplay() override;
    void finish_gameplay() override { suppress_drawing=false; }
    const DesktopOptions& options() const { return options_; }
};
}
