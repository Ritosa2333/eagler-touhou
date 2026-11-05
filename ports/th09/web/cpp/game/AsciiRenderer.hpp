#pragma once
#include "GameResources.hpp"
#include "AttackTransfer.hpp"
namespace th09 {
struct AsciiOutput {
    virtual ~AsciiOutput()=default;
    virtual void ascii_view(i32 side)=0;
    virtual void ascii_sprite(AnmVm&)=0;
};
struct ScorePopup {
    u8 digits[12]{};Vec3 position;u32 color=0;Timer time;
    u32 reserved[2]{};u8 active=0,count=0,padding[6]{};
};
static_assert(sizeof(ScorePopup)==56);
struct AsciiEntry {
    char text[64]{};Vec3 position;u32 color=0;Vec2 scale;
    u32 reserved=0;i32 field_view=0;
};
static_assert(sizeof(AsciiEntry)==96);
struct PopupFrame {
    FrameTiming timing;u32 game_flags=0,field_flags[2]{};
    bool paused=false,game_over=false;
};
class AsciiRenderer {
    GameResources& resources;AsciiOutput& output;
public:
    AnmVm glyph,digit;std::array<AsciiEntry,256> queue;
    std::array<std::array<ScorePopup,100>,2> popups;
    u32 count=0,color=0xffffffff;Vec2 scale{1,1};i32 field_view=0,spacing=9,popup_cursor=0;
    AsciiRenderer(GameResources& r,AsciiOutput& o):resources(r),output(o){}
    bool initialize();
    void text(const Vec3&,const char*);
    void number(const Vec3&,i32 width,i32 value);
    void clock(const Vec3&,i32 stage,i32 minutes,i32 seconds);
    void popup(i32 side,const Vec3&,i32 value,u32 color);
    void update(const PopupFrame&);
    void draw_popups(i32 side,const PlayfieldGeometry&,const Vec3& player);
    void draw_text();
    void clear_text(){count=0;}
};
}
