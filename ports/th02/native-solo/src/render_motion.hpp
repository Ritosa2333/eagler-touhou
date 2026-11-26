#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <span>
#include <vector>
#include "player_shots.hpp"

namespace soew {
inline constexpr unsigned motion_subpixels=256;

struct RenderPoint {
    int x=0,y=0; // 24.8 screen coordinates; not used for collision or replay.
    static constexpr RenderPoint pixels(int x,int y){return {x*256,y*256};}
    static constexpr int floor(int v){return v>=0?v/256:(v-255)/256;}
    RenderPoint offset(int dx,int dy)const{return {x+dx*256,y+dy*256};}
    RenderPoint snapped()const{return pixels(floor(x),floor(y));}
    RenderPoint nearest()const{return pixels(floor(x+128),floor(y+128));}
};

constexpr int motion_lerp(int a,int b,unsigned phase){return a+(b-a)*int(phase)/256;}
constexpr RenderPoint motion_lerp(RenderPoint a,RenderPoint b,unsigned phase){
    return {motion_lerp(a.x,b.x,phase),motion_lerp(a.y,b.y,phase)};
}

struct MotionAnchor {
    RenderPoint point;
    unsigned kind=0;
    int age=-1;
    bool visible=false;
};

// Pooled objects are keyed by their original slots, never by draw order.
enum class MotionTrack : unsigned {
    Player,Option,Enemy,Shot,Bullet,Item,Spark,Number,Laser,
    Midboss1,Midboss2,Midboss3,Midboss4,Rika,Meira,Marisa,MarisaOrb,Mima,MimaOrb,Sigma,Stone,MeiraClone,Count
};

struct RenderEffect { unsigned kind,color; int x,y,a,b; };
struct RenderDeathEffect { int x=0,y=0; unsigned frame=0,angle=0; };

struct MotionFrame {
    static constexpr std::array<unsigned,unsigned(MotionTrack::Count)> sizes={1,1,25,PlayerShots::coop_capacity,150,20,64,20,12,1,1,2,1,1,1,1,4,1,8,1,5,3};
    static constexpr auto offsets=[] {
        std::array<unsigned,sizes.size()+1> out{};
        for(unsigned i=0;i<sizes.size();++i)out[i+1]=out[i]+sizes[i];
        return out;
    }();
    std::array<MotionAnchor,offsets.back()> anchors{};
    std::uint64_t tick=0;
    int stage=-1;
    std::vector<RenderEffect> mima_effects,sigma_effects;
    std::array<std::uint8_t,3> mima_palette0{};
    bool mima_background=false;
    std::array<RenderDeathEffect,3> marisa_death{},stones_death{};
    unsigned marisa_death_count=0,stones_death_count=0;
    MotionAnchor &at(MotionTrack track,unsigned slot){return anchors[offsets[unsigned(track)]+slot];}
    const MotionAnchor &at(MotionTrack track,unsigned slot)const{return anchors[offsets[unsigned(track)]+slot];}
    void record(MotionTrack track,unsigned slot,RenderPoint point,unsigned kind=0,int age=-1){
        at(track,slot)={point,kind,age,true};
    }
};

struct MotionSample {
    const MotionFrame *previous=nullptr;
    unsigned phase=motion_subpixels;
    RenderPoint at(MotionTrack track,unsigned slot,RenderPoint current,unsigned kind=0,int age=-1)const{
        if(previous){
            const auto &old=previous->at(track,slot);
            // Reused effect slots and coordinate resets are cuts, not movement.
            if(old.visible&&old.kind==kind&&(age<0||age>old.age)&&
               std::abs(old.point.x-current.x)<=64*256&&std::abs(old.point.y-current.y)<=64*256)
                return motion_lerp(old.point,current,phase);
            return current;
        }
        return current.snapped();
    }
    RenderPoint at(MotionTrack track,unsigned slot,int x,int y,unsigned kind=0,int age=-1)const{
        return at(track,slot,RenderPoint::pixels(x,y),kind,age);
    }
    const RenderEffect &effect(const std::vector<RenderEffect> &old,std::size_t count,std::size_t i,const RenderEffect &now)const{
        if(old.size()!=count)return now;
        const auto &p=old[i];
        return p.kind==now.kind&&p.color==now.color&&std::abs(p.x-now.x)<=32&&std::abs(p.y-now.y)<=32&&
            std::abs(p.a-now.a)<=32&&std::abs(p.b-now.b)<=32?p:now;
    }
};

// Exact integer blits for the original path, coverage-aware filtering for
// fractional placement. Transparent pixels contribute coverage, not black.
inline constexpr std::uint32_t render_clear=0x1000000;
struct SpriteClip {
    int left=32,top=16,right=416,bottom=384;
    bool overlaps(RenderPoint p,int width,int height)const{
        return p.x<right*256&&p.x+width*256>left*256&&
               p.y<bottom*256&&p.y+height*256>top*256;
    }
};

// Logical pools can retire a slot before its last texels leave the playfield.
// Keep only the outgoing artwork, independent of slot reuse and collisions.
class SpriteDepartures {
    struct Sprite {
        MotionTrack track;
        unsigned pattern;
        RenderPoint previous,current;
        int width,height;
        bool in_view()const{
            const SpriteClip clip;
            return clip.overlaps(previous,width,height)||clip.overlaps(current,width,height);
        }
    };
    std::vector<Sprite> sprites;
public:
    void leave(MotionTrack track,unsigned pattern,RenderPoint previous,RenderPoint current,int width,int height){
        const Sprite sprite{track,pattern,previous,current,width,height};
        if(sprite.in_view())sprites.push_back(sprite);
    }
    void advance(){
        for(auto &s:sprites){
            const RenderPoint next{2*s.current.x-s.previous.x,2*s.current.y-s.previous.y};
            s.previous=s.current;s.current=next;
        }
        std::erase_if(sprites,[](const Sprite &s){return !s.in_view();});
    }
    void clear(MotionTrack track){std::erase_if(sprites,[&](const Sprite &s){return s.track==track;});}
    template<class Draw>
    void draw(MotionTrack track,const MotionSample &motion,const Draw &draw)const{
        for(const auto &s:sprites)if(s.track==track)
            draw(s.pattern,motion.previous?motion_lerp(s.previous,s.current,motion.phase):s.current.snapped());
    }
};

template<class Texel>
void blit_motion_sprite(std::span<std::uint32_t> pixels,RenderPoint at,int width,int height,Texel texel,SpriteClip clip={},unsigned opacity=256){
    const int left=RenderPoint::floor(at.x),top=RenderPoint::floor(at.y);
    const unsigned fx=unsigned(at.x-left*256),fy=unsigned(at.y-top*256);
    const int first_x=std::max(0,clip.left-left),last_x=std::min(width+int(fx!=0),clip.right-left);
    const int first_y=std::max(0,clip.top-top),last_y=std::min(height+int(fy!=0),clip.bottom-top);
    if(first_x>=last_x||first_y>=last_y)return;
    constexpr unsigned rb=0x00ff00ff,g=0x0000ff00;
    if(!(fx|fy)){
        for(int y=first_y;y<last_y;++y){
            auto row=pixels.data()+(top+y)*640+left+first_x;
            for(int x=first_x;x<last_x;++x,++row){
                const auto rgb=texel(x,y);if(rgb==render_clear)continue;
                if(opacity==256)*row=rgb;
                else *row=((((rgb&rb)*opacity+(*row&rb)*(256-opacity))>>8)&rb)|
                          ((((rgb&g)*opacity+(*row&g)*(256-opacity))>>8)&g);
            }
        }
        return;
    }
    const unsigned w11=fx*fy/256,w10=fx-w11,w01=fy-w11,w00=256-fx-fy+w11;
    for(int y=first_y;y<last_y;++y){
        auto row=pixels.data()+(top+y)*640+left+first_x;
        for(int x=first_x;x<last_x;++x,++row){
            unsigned red_blue=0,green=0,coverage=0;
            const auto add=[&](int sx,int sy,unsigned weight){
                if(!weight||unsigned(sx)>=unsigned(width)||unsigned(sy)>=unsigned(height))return;
                const auto rgb=texel(sx,sy);
                if(rgb==render_clear)return;
                red_blue+=(rgb&rb)*weight;green+=(rgb&g)*weight;coverage+=weight;
            };
            add(x,y,w00);add(x-1,y,w10);add(x,y-1,w01);add(x-1,y-1,w11);
            if(coverage){
                if(opacity==256){
                    red_blue+=(*row&rb)*(256-coverage);green+=(*row&g)*(256-coverage);
                    *row=((red_blue>>8)&rb)|((green>>8)&g);
                }else{
                    const auto blend=[&](unsigned src,unsigned dst){
                        return (src*opacity+dst*(65536-coverage*opacity))>>16;
                    };
                    *row=blend(red_blue>>16,(*row>>16)&255)<<16 |
                         blend(green>>8,(*row>>8)&255)<<8 |
                         blend(red_blue&65535,*row&255);
                }
            }
        }
    }
}
}
