#pragma once
#include "enemy_effects.hpp"
#include "enemy_math.hpp"
#include "collision_display.hpp"
#include <cstddef>
#include <utility>
#include <vector>

namespace soew {
// ReC98 th02_main.asm:13744–15934. Coordinates are screen pixels; phase and
// damage tests compare the original signed 16-bit words. The host owns the
// gameplay frame clock, shared projectile pools, input, and the page presenter.
struct SigmaCore {
    static constexpr int field_left = 32, field_top = 16, field_right = 416, field_bottom = 384;
    static constexpr int center_player_left = 208;
    static constexpr unsigned white = 15;
    static constexpr unsigned single = 0x20, aimed_5 = 0x17, ring_32 = 0x27;
    static constexpr unsigned ring_16 = 0x25, ring_8 = 0x24, aimed_wide_2 = 0x26, bounce_all = 0x87;
    enum class Attack { Rain, TwinBursts, DiagonalRing32, OrbitBursts, DiagonalRing16,
                        BilliardSweep, DiagonalLasers, OpposedBursts, DiagonalBursts,
                        FinalWide, FinalRing8, FinalBursts };
    // Source function table in sigma_update and the cycle lengths passed to sigma_166DE.
    struct Cycle {unsigned phase, threshold, count; std::array<Attack,3> attacks;};
    inline static constexpr std::array<Cycle,5> cycles = {{
        {1,1800,3,{Attack::Rain,Attack::TwinBursts,Attack::DiagonalRing32}},
        {3,1800,2,{Attack::OrbitBursts,Attack::DiagonalRing16,Attack::Rain}},
        {5,1800,2,{Attack::BilliardSweep,Attack::DiagonalLasers,Attack::Rain}},
        {7,1800,2,{Attack::OpposedBursts,Attack::DiagonalBursts,Attack::Rain}},
        {9,5000,3,{Attack::FinalWide,Attack::FinalRing8,Attack::FinalBursts}}
    }};
    struct Paint {
        enum Kind { Dot, Circle, Disc, Box, Sprite } kind;
        int x,y,a,b; // Size/radius/pattern in a; Box uses an inclusive right/bottom in a/b.
        unsigned color;
    };
    struct Hooks {
        std::function<void(const EnemyBulletRequest&)> bullet;
        std::function<unsigned(int,int,int,int)> damage;
        std::function<void(unsigned)> score,sound,turns_max;
        std::function<unsigned()> randring2_next;
        std::function<void()> player_hit;
        std::function<void(int,int,int,unsigned,unsigned)> laser;
        int player_x=208,player_y=336;             // _player_topleft (aiming/motion)
        int front_player_x=208,front_player_y=336; // _player_*_on_page[_page_front] (collision)
        bool reduced=false;
        CollisionFrame *collision_display=nullptr;
    };
    struct Burst {
        // 1: charging sprite, 2: ready (promoted by next page prep), 3: live, 4: imploding.
        std::uint8_t state=0,age=0,pattern=26,radius=64;
        int x=0,y=0,target_radius=0;
    };
    std::array<int,2> left{160,160},top{48,48};
    std::uint16_t damage_taken=0,phase_frame=0;
    std::uint32_t sigma_frame=0;
    unsigned phase=0,subattack=0,pattern=128,draw_pattern=128;
    bool visible=true,hit_flash=false,dying=false,done=false,cycled=false;
    unsigned slowdown=1,shake=0;
    const char *blocked=nullptr;
    std::vector<Paint> paints;
    std::size_t body_paint_index=0;
    std::array<Burst,16> bursts{};
    unsigned decor_radius=0;
    bool decor_visible=false,decor_advanced=false;

