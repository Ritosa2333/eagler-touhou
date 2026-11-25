#pragma once
#include "enemy_effects.hpp"
#include "enemy_math.hpp"
#include <functional>
namespace soew {
// th02_main.asm10651..11206. init AFTER original preboss dialogue;
// the dialogue/scene handoff must be supplied by a separate caller.
struct RikaCore {
    std::array<int,2> left{192,192};
    int cached_x=0,render_y=48;
    std::uint16_t phase_frame=0,damage_taken=0,death_frame=0,attack_frame=0;
    unsigned state=0,pattern=150,draw_pattern=150,rank=1;
    std::uint8_t sweep_angle=0;
    bool dying=false,done=false,visible=false,hit_flash=false,death_zoom=false;
    unsigned death_pattern=0;
    int target_x=-1,target_y=-1;
    struct Hooks {
        std::function<void(const EnemyBulletRequest&)> bullet;
        std::function<void(int,int,int,unsigned,unsigned)> laser;
        std::function<unsigned(int,int,int,int)> damage;
        std::function<void(unsigned)> score,sound,invincibility;
        std::function<void(int,int,int,int,bool)> spark;
        std::function<void(int,int,unsigned)> explosion;
    };
    // cached_x/sweep_angle/death counter are independent original globals.
    // Caller can pass inherited cached_x and sweep byte from prior encounter.
    struct Inherited {int cached_x;std::uint8_t sweep_angle;std::uint16_t phase_frame,death_frame,attack_frame;unsigned scroll_line;};
    void init(unsigned difficulty,const Inherited &old){
        if(difficulty>4||old.scroll_line>=400)throw std::runtime_error("Invalid Rika context");
        *this=RikaCore{};rank=difficulty;cached_x=old.cached_x;sweep_angle=old.sweep_angle;phase_frame=old.phase_frame;death_frame=old.death_frame;attack_frame=old.attack_frame;render_y=int((48+old.scroll_line)%400);
    }
    void begin_frame(unsigned back){if(back>1)throw std::runtime_error("Invalid Rika page");left[back]=left[back^1];}
    void emit(const Hooks &h,int x,int y,unsigned angle,unsigned group,unsigned speed,bool big=false,unsigned pat=0){
        EnemyBulletRequest r;r.left=x;r.top=y;r.angle=angle;r.group=group;r.speed=speed;r.pattern=pat;r.kind=big?EnemyBulletRequest::Bullet16:EnemyBulletRequest::Pellet;if(h.bullet)h.bullet(r);
    }
    void move(unsigned back,const Hooks &h){
        const int phase=enemy_s16(phase_frame);
        if(state==0){
            if(phase<16){if(!(phase_frame&3))emit(h,228,112,rank>=2?phase_frame:0,0x25,40+phase_frame*4);return;}
            if(phase<116)left[back]=enemy_s16(unsigned(left[back]-1));
            else if(phase<316)left[back]=enemy_s16(unsigned(left[back]+1));
            else {if(phase>=416)phase_frame=16;left[back]=enemy_s16(unsigned(left[back]-1));}
        }else if(state==1){if(left[back]<192)left[back]=enemy_s16(unsigned(left[back]+1));else if(phase>192)left[back]=enemy_s16(unsigned(left[back]-1));else state=2;}
        else if(state==2)phase_frame=0;
    }
    void attack(std::uint32_t stage_frame,const Hooks &h){
        const int muzzle=cached_x+28;const auto se=[&](unsigned n){if(h.sound)h.sound(n);};
        if(enemy_s16(damage_taken)<700){state=0;if(!(phase_frame&63)){se(3);emit(h,muzzle,112,0,rank==0?25:11,50);emit(h,muzzle,112,64,21,30);}return;}
        if(enemy_s16(damage_taken)<1400){if(!(stage_frame&31)){sweep_angle=std::uint8_t(sweep_angle+24)%128;se(3);for(unsigned side=0;side<2;++side){int x=cached_x+8+int(side*44);unsigned angle=side?128-sweep_angle:sweep_angle;emit(h,x,112,angle,0x20,54);emit(h,x,112,angle,rank==0?32:2,44);}}return;}
        if(state==0){state=1;attack_frame=0;sweep_angle=0;}
        ++attack_frame;
        if(attack_frame<100){pattern=(attack_frame&7)<4?150:151;if(!(attack_frame&31))se(9);return;}
        if(attack_frame==100){if(h.laser){h.laser(cached_x+2,112,160,111,64);h.laser(cached_x+46,112,160,111,64);}return;}
        if(attack_frame>164&&attack_frame<280){
            if(attack_frame<250)pattern=(attack_frame&7)<4?150:151;
            else if(attack_frame==260)pattern=152;else if(attack_frame==270)pattern=153;
            if(!(attack_frame&15)){se(6);const unsigned group=(attack_frame&63)==0||(attack_frame&63)==32?15:14;emit(h,muzzle,112,64,group,58);}return;
        }
        if(attack_frame>=280&&!(attack_frame&(rank==0?7:3)))emit(h,muzzle,72,1,0x1c,44,true,80);
    }
    void update(unsigned back,std::uint32_t stage_frame,const Hooks &h){
        visible=false;hit_flash=false;death_zoom=false;draw_pattern=pattern;if(done)return;
        target_x=cached_x+24;target_y=48;++phase_frame;
        if(dying){
            // Pascal packed DWORDs decode left,top128,speed50,count2,false.
            if(h.spark)h.spark(cached_x+32,128,50,2,false);
            const int offset=enemy_s16(std::uint16_t(death_frame-32));death_pattern=unsigned(10+offset/8);++death_frame;
            if(enemy_s16(death_frame)>=80){death_frame=0;done=true;return;}
            visible=true;death_zoom=enemy_s16(phase_frame)>=32;if(h.explosion)h.explosion(cached_x+24,112,death_frame);return;
        }
        move(back,h);cached_x=left[back];
        const unsigned dealt=h.damage?h.damage(cached_x,48,64,96):0;
        if(dealt){damage_taken=std::uint16_t(damage_taken+dealt);if(enemy_s16(damage_taken)>2260){dying=true;if(h.score)h.score(20000);if(h.invincibility)h.invincibility(200);}else{hit_flash=true;visible=true;if(h.sound)h.sound(4);}}
        else visible=true;
        // Original calls the attack helper even on the lethal transition tick.
        attack(stage_frame,h);
    }
};
}
