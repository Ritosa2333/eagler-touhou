#include "../replay.hpp"
#include "../../program_entry/program_entry.hpp"
#include "../../archive/resource_manager.hpp"
#include <filesystem>
#include <iostream>
#include <cstring>
#include <array>
// Explicit data fixture for the isolated508b90 file loader. No game owner is
// instantiated and no executable image is opened. All file/codec code is source.
namespace th20::source::program_entry {WindowStatePrefix window_state{};}
int wmain(int argc,wchar_t**argv){using namespace th20::source;try{
    if(argc!=2)throw std::runtime_error("Usage: demo_verify ORIGINAL_ASSET_ARCHIVE.dat");if(!resources::open(argv[1]))throw std::runtime_error("Cannot open reference assets");
    game_session::flags()|=0x20u;unsigned files=0,stages=0;std::uint64_t frames=0;
    for(int demo=1;demo<=4;++demo){alignas(replay::ReplayInf) std::uint8_t storage[sizeof(replay::ReplayInf)]{};auto& view=*reinterpret_cast<replay::ReplayInf*>(storage);const auto filename="demo"+std::to_string(demo)+".rpy";
        if(replay::load(view,filename.c_str())!=0)throw std::runtime_error("Source loader rejected "+filename);
        if(!view.user||!view.decoded||view.header->magic!=0x72303274u)throw std::runtime_error("Invalid decoded replay owner fields");
        unsigned this_stages=0;std::uint64_t this_frames=0;
        for(auto& cursor:view.playback)if(cursor.stage){++stages;++this_stages;this_frames+=cursor.stage->frame_count;frames+=cursor.stage->frame_count;if(cursor.stage->frame_count==0)throw std::runtime_error("Empty demo input sequence");if(reinterpret_cast<std::uint8_t*>(cursor.inputs)+cursor.stage->frame_count*6!=cursor.fps)throw std::runtime_error("Replay FPS stream mismatch");}
        if(!this_stages)throw std::runtime_error("Demo has no stages");std::cout<<filename<<" stages="<<this_stages<<" frames="<<this_frames<<"\n";++files;
        runtime::release_bytes(view.header);runtime::release_bytes(view.user);
    }
    resources::close();std::cout<<"{\"files\":"<<files<<",\"stages\":"<<stages<<",\"frames\":"<<frames<<",\"original_executable_loaded\":false}\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}
