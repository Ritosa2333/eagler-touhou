#include "ComboAttack.hpp"
#include "ScreenShake.hpp"
#include "CurveLaserPath.hpp"
#include "BossPhase.hpp"
#pragma once
#include "SpiritMeter.hpp"
#include "DeathRewards.hpp"
#include "Types.hpp"
#include "BulletChanges.hpp"
#include "BulletGraze.hpp"
#include "GrazeParticle.hpp"
#include "ReflectedBullet.hpp"
#include "StraightLaserChanges.hpp"
#include "AnmVm.hpp"
#include "EclProgram.hpp"
#include "EclVm.hpp"
#include "WaveSchedule.hpp"
#include "ChargeRegion.hpp"
#include "SpiritRouting.hpp"
#include <vector>
#include <functional>
#include <string>
#include <memory>
#include <array>
#include <unordered_map>
namespace th19 {

// ECL 600/602..605/608/700/701, native shooter stride 0x488.
struct LaserShooter {
    int type=0,color=0,sound=21,end_sound=38;
    bool has_unhandled_changes=false;
    std::array<BulletChange,24> changes{};
    unsigned shot_flags=0x23; // ECL600 native +474, separate from laser flags
    unsigned short count1=1,count2=1,aim_mode=0; // ECL606/607: +46c/+46e/+470
    float x=0,y=0,angle=0,angle2=0,speed=2,speed2=0;
    float absolute_x=0,absolute_y=0,muzzle_radius=0;bool absolute_origin=false;
    float length=0,max_length=0,offset=0,width=0;
    int warning=0,grow=0,hold=0,shrink=0,flags=0;
};
struct InfiniteLaser {
    u32 id=0;float x=0,y=0,angle=0,length=0,width=2;
    bool curved=false;CurveLaserPath curve;std::string curve_file;
    bool straight=false;float straight_target=0;int offscreen_grace=30,straight_bounds_grace=3,change_sounds=0;StraightLaserChanges straight_changes;
    bool fragment=false;float travelled=0,travel_cap=0;int cancel_lock=0;
    LaserShooter source;int phase=3,clock=0,age=0;bool active=true;
    // 0x52b0e0 supplies the native hit rectangle only in phases 4 and 2.
    float collisionWidth()const{return width>=32?width-(width+16)/(straight?2.f:3.f):width*.5f;}
    bool hitsPoint(float px,float py)const{
        if(curved)return active&&curve.hits(px,py,0,width,source.type,angle);
        if(!active||(phase!=4&&phase!=2)||length<=16||width<=(straight?3.f:0.f))return false;
        float dx=px-x,dy=py-y,c=std::cos(angle),s=std::sin(angle);
        float along=dx*c+dy*s,across=-dx*s+dy*c;
        return std::abs(along-length*.5f)<=length*(straight?.8f:.95f)*.5f&&std::abs(across)<=collisionWidth()*.5f;
    }
    // 0x549570 -> 0x4d44e0: circle against the rotated hit rectangle.
    // Side strips include equality; corner-circle boundaries are strict.
    bool hitsPlayer(float px,float py,float radius=3)const{
        if(curved)return active&&curve.hits(px,py,radius,width,source.type,angle);
        if(!active||(phase!=4&&phase!=2)||length<=16||width<=(straight?3.f:0.f))return false;
        float dx=px-x,dy=py-y,c=std::cos(angle),s=std::sin(angle);
        float a=std::abs(dx*c+dy*s-length*.5f),b=std::abs(-dx*s+dy*c);
        float hx=length*(straight?.8f:.95f)*.5f,hy=collisionWidth()*.5f;
        if((a<=hx+radius&&b<=hy)||(a<=hx&&b<=hy+radius))return true;
        return (a-hx)*(a-hx)+(b-hy)*(b-hy)<radius*radius;
    }
    // 52ea00 emits a fresh segment at each crossed edge. It does not rotate
    // the old beam: that segment continues out of the field.
    std::vector<InfiniteLaser> reflections(){
        std::vector<InfiniteLaser> result;auto&c=straight_changes;
        if(!straight||!c.reflecting)return result;
        const float dx=std::cos(angle),dy=std::sin(angle),hx=x+dx*length,hy=y+dy*length;
        bool crossed=false;
        auto bounce=[&](bool hit,float t,float newAngle){
            if(!hit)return;crossed=true;if(c.reflect_edges&16)return;
            InfiniteLaser l;l.straight=true;l.phase=2;l.source=source;l.source.angle=newAngle;l.angle=newAngle;
            l.x=x+dx*t;l.y=y+dy*t;l.source.x=l.x;l.source.y=l.y;l.source.offset=0;l.source.speed=c.reflect_speed;
            l.source.changes[c.reflect_slot].integers[0]=c.reflect_count;
            l.length=l.source.length;l.straight_target=l.source.max_length;l.width=l.source.width;
            l.travelled=l.straight_target<l.length?.01f:0;result.push_back(std::move(l));
        };
        bounce((c.reflect_edges&1)&&hy<0&&std::abs(dy)>1e-7f,-y/dy,-angle);
        bounce((c.reflect_edges&2)&&hy>448&&std::abs(dy)>1e-7f,(448-y)/dy,-angle);
        bounce((c.reflect_edges&4)&&hx<-148&&std::abs(dx)>1e-7f,(-148-x)/dx,std::remainder(-angle-3.141592741f,6.283185482f));
        bounce((c.reflect_edges&8)&&hx>148&&std::abs(dx)>1e-7f,(148-x)/dx,std::remainder(-angle-3.141592741f,6.283185482f));
        if(crossed){c.reflecting=false;++change_sounds;}
        return result;
    }
    void tick(float player_x=0,float player_y=400,std::vector<InfiniteLaser>* reflected=nullptr){
        ++age;
        if(curved){active=curve.tick(width,player_x,player_y);if(curve.dynamic&&!curve.nodes.empty())angle=curve.nodes.front().angle;return;}
        if(straight){ // 0x52e0c0: grow to target, then move; optional reach cap.
            change_sounds+=straight_changes.tick(source.changes,source.speed,angle,straight_bounds_grace,cancel_lock,[&]{
                auto children=reflections();
                if(reflected)for(auto&child:children)reflected->push_back(std::move(child));
                return !straight_changes.reflecting;
            });
            if(length>=straight_target){
                travelled+=source.speed;x+=straight_changes.vx;y+=straight_changes.vy;
                if(travel_cap>0&&travelled+length>travel_cap){length=travel_cap-travelled;straight_target=length;if(length<=0){active=false;return;}}
            }else length=std::min(straight_target,length+source.speed);
            if(offscreen_grace<=0&&straight_bounds_grace<=0){auto outside=[&](float px,float py){return px+width<=-148||px-width>=148||py+width<=0||py-width>=448;};
                if(outside(x,y)&&outside(x+std::cos(angle)*length,y+std::sin(angle)*length))active=false;}
            if(offscreen_grace>0)--offscreen_grace;
            if(straight_bounds_grace>0)--straight_bounds_grace;
            return;
        }
        if(fragment){ // 0x52e0c0: moving remnant, capped by original remaining reach.
            travelled+=8;x+=std::cos(angle)*8;y+=std::sin(angle)*8;
            if(travel_cap>0&&travelled+length>travel_cap)length=travel_cap-travelled;
            if(length<=0)active=false;return;
        }
        // 52b550 grows only while below the target. Longer initial beams keep their length.
        if(length<source.max_length)length=std::min(source.max_length,length+source.speed);
        // Original 0x52b550: separate timer reset at each phase transition.
        if(phase==3&&clock>=source.warning){phase=4;clock=0;}
        else if(phase==4){if(clock<source.grow)width=source.width*clock/std::max(1,source.grow);
            else{phase=2;clock=0;width=source.width;}}
        if(phase==2&&clock>=source.hold){phase=5;clock=0;}
        if(phase==5){if(clock>=source.shrink)active=false;
            else width=source.width*(1-float(clock)/std::max(1,source.shrink));}
        ++clock;
    }
};

// Minimal game-object world for the th19 versus engine: enemies, bullets, spirits.
// The ECL VM's >=300 game-object instructions dispatch here.
struct Enemy {
    u32 id=0;
    bool active=true;
    i32 sub=-1;                 // index into the shared enemy ECL program
    float x=0,y=0;
    float vx=0,vy=0,angle=0,speed=0;
    bool mirrored=false,polar=false;
    // ECL 408: angle/speed become orbital phase/angular speed; +0x20/+0x24
    // in the native movement record hold radius/radial speed.
    bool orbit=false;
    bool relative_ellipse=false;
    float relative_phase=0,relative_omega=0,relative_radius=0,relative_radial=0;
    float relative_rotation=0,relative_ratio=1,relative_x=0,relative_y=0,relative_center_x=0,relative_center_y=0;
    bool ellipse=false;
    float ellipse_angle=0,ellipse_ratio=1;
    float orbit_x=0,orbit_y=0,radius=0,radial_speed=0;
    struct Motion {int duration=0,elapsed=0,mode=0;float from=0,to=0;} turn,accelerate;
    Motion move_x,move_y,scale_x,scale_y;
    float sx=1,sy=1;
    float bound_x=0,bound_y=0,bound_w=0,bound_h=0;
    int native_update_callback=0;
    // Set by the retail 0x509030 aura-disable dispatch.  This is kept
    // separately because pure spirits use several different kind bits.
    bool spirit_aura_exit=false;
    void apply_suika_steering(unsigned controls,float target_x,float target_y){
        if(native_update_callback!=3)return;
        const float pi=3.141592741f;
        float target=std::atan2(target_y-y,target_x-x);
        // The native callback adds the side movement quarter-turn only when
        // the corresponding opponent-direction flag is present.
        target+=((controls&64)?pi*.25f:(controls&128)?-pi*.25f:0.f);
        float heading=std::atan2(vy,vx);
        angle=std::remainder(heading+(target-heading)*.05f,2*pi);
    }

