// 原A79000的80字节提示块，加A78E28曲目和A8CE5C音频句柄。
#pragma once
#include "core/shared/ObjectRecord.hpp"
namespace kaikeidou {
struct MusicPresentationContext {
 ObjectRecord<80> banner{};
 std::uint32_t current_track=0;
 std::uint32_t audio_handle=0;
};
static_assert(sizeof(MusicPresentationContext)==88,"Original music state layout");
}
