#include "PlayerShotSystem.hpp"
namespace th08 {
bool PlayerShotSystem::load_shots(bool focused,const u8* data,u32 size){if(loaded[focused])return false;loaded[focused]=resources[focused].load(data,size);return loaded[focused];}
bool PlayerShotSystem::load_animation(const u8* data,u32 size){if(animation_loaded)return false;animation_loaded=animations.load(5,data,size)!=nullptr;return animation_loaded;}
void PlayerShotSystem::reset(){*state=PlayerShotsState{};state->shooting_timer.set(-1);animations.release(5);for(auto& resource:resources)resource.release();loaded[0]=loaded[1]=animation_loaded=failed=false;requests.clear();firing={};timing={};power=0;bomb_ready=false;renderer=nullptr;}
void PlayerShotSystem::fire(i32 frame){
    const auto& resource=resources[state->focused!=0];const i32 selected=resource.select(power,state->character,state->bomb,firing.bomb_type,bomb_ready);
    if(selected<0){failed=true;return;}shots.emit(*resource.stream(selected),frame);failed|=shots.failure!=PlayerShots::Failure::None;
}
bool PlayerShotSystem::emit(i32 frame){
    if(!ready())return false;failed=false;executor.timing=shots.timing=timing;fire(frame);return !failed;
}
bool PlayerShotSystem::update(){
    if(!ready())return false;failed=false;executor.timing=shots.timing=timing;
    if(!shots.update())return false;
    firing.bomb=state->bomb;firing.character=state->character;firing.player_state=state->player_state;firing.gui_blocked=state->gui_blocked;
    update_shooting(state->shooting_timer,firing,timing,*this);return !failed;
}
i32 PlayerShotSystem::damage(const Vec3& position,const Vec3& size,i32& time_items,i32* bomb_hit){if(!ready())return 0;executor.timing=shots.timing=timing;const i32 result=shots.damage(position,size,time_items,bomb_hit);failed|=shots.failure!=PlayerShots::Failure::None;return result;}
bool PlayerShotSystem::draw(AnmRenderer& output,bool impact,const Vec2& offset){if(!ready())return false;renderer=&output;shots.draw(impact,offset);renderer=nullptr;failed|=shots.failure!=PlayerShots::Failure::None;return !failed;}
}
