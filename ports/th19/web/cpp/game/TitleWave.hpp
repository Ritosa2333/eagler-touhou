#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
namespace th19 {
// th19.exe 00554130 / 005059e0. This is a deformed copy of the title
// background render target, not a generated ring texture or a flat panel.
struct TitleWave {
    static constexpr int columns=42, rows=32;
    static constexpr float pi=3.1415927410125732f;
    // DAT_005a1eb0: native order B,G,R,A (four int32 values per entry).
    static constexpr int colors[10][4]={
        {0,128,224,160},{255,64,64,160},{255,64,64,160},
        {128,128,128,160},{224,96,240,160},{224,224,64,160},
        {96,224,96,160},{64,64,64,160},{128,128,128,224},{64,64,64,192}
    };
    int selection=-1, age=40;
    std::array<int,4> color{},from{},frameColor{};
    float phase=0,framePhase=0;
    float background=255,backgroundFrom=255;
    void select(int index) {
        index=std::clamp(index,0,9);
        if(index==selection)return;
        selection=index;from=color;backgroundFrom=background;age=0;
    }
    void tick() {
        // 0055ac60 calls the wave update BEFORE the 40-frame color update.
        frameColor=color;framePhase=phase;
        phase-=pi/100;
        if(phase < -pi)phase+=2*pi;
        if(age<40&&selection>=0){
            const float t=float(age)/40;
            for(int i=0;i<4;++i)color[i]=int(from[i]+(colors[selection][i]-from[i])*t)&255;
            // The background sprite uses a separate 40-frame ANM RGB lerp.
            background=backgroundFrom+((selection==9?128.f:255.f)-backgroundFrom)*float(age+1)/40;
            ++age;
        }
    }
    struct Vertex {float x,y,u,v;std::uint32_t tint;};
    Vertex vertex(int column,int row) const {
        const float x=float(column)*1280/(columns-1), y=float(row)*960/(rows-1);
        Vertex v{x,y,x/1280,y/960,0xffffffff};
        // 005059e0 resets ALL vertices to white; 00554130 changes only the interior.
        if(column==0||row==0||column==columns-1||row==rows-1)return v;
        const float dx=x-640,dy=y-480;
        float angle=2*pi*std::sqrt(dx*dx+dy*dy)/120;
        while(angle>pi)angle-=2*pi;
        angle+=framePhase;
        while(angle>pi)angle-=2*pi;
        while(angle < -pi)angle+=2*pi;
        const float s=std::sin(angle),c=std::cos(angle),sum=s+c;
        v.x+=s*14;v.y+=c*14;
        auto channel=[&](int i){return std::uint32_t(std::clamp(int(frameColor[i]-frameColor[i]*sum/2),0,255));};
        v.tint=(std::uint32_t(frameColor[3])<<24)|(channel(2)<<16)|(channel(1)<<8)|channel(0);
        return v;
    }
};
}
