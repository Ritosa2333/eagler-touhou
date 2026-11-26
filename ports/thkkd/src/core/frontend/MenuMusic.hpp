// 原5E5E80..5E5EE4初始化、5EA4D0..5EA89B更新，以及5E9800返回标题。
#pragma once
#include "core/frontend/MenuMusicContext.hpp"
#include "core/frontend/MenuMusicServices.hpp"
#include "core/frontend/MenuMusicArithmetic.hpp"
#include "core/frontend/MenuMusicConstants.hpp"
#include "core/shared/MusicPresentation.hpp"
namespace kaikeidou {
inline void initialize_menu_music(MenuMusicContext& menu){
    menu.outer_scene=14;menu.selected=0;menu.scroll=0;menu.phase=0;menu.timer=0;
    for(auto& entry:menu.entry_x_bits)entry=0x4050000000000000ull;
}
inline void update_menu_music(MenuMusicContext& menu,MusicPresentationContext& music,
    const std::uint32_t& game_scene,const std::uint32_t& volume,unsigned precision,MenuMusicServices& services){
    using namespace menu_music_detail;
    for(std::uint32_t i=0;i<17;++i){
        if(i==menu.selected){if(above60(menu.entry_x_bits[i]))menu.entry_x_bits[i]=step(menu.entry_x_bits[i],true,precision);}
        else if(below64(menu.entry_x_bits[i]))menu.entry_x_bits[i]=step(menu.entry_x_bits[i],false,precision);
        if(signed_word(menu.scroll)<=static_cast<std::int32_t>(i)&&static_cast<std::int32_t>(i)<=signed_word(menu.scroll+9u)){
            const auto ink=i==menu.selected?services.color(255,255,128):services.color(128,128,128);
            const auto title=music_presentation_detail::title(i);
            const auto y=signed_word((i-menu.scroll)*20u+98u);
            const auto x=coordinate(menu.entry_x_bits[i]);
            services.draw_text(x,y,ink,formats[i+1<10?0:1],i+1,title);
        }
    }
    const auto title=music_presentation_detail::title(music.current_track);
    const auto title_color=services.color(255,255,255);
    services.draw_text(68,324,title_color,formats[2],0,title);
    const auto description=descriptions[music.current_track<17?music.current_track:17];
    const auto description_color=services.color(255,255,255);
    services.draw_text(68,357,description_color,formats[3],0,description);
    if(menu.phase==0){
        if(services.input_count(menu.bindings[0])==1){
            if(menu.selected==0){menu.selected=16;menu.scroll=7;}else --menu.selected;
            if(signed_word(menu.selected)<signed_word(menu.scroll))--menu.scroll;
            menu.nav_sound=1;
        }
        if(services.input_count(menu.bindings[1])==1){
            if(menu.selected==16){menu.selected=0;menu.scroll=0;}else ++menu.selected;
            if(signed_word(menu.scroll+9u)<signed_word(menu.selected))++menu.scroll;
            menu.nav_sound=1;
        }
        if(services.input_count(menu.bindings[2])==1)
            switch_music(menu.selected,1,music,menu.outer_scene,game_scene,volume,precision,services);
        else if(services.input_count(menu.bindings[3])==1){menu.phase=1;menu.timer=0;menu.cancel_sound=1;}
    }else if(menu.phase==1){
        ++menu.timer;
        if(menu.timer==20){
            if(music.current_track!=0)switch_music(0,1,music,menu.outer_scene,game_scene,volume,precision,services);
            menu.outer_scene=10;menu.phase=0;menu.timer=0;
            services.change_stage_resources(-1);
        }
    }
}
}
