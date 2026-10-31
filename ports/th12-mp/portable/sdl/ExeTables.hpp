// Static tables read out of the player's own th12.exe (1.00b), exactly the
// ones upstream's content preparation reads (scripts/prepare-sounds.py and
// prepare-presentation.py). Nothing is executed.
#pragma once
#include "Platform.hpp"
#include <array>
#include <string>

namespace th12host {
struct BulletType {
    i32 script=0;
    struct Color {i32 sprite=0,spawn=0,cancel=0;};
    std::array<Color,16> colors{};
    float size=0;u32 flags=0;
};
struct SoundRecord {i32 id=0;std::string file;i16 volume=0;bool loop=false;};
struct ExeTables {
    std::array<BulletType,31> bullets{};
    std::array<SoundRecord,60> sounds{};
    float near_plane=30,far_plane=1800;
    std::vector<std::string> music_warning; // Music Room's eight CP932 warning lines (0x4b33c0)
    std::string sha256,version; // version: empty when the hash is unknown
    std::string error;
    bool warning_utf8=false;
    bool load(const std::vector<u8>& exe);
    bool load_prepared(const std::vector<u8>& bytes);
};
std::string sha256_hex(const u8* data,size_t size);
}
