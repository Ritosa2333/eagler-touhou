#pragma once
#include "render_motion.hpp"
#include <algorithm>
#include <array>
#include <bitset>
#include <cstdint>
#include <limits>
#include <optional>
#include <span>

namespace soew {
enum class HitboxMode { Off, Self, All };

struct CollisionPoint {
    int x,y;
    constexpr bool operator==(const CollisionPoint &) const = default;
};
inline constexpr CollisionPoint player_projectile_offset{16,24};

struct CollisionAxis {
    static constexpr int unbounded_low=std::numeric_limits<int>::min();
    static constexpr int unbounded_high=std::numeric_limits<int>::max();
    int low,high;
    bool low_closed,high_closed;

    constexpr std::int64_t first() const {return std::int64_t(low)+!low_closed;}
    constexpr std::int64_t last() const {return std::int64_t(high)-!high_closed;}
    constexpr bool contains(int n) const {return (low_closed?n>=low:n>low)&&(high_closed?n<=high:n<high);}
    static constexpr CollisionAxis open(int lo,int hi){return {lo,hi,false,false};}
    static constexpr CollisionAxis half_open(int lo,int hi){return {lo,hi,true,false};}
    static constexpr CollisionAxis closed(int lo,int hi){return {lo,hi,true,true};}
    static constexpr CollisionAxis at_least(int lo){return closed(lo,unbounded_high);}
    static constexpr CollisionAxis any(){return closed(unbounded_low,unbounded_high);}
};

struct CollisionBounds {
    CollisionAxis x,y;
    constexpr bool contains(CollisionPoint p) const {return x.contains(p.x)&&y.contains(p.y);}
    constexpr bool empty() const {return x.first()>x.last()||y.first()>y.last();}
    static constexpr CollisionBounds open(int left,int top,int right,int bottom){
        return {CollisionAxis::open(left,right),CollisionAxis::open(top,bottom)};
    }
};

constexpr CollisionBounds bullet_hit_window(CollisionPoint player,bool pellet){
    return {CollisionAxis::half_open(player.x+(pellet?7:-3),player.x+(pellet?17:19)),
            CollisionAxis::closed(player.y+(pellet?12:4),player.y+(pellet?22:24))};
}

constexpr int bullet_center_offset(bool pellet){return pellet?4:8;}

inline void draw_player_focus_point(std::span<std::uint32_t> pixels,CollisionPoint player,int view_shift_x=0){
    constexpr SpriteClip clip;
    constexpr std::uint32_t rose=0xff456b,ink=0x230d19;
    const auto dot=[&](int x,int y,std::uint32_t color){
        x+=view_shift_x;
        if(x>=clip.left&&x<clip.right&&y>=clip.top&&y<clip.bottom)pixels[y*640+x]=color;
    };
    const auto box=bullet_hit_window(player,true);
    constexpr int offset=bullet_center_offset(true);
    const int left=int(box.x.first())+offset,right=int(box.x.last())+offset;
    const int top=int(box.y.first())+offset,bottom=int(box.y.last())+offset;
    // Rose corners mark the exact window for white pellet centers; the core
    // is a position reference, not a circular hitbox.
    for(int x:{left,right})for(int y:{top,bottom}){
        const int dx=x==left?1:-1,dy=y==top?1:-1;
        for(int i=0;i<3;++i){dot(x+i*dx,y-dy,ink);dot(x-dx,y+i*dy,ink);}
        dot(x-dx,y-dy,ink);
        for(int i=0;i<3;++i){dot(x+i*dx,y,rose);dot(x,y+i*dy,rose);}
    }
    const int cx=player.x+player_projectile_offset.x,cy=player.y+player_projectile_offset.y;
    for(int dy=-2;dy<=2;++dy)for(int dx=-2;dx<=2;++dx){
        const int d=dx*dx+dy*dy;
        if(d<=5)dot(cx+dx,cy+dy,d<=1?0xffffff:d<=4?rose:ink);
    }
}

enum class CollisionSample { Projectile, Current, Front, FrontXCurrentY, Count };
enum class CollisionSource { Laser, Road, MeiraBody, MeiraMarker, MarisaBody, MarisaOrb,
                             MimaBody, MimaOrb, MimaBeam, SigmaBody, SigmaBurst, SigmaBeam };

struct CollisionFrame {
    struct BulletPoint {CollisionPoint position;bool pellet;};
    struct Hazard {CollisionSource source;CollisionBounds bounds;CollisionSample sample;CollisionPoint player;};

    // Maximum simultaneous check sites, including every slot of each native pool.
    static constexpr unsigned bullet_capacity=150;
    static constexpr unsigned hazard_capacity=12+25+(1+40)+(1+4)+(1+8+1)+(1+16+1);
    std::array<BulletPoint,bullet_capacity> bullets{};
    std::bitset<bullet_capacity> projectile_contacts;
    std::array<Hazard,hazard_capacity> hazards{};
    std::array<std::optional<CollisionPoint>,static_cast<unsigned>(CollisionSample::Count)> player{};
    unsigned bullet_count=0,hazard_count=0;
    HitboxMode mode=HitboxMode::Off;
    const void *scene=nullptr;
    std::uint64_t completed_tick=0;
    std::uint32_t logic_frame=0;
    unsigned page=0;
    bool shake=false;

