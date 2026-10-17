#pragma once
#include "BackgroundView.hpp"
#include "EffectSystem.hpp"
namespace th08 {
class SpellBackground {
    BackgroundState& state;BackgroundView& view;EffectSystem& effects;AnmExecutor& anm;
    static void callback(BackgroundState&,AnmRenderer&,void*);
public:
    SpellBackground(BackgroundState& s,BackgroundView& v,EffectSystem& e,AnmExecutor& a):state(s),view(v),effects(e),anm(a){view.callback_context=this;}
    bool begin(u32 variant,const Vec3& boss_position);
    void end();void draw(AnmRenderer&);
};
}
