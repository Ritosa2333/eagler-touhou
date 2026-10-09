#ifdef TH11_MULTIPLAYER
#include "GameBattle.hpp"
#include "GraphicsMath.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace th11 {
namespace {
double distance2(Vec3 a,Vec3 b){const double x=double(a.x)-b.x,y=double(a.y)-b.y;return x*x+y*y;}
void position(PlayerFrame& player,Vec3 p){auto& s=player.motion.state;s.x=truncate_int(double(p.x)*128);s.y=truncate_int(double(p.y)*128);s.position={float(double(s.x)/128),float(double(s.y)/128),0};for(auto& o:player.motion.options)o.snap=1;}
u32 random_next(u32& state){state^=state<<13;state^=state>>17;state^=state<<5;return state;}
void erase_animation(AnmManager& a,u32 id){if(auto* v=a.find(id)){v->flags|=0x4000000;if(!v->child.previous)for(auto* n=v->child.next;n;n=n->next)n->value->flags|=0x4000000;}}
struct EffectOwner {GameBattle& b;unsigned old,old_animation;EffectOwner(GameBattle& w,unsigned seat):b(w),old(w.mp_effect_seat),old_animation(w.animations.multiplayer_owner){b.mp_effect_seat=seat;b.animations.multiplayer_owner=seat+1;}~EffectOwner(){b.mp_effect_seat=old;b.animations.multiplayer_owner=old_animation;}};
}
MultiplayerPilot::MultiplayerPilot(GameBattle& w,unsigned index):world(w),seat(index),owned_economy(w.economy.score_units),economy(index?owned_economy:w.economy),communication(index?owned_communication:w.communication),rewards(economy,*this){owned_economy=w.economy;ghost_random=w.mp_options.seed^(0x9e3779b9u*(index+1));if(!ghost_random)ghost_random=1;}
bool MultiplayerPilot::alive()const noexcept{return player&&!ghost&&player->state.life_state!=2&&player->state.life_state!=4;}
bool MultiplayerPilot::initialize(int selection){
    EffectOwner effect(world,seat);
    input={};input.challenge=world.mp_options.challenge;input.movement.character=selection/3;input.movement.subtype=selection%3;input.movement.enemy_manager=true;
    communication.reset(&world.animations.rate);
    if(seat){owned_player=std::make_unique<PlayerFrame>(world.resources.core.shots[selection],world.resources.core.players[selection/3],world.resources.core.bullet,world.animations,economy,*this,7,6);player=owned_player.get();player->input=input;
        world.animations.multiplayer_tags[&player->motion.body]=seat+1;
        if(!player->initialize()||!player->start_stage())return false;
        if(world.mp_options.stage>1&&world.mp_options.stage<=6)economy.power=economy.max_power;
        owned_bomb=std::make_unique<BombController>(world.animations,world.resources.core.players[selection/3],*player,*this,7,&world.resources.core.text);bomb=owned_bomb.get();bomb->selection=selection;
    }else{player=world.player.get();bomb=world.bomb.get();}
    player->input.challenge=world.mp_options.challenge;
    world.animations.multiplayer_tags[&player->motion.body]=seat+1;
    constexpr i32 minimum[]={2500000,5000000,10000000,20000000,20000000};player->state.minimum_point_value=minimum[economy.difficulty];
    const float spacing=world.mp_options.seat_count==2?48.f:56.f;
    position(*player,{(float(seat)-(world.mp_options.seat_count-1)*.5f)*spacing,400,0});
    return power_changed();
}
bool MultiplayerPilot::tick(){
    if(!player)return false;
    force_attract=false;
    if(ghost){
        if(ghost_clock++%90==0){ghost_dx=i32(random_next(ghost_random)%97)-48;ghost_dy=i32(random_next(ghost_random)%81)-40;}
        auto& s=player->motion.state;s.x=std::clamp(s.x+ghost_dx,-23552,23552);s.y=std::clamp(s.y+ghost_dy,4096,55296);s.position={float(s.x)/128,float(s.y)/128,0};
        return player->motion.body.update(world.animations)>=0;
    }
    player->input=input;player->spell={world.spell_flags,world.spell_elapsed,world.spell_bonus};
    const i32 before=world.mp_rank;economy.rank=before;
    EffectOwner effect(world,seat);
    if(!player->update())return false;
    world.spell_flags=player->spell.flags;world.spell_bonus=player->spell.bonus;
    const i32 delta=economy.rank-before;world.mp_rank=std::clamp(before+(delta<0?delta/i32(world.mp_options.seat_count):delta),-1024,1024);economy.rank=world.mp_rank;
    return true;
}
bool MultiplayerPilot::tick_bomb(){if(ghost||!bomb)return true;EffectOwner effect(world,seat);player->spell={world.spell_flags,world.spell_elapsed,world.spell_bonus};if(!bomb->update())return false;world.spell_flags=player->spell.flags;world.spell_bonus=player->spell.bonus;return true;}
bool MultiplayerPilot::revive(bool rescued){
    if(!player)return false;
    EffectOwner effect(world,seat);
    const auto p=player->motion.state.position;ghost=false;ghost_clock=0;life_hold=power_taps=0;life_latched=false;
    economy.lives=std::max(economy.lives,0);economy.power=economy.max_power/2;economy.communication=0;communication.reset(&world.animations.rate);
    player->state.life_state=1;player->state.state_timer.set(60,&world.animations.rate);player->state.transition_timer.set(60,&world.animations.rate);player->state.invincibility.set(rescued?280:120,&world.animations.rate);
    player->motion.state.flags=0;player->motion.state.warp=player->motion.state.warp_timer=0;player->motion.clear_focus();player->motion.clear_options();
    if(!player->motion.reset_body())return false;position(*player,p);
    if(bomb)bomb->multiplayer_clear();
    input.movement.held=input.movement.pressed=0;player->input=input;player->input.special_active=false;
    return power_changed();
}
bool MultiplayerPilot::sound(i32 id,float x){return world.sound(id,x,true);}
bool MultiplayerPilot::sound(i32 id,float x,bool positional){return world.sound(id,x,positional);}
bool MultiplayerPilot::special_damage(const Vec3& p,const Vec2& size,i32& out){return bomb&&bomb->damage(p,size,out);}
bool MultiplayerPilot::player_sound(i32 id){return world.sound(id,0,false);}
bool MultiplayerPilot::attract_items(){force_attract=true;return true;}
bool MultiplayerPilot::cancel_bullets(const Vec3* p,float r,bool skip){return world.cancel_bullets(p,r,skip);}
bool MultiplayerPilot::cancel_lasers(const Vec3* p,float r,bool reward,bool skip){return world.cancel_lasers(p,r,reward,skip);}
bool MultiplayerPilot::start_bomb(){if(world.mp_options.challenge||!bomb||ghost)return false;EffectOwner effect(world,seat);return bomb->start()!=-2;}
bool MultiplayerPilot::spawn_item(i32 type,Vec3 p,u32 color,float angle,float speed){return world.items.spawn(type,p,color,angle,speed)==0;}
bool MultiplayerPilot::display_lives(i32 lives,i32 fragments){if(!seat)return world.display_lives(lives,fragments);return true;}
bool MultiplayerPilot::record_death(){if(world.mp_options.challenge&&challenge_misses!=0xffffffffu)++challenge_misses;world.events.push_back({BattleEventKind::Death,i32(seat)});return true;}
bool MultiplayerPilot::enemy_death(){return world.enemy_death();}
bool MultiplayerPilot::game_over(bool){
    EffectOwner effect(world,seat);
    if(ghost)return true;ghost=true;ghost_clock=0;player->motion.clear_options();player->motion.clear_focus();
    for(u32 i=0;i<256;++i){auto& shot=player->shots.at(i);erase_animation(world.animations,shot.animation);erase_animation(world.animations,shot.extra_animation);shot.state=0;}
    for(auto& a:player->shots.damage_areas.areas)a.flags=0;
    if(bomb)bomb->multiplayer_clear();
    return player->motion.reset_body();
}
bool MultiplayerPilot::bomb_sound(i32 id,float x,bool positional){return sound(id,x,positional);}
bool MultiplayerPilot::bomb_stop_sound(i32 id){world.events.push_back({BattleEventKind::StopSound,id});return true;}
bool MultiplayerPilot::bomb_cancel(Vec3 p,float radius,u32 rewards,bool convert){EffectOwner effect(world,seat);const BulletCancelContext c{world.spell_flags,world.spell_id};if(convert?!world.bullets.convert_circle(p,radius,rewards&1,c):!world.bullets.cancel_circle(p,radius,rewards&1,true,c))return false;return world.lasers.cancel_circle(p,radius,rewards,true)>=0;}
i32& MultiplayerPilot::bomb_count(){return world.enemy_environment.shared_integers[1];}
bool MultiplayerPilot::bomb_background_color(u32 color){if(!world.stage)return false;world.stage->multiplayer_tint_owner=seat+1;world.stage->multiplayer_tint_source=color;return world.bomb_background_color(color);}
bool MultiplayerPilot::bomb_refund_power(){bool changed=false;return rewards.add_power(10,changed)&&power_changed();}
bool MultiplayerPilot::bomb_cancel_beam(Vec3 p,bool reward){EffectOwner effect(world,seat);const BulletCancelContext c{world.spell_flags,world.spell_id};if(!world.bullets.cancel_circle(p,48,reward,true,c)||!world.bullets.cancel_beam(p,32,reward,c)||world.lasers.cancel_circle(p,48,reward,true)<0)return false;p.y=float(double(p.y)-224);return world.lasers.cancel_rectangle(p,{32,448,0},reward)>=0;}
bool MultiplayerPilot::popup(Vec3 p,i32 value,u32 color){return world.popup(p,value,color);}
bool MultiplayerPilot::notify(i32 value){return world.notify(value);}
bool MultiplayerPilot::power_changed(){EffectOwner effect(world,seat);return player&&player->motion.rebuild_options(world.resources.core.shots[world.mp_options.selections[seat]].header,economy,world.mp_options.selections[seat]);}
bool MultiplayerPilot::lives_changed(i32 lives,i32 fragments){return display_lives(lives,fragments);}

