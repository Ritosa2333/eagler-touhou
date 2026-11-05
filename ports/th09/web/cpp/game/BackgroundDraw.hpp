#pragma once
#include "Background.hpp"
#include "AttackTransfer.hpp"
#include "ZunGraphics.hpp"
namespace th09 {
struct BackgroundDrawState {
    bool clear_pending=false,preserve_tint=false,capture_positions=false;
};
struct BackgroundDrawServices {
    PlayfieldGeometry geometry;i32 side=0;bool fog_supported=true;
    u32 tint=0x80808080;bool tint_enabled=false;
    virtual ~BackgroundDrawServices()=default;
    virtual void select_field(i32)=0;
    virtual void flush()=0;
    virtual void clear(bool color,bool depth,u32)=0;
    virtual void rectangle(float left,float top,float right,float bottom,u32)=0;
    virtual void draw_animation(AnmVm&)=0;
    virtual void models(Background&,i32 layer)=0;
    virtual void depth_compare(Compare)=0;
    virtual void fog(bool)=0;
    virtual void fog_color(u32)=0;
    virtual void fog_range(float near_plane,float far_plane)=0;
    virtual void texture_mode(bool world)=0;
    virtual void screen_camera()=0;
    virtual void custom_boss(Background&)=0;
    u32 tinted(u32 color,bool alpha)const noexcept;
};
class BackgroundDraw {
public:
    static void base(Background&,BackgroundDrawState&,BackgroundDrawServices&);
    static void overlay(Background&,BackgroundDrawState&,BackgroundDrawServices&);
};
}
