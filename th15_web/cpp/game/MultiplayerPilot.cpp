#if TH15_MULTIPLAYER
#include "GameBattle.hpp"
#include "MultiplayerPilot.hpp"
#include "BattleSceneBindings.hpp"
#include <algorithm>
namespace th15 {
MultiplayerPilot::MultiplayerPilot(GameBattle& w,unsigned s,int ch):world(w),seat(s),character(ch),owned_session(w.session),owned_life_piece_tier(w.score.life_piece_tier),session(s?owned_session:w.session),score(w.score){}
bool MultiplayerPilot::initialize(const ShtResource& sht,int bank){
    if(seat){
        owned_player=std::make_unique<Player>(sht,world.animations,world.effects,world.game_random,world.visual_random,session,world.spell,*this,character,bank,world.resources.effect);
        player=owned_player.get();
        if(!player->initialize()||!player->reset_for_stage())return world.fail(player->error.empty()?player->options.error:player->error);
        player->damage.maximum_damage=player->resource.header.maximum_damage();
        context=std::make_unique<BombContext>(BombContext{world.animations,player->motion,player->life,player->frame.bounds,player->resource.header,session,world.spell,world.bomb_enemies,*this,bank});
        if(!rebuild_bomb())return false;
        owned_collection=std::make_unique<ItemCollection>(session,score,player->motion,*this,character,&owned_life_piece_tier);collection=owned_collection.get();
    }else{player=world.player.get();bomb=world.bomb.get();collection=world.collection.get();}
    const float x=(float(seat)-float(world.mp_options.seat_count-1)*.5f)*48.f;
    player->motion.position={x,400,0};player->motion.position_fixed={i32(x*128),51200};player->motion.snap_options();
    return true;
}
bool MultiplayerPilot::rebuild_bomb(){
    if(!seat){bomb=world.bomb.get();return true;}
    if(bomb){if(!bomb->stop())return world.fail(bomb->error);auto retire=[&](u32 h){return world.animations.retire(h)||world.fail(world.animations.error);};
      switch(character){case 0:if(!retire(static_cast<BombReimu&>(*bomb).aura))return false;break;
      case 1:{auto& b=static_cast<BombMarisa&>(*bomb);if(!retire(b.beam)||!retire(b.aura))return false;break;}
      case 2:{auto& b=static_cast<BombSanae&>(*bomb);if(!retire(b.field)||!retire(b.aura))return false;break;}
      case 3:{auto& b=static_cast<BombReisen&>(*bomb);if(!retire(b.barrier)||!retire(b.aura))return false;break;}}
    }
    switch(character){
    case 0:owned_bomb=std::make_unique<BombReimu>(*context,player->damage,&world.enemy_world);break;
    case 1:owned_bomb=std::make_unique<BombMarisa>(*context,player->damage);break;
    case 2:owned_bomb=std::make_unique<BombSanae>(*context,world.effects,world.game_random,world.visual_random,player->damage);break;
    case 3:owned_bomb=std::make_unique<BombReisen>(*context);break;
    default:return world.fail("Invalid multiplayer character");
    }bomb=owned_bomb.get();return true;
}
bool MultiplayerPilot::alive()const noexcept{return player&&!ghost&&player->life.state!=2&&player->life.state!=4;}
bool MultiplayerPilot::refresh(PlayerFrameContext& c){
    const auto& f=world.input;c.hud_available=f.hud_available;c.hud_bomb_active=f.hud_collect||world.dialogue_present();c.enemy_present=true;c.enemies=&world.enemy_world;c.game_flags=f.game_flags;c.focus_allowed=world.scene?world.enemy_world.enemy_count!=0:f.focus_allowed;c.background_delta=f.background_delta;
    player->shot_context.screen_origin=world.environment.playfield_origin;return true;
}
bool MultiplayerPilot::tick(){
    if(ghost)return true;
    keys.update(world.mp_options.challenge?input.held&~2u:input.held);
    PlayerFrameContext c;c.held=keys.held;c.pressed=keys.pressed;refresh(c);
    // The target is transmitted as an absolute field coordinate. Never derive
    // movement from a sender's pre-delay simulation state.
    player->motion.touch={input.touch_mode,input.touch_x,input.touch_y};
    if(!player->update(c,world.input.rate))return world.fail(player->error);
    return true;
}
bool MultiplayerPilot::tick_bomb(){
    if(ghost)return true;
    if(context){context->hud_available=world.input.hud_available;context->hud_collect=world.input.hud_collect||world.dialogue_present();}
    return bomb->update(world.input.rate)||world.fail(bomb->error);
}
bool MultiplayerPilot::reset_stage(){
    session.bomb_state=0;keys={};input={};
    if(!rebuild_bomb())return false;
    return ghost||player->reset_for_stage()||world.fail(player->error);
}
bool MultiplayerPilot::allow_bomb(){return !ghost&&!world.mp_options.challenge&&bomb&&bomb->allowed();}
bool MultiplayerPilot::begin_bomb(){
    if(!allow_bomb()||!bomb->begin())return false;
    world.enemy_world.counters[1]=world.bomb_enemies.uses;world.enemy_world.counters[2]=world.bomb_enemies.chain;
    // Non-shield bombs protect all operable teammates; Reisen's shields stay personal.
    if(character!=3)for(unsigned s=0;s<world.mp_options.seat_count;++s)if(s!=seat&&!world.pilots[s]->ghost)world.pilots[s]->player->life.invulnerability.set(180);
    return true;
}
bool MultiplayerPilot::cancel_lasers_near(const Vec3& p,float r,i32 k,bool h){return world.laser_scene->cancel_circle(p,r,k,h)>=0;}
bool MultiplayerPilot::cancel_bullets_near(const Vec3& p,float r,i32 k){return world.bullet_scene->cancel_circle(p,r,k,false);}
bool MultiplayerPilot::cancel_lasers(i32 k,bool h){return world.laser_scene->cancel_all(k,h);}
bool MultiplayerPilot::item(i32 k,const Vec3& p,float a,float s){return world.spawn_item(k,p,a,s);}
bool MultiplayerPilot::options_changed(){return player->configure_options();}
bool MultiplayerPilot::game_over(){
    ghost=true;if(bomb&&!bomb->stop())return false;
    for(auto& source:player->damage.sources)source.flags=0;
    for(auto& shot:player->shots.shots)shot.state=0;
    bool live=false;for(unsigned s=0;s<world.mp_options.seat_count;++s)live|=!world.pilots[s]->ghost;
    return live||world.check(world.game_over_begin?world.game_over_begin():world.presentation.game_over(),"Multiplayer game-over failed");
}
bool MultiplayerPilot::respawn_damage(const Vec3& p,float r,float g,i32 t,i32 v){return player->damage.circle(p,r,g,t,v)>0;}
bool MultiplayerPilot::bomb_hud(i32 b,i32 p){return seat!=0||world.bomb_hud(b,p);}
bool MultiplayerPilot::life_hud(i32 l,i32 p){return seat!=0||world.life_hud(l,p);}
bool MultiplayerPilot::stop_sound(i32 id){return world.sound(id,0,PlayerShots::SoundAction::stop,true);}
bool MultiplayerPilot::sound(i32 id,float p,PlayerShots::SoundAction a){return world.sound(id,p,a);}
bool MultiplayerPilot::hit_sound(i32 id){return world.sound(id,0,PlayerShots::SoundAction::play,true);}
bool MultiplayerPilot::sound(i32 id,bool q){return world.sound(id,0,PlayerShots::SoundAction::play,q);}
bool MultiplayerPilot::popup(const Vec3& p,i32 v,u32 c){return world.popup(p,v,c);}
bool MultiplayerPilot::notice(i32 id){return world.check(world.presentation.notice(id),"Multiplayer item notice failed");}
bool GameBattle::mp_reward_item(const ItemState& item){
    for(unsigned s=0;s<mp_options.seat_count;++s){auto& p=*pilots[s];if(p.ghost||p.session.extra_lives<0)continue;
        if(!p.collection->award(item))return fail(p.collection->error);}
    return true;
}
bool MultiplayerPilot::cancel_bullets(const Vec3& p,float r,i32 k){return cancel_bullets_near(p,r,k);}
bool MultiplayerPilot::cancel_lasers(const Vec3& p,float r,i32 k,bool h){return cancel_lasers_near(p,r,k,h);}
bool MultiplayerPilot::cancel_bullets_rectangle(const Vec3& p,const Vec2& s,float a,i32 k){return world.bullet_scene->cancel_rectangle(p,s,a,k);}
bool MultiplayerPilot::cancel_lasers_rectangle(const Vec3& p,const Vec2& s,float a,i32 k,bool h){return h&&world.laser_scene->cancel_rectangle(p,s,a,k)>=0;}
bool MultiplayerPilot::shake(const ScreenShakeSpec& s){return world.presentation.shake(s);}
bool MultiplayerPilot::nudge(const ScreenNudgeSpec& s){return world.presentation.nudge(s);}
bool GameBattle::mp_enable(const MultiplayerOptions& o,const std::array<ShtResource,4>& shots,const std::array<int,4>& banks){
    if(!initialized||mp_enabled||!o.valid()||o.selections[0]!=character||(session.mode_flags&0x300))return fail("Invalid TH15 Legacy multiplayer setup");
    mp_options=o;mp_enabled=true;mp_item_targets.fill(-1);
    for(unsigned s=0;s<o.seat_count;++s)pilots[s]=std::make_unique<MultiplayerPilot>(*this,s,o.selections[s]);
    for(unsigned s=0;s<o.seat_count;++s)if(!pilots[s]->initialize(shots[o.selections[s]],banks[o.selections[s]]))return false;
    enemy_world.multiplayer_target=[this](const Vec3& p){return mp_target(p);};bullet_scene->manager.multiplayer_target=enemy_world.multiplayer_target;return true;
}
int GameBattle::mp_item_owner(const ItemState& item)const{
    const bool power=item.kind==1||item.kind==3||item.kind==8;
    const auto contains=[](const PlayerBox& box,const Vec3& p){return p.x>=box.minimum.x&&p.x<=box.maximum.x&&p.y>=box.minimum.y&&p.y<=box.maximum.y;};
    auto eligible=[&](const MultiplayerPilot& p){
        if(!p.alive()||item.state==5||(item.state==1&&item.delay>0))return false;
        if(item.state==3||item.state==4)return true;
        if(item.state==1&&(p.player->motion.position.y<float(p.character==1?148:128)||(p.session.bomb_state==1&&p.bomb->age.current<60)||input.hud_collect||dialogue_present()))return true;
        if(item.state==2&&float(item.velocity.y+input.rate*.03f)>=0)return true;
        if(item.state==2&&(item.kind==13||item.kind==14||item.kind==15))return false;
        auto position=item.position;if(item.state==1||item.state==2){const float scale=item.state==1?items->motion_scale:1.f;position.x+=float(item.velocity.x*input.rate)*scale;position.y+=float(item.velocity.y*input.rate)*scale;if(position.y>472)return false;}
        if(contains(p.player->frame.bounds.item_near,position))return true;
        const bool special=item.kind==9||item.kind==10||item.kind==11||item.kind==13||item.kind==14||item.kind==15;
        return !special&&contains(p.keys.held&8?p.player->frame.bounds.graze:p.player->frame.bounds.item_full,position);
    };
    auto nearest=[&](bool needs_power){int chosen=-1;double best=0;for(unsigned s=0;s<mp_options.seat_count;++s){const auto& p=*pilots[s];if((needs_power&&p.session.power>=p.score.max_power)||!eligible(p))continue;
        const double dx=double(item.position.x)-p.player->motion.position.x,dy=double(item.position.y)-p.player->motion.position.y,d=dx*dx+dy*dy;if(chosen<0||d<best){chosen=int(s);best=d;}}return chosen;};
    int target=nearest(power);return target<0&&power?nearest(false):target;
}
void GameBattle::mp_input(const std::array<MultiplayerInput,3>& in){for(unsigned s=0;s<mp_options.seat_count;++s)pilots[s]->input=in[s];}
const PlayerCollision& GameBattle::mp_collision(unsigned s){
    auto& p=*pilots[s]->player;auto& c=p.frame.collision;
    c.position={p.motion.position.x,p.motion.position.y};c.state=pilots[s]->ghost?2:p.life.state;c.invulnerability=p.life.invulnerability.current;
    c.radius=p.resource.header.hitbox;c.enlarged=p.motion.behavior_flags&16;c.size_multiplier=p.motion.enlargement;
    c.laser_half_size={p.frame.bounds.hit_half_size.x,p.frame.bounds.hit_half_size.y};c.bomb_active=input.hud_collect||dialogue_present();return c;
}
int GameBattle::mp_nearest(const Vec3& p,bool power)const{
    int chosen=-1;double best=0;
    for(unsigned s=0;s<mp_options.seat_count;++s){const auto& pilot=*pilots[s];if(!pilot.alive()||(power&&pilot.session.power>=pilot.score.max_power))continue;
        const auto& q=pilot.player->motion.position;const double x=double(p.x)-q.x,y=double(p.y)-q.y,d=x*x+y*y;
        if(chosen<0||d<best){chosen=int(s);best=d;}}
    return chosen;
}
Vec3 GameBattle::mp_target(const Vec3& p)const{const auto s=mp_nearest(p);return s<0?Vec3{0,400,0}:pilots[s]->player->motion.position;}
bool GameBattle::mp_damage(const DamageQuery& q,DamageResult& out){
    out={};for(unsigned s=0;s<mp_options.seat_count;++s){auto& p=*pilots[s];if(p.ghost)continue;
        const bool changed=p.player->frame.input_age.current!=p.player->frame.input_age.previous;
        const i32 before=p.player->damage.score;const auto hit=p.player->damage.query(q,changed,0);if(!p.player->damage.error.empty())return fail(p.player->damage.error);
        const i32 gain=wrapping_sub(p.player->damage.score,before);
        if(s)player->damage.score=std::min(wrapping_add(player->damage.score,gain),999999999);
        score.score=std::min(wrapping_add(score.score,gain),999999999);
        out.amount=wrapping_add(out.amount,hit.amount);out.direct|=hit.direct;if(hit.position_written){out.position=hit.position;out.position_written=true;}}
    return true;
}
bool GameBattle::mp_draw(AnmRenderer& r){for(unsigned s=0;s<mp_options.seat_count;++s)if(!pilots[s]->ghost&&!pilots[s]->player->draw(r))return fail(pilots[s]->player->error);return true;}
}
#endif
