#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>
namespace soew {
// Source op_04.cpp:114-138; GRC clipping endpoints are inclusive.
struct LogoCel {std::array<std::uint8_t,64*32> indices{};};
struct ScoreAnimation {
    unsigned frame=0;
    static constexpr unsigned limit=2000;
    bool tick(){++frame;return frame>limit;}
    void reset(){frame=0;}
};
inline void score_logo_background(std::vector<std::uint32_t> &pixels,
    const std::array<LogoCel,2> &logo,const std::array<std::uint32_t,16> &palette,
    std::uint8_t transparent,unsigned frame) {
    if(pixels.size()!=640*400)throw std::runtime_error("Score canvas dimensions invalid");
    for(int y=96;y<=304;++y)for(int x=128;x<=512;++x)pixels[y*640+x]=palette[10];
    const auto blit=[&](int x,int y,int cel){
        for(int row=0;row<32;++row)for(int col=0;col<64;++col){
            const int px=x+col,py=y+row;
            if(px<128||px>512||py<96||py>304)continue;
            const auto c=logo[cel].indices[row*64+col];
            if(c>=16)throw std::runtime_error("Score logo palette index invalid");
            if(c!=transparent)pixels[py*640+px]=palette[c];
        }
    };
    // Initial displayed page contains fill but no logos, before frame++.
    if(!frame)return;
    for(int offset:{0,320})for(int i=0;i<4;++i){
        const int x=int((frame+160u*unsigned(i)+unsigned(offset))%640u);
        int y=(i*100-int(frame%400)+400)%400;
        blit(x,y,0);blit(x+64,y,1);
    }
}
}
