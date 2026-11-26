// 连续探针的暂停适配：原场景字段在每个服务调用前发布，返回后重读。
#pragma once
#include "core/frontend/ReplayPauseEntryServices.hpp"
#include <cstring>
#include <functional>
namespace kaikeidou {
template<class Host> struct ReplayPauseProbeServices final:ReplayPauseEntryServices {
    Host& host;
    std::function<void()> publish,collect,draw;
    ReplayPauseProbeServices(Host& value,std::function<void()> before,std::function<void()> after,
        std::function<void()> drawing):host(value),publish(before),collect(after),draw(drawing){}
    template<class Function> auto boundary(Function function)->decltype(function()) {
        publish();const auto result=function();collect();return result;
    }
    std::int32_t physical_input_count(std::uint32_t binding)override{
        const std::uint32_t raw=binding<host.state.input.size()?host.state.input[binding]:0xffffffffu;
        std::int32_t result;std::memcpy(&result,&raw,4);return result;
    }
    void draw_pause_interface()override{publish();draw();collect();}
    std::int32_t stop_sound(std::uint32_t handle)override{
        publish();auto& state=host.state;
        state.presentation.begin(state.resource_tick,host.timeline.frame,1,3,{},&state.music,sizeof(state.music));
        const auto result=host.stop_sound(handle);
        state.presentation.end(&state.music,sizeof(state.music));collect();return result;
    }
    std::uint32_t load_graph(std::string_view path,std::uint32_t flag)override{return boundary([&](){return host.load_graph(path,flag);});}
    std::int32_t delete_graph(std::uint32_t handle,std::uint32_t flag)override{return boundary([&](){return host.delete_graph(handle,flag);});}
    std::uint32_t platform_random(std::uint32_t maximum)override{return boundary([&](){return host.platform_random(maximum);});}
    std::int32_t movie_seek(std::uint32_t handle,std::uint32_t time)override{return boundary([&](){return host.movie_seek(handle,time);});}
    std::int32_t movie_play(std::uint32_t handle,std::uint32_t type,std::uint32_t top)override{return boundary([&](){return host.movie_play(handle,type,top);});}
    std::int32_t movie_pause(std::uint32_t handle,std::uint32_t flag)override{return boundary([&](){return host.movie_pause(handle,flag);});}
    std::int32_t set_viewport_matrix(const std::array<std::uint32_t,16>& matrix)override{return boundary([&](){return host.set_viewport_matrix(matrix);});}
    std::int32_t set_create_sound_data_type(std::uint32_t type)override{return boundary([&](){return host.set_create_sound_data_type(type);});}
    std::int32_t delete_sound(std::uint32_t handle,std::uint32_t flag)override{return boundary([&](){return host.delete_sound(handle,flag);});}
    std::int32_t load_sound(std::string_view path,std::uint32_t count,std::uint32_t union_handle)override{return boundary([&](){return host.load_sound(path,count,union_handle);});}
    std::int32_t set_volume(std::uint32_t volume,std::uint32_t handle)override{return boundary([&](){return host.set_volume(volume,handle);});}
    std::int32_t play_sound(std::uint32_t handle,std::uint32_t type,std::uint32_t top)override{return boundary([&](){return host.play_sound(handle,type,top);});}
};
}
