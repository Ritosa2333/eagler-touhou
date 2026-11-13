#pragma once
#include "Types.hpp"
#include "PortTitle.hpp"
#include <array>
namespace th11 {
// Original fixed-size th11.cfg. Platform-specific display fields are retained
// when imported, while the SDL backend owns its actual window and swap chain.
struct GameConfig {
    std::array<u8,kPortTitle==14?0x5c:0x3c> bytes{};
    static constexpr u32 volume_offset=kPortTitle==14?0x22:0x20;
    GameConfig(){reset();}
    void reset(const u8* controller=nullptr)noexcept;
    bool open(const u8*,u32)noexcept;
    i32 music_volume()const noexcept{return bytes[volume_offset];}
    i32 sound_volume()const noexcept{return bytes[volume_offset+1];}
    bool auto_focus()const noexcept{return bytes[volume_offset+2]!=0;}
#if PORT_TITLE == 14
    // Retail46af64 selects the mode;46ae40 supplies the internal pixel size.
    u32 render_width()const noexcept{
        const u32 mode=bytes[0x1f];
        return mode==1||mode==4?960:mode==2||mode==5?1280:640;
    }
    u32 render_height()const noexcept{return render_width()*3/4;}
#endif
};
}
