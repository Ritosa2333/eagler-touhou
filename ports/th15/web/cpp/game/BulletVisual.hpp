#pragma once
#include "BulletSpriteSource.hpp"
#include "AnmRenderer.hpp"
namespace th15 {
// Animation ownership for one pooled bullet. Collision and frame logic share
// the same visual fields rather than maintaining an unrelated display copy.
class BulletVisual {
    BulletState& state;Rng& random;BulletSpriteSource source;
    void push()noexcept;void pull()noexcept;
public:
    AnmVm body,overlay;AnmResource* resource=nullptr;float rate=1;
    std::string error;
    BulletVisual(BulletState& bullet,Rng& visual_random):state(bullet),random(visual_random),source(bullet){}
    ~BulletVisual(){clear_children();}
    bool clear_children();
    void restore_sprite_source()noexcept{body.sprite_source=&source;}
    bool bind(i32 script,i32 overlay_script);
    bool interrupt(i32 id)noexcept;
    bool spawn()noexcept{return interrupt(2);}
    bool hit()noexcept{return interrupt(1);}
    bool interrupt_overlay(i32 id)noexcept;
    // 1 destroys the animation; 0 keeps it; -1 reports an execution error.
    i32 tick(bool overlay_animation);
    bool draw(AnmRenderer&);
};
}
