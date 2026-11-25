#pragma once
#include "stage_sprites.hpp"

namespace soew::coop_art {
// ghost-preview/th02-player-colors/{manifest,projectiles-manifest}.json:
// replace only approved palette indices, preserving the original BFT put mask.
inline constexpr std::array<std::array<std::uint32_t,7>,2> shot_rgb={{
    {0x2244aa,0x4488ff,0x6633aa,0xaa77ee,0x33bbbb,0x116677,0x112255},
    {0x117744,0x33cc77,0x117788,0x55ddcc,0xddbb44,0x886622,0x004422},
}};
inline constexpr std::array<std::array<std::uint32_t,4>,2> player_rgb={{
    {0x2244aa,0x4488ff,0x2244aa,0x66bbff},
    {0x117744,0x33cc77,0x117744,0x55dd99},
}};
inline constexpr std::array<std::uint32_t,3> seat_ink={0xffff6969,0xff66bbff,0xff55dd99};
inline StageBackground::Palette shot_palette(StageBackground::Palette palette,unsigned seat){
    if(seat)for(unsigned i=0;i<7;++i){
        const auto c=shot_rgb[seat-1][i];
        palette[i==6?10:i+1]={std::uint8_t(c>>16),std::uint8_t(c>>8),std::uint8_t(c)};
    }
    return palette;
}
inline void player(std::span<std::uint32_t> pixels,const StageSprites &bank,const StageBackground::Palette &palette,unsigned seat,unsigned pattern,RenderPoint point){
    if(!seat){bank.put(pixels,palette,pattern,point);return;}
    const auto &image=*bank.sprites.at(pattern);
    blit_motion_sprite(pixels,point,int(image.width),int(image.height),[&](int x,int y)->std::uint32_t{
        const auto index=image.pixels[y*image.width+x];
        if(index==image.transparent)return render_clear;
        if(index>=1&&index<=4&&(pattern>=4||y<30))return player_rgb[seat-1][index-1];
        const auto c=palette[index];return (unsigned(c[0])<<16)|(unsigned(c[1])<<8)|c[2];
    });
}
// ghost-preview/reimu-ghost-32.png: original P1 pixels and the matching
// bow-only P2/P3 recolors from ghost-preview/th01-player-colors.
inline constexpr std::array<std::string_view,32> ghost_pixels={
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
    "00000011111000000000011111000000",
    "00000012222100000000122221000000",
    "00000013222211000011222231000000",
    "00000013222222444512222231000000",
    "00000013222222555522222231000000",
    "00000013222221555511222231000000",
    "00000012222116666666122221000000",
    "00000011111111111111111111000000",
    "00000000111111133111111110000000",
    "00000000111331333313311110000000",
    "00000001711333333333311110000000",
    "00000001713333333333338110000000",
    "00000001783331333333138871000000",
    "00000001783311333331138871000000",
    "00000001783311333331138871000000",
    "00000001787333333333387771000000",
    "00000001188833339933888710000000",
    "00000000178888888888877710000000",
    "00000000178888888888777110000000",
    "00000000010808080808070100000000",
    "00000000001070707080701000000000",
    "00000000000101010707010000000000",
    "00000000000000001070701010000000",
    "00000000000000000107070700000000",
    "00000000000000000010101000000000",
    "00000000000000000000000000000000",
    "00000000000000000000000000000000",
};
inline constexpr std::array<std::array<std::uint32_t,10>,3> ghost_rgb={{
    {0x00000000u,0xff282038u,0xffd84858u,0xfffff8efu,0xfff88080u,0xff982840u,0xff584068u,0xff70b8cfu,0xffc8f0efu,0xff487898u},
    {0x00000000u,0xff282038u,0xff4488eeu,0xfffff8efu,0xff88b8ffu,0xff284898u,0xff584068u,0xff70b8cfu,0xffc8f0efu,0xff487898u},
    {0x00000000u,0xff282038u,0xff38b878u,0xfffff8efu,0xff88e0a8u,0xff206848u,0xff584068u,0xff70b8cfu,0xffc8f0efu,0xff487898u},
}};
inline void ghost(std::span<std::uint32_t> pixels,unsigned seat,RenderPoint point){
    blit_motion_sprite(pixels,point.offset(0,12),32,32,[&](int x,int y)->std::uint32_t{
        const char index=ghost_pixels[y][x];
        if(index=='0')return render_clear;
        return ghost_rgb[seat][index>='a'?index-'a'+10:index-'0']&0xffffffu;
    });
}
}
