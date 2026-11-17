#include "PolygonContact.hpp"
#include "CardEffects.hpp"
#include "BulletVisual.hpp"
#include "BulletPattern.hpp"
#include "PlayerDamage.hpp"
#include "EnemyDeath.hpp"
#include "EnemyOrbit.hpp"
#include "BulletSprites.hpp"
#include "GameWorld.hpp"
#include "EclVm.hpp"
#include <cmath>
#include <cstdio>
#include <algorithm>
namespace th19 {
Enemy& GameWorld::spawn_enemy(const std::string& sub_name,float x,float y,i32 life,i32 item,i32 score,
                             const std::vector<const EclProgram*>* source_programs){
    const auto& scripts=source_programs?*source_programs:programs_;
    enemies.emplace_back();
    Enemy& e=enemies.back();
    // Native constructor 0x50ac97..0x50acbb initializes both boxes to 24.
    // ExAttack2 in pl04 intentionally leaves the contact box at this default.
    e.hitbox_w=e.hitbox_h=e.contact_w=e.contact_h=24.f;
    // Includes load before the character root: wave*.ecl declares only slot 2,
    // while every plXX.ecl declares slots 2 and 3. Preserve declaration indices
    // and extend the table; stopping at the first declaration loses plXXb.anm.
    for(auto*p:scripts)for(size_t i=e.animation_files.size();i<p->anims.size();++i)
        e.animation_files.push_back(p->anims[i]);
    e.id=next_id++;e.x=x;e.y=y;e.life=life;e.item=item;e.score=score;
    last_spawned_id=e.id;
    e.vm=std::make_unique<EclVm>();
    // attach the FULL program set so cross-file sub names resolve (Wave01_GirlB01 etc.)
    for(auto* p:scripts)e.vm->attach_program(*p);
    e.vm->attach_world(this);
    e.vm->has_host=true;e.vm->host_id=e.id;
    // resolve sub index
    for(auto* p:scripts){
        for(u32 i=0;i<p->subs.size();++i)if(p->subs[i].name==sub_name){e.sub=i32(i);break;}
        if(e.sub>=0)break;
    }
    if(e.sub>=0)e.vm->start(sub_name.c_str());
    return e;
}
// ECL512 (0x501ad3), manager setters 0x40f810/0x4100e0.
bool GameWorld::register_boss(Enemy& e,int slot){
    if(slot>=int(boss_slots.size()))return false; // native asserts; reject malformed scripts safely.
    boss_control_flags&=~1u;
    if(slot<0){
        if(e.status_flags&0x80u)boss_slots[e.boss_slot]=0;
        e.status_flags&=~0x80u;
    }else{
        e.status_flags|=0x80u;boss_slots[slot]=e.id;e.boss_slot=slot;
    }
    return true;
}

bool GameWorld::receive_charge_attack(int tier,float x,float y,const std::vector<const EclProgram*>& source){
    const char* name=tier==3?"ExtraAttack":tier==4?"BossAttack":nullptr;
    if(!name)return false;
    bool found=false;for(const auto*p:source)for(const auto&s:p->subs)if(s.name==name)found=true;
    if(!found)return false;
    spawn_enemy(name,x,y,200,0,0,&source);return true;
}
// ECL1037 -> character vtable+0x28 -> ExtraInf arrival callback.
bool GameWorld::queue_extra_attack(int character,float x,float y,int variant,const std::vector<const EclProgram*>& source){
    if(character<0||character>=19)return false;
    if(character==0&&variant!=0)return false; // Other onmyo variants need their native table entry.
    const char* sub=character<=1?"ExtraAttack00":"ExAttack";
    if(character==9){static const char* names[]={"ExAttackW","ExAttackO","ExAttackE"};if(variant<0||variant>2)return false;sub=names[variant];}
    bool found=false;for(const auto*p:source)for(const auto&s:p->subs)if(s.name==sub)found=true;
    if(!found||!std::isfinite(x)||!std::isfinite(y))return false;
    ExtraFlight f;f.id=next_id++;f.character=character;f.x=x;f.y=y;
    f.source_x=opponent_x;f.source_y=opponent_y;
    f.duration=(extra_rng.next()&65535)%((character<=1||character==4||character==5||character==6)?100:character==2?80:character==3?40:60)+((character<=1||character==4||character==5||character==6)?60:character==2?80:character==3?50:70);
    f.tangent_start=extra_rng.signedUnit()*100;f.tangent_end=extra_rng.signedUnit()*100;
    f.life=character==10?2400:(character>=11)?4000:200;
    f.script=character==0?37:character==1?63:character==2?61:character==4?17:character==8?16:character==14?17:character==16?15:character==18?12:character==12?21:character==17?12:character==6?15:character==5?11:character==3?14:character==7?31:character==9?13:character==10?14:character==13?12:18;
    char file[32];std::snprintf(file,sizeof file,"pl%02db.anm",character);f.file=character<=2?"bullet.anm":file;
    f.sub=sub;f.programs=source;extra_flights.push_back(std::move(f));return true;
}
// 5402e0 -> 540620 -> 41af40/41b2e0. Reservations survive arrival
// for another 240 frames (50d640 queue, 50df40 decrement).
int GameWorld::send_native_extra_credit(GameWorld& target,const std::vector<const EclProgram*>& source,int character){
    if(character<0||character>=19)return 0;
    static constexpr int costs[]={100,90,90,80},ranCosts[]={75,65,65,55},aunnCosts[]={50,45,45,40},animalCosts[]={80,70,70,60};
    // The index is computed first: GCC 13 miscompiles a conditional array
    // subscripted by std::clamp's returned reference (it reads element 0).
    const int rank=std::clamp(difficulty,0,3);
    const int cost=(character==3?ranCosts:character==4?aunnCosts:(character==9||character==14)?animalCosts:costs)[rank];int sent=0;
    while(pending_native_attack_credit>=cost){
        pending_native_attack_credit-=cost;
        if(native_extra_reservations.size()>=20)break;
        int variant=0;
        float tx=target.extra_rng.signedUnit()*((character==11||character==13)?128:120),ty=(target.extra_rng.signedUnit()+1)*.5f*((character==11||character==13)?32:character==6?180:210)+((character==11||character==13)?0:character==6?120:130);
        // 42a3b0: Enoko places the arrival relative to the receiving player.
        // Reimu 53ac80 / 53bd80: top arrival, public bullet table entry 0.
        if(character==0){tx=target.extra_rng.signedUnit()*160;ty=(target.extra_rng.signedUnit()+1)*.5f*60;}
        if(character==3){ // 419c00: eleven arrivals, cycling all four field edges.
            const unsigned phase=native_extra_sequence%11;
            if(phase==0||phase==3||phase==7||phase==4||phase==8){tx=target.extra_rng.signedUnit()*132;ty=(phase==4||phase==8)?448:0;}
            else {tx=(phase==1||phase==6||phase==9)?-148:148;ty=(target.extra_rng.signedUnit()+1)*208+16;}
        }
        if(character==2){tx=(native_extra_sequence&1)?148-(target.extra_rng.signedUnit()+1)*16:-148+(target.extra_rng.signedUnit()+1)*16;ty=(target.extra_rng.signedUnit()+1)*10;}
        if(character==1){tx=target.extra_rng.signedUnit()*120;ty=448;}
        // Aunn 53eeb0 replaces the controller's provisional destination.
        if(character==4){tx=target.extra_rng.signedUnit()*120;ty=(target.extra_rng.signedUnit()+1)*100+40;}
        if(character==8){tx=((native_extra_sequence&1)?148:-148)+target.extra_rng.signedUnit()*16;ty=(target.extra_rng.signedUnit()+1)*100+160;}
        if(character==7){tx=target.extra_rng.signedUnit()*132;ty=(target.extra_rng.signedUnit()+1)*.5f*160;}
        if(character==12){tx=target.extra_rng.signedUnit()*128;ty=(target.extra_rng.signedUnit()+1)*.5f*32;}
        if(character==17){tx=target.extra_rng.signedUnit()*140;ty=(target.extra_rng.signedUnit()+1)*.5f*224;}
        if(character==15){tx=std::clamp(target.player_x+target.extra_rng.signedUnit()*128,-150.f,150.f);ty=std::clamp(target.player_y-(target.extra_rng.signedUnit()+1)*.5f*128,0.f,448.f);}
        if(character==9){ // 4204f0: wolf, otter, eagle each select a different entry region.
            variant=target.extra_rng.next()%3;
            if(variant==2){tx=(target.extra_rng.signedUnit()+1)*74;ty=0;}
            else {tx=(target.extra_rng.next()&1)?148:-148;const float u=(target.extra_rng.signedUnit()+1)*.5f;ty=variant==0?target.player_y-u*128:128+u*128;}
        }
        if(character==10){tx=target.extra_rng.signedUnit()*128;ty=(target.extra_rng.signedUnit()+1)*64+128;}
        if(character==14){ // 428730: arc above the receiving player, then extra upward jitter.
            const float angle=target.extra_rng.signedUnit()*(3.141592741f/6)-3.141592741f/2;
            tx=std::clamp(target.player_x+std::cos(angle)*128,-152.f,152.f);
            ty=target.player_y+std::sin(angle)*128-(target.extra_rng.signedUnit()+1)*24;
        }
        if(character==16){tx=target.extra_rng.signedUnit()*150;ty=448-(target.extra_rng.signedUnit()+1)*130;}
        if(character==18){ // 42e850: top, left, right in a three-entry cycle.
            const unsigned phase=native_extra_sequence%3;
            if(phase==0){tx=target.extra_rng.signedUnit()*128;ty=(target.extra_rng.signedUnit()+1)*16;}
            else {const float offset=(target.extra_rng.signedUnit()+1)*8;tx=phase==1?-159+offset:159-offset;ty=100+(target.extra_rng.signedUnit()+1)*80;}
        }
        if(!target.queue_extra_attack(character,tx,ty,variant,source))break;
        if(character==14)target.sound_events.push_back(44);
        auto&flight=target.extra_flights.back();flight.source_x=attack_credit_x;flight.source_y=attack_credit_y;
        native_extra_reservations.push_back(flight.duration+240);++native_extra_sequence;++sent;
    }
    for(auto&t:native_extra_reservations)--t;
    native_extra_reservations.erase(std::remove(native_extra_reservations.begin(),native_extra_reservations.end(),0),native_extra_reservations.end());
    return sent;
}
bool GameWorld::receive_spirit(const SpiritArrival&a,float x,float y){
    if(!a.sub)return false;
    bool found=false;for(const auto*p:programs_)for(const auto&s:p->subs)if(s.name==a.sub)found=true;
    if(!found)return false;
    auto&e=spawn_enemy(a.sub,x,y,a.life,0,0);e.ecl_ints=a.variables;return true;
}
void GameWorld::set_spirit_kind(Enemy&e,int kind,bool purified){
    e.kind=kind;e.status_flags=(e.status_flags&~0x780000u)|((u32(kind)&7)<<20)|(purified?0x80000u:0u);
    // 0x504d80: only kind 1 registers, once. Changing kind does not unregister.
    if((kind&7)==1)e.status_flags|=0x1000000u;
}
// 41ad77 calls 508420 with radius320. Only unconverted, eligible spirits qualify.
ChargeRegion::Point GameWorld::nazrin_seek_target(float x,float y,float fallback_x,float fallback_y)const{
    ChargeRegion::Point target{fallback_x,fallback_y};float best=320.f*320.f;
    for(const auto&e:enemies){
        if(!e.active||(e.flags&0x21)||!(e.status_flags&0x700000u)||(e.status_flags&0x80c00u))continue;
        const float dx=e.x-x,dy=e.y-y,d=dx*dx+dy*dy;
        if(d<best){best=d;target={e.x,e.y};}
    }
    return target;
}
int GameWorld::counted_spirits() const{
    // 0x50aa67 unregisters on destruction, including escape/script removal.
    int n=0;for(const auto&e:enemies)if(e.active&&(e.status_flags&0x1000000u))++n;return n;
}
bool GameWorld::fire_bullets(const LaserShooter&s,float ox,float oy){
    for(const auto&c:s.changes)if(!supportedBulletChange(c))return false;
    const int sprite=nativeBulletSprite(s.type,s.color);
    for(const auto&c:s.changes){if(c.flags==2&&!bullet_scripts)return false;
        if(c.flags==0x100&&c.integers[1]&&(!bullet_scripts||!bullet_sprite_sizes))return false;}
    // Unknown changes must never silently become straight bullets.
    if(s.has_unhandled_changes||(s.aim_mode<0||s.aim_mode>10)||(sprite<0&&(!bullet_scripts||!bullet_scripts->count(nativeBulletAnimation(s.type))))||s.count1>32767||s.count2>32767||size_t(s.count1)*s.count2>8192)return false;
    const float x=(s.absolute_origin?s.absolute_x:ox)+s.x,y=(s.absolute_origin?s.absolute_y:oy)+s.y,dx=player_x-x,dy=player_y-y;
    const float aim=dx==0&&dy==0?1.57079632679f:std::atan2(dy,dx);
    for(int row=0;row<s.count2;++row)for(int col=0;col<s.count1;++col){
        const float randomAngle=(s.aim_mode==6||s.aim_mode==8)?bullet_rng.signedUnit():0;
        const float randomSpeed=(s.aim_mode==7||s.aim_mode==8)?bullet_rng.unit():0;
        BulletPatternPoint p;if(!nativeBulletPattern(s.aim_mode,s.count1,s.count2,col,row,s.angle,s.angle2,s.speed,s.speed2,aim,p,randomAngle,randomSpeed))return false;
        // A full native pool refuses the new bullet; existing live bullets stay.
        // Keep the emitter's random draws in order even when its pool is full.
        if(bullets.size()>=bullet_capacity){++dropped_bullets;continue;}
        auto&b=spawn_bullet();b.x=x+std::cos(p.angle)*s.muzzle_radius;b.y=y+std::sin(p.angle)*s.muzzle_radius;b.kind=s.type;b.color=s.color;b.sprite=sprite;b.scripted=true;
        b.vx=std::cos(p.angle)*p.speed;b.vy=std::sin(p.angle)*p.speed;
        b.cancel_script=nativeBulletCancelScript(b.kind,b.color);b.visual_heading=p.angle;b.changes.records=s.changes;b.change_sound=s.end_sound;
        if(bullet_scripts){auto it=bullet_scripts->find(nativeBulletAnimation(s.type));if(it!=bullet_scripts->end()){
            b.animation.bind_entry(it->second.stream,it->second.len,bullet_scripts);b.animation.tick();
            if(s.changes[0].flags!=2){b.animation.interrupt(2);b.animation.tick();}}}
        if(bullet_scripts){int id=nativeBulletAttachedEffect(b.kind);auto it=bullet_scripts->find(id);
            if(id&&it!=bullet_scripts->end()){b.attached_effect.bind_entry(it->second.stream,it->second.len,bullet_scripts);b.attached_effect.tick();}}
        const int sounds=b.changes.begin(p.speed,p.angle,b.x,b.y,player_x,player_y);
        apply_bullet_changes(b);
        if(b.state==4)continue;
        if(b.changes.animation_interrupt>=0){
            // 4f3910: an initial mode-1 spawn runs the default ANM entry.
            // Its label 8 is only dispatched by a later change, not at creation.
            if(s.changes[0].flags!=2||static_cast<short>(s.changes[0].integers[0])!=1)
                b.animation.interrupt(b.changes.animation_interrupt);
            b.animation.tick();
            b.x-=4*b.vx;b.y-=4*b.vy;b.state=2;b.changes.animation_interrupt=-1;}
        if(b.changes.native_timer>=0){b.native_timer=b.changes.native_timer;b.changes.native_timer=-1;}
        for(int i=0;i<sounds&&b.change_sound>=0;++i)sound_events.push_back(b.change_sound);
    }
    if((s.shot_flags&0x20)&&s.sound>=0)sound_events.push_back(s.sound);
    return true;
}
Bullet& GameWorld::spawn_bullet(){bullets.emplace_back();bullets.back().id=next_id++;return bullets.back();}
Spirit& GameWorld::spawn_spirit(float x,float y){spirits.emplace_back();spirits.back().id=next_id++;spirits.back().x=x;spirits.back().y=y;return spirits.back();}
bool GameWorld::damage_enemy(Enemy& e,int damage,bool forced){
    // 0x509980: bit 0/5 excludes damage; bit 4 excludes HP reduction;
    // bit 7 suppresses automatic death. Invisible controllers are never targets.
    if(!e.active||e.sprite<0||(e.flags&0x21)||(!forced&&damage<=0))return false;
    if(e.death_pending||!e.pending_phase.empty())return false;
    if(!forced){if(card_near_damage&&(player_x-e.x)*(player_x-e.x)+(player_y-e.y)*(player_y-e.y)<=2304.f){
            if(e.card_damage_frame!=battle_frame){e.card_damage_frame=battle_frame;e.card_damage_sum=0;}
            const int previous=e.card_damage_sum;e.card_damage_sum+=damage;
            damage+=e.card_damage_sum/2-previous/2;
        }
        // 0x520864..0x520875 caps the aggregate damage for one target,
        // after the proximity-card bonus. Overlapping flame regions share it.
        if(player_damage_cap>0){
            if(e.damage_budget_frame!=battle_frame){e.damage_budget_frame=battle_frame;e.damage_budget_used=0;}
            damage=std::min(damage,std::max(0,player_damage_cap-e.damage_budget_used));
            e.damage_budget_used+=damage;
            if(damage<=0)return false;
        }
        e.hit_pending=true;
        if(e.native_update_callback==1)e.recoil_pending=true;
        // 0x509e16: keep hit feedback while protecting HP, before phase/death checks.
        if((e.flags&0x10)||int(e.invulnerability_time)>0)return false;
        if(e.spell_damage_scaled){
            // 0x509980: HP+0xc stores raw damage; HP+0x10 is the native anchor.
            e.life_reset_c=i32(u32(e.life_reset_c)-u32(damage));
            e.life=i32((int64_t(e.life_reset_c)-int64_t(e.life_scale_anchor)*7)/7+e.life_scale_anchor);
        }else e.life-=damage;
        auto trigger=e.phases.check(e.life,false);
        if(trigger&&!trigger.sub.empty()){
            e.life=trigger.hp;e.phases.consume(trigger);e.status_flags&=~0x100u;e.pending_phase=trigger.sub;
            // Restart at the next safe world update; never destroy a VM currently
            // executing ECL or invalidate callers iterating the enemy vector.
            return false;
        }
        if(e.life>0||(e.flags&0x80))return false;
    }
    // 0x50c4ca: the death callback runs after death effects/rewards. Keep the
    // host until the damage pass ends, so an ECL spawn cannot invalidate a
    // caller's enemy iteration. Only native flag 0x08000000 preserves it.
    e.death_pending=!e.death_callback.empty()&&bool(e.vm);
    e.active=e.death_pending;++defeated;earned_score+=std::max(0,e.score);
    // Native death dispatch preserves ECL323's file/script override.
    const int deathScript=e.death_script?e.death_script:nativeEnemyDeathScript(e.animations[0].file,e.sprite);
    const float heading=e.vx*e.vx+e.vy*e.vy<.2f*.2f?-1.57079632679f:std::atan2(e.vy,e.vx);
    deaths.push_back({e.x,e.y,heading,deathScript,3+int(e.id&1),e.death_file});
    auto drop=[&](int type,float vx,float vy){if(type<1||type>14)return;Pickup p;p.id=next_id++;p.x=std::clamp(e.x,-192.f,192.f);p.y=e.y;p.type=type;p.vx=vx;p.vy=vy;pickups.push_back(p);};
    if(e.item>0)drop(e.item,0,-2.2f);
    e.item=0; // 0x50c3c5: an item is dropped once, even if the host revives.
    // 0x50c1f0 -> 0x549780: excluded actors do not extend/count the chain.
    // ECL 1032 supplies the extension (default 20), capped at 60 frames.
    if(!(e.status_flags&0x20000000u)){
        // 0x50c1f0 -> 0x51d4e0/0x51cc70: independent expanding damage region.
        const int kind=(e.status_flags>>20)&7;
        combo_attack.on_death(kind, combo);
        // 0x51d4e0 case0 checks the pre-increment combo count; case3 (kind5)
        // contributes 50 independently. Other death rewards need mode parameters.
        if(reward_mode>=0){const auto r=nativeDeathRewards(kind,e.ecl_ints[2],combo,battle_frame,reward_mode,reward_stage);
            if(reward_mode==0)pending_story_credit+=(r.bullets?1:0)+(r.pure?5:0)+(r.charge?10:0);
            if(r.bullets)pure_spirit_meter.add_bullets(r.bullets,difficulty,e.x,e.y);
            if(r.pure)pure_spirit_meter.add(r.pure,difficulty,e.x,e.y);
            // 0x548d00 feeds attack-manager +0x1c (consumed by character routines), NOT a pickup/15-point
            // pending_energy increment. Retain the event until that consumer is wired.
            if(r.charge){attack_credit_x=e.x;attack_credit_y=e.y;}
            pending_native_attack_credit+=int(r.charge*SpiritMeter::multiplier(difficulty))+
                (std::clamp(spirit_rank,0,std::max(0,spirit_rank_cap))*r.charge/100)*2;
        }else{
            if(kind==0&&combo>9)pure_spirit_meter.add(30,difficulty,e.x,e.y);
            if(kind==5)pure_spirit_meter.add(50,difficulty,e.x,e.y);
        }
        // 0x50c41b -> 0x51d4e0 case 1: ordinary enemies do NOT send animals.
        // +0x14d4 is the third host integer (-9983), not the attack level.
        if(kind>=1&&kind<=4)spirit_requests.push_back({e.x,e.y,deathSpiritKind(e.x,e.y,player_x,player_y),e.ecl_ints[2]});
        if(kind<=6){DeathChain c;c.x=e.x;c.y=e.y;c.target=e.spirit_type;
            c.duration=e.spirit_params[2];c.damage=e.spirit_params[0];
            c.type=kind==0?0:kind<=4?1:kind==5?3:4;
            if(c.duration>0){death_chains.push_back(c);death_chain_visuals.push_back(c);}}

        combo_timer=std::min(60,combo_timer+e.combo_frames);
        ++combo;combo_peak=std::max(combo_peak,combo);
        // 0x544f30: show only chains above one and refresh on a higher count.
        if(combo>1&&combo>combo_display&&combo_timer>0){combo_display=combo;combo_display_timer=120;}
    }
    // 0x50c1f0/0x521c60: six or more chain hits permit extra charge drops.
    // Flag 0x8000000 limits the number of drop attempts to one.
    if(combo>5)for(int n=0;n<((e.status_flags&0x8000000u)?1:std::min(3,combo/12+1));++n){pickup_fraction+=pickup_rate;while(pickup_fraction>=100){pickup_fraction-=100;drop(9,0,-2.f);}}
    return true;
}
void GameWorld::finish_enemy_deaths(){
    for(size_t i=0;i<enemies.size();++i){
        if(!enemies[i].death_pending)continue;
        const std::string callback=enemies[i].death_callback;
        auto*vm=enemies[i].vm.get();
        // 0x50c4d9..0x50c510 resets the VM, executes time zero, clears the
        // one-shot name, then tests the retention flag. Reacquire after step.
        const bool started=vm&&vm->start(callback.c_str());
        if(started)vm->step();
        auto&e=enemies[i];e.death_callback.clear();e.death_pending=false;
        if(!started||!(e.status_flags&0x08000000u))e.active=false;
    }
}
// 0x509980 tail: evaluate once after all damage, never once per projectile.
void GameWorld::finish_hit_feedback(u32 mode){
    finish_enemy_deaths();
    for(auto&e:enemies){
        const bool hit=e.hit_pending;e.hit_pending=false;
        if(!e.active)continue;
        // 0x5089d0 calls damage/feedback first, then decrements positive timer.
        // Do this once after all shots, even during hit-color cooldown.
        if(int(e.invulnerability_time)>0){
            const bool slowed=!(e.status_flags&0x700000u)&&e.sprite>=0&&!(e.flags&0x20)&&charge_region.contains(e.x,e.y,e.hitbox_w*.5f);
            e.invulnerability_time-=slowed?.5f:1.f;
        }
        e.feedback_color=0;
        if(e.hit_cooldown>0){--e.hit_cooldown;continue;}
        const bool boss=(e.status_flags&0x4080)!=0&&(mode&9)!=9;
        const int hp=e.phase_health();
        if(hit&&!(e.flags&0x2000)){
            e.feedback_color=0xff0000ff;e.hit_cooldown=4;
            sound_events.push_back(e.hit_sound>=0?e.hit_sound:
                boss&&hp<((mode&1)?200:900)?35:34);
        }else if((e.age&3)==0){
            if(e.status_flags&0x8000)e.feedback_color=0xffff00ff;
            if(boss&&hp<=((mode&1)?100:500))e.feedback_color=0xff0000ff;
        }
    }
}
int GameWorld::cancel_bullets_circle(float x,float y,float radius,int limit,bool respect_lock){
    // 0x4ee770/0x4ee950: live or spawning, unlocked; include half hit width.
    int count=0;for(auto&b:bullets){if(count>=limit)break;
        if((b.state!=1&&b.state!=2)||(respect_lock&&b.cancel_lock!=0))continue;
        const float dx=b.x-x,dy=b.y-y,r=radius+b.hit_width*.5f;
        if(dx*dx+dy*dy>r*r)continue;
        b.state=4;++count;++cancelled_bullets;
        if(b.cancel_script<0)b.cancel_script=nativeBulletCancelScript(b.kind,b.color);
        if(!(b.flags&0x200)){bullet_cancels.push_back({b.x,b.y,b.cancel_script});sound_events.push_back(71);}
    }
    return count;
}
int GameWorld::cancel_bullets_shape(const std::function<bool(float,float,float)>& contact){
 int count=0;for(auto&b:bullets){
  if((b.state!=1&&b.state!=2)||b.cancel_lock!=0||!contact(b.x,b.y,b.hit_width))continue;
  b.state=4;++count;++cancelled_bullets;
  if(b.cancel_script<0)b.cancel_script=nativeBulletCancelScript(b.kind,b.color);
  if(!(b.flags&0x200)){bullet_cancels.push_back({b.x,b.y,b.cancel_script});sound_events.push_back(71);}
 }return count;
}
int GameWorld::cancel_bullets_polygon(float x,float y,float radius,float angle,int sides){
 int count=0;
 for(auto&b:bullets){
  if((b.state!=1&&b.state!=2)||b.cancel_lock!=0)continue;
  if(!nativePolygonContact(b.x-x,b.y-y,radius,angle,sides,b.hit_width))continue;
  b.state=4;++count;++cancelled_bullets;
  if(b.cancel_script<0)b.cancel_script=nativeBulletCancelScript(b.kind,b.color);
  if(!(b.flags&0x200)){bullet_cancels.push_back({b.x,b.y,b.cancel_script});sound_events.push_back(71);}
 }return count;
}
int GameWorld::cancel_bullets_rect(float x,float y,float width,float height,float angle){
    // 4ee0d0 -> 4d44e0: transform into the rectangle local axes,
    // including the rounded corners, and preserving cancellation locks.
    int count=0;
    for(auto&b:bullets){
        if((b.state!=1&&b.state!=2)||b.cancel_lock!=0)continue;
        float rx=b.x-x,ry=b.y-y;
        float dx=std::abs(rx*std::cos(angle)+ry*std::sin(angle)),dy=std::abs(-rx*std::sin(angle)+ry*std::cos(angle)),hw=width*.5f,hh=height*.5f,r=b.hit_width;
        bool hit=(dx<=hw+r&&dy<=hh)||(dx<=hw&&dy<=hh+r);
        if(!hit){float cx=std::max(0.f,dx-hw),cy=std::max(0.f,dy-hh);hit=cx*cx+cy*cy<r*r;}
        if(!hit)continue;
        b.state=4;++count;++cancelled_bullets;
        if(b.cancel_script<0)b.cancel_script=nativeBulletCancelScript(b.kind,b.color);
        if(!(b.flags&0x200)){bullet_cancels.push_back({b.x,b.y,b.cancel_script});sound_events.push_back(71);}
    }
    return count;
}
// Retail 0x54aae0 -> 0x4ee0d0 and 0x5303f0. No bullet-style whitelist.
void GameWorld::cancel_enoko_trap(float x,float y,float angle){
    cancel_bullets_shape([=](float px,float py,float radius){
        return nativeEnemyRectangleContact(px,py-224,0,296,448,radius) &&
               nativeEnemyRectangleContact(px-x,py-y,angle,24,56,radius);
    });
    const auto point=[=](float px,float py){
        const float dx=px-x,dy=py-y,c=std::cos(angle),s=std::sin(angle);
        return std::abs(dx*c+dy*s)<=12&&std::abs(dy*c-dx*s)<=28;
    };
    // 52a970/52d350 -> 4d2c30: oriented rectangle against each
    // 16-unit sample rectangle (dimensions are the native step vector).
    const auto sample=[=](float px,float py,float a){
        const float c=std::cos(angle),s=std::sin(angle),bc=std::cos(a),bs=std::sin(a);
        const float hx=std::abs(8*bc),hy=std::abs(8*bs),dx=px-x,dy=py-y;
        if(std::hypot(dx,dy)>=std::hypot(12.f,28.f)+std::hypot(hx,hy))return false;
        const float axes[4][2]={{c,s},{-s,c},{bc,bs},{-bs,bc}};
        for(const auto&axis:axes){const float u=axis[0],v=axis[1];
            if(std::abs(dx*u+dy*v)>12*std::abs(c*u+s*v)+28*std::abs(-s*u+c*v)
                +hx*std::abs(bc*u+bs*v)+hy*std::abs(-bs*u+bc*v))return false;
        }
        return true;
    };
    cancel_lasers_region(point,true,sample);
}
int GameWorld::cancel_lasers_circle(float x,float y,float radius,bool respect_lock){
    return cancel_lasers_region([=](float px,float py){return (x-px)*(x-px)+(y-py)*(y-py)<=radius*radius;},respect_lock);
}
int GameWorld::cancel_lasers_region(const std::function<bool(float,float)>& insideRegion,bool respect_lock,
    const std::function<bool(float,float,float)>& rectangleSample){
    // 0x530c60 -> 0x52a040 (+0x30); 0x52ac90 splits on 16-unit samples.
    const size_t initial=lasers.size();int hits=0;
    for(size_t i=0;i<initial;++i){auto base=lasers[i];
        if(!base.active||base.phase==1||(respect_lock&&base.cancel_lock!=0))continue;
        if(base.curved){
            // 524930 marks individual nodes; 525270 preserves runs of >=4.
            const auto&nodes=base.curve.nodes;std::vector<bool> cut(nodes.size());bool touched=false;
            for(size_t k=0;k<nodes.size();++k)if(insideRegion(nodes[k].x,nodes[k].y)){
                cut[k]=true;touched=true;++hits;++cancelled_bullets;
                if(k%10==0)bullet_cancels.push_back({nodes[k].x,nodes[k].y,212+base.source.color*2});
            }
            if(!touched)continue;
            lasers[i].active=false;bool kept=false;
            for(size_t k=0;k<nodes.size();){if(cut[k]){++k;continue;}
                size_t begin=k;while(k<nodes.size()&&!cut[k])++k;
                if(k-begin<4)continue;
                // Native dynamic curves keep only the first surviving run.
                if(base.curve.dynamic&&kept)break;
                auto part=base;part.curve.nodes.assign(nodes.begin()+begin,nodes.begin()+k);
                if(!base.curve.dynamic)part.curve.time-=int(begin);
                if(!kept){lasers[i]=part;kept=true;}
                else{part.id=next_id++;lasers.push_back(std::move(part));}
            }
            continue;
        }
        if(base.straight&&(base.source.type==8||rectangleSample)){ // 0x52c9f0 -> 0x52d680, 16-unit samples.
            std::vector<bool> mask;
            for(float d=8;(rectangleSample?d+8<=base.length:d+8<base.length)&&mask.size()<512;d+=16){float px=base.x+std::cos(base.angle)*d,py=base.y+std::sin(base.angle)*d;
                bool cut=rectangleSample?rectangleSample(px,py,base.angle):insideRegion(px,py);mask.push_back(cut);
                if(cut){++hits;++cancelled_bullets;bullet_cancels.push_back({px,py,212+base.source.color*2});}}
            if(std::none_of(mask.begin(),mask.end(),[](bool b){return b;}))continue;
            lasers[i].active=false;bool kept=false;
            for(size_t k=0;k<mask.size();){if(mask[k]){++k;continue;}size_t first=k;while(k<mask.size()&&!mask[k])++k;
                float len=16.f*(k-first);if(len<=32)continue;
                auto part=base;const float offset=16.f*first;part.x+=std::cos(base.angle)*offset;part.y+=std::sin(base.angle)*offset;
                part.length=part.straight_target=len;part.travelled=0;part.travel_cap=len;part.offscreen_grace=30;
                if(!kept){part.id=base.id;part.travel_cap=base.travel_cap;part.travelled=first?offset:base.travelled;part.offscreen_grace=base.offscreen_grace;lasers[i]=part;kept=true;}else{part.id=next_id++;part.straight_changes={};part.straight_bounds_grace=3;part.change_sounds=0;lasers.push_back(part);}
            }
            continue;
        }
        // Only descriptor 38 and its moving remnants are verified at present.
        if(base.source.type!=38&&!rectangleSample)continue;
        std::vector<bool> cut;
        for(float d=8;(rectangleSample?d+8<=base.length:d+8<base.length)&&cut.size()<512;d+=16){
            float px=base.x+std::cos(base.angle)*d,py=base.y+std::sin(base.angle)*d;
            bool inside=rectangleSample?rectangleSample(px,py,base.angle):insideRegion(px,py);cut.push_back(inside);
            if(inside){++hits;++cancelled_bullets;
                if(px+32>-148&&px-32<148&&py+32>0&&py-32<448)
                    bullet_cancels.push_back({px,py,212+base.source.color*2});}
        }
        if(std::none_of(cut.begin(),cut.end(),[](bool v){return v;}))continue;
        size_t first=0;while(first<cut.size()&&!cut[first])++first;
        lasers[i].length=float(first)*16;bool kept=first>0;
        for(size_t k=first;k<cut.size();){if(cut[k]){++k;continue;}
            const size_t begin=k;while(k<cut.size()&&!cut[k])++k;
            auto part=base;float offset=float(begin)*16;
            part.x+=std::cos(base.angle)*offset;part.y+=std::sin(base.angle)*offset;
            part.length=float(k-begin)*16;
            part.fragment=true;part.phase=2;part.clock=0;part.age=0;part.travelled=0;
            part.travel_cap=(base.fragment?base.travel_cap-base.travelled:base.source.max_length)-offset;
            if(base.fragment&&!kept){part.id=base.id;lasers[i]=part;kept=true;}
            else{part.id=next_id++;lasers.push_back(part);}
        }
        if(base.fragment&&!kept)lasers[i].active=false;
    }
    return hits;
}
void GameWorld::tick_death_chains(){
    // Process the initial list only. Deaths generated here start next tick;
    // copying the record avoids invalid references when another death appends.
    const size_t count=death_chains.size();
    for(size_t i=0;i<count;++i){auto c=death_chains[i];
        c.radius=1+(c.target-1)*nativeEase(5,float(c.age)/c.duration);
        if(c.age<c.duration&&c.damage!=0)for(auto&e:enemies){
            if(!e.active||e.sprite<0)continue;
            float dx=std::max(0.f,std::abs(e.x-c.x)-e.hitbox_w*.5f);
            float dy=std::max(0.f,std::abs(e.y-c.y)-e.hitbox_h*.5f);
            if(dx*dx+dy*dy<=c.radius*c.radius){e.status_flags|=0x2000000;damage_enemy(e,c.damage);}
        }
        // 0x51c8c0: only live descriptors 0..3, radius+1, without the
        // ordinary cancellation lock/width filter. Each becomes a cross-field transit.
        if(c.age<c.duration)for(auto&b:bullets)if(tsukasaReflects(b.state,unsigned(b.kind),b.x,b.y,c.x,c.y,c.radius)){
            chain_bullet_transits.push_back({b.x,b.y,std::hypot(b.vx,b.vy),b.kind,b.color,battle_frame});
            b.state=4;++cancelled_bullets;
            if(b.cancel_script<0)b.cancel_script=nativeBulletCancelScript(b.kind,b.color);
            if(!(b.flags&0x200)){bullet_cancels.push_back({b.x,b.y,b.cancel_script});sound_events.push_back(71);}
            ++combo;combo_peak=std::max(combo_peak,combo);
            if(combo>1&&combo>combo_display&&combo_timer>0){combo_display=combo;combo_display_timer=120;}
            pure_spirit_meter.add_bullets(30,difficulty,b.x,b.y);
            attack_credit_x=b.x;attack_credit_y=b.y;
            pending_native_attack_credit+=int(SpiritMeter::multiplier(difficulty))+
                (std::clamp(spirit_rank,0,std::max(0,spirit_rank_cap))/100)*2;
        }
        ++c.age;death_chains[i]=c;
    }
    death_chains.erase(std::remove_if(death_chains.begin(),death_chains.end(),[](const auto&c){return c.age>=c.duration;}),death_chains.end());
}
void GameWorld::tick_pickups(float px,float py){
    // 0x520d40; defaults from Player +0x20b0..0x20c0: 5,30,70,70,128.
    for(auto&p:pickups){if(!p.active)continue;float dx=px-p.x,dy=py-p.y,d=std::hypot(dx,dy);
        if(py<128&&p.state!=3){p.state=3;p.speed=pickup_speed;}
        if(p.state==1&&d<pickup_range){p.state=4;p.speed=pickup_speed/3.f;}
        if(p.state==3||p.state==4){float step=std::min(d,p.speed);if(d>0){p.x+=dx/d*step;p.y+=dy/d*step;}p.speed=std::min(12.f,p.speed+.2f);}
        else{p.x+=p.vx;p.y+=p.vy;p.vy=std::min(2.f,p.vy+.03f);if(p.vy>=0)p.vx=0;}
        if(std::hypot(px-p.x,py-p.y)<30){p.active=false;++collected;if(p.type>=9&&p.type<=14)pending_energy+=15;sound_events.push_back(37);}
        if(p.y>496||std::abs(p.x)>240)p.active=false;
    }
    pickups.erase(std::remove_if(pickups.begin(),pickups.end(),[](const auto&p){return !p.active;}),pickups.end());
}
int GameWorld::purify_orin(float x,float y,float radius){
    // 0x41ca40 -> 0x509030 animal-kind branch: switch same host to its callback.
    int converted=0;for(auto&e:enemies){
        if(!e.active||!(e.status_flags&0x700000)||(e.status_flags&0x80000)||e.callbacks[0].empty()||!e.vm)continue;
        float dx=e.x-x,dy=e.y-y,r=radius+e.hitbox_w*.5f;if(dx*dx+dy*dy>r*r)continue;
        if(e.vm->start(e.callbacks[0].c_str())){e.status_flags|=0x80000;e.spirit_aura_exit=true;++converted;}
    }return converted;
}
int GameWorld::convert_zanmu(float x,float y,float radius,int&ordinary_count){
    // 0x42e850: snapshot originals; created purified spirits cannot convert again.
    struct Spawn{float x,y;const char* sub;};std::vector<Spawn> pending;
    for(auto&e:enemies){const auto f=e.status_flags;
        if(!e.active||!(f&(0x10000000u|0x700000u))||(f&(0x200u|0x800000u)))continue;
        float dx=e.x-x,dy=e.y-y;if(dx*dx+dy*dy>=radius*radius)continue;
        const char*sub="GhostPure2G";
        if(f&0x10000000u){sub=ordinary_count%3==0?"GhostPure2b":"GhostPure2";++ordinary_count;}
        pending.push_back({e.x,e.y,sub});e.status_flags|=0x200;e.active=false;
    }
    for(const auto&p:pending)spawn_enemy(p.sub,p.x,p.y,100,0,0);
    return int(pending.size());
}
// ECL624(2), 0x4fbff0: strict 32-unit radius, age >20, last host ID.
// 0x4f11a0 changes appearance/contact size without resetting motion or age.
void GameWorld::strengthen_tsukasa_bullets(const Enemy& e){
    for(auto&b:bullets){
        if(b.state==4||b.age<=20||b.last_tsukasa_id==e.id)continue;
        const float dx=b.x-e.x,dy=b.y-e.y;
        if(dx*dx+dy*dy>=1024.f)continue;
        b.last_tsukasa_id=e.id;
        int kind=-1,color=1;
        switch(b.kind){case 0:case 1:kind=4;color=b.color;break;
            case 4:case 5:kind=18;break;case 8:kind=20;break;
            case 9:kind=31;break;case 18:kind=33;break;default:continue;}
        b.kind=kind;b.color=color;b.sprite=nativeBulletSprite(kind,color);
        b.hit_width=nativeBulletContactRadius(kind);
        b.cancel_script=nativeBulletCancelScript(kind,color);
        b.animation={};b.attached_effect={};
        if(bullet_scripts){
            auto it=bullet_scripts->find(nativeBulletAnimation(kind));
            if(it!=bullet_scripts->end()){b.animation.bind_entry(it->second.stream,it->second.len,bullet_scripts);b.animation.tick();}
            const int attached=nativeBulletAttachedEffect(kind);it=bullet_scripts->find(attached);
            if(attached&&it!=bullet_scripts->end())b.attached_effect.bind_entry(it->second.stream,it->second.len,bullet_scripts);
        }
    }
}
void GameWorld::apply_bullet_changes(Bullet& b){
    // 0x4f1460 immediate type/color/ANM, sound and blend-mode changes.
    for(const auto&c:b.changes.immediate_events){
        if(c.flags==0x400){
            // 0x4f1460 -> 0x4eeb30(0): scripted cancellation produces no item.
            // Parameter 1 disables only the cancellation ANM, not its sound.
            if(c.integers[0]==1)b.cancel_script=-1;
            if(b.state!=4&&!(b.flags&0x200)){
                if(b.cancel_script>=0)bullet_cancels.push_back({b.x,b.y,b.cancel_script});
                sound_events.push_back(71);
            }
            b.state=4;continue;
        }
        if(c.flags==0x800){sound_events.push_back(c.integers[0]);continue;}
        // Bullet+4d3 = ANM+4bb: 2 subtractive, 1 additive, all others alpha.
        if(c.flags==0x100000){b.animation.blend_mode=c.integers[0]==2?2:c.integers[0]==1?1:0;continue;}
        b.kind=c.integers[0];b.color=c.integers[1]&0x7fff;
        b.sprite=nativeBulletSprite(b.kind,b.color);
        b.cancel_script=nativeBulletCancelScript(b.kind,b.color);
        b.animation={};b.attached_effect={};
        if(bullet_scripts){auto it=bullet_scripts->find(nativeBulletAnimation(b.kind));
            if(it!=bullet_scripts->end()){
                b.animation.bind_entry(it->second.stream,it->second.len,bullet_scripts);b.animation.tick();
                // Native color high bit requests label 2; otherwise keep the startup script.
                if(c.integers[1]&0x8000){b.animation.interrupt(2);b.animation.tick();}
            }
        }
    }
    b.changes.immediate_events.clear();
}
// 4f5130 -> 549fb0 -> 521c60: physical and positive aura contact share
// graze stock, attached animation, secondary tint and energy-item accumulator.
void GameWorld::tick_bullet_graze(float px,float py,float player_radius){
    for(auto&b:bullets){
        b.graze_x=b.graze_y=0;b.graze_tint=0;
        if(b.state!=1||(b.flags&0x200))continue;
        const int contact=nativeOrdinaryBulletContact(b.kind,px-b.x,py-b.y,nativeBulletContactRadius(b.kind),player_radius);
        // The positive (X) region is checked independently of the normal
        // 40px graze shell. A small bullet can therefore be both in the
        // visual graze shell and inside the charge region; retail credits the
        // charge-region reward path in that case and does not spawn a pickup.
        const bool aura_graze=charge_region.contains(b.x,b.y,nativeBulletContactRadius(b.kind));
        const bool grazed=contact==2||aura_graze;
        const auto result=b.graze.tick(grazed);
        if(result.interrupt&&b.attached_effect.stream){b.attached_effect.interrupt(result.interrupt);b.attached_effect.tick();}
        if(grazed){b.graze_tint=0xffff0080u|(u32(result.alpha)<<8);b.graze_y=graze_rng.signedUnit();b.graze_x=graze_rng.signedUnit();}
        if(!result.reward)continue;
        if(aura_graze){
            // 0x4f5130 -> 0x549fb0: the positive-region graze reward emits
            // exactly one type-9 charge point at the reward tick. Native
            // pickup initialization uses vx=0, vy=-2 (upward throw).
            Pickup p;p.id=next_id++;p.x=b.x;p.y=b.y;p.type=9;p.vx=0;p.vy=-2.f;pickups.push_back(p);
            continue;
        }
        grazed_bullets=std::min(grazed_bullets+1,99999999);sound_events.push_back(42);
        GrazeParticle particle;particle.start(b.x,b.y,nativeGrazeColor(b.kind,b.color),graze_rng);graze_particles.push_back(particle);
        // Retail 0x4f5130 routes positive charge-region contact through the
        // same 521c60 reward path: create a visible charge point first. The
        // player gauge is increased only when that point is collected by
        // tick_pickups (not at the moment of bullet contact).
        graze_fraction+=pickup_rate;
        while(graze_fraction>=100){graze_fraction-=100;
            const float a=-1.5707963705f+graze_rng.signedUnit()*.1745329252f;
            Pickup p;p.id=next_id++;p.x=b.x;p.y=b.y;p.type=9;p.vx=2*std::cos(a);p.vy=2*std::sin(a);pickups.push_back(p);
        }
    }
}
void GameWorld::tick_wave(){
    ++battle_frame;
    for(auto&f:extra_flights)if(++f.age>=f.duration){
        spawn_enemy(f.sub,f.x,f.y,f.life,0,0,&f.programs);if(f.character==4||f.character==5||f.character==6)sound_events.push_back(62);++extra_arrivals;
    }
    extra_flights.erase(std::remove_if(extra_flights.begin(),extra_flights.end(),[](const auto&f){return f.age>=f.duration;}),extra_flights.end());
    for(auto&p:graze_particles)p.tick(graze_rng);
    graze_particles.erase(std::remove_if(graze_particles.begin(),graze_particles.end(),[](const auto&p){return !p.active;}),graze_particles.end());
    int counterCount=0;
    const int pureCount=pure_spirit_meter.tick(spirit_rank,spirit_rank_cap,counted_spirits(),&counterCount);
    for(int i=0;i<counterCount;++i)counter_bullet_requests.push_back({pure_spirit_meter.x,pure_spirit_meter.y});
    for(int i=0;i<pureCount;++i)spirit_requests.push_back({pure_spirit_meter.x,pure_spirit_meter.y,1,0});
    tick_death_chains();
    bullets.erase(std::remove_if(bullets.begin(),bullets.end(),[](const auto&b){return b.state==4;}),bullets.end());
    for(auto&b:bullets)if((b.reflected||b.scripted)&&(b.state==1||b.state==2)){
        // 4f5680 case 2 first drifts half a step, then tests the ANM flag
        // as it stood on entry. On the release frame case 1 also runs.
        const bool spawning=b.state==2;
        if(spawning){b.x+=b.vx*.5f;b.y+=b.vy*.5f;if(b.animation.ivar[0])b.state=1;}
        const bool auraSlow=!spawning&&b.state==1&&!(b.flags&0x200)&&charge_region.contains(b.x,b.y,nativeBulletContactRadius(b.kind));
        if(b.state==1&&b.scripted){
            float spriteWidth=0,spriteHeight=0;
            if(bullet_sprite_sizes){auto it=bullet_sprite_sizes->find(b.animation.stream?b.animation.sprite:b.sprite);
                if(it!=bullet_sprite_sizes->end()){spriteWidth=it->second[0];spriteHeight=it->second[1];}}
            int sounds=b.changes.tick(b.vx,b.vy,b.x,b.y,player_x,player_y,auraSlow?.5f:1.f,spriteWidth,spriteHeight);
            apply_bullet_changes(b);
            if(b.state==4)continue;
            for(int i=0;i<sounds&&b.change_sound>=0;++i)sound_events.push_back(b.change_sound);
            if(b.changes.animation_interrupt>=0){b.animation.interrupt(b.changes.animation_interrupt);
                b.x-=4*b.vx;b.y-=4*b.vy;b.state=2;b.changes.animation_interrupt=-1;}
            if(b.changes.native_timer>=0){b.native_timer=b.changes.native_timer;b.changes.native_timer=-1;}}
        // 0x4f5219 sets Bullet+0x1024 to 0.5 on positive Aura contact.
        // Spawning bullets and flag0x200 are excluded by 0x4f5680.
        if(b.state==1&&!(b.flags&0x200)){const float movement=auraSlow?.5f:1.f;b.x+=b.vx*movement;b.y+=b.vy*movement;}
        if(b.scripted)b.visual_heading=b.changes.angle;
        else if(b.vx!=0||b.vy!=0)b.visual_heading=std::atan2(b.vy,b.vx);
        if(!b.animation.stream&&bullet_scripts){auto it=bullet_scripts->find(nativeBulletAnimation(b.kind));if(it!=bullet_scripts->end()){b.animation.bind_entry(it->second.stream,it->second.len,bullet_scripts);b.animation.tick();b.animation.interrupt(2);}}
        if(!b.attached_effect.stream&&bullet_scripts){int id=nativeBulletAttachedEffect(b.kind);auto it=bullet_scripts->find(id);if(id&&it!=bullet_scripts->end())b.attached_effect.bind_entry(it->second.stream,it->second.len,bullet_scripts);}
        if(b.attached_effect.stream)b.attached_effect.tick();
        if(b.animation.stream)b.animation.tick();
        if(b.native_timer>0)--b.native_timer;
        ++b.age;
        if(!b.changes.preserves_bounds()&&(b.x < -220||b.x>220||b.y < -80||b.y>520))b.state=4;
    }
    std::vector<InfiniteLaser> reflected_lasers;
    for(auto&laser:lasers){laser.tick(player_x,player_y,&reflected_lasers);
        for(int n=0;n<laser.change_sounds&&laser.source.end_sound>=0;++n)sound_events.push_back(laser.source.end_sound);laser.change_sounds=0;
    }
    for(auto&laser:reflected_lasers)if(lasers.size()<512){laser.id=next_id++;lasers.push_back(std::move(laser));}
    lasers.erase(std::remove_if(lasers.begin(),lasers.end(),[](const auto&l){return !l.active;}),lasers.end());
    // 0x545120..0x5451a6: the display has its own countdown after a chain ends.
    if(combo_display_timer>0&&--combo_display_timer==0)combo_display=0;
    // Player 0x5450e0: the chain clock is independent of enemy aura slowdown.
    // Pausing stops world ticks; a chain expires when its positive timer reaches 0.
    if(combo_timer>0&&--combo_timer==0)combo=0;
    // index-based iteration: step() may spawn new enemies (reallocation-safe)
    for(size_t i=0;i<enemies.size();++i){
        if(!enemies[i].active)continue;
        // 0x509030: spirits enter the registered Disable script once;
        // only ordinary enemies use the half-speed clock below.
        auto&spirit=enemies[i];
        if((spirit.status_flags&0x700000u)&&!(spirit.status_flags&0x80000u)&&
           spirit.vm&&!spirit.callbacks[0].empty()&&charge_region.contains(spirit.x,spirit.y,spirit.hitbox_w*.5f)){
            const auto callback=spirit.callbacks[0];
            if(spirit.vm->start(callback.c_str())){spirit.status_flags|=0x80000u;spirit.spirit_aura_exit=true;}
        }
        // 0x509030 applies 0.5 to ordinary enemy movement and script clocks.
        // Invisible wave controllers and bosses do not enter this path.
        auto& timed=enemies[i];
        const bool slowed=!(timed.status_flags&0x700000u)&&timed.sprite>=0&&!(timed.flags&0x20)&&charge_region.contains(timed.x,timed.y,timed.hitbox_w*.5f);
        timed.slow_clock+=slowed?.5f:1.f;
        if(timed.slow_clock<1)continue;
        timed.slow_clock-=1;
        if(timed.pending_phase.empty()){
            auto trigger=timed.phases.check(timed.life,true);
            if(trigger&&!trigger.sub.empty()){timed.life=trigger.hp;timed.phases.consume(trigger);
                timed.status_flags|=0x100u;timed.pending_phase=trigger.sub;}
        }
        if(!timed.pending_phase.empty()&&timed.vm){
            std::string next=std::move(timed.pending_phase);timed.pending_phase.clear();timed.vm->start(next.c_str());
        }
        ++timed.phases.clock;
        // step() can grow enemies and invalidate every Enemy reference.
        // The separately allocated VM remains stable across vector reallocations.
        EclVm* vm=enemies[i].vm.get();
        if(enemies[i].script_initialized_this_tick)enemies[i].script_initialized_this_tick=false;
        else {
            // EXE 0x4fbea0 is dispatched before the enemy ECL step.  Suika's
            // ExAttackFunc reads the updated Enemy+0xac angle while it builds
            // its shooter descriptor; running this callback after vm->step()
            // leaves the descriptor at its initial angle (0 = right).
            enemies[i].apply_suika_steering(opponent_controls,opponent_x,opponent_y);
            if(vm)vm->step();
        }
        // Integrate field-local movement after executing this frame's ECL.
        Enemy& e=enemies[i];
        if(!e.death_initialized){
            if(e.death_script==0){e.death_script=nativeEnemyDeathScript(e.animations[0].file,e.sprite);e.death_file=1;}
            e.death_initialized=true;
        }
        if(e.active){
            auto advance=[](Enemy::Motion&m,float&v){
                if(m.duration<=0)return;
                ++m.elapsed;
                // ECL445 mode 7 (pl04 ExAttack2): the operand is an increment,
                // not a target speed. Native interpolators retain the accumulated
                // value at expiry, without adding on the terminal frame.
                if(m.mode==7){if(m.elapsed<m.duration)v+=m.to;else m.duration=0;return;}
                float t=nativeEase(m.mode,float(m.elapsed)/m.duration);
                v=m.from+(m.to-m.from)*t;
                if(m.elapsed>=m.duration){v=m.to;m.duration=0;}
            };
            const float previous_combined_x=e.x;
            const float previous_combined_y=e.y;
            if(e.relative_ellipse){e.x-=e.relative_x;e.y-=e.relative_y;}
            advance(e.scale_x,e.sx);advance(e.scale_y,e.sy);
            advance(e.turn,e.angle);advance(e.accelerate,e.speed);
            if(e.move_x.duration>0||e.move_y.duration>0){
                const float x=e.x,y=e.y;advance(e.move_x,e.x);advance(e.move_y,e.y);
                e.vx=e.x-x;e.vy=e.y-y;
                if(e.move_x.duration==0&&e.move_y.duration==0)e.vx=e.vy=0;
            }else if(e.orbit){
                // Native 0x4d4dc0 mode 2 advances radius, then phase;
                // 0x4d4a00 resolves position relative to the saved center.
                e.radius+=e.radial_speed;
                e.angle=std::remainder(e.angle+e.speed,6.28318530717958647692f);
                float u=std::cos(e.angle)*e.radius,v=std::sin(e.angle)*e.radius;
                if(e.ellipse){u*=e.ellipse_ratio;float x=u;u=x*std::cos(e.ellipse_angle)-v*std::sin(e.ellipse_angle);v=x*std::sin(e.ellipse_angle)+v*std::cos(e.ellipse_angle);}
                const float x=e.orbit_x+u;
                const float y=e.orbit_y+v;
                e.vx=x-e.x;e.vy=y-e.y;e.x=x;e.y=y;
            }else{
                // EXE callback table 0x5994f8 entry 3 -> 0x4fbea0
                // rewrites Enemy+0xac (angle).  The following native motion
                // step consumes that angle even for the small-Suika actor;
                // its ECL record does not set the generic polar bit.
                if(e.polar||e.native_update_callback==3){e.vx=std::cos(e.angle)*e.speed;e.vy=std::sin(e.angle)*e.speed;}
                e.x+=e.vx;e.y+=e.vy;
            }
            if(e.relative_ellipse){
                e.relative_phase=std::remainder(e.relative_phase+e.relative_omega,6.28318530718f);
                e.relative_radius+=e.relative_radial;
                const auto offset=enemyEllipseOffset(e.relative_phase,e.relative_rotation,e.relative_radius,e.relative_ratio);
                e.relative_x=e.relative_center_x+offset[0];
                e.relative_y=e.relative_center_y+offset[1];
                e.x+=e.relative_x;e.y+=e.relative_y;
            }
            // 0x50c540 writes +80 from absolute+relative minus previous
            // combined position, before clamping. Do not overwrite base vx.
            const float animation_dx=e.x-previous_combined_x;
            // pl14b ANM13/15: after the 60/40-frame warning, ANM424(1)
            // follows motion. Retail 0x508af8..0x508b19 writes this angle
            // to BOTH the animation and EnemyInf+0x130 (contact rectangle).
            // Keep this in simulation, independent of rendering/rollback skips.
            const auto& body=e.animations[0];
            if(body.file>=2&&body.file-2<int(e.animation_files.size())&&
               e.animation_files[body.file-2]=="pl14b.anm"&&
               (body.script==13||body.script==15)&&!(e.flags&2))
                e.contact_angle=std::atan2(e.y-previous_combined_y,animation_dx);
            // 0x50c540: ECL504 bounds clamp the combined enemy position.
            if(e.status_flags&2u){
                e.x=std::clamp(e.x,e.bound_x-e.bound_w*.5f,e.bound_x+e.bound_w*.5f);
                e.y=std::clamp(e.y,e.bound_y-e.bound_h*.5f,e.bound_y+e.bound_h*.5f);
            }
            if(e.native_update_callback==2)strengthen_tsukasa_bullets(e);
            e.apply_hit_recoil(player_x,player_y);
            e.update_direction_animation(animation_dx);
            ++e.age;
            // Retail's enemy-list pass (0x42ec80..0x42edb0) retires a
            // converted spirit as soon as it has left the playfield.  The
            // previous web path applied the ordinary enemy grace period
            // (age>90 and y<-128), so an aura-held spirit could travel well
            // past the top edge and remain registered.  Converted spirits
            // use the field edge directly; ordinary enemies retain their
            // existing grace-period cull.
            // 0x10000000 is the retail pure-spirit marker; it is checked
            // alongside the 0x700000 animal-kind mask by 0x42ecaa before
            // the spirit object is retired.  Pure spirits do not carry the
            // aura-converted 0x80000 bit, so testing that bit alone leaves
            // them registered after their upward exit.
            const bool spirit_like=e.spirit_aura_exit||((e.status_flags&(0x10000000u|0x700000u))!=0);
            if(spirit_like){
                if(e.x < -240||e.x>240||e.y < -32||e.y>480)e.active=false;
                // The retail object manager retires a spirit when its
                // registered disable/exit ECL returns.  The web VM used to
                // leave the host enemy alive after that return, which is why
                // spirits that stop just above the field edge remained
                // visible forever.
                if(e.vm&&e.vm->finished()&&(e.status_flags&(0x80000u|0x10000000u)))e.active=false;
            }else if(e.sprite>=0&&e.age>90&&(e.x < -240||e.x>240||e.y < -128||e.y>520))e.active=false;
        }
    }
    // integrate spirit velocity toward the player
    for(auto& s:spirits){if(s.active){s.x+=s.vx;s.y+=s.vy;}}
    // Native destruction unregisters these objects. Retaining dead enemies kept
    // their VM stacks/children forever, and copied them into every rollback slot.
    enemies.erase(std::remove_if(enemies.begin(),enemies.end(),[](const auto&e){return !e.active;}),enemies.end());
    spirits.erase(std::remove_if(spirits.begin(),spirits.end(),[](const auto&s){return !s.active;}),spirits.end());
}
}
