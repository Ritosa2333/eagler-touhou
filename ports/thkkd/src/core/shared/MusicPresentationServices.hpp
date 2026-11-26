// 六个音频平台边界；load_sound返回值按原位模式存回句柄。
#pragma once
#include <cstdint>
#include <string_view>
namespace kaikeidou {
struct MusicPresentationServices {
 virtual ~MusicPresentationServices()=default;
 virtual std::int32_t set_create_sound_data_type(std::uint32_t type)=0;
 virtual std::int32_t stop_sound(std::uint32_t handle)=0;
 virtual std::int32_t delete_sound(std::uint32_t handle,std::uint32_t flag)=0;
 virtual std::int32_t load_sound(std::string_view path,std::uint32_t buffer_count,std::uint32_t union_handle)=0;
 virtual std::int32_t set_volume(std::uint32_t volume,std::uint32_t handle)=0;
 virtual std::int32_t play_sound(std::uint32_t handle,std::uint32_t play_type,std::uint32_t top_position)=0;
};
}
