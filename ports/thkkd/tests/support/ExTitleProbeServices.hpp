// 标题模块复用资源和音乐平台流，235次随机返回由独立标题请求提供。
#pragma once
#include "core/frontend/MenuTitleReturnServices.hpp"
#include <functional>
#include <stdexcept>
namespace kaikeidou {
template<class Host> struct ExTitleProbeServices final:MenuTitleReturnServices {
    Host& host;
    std::function<void()> publish;
    bool music_active=false;
    explicit ExTitleProbeServices(Host& value,std::function<void()> sync):host(value),publish(sync){}
    void begin_music(){
        publish();if(music_active)return;
        auto& s=host.state;s.presentation.begin(s.resource_tick,host.timeline.frame,1,1,{0,1},&s.music,sizeof(s.music));
        music_active=true;
    }
    void finish_music(){
        if(!music_active)throw std::runtime_error("EX title music request missing");
        auto& s=host.state;s.presentation.end(&s.music,sizeof(s.music));music_active=false;publish();
    }
    std::int32_t delete_graph(std::uint32_t handle,std::uint32_t flag)override{publish();return host.delete_graph(handle,flag);}
    std::uint32_t load_graph(std::string_view path,std::uint32_t flag)override{publish();return host.load_graph(path,flag);}
    std::uint32_t platform_random(std::uint32_t maximum)override{publish();return host.state.ex_title.random(maximum);}
    std::int32_t movie_seek(std::uint32_t handle,std::uint32_t value)override{publish();return host.movie_seek(handle,value);}
    std::int32_t movie_play(std::uint32_t handle,std::uint32_t mode,std::uint32_t flag)override{publish();return host.movie_play(handle,mode,flag);}
    std::int32_t set_create_sound_data_type(std::uint32_t type)override{begin_music();return host.set_create_sound_data_type(type);}
    std::int32_t stop_sound(std::uint32_t handle)override{begin_music();return host.stop_sound(handle);}
    std::int32_t delete_sound(std::uint32_t handle,std::uint32_t flag)override{begin_music();return host.delete_sound(handle,flag);}
    std::int32_t load_sound(std::string_view path,std::uint32_t count,std::uint32_t union_handle)override{begin_music();return host.load_sound(path,count,union_handle);}
    std::int32_t set_volume(std::uint32_t volume,std::uint32_t handle)override{begin_music();return host.set_volume(volume,handle);}
    std::int32_t play_sound(std::uint32_t handle,std::uint32_t type,std::uint32_t top)override{begin_music();return host.play_sound(handle,type,top);}
};
}
