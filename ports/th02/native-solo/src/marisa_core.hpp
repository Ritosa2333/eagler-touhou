#pragma once
#include "midboss1_core.hpp"
#include "collision_display.hpp"

namespace soew {
// Fourth-stage boss logic; attack and effect ordering refer to th02_main.asm.
// Presentation, RNG and projectile allocation are owned by the scene adapter.
struct MarisaCore {
    enum class Phase { Opening, DeployOrbs, ArcFlight, AttackWait, ExposedAttack, ExpandingOrbs, OrbVolley, StarRain, SplitOrbits, OrbRelease };
    int flight_x=0,flight_y=0,flight_direction=0;
    std::uint8_t flight_angle=0;
    int pending_attack=0,approach_dx=0,approach_dy=0;
    std::uint8_t drift_angle=2;
    int rain_side=1;
    unsigned shielded_selections=0,exposed_selections=0,rounds=0;
    struct Orb {
        int x=0,y=0,radius=0;std::uint8_t angle=0;
        bool active=false,dying=false,hit_flash=false;
        BossDamage hits;
        unsigned death_frame=0,death_pattern=10;
        int angular_step=2;
        std::uint8_t volley_offset=0;
    };
    std::array<Orb,4> orbs{};
    static constexpr unsigned opening_length=130;
    static constexpr unsigned deployment_end=154,attack_wait=31,shielded_attack_count=4,
        exposed_attack_count=2,round_limit=7,release_frame=110,orb_retirement_frames=48;
    // End frames for dispatch IDs 1…5 and -1…-3, respectively.
    static constexpr std::array shielded_end{266u,510u,300u,200u,420u};
    static constexpr std::array exposed_end{130u,193u,130u};
    struct Hooks : Midboss1Core::Hooks {
        // Orb damage uses the original Bomb mask 3 (separate from body damage).
        std::function<unsigned(int,int,int,int)> orb_damage;
        std::function<unsigned()> random8,random16;
        // Shared special-bullet drift angle, speed and duration.
        std::function<void(unsigned,unsigned,unsigned)> drift;
        std::function<void(int,int,unsigned)> item;
        std::function<void()> player_hit;
        std::function<void(unsigned)> invincibility;
        CollisionFrame *collision_display=nullptr;
    };
    BossDamage damage_taken;
    int target_x=-1,target_y=-1;
    bool body_damage_enabled=false,body_flash=false,defeated=false;
    struct DeathEffect {int x,y;unsigned frame;std::uint8_t angle;};
    std::array<DeathEffect,3> death_effects{};
    unsigned death_effect_count=0,death_frame=0,death_pattern=0;
    bool death_zoom=false,cleared=false;
    std::uint8_t death_angle=0xe0;
    bool terminal_death_frame() const {return defeated&&cleared&&death_frame==100&&death_effect_count!=0;}
    struct Player {int front_x,front_y;unsigned power;int x=-32768;unsigned rank=1;};
    int left=176,top=64;
    unsigned phase_frame=0,pattern=128;
    Phase phase=Phase::Opening;
    std::uint8_t angle=0;

