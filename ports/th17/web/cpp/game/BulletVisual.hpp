#pragma once
#include "BulletSpriteSource.hpp"
#include "AnmRenderer.hpp"
#include "AnmManager.hpp"
namespace th17 {
class BulletVisual {
    BulletState& state;Rng& random;BulletSpriteSource source;
    void push()noexcept;void pull()noexcept;
public:
    AnmVm body;AnmResource* resource=nullptr;AnmManager* manager=nullptr;u32 overlay_handle=0;float rate=1;
    std::string error;
    BulletVisual(BulletState& bullet,Rng& visual_random):state(bullet),random(visual_random),source(bullet){}
    ~BulletVisual(){clear_children();}
    bool clear_children();void restore_sprite_source()noexcept{body.sprite_source=&source;}
    bool bind(i32 script,i32 overlay_script);bool interrupt(i32 id)noexcept;
    bool spawn()noexcept{return interrupt(2);}bool hit()noexcept{return interrupt(1);}
    bool interrupt_overlay(i32 id)noexcept;i32 tick(bool overlay_animation);bool draw(AnmRenderer&);
};
}
