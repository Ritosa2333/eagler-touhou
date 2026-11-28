#pragma once
#include "Types.hpp"
#include <string>
#include <vector>
namespace th13 {
struct AnmInstruction {
    i16 opcode;
    u16 length;
    i16 time;
    u16 references;
    template <class T> T argument(u32 index) const noexcept {
        T value;
        std::memcpy(&value, reinterpret_cast<const u8*>(this) + 8 + index * 4, sizeof value);
        return value;
    }
};
static_assert(sizeof(AnmInstruction) == 8);
struct AnmSprite {
    u32 source_id = 0, texture = 0;
    float x = 0, y = 0, width = 0, height = 0;
    float u0 = 0, v0 = 0, u1 = 0, v1 = 0;
};
struct AnmTexture {
    enum class Kind { Embedded, External, Blank, RenderTarget };
    std::string name;
    Kind kind = Kind::Embedded;
    u16 width = 0, height = 0, format = 0, pixel_width = 0, pixel_height = 0, pixel_format = 0;
    i16 source_x = 0, source_y = 0;
    u32 priority = 0;
    bool low_resolution_scale = false;
    std::vector<u8> pixels;
    bool rgba(std::vector<u8>& output) const;
};
struct AnmScript {
    i32 source_id = 0;
    u32 file_offset = 0;
    std::vector<u8> bytes;
    u32 sprite_base = 0, script_base = 0;
};
class AnmResource {
    mutable u64 content_identity_ = 0;

public:
    std::vector<AnmTexture> textures;
    std::vector<AnmSprite> sprites;
    std::vector<AnmScript> scripts;
    u64 content_identity() const noexcept;
    void refresh_identity() const noexcept;
    i32 script_index(const std::vector<u8>* script) const noexcept;
    i32 sprite_index(const AnmSprite* sprite) const noexcept;
    bool open(const u8* bytes, u32 size);
    bool append(const u8* bytes, u32 size);
};
u64 anm_script_identity(const std::vector<u8>& bytes) noexcept;
}