    void update(const Hooks &h){update(h,Player{-32768,-32768,0});}
    void update(const Hooks &h,const Player &player){
        death_effect_count=0;
        if(cleared)return;
        if(defeated){advance_death(h);return;}
        const bool exposed=sample_body_target();
        ++phase_frame;
        const auto pose=[&](unsigned cel,unsigned sound){pattern=cel;if(h.sound)h.sound(sound);};
        if(phase==Phase::ArcFlight||phase==Phase::AttackWait||phase==Phase::ExposedAttack||phase==Phase::ExpandingOrbs||phase==Phase::OrbVolley||phase==Phase::StarRain||phase==Phase::SplitOrbits||phase==Phase::OrbRelease){
            if(phase==Phase::ArcFlight)advance_flight(h,player,exposed);
            else if(phase==Phase::ExposedAttack)advance_exposed_attack(h,player,pose);
            else if(phase==Phase::ExpandingOrbs)advance_expanding_orbs(h,player,exposed,pose);
            else if(phase==Phase::OrbVolley)advance_orb_volley(h,player,exposed,pose);
            else if(phase==Phase::StarRain)advance_star_rain(h,player,exposed,pose);
            else if(phase==Phase::SplitOrbits||phase==Phase::OrbRelease)advance_split_or_release(h,player,exposed,pose);
            else{
                advance_approach(h,player);
                if(phase_frame>=attack_wait)select_attack(h,exposed);
            }
            update_body(h,player,exposed);
            update_orbs(h,player);
            if(defeated)advance_death(h);
            return;
        }
        if(phase==Phase::DeployOrbs){
            if(phase_frame==100)pose(130,9);
            if(phase_frame==130){
                for(unsigned i=0;i<orbs.size();++i)orbs[i]={left+32,top+32,8,std::uint8_t(i*64),true};
            }else if(phase_frame==deployment_end){
                pattern=128;phase_frame=0;phase=rounds==0?Phase::ArcFlight:Phase::AttackWait;
            }else if(phase_frame>130){
                for(auto &orb:orbs)orb.radius+=2;
            }
            update_body(h,player,exposed);
            update_orbs(h,player);
            if(defeated)advance_death(h);
            return;
        }
        switch(phase_frame){
        case 50:angle=24;pose(130,9);break;
        case 100:pose(132,10);break;
        case 108:pose(134,10);break;
        case 116:pose(136,10);break;
        case opening_length:pattern=128;phase_frame=0;phase=Phase::DeployOrbs;break;
        default:break;
        }
        if(phase_frame>100&&phase_frame%3==0){
            for(int dx:{36,52})if(h.bullet){
                EnemyBulletRequest r;r.left=left+dx;r.top=top+64;
                r.angle=angle;r.group=0x20;r.speed=60;
                h.bullet(r);
            }
            angle=std::uint8_t(angle+10);
        }
        update_body(h,player,exposed);
        if(defeated)advance_death(h);
    }
    unsigned random_byte(const Hooks &h)const{
        if(!h.random8)throw std::runtime_error("Marisa attack RNG unavailable");
        return h.random8()&255u;
    }
    // ASM 23888–23996: move along a radius-112 arc and sample one orb every 16 ticks.
    void advance_flight(const Hooks &h,const Player &player,bool exposed){
        if(phase_frame<10)return;
        if(phase_frame==10){
            const int player_x=player.x==-32768?player.front_x:player.x;
            flight_direction=left+32<player_x?1:-1;
            flight_x=left;flight_y=top+112;flight_angle=192;
        }
        flight_angle=std::uint8_t(flight_angle+flight_direction);
        const auto offset=enemy_vector(flight_angle,112);
        left=flight_x+offset[0];top=flight_y+offset[1];
        if(phase_frame%16==0){
            const auto i=random_byte(h)&3u;
            const auto pellet=[&](int x,int y,unsigned group){
                if(!h.bullet)return;
                EnemyBulletRequest r;r.left=x;r.top=y;r.group=std::uint8_t(group);r.speed=80;h.bullet(r);
            };
            const auto &orb=orbs[i];
            if(orb.active&&!orb.dying)pellet(orb.x+12,orb.y+12,0x19);
            if(exposed&&i==0)pellet(left+64,top+16,0x0a);
        }
        if(phase_frame==shielded_end[0]){phase_frame=0;phase=Phase::AttackWait;}
    }
    // ASM 25175–25237: the waiting state resets the next attack timer to 1.
    void select_attack(const Hooks &h,bool exposed){
        if(!exposed){
            if(shielded_selections>=shielded_attack_count)pending_attack=6;
            else{pending_attack=int(random_byte(h)%5)+1;++shielded_selections;}
        }else if(exposed_selections>=exposed_attack_count){
            shielded_selections=exposed_selections=0;pending_attack=0;
            ++rounds;
            if(rounds>=2)body_damage_enabled=true;
            if(!h.lock_timeout&&rounds>=round_limit)defeated=true;
        }else{
            ++exposed_selections;pending_attack=-1-int(random_byte(h)%3);
        }
        phase_frame=1;
        // Dispatcher case 1 reuses the opening arc (ASM 25140–25143, 25290).
        if(pending_attack==1)phase=Phase::ArcFlight;
        else if(pending_attack==0)phase=Phase::DeployOrbs;
        else if(pending_attack<0)phase=Phase::ExposedAttack;
        else if(pending_attack==2)phase=Phase::ExpandingOrbs;
        else if(pending_attack==3)phase=Phase::OrbVolley;
        else if(pending_attack==4)phase=Phase::StarRain;
        else if(pending_attack==5)phase=Phase::SplitOrbits;
        else phase=Phase::OrbRelease;
    }
    template<class Pose>
    void advance_star_rain(const Hooks &h,const Player &player,bool exposed,const Pose &pose){
        // ASM 24682–24900. Preserve the eight-/sixteen-bit RNG call order.
        const auto f=phase_frame;
        if(f==50){pose(130,9);if(h.drift)h.drift(unsigned(std::uint16_t(rain_side)),0,64);rain_side=-rain_side;}
        if(f==50||f==70||f==80||f==shielded_end[3]){
            const int step=f==70?3:f==80?4:2;
            for(unsigned i=0;i<orbs.size();++i)orbs[i].angular_step=i%2?-step:step;
        }
        if(f==150)pattern=130;
        if(f==156)pattern=132;
        if(f==162)pattern=134;
        if(f==170)pattern=136;
        if(f==180)pattern=128;
        if(f==shielded_end[3])phase_frame=0;
        if(f>70&&f<shielded_end[3]&&f%14==0){
            if(h.sound)h.sound(10);
            const auto star=[&](int x,int y,unsigned angle,unsigned group,unsigned speed){
                if(!h.bullet)return;
                EnemyBulletRequest r;r.left=x;r.top=y;r.angle=std::uint8_t(angle);r.group=std::uint8_t(group);
                r.speed=speed;r.kind=EnemyBulletRequest::Bullet16;r.pattern=85;h.bullet(r);
            };
            if(f<=120){
                for(const auto &orb:orbs)if(orb.active&&!orb.dying)star(orb.x+8,orb.y+8,192,0xff,70);
                star(left+40,top+40,192,0xff,70);
            }else{
                if(!h.random16)throw std::runtime_error("Marisa star rain RNG16 unavailable");
                for(unsigned i=0;i<(player.rank==0?2u:4u);++i){
                    const unsigned a=unsigned(64+16*rain_side)+(random_byte(h)&7u);
                    const int x=32+int((h.random16()&65535u)%368);
                    star(x,16,a,0xff,16+(random_byte(h)&31u));
                    const int y=16+int((h.random16()&65535u)%320);
                    star(rain_side==1?400:32,y,a,0xff,16+(random_byte(h)&31u));
                }
            }
            if(exposed)for(unsigned i=0;i<4;++i)star(left+40,top+40,i*64+2*f,0x85,70);
        }
        advance_approach(h,player);
        if(!phase_frame)phase=Phase::AttackWait;
    }
    template<class Pose>
    void advance_split_or_release(const Hooks &h,const Player &player,bool exposed,const Pose &pose){
        const auto f=phase_frame;
        const auto fire=[&](int x,int y,int a,unsigned speed,bool star=false){
            if(!h.bullet)return;
            EnemyBulletRequest r;r.left=x;r.top=y;r.angle=std::uint8_t(a);r.group=star?0xff:0x20;r.speed=speed;
            if(star){r.kind=EnemyBulletRequest::Bullet16;r.pattern=85;}
            h.bullet(r);
        };
        if(phase==Phase::OrbRelease){
            // ASM 24389–24541: wait for the released orbs to finish dying.
            if(f==10)for(auto &orb:orbs)orb.angular_step=-3;
            if(exposed){phase_frame=0;pattern=128;}
            else if(f==50){for(auto &orb:orbs)orb.angular_step=-4;pose(130,9);}
            else if(f==100||f==release_frame){
                if(h.sound)h.sound(3);
                for(auto &orb:orbs)if(orb.active&&!orb.dying){
                    if(f==100)for(int a=-64;a<64;a+=16)fire(orb.x+12,orb.y+12,orb.angle+a,70);
                    else{
                        for(int a=-70;a<70;a+=12)fire(orb.x+12,orb.y+12,orb.angle+a,16+(random_byte(h)&31u));
                        orb.dying=true;orb.hit_flash=false;
                    }
                }
            }
        }else{
            // ASM 24190–24382: even/odd orbit pairs use different radii.
            if(f==10)for(unsigned i=0;i<4;++i)orbs[i].angular_step=i%2?-2:3;
            if(f>=50){
                if(f<130){orbs[0].radius+=2;orbs[2].radius+=2;}
                if(f>=260&&f<340){orbs[0].radius-=2;orbs[2].radius-=2;if(player.rank){++orbs[1].radius;++orbs[3].radius;}}
                if(f>=340&&f<420&&player.rank){--orbs[1].radius;--orbs[3].radius;}
                if(f==140){pose(130,9);angle=0;}
                if(f==170)pattern=130;
                if(f==176)pattern=132;
                if(f==182)pattern=134;
                if(f==190)pattern=136;
                if(f==200)pattern=128;
                if(f==shielded_end[4]){phase_frame=0;pattern=128;for(unsigned i=0;i<4;++i)orbs[i].angular_step=i%2?-2:2;}
                if(exposed){pattern=130;angle=std::uint8_t(int(angle)-24+int(player.rank));fire(left+64,top+16,angle,60,true);}
                else if(f>=150&&f<250&&f%4==0){
                    angle=std::uint8_t(angle+3);if(h.sound)h.sound(10);
                    for(unsigned i:{0u,2u}){const auto &orb=orbs[i];
                        if(orb.active&&!orb.dying&&orb.x>0&&orb.x<416&&orb.y>=0&&orb.y<=368)fire(orb.x+12,orb.y+12,orb.angle+angle,70);
                    }
                }
            }
        }
        advance_approach(h,player);
        if(!phase_frame)phase=Phase::AttackWait;
    }
    unsigned volley_index=0;
    template<class Pose>
    void advance_orb_volley(const Hooks &h,const Player &player,bool exposed,const Pose &pose){
        // ASM 24548–24675: use pre-movement orb coordinates and reverse at 300.
        const auto f=phase_frame;
        if(f==50){pose(130,9);volley_index=0;for(auto &orb:orbs)orb.volley_offset=0;}
        if(f==150)pattern=130;
        if(f==156)pattern=132;
        if(f==162)pattern=134;
        if(f==170)pattern=136;
        if(f==180)pattern=128;
        if(f==shielded_end[2]){phase_frame=0;for(auto &orb:orbs)orb.angular_step=-orb.angular_step;}
        if(phase_frame>70&&f%5==0){
            if(h.sound)h.sound(10);
            const auto fire=[&](int x,int y,unsigned a,unsigned group,unsigned speed){
                if(!h.bullet)return;
                EnemyBulletRequest r;r.left=x;r.top=y;r.angle=std::uint8_t(a);
                r.group=std::uint8_t(group);r.speed=speed;h.bullet(r);
            };
            for(auto &orb:orbs)if(orb.active&&!orb.dying){
                fire(orb.x+12,orb.y+12,orb.angle+orb.volley_offset,0x20,25+f/4);
                orb.volley_offset=std::uint8_t(orb.volley_offset+2*orb.angular_step);
            }
            if(exposed&&volley_index%2==0)fire(left+44,top+44,f,0x25,50);
            ++volley_index;
        }
        advance_approach(h,player);
        if(!phase_frame)phase=Phase::AttackWait;
    }
    template<class Pose>
    void advance_expanding_orbs(const Hooks &h,const Player &player,bool exposed,const Pose &pose){
        // ASM 24003–24182: expand and contract the orbit, with a short
        // shielded sweep and a sustained spiral after the shield is gone.
        const auto f=phase_frame;
        if(f>=50&&f<shielded_end[1]){
            for(auto &orb:orbs)orb.radius+=f<280?1:-1;
            if(f==220){pose(130,9);angle=0;}
            if(f==250)pattern=130;
            if(f==256)pattern=132;
            if(f==262)pattern=134;
            if(f==270)pattern=136;
            if(f==280)pattern=128;
            if(exposed)pattern=130;
            if(exposed||(f>=250&&f<270)){
                angle=std::uint8_t(angle+(exposed?10:13));
                if(h.bullet&&(exposed||player.rank>=(f&1u))){
                    EnemyBulletRequest r;r.left=left+(exposed?64:44);r.top=top+(exposed?16:64);
                    r.angle=angle;r.group=0x20;r.speed=exposed?60:50;h.bullet(r);
                }
            }
        }else if(f>=shielded_end[1]){phase_frame=0;pattern=128;}
        advance_approach(h,player);
        if(phase_frame==0)phase=Phase::AttackWait;
    }
    template<class Pose>
    void advance_exposed_attack(const Hooks &h,const Player &player,const Pose &pose){
        // ASM 23534–23715, 24907–24971 and the signed dispatch table at 25286.
        const auto f=phase_frame;
        const auto fire=[&](int dx,int dy,unsigned a,unsigned group,unsigned speed,unsigned cel=0){
            if(!h.bullet)return;
            EnemyBulletRequest r;r.left=left+dx;r.top=top+dy;r.angle=std::uint8_t(a);
            r.group=std::uint8_t(group);r.speed=speed;
            if(cel){r.kind=EnemyBulletRequest::Bullet16;r.pattern=std::uint8_t(cel);}
            h.bullet(r);
        };
        if(pending_attack==-2){
            if(f==50)pose(130,9);
            if(f>=50&&f%24==0){
                fire(64,16,0,player.rank==0?0x18:0x17,54);
                fire(64,16,0,player.rank==0?0x12:0x11,60);
            }
            if(f>=exposed_end[1]){pattern=128;phase_frame=0;}
        }else{
            if(f==50){
                pose(130,9);
                if(pending_attack==-3){
                    angle=std::uint8_t(random_byte(h));
                    if(h.drift)h.drift(drift_angle,2,128);
                    drift_angle=std::uint8_t(0u-drift_angle);
                }else angle=24;
            }
            if(f==100)pose(132,10);
            if(f==108)pose(134,10);
            if(f==116)pose(136,10);
            if(f==exposed_end[0]){pattern=128;phase_frame=0;}
            else if(pending_attack==-3&&f>=100&&f%4==0){
                fire(44,64,angle,0x84,40,85);angle=std::uint8_t(angle+43);
            }else if(pending_attack==-1&&f>100&&f%3==0){
                fire(44,64,angle,0x20,50);angle=std::uint8_t(angle+10);
            }
        }
        advance_approach(h,player);
        if(phase_frame==0)phase=Phase::AttackWait;
    }
    void advance_approach(const Hooks &h,const Player &player){
        if(phase_frame==2){
            const int player_x=player.x==-32768?player.front_x:player.x;
            approach_dx=left+32<player_x?1:-1;
            approach_dy=top<72?1:top>108?-1:1-int(random_byte(h)%3);
        }
        if(phase_frame!=1&&phase_frame<50){left+=approach_dx;top+=approach_dy;}
    }
    void advance_death(const Hooks &h){
        // ASM 22874–22974: angle changes apply between the ordered calls.
        const auto t=death_frame;
        const auto effect=[&](unsigned frame){
            death_effects[death_effect_count++]={left+40,top+40,frame,death_angle};
            if(h.sound&&frame==1)h.sound(18);
            if(h.sound&&frame==32)h.sound(14);
        };
        effect(t);
        if(t>=24){if(t==56)death_angle=0x20;effect(t-24);}
        if(t>=48){if(t==80)death_angle=0;effect(t-48);}
        if(t>=64&&t%16==0&&h.spark)h.spark({left+48,top+48,136,24,true});
        death_zoom=t>=31;
        if(death_zoom)death_pattern=unsigned(10+((int(t)-32)>>4));
        if(++death_frame==100){cleared=true;death_zoom=false;}
    }
    bool sample_body_target(){
        // ASM 24984–25011: sample orb states before attacks or collisions.
        for(const auto &orb:orbs)if(orb.active){target_x=target_y=-1;return false;}
        target_x=left+40;target_y=top+40;return true;
    }
    void update_body(const Hooks &h,const Player &player,bool exposed){
        // ASM 22655–22734: still consume collisions while shielded.
        body_flash=false;
        if(defeated)return;
        const auto dealt=h.damage?h.damage(left+24,top+24,48,48):0u;
        if(dealt){
            if(h.sound)h.sound(exposed?4:11);
            if(exposed){
                body_flash=true;
                if(body_damage_enabled)damage_taken.add(dealt,h.damage_divisor);
            }
            if(enemy_s16(damage_taken)>=900){
                defeated=true;
                if(h.score)h.score(20000);
                if(h.invincibility)h.invincibility(200);
            }
        }
        const auto bounds=CollisionBounds::open(left-16,top-16,left+48,top+48);
        const CollisionPoint front{player.front_x,player.front_y};
        if(h.collision_display)h.collision_display->hazard(CollisionSource::MarisaBody,bounds,CollisionSample::Front,front);
        if(h.player_hit&&bounds.contains(front))h.player_hit();
    }
    // Shared substep for both deployment and the later attack dispatcher.
    void update_orbs(const Hooks &h,const Player &player){
        // ASM 22741–22867: one hit per nonzero query, not per damage point.
        for(auto &orb:orbs){
            orb.hit_flash=false;
            if(!orb.active||orb.dying)continue;
            if(h.orb_damage&&h.orb_damage(orb.x+4,orb.y,24,32)){
                orb.hit_flash=true;
                orb.hits.add(1,h.damage_divisor);
                if(orb.hits>=110){
                    if(!h.random8)throw std::runtime_error("Marisa orb drop RNG unavailable");
                    if((h.random8()&255u)%80>=player.power&&h.item)h.item(orb.x+8,orb.y+8,3);
                    if(h.sound)h.sound(2);
                    orb.dying=true;orb.hit_flash=false;
                    if(h.score)h.score(5000);
                }
            }
            const auto bounds=CollisionBounds::open(orb.x-16,orb.y-16,orb.x+16,orb.y+16);
            const CollisionPoint front{player.front_x,player.front_y};
            if(h.collision_display)h.collision_display->hazard(CollisionSource::MarisaOrb,bounds,CollisionSample::Front,front);
            if(h.player_hit&&bounds.contains(front))h.player_hit();
        }
        // ASM 22981–23090 runs the death callback on the lethal tick.
        for(auto &orb:orbs)if(orb.dying){
            if(h.spark)h.spark({orb.x+16,orb.y+16,60,2,false});
            orb.death_pattern=10+orb.death_frame/6;
            if(++orb.death_frame==orb_retirement_frames){orb.active=orb.dying=false;orb.hits=orb.death_frame=0;}
        }
        // ASM 23722–23782 updates only live orbs, after collision/death.
        for(auto &orb:orbs)if(orb.active&&!orb.dying){
            orb.angle=std::uint8_t(orb.angle+orb.angular_step);
            const auto offset=enemy_vector(orb.angle,orb.radius);
            orb.x=left+32+offset[0];orb.y=top+32+offset[1];
        }
    }
};
}
