#pragma once
#include "FighterPattern.hpp"
#include "QuadTransform.hpp"
#include "ZunGraphics.hpp"
#include <deque>
namespace th075 {
struct SpriteImage {u32 texture=0,width=0,height=0;};
struct SpriteResources {virtual ~SpriteResources()=default;virtual SpriteImage image(i16 sprite)=0;};
struct SpritePose {
    bool active=false;u8 flags=0;u32 color=0xffffffff;
    std::array<i16,3> projection{};
    std::array<float,3> scale{1,1,1},rotation{},offset{};
    u8 channel(u32 shift)const{return u8(color>>shift);}
    void set_channel(u32 shift,u8 value){color=(color&~(255u<<shift))|(u32(value)<<shift);}
};
struct SpriteTrail {
    i16 sprite=0;QuadPoints points{};float worldX=0,worldY=0;
    std::array<float,3> projection{};u32 color=0xffffffff;u8 blend=1;
};
class SpriteTrails {
public:
    std::deque<SpriteTrail> entries;i8 interval=1;i16 maximum=32;u32 colorMask=0xffffff;
    void append(const SpriteTrail&);
    void remove_front();
    void clear(){entries.clear();}
};
struct ShadowStyle {std::array<float,3> scale{1,0.3f,1},rotation{160,0,0},projection{};u32 color=0x80000000;};
struct TrailPolicy {bool disabled=false,trimIdle=false;};
class SpriteRenderer {
    ZunGraphics& graphics;SpriteResources& resources;
    void set_blend(u8 visualMode);
    void draw(const SpriteTrail&,const SpriteImage&);
public:
    SpriteRenderer(ZunGraphics& g,SpriteResources& r):graphics(g),resources(r){}
    void fighter(const ImageFrame&,const FighterMotion&,const SpritePose&,i16 action,i16 freeze,i8 flashTimer,i32 noticeTimer,SpriteTrails&,TrailPolicy,float cameraX,float cameraY);
    void object(const ImageFrame&,const FighterMotion&,const SpritePose&,float cameraX,float cameraY);
    void screen(const ImageFrame&,const FighterMotion&,const SpritePose&);
    void shadow(const ImageFrame&,const FighterMotion&,const SpritePose&,const ShadowStyle&,float cameraX,float cameraY,float groundOffset);
    void trails(const SpriteTrails&,float cameraX,float cameraY);
};
}
