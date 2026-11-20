#include "PolygonContact.hpp"
#include "CardEffects.hpp"
#include "BulletVisual.hpp"
#include "BulletPattern.hpp"
#include "PlayerDamage.hpp"
#include "ShotCollision.hpp"
#include "EnemyDeath.hpp"
#include "EnemyOrbit.hpp"
#include "BulletSprites.hpp"
#include "GameWorld.hpp"
#include "EclVm.hpp"
#include <cmath>
#include <cstdio>
#include <algorithm>
namespace th19 {
extern bool traceOpeningRng;
static void traceCancelSource(const char* source,int frame,int script,u32 id,float x,float y){
    if(traceOpeningRng)std::printf("ENEMYDBG {\"event\":\"cancel-source\",\"source\":\"%s\",\"frame\":%d,\"script\":%d,\"id\":%u,\"position\":[%.9g,%.9g],\"seed\":%u}\n",source,frame,script,id,x,y,AnmVm::random_checkpoint());
}
void Enemy::Motion::advance(float& value,float step){
    if(duration<=0)return;
    elapsed+=step;
    // ECL445 mode 7 adds its increment until the terminal frame.
    if(mode==7){
        if(elapsed<duration)value+=to*step;
        else duration=0;
        return;
    }
    const float progress=nativeEase(mode,elapsed/duration);
    value=from+(to-from)*progress;
    if(elapsed>=duration){value=to;duration=0;}
}
namespace {
float snapEnemyCoordinate(float value){return std::floor(value*100.f)/100.f;}
void hold_finished_attack_controller(Enemy& enemy){
    // The native CPU can still see the controller while its two birth guards clear.
    enemy.active=true;
    enemy.spawn_hold=2;
    enemy.attack_entry=Enemy::AttackEntry::RetireController;
}
}
Enemy& GameWorld::spawn_enemy(const std::string& sub_name,float x,float y,i32 life,i32 item,i32 score,
                             const std::vector<const EclProgram*>* source_programs){
    const auto& scripts=source_programs?*source_programs:programs_;
    enemies.emplace_back();
    Enemy& e=enemies.back();
    // Native constructor 0x50ac97..0x50acbb initializes both boxes to 24.
    // ExAttack2 in pl04 intentionally leaves the contact box at this default.
    e.hitbox_w=e.hitbox_h=e.contact_w=e.contact_h=24.f;
    // 0x50bf6f initializes the timer before the first ECL update.
    e.invulnerability_time=2.f;
    // Includes load before the character root: wave*.ecl declares only slot 2,
    // while every plXX.ecl declares slots 2 and 3. Preserve declaration indices
    // and extend the table; stopping at the first declaration loses plXXb.anm.
    for(auto*p:scripts)for(size_t i=e.animation_files.size();i<p->anims.size();++i)
        e.animation_files.push_back(p->anims[i]);
    e.id=next_id++;e.set_absolute_position(x,y);e.previous_x=x;e.previous_y=y;e.life=life;e.item=item;e.score=score;
    extern bool traceOpeningRng;
    if(traceOpeningRng)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"spawn\",\"id\":%u,\"position\":[%.9g,%.9g]}\n",battle_frame,e.id,x,y);
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
    // Opponent attacks are rejected once this VS field has entered the result phase.
    if(reward_mode==2&&result_pause_id)return false;
    const char* name=tier==3?"ExtraAttack":tier==4?"BossAttack":nullptr;
    if(!name)return false;
    bool found=false;for(const auto*p:source)for(const auto&s:p->subs)if(s.name==name)found=true;
    if(!found)return false;
    const size_t first=enemies.size();
    auto& attack=spawn_enemy(name,x,y,200,0,0,&source);
    if(reward_mode==2){
        // 0x50c012 runs the initial ECL before the enemy manager visits the
        // attack and its children. That visit only clears the update flag.
        attack.vm->step();
        for(size_t i=first;i<enemies.size();++i){
            auto& actor=enemies[i];
            actor.script_initialized_this_tick=false;
            actor.spawn_hold=1;
            if(i==first&&!actor.active){
                hold_finished_attack_controller(actor);
            }else if(actor.active)actor.attack_entry=Enemy::AttackEntry::StepScript;
        }
    }
    return true;
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
    if(reward_mode==2&&character==1){
        // 0x53cf10 replaces the provisional target before 0x53d280 builds the flight.
        f.x=AnmVm::rand_signed_unit(120.f,true);
        f.y=448.f;
        AnmVm::rand_integer(true);
        f.duration=ReflectedBulletFlight::flightDuration(AnmVm::random_checkpoint(true));
        f.tangent_start=AnmVm::rand_signed_unit(100.f,true);
        f.tangent_end=AnmVm::rand_signed_unit(100.f,true);
    }else{
        f.duration=(extra_rng.next()&65535)%((character<=1||character==4||character==5||character==6)?100:character==2?80:character==3?40:60)+((character<=1||character==4||character==5||character==6)?60:character==2?80:character==3?50:70);
        f.tangent_start=extra_rng.signedUnit()*100;f.tangent_end=extra_rng.signedUnit()*100;
    }
    f.life=character==10?2400:(character>=11)?4000:200;
    f.script=character==0?37:character==1?63:character==2?61:character==4?17:character==8?16:character==14?17:character==16?15:character==18?12:character==12?21:character==17?12:character==6?15:character==5?11:character==3?14:character==7?31:character==9?13:character==10?14:character==13?12:18;
    char file[32];std::snprintf(file,sizeof file,"pl%02db.anm",character);f.file=character<=2?"bullet.anm":file;
    f.sub=sub;f.programs=source;extra_flights.push_back(std::move(f));return true;
}
// 5402e0 -> 540620 -> 41af40/41b2e0. Reservations survive arrival
// for another 240 frames (50d640 queue, 50df40 decrement).
int GameWorld::send_native_extra_credit(GameWorld& target,const std::vector<const EclProgram*>& source,int character,int versus_frame,bool result_phase){
    if(character<0||character>=19)return 0;
    static constexpr int costs[]={100,90,90,80},ranCosts[]={75,65,65,55},aunnCosts[]={50,45,45,40},animalCosts[]={80,70,70,60};
    const int cost=(character==3?ranCosts:character==4?aunnCosts:(character==9||character==14)?animalCosts:costs)[std::clamp(difficulty,0,3)];int sent=0;
    // 0x50e08c raises the VS limit after dispatch at 60, 120 and 180 seconds.
    const size_t capacity=reward_mode!=2?20:versus_frame<=3600?32:versus_frame<=7200?64:versus_frame<=10800?128:999;
    if(traceOpeningRng&&result_phase)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"result-ex-budget\",\"credit\":%d,\"reserved\":%u,\"capacity\":%u}\n",battle_frame,pending_native_attack_credit,unsigned(native_extra_reservations.size()),unsigned(capacity));
    if(traceOpeningRng)std::printf("ENEMYDBG {\"event\":\"extra-budget\",\"frame\":%d,\"credit\":%d,\"reserved\":%d,\"capacity\":%zu,\"sequence\":%u}\n",battle_frame,pending_native_attack_credit,native_extra_reserved_count,capacity,native_extra_sequence);
    while(pending_native_attack_credit>=cost){
        pending_native_attack_credit-=cost;
        if(native_extra_reserved_count>=int(capacity))break;
        ++native_extra_reserved_count;
        if(reward_mode==2&&character==1){
            // 0x53cbe0 chooses a provisional Y then X before virtual dispatch.
            const float ty=AnmVm::rand_unit(true)*60.f;
            const float tx=AnmVm::rand_signed_unit(160.f,true);
            // 0x53cf2c rejects the result flag after provisional sampling.
            if(result_phase){
                // 0x53cc7a reserves the slot before virtual dispatch rejects
                // the result flag. No flight is queued to release that slot.
                // Result rejection keeps its reserved count without a release timer.
                ++native_extra_sequence;++sent;continue;
            }
            if(!target.queue_extra_attack(character,tx,ty,0,source))break;
        }else{
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
        }
        if(character==14)target.sound_events.push_back(44);
        auto&flight=target.extra_flights.back();flight.source_x=attack_credit_x;flight.source_y=attack_credit_y;
        if(character!=1)native_extra_reservations.push_back(flight.duration+240);
        ++native_extra_sequence;++sent;
    }
    for(auto&t:native_extra_reservations)if(t>0&&--t==0)--native_extra_reserved_count;
    native_extra_reservations.erase(std::remove(native_extra_reservations.begin(),native_extra_reservations.end(),0),native_extra_reservations.end());
    return sent;
}
bool GameWorld::receive_spirit(const SpiritArrival&a,float x,float y,bool current_aura_held){
    if(!a.sub)return false;
    bool found=false;for(const auto*p:programs_)for(const auto&s:p->subs)if(s.name==a.sub)found=true;
    if(!found)return false;
    auto&e=spawn_enemy(a.sub,x,y,a.life,0,0);
    e.ecl_ints=a.variables;
    // Native VS arrivals start on the enemy coordinate grid. The motion
    // pauses on the next update while the script keeps its battle timing.
    if(reward_mode==2){
        e.set_absolute_position(snapEnemyCoordinate(e.x),snapEnemyCoordinate(e.y));
        e.arrival_motion=Enemy::ArrivalMotion::FirstUpdate;
        // 0x50c012 starts the receiving actor's ECL in the flight callback.
        const size_t index=enemies.size()-1;
        step_enemy_script(e);
        auto& arrived=enemies[index];
        if(current_aura_held&&(arrived.status_flags&0x700000u)==0x200000u&&!(arrived.status_flags&0x80000u)&&charge_region.contains(arrived.x,arrived.y,arrived.hitbox_w*.5f))
            // The native registration guard delays Aura conversion, not the
            // birth displacement. Keep its phase separate from spawn_hold.
            arrived.arrival_aura_registered=true;
        extern bool traceOpeningRng;

    }
    return true;
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
    int n=0;for(const auto&e:enemies)if((e.active||(!timeout_result_clear&&(e.status_flags&0x200u)))&&(e.status_flags&0x1000000u))++n;return n;
}
bool GameWorld::fire_bullets(const LaserShooter&s,float ox,float oy){
    for(const auto&c:s.changes)if(!supportedBulletChange(c))return false;
    extern bool traceOpeningRng;
    if(traceOpeningRng)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"fire-rng\",\"position\":[%.9g,%.9g],\"seed\":%u}\n",battle_frame,ox,oy,AnmVm::random_checkpoint(true));
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
        extern bool traceOpeningRng;
        if(traceOpeningRng)std::printf("BULLETDBG {\"frame\":%d,\"event\":\"pattern\",\"mode\":%d,\"counts\":[%d,%d],\"index\":[%d,%d],\"kind\":%d,\"color\":%d,\"position\":[%.9g,%.9g],\"angles\":[%.9g,%.9g,%.9g,%.9g],\"speed\":%.9g}\n",battle_frame,s.aim_mode,s.count1,s.count2,col,row,s.type,s.color,x,y,s.angle,s.angle2,aim,p.angle,p.speed);
        // A full native pool refuses the new bullet; existing live bullets stay.
        // Keep the emitter's random draws in order even when its pool is full.
        if(bullets.size()>=bullet_capacity){++dropped_bullets;continue;}
        // 0x4f4137 and 0x4f42b7 multiply the x87 sine/cosine before
        // rounding the muzzle offset and launch velocity to float.
        const double cosine=std::cos(double(p.angle)), sine=std::sin(double(p.angle));
        auto&b=spawn_bullet();b.x=x+float(cosine*s.muzzle_radius);b.y=y+float(sine*s.muzzle_radius);
        b.kind=s.type;b.color=s.color;b.sprite=sprite;b.scripted=true;
        // 0x4f42b3 reads the unwrapped stack angle retained at 0x4f3ba6.
        b.vx=float(std::cos(double(p.launch_angle))*p.speed);
        b.vy=float(std::sin(double(p.launch_angle))*p.speed);
        b.launch_speed=p.speed;
        b.cancel_script=nativeBulletCancelScript(b.kind,b.color);b.visual_heading=p.angle;b.changes.records=s.changes;b.change_sound=s.end_sound;
        if(bullet_scripts){auto it=bullet_scripts->find(nativeBulletAnimation(s.type));if(it!=bullet_scripts->end()){
            b.animation.bind_entry(it->second.stream,it->second.len,bullet_scripts);
            if(b.contact_interrupt_pending){b.animation.interrupt(1);b.contact_interrupt_pending=false;}
            b.animation.tick();
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
            // A recycled VS slot carrying 0x400 skips the first animation
            // update and half-step. The manager clears the bit that frame.
            const bool skip_birth_update=reward_mode==2&&b.motion_advanced;
            if(!skip_birth_update)b.animation.tick();
            b.x-=4*b.vx;b.y-=4*b.vy;b.state=2;b.changes.animation_interrupt=-1;
            if(reward_mode==2){
                if(!skip_birth_update){b.x+=.5f*b.vx;b.y+=.5f*b.vy;}
                b.motion_advanced=false;
            }
        }
        if(b.changes.native_timer>=0){b.native_timer=b.changes.native_timer;b.changes.native_timer=-1;}
        for(int i=0;i<sounds&&b.change_sound>=0;++i)sound_events.push_back(b.change_sound);
    }
    if((s.shot_flags&0x20)&&s.sound>=0)sound_events.push_back(s.sound);
    return true;
}
Bullet& GameWorld::spawn_bullet(){
    auto& bullet=bullets.emplace_back();
    bullet.id=next_id++;
    // 0x4f45c1 gives newly registered bullets five updates before bounds checks.
    if(reward_mode==2)bullet.native_timer=5;
    if(!reusable_bullet_motion_.empty()){
        bullet.animation.ivar[0]=reusable_bullet_release_.back();
        reusable_bullet_release_.pop_back();
        bullet.contact_interrupt_pending=reusable_bullet_interrupt_.back();
        reusable_bullet_interrupt_.pop_back();
        bullet.motion_advanced=reusable_bullet_motion_.back();
        reusable_bullet_motion_.pop_back();
        bullet.native_slot=reusable_bullet_slots_.back();
        reusable_bullet_slots_.pop_back();
    }else if(reward_mode==2){
        bullet.native_slot=--unused_bullet_slots_;
    }
    extern bool traceOpeningRng;
    return bullet;
}
void GameWorld::retire_bullet(Bullet& bullet){
    if(bullet.state==4)return;
    extern bool traceOpeningRng;
    if(traceOpeningRng)std::printf("BULLETDBG {\"frame\":%d,\"event\":\"retire\",\"id\":%u,\"type\":[%d,%d],\"state\":%d,\"position\":[%.9g,%.9g],\"age\":%d,\"flags\":%u,\"cursor\":%d}\n",battle_frame,bullet.id,bullet.kind,bullet.color,bullet.state,bullet.x,bullet.y,bullet.age,bullet.flags,bullet.changes.cursor);
    // 0x4f50fb preserves the movement bit when pushing onto the free list.
    if(reward_mode==2){
        reusable_bullet_release_.push_back(bullet.animation.ivar[0]);
        reusable_bullet_interrupt_.push_back(bullet.contact_interrupt_pending);
        reusable_bullet_motion_.push_back(bullet.motion_advanced);
        reusable_bullet_slots_.push_back(bullet.native_slot);
    }
    bullet.state=4;
}
void GameWorld::discard_retired_bullets(){
    bullets.erase(std::remove_if(bullets.begin(),bullets.end(),[](const auto& b){return b.state==4;}),bullets.end());
}
void GameWorld::clear_bullets(){
    bullets.clear();
    reusable_bullet_release_.clear();
    reusable_bullet_interrupt_.clear();
    reusable_bullet_motion_.clear();
    reusable_bullet_slots_.clear();
    unused_bullet_slots_=bullet_capacity;
}
Spirit& GameWorld::spawn_spirit(float x,float y){spirits.emplace_back();spirits.back().id=next_id++;spirits.back().x=x;spirits.back().y=y;return spirits.back();}
void GameWorld::launch_animal_spirit(const Enemy& enemy){
    const SpiritRequest request{enemy.x,enemy.y,deathSpiritKind(enemy.x,enemy.y,player_x,player_y),enemy.ecl_ints[2]};
    extern bool traceOpeningRng;
    if(traceOpeningRng)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"animal-launch\",\"id\":%u,\"kind\":%d,\"position\":[%.9g,%.9g],\"player\":[%.9g,%.9g],\"seed\":%u}\n",battle_frame,enemy.id,request.kind,enemy.x,enemy.y,player_x,player_y,AnmVm::random_checkpoint(true));
    if(reward_mode!=2){
        spirit_requests.push_back(request);
        return;
    }
    // 0x413d10 draws the target before 0x51bb00 checks the sender's limit.
    const float targetX=AnmVm::rand_signed_unit(148.f,true);
    const float targetY=AnmVm::rand_unit(true)*64.f;
    // The dying spirit is still registered during the native death callback.
    const int dyingSpirit=!enemy.active&&(enemy.status_flags&0x1000000u)?1:0;
    if(counted_spirits()+dyingSpirit>=spirit_flight_limit){
        pure_spirit_meter.add_bullets(100,difficulty,request.x,request.y);
        ++spirit_launch_overflows;
        return;
    }
    AnmVm::rand_integer(true);
    const unsigned duration=AnmVm::random_checkpoint(true);
    float first=0,second=0;
    unsigned sign=0;
    if(request.kind==2||request.kind==3)first=AnmVm::rand_signed_unit(1.f,true);
    if(request.kind==3){
        second=AnmVm::rand_signed_unit(1.f,true);
        sign=AnmVm::rand_integer(true);
    }
    SpiritLaunch launch;
    launch.transit=prepareSpiritTransit(request,duration,targetX,targetY,first,second,sign,opponent_x);
    launch.endTangentX=AnmVm::rand_signed_unit(100.f,true);
    launch.startTangentX=AnmVm::rand_signed_unit(100.f,true);
    spirit_launches.push_back(launch);
}
bool GameWorld::damage_enemy(Enemy& e,int damage,bool forced,u32 damage_frame){
    // 0x509980: bit 0/5 excludes damage; bit 4 excludes HP reduction;
    // bit 7 suppresses automatic death. Invisible controllers are never targets.
    if(!e.active||e.sprite<0||(e.flags&0x21)||(!forced&&damage<=0))return false;
    if(e.death_pending||!e.pending_phase.empty())return false;
    if(!forced){if(card_near_damage&&(player_x-e.x)*(player_x-e.x)+(player_y-e.y)*(player_y-e.y)<=2304.f){
            if(e.card_damage_frame!=damage_frame){e.card_damage_frame=damage_frame;e.card_damage_sum=0;}
            const int previous=e.card_damage_sum;e.card_damage_sum+=damage;
            damage+=e.card_damage_sum/2-previous/2;
        }
        // 0x520864..0x520875 caps the aggregate damage for one target,
        // after the proximity-card bonus. Overlapping flame regions share it.
        if(player_damage_cap>0){
            if(e.damage_budget_frame!=damage_frame){e.damage_budget_frame=damage_frame;e.damage_budget_used=0;}
            damage=std::min(damage,std::max(0,player_damage_cap-e.damage_budget_used));
            e.damage_budget_used+=damage;
            if(damage<=0)return false;
        }
        // 0x509cf7 reduces outgoing damage to one fifth while dead or respawning.
        if(player_damage_reduced)damage/=5;
        if(damage<=0)return false;
        e.hit_pending=true;
        if(e.native_update_callback==1)e.recoil_pending=true;
        // 0x509e16: keep hit feedback while protecting HP, before phase/death checks.
        if((e.flags&0x10)||int(e.invulnerability_time)>0)return false;
        if(e.spell_damage_scaled){
            // 0x509980: HP+0xc stores raw damage; HP+0x10 is the native anchor.
            e.life_reset_c=i32(u32(e.life_reset_c)-u32(damage));
            e.life=i32((int64_t(e.life_reset_c)-int64_t(e.life_scale_anchor)*7)/7+e.life_scale_anchor);
        }else e.life-=damage;
        extern bool traceOpeningRng;
        if(traceOpeningRng)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"damage\",\"id\":%u,\"position\":[%.9g,%.9g],\"damage\":%d,\"hp\":%d}\n",battle_frame,e.id,e.x,e.y,damage,e.life);
        auto trigger=e.phases.check(e.life,false);
        if(trigger&&!trigger.sub.empty()){
            e.life=trigger.hp;e.phases.consume(trigger);e.status_flags&=~0x100u;e.pending_phase=trigger.sub;
            // Let the world dispatch the new phase once damage iteration is safe.
            return false;
        }
        if(e.life>0||(e.flags&0x80))return false;
    }
    return dispatch_enemy_death(e);
}
bool GameWorld::dispatch_enemy_death(Enemy& e,bool opponent_animation_queue){
    if(!e.active||e.death_pending||!e.pending_phase.empty())return false;
    // 0x50c4ca: the death callback runs after death effects/rewards. Keep the
    // host until the damage pass ends, so an ECL spawn cannot invalidate a
    // caller's enemy iteration. Only native flag 0x08000000 preserves it.
    e.death_pending=!e.death_callback.empty()&&bool(e.vm);
    e.active=e.death_pending;++defeated;earned_score+=std::max(0,e.score);
    // Native death dispatch preserves ECL323's file/script override.
    const int deathScript=e.death_script?e.death_script:nativeEnemyDeathScript(e.animations[0].file,e.sprite);
    const float heading=e.vx*e.vx+e.vy*e.vy<.2f*.2f?-1.57079632679f:std::atan2(e.vy,e.vx);
    if(traceOpeningRng)std::printf("ENEMYDBG {\"event\":\"death-effect-request\",\"frame\":%d,\"host\":%u,\"script\":%d}\n",battle_frame,e.id,deathScript);
    EnemyDeath death{e.x,e.y,heading,deathScript,3+int(e.id&1),e.death_file};
    death.opponent_animation_queue=reward_mode==2&&e.death_file==1&&opponent_animation_queue;
    const auto* deathScripts=e.death_file==1?effect_scripts:e.death_file==0?bullet_scripts:nullptr;
    if(reward_mode==2&&deathScripts){
        const auto script=deathScripts->find(deathScript);
        if(script!=deathScripts->end()){
            death.animation.bind_entry(script->second.stream,script->second.len,deathScripts);
            death.animation.rot_z=heading;death.animation.initialize_constructor();
        }
    }
    deaths.push_back(std::move(death));
    if(e.item>0)spawn_pickup(e.item,e.x,e.y,0,-2.2f);
    e.item=0; // 0x50c3c5: an item is dropped once, even if the host revives.
    // 0x50c6d0 selects a scatter heading before checking the extra-drop count.
    extern bool traceOpeningRng;
    if(traceOpeningRng)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"death\",\"id\":%u,\"position\":[%.9g,%.9g],\"status\":%u,\"seed\":%u}\n",battle_frame,e.id,e.x,e.y,e.status_flags,AnmVm::random_checkpoint(true));
    AnmVm::rand_signed_unit(3.14159265359f,true);
    // 0x50c1f0 -> 0x549780: excluded actors do not extend/count the chain.
    // ECL 1032 supplies the extension (default 20), capped at 60 frames.
    if(!(e.status_flags&0x20000000u)){
        // 0x50c1f0 -> 0x51d4e0/0x51cc70: independent expanding damage region.
        const int kind=(e.status_flags>>20)&7;
        combo_attack.on_death(kind, combo);
        // 0x51d4e0 case0 checks the pre-increment combo count; case3 (kind5)
        // contributes 50 independently. Other death rewards need mode parameters.
        if(reward_mode>=0){
            // The VS reward dispatcher observes the current battle-manager tick.
            const int reward_frame=battle_frame+(reward_mode==2?1:0);
            const auto r=nativeDeathRewards(kind,e.ecl_ints[2],combo,reward_frame,reward_mode,reward_stage);
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
        if(kind>=1&&kind<=4)launch_animal_spirit(e);
        if(kind<=6){DeathChain c;c.area_x=c.x=e.x;c.area_y=c.y=e.y;c.target=e.spirit_type;
            // A birth-tick ECL525 clear precedes the first body publication.
            // 0x51cc70 keeps the visible center, while the damage descriptor
            // still uses the position from the guarded registration visit.
            if(reward_mode==2&&e.age==1&&e.body_half_width==0&&e.body_half_height==0){
                c.area_x=e.previous_x;c.area_y=e.previous_y;
            }
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
    if(combo>5)for(int n=0;n<((e.status_flags&0x8000000u)?1:std::min(3,combo/12+1));++n){
        scatter_charge_pickup(e.x,e.y);
    }
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
// The native enemy visit advances protection after damage. VS feedback may
// run before or after deferred SHT contacts, so it cannot own this clock.
void GameWorld::advance_enemy_protection(Enemy& e){
    if(!e.active)return;
        // 0x5089d0 calls damage/feedback first, then decrements positive timer.
        // The intermediate spawn tick exits at 0x5089e3 before the timer update.
        // A dispatched attack already ran the constructor update at 0x50c012.
        if(!(e.arrival_motion==Enemy::ArrivalMotion::Hold&&int(e.invulnerability_time)==1&&
             (e.status_flags&0x780000u)==0x180000u)&&
           ((e.spawn_hold!=1&&!(reward_mode==2&&e.spawn_hold==2&&int(e.invulnerability_time)==1))||
            e.attack_entry==Enemy::AttackEntry::StepScript)&&
           (int(e.invulnerability_time)>0||int(e.contact_protection_time)>0)){
            const bool slowed=!(e.status_flags&0x700000u)&&e.sprite>=0&&!(e.flags&0x20)&&charge_region.contains(e.x,e.y,e.hitbox_w*.5f);
            // Native protection timers bind to the global clock; VS Aura
            // halves displacement without halving these timers.
            const float step=slowed&&reward_mode!=2?.5f:1.f;
            if(int(e.invulnerability_time)>0)e.invulnerability_time-=step;
            if(int(e.contact_protection_time)>0)e.contact_protection_time-=step;
        }
}
// 0x509980 tail: evaluate once after all damage, never once per projectile.
void GameWorld::finish_hit_feedback(u32 mode){
    finish_enemy_deaths();
    for(auto&e:enemies){
        const bool hit=e.hit_pending;e.hit_pending=false;
        if(!e.active)continue;
        if(reward_mode!=2)advance_enemy_protection(e);
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
void GameWorld::record_bullet_cancel(Bullet& bullet,float x,float y,bool initialize,bool play_sound){
    if(bullet.cancel_script<0)bullet.cancel_script=nativeBulletCancelScript(bullet.kind,bullet.color);
    if(bullet.flags&0x200)return;
    traceCancelSource("record",battle_frame,bullet.cancel_script,bullet.id,x,y);
    BulletCancel event{x,y,bullet.cancel_script,{},next_visual_order++};
    if(initialize&&bullet_scripts){
        const auto script=bullet_scripts->find(bullet.cancel_script);
        if(script!=bullet_scripts->end()){
            event.animation.bind_entry(script->second.stream,script->second.len,bullet_scripts);
            event.animation.clock_after_execution=true;
            event.animation.tick();
        }
    }
    bullet_cancels.push_back(std::move(event));
    if(play_sound)sound_events.push_back(71);
}
void GameWorld::cancel_contact_bullet(Bullet& bullet){
    // 0x4f5379 constructs the separate cancellation root on physical contact.
    bullet.state=Bullet::Canceling;
    bullet.contact_cancel=true;
    bullet.contact_interrupt_pending=true;
    extern bool traceOpeningRng;
    bullet.animation.interrupt(1);
    record_bullet_cancel(bullet,bullet.x,bullet.y,true,false);
}
void GameWorld::cancel_bullet(Bullet& bullet,bool inlineAnimation){
    extern bool traceOpeningRng;
    if(traceOpeningRng)std::printf("BULLETDBG {\"frame\":%d,\"event\":\"cancel\",\"id\":%u,\"type\":[%d,%d],\"state\":%d,\"position\":[%.9g,%.9g],\"age\":%d,\"flags\":%u,\"cursor\":%d}\n",battle_frame,bullet.id,bullet.kind,bullet.color,bullet.state,bullet.x,bullet.y,bullet.age,bullet.flags,bullet.changes.cursor);
    if(inlineAnimation&&bullet.animation.stream){
        bullet.animation.interrupt(1);
        bullet.animation.tick();
        bullet.state=Bullet::Canceling;
    }else retire_bullet(bullet);
    ++cancelled_bullets;
    record_bullet_cancel(bullet,bullet.x,bullet.y,inlineAnimation);
    if(inlineAnimation){
        bullet.x+=bullet.vx*.5f;
        bullet.y+=bullet.vy*.5f;
    }
}
int GameWorld::cancel_bullets_circle(float x,float y,float radius,int limit,bool respect_lock,bool preserve_new_counters){
    extern bool traceOpeningRng;
    if(traceOpeningRng)std::printf("BULLETDBG {\"frame\":%d,\"event\":\"circle\",\"position\":[%.9g,%.9g],\"radius\":%.9g,\"lock\":%d}\n",battle_frame,x,y,radius,respect_lock);
    // ECL 615/616 calls 0x4ee880, which keeps the slot alive during its cancel ANM.
    const bool scriptClear=reward_mode==2&&!respect_lock;
    int count=0;
    for(size_t i=0;i<bullets.size()&&count<limit;++i){
        auto& b=bullets[scriptClear?bullets.size()-1-i:i];
        if((b.state!=1&&b.state!=2)||(respect_lock&&b.cancel_lock!=0))continue;
        // The shared counter callback registers new arrivals after the respawn clear.
        if(preserve_new_counters&&b.reflected&&b.age==0)continue;
        // 0x4ee8c3 adds half the bullet type's contact size to the circle.
        const float width=scriptClear?nativeBulletContactRadius(b.kind):b.hit_width;
        const float dx=b.x-x,dy=b.y-y,r=radius+width*.5f;
        if(dx*dx+dy*dy>r*r)continue;
        cancel_bullet(b,scriptClear);
        ++count;
    }
    return count;
}
void GameWorld::cancel_all_bullets(){
    // 0x4edf90 cancels every occupied slot except contact-canceled state 3.
    // Script-canceled state 4 is interrupted again and restarts its lifetime.
    std::vector<Bullet*> occupied;
    for(auto& bullet:bullets)
        if(bullet.state!=4&&!bullet.contact_cancel)occupied.push_back(&bullet);
    if(reward_mode==2)std::sort(occupied.begin(),occupied.end(),[](const Bullet* a,const Bullet* b){return a->native_slot<b->native_slot;});
    for(auto* bullet:occupied)cancel_bullet(*bullet,reward_mode==2);
}
int GameWorld::cancel_bullets_shape(const std::function<bool(float,float,float)>& contact){
 int count=0;for(auto&b:bullets){
  if((b.state!=1&&b.state!=2)||b.cancel_lock!=0||!contact(b.x,b.y,b.hit_width))continue;
  retire_bullet(b);++count;++cancelled_bullets;
  if(b.cancel_script<0)b.cancel_script=nativeBulletCancelScript(b.kind,b.color);
  if(!(b.flags&0x200)){traceCancelSource("shape",battle_frame,b.cancel_script,b.id,b.x,b.y);bullet_cancels.push_back({b.x,b.y,b.cancel_script,{},next_visual_order++});sound_events.push_back(71);}
 }return count;
}
int GameWorld::cancel_bullets_polygon(float x,float y,float radius,float angle,int sides){
 int count=0;
 for(auto&b:bullets){
  if((b.state!=1&&b.state!=2)||b.cancel_lock!=0)continue;
  if(!nativePolygonContact(b.x-x,b.y-y,radius,angle,sides,b.hit_width))continue;
  retire_bullet(b);++count;++cancelled_bullets;
  if(b.cancel_script<0)b.cancel_script=nativeBulletCancelScript(b.kind,b.color);
  if(!(b.flags&0x200)){traceCancelSource("shape",battle_frame,b.cancel_script,b.id,b.x,b.y);bullet_cancels.push_back({b.x,b.y,b.cancel_script,{},next_visual_order++});sound_events.push_back(71);}
 }return count;
}
int GameWorld::cancel_bullets_rect(float x,float y,float width,float height,float angle){
    extern bool traceOpeningRng;
    if(traceOpeningRng)std::printf("BULLETDBG {\"frame\":%d,\"event\":\"rectangle\",\"position\":[%.9g,%.9g],\"size\":[%.9g,%.9g],\"angle\":%.9g}\n",battle_frame,x,y,width,height,angle);
    // 0x4ee0d0 checks both the rectangle and the field using the bullet radius.
    std::vector<Bullet*> targets;
    const bool versus=reward_mode==2;
    for(auto& b:bullets){
        if((b.state!=1&&b.state!=2)||b.cancel_lock!=0)continue;
        const float radius=versus?nativeBulletContactRadius(b.kind):b.hit_width;
        if(!nativeEnemyRectangleContact(b.x-x,b.y-y,angle,width,height,radius))continue;
        if(versus&&!nativeEnemyRectangleContact(b.x,b.y-224,0,296,448,radius))continue;
        targets.push_back(&b);
    }
    // 0x4ee203 advances the physical slot pointer by 0x1088.
    // Reused slots retain this order independently of bullet birth order.
    if(versus)std::sort(targets.begin(),targets.end(),[](const Bullet* a,const Bullet* b){return a->native_slot<b->native_slot;});
    for(auto* bullet:targets)cancel_bullet(*bullet,versus);
    return int(targets.size());
}
// Retail 0x54aae0 -> 0x4ee0d0 and 0x5303f0. No bullet-style whitelist.
void GameWorld::cancel_enoko_trap(float x,float y,float angle){
    cancel_bullets_shape([=](float px,float py,float radius){
        return nativeEnemyRectangleContact(px,py-224,0,296,448,radius) &&
               nativeEnemyRectangleContact(px-x,py-y,angle,24,56,radius);
    });
    cancel_lasers_rect(x,y,24,56,angle);
}
int GameWorld::cancel_lasers_rect(float x,float y,float width,float height,float angle,u32 laser_id){
    const float hw=width*.5f,hh=height*.5f;
    const auto point=[=](float px,float py){
        const float dx=px-x,dy=py-y,c=std::cos(angle),s=std::sin(angle);
        return std::abs(dx*c+dy*s)<=hw&&std::abs(dy*c-dx*s)<=hh;
    };
    // 52a970/52d350 -> 4d2c30: oriented rectangle against each
    // 16-unit sample rectangle (dimensions are the native step vector).
    const auto sample=[=](float px,float py,float a){
        const float c=std::cos(angle),s=std::sin(angle),bc=std::cos(a),bs=std::sin(a);
        const float hx=std::abs(8*bc),hy=std::abs(8*bs),dx=px-x,dy=py-y;
        if(std::hypot(dx,dy)>=std::hypot(hw,hh)+std::hypot(hx,hy))return false;
        const float axes[4][2]={{c,s},{-s,c},{bc,bs},{-bs,bc}};
        for(const auto&axis:axes){const float u=axis[0],v=axis[1];
            if(std::abs(dx*u+dy*v)>hw*std::abs(c*u+s*v)+hh*std::abs(-s*u+c*v)
                +hx*std::abs(bc*u+bs*v)+hy*std::abs(-bs*u+bc*v))return false;
        }
        return true;
    };
    // VS BombInf runs before newborn fixed beams join the laser manager.
    return cancel_lasers_region(point,true,sample,laser_id,reward_mode==2);
}
int GameWorld::cancel_lasers_circle(float x,float y,float radius,bool respect_lock,bool registered_only,u32 before_owner){
    return cancel_lasers_region([=](float px,float py){return (x-px)*(x-px)+(y-py)*(y-py)<=radius*radius;},respect_lock,{},0,registered_only,false,before_owner);
}
void GameWorld::cancel_all_lasers(){
    cancel_lasers_region([](float,float){return true;},false,{},0,false,true);
    for(auto& laser:lasers)laser.active=false;
    // ECL613 retires the controllers before native laser contact. Earlier
    // fixed-laser samples must not pay their deferred graze after this clear.
    laser_graze_rewards_.clear();
}
int GameWorld::cancel_lasers_region(const std::function<bool(float,float)>& insideRegion,bool respect_lock,
    const std::function<bool(float,float,float)>& rectangleSample,u32 laser_id,bool registered_only,bool global_clear,u32 before_owner){
    // 0x530c60 -> 0x52a040 (+0x30); 0x52ac90 splits on 16-unit samples.
    const float cancelMargin=global_clear?16.f:32.f;
    const size_t initial=lasers.size();int hits=0;
    // The VS laser manager visits the most recently registered controller first.
    for(size_t index=0;index<initial;++index){
        const size_t i=reward_mode==2?initial-1-index:index;
        auto base=lasers[i];
        if(laser_id&&base.id!=laser_id)continue;
        // The native player clears respawn circles before arrival lasers join
        // the laser manager. Their first registration pass still awaits here.
        // Circle-cut straight children also wait for that first manager visit.
        if(registered_only&&reward_mode==2&&
           ((base.fixed()&&!base.age)||(base.straight&&base.fragment&&base.age==1)||(base.straight&&base.motion_frame<0)))continue;
        // Guarded EX controllers register their new fixed beams after older
        // discharge controllers finish this ECL pass.
        if(before_owner&&base.fixed()&&!base.age&&base.owner_id>before_owner)continue;
        if(!base.active||base.phase==1||(respect_lock&&base.cancel_lock!=0))continue;
        if(base.curved){
            // 524930 marks individual nodes; 525270 preserves runs of >=4.
            const auto&nodes=base.curve.nodes;std::vector<bool> cut(nodes.size());bool touched=false;
            for(size_t k=0;k<nodes.size();++k)if(insideRegion(nodes[k].x,nodes[k].y)){
                cut[k]=true;touched=true;++hits;++cancelled_bullets;
                if(k%10==0){traceCancelSource("laser-curved",battle_frame,212+base.source.color*2,base.id,nodes[k].x,nodes[k].y);bullet_cancels.push_back({nodes[k].x,nodes[k].y,212+base.source.color*2,{},next_visual_order++});}
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
        if(base.straight){ // 0x52c9f0 -> 0x52d680, including moving fixed-laser remnants.
            std::vector<bool> mask;
            for(float d=8;(global_clear?d+8<base.length:d+8<=base.length)&&mask.size()<512;d+=16){float px=base.x+std::cos(base.angle)*d,py=base.y+std::sin(base.angle)*d;
                bool cut=rectangleSample?rectangleSample(px,py,base.angle):insideRegion(px,py);mask.push_back(cut);
                if(cut){
                    ++hits;++cancelled_bullets;
                    // 0x52bce0 keeps straight remnants outside the field too.
                    traceCancelSource("laser",battle_frame,212+base.source.color*2,base.id,px,py);
                    bullet_cancels.push_back({px,py,212+base.source.color*2,{},next_visual_order++});
                }
            }
            if(std::none_of(mask.begin(),mask.end(),[](bool b){return b;}))continue;
            // 0x52cd04 retires the controller on its next manager update.
            // Later cancellation callbacks in this tick still sample its span.
            if(reward_mode==2&&std::all_of(mask.begin(),mask.end(),[](bool b){return b;})){
                lasers[i].cancel_pending=true;continue;
            }
            lasers[i].active=false;bool kept=false;
            for(size_t k=0;k<mask.size();){if(mask[k]){++k;continue;}size_t first=k;while(k<mask.size()&&!mask[k])++k;
                // 0x52d8ec..0x52d923 builds each split from whole samples.
                const float len=16.f*(k-first);
                if(len<=24.f){
                    // A retiring circle-cut head still owns its controller.
                    // Any surviving tail must register a fresh controller.
                    if(!first){lasers[i].length=len;kept=true;}
                    continue;
                }
                auto part=base;const float offset=16.f*first;part.x+=std::cos(base.angle)*offset;part.y+=std::sin(base.angle)*offset;
                const float retainedLength=reward_mode==2&&!kept&&first&&k==mask.size()
                    ?base.length-offset:len;
                part.length=part.straight_target=retainedLength;part.travelled=0;part.travel_cap=base.source.offset;part.offscreen_grace=30;
                if(!kept){
                    // 0x52db90 registers the hit rectangle before the bomb's
                    // rectangular cut. The original segment uses it this tick.
                    // Repeated cuts keep the rectangle registered before this
                    // manager pass until the next motion update.
                    part.cut_contact=base.contact_span();
                    // 0x52d875 subtracts the removed length from the existing
                    // growth target. A head segment still growing keeps its
                    // remaining growth; a shifted leading run resets the target.
                    if(!first)part.straight_target=base.straight_target-(base.length-len);
                    part.travel_cap=base.travel_cap;part.travelled=first?offset:base.travelled;
                    part.offscreen_grace=base.offscreen_grace;
                    lasers[i]=part;kept=true;
                }else{
                    // A split creates a fresh native laser controller. Its first
                    // manager pass moves it and starts a new contact timer.
                    // A new controller does not inherit an earlier clear's
                    // deferred retirement flag from its source controller.
                    part.cancel_pending=false;
                    part.id=next_id++;part.cut_contact.reset();
                    part.graze_clock=0;part.aura_slowed=false;part.motion_frame=-1;
                    // A rectangular player clear runs after the native laser
                    // visit, before battle_frame advances in the VS world.
                    // Its child first contacts on the following manager visit.
                    const bool beforeWorldTick=rectangleSample&&base.motion_frame>battle_frame;
                    part.split_registration_frame=beforeWorldTick?base.motion_frame:-1;
                    // Player-contact callbacks register after this manager visit.
                    // Bomb clears still admit the new controller to the late pass.
                    if(beforeWorldTick&&laser_id)part.motion_frame=part.split_registration_frame;
                    part.straight_changes={};part.straight_bounds_grace=3;part.change_sounds=0;
                    lasers.push_back(part);
                }
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
                // 0x52a970..0x52abe2 emits every rectangular-cut sample,
                // including samples outside the visible playfield.
                if(rectangleSample||(px+cancelMargin>-148&&px-cancelMargin<148&&py+cancelMargin>0&&py-cancelMargin<448))
                    {traceCancelSource("laser",battle_frame,212+base.source.color*2,base.id,px,py);bullet_cancels.push_back({px,py,212+base.source.color*2,{},next_visual_order++});}}
        }
        if(std::none_of(cut.begin(),cut.end(),[](bool v){return v;}))continue;
        size_t first=0;while(first<cut.size()&&!cut[first])++first;
        // 0x52b250 reads the fixed laser rectangle registered before the bomb.
        if(base.fixed())lasers[i].cut_contact=base.contact_span();
        lasers[i].length=float(first)*16;bool kept=first>0;
        extern bool traceOpeningRng;
        if(traceOpeningRng&&base.fixed())std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"fixed-cut\",\"id\":%u,\"position\":[%.9g,%.9g],\"angle\":%.9g,\"before\":%.9g,\"after\":%.9g,\"rectangle\":%d}\n",battle_frame,base.id,base.x,base.y,base.angle,base.length,lasers[i].length,bool(rectangleSample));
        for(size_t k=first;k<cut.size();){if(cut[k]){++k;continue;}
            const size_t begin=k;while(k<cut.size()&&!cut[k])++k;
            auto part=base;float offset=float(begin)*16;
            part.x+=std::cos(base.angle)*offset;part.y+=std::sin(base.angle)*offset;
            part.length=float(k-begin)*16;
            part.fragment=true;part.phase=2;part.clock=0;part.age=0;part.travelled=0;
            part.travel_cap=(base.fragment?base.travel_cap-base.travelled:base.source.max_length)-offset;
            // 0x52ae4e..0x52afa1 initializes a straight remnant moving at 8.
            // Circle cuts initialize movement before the next manager pass.
            // Contact remnants retain their player-manager ownership.
            part.straight=true;part.straight_target=part.length;
            part.source.speed=8;part.source.changes={};
            part.source.sound=part.source.end_sound=-1;
            part.straight_changes={};part.straight_changes.initialized=true;
            part.straight_changes.vx=float(std::cos(double(part.angle))*8);
            part.straight_changes.vy=float(std::sin(double(part.angle))*8);
            part.offscreen_grace=30;part.straight_bounds_grace=3;part.cancel_lock=0;
            part.cut_contact.reset();part.graze_clock=0;part.aura_slowed=false;
            part.contact_fragment=laser_id!=0;
            if(rectangleSample){
                // Player-contact remnants register after this laser-manager visit.
                // Bomb remnants remain available to intervening ECL circles.
                part.motion_frame=laser_id?battle_frame+1:-1;
            }else{
                part.tick(player_x,player_y);
                // Preserve the early-pass guard and consume late registration once.
                part.motion_frame=std::max(part.motion_frame,battle_frame);
            }
            if(base.fragment&&!kept){part.id=base.id;lasers[i]=part;kept=true;}
            else{part.id=next_id++;lasers.push_back(part);}
        }
        if(base.fragment&&!kept)lasers[i].active=false;
    }
    if(reward_mode==2&&bullet_scripts){
        for(auto& effect:bullet_cancels){
            if(effect.animation.stream||effect.script<212)continue;
            const auto script=bullet_scripts->find(effect.script);
            if(script==bullet_scripts->end())continue;
            effect.animation.bind_entry(script->second.stream,script->second.len,bullet_scripts);
            effect.animation.clock_after_execution=true;
            effect.animation.tick();
        }
    }
    return hits;
}
static float deathChainRadius(const DeathChain& chain,int age){
    return 1+(chain.target-1)*nativeEase(5,float(age)/chain.duration);
}
void GameWorld::tick_vs_death_chain_cancels(){
    if(reward_mode!=2)return;
    for(auto chain=death_chains.rbegin();chain!=death_chains.rend();++chain){
        const int age=chain->age+1;
        // 0x51c8c0 scans at full radius on the last tick before retirement.
        if(age<=chain->duration)cancel_death_chain_bullets(*chain,deathChainRadius(*chain,age),battle_frame+1);
    }
}
void GameWorld::cancel_death_chain_bullets(const DeathChain& c,float radius,int frame){
    // 0x51c8c0: only live descriptors 0..3, radius+1, without the
    // ordinary cancellation lock/width filter. Each becomes a cross-field transit.
    // VS traverses the active list from the newest slot.
    for(size_t i=0;i<bullets.size();++i){
        auto& b=bullets[reward_mode==2?bullets.size()-1-i:i];
        extern bool traceOpeningRng;
        if(traceOpeningRng&&b.state==1&&b.kind<4&&std::abs(std::hypot(b.x-c.x,b.y-c.y)-(radius+1))<1.f)
            std::printf("BULLETDBG {\"frame\":%d,\"event\":\"chain-boundary\",\"id\":%u,\"position\":[%.9g,%.9g],\"center\":[%.9g,%.9g],\"radius\":%.9g,\"age\":%d,\"duration\":%d,\"target\":%.9g}\n",battle_frame,b.id,b.x,b.y,c.x,c.y,radius,c.age,c.duration,c.target);
        if(!tsukasaReflects(b.state,unsigned(b.kind),b.x,b.y,c.x,c.y,radius))continue;
        // The VS bullet manager exposes counter arrivals to this scan on the next tick.
        if(reward_mode==2&&b.reflected&&!b.scripted&&b.age==0)continue;
        const float effect_x=b.x,effect_y=b.y;
        if(reward_mode==2){
            // 0x4eed33 moves the canceled bullet by half a step after the
            // cancellation effect starts. 0x51c9c6 uses the advanced position.
            b.x+=b.vx*.5f;
            b.y+=b.vy*.5f;
        }
        // 0x51ca73 reads the scalar stored at launch (Bullet+0x66c).
        const float reflectedSpeed=reward_mode==2?b.launch_speed:std::hypot(b.vx,b.vy);
        ChainBulletTransit transit{b.x,b.y,reflectedSpeed,b.kind,b.color,frame};
        if(reward_mode==2){
            extern bool traceOpeningRng;
            if(traceOpeningRng)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"chain-rng-before\",\"id\":%u,\"seed\":%u}\n",battle_frame,b.id,AnmVm::random_checkpoint(true));
            // 0x4eeb30 executes label 1 on the bullet's own VM at cancellation.
            if(b.animation.stream){b.animation.interrupt(1);b.animation.tick();}
            if(traceOpeningRng)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"chain-rng-after-anm\",\"id\":%u,\"seed\":%u}\n",battle_frame,b.id,AnmVm::random_checkpoint(true));
            // 0x51c8c0 builds the reflected shot during cancellation.
            const float dx=AnmVm::rand_signed_unit(1.f,true);
            const float dy=AnmVm::rand_unit(true);
            const auto target=ReflectedBulletFlight::destination(b.x,dx,dy);
            transit.targetX=target.x;
            transit.targetY=target.y;
            transit.kind=chainBulletType(b.kind,frame,frame>=3600?AnmVm::rand_integer(true):0);
            AnmVm::rand_integer(true);
            transit.duration=ReflectedBulletFlight::flightDuration(AnmVm::random_checkpoint(true));
            transit.startTangentX=AnmVm::rand_signed_unit(100.f,true);
            transit.endTangentX=AnmVm::rand_signed_unit(100.f,true);
        }
        extern bool traceOpeningRng;
        if(traceOpeningRng)std::printf("BULLETDBG {\"frame\":%d,\"event\":\"chain-flight\",\"id\":%u,\"source\":[%.9g,%.9g],\"target\":[%.9g,%.9g],\"kind\":%d,\"color\":%d,\"duration\":%d}\n",battle_frame,b.id,b.x,b.y,transit.targetX,transit.targetY,transit.kind,transit.color,transit.duration);
        chain_bullet_transits.push_back(transit);
        // 0x4eeb30 leaves the slot occupied until the embedded ANM ends.
        if(reward_mode==2&&b.animation.stream)b.state=Bullet::Canceling;
        else retire_bullet(b);
        ++cancelled_bullets;
        record_bullet_cancel(b,effect_x,effect_y,reward_mode==2);
        ++combo;combo_peak=std::max(combo_peak,combo);
        combo_attack.on_bullet_cancel(combo);
        if(combo>1&&combo>combo_display&&combo_timer>0){combo_display=combo;combo_display_timer=120;}
        pure_spirit_meter.add_bullets(30,difficulty,b.x,b.y);
        attack_credit_x=b.x;attack_credit_y=b.y;
        pending_native_attack_credit+=int(SpiritMeter::multiplier(difficulty))+
            (std::clamp(spirit_rank,0,std::max(0,spirit_rank_cap))/100)*2;
    }
}
static void advanceDeathChainArea(DeathChain& chain,bool versus){
    if(++chain.age>=chain.duration)return;
    // The native manager publishes the next radius before enemy contacts.
    chain.radius=deathChainRadius(chain,chain.age);
    if(versus){
        chain.area_x=snapEnemyCoordinate(chain.area_x);
        chain.area_y=snapEnemyCoordinate(chain.area_y);
    }
}
static bool deathChainHits(const DeathChain& chain,const Enemy& enemy){
    const float dx=chain.area_x-enemy.x,dy=chain.area_y-enemy.y;
    const float radius=chain.radius+enemy.hitbox_w*.5f;
    // 0x51e740 tests the area circle against the enemy's hit shape.
    return (enemy.flags&0x1000)
        ? nativeEnemyRectangleContact(dx,dy,enemy.contact_angle,enemy.hitbox_w,enemy.hitbox_h,chain.radius)
        : dx*dx+dy*dy<=radius*radius;
}
void GameWorld::tick_death_chains(){
    // Copy the record because a death can append another chain.
    const size_t count=death_chains.size();
    for(size_t i=0;i<count;++i){
        auto c=death_chains[i];
        advanceDeathChainArea(c,reward_mode==2);
        if(c.age<c.duration){
            if(c.damage!=0)for(auto& e:enemies){
                if(!e.active||e.sprite<0||!deathChainHits(c,e))continue;
                e.status_flags|=0x2000000;
                damage_enemy(e,c.damage,false,battle_frame-1);
            }
            if(reward_mode!=2)cancel_death_chain_bullets(c,c.radius,battle_frame);
        }
        death_chains[i]=c;
    }
    death_chains.erase(std::remove_if(death_chains.begin(),death_chains.end(),[](const auto&c){return c.age>=c.duration;}),death_chains.end());
}
void GameWorld::scatter_charge_pickup(float x,float y){
    // 0x521c60 shares the accumulator between chain and graze rewards.
    pickup_fraction+=pickup_rate;
    while(pickup_fraction>=100){
        pickup_fraction-=100;
        constexpr float pi=3.141592741f;
        const float angle=AnmVm::rand_signed_unit((pi/180.f)*10.f,true)-pi*.5f;
        spawn_pickup(9,x,y,2.f*std::cos(angle),2.f*std::sin(angle));
    }
}
void GameWorld::spawn_pickup(int type,float x,float y,float vx,float vy){
    if(type<1||type>14)return;
    Pickup p;
    p.id=next_id++;p.x=std::clamp(x,-192.f,192.f);p.y=y;p.type=type;p.vx=vx;p.vy=vy;
    ++pickup_serial;
    // 0x413567 / 0x4135cf initialize separate 16- and 300-slot pools.
    // 0x521d75 advances the spawn serial even when the pool is saturated.
    const bool charge=type>=9;
    const auto occupied=std::count_if(pickups.begin(),pickups.end(),[charge](const auto& item){return item.active&&(item.type>=9)==charge;});
    if(occupied+retiring_pickup_slots[charge]>=(charge?300:16))return;
    if(type>=9){
        // 0x521fa1 queues charge points and staggers their launch by spawn order.
        p.x=x;
        p.delay=int(pickup_serial%4);
        p.state=p.delay?5:2;
    }
    pickups.push_back(p);
}
void GameWorld::tick_pickups(float px,float py,bool bomb_collect,bool can_collect,bool hit_pending){
    retiring_pickup_slots={};
    // 0x520d40; defaults from Player +0x20b0..0x20c0: 5,30,70,70,128.
    for(auto&p:pickups){if(!p.active)continue;
        if(p.state==5){
            if(--p.delay<=0)p.state=2;
            continue;
        }
        float dx=px-p.x,dy=py-p.y,d=std::hypot(dx,dy);
        // 0x520ecb enables the full-speed pull during the opening bomb ticks.
        // 0x520e72 and 0x521139 admit free or magnetized pickups.
        // Rising charge points keep state 2 until their upward launch ends.
        if((can_collect&&!hit_pending&&py<128&&(p.state==1||p.state==4))||(bomb_collect&&(p.state==1||p.state==4))){p.state=3;p.speed=pickup_speed;}
        if(p.state==3||p.state==4){
            // 0x5211be/0x521263 choose pi/2 for coincident positions.
            // Both magnet states share the x87 full-speed integration path.
            const float angle=(dx==0&&dy==0)?1.57079637050628662109f:std::atan2(dy,dx);
            p.vx=float(std::cos(double(angle))*p.speed);
            p.vy=float(std::sin(double(angle))*p.speed);
            p.x+=p.vx;p.y+=p.vy;
            // 0x521303 permits the final acceleration to exceed 12 slightly.
            if(p.speed<12.f)p.speed+=.2f;
            if(hit_pending){p.state=1;p.vx=p.vy=0;}
        }
        else{
            p.x+=p.vx;p.y+=p.vy;p.vy=std::min(2.f,p.vy+.03f);
            if(p.state==2){if(p.vy>=0)p.state=1;}
            else if(p.vy>=0)p.vx=0;
            // 0x520fcf / 0x5210d3 retire free pickups before the collection test.
            if(p.y>472.f||std::abs(p.x)>=156.f){p.active=false;continue;}
        }
        const float distance=std::hypot(px-p.x,py-p.y);
        // 0x521356 skips collection and magnet acquisition while Player is Dead.
        if(can_collect&&distance<30){p.active=false;++collected;if(p.type>=9&&p.type<=14)pending_energy+=15;sound_events.push_back(37);}
        // 0x521449 enters magnetized state after this tick's movement.
        else if(can_collect&&p.state==1&&distance<pickup_range){p.state=4;p.speed=pickup_speed/3.f;}
    }
    // Late graze callbacks share the pool state from before this item pass.
    for(const auto& pickup:pickups)if(!pickup.active)++retiring_pickup_slots[pickup.type>=9];
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
        if(b.state>=Bullet::Canceling||b.age<=20||b.last_tsukasa_id==e.id)continue;
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
                traceCancelSource("script",battle_frame,b.cancel_script,b.id,b.x,b.y);
                if(b.cancel_script>=0)bullet_cancels.push_back({b.x,b.y,b.cancel_script,{},next_visual_order++});
                sound_events.push_back(71);
            }
            retire_bullet(b);continue;
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
static bool marisaAuraOverlapsLaser(const ChargeRegion& region,const InfiniteLaser& laser,float cx,float cy,float length){
    return region.held&&region.character==1&&
        shotRectanglesOverlap(cx-region.x,cy-224.f,laser.angle,length,laser.collisionWidth(),
                              0.f,region.radius*2.f,448.f);
}
// 4f5130 -> 549fb0 -> 521c60: physical and positive aura contact share
// graze stock, attached animation, secondary tint and energy-item accumulator.
void GameWorld::tick_bullet_graze(Bullet& b,float px,float py,float player_radius){
    b.graze_x=b.graze_y=0;b.graze_tint=0;
    // 0x4f4d6a checks contacts during spawn. The extra pass at
    // 0x4f4d54 treats overlap as graze once the slot is eight ticks old.
    const bool spawningGraze=reward_mode==2&&b.state==2;
    if((b.state!=1&&!spawningGraze)||(b.flags&0x200))return;
    const float radius=nativeBulletContactRadius(b.kind);
    const int contact=nativeOrdinaryBulletContact(b.kind,px-b.x,py-b.y,radius,player_radius);
    // 0x4f52dc joins aura-only contact to the ordinary graze reward
    // path at 0x4f54a7, including the trail and pickup accumulator.
    const bool aura_graze=b.state==1&&charge_region.contains(b.x,b.y,radius);
    const bool grazed=contact==2||(spawningGraze&&b.age>=8&&contact==1)||aura_graze;
    // 0x4f4d60 adds a pass for state 2 at age eight. The normal pass
    // at 0x4f4d77 still runs unless the first pass hit the player.
    const int passes=spawningGraze&&b.age>=8&&contact!=1?2:1;
    for(int pass=0;pass<passes;++pass){
        const auto result=b.graze.tick(grazed);
        if(result.interrupt&&b.attached_effect.stream){b.attached_effect.interrupt(result.interrupt);b.attached_effect.tick();}
        if(result.reward){
            const u32 color=nativeGrazeColor(b.kind,b.color);
            award_graze(b.x,b.y,color);
        }
        if(grazed){
            b.graze_tint=0xffff0080u|(u32(result.alpha)<<8);
            b.graze_y=AnmVm::rand_signed_unit(1.f,false);
            b.graze_x=AnmVm::rand_signed_unit(1.f,false);
        }
    }
}
void GameWorld::tick_graze(float px,float py,float player_radius,bool defer_rewards,bool invulnerable){
    if(defer_rewards)bullet_graze_pending_=true;
    else for(auto& bullet:bullets)tick_bullet_graze(bullet,px,py,player_radius);
    // 0x52b34f checks Aura contact if the fixed laser misses the player.
    // In versus, the laser manager scans the most recent registrations first.
    for(size_t i=0;i<lasers.size();++i){
        auto& laser=lasers[reward_mode==2?lasers.size()-1-i:i];
        const auto span=laser.contact_span();
        extern bool traceOpeningRng;
        if(!laser.fixed()||!laser.active||
           (laser.phase!=2&&laser.phase!=4)||span.length<=16.f||laser.width<=0.f)continue;
        const float dx=std::cos(laser.angle),dy=std::sin(laser.angle);
        const float half=span.length*.5f,length=span.length*.95f;
        const float cx=span.x+dx*half,cy=span.y+dy*half;
        const bool hit=laser.hitsPlayer(px,py,player_radius);
        const bool physical=!hit&&laser.hitsPlayer(px,py,player_radius+30.f);
        const bool aura=marisaAuraOverlapsLaser(charge_region,laser,cx,cy,length);
        // 0x5496f4 returns zero for invulnerable physical contact, allowing
        // 0x52b34f to check the active Aura before advancing the graze timer.
        if((!physical&&!aura)||(hit&&!invulnerable)||laser.graze_clock++%8!=0)continue;
        const auto target=physical?ChargeRegion::Point{px,py}:charge_region.origin;
        const float along=std::clamp((target.x-cx)*dx+(target.y-cy)*dy,-length*.5f,length*.5f);
        const float x=cx+dx*along,y=cy+dy*along;
        const u32 color=nativeGrazeColor(laser.source.type,laser.source.color);
        if(defer_rewards)laser_graze_rewards_.push_back({x,y,color,laser.id});
        else award_graze(x,y,color);
    }
}

void GameWorld::award_graze(float x,float y,u32 color){
    extern bool traceOpeningRng;
    if(traceOpeningRng)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"graze-reward\",\"position\":[%.9g,%.9g]}\n",battle_frame,x,y);
    if(traceOpeningRng)std::printf("GRAZEDBG {\"frame\":%d,\"position\":[%.9g,%.9g],\"player\":[%.9g,%.9g],\"seed\":%u,\"color\":%u}\n",battle_frame,x,y,player_x,player_y,AnmVm::random_checkpoint(true),color);
    grazed_bullets=std::min(grazed_bullets+1,99999999);
    sound_events.push_back(42);
    // 0x54a033 queues the trail using the low three bits as delay.
    const int delay=AnmVm::rand_integer(true)&7u;
    scatter_charge_pickup(x,y);
    graze_particles.emplace_back().start(x,y,color,delay);
    graze_particles.back().order=next_visual_order++;
}
void GameWorld::finish_laser_graze(bool player_contacts,float player_radius){
    if(player_contacts)for(auto it=lasers.rbegin();it!=lasers.rend();++it){
        auto& laser=*it;
        const auto span=laser.contact_span();
        // A newly split controller joins the native contact list on its next visit.
        if(laser.split_registration_frame==battle_frame)continue;
        if(!laser.straight||!laser.active||laser.cancel_pending||(laser.fragment&&laser.age==0)||(laser.phase!=2&&laser.phase!=4)||span.length<=16.f||laser.width<=3.f)continue;
        const float dx=std::cos(laser.angle),dy=std::sin(laser.angle);
        const float half=span.length*.5f;
        const float cx=span.x+dx*half,cy=span.y+dy*half;
        const float length=span.length*.8f;
        // 0x417a60 tests Marisa's Aura against the straight laser's hit rectangle.
        const bool aura=marisaAuraOverlapsLaser(charge_region,laser,cx,cy,length);
        if(aura)laser.aura_slowed=true; // 0x52de63 sets the 0.75 multiplier for subsequent updates.
        const bool physical=laser.hitsPlayer(player_x,player_y,player_radius+30.f);
        extern bool traceOpeningRng;
        if(traceOpeningRng)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"laser-graze\",\"id\":%u,\"position\":[%.9g,%.9g],\"angle\":%.9g,\"clock\":%u,\"physical\":%d,\"aura\":%d}\n",battle_frame,laser.id,span.x,span.y,laser.angle,laser.graze_clock,physical,aura);
        if((!aura&&!physical)||laser.hitsPlayer(player_x,player_y,player_radius)||laser.graze_clock++%8!=0)continue;
        // 0x52df1b projects either the player or the Aura's activation point.
        const auto target=physical?ChargeRegion::Point{player_x,player_y}:charge_region.origin;
        const float along=std::clamp((target.x-cx)*dx+(target.y-cy)*dy,-length*.5f,length*.5f);
        laser_graze_rewards_.push_back({cx+dx*along,cy+dy*along,nativeGrazeColor(laser.source.type,laser.source.color),laser.id});
    }
    // Fixed and straight controllers share the newest-first native laser list.
    // Preserve that order when their deferred rewards create charge pickups.
    std::stable_sort(laser_graze_rewards_.begin(),laser_graze_rewards_.end(),
        [](const auto& a,const auto& b){return a.laser_id>b.laser_id;});
    for(const auto& reward:laser_graze_rewards_)award_graze(reward.x,reward.y,reward.color);
    laser_graze_rewards_.clear();
}
void GameWorld::finish_bullet_graze(bool player_contacts,float player_radius,bool cancel_contacts,u32 shield_contact){
    // 0x4f4c4b scans newest first after the enemy update and its clears.
    if(player_contacts&&bullet_graze_pending_){
        for(auto it=bullets.rbegin();it!=bullets.rend();++it){
            auto& bullet=*it;
            if(cancel_contacts&&bullet.id!=shield_contact&&(bullet.state==1||bullet.state==2)&&!bullet.cancel_lock&&!(bullet.flags&0x200)&&
               nativeOrdinaryBulletContact(bullet.kind,player_x-bullet.x,player_y-bullet.y,nativeBulletContactRadius(bullet.kind),player_radius)==1)
                cancel_contact_bullet(bullet);
            else tick_bullet_graze(bullet,player_x,player_y,player_radius);
        }
    }
    bullet_graze_pending_=false;
    if(reward_mode!=2)tick_graze_particles();
}
void GameWorld::step_enemy_script(Enemy& enemy){
    if(result_pause_id&&enemy.id<=result_pause_id&&battle_frame>result_pause_frame)return;
    // 0x508630 compares the advanced phase clock against the timeout.
    ++enemy.phases.clock;
    // 0x508630 updates the HUD cache only when a registered boss has a timed phase.
    if(reward_mode==2&&(enemy.status_flags&0x80u)){
        const int remaining=enemy.phases.remaining_frames();
        if(remaining>=0)boss_timer_remaining=remaining;
    }
    const auto checkTimeout=[](Enemy& actor){
        if(!actor.pending_phase.empty())return;
        const auto trigger=actor.phases.check(actor.life,true);
        if(trigger&&!trigger.sub.empty()){
            actor.life=trigger.hp;
            actor.phases.consume(trigger);
            actor.status_flags|=0x100u;
            actor.pending_phase=trigger.sub;
        }
    };
    const auto enterPendingPhase=[](Enemy& actor){
        if(actor.pending_phase.empty()||!actor.vm)return false;
        std::string next=std::move(actor.pending_phase);
        actor.pending_phase.clear();
        actor.vm->start(next.c_str());
        return true;
    };
    if(reward_mode!=2)checkTimeout(enemy);
    const bool enteredPhase=enterPendingPhase(enemy);
    if(enemy.script_initialized_this_tick){
        enemy.script_initialized_this_tick=false;
        // The manager clears bit 4 first, then 0x508ee5 clears 0x4000000.
        enemy.spawn_hold=2;
        return;
    }
    // EXE 0x4fbea0 updates Suika's aim before ExAttackFunc reads it.
    enemy.apply_suika_steering(opponent_controls,opponent_x,opponent_y);
    // The separately allocated VM survives enemy-vector growth during step().
    if(!enemy.vm)return;
    const auto index=&enemy-enemies.data();
    enemy.vm->step();
    // 0x50925f runs the active ECL before 0x509b32 checks its phase timeout.
    if(reward_mode!=2||enteredPhase)return;
    auto& actor=enemies[index];
    if(!actor.active)return;
    checkTimeout(actor);
    if(enterPendingPhase(actor))actor.vm->step();
}
void GameWorld::advance_discharge_scripts(bool birth_only){
    // Birth constructors precede both field managers after player movement.
    // The first post-birth update runs with the persistent player callbacks.
    // Later radii run in the enemy pass after chain-flight cancellation.
    for(size_t i=enemies.size();i>0;--i){
        auto& enemy=enemies[i-1];
        // The birth update samples the moved player position in late dispatch.
        // The next update is reused by the later world pass.
        if(!enemy.active||enemy.age!=(birth_only?0:1)||enemy.spawn_hold||enemy.script_initialized_this_tick||enemy.script_advanced_before_cpu||!enemy.vm)continue;
        if(result_pause_id&&enemy.id<=result_pause_id&&battle_frame>=result_pause_frame)continue;
        if(std::string(enemy.vm->current_sub_name()).rfind("Ecl_EtBreakLv",0)!=0)continue;
        // This early callback precedes the normal enemy motion-grid update.
        if(birth_only)enemy.set_absolute_position(snapEnemyCoordinate(enemy.x),snapEnemyCoordinate(enemy.y));
        step_enemy_script(enemy);
        enemies[i-1].script_advanced_before_cpu=true;
    }
}
void GameWorld::advance_guarded_enemy_scripts(bool first_async_only){
    // On the guarded update after registration, native ECL advances before
    // CPU target selection while the actor's position is held.
    for(size_t i=enemies.size();i>0;--i){
        auto& enemy=enemies[i-1];
        if(!enemy.active||enemy.spawn_hold||(result_pause_id&&enemy.id<=result_pause_id&&battle_frame>=result_pause_frame))continue;
        // A registered wolf's deferred async context first runs on the next
        // manager visit, before CPU RNG, even while its arrival motion is held.
        const bool firstWolfAsync=(enemy.arrival_motion==Enemy::ArrivalMotion::FirstUpdate||
            (enemy.arrival_motion==Enemy::ArrivalMotion::Hold&&enemy.arrival_aura_registered))&&
            (enemy.status_flags&0x780000u)==0x200000u&&enemy.vm&&
            !enemy.vm->children.empty()&&enemy.vm->children.front()->current_time()<0&&
            !charge_region.contains(enemy.x,enemy.y,enemy.hitbox_w*.5f);
        if(firstWolfAsync&&!enemy.script_advanced_before_cpu){
            step_enemy_script(enemy);
            enemies[i-1].script_advanced_before_cpu=true;
            continue;
        }
        if(first_async_only||enemy.script_advanced_before_cpu)continue;
        // 0x4f9df5 calls 0x509030 before admitting a CPU target.
        // Their registered body bounds are available before CPU selection.
        if((enemy.status_flags&0x80000u)&&(enemy.status_flags&0x700000u)){
            // The prior animation pass published the current body extent.
            // Retire a fully escaped spirit before it can steer the CPU.
            const float halfWidth=enemy.cpu_body_half_width>=0?enemy.cpu_body_half_width:enemy.body_half_width;
            const float halfHeight=enemy.cpu_body_half_height>=0?enemy.cpu_body_half_height:enemy.body_half_height;
            const bool outsideX=enemy.x+halfWidth< -148.f||enemy.x-halfWidth>148.f;
            const bool outsideY=enemy.y+halfHeight<0.f||enemy.y-halfHeight>448.f;
            if((enemy.status_flags&1u)&&(outsideX||outsideY)&&!(enemy.flags&(outsideX?4:8))){
                enemy.active=false;
                continue;
            }
            // Converted spirits retire from ECL during the field update,
            // after the CPU has selected its target for this frame.
            continue;
        }
        // An arriving wolf inside the Aura joins on the following manager
        // visit. Convert before that visit advances its first displacement.
        if(((enemy.arrival_motion==Enemy::ArrivalMotion::FirstUpdate&&!enemy.arrival_aura_registered)||
            (enemy.arrival_motion==Enemy::ArrivalMotion::Hold&&enemy.arrival_aura_registered))&&!enemy.spawn_hold&&(enemy.status_flags&0x700000u)==0x200000u&&enemy.vm&&!enemy.callbacks[0].empty()&&charge_region.contains(enemy.x,enemy.y,enemy.hitbox_w*.5f)){
            if(enemy.vm->start(enemy.callbacks[0].c_str())){
                enemy.status_flags|=0x80000u;
                enemy.spirit_aura_exit=true;
                step_enemy_script(enemy);
                enemies[i-1].script_advanced_before_cpu=true;
                continue;
            }
        }
        // Guarded eagle arrivals enter the Aura callback before the held update.
        if(enemy.arrival_motion==Enemy::ArrivalMotion::Hold&&
           (enemy.status_flags&0x700000u)==0x400000u&&enemy.vm&&
           !enemy.callbacks[0].empty()&&charge_region.contains(enemy.x,enemy.y,enemy.hitbox_w*.5f)){
            if(enemy.vm->start(enemy.callbacks[0].c_str())){
                enemy.status_flags|=0x80000u;
                enemy.spirit_aura_exit=true;
                step_enemy_script(enemy);
            }
        }
        if(enemy.attack_entry!=Enemy::AttackEntry::StepScript&&enemy.arrival_motion!=Enemy::ArrivalMotion::Hold)continue;
        enemy.attack_entry=Enemy::AttackEntry::None;
        enemy.arrival_motion=Enemy::ArrivalMotion::HoldScriptUpdated;
        step_enemy_script(enemy);
    }
}
void GameWorld::tick_spirit_meter(){
    extern bool traceOpeningRng;
    int counterCount=0;
    const int pureCount=pure_spirit_meter.tick(spirit_rank,spirit_rank_cap,counted_spirits(),&counterCount);
    for(int i=0;i<counterCount;++i)counter_bullet_requests.push_back({pure_spirit_meter.x,pure_spirit_meter.y});
    for(int i=0;i<pureCount;++i)spirit_requests.push_back({pure_spirit_meter.x,pure_spirit_meter.y,1,0});
}
void GameWorld::tick_combo_timer(){
    // 0x5459f5: an expired chain of at least 20 raises this field's rank.
    if(combo_timer<=0||--combo_timer!=0)return;
    if(reward_mode==2&&combo>=20)increase_field_rank();
    combo=0;
}
void GameWorld::advance_bullet(Bullet& b){
    // 4f5680 case 2 first drifts half a step, then tests the ANM flag
    // as it stood on entry. On the release frame case 1 also runs.
    const bool spawning=b.state==2;
    if(spawning){b.x+=b.vx*.5f;b.y+=b.vy*.5f;if(b.animation.ivar[0])b.state=1;}
    const bool auraSlow=!spawning&&b.state==1&&!(b.flags&0x200)&&
        (!b.reflected||b.scripted||b.age>0)&&charge_region.contains(b.x,b.y,nativeBulletContactRadius(b.kind));
    b.motion_advanced=true;
    if(b.state==1&&b.scripted){
        float spriteWidth=0,spriteHeight=0;
        if(bullet_sprite_sizes){auto it=bullet_sprite_sizes->find(b.animation.stream?b.animation.sprite:b.sprite);
            if(it!=bullet_sprite_sizes->end()){spriteWidth=it->second[0];spriteHeight=it->second[1];}}
        int sounds=b.changes.tick(b.vx,b.vy,b.x,b.y,player_x,player_y,auraSlow?.5f:1.f,spriteWidth,spriteHeight);
        apply_bullet_changes(b);
        if(b.state==4)return;
        for(int i=0;i<sounds&&b.change_sound>=0;++i)sound_events.push_back(b.change_sound);
        if(b.changes.animation_interrupt>=0){b.animation.interrupt(b.changes.animation_interrupt);
            b.x-=4*b.vx;b.y-=4*b.vy;b.state=2;b.changes.animation_interrupt=-1;}
        if(b.changes.native_timer>=0){b.native_timer=b.changes.native_timer;b.changes.native_timer=-1;}}
    // 0x4f5219 sets Bullet+0x1024 to 0.5 on positive Aura contact.
    // Spawning bullets and flag0x200 are excluded by 0x4f5680.
    if(b.state==1&&!(b.flags&0x200)){const float movement=auraSlow?.5f:1.f;b.x+=b.vx*movement;b.y+=b.vy*movement;}
}
void GameWorld::advance_canceling_bullet(Bullet& b){
    extern bool traceOpeningRng;
    if(b.motion_advanced)return;
    // 0x4f568c marks the slot before updating the embedded animation.
    b.motion_advanced=true;
    if(b.contact_cancel){b.x+=b.vx*.5f;b.y+=b.vy*.5f;}
    // Bounds read the sprite published before the pending contact interrupt.
    cull_vs_bullet(b);
    if(b.state==4)return;
    b.animation.tick();b.contact_interrupt_pending=false;
    if(b.animation.deleted)retire_bullet(b);
}
void GameWorld::advance_vs_projectiles(){
    // 0x4f9b17 walks the newest-first active list before CPU collection.
    // Retiring each slot in that same pass preserves the native free-list order.
    for(size_t index=bullets.size();index>0;--index){
        auto& bullet=bullets[index-1];
        if(bullet.state==Bullet::Canceling){
            advance_canceling_bullet(bullet);
        }else if(bullet.scripted&&(bullet.state==1||bullet.state==2)&&!bullet.motion_advanced){
            advance_bullet(bullet);
            if(bullet.state==1||bullet.state==2)cull_vs_bullet(bullet);
        }else if(bullet.state==1&&bullet.reflected&&!bullet.scripted&&!(bullet.flags&0x200)){
            const float movement=bullet.age>0&&charge_region.contains(bullet.x,bullet.y,nativeBulletContactRadius(bullet.kind))?.5f:1.f;
            bullet.x+=bullet.vx*movement;
            bullet.y+=bullet.vy*movement;
            bullet.motion_advanced=true;
            cull_vs_bullet(bullet);
        }
    }
    // 0x4f9c7d advances the laser before 0x4f9d4e exports its CPU geometry.
    advance_lasers(LaserPass::BeforeCpu,battle_frame+1);
}
void GameWorld::cull_vs_bullet(Bullet& b){
    // 0x4f5ec9 tests the current sprite after movement and before contacts.
    // 0x4f5fa6 advances the grace timer after that update's bounds gate.
    if(b.native_timer>0){--b.native_timer;return;}
    if(b.changes.preserves_bounds()||!bullet_sprite_sizes)return;
    const int sprite=b.animation.stream?nativeBulletFrameSprite(b.kind,b.color,b.animation.sprite):b.sprite;
    const auto size=bullet_sprite_sizes->find(sprite);
    if(size==bullet_sprite_sizes->end())return;
    const auto [width,height]=size->second;
    if(b.x+width*.5f<=-148.f||b.x-width*.5f>=148.f||
       b.y+height*.5f<=-64.f||b.y-height*.5f>=448.f)retire_bullet(b);
}
void GameWorld::tick_graze_particles(){
    for(auto& p:graze_particles)p.tick();
    graze_particles.erase(std::remove_if(graze_particles.begin(),graze_particles.end(),[](const auto& p){return !p.active;}),graze_particles.end());
}
void GameWorld::advance_lasers(LaserPass pass,int motion_frame){
    std::vector<InfiniteLaser> reflected_lasers;
    for(auto& laser:lasers){
        const bool beforeCpu=reward_mode==2&&(laser.straight||laser.fixed());
        if(beforeCpu!=(pass!=LaserPass::Remaining))continue;
        if(pass==LaserPass::NewStraight&&!laser.straight&&!laser.fixed())continue;
        if(laser.motion_frame==motion_frame)continue;
        laser.motion_frame=motion_frame;
        if(laser.cancel_pending){laser.active=false;continue;}
        laser.tick(player_x,player_y,&reflected_lasers);
        for(int n=0;n<laser.change_sounds&&laser.source.end_sound>=0;++n)sound_events.push_back(laser.source.end_sound);
        laser.change_sounds=0;
    }
    for(auto& laser:reflected_lasers)if(lasers.size()<512){
        laser.id=next_id++;
        laser.motion_frame=motion_frame;
        lasers.push_back(std::move(laser));
    }
    lasers.erase(std::remove_if(lasers.begin(),lasers.end(),[](const auto& laser){return !laser.active;}),lasers.end());
}
void GameWorld::finish_new_lasers(){
    advance_lasers(LaserPass::NewStraight,battle_frame);
}
void GameWorld::register_marisa_extra_arrival(const std::string& sub,float x,float y,int life,
                                             const std::vector<const EclProgram*>& source){
    const auto actorIndex=enemies.size();
    auto& enemy=spawn_enemy(sub,x,y,life,0,0,&source);
    // Native rounds absolute coordinates and then the combined position.
    enemy.set_absolute_position(snapEnemyCoordinate(enemy.x),snapEnemyCoordinate(enemy.y));
    enemy.set_absolute_position(snapEnemyCoordinate(enemy.x),snapEnemyCoordinate(enemy.y));
    // 0x50c540 computes the arrival's combined displacement from the
    // constructor's zero position before its two registration guards clear.
    // CPU obstacle collection at 0x4f9e6d reads this displacement.
    enemy.previous_x=0;
    enemy.previous_y=0;
    const auto firstLaser=lasers.size();
    step_enemy_script(enemy);
    auto& actor=enemies[actorIndex];
    if(!actor.active)hold_finished_attack_controller(actor);
    extern bool traceOpeningRng;
    if(traceOpeningRng)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"arrival-register\",\"id\":%u,\"active\":%d,\"hold\":%u,\"entry\":%u,\"sprite\":%d,\"flags\":%u,\"status\":%u}\n",battle_frame,actor.id,actor.active,unsigned(actor.spawn_hold),unsigned(actor.attack_entry),actor.sprite,actor.flags,actor.status_flags);
    // Fixed lasers receive their registration update after this tick's ECL.
    // Preserve the late-pass guard for other emitted lasers.
    for(size_t i=firstLaser;i<lasers.size();++i)
        if(!lasers[i].fixed())lasers[i].motion_frame=battle_frame+1;
}
void GameWorld::register_native_extra_release(){
    // 0x50d6a3: only 100 post-arrival clocks exist. C2 uses the same
    // arrival callback without first reserving a slot, so the count is signed.
    // Arrival is followed by the native sender clock in the same frame.
    if(native_extra_reservations.size()<100)native_extra_reservations.push_back(239);
}
void GameWorld::resolve_marisa_extra_arrivals(u32 newer_than,bool result_phase,GameWorld* sender){
    // 0x53d019 links new flights at the head. 0x53d200 runs ExtraAttack00
    // synchronously, so simultaneous arrivals take RNG in that order.
    for(auto it=extra_flights.rbegin();it!=extra_flights.rend();++it){
        auto& flight=*it;
        if(flight.character!=1||flight.age<flight.duration||flight.id<=newer_than)continue;
        extern bool traceOpeningRng;
        if(traceOpeningRng)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"extra-flight-arrival\",\"flight\":%u,\"age\":%d,\"duration\":%d,\"script\":%d,\"source\":[%.9g,%.9g],\"target\":[%.9g,%.9g]}\n",battle_frame,flight.id,flight.age,flight.duration,flight.script,flight.source_x,flight.source_y,flight.x,flight.y);
        if(!result_phase)register_marisa_extra_arrival(flight.sub,flight.x,flight.y,flight.life,flight.programs);
        if(sender)sender->register_native_extra_release();
        ++extra_arrivals;
        flight.age=flight.duration+1;
    }
    extra_flights.erase(std::remove_if(extra_flights.begin(),extra_flights.end(),[](const auto& flight){return flight.age>flight.duration;}),extra_flights.end());
}
void GameWorld::tick_wave(bool tickSpiritMeter,
    const std::function<int(Enemy&)>& collect_shot_damage,bool defer_new_lasers){
    constructor_contacts=&collect_shot_damage;
    ++battle_frame;
    for(auto&f:extra_flights){
        // 0x53d177 checks Marisa's flight age before 0x53d270 advances it.
        const int age=reward_mode==2&&f.character==1?f.age++:++f.age;
        if(age<f.duration)continue;
        spawn_enemy(f.sub,f.x,f.y,f.life,0,0,&f.programs);if(f.character==4||f.character==5||f.character==6)sound_events.push_back(62);++extra_arrivals;
        f.age=f.duration+1;
    }
    extra_flights.erase(std::remove_if(extra_flights.begin(),extra_flights.end(),[](const auto&f){return f.age>f.duration;}),extra_flights.end());
    if(tickSpiritMeter){
        tick_graze_particles();
        tick_spirit_meter();
    }
    discard_retired_bullets();
    for(auto& b:bullets)if(b.state==Bullet::Canceling){
        advance_canceling_bullet(b);
        if(b.state==Bullet::Canceling)b.motion_advanced=false;
    }
    for(auto&b:bullets)if((b.reflected||b.scripted)&&(b.state==1||b.state==2)){
        if(!b.motion_advanced){
            advance_bullet(b);
            // Reused slots retain the movement flag. Only a constructor update
            // that actually runs also advances the native bounds grace timer.
            if(reward_mode==2&&(b.state==1||b.state==2))cull_vs_bullet(b);
        }
        if(b.state==4)continue;
        if(b.scripted)b.visual_heading=b.changes.angle;
        else if(b.vx!=0||b.vy!=0)b.visual_heading=std::atan2(b.vy,b.vx);
        if(!b.animation.stream&&bullet_scripts){auto it=bullet_scripts->find(nativeBulletAnimation(b.kind));if(it!=bullet_scripts->end()){b.animation.bind_entry(it->second.stream,it->second.len,bullet_scripts);b.animation.tick();b.animation.interrupt(2);}}
        if(!b.attached_effect.stream&&bullet_scripts){int id=nativeBulletAttachedEffect(b.kind);auto it=bullet_scripts->find(id);if(id&&it!=bullet_scripts->end())b.attached_effect.bind_entry(it->second.stream,it->second.len,bullet_scripts);}
        if(b.attached_effect.stream)b.attached_effect.tick();
        if(b.animation.stream)b.animation.tick();
        if(reward_mode!=2&&b.native_timer>0)--b.native_timer;
        ++b.age;
        if(!b.changes.preserves_bounds()&&(b.x < -220||b.x>220||b.y < -80||b.y>520))retire_bullet(b);
        else b.motion_advanced=false;
    }
    advance_lasers(LaserPass::Remaining,battle_frame);
    // 0x545120..0x5451a6: the display has its own countdown after a chain ends.
    if(combo_display_timer>0&&--combo_display_timer==0)combo_display=0;
    if(reward_mode!=2)tick_combo_timer();
    // VS wave scripts can observe a death caused solely by chain damage.
    const bool perEnemyDamage=reward_mode==2||bool(collect_shot_damage);
    if(perEnemyDamage)
        for(auto& chain:death_chains)advanceDeathChainArea(chain,true);
    // 0x508a03 checks the areas for this enemy before visiting the next.
    // Accumulate all admitted chain and shot damage before writing HP.
    const auto apply_damage=[&](Enemy& enemy){
        if(!perEnemyDamage||!enemy.active||enemy.sprite<0)return;
        int damage=0;
        // A chain born from an earlier enemy in this pass can already deal damage.
        for(auto& chain:death_chains){
            if(!chain.damage||chain.age>=chain.duration||!deathChainHits(chain,enemy))continue;
            enemy.status_flags|=0x2000000;
            extern bool traceOpeningRng;
            if(traceOpeningRng)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"chain-damage\",\"id\":%u,\"damage\":%d,\"area\":[%.9g,%.9g,%.9g],\"age\":%d}\n",battle_frame,enemy.id,chain.damage,chain.area_x,chain.area_y,chain.radius,chain.age);
            damage+=std::max(0,chain.damage);
        }
        if(collect_shot_damage){
            const int shot_damage=collect_shot_damage(enemy);
            extern bool traceOpeningRng;
            if(traceOpeningRng&&(damage||shot_damage))std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"damage-sources\",\"id\":%u,\"chain\":%d,\"shot\":%d}\n",battle_frame,enemy.id,damage,shot_damage);
            damage+=shot_damage;
        }
        if(damage>0)damage_enemy(enemy,damage,false,battle_frame-1);
    };
    // 0x50b32d walks the VS enemy list from the newest registered enemy.
    // Use indices because an ECL step can append enemies and move the vector.
    // Stage constructors precede the registration of prepared flight arrivals.
    // Publish those arrivals last before traversing the newest-first list.
    if(reward_mode==2)std::stable_partition(enemies.begin(),enemies.end(),[](const Enemy& enemy){
        return enemy.arrival_motion!=Enemy::ArrivalMotion::FirstUpdate;
    });
    const size_t initial_count=enemies.size();
    for(size_t update=0;update<enemies.size();++update){
        const size_t i=reward_mode==2&&update<initial_count?initial_count-1-update:update;
        if(!enemies[i].active||(result_pause_id&&enemies[i].id<=result_pause_id&&battle_frame>result_pause_frame))continue;
        // 0x509030: spirits enter the registered Disable script once;
        // only ordinary enemies use the half-speed clock below.
        auto&spirit=enemies[i];
        if(spirit.spawn_hold){
            // VS accounts for the prepass displacement on the birth tick.
            // Advance ECL once on the first held tick, as 0x4f9df5 does.
            if(--spirit.spawn_hold==1&&reward_mode==2&&spirit.vm&&
               spirit.attack_entry!=Enemy::AttackEntry::RetireController)spirit.vm->step();
            apply_damage(enemies[i]);
            continue;
        }
        if(spirit.attack_entry==Enemy::AttackEntry::RetireController){spirit.active=false;continue;}
        const bool guarded_script_updated=spirit.arrival_motion==Enemy::ArrivalMotion::HoldScriptUpdated;
        bool script_updated=guarded_script_updated||spirit.script_advanced_before_cpu;
        spirit.script_advanced_before_cpu=false;
        const bool hold_motion=spirit.arrival_motion==Enemy::ArrivalMotion::Hold||guarded_script_updated;
        const bool first_arrival_update=spirit.arrival_motion==Enemy::ArrivalMotion::FirstUpdate;
        // 0x509108 tests the initial combined position against the Aura
        // after the player's movement, before copying the flight destination.
        if(first_arrival_update)
            spirit.arrival_slow_motion=charge_region.contains(0.f,0.f,spirit.hitbox_w*.5f);
        const bool in_aura=charge_region.contains(spirit.x,spirit.y,spirit.hitbox_w*.5f);
        // Registration samples the preceding Aura, but movement samples its
        // current geometry. Retain the held registration visit only while the
        // arriving wolf is still inside after the player's update.
        if(first_arrival_update&&spirit.arrival_aura_registered&&in_aura){
            // Registration already consumed the first arrival visit. Keep its
            // guard for the following Aura callback without scheduling it twice.
            spirit.arrival_motion=Enemy::ArrivalMotion::Hold;
            apply_damage(enemies[i]);
            continue;
        }
        const bool converted_wolf=(spirit.status_flags&0x780000u)==0x280000u;
        bool converted_this_tick=first_arrival_update&&script_updated&&converted_wolf;
        // 0x50903f skips Aura conversion while the arrival guard is set.
        if(!hold_motion&&!(first_arrival_update&&spirit.arrival_aura_registered)&&
           (spirit.status_flags&0x700000u)&&!(spirit.status_flags&0x80000u)&&
           spirit.vm&&!spirit.callbacks[0].empty()&&in_aura){
            const auto callback=spirit.callbacks[0];
            if(spirit.vm->start(callback.c_str())){
                spirit.status_flags|=0x80000u;spirit.spirit_aura_exit=true;
                // 0x509030 runs the conversion callback before the arrival's
                // first displacement, replacing the flight velocity immediately.
                step_enemy_script(spirit);script_updated=true;
                converted_this_tick=!first_arrival_update;
            }
        }
        // 0x50916e halves ordinary enemy displacement in the Aura.
        // 0x50925f reads the separate ECL rate, which stays at 1 in VS.
        auto& timed=enemies[i];
        const bool slowed=!(timed.status_flags&0x700000u)&&timed.sprite>=0&&!(timed.flags&0x20)&&in_aura;
        const float motion_step=slowed&&reward_mode==2?.5f:1.f;
        timed.slow_clock+=slowed&&reward_mode!=2?.5f:1.f;
        const bool script_step=timed.slow_clock>=1;
        // 0x50923f advances motion before the active ECL tick at 0x50925f.
        // Initialization and spirit conversion use their own phases.
        const bool move_before_script=reward_mode==2&&!timed.script_initialized_this_tick&&
            !first_arrival_update&&!hold_motion;
        if(!script_step&&motion_step==1.f){apply_damage(enemies[i]);continue;}
        if(script_step){
            timed.slow_clock-=1;
            timed.arrival_motion=first_arrival_update
                ? Enemy::ArrivalMotion::Hold : Enemy::ArrivalMotion::Normal;
            if(!first_arrival_update)timed.arrival_aura_registered=false;
            if(!first_arrival_update&&!script_updated&&!move_before_script)step_enemy_script(timed);
        }
        if(hold_motion){
            // The conversion callback precedes the guarded manager visit.
            // Its normal ECL step still runs while displacement and HP hold.
            if(script_updated&&!guarded_script_updated&&converted_wolf)
                step_enemy_script(enemies[i]);
            // The arrival guard protects HP while registered shots are still
            // consumed and hit feedback is published for every spirit type.
            const auto savedFlags=enemies[i].flags;
            enemies[i].flags|=0x10u;
            apply_damage(enemies[i]);
            enemies[i].flags=savedFlags;
            continue;
        }
        // Integrate field-local movement.
        Enemy& e=enemies[i];
        if(!e.death_initialized){
            if(e.death_script==0){e.death_script=nativeEnemyDeathScript(e.animations[0].file,e.sprite);e.death_file=1;}
            e.death_initialized=true;
        }
        if(e.active){
            extern bool traceOpeningRng;
            if(traceOpeningRng&&e.sprite>=0)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"enemy-health\",\"id\":%u,\"hp\":%d,\"flags\":[%u,%u],\"body\":[%.9g,%.9g],\"hitbox\":[%.9g,%.9g]}\n",battle_frame,e.id,e.life,e.flags,e.status_flags,e.body_half_width,e.body_half_height,e.hitbox_w,e.hitbox_h);
            if(traceOpeningRng&&e.sprite>=0)std::printf("ENEMYDBG {\"frame\":%d,\"event\":\"motion-before\",\"id\":%u,\"position\":[%.9g,%.9g],\"absolute\":[%.9g,%.9g],\"polar\":[%.9g,%.9g],\"velocity\":[%.9g,%.9g],\"turn\":[%d,%.9g,%d,%.9g,%.9g]}\n",battle_frame,e.id,e.x,e.y,e.absolute_x(),e.absolute_y(),e.angle,e.speed,e.vx,e.vy,e.turn.duration,e.turn.elapsed,e.turn.mode,e.turn.from,e.turn.to);
            if(traceOpeningRng&&(e.status_flags&0x700000u)==0x200000u&&e.vm)
                std::printf("ENEMYDBG {\"event\":\"wolf-ecl-clock\",\"frame\":%d,\"id\":%u,\"clock\":%.9g,\"arrival\":%d,\"converted\":%d}\n",battle_frame,e.id,e.vm->current_time(),int(e.arrival_motion),bool(e.status_flags&0x80000u));
            e.previous_x=e.x;
            e.previous_y=e.y;
            if(e.relative_ellipse||reward_mode==2){e.x=e.absolute_x();e.y=e.absolute_y();}
            e.scale_x.advance(e.sx,motion_step);e.scale_y.advance(e.sy,motion_step);
            // 0x50a4cc and 0x50a4f3 advance the scalar clocks before
            // 0x4d4dc0 applies the Aura rate to the resulting movement.
            e.turn.advance(e.angle);e.accelerate.advance(e.speed);
            if(e.move_x.duration>0||e.move_y.duration>0){
                // 0x50a5d4 -> 0x4108c0 advances the position interpolator
                // with the frame clock, independently of Aura displacement rate.
                const float interpolation_step=reward_mode==2?1.f:motion_step;
                const float x=e.x,y=e.y;e.move_x.advance(e.x,interpolation_step);e.move_y.advance(e.y,interpolation_step);
                e.vx=e.x-x;e.vy=e.y-y;
                if(e.move_x.duration==0&&e.move_y.duration==0)e.vx=e.vy=0;
            }else if(e.orbit){
                // Native 0x4d4dc0 mode 2 advances radius, then phase;
                // 0x4d4a00 resolves position relative to the saved center.
                e.radius+=e.radial_speed*motion_step;
                e.angle=std::remainder(e.angle+e.speed*motion_step,6.28318530717958647692f);
                // 0x4d4ab0 keeps FSINCOS and the radius product in x87
                // precision until each component is stored before the center add.
                float u=float(std::cos(double(e.angle))*e.radius),v=float(std::sin(double(e.angle))*e.radius);
                if(e.ellipse){u*=e.ellipse_ratio;float x=u;u=x*std::cos(e.ellipse_angle)-v*std::sin(e.ellipse_angle);v=x*std::sin(e.ellipse_angle)+v*std::cos(e.ellipse_angle);}
                const float x=e.orbit_x+u;
                const float y=e.orbit_y+v;
                e.vx=x-e.x;e.vy=y-e.y;e.x=x;e.y=y;
            }else{
                // EXE callback table 0x5994f8 entry 3 -> 0x4fbea0
                // rewrites Enemy+0xac (angle).  The following native motion
                // step consumes that angle even for the small-Suika actor;
                // its ECL record does not set the generic polar bit.
                if(e.polar||e.native_update_callback==3){
                    // 0x4d4e0c keeps FSINCOS and the speed product in x87
                    // precision until FSTP writes the velocity components.
                    e.vx=float(std::cos(double(e.angle))*e.speed);
                    e.vy=float(std::sin(double(e.angle))*e.speed);
                }
                // The ordinary path sets half-speed at birth. The spirit
                // path at 0x509058 preserves it, including during conversion.
                const float movement=e.arrival_slow_motion?.5f:motion_step;
                e.x+=e.vx*movement;e.y+=e.vy*movement;
            }
            // 0x50a686 and 0x50a6cd advance the separate positions through
            // 0x4d4d41 before 0x50c540 combines them on the same grid.
            e.x=snapEnemyCoordinate(e.x);e.y=snapEnemyCoordinate(e.y);
            if(e.relative_ellipse){
                e.relative_phase=std::remainder(e.relative_phase+e.relative_omega*motion_step,6.28318530718f);
                e.relative_radius+=e.relative_radial*motion_step;
                const auto offset=enemyEllipseOffset(e.relative_phase,e.relative_rotation,e.relative_radius,e.relative_ratio);
                e.relative_x=snapEnemyCoordinate(e.relative_center_x+offset[0]);
                e.relative_y=snapEnemyCoordinate(e.relative_center_y+offset[1]);
            }
            // 0x50c540 rounds the combined position separately from absolute motion.
            e.set_absolute_position(e.x,e.y);
            e.combined_dx=e.x-e.previous_x;
            e.combined_dy=e.y-e.previous_y;
            if(reward_mode==2){
                // 0x50c578 stores absolute + relative - previous as a float.
                // 0x4d4a00 adds that displacement back before grid rounding.
                const float dx=e.x-e.previous_x,dy=e.y-e.previous_y;
                e.x=e.previous_x+dx;e.y=e.previous_y+dy;
            }
            if(e.relative_ellipse||reward_mode==2){
                e.x=snapEnemyCoordinate(e.x);e.y=snapEnemyCoordinate(e.y);
            }
            // 0x50c540 writes +80 from absolute+relative minus previous
            // combined position, before clamping. Do not overwrite base vx.
            const float animation_dx=e.x-e.previous_x;
            // pl14b ANM13/15: after the 60/40-frame warning, ANM424(1)
            // follows motion. Retail 0x508af8..0x508b19 writes this angle
            // to BOTH the animation and EnemyInf+0x130 (contact rectangle).
            // Keep this in simulation, independent of rendering/rollback skips.
            const auto& body=e.animations[0];
            if(body.file>=2&&body.file-2<int(e.animation_files.size())&&
               e.animation_files[body.file-2]=="pl14b.anm"&&
               (body.script==13||body.script==15)&&!(e.flags&2))
                e.contact_angle=std::atan2(e.y-e.previous_y,animation_dx);
            // 0x50c540: ECL504 bounds clamp the combined enemy position.
            if(e.status_flags&2u){
                e.x=std::clamp(e.x,e.bound_x-e.bound_w*.5f,e.bound_x+e.bound_w*.5f);
                e.y=std::clamp(e.y,e.bound_y-e.bound_h*.5f,e.bound_y+e.bound_h*.5f);
                if(reward_mode==2){
                    // 0x50c67e writes the clamped position minus relative motion
                    // back into the absolute motion record.
                    e.absolute_base_x=e.x-e.relative_x;
                    e.absolute_base_y=e.y-e.relative_y;
                }else if(!e.relative_ellipse)e.set_absolute_position(e.x,e.y);
            }
            if(e.native_update_callback==2)strengthen_tsukasa_bullets(e);
            e.apply_hit_recoil(player_x,player_y);
            e.update_direction_animation(animation_dx);
            if(script_step)++e.age;
            if(e.sprite>=0){
                // 0x50a8d9: once the body has entered, retire it when its
                // full sprite leaves the field unless ECL protects that axis.
                const bool outsideX=e.x+e.body_half_width< -148.f||e.x-e.body_half_width>148.f;
                const bool outsideY=e.y+e.body_half_height<0.f||e.y-e.body_half_height>448.f;
                if(!outsideX&&!outsideY)e.status_flags|=1u;
                else if((e.status_flags&1u)&&!(e.flags&(outsideX?4:8)))e.active=false;
            }
            // ECL 1039 also marks wave fairies eligible for conversion.
            // Keep those actors on the ordinary bounds and ECL flag path.
            if(e.spirit_aura_exit||(e.status_flags&0x700000u)){
                if(e.x < -240||e.x>240||e.y < -32||e.y>480)e.active=false;
                if(e.vm&&e.vm->finished()&&(e.status_flags&0x80000u))e.active=false;
            }
        }
        // 0x50920a initializes conversion, then 0x50925f advances it
        // after the same tick's displacement.
        if(script_step&&enemies[i].active&&
           ((move_before_script&&!script_updated)||converted_this_tick))step_enemy_script(enemies[i]);
        auto& updated=enemies[i];
        if((updated.status_flags&0x80000u)&&updated.vm&&updated.vm->finished())updated.active=false;
        apply_damage(updated);
    }
    // The VS laser manager advances newly registered beams after ECL clears.
    if(reward_mode==2&&!defer_new_lasers)finish_new_lasers();
    if(perEnemyDamage)
        death_chains.erase(std::remove_if(death_chains.begin(),death_chains.end(),[](const auto&c){return c.age>=c.duration;}),death_chains.end());
    else tick_death_chains();
    if(reward_mode==2){
        // 0x50a079..0x50a095 enters and runs the new phase on the damage tick.
        for(size_t i=enemies.size();i>0;--i){
            auto& enemy=enemies[i-1];
            if(!enemy.active||enemy.pending_phase.empty())continue;
            const auto firstChild=enemies.size();
            step_enemy_script(enemy);
            // The regular pass already ended. Consume the birth guard of children
            // initialized by this phase callback on their registration tick.
            for(size_t child=firstChild;child<enemies.size();++child)
                if(enemies[child].active&&enemies[child].script_initialized_this_tick)
                    step_enemy_script(enemies[child]);
        }
    }
    // integrate spirit velocity toward the player
    for(auto& s:spirits){if(s.active){s.x+=s.vx;s.y+=s.vy;}}
    // Native destruction unregisters these objects. Retaining dead enemies kept
    // their VM stacks/children forever, and copied them into every rollback slot.
    enemies.erase(std::remove_if(enemies.begin(),enemies.end(),[](const auto&e){return !e.active;}),enemies.end());
    spirits.erase(std::remove_if(spirits.begin(),spirits.end(),[](const auto&s){return !s.active;}),spirits.end());
    if(reward_mode==2)for(auto& enemy:enemies)advance_enemy_protection(enemy);
    constructor_contacts=nullptr;
}
}
