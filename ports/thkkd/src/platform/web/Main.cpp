#include "WebHost.hpp"
#include <emscripten.h>
#include <filesystem>
#include <fstream>

namespace {
using namespace kaikeidou;
std::unique_ptr<GameState> world;
std::unique_ptr<WebHost> platform;
std::unique_ptr<CombinedGameHost> game;
std::string error;
char replay_path[512]{};
template<class F> int checked(F&& f){try{return f();}catch(const std::exception& e){error=e.what();return -1;}catch(const GameStartEntryAccessFault& e){error="State access fault at "+std::to_string(e.address);return -2;}}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE const char* thkkd_error(){return error.c_str();}
EMSCRIPTEN_KEEPALIVE void thkkd_stop(){game.reset();platform.reset();world.reset();}
EMSCRIPTEN_KEEPALIVE int thkkd_start(){return checked([]{
    thkkd_stop();std::filesystem::create_directories("/savesthkkd/replay");
    world=std::make_unique<GameState>();world->prepare_frame(true);
    platform=std::make_unique<WebHost>(*world);
    game=std::make_unique<CombinedGameHost>(world->combined_refs(),platform->combined_platform(),true,24,platform->math);
    platform->memory=&game->memory();world->initialize(*game,true);
    auto result=game->begin_application();if(result.kind!=ApplicationStartupKind::entered_outer)throw std::runtime_error("Application startup did not reach menu");return 0;
});}
EMSCRIPTEN_KEEPALIVE int thkkd_step(){return checked([]{
    if(!game)throw std::runtime_error("Game is not running");auto result=game->step();
    if(result.kind==OuterSceneOutcomeKind::exit_requested)return 1;
    if(result.kind!=OuterSceneOutcomeKind::completed)throw std::runtime_error("Unsupported game boundary at "+std::to_string(result.source));return 0;
});}
EMSCRIPTEN_KEEPALIVE unsigned thkkd_word(unsigned address){return game?game->memory().word(address):0;}
EMSCRIPTEN_KEEPALIVE char* thkkd_replay_path(){return replay_path;}
EMSCRIPTEN_KEEPALIVE int thkkd_replay_open(){return checked([]{
    if(!game)throw std::runtime_error("Game is not running");
    if(game->memory().word(0xa8ce58)==0){auto result=game->step();if(result.kind!=OuterSceneOutcomeKind::completed)throw std::runtime_error("Replay bootstrap failed");}
    platform->replay_override=replay_path;
    // 外部播放入口按原录像的练习标志恢复菜单原先提供的 game_mode。
    std::ifstream file(std::string("/savesthkkd/")+replay_path,std::ios::binary);
    file.seekg(-58,std::ios::end);unsigned char mode[4]{};file.read(reinterpret_cast<char*>(mode),4);
    if(!file)throw std::runtime_error("Replay metadata could not be read");
    const unsigned recorded_mode=unsigned(mode[0])|(unsigned(mode[1])<<8)|(unsigned(mode[2])<<16)|(unsigned(mode[3])<<24);
    auto& memory=game->memory();memory.word(0xa7e618,recorded_mode==2?1:0);memory.word(0xb4310c,1);memory.word(0xb43940,0);memory.word(0xa8ce58,99);memory.reconcile();
    auto result=game->step();platform->replay_override.clear();if(result.kind!=OuterSceneOutcomeKind::completed)throw std::runtime_error("Replay initialization failed");return 0;
});}
EMSCRIPTEN_KEEPALIVE int thkkd_restart(){return checked([]{
    if(!game)return 0;auto& memory=game->memory();
    if(memory.word(0xa8ce58)!=100||memory.word(0xa78e2c)!=1)return 0;
    memory.word(0xa87f50,2);memory.word(0xa87f48,2);memory.word(0xa87f4c,0);memory.reconcile();return 1;
});}
}
