#include "ItemSpirits.hpp"
namespace th17 {
ItemSpirits::ItemSpirits(AnmManager& a,EffectManager& e,Rng& game,Rng& visual,PlayerMotion& p,PlayerBounds& b,PlayerLifeSession& s,Timer& invulnerability,PlayerSpellStatus& spell,DamageSources& damage,ItemScoreState& score,ItemSpiritHost& h,i32 ch,i32 item_resource,i32 animal_resource):animations(a),game_random(game),visual_random(visual),player(p),bounds(b),session(s),world(h),character(ch),collection(s,score,p,h,ch),items(a,e,item_resource),spirits(a,game,*this,animal_resource),roaring(a,game,damage,p,s,invulnerability,spell,*this,animal_resource){
    items.sound=[this](i32 id){return world.sound(id,true);};items.immediate_sound=[this](i32 id){return world.sound(id,false);};
}
bool ItemSpirits::collect_spirit(i32 kind){if(!roaring.collect(kind)){error=roaring.error;return false;}before_player();return true;}
bool ItemSpirits::spirit(i32 kind,const Vec3& p,float angle){spirits.spawn(kind,p,angle,practice);if(spirits.error.empty())return true;error=spirits.error;return false;}
bool ItemSpirits::item(i32 kind,const Vec3& p,float angle,float speed,i32 delay,i32 sound){return spawn(kind,p,angle,speed,delay,sound);}
bool ItemSpirits::spawn(i32 kind,const Vec3& p,float angle,float speed,i32 delay,i32 sound){
    error.clear();ItemSpawnContext context{game_random,player.position,[this](i32 type,const Vec3& p,float angle){return spirit(type,p,angle);}};
    items.spawn(kind,p,angle,speed,delay,sound,&context);if(items.error.empty())return true;error=items.error;return false;
}
bool ItemSpirits::update_items(const ItemSpiritFrame& frame){
    error.clear();practice=frame.practice;ItemFrameContext context{player,bounds,collection,visual_random,frame.rate,frame.attraction_speed,character,frame.player_state,frame.bomb_state,frame.bomb_frame,frame.held,frame.hud_collect};
    if(items.update(context))return true;error=items.error;return false;
}
bool ItemSpirits::update_spirits(const ItemSpiritFrame& frame){
    error.clear();practice=frame.practice;if(!spirits.update(player.position,frame.rate)){error=spirits.error;return false;}
    if(!roaring.update(frame.rate,frame.hud_collect,frame.held)){error=roaring.error;return false;}before_player();return true;
}
void ItemSpirits::before_player()noexcept{session.roaring_flags=roaring.state.flags;session.roaring_countdown=roaring.state.countdown;}
void ItemSpirits::after_player()noexcept{roaring.state.flags=session.roaring_flags;roaring.state.countdown=session.roaring_countdown;}
bool ItemSpirits::end_roaring(bool death){if(!roaring.break_roaring(death)){error=roaring.error;return false;}before_player();return true;}
}
