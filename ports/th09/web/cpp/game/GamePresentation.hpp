#pragma once
#include "GameWorld.hpp"
#include "StageRenderer.hpp"
#include "AsciiRenderer.hpp"
#include "InGameMenu.hpp"
#include "HitboxDisplay.hpp"
namespace th09 {
struct GameMedia {
    virtual ~GameMedia()=default;
    virtual bool render_text(AnmVm&,const char*,u32,u32)=0;
    virtual void sound(i32,i32)=0;
    virtual void positioned_sound(i32,float)=0;
    virtual void music(i32)=0;
    virtual void fade_music()=0;
    virtual void encountered(i32)=0;
    virtual void defeated(i32)=0;
    virtual void overlay()=0;
};
class GamePresentation final:public WorldPresentation,public AsciiOutput {
    ZunGraphics& backend;GameMedia& media;
    std::array<presentation::HitboxDisplay,2> hitbox_displays;
public:
    AnmRenderer renderer;StageRenderer stages;AsciiRenderer ascii;GameWorld* world=nullptr;
    Vec2 offsets[3]{};i32 active_field=2;BackgroundDrawState stage_state[2];
    GamePresentation(ZunGraphics& g,GameResources& r,GameMedia& m):backend(g),media(m),renderer(g),stages(renderer),ascii(r,*this){renderer.state.fogMode=FogMode::Linear;}
    static Viewport viewport(i32 side){return side<2?Viewport{u32(16+320*side),16,288,448,0,1}:Viewport{};}
    void begin_field(i32)override;
    void animation(AnmVm&,BattleSprite)override;
    void hitbox(i32 side,AnmLoaded* file,const Vec3& position,i32 frame)override{
        if(!file){hitbox_displays[side].reset();return;}
        auto marker=hitbox_displays[side].sample(*file,position,frame);renderer.draw_2d(marker);
    }
    void colored(const AnmVm*,const AttackColorVertex*,u32,BattleGeometry,bool)override;
    void textured(const AnmVm&,const AttackTextureVertex*,u32,BattleGeometry)override;
    void text(AnmVm& a,const char* s,u32 color,u32 shadow)override{renderer.flush();media.render_text(a,s,color,shadow);}
    void sound(i32 id,i32 pan)override{media.sound(id,pan);}
    void positioned_sound(i32,i32 id,float x)override{media.positioned_sound(id,x);}
    void music(i32 track)override{media.music(track);}
    void fade_music()override{media.fade_music();}
    void background(Background&,const Background&,i32,bool)override;
    void score_popup(i32 side,const Vec3& p,i32 value,u32 color)override{ascii.popup(side,p,value,color);}
    void draw_score_popups()override;
    void draw_overlay()override{ascii.draw_text();ascii.clear_text();media.overlay();}
    void number(const Vec3& p,i32 width,i32 value)override{ascii.number(p,width,value);}
    void clock(const Vec3& p,i32 stage,i32 min,i32 sec)override{ascii.clock(p,stage,min,sec);}
    void shake(i32 side,float x,float y)override{if(side>=0&&side<3)offsets[side]={x,y};}
    void rectangle(float,float,float,float,u32,bool)override;
    void encountered(i32 n)override{media.encountered(n);}
    void defeated(i32 n)override{media.defeated(n);}
    void ascii_view(i32 side)override{begin_field(side);}
    void ascii_sprite(AnmVm& a)override{renderer.draw_no_rotation(a);}
    void begin_frame(){begin_field(2);renderer.clear(true,true,0xff000000);}
    void finish_frame(){renderer.flush();}
};
}
