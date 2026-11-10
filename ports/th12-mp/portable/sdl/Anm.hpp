// TH12 ANM v7 files read straight from th12.dat: entries, sprites, scripts and
// THTX textures. The same reading as upstream's scripts/prepare-assets.py
// (which turned them into atlases.json + PNGs for the browser), done at load
// time instead.
#pragma once
#include "Platform.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace th12host {
struct AnmCommand {
    u32 offset=0;u16 op=0;i16 time=0;u16 mask=0;
    std::vector<i32> ints;std::vector<float> floats; // floats: non-finite values read as 0 (upstream null)
};
struct AnmScript {
    i32 source_id=0,entry=0;std::vector<AnmCommand> commands;
    std::unordered_map<u32,u32> offsets; // command byte offset -> index
};
struct AnmSprite {i32 entry=0;float x=0,y=0,w=0,h=0;};
struct AnmEntry {
    std::string name;
    u32 width=0,height=0,format=0;bool has_image=false;
    u32 offset_x=0,offset_y=0,address_flags=3;
    // The texture upstream made for this entry: max(THTX, header) in size,
    // the THTX rectangle at (0,0); R5G6B5 padding is opaque black.
    u32 texture_width=0,texture_height=0;std::vector<u8> rgba;
    u32 texture=0; // GL name once uploaded
};
struct AnmFile {
    std::string name; // file stem, e.g. "front", "stage01"
    std::vector<AnmEntry> entries;std::vector<AnmSprite> sprites;std::vector<AnmScript> scripts;
    const AnmSprite* sprite(i32 id)const{return id>=0&&u32(id)<sprites.size()?&sprites[id]:nullptr;}
    const AnmScript* script(i32 id)const{return id>=0&&u32(id)<scripts.size()?&scripts[id]:nullptr;}
};
// keep_pixels=false skips texture decoding (scripts and sprites only).
bool parse_anm(const std::string& name,const std::vector<u8>& bytes,AnmFile& out,std::string& error,bool keep_pixels=true);
}