void GameBattle::enable_multiplayer(const MultiplayerOptions& options){mp_options=options;mp_enabled=true;hud.multiplayer=true;mp_rank=economy.rank;for(unsigned i=0;i<options.seat_count;++i){pilots[i]=std::make_unique<MultiplayerPilot>(*this,i);completion.multiplayer_economies[i]=&pilots[i]->economy;}completion.multiplayer_count=options.seat_count;}
bool GameBattle::mp_initialize_players(){for(unsigned i=0;i<mp_options.seat_count;++i)if(!pilots[i]->initialize(mp_options.selections[i])){error="MP player initialization";return false;}return true;}
void GameBattle::mp_set_input(const std::array<MultiplayerInput,3>& frame_input){
    for(unsigned i=0;i<mp_options.seat_count;++i){auto& p=*pilots[i];const auto& in=frame_input[i];p.keys.update(mp_options.challenge?in.held&~2u:in.held,false);p.input.movement.held=p.ghost?0:p.keys.held;p.input.movement.pressed=p.ghost?0:p.keys.pressed;p.input.movement.touch_mode=p.ghost?0:in.touch_mode;p.input.movement.touch_x=in.touch_x;p.input.movement.touch_y=in.touch_y;}
    player_input=pilots[0]->input;
}
void GameBattle::mp_sync(){
    bool any=false,nonshield=false,ending=false;
    for(unsigned i=0;i<mp_options.seat_count;++i){auto& p=*pilots[i];if(!p.player)continue;p.enemies=enemies.first;p.economy.rank=mp_rank;
        p.input.movement.enemies=enemies.count!=0;p.input.movement.bomb=dialogue&&dialogue->active;p.input.special_available=p.bomb&&!p.ghost&&!mp_options.challenge;p.input.special_active=p.bomb&&!p.ghost&&p.bomb->state.active;p.input.shooting_blocked=(completion.state.hud_flags&0x10)!=0||p.ghost;
        p.player->input=p.input;
        if(p.input.special_active){any=true;if(!(p.player->motion.state.flags&4))nonshield=true;}
        if(!p.ghost&&!(p.player->motion.state.flags&4)&&(p.player->state.invincibility.current>0||(p.player->motion.state.flags&2)))ending=true;
    }
    player_input=pilots[0]->input;enemy_environment.rank=mp_rank;enemy_environment.rate=animations.rate;LaserWorld::rate=animations.rate;
    special_active=any;special_ending=ending;player_flags=any&&!nonshield?4:0;player_state=1;spells.bomb_active=nonshield;spells.replay=true;cancellation={spell_flags,spell_id};
    const auto origin=enemy_commands.bosses[0]?enemy_commands.bosses[0]->current.position:Vec3{0,0,0};multiplayer_enemy_target(origin);
    ShotWorld::enemies=enemies.first;
}
bool GameBattle::mp_update_players(){for(unsigned i=0;i<mp_options.seat_count;++i)if(!pilots[i]->tick()){error="MP player "+std::to_string(i)+" update";last_error=-601;return false;}for(unsigned i=0;i<mp_options.seat_count;++i)if(!pilots[i]->tick_bomb()){error="MP bomb "+std::to_string(i)+" update";last_error=-608;return false;}return true;}
unsigned GameBattle::mp_operable_count()const noexcept{unsigned count=0;for(unsigned i=0;i<mp_options.seat_count;++i)if(pilots[i]&&!pilots[i]->ghost)++count;return count;}
int GameBattle::mp_target(Vec3 origin,bool power)const noexcept{int selected=-1;double nearest=0;for(unsigned i=0;i<mp_options.seat_count;++i){const auto& p=*pilots[i];if(!p.alive()||(power&&p.economy.power>=p.economy.max_power))continue;const double d=distance2(origin,p.player->motion.state.position);if(selected<0||d<nearest){selected=i;nearest=d;}}return selected;}
Vec3 GameBattle::multiplayer_target(Vec3 origin)const{const auto target=mp_target(origin);return target<0?Vec3{0,400,0}:pilots[unsigned(target)]->player->motion.state.position;}
void GameBattle::multiplayer_enemy_target(Vec3 origin){const Vec3 p=multiplayer_target(origin);enemy_environment.player_position=p;bullets.player=p;LaserWorld::player=p;}
bool GameBattle::mp_all_power_full()const noexcept{bool found=false;for(unsigned i=0;i<mp_options.seat_count;++i){const auto& p=*pilots[i];if(p.ghost)continue;found=true;if(p.economy.power<p.economy.max_power)return false;}return found;}
bool GameBattle::multiplayer_item_player(ItemState& item,ItemPlayer& out){
    const auto index=&item-&items.at(0);auto& directed=items.multiplayer_targets[index];
    const bool power=item.type==1||item.type==4||item.type==6||item.type==10||item.type==11;
    // A Power gift whose recipient becomes a ghost returns to the ordinary
    // field pool. The successful emission already consumed the donor's Power.
    if(directed>=0&&power&&pilots[unsigned(directed)]->ghost){directed=-1;item.state=1;item.velocity={};}
    items.multiplayer_current_target=-1;
    auto snapshot=[&](unsigned seat,ItemPlayer& candidate){const auto& p=*pilots[seat];p.player->copy_item_state(candidate);candidate.force_attract=p.force_attract||(dialogue&&dialogue->active);};
    if(directed>=0){
        const auto& p=*pilots[unsigned(directed)];snapshot(unsigned(directed),out);out.force_attract=true;
        // Life gifts retain their existing reserve-life delivery semantics.
        if(p.ghost){out.state=1;out.position=p.player->motion.state.position;out.pickup={out.position.x-20,out.position.y-20,out.position.x+20,out.position.y+20};}
        items.multiplayer_current_target=directed;return true;
    }
    auto eligible=[&](const ItemPlayer& p){
        if(p.state==2||p.state==4||item.state==5)return false;
        if(item.state==3||item.state==4)return true;
        if(item.state==1&&(p.force_attract||p.position.y<128||(item.lifetime.current>=40&&p.communication>=10000)))return true;
        if(item.state==2&&float(double(item.velocity.y)+double(animations.rate)*double(.03f))>=0)return true;
        // Native pickup/near-attraction follows this frame's falling motion.
        // Qualifying on the old point can both miss a new pickup and let a
        // player whose item just moved away block another valid collector.
        Vec3 contact=item.position;
        if(item.state==1||item.state==2){
            auto move=[&](float coordinate,float velocity){return float(double(coordinate)+float(double(velocity)*animations.rate));};
            contact={move(contact.x,item.velocity.x),move(contact.y,item.velocity.y),move(contact.z,item.velocity.z)};
            if(contact.y>472)return false;
        }
        return p.pickup.contains(contact)||(p.focused?p.focused_attract:p.unfocused_attract).contains(contact);
    };
    auto nearest=[&](bool needs_power){int selected=-1;double distance=0;ItemPlayer candidate{};
        for(unsigned seat=0;seat<mp_options.seat_count;++seat){const auto& p=*pilots[seat];if(!p.alive()||(needs_power&&p.economy.power>=p.economy.max_power))continue;snapshot(seat,candidate);if(!eligible(candidate))continue;const auto next=distance2(item.position,candidate.position);if(selected<0||next<distance){selected=i32(seat);distance=next;out=candidate;}}
        return selected;
    };
    int selected=nearest(power);if(selected<0&&power)selected=nearest(false);
    items.multiplayer_current_target=selected;return selected>=0;
}
bool GameBattle::mp_collect(ItemState& item,bool& convert){
    const int seat=items.multiplayer_current_target;if(seat<0||unsigned(seat)>=mp_options.seat_count)return false;auto& p=*pilots[unsigned(seat)];
    if(item.type==5){if(++mp_fragments>=5){mp_fragments=0;for(unsigned i=0;i<mp_options.seat_count;++i)if(!pilots[i]->rewards.add_life())return false;}for(unsigned i=0;i<mp_options.seat_count;++i)pilots[i]->economy.life_fragments=i32(mp_fragments);rank_delta(256);return true;}
    const i32 before=p.economy.rank;EffectOwner owner(*this,unsigned(seat));bool native_convert=false;if(!p.rewards.collect(item,native_convert))return false;mp_rank=std::clamp(mp_rank+(p.economy.rank-before),-1024,1024);convert=convert||mp_all_power_full();return true;
}
bool GameBattle::mp_stage_reset(){
    mp_wipe_frames=0;mp_damage_remainders.clear();
    for(unsigned i=0;i<mp_options.seat_count;++i){auto& p=*pilots[i];EffectOwner effect(*this,i);if(p.ghost&&!p.revive(false))return false;if(!p.player->start_stage())return false;p.communication.reset(&animations.rate);p.economy.communication=0;p.life_hold=p.power_taps=0;p.life_latched=false;p.keys={};}
    return true;
}
int GameBattle::mp_life_receiver(unsigned giver)const noexcept{
    if(giver>=mp_options.seat_count||!pilots[giver])return -1;
    const auto& p=*pilots[giver];int receiver=-1;
    if(!p.alive()||(p.keys.held&9)!=8||p.life_latched||p.economy.lives<=0||(dialogue&&dialogue->active)||!stage_active)return -1;
    for(unsigned j=0;j<mp_options.seat_count;++j){if(giver==j)continue;const auto& q=*pilots[j];if((!q.alive()&&!q.ghost)||(!q.ghost&&q.economy.lives>=9)||distance2(p.player->motion.state.position,q.player->motion.state.position)>400)continue;
        if(receiver<0||q.ghost>pilots[unsigned(receiver)]->ghost||(q.ghost==pilots[unsigned(receiver)]->ghost&&q.economy.lives<pilots[unsigned(receiver)]->economy.lives))receiver=j;
    }
    return receiver;
}
bool GameBattle::mp_update_rules(){
    ++mp_tick;
    for(unsigned i=0;i<mp_options.seat_count;++i){auto& p=*pilots[i];if(!p.player)continue;const u32 held=p.keys.held;
        if(!(held&8)){p.life_hold=0;p.life_latched=false;}
        if(p.power_gap<25)++p.power_gap;if(p.power_gap>24)p.power_taps=0;
        if(!p.alive()||(dialogue&&dialogue->active)||!stage_active)continue;
        int receiver=-1;
        if((held&9)==8&&!p.life_latched&&p.economy.lives>0){
            receiver=mp_life_receiver(i);
            if(receiver>=0){if(++p.life_hold>=90){auto& q=*pilots[unsigned(receiver)];bool transferred=false;if(q.ghost){--p.economy.lives;transferred=q.revive(true);if(transferred&&!sound(0x2c,0,false))return false;}else if(items.spawn_transfer(7,p.player->motion.state.position,unsigned(receiver))){--p.economy.lives;transferred=true;}if(transferred){p.life_latched=true;p.life_hold=0;}}}else p.life_hold=0;
        }else if(!p.life_latched)p.life_hold=0;
        if(p.keys.pressed&1){if(p.power_gap>24)p.power_taps=0;p.power_gap=0;++p.power_taps;
            if(p.power_taps>=5){p.power_taps=0;receiver=-1;
                if(p.economy.power>=p.economy.power_step)for(unsigned j=0;j<mp_options.seat_count;++j){if(i==j)continue;const auto& q=*pilots[j];if(!q.alive()||q.economy.power>=q.economy.max_power||distance2(p.player->motion.state.position,q.player->motion.state.position)>400)continue;
                    if(receiver<0||i64(q.economy.power)*pilots[unsigned(receiver)]->economy.power_step<i64(pilots[unsigned(receiver)]->economy.power)*q.economy.power_step)receiver=j;
                }
                if(receiver>=0&&items.spawn_transfer(4,p.player->motion.state.position,unsigned(receiver))){p.economy.power-=p.economy.power_step;if(!p.power_changed())return false;}
            }
        }
    }
    if(mp_operable_count()==0){if(++mp_wipe_frames>=180){game_over_requested=true;mp_failed=true;}}
    else mp_wipe_frames=0;
    return true;
}
bool GameBattle::mp_hit(unsigned seat,PlayerCollisionResult c){if(!c.trigger_hit)return true;EffectOwner effect(*this,seat);auto& p=*pilots[seat];p.player->spell={spell_flags,spell_elapsed,spell_bonus};if(!p.player->hit())return false;spell_flags=p.player->spell.flags;spell_bonus=p.player->spell.bonus;return true;}
bool GameBattle::mp_reward_graze(unsigned seat,bool laser){auto& p=*pilots[seat];if(p.economy.graze<99999999)++p.economy.graze;constexpr i32 bullet_reward[]={800,500,500,500,500},laser_reward[]={500,400,300,200,200};p.communication.reward(p.economy,(laser?laser_reward:bullet_reward)[economy.difficulty],&animations.rate);return true;}
bool GameBattle::multiplayer_bullet_collision(const BulletState& b,u8& grazed,i32& result){
    result=0;for(unsigned i=0;i<mp_options.seat_count;++i){auto& p=*pilots[i];if(!p.alive())continue;const auto shape=p.player->collision();const Vec2 pos{b.position.x,b.position.y};const auto c=b.flags&0x10?shape.circle(pos,b.hitbox.x):shape.rectangle(pos,b.hitbox);if(!mp_hit(i,c))return false;
        if(c.kind==CollisionKind::Hit)result=1;
        else if(c.kind==CollisionKind::Graze&&!(grazed&(1u<<i))){grazed|=u8(1u<<i);if(!mp_reward_graze(i,false)||!graze_effect(b.position)||!sound(28,b.position.x,true))return false;}
    }return true;
}
bool GameBattle::mp_damage_position(Vec3 pos,Vec2 size,i32& out){
    out=0;for(unsigned i=0;i<mp_options.seat_count;++i){auto& p=*pilots[i];if(p.ghost)continue;const auto& timer=p.player->state.state_timer;i32 amount=0;EffectOwner owner(*this,i);if(!p.player->shots.damage(pos,size,timer.previous!=timer.current,p.economy,amount))return false;
        if(p.player->state.life_state==0||p.player->state.life_state==2)amount/=5;
        if((spell_flags&1)&&spell_id>=0x9e&&spell_id<=0xa1&&(p.input.special_active||p.player->state.invincibility.current!=0||(p.player->motion.state.flags&2))&&!(p.player->motion.state.flags&4))amount/=5;
        out=wrapping_add(out,amount);
    }return true;
}
void GameBattle::multiplayer_shot_target(EnemyState& e){
    if(e.flags&0xc00021)return;
    for(unsigned i=0;i<mp_options.seat_count;++i){auto& p=*pilots[i];if(p.ghost)continue;auto& shots=p.player->shots;const float x=p.player->motion.state.position.x;
        // Preserve the native target traversal separately for each shot owner.
        if(!shots.locked_target||std::abs(float(double(shots.locked_target->state.current.position.x)-x))<std::abs(float(double(e.current.position.x)-x))){if(!shots.target_locked)shots.locked_target=e.script_owner;shots.target_locked=1;}
    }
}
i32 GameBattle::multiplayer_boss_damage(EnemyState& e,i32 amount){
    if(amount<=0||(!(e.flags&0x4000000)&&!(e.flags&0x80000)))return amount;
    const unsigned count=mp_operable_count();const unsigned numerator=count<2?12:count==2?9:8;
    // A fixed denominator retains an exact fractional carry as seats become
    // ghosts or are rescued. Changing denominators would reinterpret the carry.
    auto& rem=mp_damage_remainders[e.script_owner];const u64 scaled=u64(unsigned(amount))*numerator+rem;rem=unsigned(scaled%12);return i32(scaled/12);
}
bool GameBattle::mp_attract_players(const EnemyState& e){for(unsigned i=0;i<mp_options.seat_count;++i){auto& p=*pilots[i];if(!p.alive())continue;const Vec3 old=p.player->motion.state.position;auto d=GraphicsMath::normalize({float(double(e.current.position.x)-old.x),float(double(e.current.position.y)-old.y),0});position(*p.player,{float(double(old.x)+double(d.x)*e.floats[3]),float(double(old.y)+double(d.y)*e.floats[3]),0});}return true;}
MultiplayerSeatView GameBattle::mp_seat(unsigned seat)const noexcept{MultiplayerSeatView v;v.seat=seat;if(!mp_enabled||seat>=mp_options.seat_count||!pilots[seat]||!pilots[seat]->player)return v;const auto& p=*pilots[seat];const auto& e=p.economy;v.active=true;v.ghost=p.ghost;v.life_state=p.player->state.life_state;v.power_taps=p.power_taps;v.misses=p.challenge_misses;v.selection=mp_options.selections[seat];v.lives=e.lives;v.life_fragments=mp_fragments;v.power=e.power;v.max_power=e.max_power;v.power_step=e.power_step;v.score=i64(e.score_units)*10;v.graze=e.graze;v.communication=e.communication;v.x=p.player->motion.state.position.x;v.y=p.player->motion.state.position.y;v.fast_speed=float(p.player->motion.state.normal_speed)/128;v.slow_speed=float(p.player->motion.state.focus_speed)/128;return v;}
u32 GameBattle::mp_hash()const noexcept{
    u32 hash=2166136261u;auto word=[&](u32 v){for(unsigned i=0;i<4;++i){hash^=(v>>(i*8))&255;hash*=16777619u;}};auto flt=[&](float v){u32 bits;std::memcpy(&bits,&v,4);word(bits);};
    auto timer=[&](const Timer& t){word(t.previous);word(t.current);flt(t.fractional);};
    auto pc=[&](const EclOwner& owner,const EclInstruction* instruction){if(!instruction){word(0);return;}const auto address=reinterpret_cast<uintptr_t>(instruction);unsigned index=0;for(const auto& file:owner.program->files){++index;const auto begin=reinterpret_cast<uintptr_t>(file->bytes.data());if(address>=begin&&address<begin+file->bytes.size()){word(index);word(u32(address-begin));return;}}word(~0u);};
    word(mp_options.challenge);for(unsigned i=0;i<mp_options.seat_count;++i)word(pilots[i]->challenge_misses);
    word(mp_tick);word(mp_fragments);word(mp_wipe_frames);word(mp_rank);word(economy.score_units);word(frame);word(resources.stage_number);word(animations.script_rng.seed);word(animations.script_rng.calls);word(spell_flags);word(spell_id);word(spell_bonus);word(spell_elapsed);
    for(auto value:enemy_environment.shared_integers)word(value);flt(animations.rate);word(transitioning);word(transition_started);word(stage_active);timer(transition_timer);
    if(stage){const auto& s=stage->state;word(s.instruction_offset);timer(s.script_timer);timer(s.fade_timer);word(s.frame_count);word(s.draw_flags);word(s.effects_enabled);}
    for(unsigned i=0;i<mp_options.seat_count;++i){const auto& p=*pilots[i];const auto& s=p.player->motion.state;const auto& e=p.economy;word(p.ghost);word(s.x);word(s.y);word(s.weapon_mode);word(s.flags);word(p.player->state.life_state);word(p.player->state.state_timer.current);word(p.player->state.invincibility.current);word(e.lives);word(e.power);word(e.communication);word(e.graze);word(e.point_value);word(p.keys.held);word(p.life_hold);word(p.life_latched);word(p.power_taps);word(p.power_gap);word(p.ghost_random);word(p.ghost_dx);word(p.ghost_dy);word(p.bomb?p.bomb->state.active:0);word(p.bomb?p.bomb->state.elapsed.current:0);
        for(u32 j=0;j<256;++j){const auto& shot=p.player->shots.at(j);if(!shot.state)continue;word(j);word(shot.state);flt(shot.movement.position.x);flt(shot.movement.position.y);word(shot.timer.current);}
    }
    for(auto* n=enemies.first;n;n=n->next){const auto& e=n->value->state;word(e.health);word(e.max_health);word(e.flags);timer(e.lifetime);flt(e.current.position.x);flt(e.current.position.y);flt(e.current.velocity.x);flt(e.current.velocity.y);for(auto v:e.integers)word(v);for(auto v:e.floats)flt(v);for(auto v:e.extra_floats)flt(v);auto r=mp_damage_remainders.find(n->value);word(r==mp_damage_remainders.end()?0:r->second);
        const auto& script=n->value->script;for(auto* thread=&script.threads;thread;thread=thread->next){const auto& c=*thread->value;word(c.thread_id);flt(c.time);pc(script,c.instruction);word(c.state);word(c.flags);word(c.stack.top);word(c.stack.frame_base);}word(~0u);
    }
    for(u32 i=0;i<BulletManager::capacity;++i){const auto& b=const_cast<BulletManager&>(bullets).at(i);if(!b.state)continue;word(i);word(b.state);word(b.flags);flt(b.position.x);flt(b.position.y);flt(b.speed);flt(b.angle);word(b.lifetime.current);word(bullets.multiplayer_grazed[i]);}
    for(u32 i=0;i<ItemManager::capacity;++i){const auto& item=const_cast<ItemManager&>(items).at(i);if(!item.state)continue;word(i);word(item.state);word(item.type);flt(item.position.x);flt(item.position.y);word(item.lifetime.current);word(items.multiplayer_targets[i]);}
    for(auto* l=lasers.first();l;l=l->next){word(l->id);word(l->state);word(l->type);flt(l->position.x);flt(l->position.y);flt(l->velocity.x);flt(l->velocity.y);flt(l->angle);flt(l->length);flt(l->width);flt(l->speed);flt(l->offset);timer(l->lifetime);timer(l->graze_timer);timer(l->offscreen_timer);word(l->active_transforms);word(l->transform_index);
        for(const auto& motion:l->motion){timer(motion.timer);flt(motion.a);flt(motion.b);flt(motion.vector.x);flt(motion.vector.y);word(motion.duration);word(motion.limit);word(motion.count);}
        if(l->type==0){const auto& p=reinterpret_cast<const LaserLine*>(l)->parameters;flt(p.growth_limit);flt(p.initial_length);flt(p.end_distance);word(p.flags);}else{const auto& p=reinterpret_cast<const LaserInfinite*>(l)->parameters;flt(p.angular_velocity);flt(p.max_length);word(p.warning_frames);word(p.expand_frames);word(p.active_frames);word(p.shrink_frames);word(p.flags);}
    }
    return hash;
}
}
#endif