    bool recoil_pending=false;
    // ECL624(1), EXE 0x4fc120: one normalized displacement away from the
    // player for a nonzero damage accumulator, including HP-protected actors.
    void apply_hit_recoil(float playerX,float playerY){
        if(native_update_callback!=1||!recoil_pending)return;
        recoil_pending=false;
        float dx=playerX-x,dy=playerY-y,d=std::hypot(dx,dy);
        if(d>=.01f){dx/=d;dy/=d;}
        x-=dx;y-=dy;
    }
    bool directional=false;
    int directional_base=0; // Enemy+280: written by ECL306, independent from ECL303 current pose (+27c).
    int directional_file=0; // native Enemy+278, saved by ECL306
    int animation_direction=0;
    // 0x50a470: six direction transitions, with +/-0.03 horizontal threshold.
    // Offsets 3/4 are return-to-idle scripts, not the rightward moving script.
    void update_direction_animation(){update_direction_animation(vx);}
    void update_direction_animation(float horizontal_displacement){
        if(!directional)return;
        const int next=horizontal_displacement<-.03f?-1:horizontal_displacement>.03f?1:0;
        if(next==animation_direction)return;
        const int offset=animation_direction==-1?(next==0?3:2):
                         animation_direction==0?(next==-1?1:2):(next==0?4:1);
        animation_direction=next;animations[0].script=directional_base+offset;
        animations[0].file=directional_file;++animations[0].generation;
    }
    std::array<i32,4> ecl_ints{};
    std::array<float,4> ecl_floats{};
    std::unordered_map<int,LaserShooter> laser_shooters;
    std::array<int,16> change_append{}; // native +4e2c, NOT copied by ECL614
    i32 death_script=0,death_file=1;bool death_initialized=false;
    std::string death_callback; // ECL550, outer Enemy+0x6818 (one-shot).
    bool death_pending=false;
    BossPhaseTriggers phases;std::string pending_phase;
    int boss_slot=0; // EnemyInf +0x5180; valid while status_flags bit 0x80 is set.
    i32 age=0;
    float invulnerability_time=0; // ECL515 native timer +0x5128; integer value truncates float clock.
    int cpu_target_cooldown=0;
    int card_damage_frame=-1,card_damage_sum=0;
    int damage_budget_frame=-1,damage_budget_used=0;
    int hit_cooldown=0,hit_sound=-1;bool hit_pending=false;
    u32 feedback_color=0; // zero: use original ANM color, otherwise native secondary color
    int phase_health()const{for(const auto&t:phases.entries)if(t.hp>=0)return life-t.hp;return life;}
    float slow_clock=0;
    i32 life=0,item=0,score=0;
    bool spell_damage_scaled=false; i32 life_scale_anchor=0;
    i32 life_reset_4=0,life_reset_8=0,life_reset_c=0; // native HP record +4/+8/+c
    i32 combo_frames=20;        // EnemyInf +0x5120, ECL 1032; native 0x50ac4a
    bool script_initialized_this_tick=false;
    i32 sprite=-1,kind=0;        // sprite = main sprite (slot 0); kind from ins_1027
    u32 flags=0;                // enemy flags: ins_502 sets (bitmask), ins_503 clears
    u32 status_flags=0;         // EnemyInf+0x5170, distinct from +0x516c above
    std::vector<std::string> animation_files; // source ECL ANIM table, retained across fields
    std::array<std::string,8> callbacks; // ECL521, outer Enemy+0x6400 stride0x88
    i32 anm_slot=0;             // ANM file slot selected by ins_302
    float hitbox_w=0,hitbox_h=0; // ins_500/501 (ff: w,h)
    float contact_w=0,contact_h=0,contact_angle=0; // native +128/+12c/+130
    // Native 0x4fe980 indexes sixteen handles directly, not a bitmask.
    struct AnimationSlot {i32 script=-1,file=0;float rotation=0;bool rotation_set=false;u32 generation=0; i32 interrupt_label=0;u32 interrupt_generation=0;};
    std::array<AnimationSlot,16> animations{};
    float spirit_type=32;       // death-chain radius, ECL 1028 -> EnemyInf+0x5110
    bool spirit_active=false;   // armed flag
    i32 spirit_params[5]={10,10,30,20,10}; // 1029 damage, 1030 secondary, 1031 duration; 1032 stored separately
    i32 attack_rank=-1; // EnemyInf +0x5168; 1034/1035 select opposite player's attack level.
    EclInstance vm;  // the sub's VM (owns host_id=id)
};
struct Bullet {
    u32 id=0;
    float x=0,y=0,vx=0,vy=0;
    i32 sprite=0,kind=0;
    int color=0;bool reflected=false,scripted=false;
    int age=0;u32 last_tsukasa_id=0; // Bullet+104c / +1070; preserved across morphs.
    BulletChanges changes;int change_sound=-1;
    BulletGraze graze;float graze_x=0,graze_y=0;u32 graze_tint=0;
    AnmVm animation,attached_effect;float visual_heading=0;int native_timer=0;
    int state=1,cancel_lock=0,cancel_script=-1;float hit_width=0;u32 flags=0;
};
struct Spirit {
    u32 id=0;
    float x=0,y=0;
    float vx=0,vy=0;            // flies toward the player
    i32 type=0;                 // animal-spirit appearance (10/20/30/32/80...)
    bool active=false;
};
struct Pickup {u32 id=0;float x=0,y=0,vx=0,vy=-2.f,speed=0;int type=9,state=1;bool active=true;};
struct DeathChain {float x=0,y=0,target=32,radius=1;int age=0,duration=30,damage=10,type=0;};
struct ChainBulletTransit {float x=0,y=0,speed=0;int kind=0,color=0,frame=0;};
struct BulletCancel {float x=0,y=0;int script=-1;};
struct EnemyDeath {float x=0,y=0,angle=0;int script=37,sound=3,file=1;};

class GameWorld {
public:
    unsigned opponent_controls=0;

