#pragma once
#include "Types.hpp"
#include <string>
#include <vector>
namespace th19 {
// ANM v8 (th13+, thtk thanm.c anm_header11_t). th19 adds jpeg_quality + w_max/h_max.
struct AnmSprite {u32 id=0;float x=0,y=0,w=0,h=0;float offset_x=0,offset_y=0,scale_x=1,scale_y=1;};
struct AnmScript {i32 id=0;u32 offset=0;};
struct AnmEntry {
    u32 base=0;                    // entry base offset within the .anm file
    std::string name;
    u16 w=0,h=0,format=0;u32 name_offset=0,thtx_offset=0,next_offset=0;
    u16 has_data=0;u8 lowres_scale=0,jpeg_quality=0;u16 w_max=0,h_max=0;
    std::vector<AnmSprite> sprites;
    std::vector<AnmScript> scripts;
    // THTX texture (th19: JPEG or PNG-encoded data)
    u32 thtx_w=0,thtx_h=0,thtx_size=0;u16 thtx_format=0;u32 data_offset=0;
};
// Parse all entries in an .anm file (chained via next_offset).
bool parse_anm(const u8* data,u32 size,std::vector<AnmEntry>& entries);
}
