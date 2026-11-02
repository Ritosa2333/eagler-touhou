#include "../multiplayer/FrameFence.hpp"
#include <eagler/gameplay/CooperativeRules.hpp>
#if TH15_MULTIPLAYER
#include "GameBattle.hpp"
#include "MultiplayerPilot.hpp"
#include "BattleSceneBindings.hpp"
#include "MultiplayerBombProtection.hpp"
#include "MultiplayerVisualPolicy.hpp"
#include <algorithm>
namespace th15 {
namespace {
u32 ghost_stage_seed(u32 seed,unsigned seat){const auto value=seed^(0x9e3779b9u*(seat+1));return value?value:1;}
}
MultiplayerPilot::MultiplayerPilot(GameBattle& w,unsigned s,int ch):world(w),seat(s),character(ch),owned_session(w.session),owned_life_piece_tier(w.score.life_piece_tier),session(s?owned_session:w.session),score(w.score){ghost_random=ghost_stage_seed(w.mp_options.seed,s);}
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
    player->life.base_bombs=world.mp_options.challenge?0:2;session.bombs=player->life.base_bombs;
    const float x=(float(seat)-float(world.mp_options.seat_count-1)*.5f)*48.f;
    player->motion.position={x,400,0};player->motion.position_fixed={i32(x*128),51200};player->motion.snap_options();
    return true;
}
bool MultiplayerPilot::rebuild_bomb(){if(!multiplayer::FrameFence::allow())return false;
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
    if(ghost){
        // TH11's bounded drift uses a private per-seat stream and ignores input.
        if(ghost_clock++%90==0){const auto next=[&](){ghost_random^=ghost_random<<13;ghost_random^=ghost_random>>17;ghost_random^=ghost_random<<5;return ghost_random;};ghost_dx=i32(next()%97)-48;ghost_dy=i32(next()%81)-40;}
        auto& motion=player->motion;auto& fixed=motion.position_fixed;
        fixed.x=std::clamp(fixed.x+ghost_dx,-23552,23552);fixed.y=std::clamp(fixed.y+ghost_dy,4096,55296);
        motion.position={float(fixed.x)/128,float(fixed.y)/128,0};
        return world.animations.tick_instance(player->visuals.root)>=0||world.fail(world.animations.error);
    }
    keys.update(world.mp_options.challenge?input.held&~2u:input.held);
    PlayerFrameContext c;c.held=keys.held;c.pressed=keys.pressed;refresh(c);
    // The target is transmitted as an absolute field coordinate. Never derive
    // movement from a sender's pre-delay simulation state.
    player->motion.touch={input.touch_mode,input.touch_x,input.touch_y};
    session.enemy_deaths=world.enemy_world.counters[0];session.enemy_chain=world.enemy_world.counters[2];
    const i32 before=session.deaths,before_enemy_deaths=world.enemy_world.counters[0];
    if(!player->update(c,world.input.rate))return world.fail(player->error);
    const i32 misses=wrapping_sub(session.deaths,before);
    if(misses>0){world.enemy_world.counters[0]=wrapping_add(before_enemy_deaths,misses);world.enemy_world.counters[2]=world.bomb_enemies.chain=0;
        if(world.progress){auto& progress=*world.progress;progress.chapter_deaths=wrapping_add(progress.chapter_deaths,misses);progress.stage_deaths[0]=wrapping_add(progress.stage_deaths[0],misses);if(progress.stage>=1&&progress.stage<8)progress.stage_deaths[progress.stage+1]=wrapping_add(progress.stage_deaths[progress.stage+1],misses);}}
    return true;
}
bool MultiplayerPilot::tick_bomb(){
    if(ghost)return true;
    if(context){context->hud_available=world.input.hud_available;context->hud_collect=world.input.hud_collect||world.dialogue_present();}
    const bool active=session.bomb_state==1;
    const i32 charges=character==3?static_cast<BombReisen&>(*bomb).charges:0;
    if(!bomb->update(world.input.rate))return world.fail(bomb->error);
    const bool broken=character==3&&static_cast<BombReisen&>(*bomb).charges<charges;
    // Also share the final frame/tail, when update just ended the bomb.
    protect_teammates(multiplayer_bomb_protection(character,active,broken));
    return true;
}
void MultiplayerPilot::protect_teammates(i32 frames){
    if(frames<=0)return;
    for(unsigned s=0;s<world.mp_options.seat_count;++s){auto& p=*world.pilots[s];
        if(s==seat||p.ghost||p.session.extra_lives<0)continue;
        auto& timer=p.player->life.invulnerability;
        const i32 next=multiplayer_protection_timer(timer.current,frames);
        if(next!=timer.current)timer.set(next);
    }
}
bool MultiplayerPilot::reset_stage(){
    // The native Replay entrance creates a fresh pilot. Give retained live
    // pilots the same private drift origin when the next stage is bound.
    ghost_random=ghost_stage_seed(world.mp_options.seed,seat);ghost_clock=0;ghost_dx=ghost_dy=0;
    session.bomb_state=0;spell_startup_bomb=false;keys={};input={};life_hold=power_taps=power_gap=0;life_latched=false;
    if(!rebuild_bomb())return false;
    if(ghost)return revive(false);
    if(world.mp_options.replay_legacy_stage_flow)return player->reset_for_stage()||world.fail(player->error);
    // Retained live players reset through RunSession at scene activation.
    return true;
}
bool MultiplayerPilot::allow_bomb(){return !ghost&&!world.mp_options.challenge&&bomb&&bomb->allowed();}
bool MultiplayerPilot::begin_bomb(){
    if(!allow_bomb()||!bomb->begin())return false;
    spell_startup_bomb=(character!=3||world.mp_options.replay_legacy_stage_flow)&&(world.spell.flags&1)&&world.spell.frame<60;
    if(!world.mp_options.replay_legacy_stage_flow&&character==3&&!world.mp_spell_startup_active())world.spell.flags&=~32u;
    world.enemy_world.counters[1]=world.bomb_enemies.uses;world.enemy_world.counters[2]=world.bomb_enemies.chain;
    protect_teammates(multiplayer_bomb_start_protection(character));
    return true;
}
bool MultiplayerPilot::cancel_lasers_near(const Vec3& p,float r,i32 k,bool h){return world.laser_scene->cancel_circle(p,r,k,h)>=0;}
bool MultiplayerPilot::cancel_bullets_near(const Vec3& p,float r,i32 k){return world.bullet_scene->cancel_circle(p,r,k,false);}
bool MultiplayerPilot::cancel_lasers(i32 k,bool h){return world.laser_scene->cancel_all(k,h);}
bool MultiplayerPilot::item(i32 k,const Vec3& p,float a,float s){return world.spawn_item(k,p,a,s);}
bool MultiplayerPilot::options_changed(){return player->configure_options();}
bool MultiplayerPilot::game_over(){
    if(ghost)return true;ghost=true;ghost_clock=0;ghost_dx=ghost_dy=0;keys={};input={};spell_startup_bomb=false;life_hold=power_taps=power_gap=0;life_latched=false;
    if(bomb&&!bomb->stop())return false;
    player->motion.option_count=0;for(auto& option:player->motion.options)option.active=0;
    for(auto& pair:player->visuals.option_handles)for(auto& handle:pair)if(!world.animations.retire(handle))return world.fail(world.animations.error);
    if(!player->visuals.focus_begin(false,1)||!player->visuals.barrier_remove())return world.fail(world.animations.error);
    for(auto& source:player->damage.sources)source.flags=0;
    // Shot animations belong to the shared ANM registry, which keeps ticking
    // after this pilot stops updating. Retire them before freeing their slots.
    for(auto& shot:player->shots.shots){
        if(!world.animations.retire(shot.animation))return world.fail(world.animations.error);
        shot.state=0;shot.damage_source=0;
    }
    auto& shooting=player->frame.shooting;shooting.shot.set(-1);shooting.continuous.set(-1);shooting.auxiliary=0;shooting.auxiliary_active=false;
    player->shot_context.shoot_frame=-1;player->shot_context.laser_power.fill(0);
    if(!stop_sound(20)||!stop_sound(30)||!stop_sound(55))return false;
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
    if(item.kind==4)return mp_reward_fragment();
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
    mp_options=o;mp_enabled=true;mp_item_targets.fill(-1);mp_gift_targets.fill(-1);mp_fragments=session.life_pieces;
    for(unsigned s=0;s<o.seat_count;++s)pilots[s]=std::make_unique<MultiplayerPilot>(*this,s,o.selections[s]);
    for(unsigned s=0;s<o.seat_count;++s)if(!pilots[s]->initialize(shots[o.selections[s]],banks[o.selections[s]]))return false;
    for(unsigned s=0;s<o.seat_count;++s){auto& c=*pilots[s]->collection;c.small_power_amount=i32(o.seat_count);c.shared_life_piece=[this](){return mp_reward_fragment();};}
    items->multiplayer_release=[this](u32 index){mp_gift_targets[index]=mp_item_targets[index]=-1;};
    enemy_world.multiplayer_target=[this](const Vec3& p){return mp_target(p);};bullet_scene->manager.multiplayer_target=enemy_world.multiplayer_target;laser_scene->multiplayer_target=enemy_world.multiplayer_target;return true;
}
int GameBattle::mp_item_owner(u32 index,ItemState& item){
    const bool power=item.kind==1||item.kind==3||item.kind==8;
    auto& directed=mp_gift_targets[index];
    if(directed>=0&&power&&pilots[unsigned(directed)]->ghost){directed=-1;item.state=1;item.velocity={};}
    auto nearest_power=[&](bool only_needy){return eagler::cooperative::NearestPowerRecipient(mp_options.seat_count,
        [&](unsigned s){const auto& p=*pilots[s];return p.alive()&&
            (!only_needy||p.session.power<p.score.max_power);},
        [&](unsigned s){return pilots[s]->session.power<pilots[s]->score.max_power;},
        [&](unsigned s){const auto& p=pilots[s]->player->motion.position;
            const double dx=double(p.x)-item.position.x,dy=double(p.y)-item.position.y;return dx*dx+dy*dy;});};
    auto overflow=[&](int seat){
        if(!power||pilots[unsigned(seat)]->session.power<pilots[unsigned(seat)]->score.max_power)return seat;
        const int target=nearest_power(true);
        if(target>=0){directed=-1;item.state=3;item.attraction_speed=8;return target;}return seat;
    };
    if(directed>=0)return overflow(directed);
    if((item.state==3||item.state==4)&&mp_item_targets[index]>=0){
        const int owner=mp_item_targets[index];if(pilots[unsigned(owner)]->alive())return overflow(owner);
        item.state=1;item.velocity={};mp_item_targets[index]=-1;
    }
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
    int target=nearest(false);
    if(target>=0&&power&&item.state==1&&item.delay<=0){
        const auto& p=*pilots[unsigned(target)];
        if(p.player->motion.position.y<float(p.character==1?148:128)||
           (p.session.bomb_state==1&&p.bomb->age.current<60)||input.hud_collect||dialogue_present()){
            target=nearest_power(false);
            if(target>=0){item.state=3;item.attraction_speed=8;}
            return target;
        }
    }
    return target>=0?overflow(target):target;
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
        i32 amount=hit.amount;if(p.player->life.state==0||p.player->life.state==2)amount/=5;
        out.amount=wrapping_add(out.amount,amount);out.direct|=hit.direct;if(hit.position_written){out.position=hit.position;out.position_written=true;}}
    return true;
}
unsigned GameBattle::mp_viable_count()const{unsigned count=0;for(unsigned s=0;s<mp_options.seat_count;++s)if(!pilots[s]->ghost&&pilots[s]->session.extra_lives>=0)++count;return count;}
bool GameBattle::mp_bomb_active(bool damaging_only)const{for(unsigned s=0;s<mp_options.seat_count;++s){const auto& p=*pilots[s];if(!p.ghost&&p.session.bomb_state==1&&(!damaging_only||p.character!=3))return true;}return false;}
bool GameBattle::mp_prepare_spell_startup(){
    bool active=false;for(unsigned s=0;s<mp_options.seat_count;++s){auto& p=*pilots[s];p.spell_startup_bomb=!p.ghost&&p.session.bomb_state==1&&p.character!=3;active|=p.spell_startup_bomb;}return active;
}
bool GameBattle::mp_spell_startup_active(){
    bool active=false;for(unsigned s=0;s<mp_options.seat_count;++s){auto& p=*pilots[s];if(p.ghost||p.session.bomb_state!=1||(p.character==3&&!mp_options.replay_legacy_stage_flow)){p.spell_startup_bomb=false;continue;}
        if(mp_options.replay_legacy_stage_flow&&p.character==3&&p.player->life.state==4&&spell.frame<60&&(spell.flags&32))p.spell_startup_bomb=true;
        active|=p.spell_startup_bomb;}return active;
}
i32 GameBattle::mp_boss_damage(EnemyState& enemy,i32 amount){
    if(amount<=0||!(enemy.flags&0x40800000u))return amount;
    const unsigned count=mp_viable_count(),numerator=count>=3?8:count==2?9:12;
    const u64 scaled=u64(unsigned(amount))*numerator+enemy.multiplayer_damage_remainder;
    enemy.multiplayer_damage_remainder=unsigned(scaled%12);return i32(scaled/12);
}
void GameBattle::mp_prepare_presentation(AnmRenderer& r){
    // Tag the current native VMs on every draw, including registered focus/
    // option/shot trees. The renderer scales draw copies, never simulation VMs.
    r.multiplayer_player_opacity.clear();
    r.multiplayer_hidden_hitboxes.clear();
    for(auto it=r.multiplayer_hitbox_owners.begin();it!=r.multiplayer_hitbox_owners.end();)
        if(!animations.registry.find(it->first))it=r.multiplayer_hitbox_owners.erase(it);else ++it;
    const auto local_position=pilots[mp_options.local_seat]->player->motion.position;
    for(unsigned s=0;s<mp_options.seat_count;++s){auto& pilot=*pilots[s];const auto position=pilot.player->motion.position;
        const double dx=double(local_position.x)-position.x,dy=double(local_position.y)-position.y;
        const u8 opacity=multiplayer_player_opacity(mp_options.spectator||s==mp_options.local_seat,pilot.ghost,mp_options.spectator?1e20:dx*dx+dy*dy);
        auto& visuals=pilot.player->visuals;
        // Keep ownership until the generation-tagged VM is actually retired,
        // including fade-out and subsequent changes of the selected viewpoint.
        const auto remember=[&](auto&& self,AnmVm* vm)->void{if(!vm)return;if(auto id=animations.registry.handle(*vm))r.multiplayer_hitbox_owners[id]=s;for(auto* child:animations.registry.children(*vm))self(self,child);};remember(remember,animations.registry.find(visuals.focus_handle));
        remember(remember,animations.registry.find(visuals.barrier_handle));
        auto& grazes=pilot.graze_animations;grazes.erase(std::remove_if(grazes.begin(),grazes.end(),[&](u32 h){return !animations.registry.find(h);}),grazes.end());
        for(auto h:grazes)remember(remember,animations.registry.find(h));
        // Retail Reisen shield scripts 14-16 are the persistent hitbox markers;
        // scripts 11-13 are shield rings and remain visible to teammates.
        if(pilot.character==3&&pilot.bomb)if(auto* shield=animations.registry.find(static_cast<BombReisen&>(*pilot.bomb).barrier))
            for(auto* child:animations.registry.children(*shield))if(child->source_script>=14&&child->source_script<=16)remember(remember,child);
        if(opacity==255)continue;
        const auto tag=[&](auto&& self,AnmVm* vm)->void{if(!vm)return;r.multiplayer_player_opacity[vm]=opacity;for(auto* child:animations.registry.children(*vm))self(self,child);};
        tag(tag,&visuals.root);tag(tag,animations.registry.find(visuals.focus_handle));tag(tag,animations.registry.find(visuals.barrier_handle));
        for(const auto& pair:visuals.option_handles)for(auto handle:pair)tag(tag,animations.registry.find(handle));
        for(const auto& shot:pilot.player->shots.shots)if(shot.state)tag(tag,animations.registry.find(shot.animation));
    }
    for(const auto& entry:r.multiplayer_hitbox_owners)if(!mp_options.spectator&&entry.second!=mp_options.local_seat)
        if(auto* vm=animations.registry.find(entry.first))r.multiplayer_hidden_hitboxes.push_back(vm);
}
bool GameBattle::mp_draw(AnmRenderer& r){
    mp_prepare_presentation(r);
    for(unsigned s=0;s<mp_options.seat_count;++s){auto& pilot=*pilots[s];
        if(!pilot.ghost){if(!pilot.player->draw(r))return fail(pilot.player->error);continue;}
        // A presentation copy keeps the original character sprite visible,
        // independently of native death hiding and invulnerability flashing.
        auto body=pilot.player->visuals.root;body.visual.flags=(body.visual.flags|3u)&~0x60000u;
        body.visual.render_flags=(body.visual.render_flags&~0x80060u)|0x40000u;
        const unsigned base_alpha=mp_options.spectator||s==mp_options.local_seat?112u:88u;unsigned alpha=base_alpha;
        // TH11 rescue feedback: the recipient becomes opaque as the donor's
        // confirmed 90-tick hold progresses. No authoritative ANM mutation.
        for(unsigned giver=0;giver<mp_options.seat_count;++giver){const auto& donor=*pilots[giver];
            if(donor.life_hold&&mp_life_receiver(giver)==int(s))alpha=std::max(alpha,base_alpha+(255-base_alpha)*eagler::cooperative::TapCount(donor.life_hold)/5);}
        body.visual.color=(body.visual.color&0xffffffu)|(alpha<<24);
        body.visual.translation=pilot.player->motion.position;body.presentation_motion=true;
        if(r.draw(body)==-2)return fail(r.error);
    }return true;
}
}
#endif
