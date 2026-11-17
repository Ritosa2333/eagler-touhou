#include "TrophyNotice.hpp"
#include <string_view>

namespace th17::sdl {
TrophyNotice::~TrophyNotice(){if(auto* resource=animations.resource(bank))graphics.unload(*resource);}

bool TrophyNotice::start(const TrophyEntry& entry){
    if(!animations.resource(bank)){
        std::vector<u8> bytes;
        if(!assets.read("trophy.anm",bytes)){error="TH17 achievement animation unavailable";return false;}
        if(!animations.load(bank,bytes.data(),bytes.size())){error=animations.error;return false;}
        if(!graphics.preload(*animations.resource(bank))){error=graphics.error;return false;}
    }
    root=animations.create_overlay(bank,0);
    if(!root){error=animations.error;return false;}
    // Scripts 4 and 5 write into the shared blank texture. Script 6 is the
    // visible child of script 0 that draws both lines with the native fade.
    TextureText line;
    line.font=9;
    line.color=0x80c0c0;
    line.centered=true;
    line.utf8=entry.utf8[0];
    const char* suffix="\x82\xcc\x8e\xc0\x90\xd1\x82\xf0\x8e\xe8\x82\xc9\x93\xfc\x82\xea\x82\xbd";
    if(line.utf8){
        // The original font takes a Windows COLORREF; SDL text takes RGB.
        line.color=0xc0c080;
        size_t size=0;
        auto* locale=static_cast<char*>(SDL_LoadFile("/thcrap/th17/runtime-language.txt",&size));
        const bool chinese=locale&&std::string_view(locale,size)=="lang_zh-hans";
        SDL_free(locale);
        line.bytes=chinese?u8"『"+entry.title+u8"』":"\""+entry.title+"\"";
        suffix=chinese?u8"已获得成就":"Achievement unlocked";
    }else{
        line.bytes="\x81\x77"+entry.title+"\x81\x78";
    }
    const auto write=[&](i32 script){
        u32 handle=animations.create_overlay(bank,script);
        auto* vm=animations.registry.find(handle);
        if(vm&&fonts.text(*vm,line)&&animations.retire(handle))return true;
        error=vm?fonts.error:animations.error;
        return false;
    };
    if(!write(4))return false;
    line.bytes=suffix;
    if(!write(5))return false;
    if(!audio.effects.enqueue(79,0)){error=audio.effects.error;return false;}
    return true;
}

bool TrophyNotice::draw(AnmRenderer& renderer,ScreenViews& views,const AnmEnvironment& screen){
    if(graphics.logical_drawing||(age==interval&&pending.empty()))return true;
    environment.resolution_scale=screen.resolution_scale;
    environment.screen_width=screen.screen_width;
    environment.screen_height=screen.screen_height;
    if(age==interval){
        const auto* entry=catalog.entry(pending.front());
        pending.pop_front();
        if(!start(*entry))return false;
        age=0;
    }
    // The invisible controller stops spawning sparkles at tick 45. Keep its
    // origin alive until the original children have finished their fades.
    if(auto* vm=animations.registry.find(root);vm&&vm->timer.current>=45)vm->instruction_offset=-1;
    if(age==interval-1&&!animations.retire(root)){error=animations.error;return false;}
    if(!animations.update(true)||!animations.update(false)){error=animations.error;return false;}
    ++age;
    renderer.flush();
    views.camera(DrawCamera::Fullscreen,false);
    renderer.invalidate();
    graphics.set_fog(false);
    graphics.set_depth_mask(false);
    graphics.set_depth_compare(touhou::graphics::Compare::Always);
    if(!renderer.draw_layer(animations.registry.layer(42))){error=renderer.error;return false;}
    renderer.flush();
    return true;
}
}