    void attach_program(const EclProgram& p){programs_.push_back(&p);}
    void attach_enemy_program(const EclProgram& p) noexcept {enemy_prog_=&p;}
    const EclProgram& enemy_program() const {return *enemy_prog_;}

    Enemy& spawn_enemy(const std::string& sub_name,float x,float y,i32 life,i32 item,i32 score,
                      const std::vector<const EclProgram*>* source_programs=nullptr);
    // 0x50d8a0/0x50d800: receiving world, sender program set, life200.
    bool receive_charge_attack(int tier,float x,float y,const std::vector<const EclProgram*>& source);
    bool fire_bullets(const LaserShooter& shooter,float origin_x,float origin_y);
    // Retail 1.10c BulletInf clamps its pool to 0x7d0 (0x4edfd3 / 0x4ee194).
    static constexpr size_t bullet_capacity=2000;
    u32 dropped_bullets=0;
    Bullet& spawn_bullet();
    void tick_bullet_graze(float px,float py,float player_radius=3);
    struct ExtraFlight {
        u32 id=0;int character=-1,age=0,duration=0,life=200,script=-1;
        float x=0,y=0,source_x=0,source_y=0,tangent_start=0,tangent_end=0;
        std::string sub,file;std::vector<const EclProgram*> programs;
    };
    std::vector<ExtraFlight> extra_flights;
    ReflectionRng extra_rng;
    int extra_arrivals=0;
    bool queue_extra_attack(int character,float x,float y,int variant,const std::vector<const EclProgram*>& source);
    bool receive_spirit(const SpiritArrival& arrival,float x,float y);
    void set_spirit_kind(Enemy& enemy,int kind,bool purified);
    int counted_spirits() const;
    ChargeRegion::Point nazrin_seek_target(float x,float y,float fallback_x,float fallback_y) const;
    std::vector<SpiritRequest> spirit_requests;
    std::vector<ChainBulletTransit> chain_bullet_transits;
    SpiritMeter pure_spirit_meter;
    struct CounterBulletRequest {float x,y;};
    std::vector<CounterBulletRequest> counter_bullet_requests;
    int spirit_rank=0,spirit_rank_cap=0; // shared native rank source still pending
    // Explicit native configuration; unknown (-1) retains only verified
    // mode-independent rewards rather than guessing story progression.
    int reward_mode=-1,reward_stage=0,battle_frame=0;
    int pending_native_attack_credit=0;
    float attack_credit_x=0,attack_credit_y=0;
    std::vector<int> native_extra_reservations;
    unsigned native_extra_sequence=0; // Sanae 53df80 alternates left/right arrivals.
    int send_native_extra_credit(GameWorld& target,const std::vector<const EclProgram*>& source,int character=5);
    int pending_story_credit=0,story_boss_phase=0,story_final_boss=0;
    int boss_escape_result=0; // ECL1020 / native player enemy-manager +0x4acc
    bool opponent_defeat_requested=false; // ECL1036, consumed by player lifecycle
    Spirit& spawn_spirit(float x,float y);

    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    const std::unordered_map<i32,AnmScriptLoc>* bullet_scripts=nullptr;
    const std::unordered_map<i32,std::array<float,2>>* bullet_sprite_sizes=nullptr;
    std::vector<InfiniteLaser> lasers;
    std::vector<Spirit> spirits;
    std::vector<Pickup> pickups;
    std::vector<EnemyDeath> deaths;
    int defeated=0,collected=0,combo=0,combo_timer=0,combo_peak=0,pickup_rate=100,pickup_fraction=0,graze_fraction=0;
    float pickup_speed=5,pickup_range=70;
    int combo_display=0,combo_display_timer=0;
    ComboAttack combo_attack;
    int incoming_extra_level=0,incoming_boss_level=0;
    // Native manager +0x54..+0x90: enemy IDs, not a count or phase variable.
    std::array<u32,16> boss_slots{};
    u32 boss_control_flags=0; // manager +0x94; ECL512 clears only bit 0.
    bool boss_occupied() const {for(u32 id:boss_slots)if(id)return true;return false;}
    bool register_boss(Enemy& enemy,int slot);
    float player_x=0,player_y=400,opponent_x=0,opponent_y=400;
    // Native field manager +0x10..+0x1c, shared by every enemy VM in this field.
    std::array<i32,4> field_ints{};
    int pending_energy=0;
    struct Spell {u32 host_id=0;bool active=false;int number=0,timeout=0,flags=0;u32 generation=0;std::string name_bytes,animation_file;} spell;
    i64 earned_score=0;
    bool card_near_damage=false;
    int player_damage_cap=0; // SHT+0x28; zero for worlds without a player SHT.
    bool damage_enemy(Enemy& enemy,int damage,bool forced=false);
    void finish_enemy_deaths();
    void finish_hit_feedback(u32 native_mode_flags=0);
    int cancel_bullets_circle(float x,float y,float radius,int limit=99999,bool respect_lock=true);
    int cancel_lasers_region(const std::function<bool(float,float)>& inside,bool respect_lock=true,
        const std::function<bool(float,float,float)>& rectangleSample={});
    void cancel_enoko_trap(float x,float y,float angle);
    int cancel_bullets_shape(const std::function<bool(float,float,float)>& contact);
    int cancel_bullets_polygon(float x,float y,float radius,float angle,int sides);
    int cancel_bullets_rect(float x,float y,float width,float height,float angle=0);
    int cancel_lasers_circle(float x,float y,float radius,bool respect_lock=true);
    void tick_death_chains();
    std::vector<DeathChain> death_chains,death_chain_visuals; // one-shot ANM creation events
    std::vector<BulletCancel> bullet_cancels;
    int cancelled_bullets=0;
    int grazed_bullets=0;
    std::vector<GrazeParticle> graze_particles;ReflectionRng graze_rng;
    int purify_orin(float x,float y,float radius);
    int convert_zanmu(float x,float y,float radius,int& ordinary_count);
    void tick_pickups(float player_x,float player_y);
    ReflectionRng bullet_rng; // Native algorithm; shared replay seed is not wired yet.
    u32 next_id=1;
    u32 last_spawned_id=0;

    WaveSchedule wave_schedule;
    std::unordered_map<i32,u32> wave_counts;
    i32 current_wave=0;
    // Runtime audit: skipped operations remain visible instead of being
    // mistaken for fully reconstructed gameplay.
    std::unordered_map<u16,u32> unhandled_opcodes;
    // 1013/1015: eight integer values per difficulty-dependent parameter row.
    // These include enemy HP; they are not animation sprite sequences.
    i32 difficulty=0;
    std::unordered_map<i32,std::array<i32,8>> difficulty_values;
    // current spirit type set by ins_502+1034 (pending reverse); 1037 uses this
    i32 current_spirit_type=0;
    bool message_active=false,message_signal=false;
    std::vector<ScreenShakeRequest> screen_shake_events;
    std::vector<i32> sound_events;
    ChargeRegion charge_region;
    void strengthen_tsukasa_bullets(const Enemy& e);
    void apply_bullet_changes(Bullet& bullet);
    void tick_wave();  // advance enemy VMs + their children

private:
    std::vector<const EclProgram*> programs_;   // full program set (root + includes)
    const EclProgram* enemy_prog_=nullptr;
};
}
