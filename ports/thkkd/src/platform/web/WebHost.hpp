#pragma once
#include "WebMovement.hpp"
// 只在浏览器宿主接入扩展输入；原核心文本与普通输入路径保持原样。
#define update_replay_gameplay_movement web_update_replay_gameplay_movement
#include "GamePlatformServices.hpp"
#undef update_replay_gameplay_movement
#include "render/DisplayState.hpp"
#include <fstream>
#include <map>
#include <memory>

namespace kaikeidou {
class WebHost final : public GamePlatformServices, public ApplicationStartupServices {
    using Bytes=std::vector<std::uint8_t>;
    std::fstream save_file_;
    std::map<U,std::fstream> files_,config_files_;
    std::map<U,std::string> recording_paths_;
    std::map<U,std::string> config_write_paths_;
    std::string save_write_path_;
    U next_handle_=0x70000000,next_allocation_=0x2000000;
    std::unique_ptr<DisplayState> display_;
    bool rendering_snapshot_=false,suppress_drawing_=false;
    std::string data_path(std::string_view) const;
    std::string memory_string(U) const;
    void draw_scene(U);
    void draw_identity();
public:
    unsigned frames=0;
    bool exiting=false;
    bool high_refresh=false;
    std::string replay_override;
    WebHost(GameState& state):GamePlatformServices(state,24,true){}
    using GamePlatformServices::invoke;
    MenuScenePlatformResult invoke(const MenuSceneRequest&,MenuReplayFormatMemory* =nullptr) override;
    ApplicationStartupPlatformResult invoke(const ApplicationStartupRequest&) override;
    CombinedGameHostPlatform combined_platform(){return {*this,*this,pause_platform(),*this,*this,*this};}
    bool open(std::string_view,bool) override;
    std::size_t read(std::uint8_t*,std::size_t) override;
    void write(const std::uint8_t*,std::size_t) override;
    void close() override;
    void present_gameplay() override;
    void finish_gameplay() override {suppress_drawing_=false;}
};
}
