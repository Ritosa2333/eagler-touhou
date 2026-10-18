#include "HitboxDisplay.hpp"
#include "../../../src/player_entity/owner.hpp"
#include "../../../src/effect_system/effect.hpp"
#include "../../../src/program_entry/program_entry.hpp"
#include "../../../src/sprite_renderer/binding.hpp"
#include "../../../src/sprite_renderer/dispatch.hpp"
#include "../../../src/sprite_renderer/anm_vm.hpp"
#include <emscripten.h>
#include "HitboxAnimation.hpp"
#include "../../../src/sprite_renderer/pool.hpp"
#include <unordered_map>
namespace {
bool enabled=false;
struct Display {th20::web::HitboxAnimation animation;unsigned native_focus=0;};
std::unordered_map<const th20::source::player_entity::Player*,Display> displays;
}
extern "C" __attribute__((export_name("sdl_always_hitbox"))) void sdl_always_hitbox(unsigned on){enabled=on!=0;}
namespace th20::web {
void clear_hitbox_display(){displays.clear();}
void draw_always_hitbox(source::player_entity::Player& player){
    if(!enabled||player.state!=1){displays.erase(&player);return;}
    using namespace source;
    auto* effects=static_cast<effects::Controller*>(player.context->objects_04[7]);
    auto* controller=program_entry::sprite_controller;
    if(!effects||!controller||!effects->files[0]||effects->files[0]->script_count<=19)return;
    auto& file=*effects->files[0];
    auto& display=displays[&player];
    if(player.handle_60c)display.native_focus=player.handle_60c;
    if(sprite::find_animation(*controller,display.native_focus)){display.animation.reset();return;}
    display.animation.sample(file,player.timers_644[0].current_f);
}
void draw_hitbox_layer(source::sprite::Controller& controller,int layer){
    if(layer!=18)return;
    using namespace source;
    for(auto& entry:displays){const auto& player=*entry.first;
        if(!enabled||player.state!=1||sprite::find_animation(controller,entry.second.native_focus))continue;
        const float scale=player.entity_flags&16u?(player.collision_expansion-1.f)*2.f+1.f:1.f;
        sprite::configure_animation_layer(controller,18,player.view_index);
        auto* effects=static_cast<effects::Controller*>(player.context->objects_04[7]);
        if(!effects||!effects->files[0])continue;
        auto markers=entry.second.animation.sample(*effects->files[0],player.timers_644[0].current_f);
        markers[1].base.vector_58={scale,scale};markers[1].base.flags[1]|=4;
        markers[1].vector_5bc=player.position_614;markers[0].vector_5bc={};
        sprite::attach_animation_parent(markers[0],&markers[1]);
        for(auto& marker:markers)sprite::draw_animation(controller,marker);
    }
}
}
