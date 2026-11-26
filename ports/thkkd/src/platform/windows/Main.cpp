#include "WindowsHost.hpp"
#include <windows.h>
#include <shellapi.h>
#include <fstream>
#include <sstream>

int WINAPI wWinMain(HINSTANCE,HINSTANCE,PWSTR,int) {
    using namespace kaikeidou;
    std::filesystem::path log_path;
    try {
        wchar_t executable[32768]{};GetModuleFileNameW(nullptr,executable,32768);
        auto root=std::filesystem::path(executable).parent_path();
        auto assets=root/"assets";
        for(auto p=root;!std::filesystem::exists(assets)&&p!=p.parent_path();p=p.parent_path())
            if(std::filesystem::exists(p/"assets"))assets=p/"assets";
        DesktopOptions options;options.assets=assets;options.userdata=root/"userdata";
        bool variant=true,settings_dialog=false;unsigned precision=24;
        int count=0;auto args=CommandLineToArgvW(GetCommandLineW(),&count);
        for(int i=1;i<count;++i) {
            std::wstring a=args[i];
            auto value=[&]()->std::wstring {if(++i>=count)throw std::runtime_error("Missing command argument");return args[i];};
            if(a==L"--assets")options.assets=value();
            else if(a==L"--userdata")options.userdata=value();
            else if(a==L"--frames")options.frame_limit=std::stoul(value());
            else if(a==L"--screenshot")options.screenshot=value();
            else if(a==L"--precision")precision=std::stoul(value());
            else if(a==L"--mute")options.muted=true;
            else if(a==L"--fast")options.fast=true;
            else if(a==L"--secondary")options.secondary=true;
            else if(a==L"--settings")settings_dialog=true;
            else if(a==L"--fullscreen")options.window_mode_override=0;
            else if(a==L"--windowed")options.window_mode_override=1;
            else if(a==L"--close-frame")options.close_frame=std::stoul(value());
            else if(a==L"--toggle-frame")options.toggle_frames.insert(std::stoul(value()));
            else if(a==L"--capture-frame")options.capture_frames.insert(std::stoul(value()));
            else if(a==L"--trace")options.trace=true;
            else if(a==L"--base")variant=false;
            else if(a==L"--keys"){
                std::wistringstream input(value());std::wstring item;
                while(std::getline(input,item,L',')){auto split=item.find(L':');if(split!=std::wstring::npos)options.scripted_keys[std::stoul(item.substr(0,split))].push_back(std::stoul(item.substr(split+1),nullptr,0));}
            }
            else throw std::runtime_error("Unknown command argument");
        }
        LocalFree(args);
        if(settings_dialog)return show_display_settings(options.userdata);
        std::filesystem::create_directories(options.userdata);log_path=options.userdata/"thkkd.log";
        {std::ofstream reset(log_path,std::ios::trunc);reset<<"thkkd Windows desktop\n";}
        std::ofstream log(log_path,std::ios::app);
        auto world=std::make_unique<GameState>();world->prepare_frame(variant);
        WindowsHost platform(*world,precision,variant,std::move(options));
        CombinedGameHost game(world->combined_refs(),platform.combined_platform(),variant,precision,platform.math);
        platform.memory=&game.memory();world->initialize(game,variant);
        const auto startup=game.begin_application();
        if(startup.kind!=ApplicationStartupKind::entered_outer){log<<"startup kind="<<unsigned(startup.kind)<<" pc="<<std::hex<<startup.source<<" fault="<<startup.fault_address<<"\n";throw std::runtime_error("Application startup did not reach menu");}
        platform.initialize_window();
        std::uint32_t previous=0xffffffffu;
        for(;;) {
            auto result=game.step();
            if(result.scene!=previous){log<<"frame="<<platform.frames<<" scene="<<result.scene<<" game="<<result.game_frame<<"\n";log.flush();previous=result.scene;}
            if(result.kind==OuterSceneOutcomeKind::exit_requested)break;
            if(result.kind!=OuterSceneOutcomeKind::completed){log<<"kind="<<unsigned(result.kind)<<" pc="<<std::hex<<result.source<<" target="<<result.target<<" fault="<<result.fault_address<<" size="<<result.fault_bytes<<"\n";log.flush();throw std::runtime_error("Game runtime stopped at an unsupported boundary");}
        }
        log<<"completed frames="<<platform.frames<<"\n";log.flush();return 0;
    } catch(const std::exception& error) {
        if(!log_path.empty())std::ofstream(log_path,std::ios::app)<<"ERROR: "<<error.what()<<"\n";
        OutputDebugStringA(error.what());return 1;
    } catch(const GameStartEntryAccessFault& error) {
        if(!log_path.empty())std::ofstream(log_path,std::ios::app)<<"ERROR: memory address="<<std::hex<<error.address<<" bytes="<<error.bytes<<"\n";
        return 2;
    }
}