    void init(){*this=SigmaCore{};}
    // 14173–14209: copy page positions and promote pending explosions exactly once per tick,
    // not every repaint. Clearing black and actually applying the paints is the host's job.
    void begin_frame(unsigned back){
        if(back>1){blocked="sigma: invalid back page";return;}
        if(blocked||done)return;
        left[back]=left[back^1];top[back]=top[back^1];
        for(auto &b:bursts)if(b.state==2)b.state=3;
    }
    void update(unsigned back,std::uint32_t stage_frame,const Hooks &h){
        if(blocked||done)return;
        if(back>1){blocked="sigma: invalid back page";return;}
        if(!(h.bullet&&h.damage&&h.score&&h.sound&&h.turns_max&&h.randring2_next&&h.player_hit&&h.laser)){
            blocked="sigma: missing gameplay hook";return;
        }
        paints.clear(); body_paint_index=0; visible=false;hit_flash=false;shake=0;
        ++phase_frame;
        const int x=left[back],y=top[back],mx=x+60,my=y+60; // Cached before the attack moves Sigma.
        decor_advanced=(stage_frame&1)==0;
        if(decor_advanced){
            decor_radius=(decor_radius+8)&255;
            decor_visible=true;
        }
        if(dying){
            if(retire(back,x,y,h)){done=true;return;}
        }else{
            if(phase==0){if(frame()>50)start_cycle();}
            else if(phase<=9){
                if(!(phase&1))start_cycle();
                else{
                    const auto &cycle=cycles[phase/2];
                    attack(cycle.attacks[subattack],back,x,y,mx,my,h);
                    // A reset caused by the attack comes before this frame's shot/Bomb damage.
                    if(phase_frame==0){
                        if(++subattack>=cycle.count){subattack=0;cycled=true;}
                        if(enemy_s16(damage_taken)>=int(cycle.threshold)){
                            damage_taken=0; ++phase; subattack=0; cycled=false;
                        }
                    }
                    // The dispatch branch was chosen before sigma_166DE; compare the post-dispatch value.
                    if(cycle.phase==9&&enemy_s16(damage_taken)>=1300){
                        dying=true;h.score(300000);phase_frame=0;
                    }
                }
            }
            if(!dying){
                unsigned dealt=h.damage(left[back]+36,top[back]+32,56,32);
                damage_taken=std::uint16_t(damage_taken+dealt);
                hit_flash=dealt!=0;
                check_contact(h,CollisionSource::SigmaBody,CollisionBounds::open(left[back]+8,top[back],left[back]+88,top[back]+64));
            }
            body(hit_flash?130:pattern);
            ++sigma_frame; // Includes the frame that arms retirement, excludes retirement itself.
        }
        update_bursts(back,h);
    }

private:
    int rain_direction=1,diagonal_direction=1,final_direction=2;
    std::uint8_t rain_width=0,orbit_angle=0,final_angle=0;
    unsigned burst_mask=7,billiard_color=0,diagonal_laser_index=0;
    int burst_margin=0,orbit_x=0,orbit_y=0,sweep_x=0;
    int chase_x=0,chase_mirror_x=0,chase_y=0,chase_dx=0,chase_dy=0;
    int beam_radius=0,beam_hit_l=0,beam_hit_r=0,beam_byte_l=0,beam_byte_r=0;
    int frame() const {return enemy_s16(phase_frame);}
    void start_cycle(){++phase;phase_frame=0;subattack=0;cycled=false;}
    static bool within(int n,int lo,int hi){return lo<n&&n<hi;}
    static void check_contact(const Hooks &h,CollisionSource source,CollisionBounds bounds){
        const CollisionPoint front{h.front_player_x,h.front_player_y};
        if(h.collision_display)h.collision_display->hazard(source,bounds,CollisionSample::Front,front);
        if(bounds.contains(front))h.player_hit();
    }
    static void check_beam(const Hooks &h,int left,int right){
        check_contact(h,CollisionSource::SigmaBeam,{CollisionAxis::open(left,right),CollisionAxis::any()});
    }
    static int direction(const Hooks &h){return h.player_x<center_player_left?1:-1;}
    static unsigned rng(const Hooks &h){return h.randring2_next()&255;}
    static void shot(const Hooks &h,int x,int y,unsigned angle,unsigned group,unsigned speed,
                     EnemyBulletRequest::Kind kind=EnemyBulletRequest::Pellet,unsigned pattern=0){
        h.bullet({kind,x,y,std::uint8_t(angle),std::uint8_t(group),std::uint8_t(pattern),speed});
    }
    void paint(Paint::Kind kind,int x,int y,int a,int b,unsigned color){paints.push_back({kind,x,y,a,b,color});}
    void body(unsigned pat){visible=true;draw_pattern=pat;body_paint_index=paints.size();}
    void circle(int x,int y,int r,unsigned color=white){paint(Paint::Circle,x,y,r,0,color);}
    void disc(int x,int y,int r,unsigned color=white){paint(Paint::Disc,x,y,r,0,color);}
    void beam(int byte_l,int byte_r,unsigned color=white){paint(Paint::Box,byte_l*8,field_top,byte_r*8+7,field_bottom-1,color);}
    // 13744–13806: overflow or clipped burst is a normal allocation failure.
    bool spawn_burst(int x,int y,int radius,const Hooks &h){
        if(!within(x,0,444)||!within(y,0,400))return false;
        for(auto &b:bursts)if(!b.state){
            b={1,0,26,64,x,y,radius};h.sound(9);return true;
        }
        return false;
    }
    void aimed_burst(int radius,const Hooks &h){spawn_burst(h.player_x+16,h.player_y+16,radius,h);}
    // 13848–14070: only the alternating owner page expands or contracts the disc;
    // collision runs on both pages, but only while live (including the transition tick).
    void update_bursts(unsigned back,const Hooks &h){
        for(unsigned i=0;i<bursts.size();++i){
            auto &b=bursts[i];if(!b.state)continue;
            ++b.age;const bool owner=(i&1)==back;
            if(b.state==1){
                if(!(b.age&burst_mask)){
                    if(++b.pattern>=34){b.state=2;h.sound(3);}
                    b.age=0;b.radius=std::uint8_t(b.radius-8);circle(b.x,b.y,b.radius);
                }
                if(b.state==1)paint(Paint::Sprite,b.x-16,b.y-16,b.pattern,0,0);
            }else if(b.state==3){
                if(owner){disc(b.x,b.y,b.radius);if(b.radius<b.target_radius)b.radius=std::uint8_t(b.radius+16);}
                if(b.age>32){b.state=4;b.age=0;b.target_radius=b.radius;b.radius=8;}
                const int r=int(b.radius)-burst_margin;
                check_contact(h,CollisionSource::SigmaBurst,CollisionBounds::open(b.x-r,b.y-r,b.x+r-32,b.y+r-32));
            }else if(b.state==4&&owner){
                disc(b.x,b.y,b.target_radius);disc(b.x,b.y,b.radius,0);
                if(b.radius>=b.target_radius)b.state=0;
                else b.radius=std::uint8_t(b.radius+16);
            }
        }
    }
    // 14219–14520: original defeat still has a lethal beam; do not clear, grant
    // invincibility, or claim victory when the 300000 is awarded.
    bool retire(unsigned back,int x,int y,const Hooks &h){
        const int f=frame();
        if(f<90){
            body(pattern);
            if(f==10){beam_radius=8;h.sound(9);}
            if(f>10&&back==0)disc(x+64,y+64,24);
            if(f>=50&&back==1)paint(Paint::Box,x+64,field_top,x+64,field_bottom-1,white);
            if(f==89)h.sound(3);
        }else if(f<100){
            beam((x+64-beam_radius)/8,(x+64+beam_radius)/8);
            check_beam(h,x+56-beam_radius,x+40+beam_radius);
            beam_radius+=8;body(pattern);
        }else if(f==100){
            beam_hit_l=x+56-beam_radius;beam_hit_r=x+40+beam_radius;
            beam_byte_l=(x+64-beam_radius)/8;beam_byte_r=(x+64+beam_radius)/8-1;
            pattern=134;
        }else if(f<=220){
            beam(beam_byte_l,beam_byte_r);
            if(f<=140)body(pattern);
            else if(f<180){
                if(f%10==0)pattern+=2;
                body(pattern);shake=unsigned(f&1);slowdown=2;
            }else slowdown=1;
            check_beam(h,beam_hit_l,beam_hit_r);
        }else if(f<224){beam(beam_byte_l,beam_byte_r,0);}
        else return true;
        return false;
    }
    // 14726–14792. Returns false only during the lead-in; at the terminal frame
    // it stores zero and the caller must still run its own modulo-0 attack logic.
    bool diagonal(unsigned back,const Hooks &h){
        const int f=frame();
        if(f<50)return false;
        if(f==50)diagonal_direction=direction(h);
        if(f<114){left[back]+=diagonal_direction;++top[back];}
        else if(f<242)left[back]-=diagonal_direction;
        else if(f<306){left[back]+=diagonal_direction;--top[back];}
        else phase_frame=0;
        return true;
    }
    // 15461–15517: faster x-only motion, same terminal-zero rule.
    bool horizontal(unsigned back,const Hooks &h){
        const int f=frame();
        if(f<50)return false;
        if(f==50)final_direction=2*direction(h);
        if(f<130)left[back]+=final_direction;
        else if(f<290)left[back]-=final_direction;
        else if(f<370)left[back]+=final_direction;
        else phase_frame=0;
        return true;
    }
    void rain(int f,unsigned back,int x,int y,const Hooks &h){
        if(f<20)return;
        if(f==20){rain_width=0;rain_direction=direction(h);}
        if(!(f&3))h.sound(3);
        if(f<340){
            left[back]+=rain_direction*((f>=148&&f<212)?-4:1);
            if(f&1){const int bx=x+int(rng(h)&127);const unsigned speed=30+(rng(h)&31);shot(h,bx,y+64,64,single,speed);}
        }else if(f<440){
            ++rain_width;
            const unsigned angle=64-rain_width+rng(h)%(rain_width*2);
            if(!(f&3))for(unsigned i=0;i<3;++i){
                const int bx=x+int(rng(h)&127);const unsigned speed=30+(rng(h)&31);
                shot(h,bx,y+64,angle,single,speed);
            }
        }else phase_frame=0;
    }
    bool windup(int f,unsigned back,const Hooks &h){
        if(f<50)return true;
        if(f==50)h.sound(9);
        if(f<100){pattern=128+4*back;return true;}
        if(f==100)pattern=128;
        return false;
    }
    void orbit_bursts(int f,unsigned back,int mx,int my,const Hooks &h){
        if(windup(f,back,h))return;
        if(f==100){orbit_x=h.player_x+16;orbit_y=h.player_y+16;burst_mask=3;burst_margin=-4;orbit_angle=0;}
        if(f<130){
            if(f%8==0)circle(orbit_x,orbit_y,112);
            if(f%8==2)circle(orbit_x,orbit_y,112,0);
        }else if(f<450){
            if(back==0)circle(orbit_x,orbit_y,112);
            if(f>=150){
                if(f%12==0){
                    for(unsigned a:{unsigned(orbit_angle),unsigned(orbit_angle)+128}){
                        const auto v=enemy_vector(a,112);spawn_burst(orbit_x+v[0],orbit_y+v[1],24,h);
                    }
                    orbit_angle=std::uint8_t(orbit_angle-16);
                }
                if(f>=200&&!(f&15))shot(h,mx,my,rng(h),ring_32,80);
            }
        }else if(back==0){circle(orbit_x,orbit_y,112,0);phase_frame=0;}
    }
    void billiard_sweep(int f,unsigned back,int x,int mx,int my,const Hooks &h){
        if(windup(f,back,h))return;
        if(f==100){sweep_x=field_left;billiard_color=0;h.turns_max(1);}
        if(f&15)return;
        shot(h,sweep_x,field_top+8,64,bounce_all,64,EnemyBulletRequest::Bullet16,122+billiard_color);
        shot(h,field_right+16-sweep_x,field_top+8,64,bounce_all,64,EnemyBulletRequest::Bullet16,122+billiard_color);
        sweep_x+=16;
        if(sweep_x==224){
            h.laser(mx,my,1,111,32);
            h.laser(x+44,my,1,111,48);h.laser(x+76,my,1,111,48);
            h.laser(x+28,my,1,111,100);h.laser(x+92,my,1,111,100);
            // The source restores byte_23A70 to 16: each request explicitly carries its own delay.
        }
        if(sweep_x>=field_right+16){sweep_x=field_left;if(++billiard_color>=2)phase_frame=0;}
    }
    void diagonal_lasers(unsigned back,int x,int mx,int my,const Hooks &h){
        if(!diagonal(back,h))return;
        const int f=frame();
        // DS:13AE (file 0x2021E of original DOS MAIN.EXE; same table in patched image).
        constexpr std::array<int,5> offsets={60,44,76,28,92};
        // The preceding sweep restores the shared delay to 16.
        const auto laser=[&]{h.laser(x+offsets[diagonal_laser_index++],my,1,111,16);};
        if(f==100)diagonal_laser_index=0;
        else if(f==114||f==242)laser();
        else if(f==118||f==246){laser();laser();}
        else if(f==122||f==250){laser();laser();diagonal_laser_index=0;}
        else if(f>120&&f<240&&!(f&15)){
            for(unsigned a=0x90,i=0;a<0xF0;a+=16,++i)
                shot(h,mx,my,a,bounce_all,60,EnemyBulletRequest::Bullet16,122+(i&1));
        }
    }
    void opposed_bursts(int f,unsigned back,int mx,int my,const Hooks &h){
        if(windup(f,back,h))return;
        if(f==100){
            const auto v=enemy_vector(enemy_atan2(h.player_y+16-my,h.player_x+16-mx),48);
            chase_dx=v[0];chase_dy=v[1];chase_x=chase_mirror_x=mx;chase_y=my;
            burst_margin=-4;burst_mask=3;
        }
        if(f&7)return;
        const bool first=spawn_burst(chase_x,chase_y,48,h);
        const bool second=spawn_burst(chase_mirror_x,chase_y,48,h);
        if(!first&&!second)phase_frame=0; // Source also ends on simultaneous pool exhaustion.
        chase_x=enemy_s16(unsigned(chase_x+chase_dx));
        chase_mirror_x=enemy_s16(unsigned(chase_mirror_x-chase_dx));
        chase_y=enemy_s16(unsigned(chase_y+chase_dy));
        const int target=h.player_x+16;
        if(target!=chase_x)chase_x=enemy_s16(unsigned(chase_x+(target>chase_x?16:-16)));
        if(target!=chase_mirror_x)chase_mirror_x=enemy_s16(unsigned(chase_mirror_x+(target>chase_mirror_x?-16:16)));
        shot(h,mx,my,rng(h),ring_16,60);
    }
    void attack(Attack a,unsigned back,int x,int y,int mx,int my,const Hooks &h){
        const int f=frame();
        switch(a){
        case Attack::Rain:rain(f,back,x,y,h);break;
        case Attack::TwinBursts: // 14674–14719
            if(f==100){burst_mask=7;burst_margin=4;spawn_burst(128,320,64,h);}
            if(f==120)spawn_burst(320,320,64,h);
            if(f>130&&f<150&&!(f&3))shot(h,mx,my,0,aimed_5,120);
            if(f>200)phase_frame=0;
            break;
        case Attack::DiagonalRing32:
            if(diagonal(back,h)&&!(phase_frame&31))shot(h,mx,my,rng(h),ring_32,85);
            break;
        case Attack::OrbitBursts:orbit_bursts(f,back,mx,my,h);break;
        case Attack::DiagonalRing16:
            if(diagonal(back,h)&&!(phase_frame&7))shot(h,mx,my,rng(h),ring_16,50);
            break;
        case Attack::BilliardSweep:billiard_sweep(f,back,x,mx,my,h);break;
        case Attack::DiagonalLasers:diagonal_lasers(back,x,mx,my,h);break;
        case Attack::OpposedBursts:opposed_bursts(f,back,mx,my,h);break;
        case Attack::DiagonalBursts: // 15416–15454
            if(!diagonal(back,h))break;
            if(phase_frame==50)burst_mask=7;
            if((phase_frame&63)==32)aimed_burst(48,h);
            if(!(phase_frame&63))shot(h,mx,my,rng(h),ring_32,60);
            break;
        case Attack::FinalWide: // 15524–15554
            if(!horizontal(back,h))break;
            if(phase_frame==50)burst_mask=7;
            if(!(phase_frame&63))aimed_burst(48,h);
            if(!(phase_frame&7))shot(h,mx,my,0,aimed_wide_2,70);
            break;
        case Attack::FinalRing8: // 15561–15587
            if(!horizontal(back,h))break;
            if(phase_frame==50)final_angle=0;
            if(!(phase_frame&7)){
                shot(h,mx,my,final_angle,ring_8,80,EnemyBulletRequest::Bullet16,86);
                final_angle=std::uint8_t(final_angle+8);
            }
            break;
        case Attack::FinalBursts: // 15594–15632
            if(!horizontal(back,h))break;
            if(phase_frame==50)burst_mask=7;
            if(!(phase_frame&31))aimed_burst(48,h);
            if((phase_frame&31)==16)shot(h,mx,my,rng(h),ring_16,60);
            break;
        }
    }
};
}