    void begin(HitboxMode value,std::uint32_t frame,unsigned back){
        scene=nullptr;mode=value;logic_frame=frame;page=back;
        bullet_count=hazard_count=0;player={};projectile_contacts.reset();
    }
    void publish(const void *owner,std::uint64_t tick,bool shifted){
        scene=owner;completed_tick=tick;shake=shifted;
    }
    void sample_player(CollisionSample sample,CollisionPoint point){
        if(mode!=HitboxMode::Off)player[static_cast<unsigned>(sample)]=point;
    }
    void bullet(CollisionPoint pos,bool pellet){
        if(mode==HitboxMode::All&&bullet_count<bullets.size())bullets[bullet_count++]={pos,pellet};
    }
    void hazard(CollisionSource source,CollisionBounds bounds,CollisionSample sample,CollisionPoint point){
        if(bounds.empty())return;
        // Only the bullet check provides the overlay's common player reference.
        if(sample!=CollisionSample::Projectile)sample_player(sample,point);
        if(mode==HitboxMode::All&&hazard_count<hazards.size())hazards[hazard_count++]={source,bounds,sample,point};
    }
};

inline void draw_collision_frame(std::span<std::uint32_t> pixels,const CollisionFrame &frame,bool reduced_effects=false,
                                 int view_shift_x=0,int view_shift_y=0){
    constexpr int width=640,height=400,left=32,right=416,top=16,bottom=384;
    constexpr std::uint32_t cyan=0x00ffff;
    const auto &player=frame.player[static_cast<unsigned>(CollisionSample::Projectile)];
    if(frame.mode==HitboxMode::Off||!player||pixels.size()<width*height)return;
    // This is the bullet check's sampled player position, not a sprite anchor.
    const std::int64_t center_x=std::int64_t(player->x)+player_projectile_offset.x,
                       center_y=std::int64_t(player->y)+player_projectile_offset.y;
    const auto dot=[&](std::int64_t x,std::int64_t y){
        if(x<left||x>=right||y<top||y>=bottom)return;
        // The battle renderer rotates each visible playfield row left one word.
        if(frame.shake&&!reduced_effects)x=x>=left+16?x-16:x+right-left-16;
        pixels[y*width+x]=cyan;
    };
    const auto outline=[&](CollisionBounds bounds,std::int64_t dx,std::int64_t dy){
        const auto first=[](CollisionAxis axis,std::int64_t shift){
            return axis.low==CollisionAxis::unbounded_low?std::numeric_limits<std::int64_t>::min():axis.first()+shift;
        };
        const auto last=[](CollisionAxis axis,std::int64_t shift){
            return axis.high==CollisionAxis::unbounded_high?std::numeric_limits<std::int64_t>::max():axis.last()+shift;
        };
        const auto x1=first(bounds.x,dx+view_shift_x),x2=last(bounds.x,dx+view_shift_x);
        const auto y1=first(bounds.y,dy+view_shift_y),y2=last(bounds.y,dy+view_shift_y);
        const auto clip_x1=std::max<std::int64_t>(x1,left),clip_x2=std::min<std::int64_t>(x2,right-1);
        const auto clip_y1=std::max<std::int64_t>(y1,top),clip_y2=std::min<std::int64_t>(y2,bottom-1);
        if(y1>=top&&y1<bottom)for(auto x=clip_x1;x<=clip_x2;++x)dot(x,y1);
        if(y2!=y1&&y2>=top&&y2<bottom)for(auto x=clip_x1;x<=clip_x2;++x)dot(x,y2);
        if(x1>=left&&x1<right)for(auto y=clip_y1;y<=clip_y2;++y)dot(x1,y);
        if(x2!=x1&&x2>=left&&x2<right)for(auto y=clip_y1;y<=clip_y2;++y)dot(x2,y);
    };
    if(frame.mode==HitboxMode::All){
        for(unsigned i=0;i<frame.bullet_count;++i){
            const auto &bullet=frame.bullets[i];const int offset=bullet_center_offset(bullet.pellet);
            dot(std::int64_t(bullet.position.x)+offset+view_shift_x,std::int64_t(bullet.position.y)+offset+view_shift_y);
        }
        for(unsigned i=0;i<frame.hazard_count;++i){
            const auto &hazard=frame.hazards[i];
            outline(hazard.bounds,center_x-hazard.player.x,center_y-hazard.player.y);
        }
    }
    for(bool pellet:{true,false})outline(bullet_hit_window(*player,pellet),bullet_center_offset(pellet),bullet_center_offset(pellet));
    dot(center_x+view_shift_x,center_y+view_shift_y);
}
}
