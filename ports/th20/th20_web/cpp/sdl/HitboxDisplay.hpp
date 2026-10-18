#pragma once
namespace th20::source::player_entity {class Player;}
namespace th20::source::sprite {struct Controller;}
namespace th20::web {
void draw_always_hitbox(source::player_entity::Player&);
void clear_hitbox_display();
void draw_hitbox_layer(source::sprite::Controller&,int layer);
}
