#include "services.hpp"
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>
namespace p=th20::source::platform;
namespace pe=th20::source::program_entry;
namespace rt=th20::source::runtime;
int wmain(int argc,wchar_t** argv) {
    try {
        if(argc!=2)throw std::runtime_error("Provide the isolated test output directory");
        const auto root=std::filesystem::absolute(argv[1]); std::filesystem::create_directories(root);
        const auto run=root/("run_"+std::to_string(GetCurrentProcessId()));
        if(!std::filesystem::create_directory(run))throw std::runtime_error("Test directory must be new");
        pe::WindowStatePrefix window{}; pe::GraphicsStatePrefix graphics{};rt::Log log;
        unsigned count=0;
        auto check=[&](bool success,const char* detail){++count;if(!success)throw std::runtime_error(detail);};
        p::initialize_directories(window,log,run.c_str(),(run/L"game.exe").c_str());
        const auto data=run/L"ShanghaiAlice"/L"th20";
        check(std::filesystem::is_directory(data/L"replay")&&std::filesystem::is_directory(data/L"snapshot"),"Save subdirectories missing");
        check(std::filesystem::path(window.module_directory)==run,"Module parent path mismatch");
        check(p::load_configuration(graphics,window,log,"th20.cfg")==0,"Default configuration could not be saved");
        auto bytes=p::read_loose_file((data/L"th20.cfg").string().c_str());
        const auto defaults=p::default_configuration();
        check(bytes&&bytes->size()==sizeof(defaults)&&std::memcmp(bytes->data(),&defaults,sizeof(defaults))==0,"Default configuration bytes mismatch");
        graphics.configuration.bindings[0].keyboard[0]=0x41;
        graphics.configuration.saved_display_mode=-1; // Original signed comparison accepts negative modes.
        graphics.configuration.flags=0x7e;
        p::save_configuration(graphics.configuration,window);
        graphics.configuration=p::default_configuration();
        check(p::load_configuration(graphics,window,log,"th20.cfg")==0,"Existing configuration load failed");
        check(graphics.configuration.bindings[0].keyboard[0]==0x41&&graphics.configuration.saved_display_mode==-1&&graphics.disable_vsync==1,"Accepted configuration state lost");
        auto corrupted=graphics.configuration;corrupted.size=0;
        check(p::write_loose_file((data/L"th20.cfg").string().c_str(),&corrupted,sizeof(corrupted))==0,"Cannot write isolated malformed test file");
        check(p::load_configuration(graphics,window,log,"th20.cfg")==0&&std::memcmp(&graphics.configuration,&defaults,sizeof(defaults))==0&&graphics.disable_vsync==0,"Invalid configuration did not reset");
        check(p::write_loose_file((data/L"th20.cfg").string().c_str(),&corrupted,4)==0,"Cannot write truncated configuration");
        check(p::load_configuration(graphics,window,log,"th20.cfg")==0&&graphics.configuration.version==0x200002,"Truncated configuration did not reset");
        check(!p::read_loose_file((data/L"absent.bin").string().c_str()),"Missing file should return no bytes");
        check(p::write_loose_file((data/L"absent_directory"/L"config.bin").string().c_str(),&defaults,sizeof(defaults))==-1,"Open failure return mismatch");
        check(p::load_configuration(graphics,window,log,"absent_directory/config.bin")==-1&&log.error==1,"Write failure was not reported");
        LARGE_INTEGER frequency,origin;QueryPerformanceFrequency(&frequency);QueryPerformanceCounter(&origin);
        std::memcpy(&window.performance_frequency,&frequency,8);std::memcpy(&window.performance_origin,&origin,8);window.clock_offset=0;
        const auto t1=p::read_clock(window),t2=p::read_clock(window);
        check(std::isfinite(t1)&&t1>=0&&t2>=t1,"Real OS performance clock sampling failed");
        std::cout<<"Platform service checks: "<<count<<"; passed; isolated files: "<<run.string()<<'\n';
        return 0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
