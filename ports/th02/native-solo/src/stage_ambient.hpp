#pragma once
#include "core_contract.hpp"
#include "enemy_math.hpp"
#include "render_motion.hpp"
#include "stage_background.hpp"

namespace soew {
// ReC98 th02_main.asm: sub_13671 (Stage 1), sub_140AE/sub_1403E
// (Stage 2), sub_10E95 (Stage 3). This layer owns no gameplay state.
class StageAmbient {
    struct Particle {int x=0,y=0,speed=0;};
    int stage;
    IRand random;
    unsigned age=0,lightning_age=0,next_flash=100;
    std::array<Particle,50> particles{};

    void falling_particles(bool spawn){
        if(spawn){
            const int x=48+random.next()%640;
            for(auto &p:particles)if(!p.speed){p={x,16,3+(random.next()&3)};break;}
        }
        for(auto &p:particles)if(p.speed){
            p.x-=p.speed;p.y+=p.speed;
            if(p.x<32||p.y>=384)p.speed=0;
        }
    }
    unsigned flash()const{
        unsigned phase=0,peak=0,frame=0;
        if(stage<=1&&lightning_age>=next_flash&&lightning_age<next_flash+8){
            phase=lightning_age-next_flash;peak=32;frame=8;
        }
        return frame?peak*std::min(phase+1,frame-phase)/(frame/2):0;
    }
    static std::uint32_t rgb(const StageBackground::Palette &palette,unsigned color){
        const auto &p=palette[color];return unsigned(p[0])<<16|unsigned(p[1])<<8|p[2];
    }
public:
    explicit StageAmbient(int id=0):stage(id),random(1234u+unsigned(id)){}
    static constexpr unsigned stage3_explosion=168;
    static constexpr unsigned stage3_ring=200;
    bool opening_explosion()const{return stage==2&&age==stage3_explosion;}
    bool opening_shake()const{return stage==2&&age>stage3_explosion&&age<stage3_ring;}
    unsigned opening_tone(bool reduced)const{
        if(reduced||!opening_shake()||age>180)return 100;
        // Keep the explosion and shake, but replace ReC98's 100/170 strobe
        // with a softer build-up and release over the same event window.
        constexpr unsigned peak_age=171, peak=26;
        if(age<peak_age)return 100+peak*(age-stage3_explosion)/(peak_age-stage3_explosion);
        return 100+peak*(180-age)/(180-peak_age);
    }
    unsigned opening_shift(bool reduced)const{return !reduced&&opening_shake()&&(age&1)?16:0;}
    void seek_practice(unsigned progress,unsigned frames_per_crossing,bool reduced){
        if(stage>2)return;
        const unsigned trigger=stage==0?116:80;
        for(unsigned i=0;i<progress*frames_per_crossing;++i)
            tick(stage<2&&i==trigger*frames_per_crossing,reduced);
    }
    void tick(bool midboss_intro,bool reduced){
        if(stage==0){
            if(!age&&!midboss_intro)return;
            ++age;
            if(age<512)return;
            falling_particles(age>512&&(age-512)%(reduced?5:3)==0);
        }else if(stage==1){
            ++age;
            falling_particles(age%(reduced?4:2)==0);
        }else if(stage==2){
            ++age;return;
        }else return;
        if(stage==0||midboss_intro||lightning_age)++lightning_age;
        if(lightning_age==next_flash+8)next_flash+=20+(random.next()&1023);
    }

    StageBackground::Palette palette(StageBackground::Palette colors,bool reduced)const{
        if(stage==0&&age>=2){
            // Keep the original byte-wrapped palette endpoints, but use native RGB
            // rather than quantizing each channel to the PC-98's DAC steps.
            const auto t=std::min(age,512u);
            const auto shift=[](unsigned value){return std::uint8_t(value);};
            colors[8]={176,shift(144+t/16),shift(144+t/8)};
            colors[13]={shift(160-t/16),shift(160-t/16),shift(255-t/4)};
            if(age>=512)colors[14]={224,192,176};
        }else if(stage==2&&age>=stage3_ring){
            const unsigned t=(age-stage3_ring)%800,fall=100-t%200/2;
            const auto channel=std::uint8_t(fall);
            switch(t/200){
            case 0:colors[0]={channel,0,0};break;
            case 1:colors[0]={0,channel,0};break;
            case 2:colors[0]={0,0,channel};break;
            case 3:colors[0]={channel,0,channel};break;
            }
        }
        // Original effects alternated palette tone every frame. Keep the light
        // pulse without reintroducing the display-page strobe.
        const unsigned light=reduced?0:flash();
        if(light)for(auto &color:colors)for(auto &c:color)c=std::uint8_t(c+(255-c)*light/100);
        return colors;
    }

    void render(std::span<std::uint32_t> pixels,const StageBackground::Palette &colors,unsigned phase,bool reduced)const{
        const int lag=int(motion_subpixels-phase);
        if(stage<=1){
            const auto color=rgb(colors,stage==0?8:3);
            for(const auto &p:particles)if(p.speed){
                const RenderPoint at{p.x*256+p.speed*lag,p.y*256-p.speed*lag};
                blit_motion_sprite(pixels,at,2,2,[&](int,int){return color;});
            }
        }else if(stage==2&&age>=169){
            const unsigned t=(std::max(age,stage3_ring)-stage3_ring)%800;
            const unsigned angle_step=reduced?8:4;
            const auto color=rgb(colors,7);
            const auto point=[](unsigned i,unsigned t){
                const unsigned a=t%200;
                const auto d=enemy_vector(i+1+((t/200)&1?-int(a):int(a)),int(a));
                return RenderPoint::pixels(224+d[0],200+d[1]);
            };
            const unsigned prev=t-(t%200!=0); // A new ring starts at the center.
            for(unsigned i=0;i<256;i+=angle_step){
                const auto at=motion_lerp(point(i,prev),point(i,t),phase);
                blit_motion_sprite(pixels,at,1,1,[&](int,int){return color;});
            }
        }
    }

};

// ReC98 th02_main.asm: sub_3E1C/sub_3F8A and marisa_update. Use both
// axes in native subpixels; do not consume the boss's projectile RNG.
class MarisaStarfield {
    struct Star {RenderPoint previous,current;unsigned angle=0;int speed=0;};
    std::array<Star,30> stars{};
    IRand random{1689};
    unsigned color=9,cycle=0;
    // DS:08EEh contains the left-aligned masks 80/C0/E0/F0/F8.
    static int size(int speed){return std::clamp((speed-1)/24,1,5);}
public:
    void tick(unsigned frame,bool reduced){
        if((frame&1)&&(!reduced||(frame&3)==3)){
            const int x=32+random.next()%384;
            for(auto &s:stars)if(!s.speed){
                const auto origin=RenderPoint::pixels(x,16);
                s={origin,origin,unsigned(64+(224-x)/3),24};
                break;
            }
        }
        if((frame&31)==0){
            // DS:1420h in the Japanese TH02 MAIN.EXE.
            constexpr std::array<unsigned,6> colors={10,12,13,13,12,10};
            cycle=(cycle+1)%colors.size();color=colors[cycle];
        }
        for(auto &s:stars)if(s.speed){
            s.previous=s.current;s.speed+=4;
            const auto v=enemy_vector(s.angle,s.speed*16);
            s.current.x+=v[0];s.current.y+=v[1];
            if(!SpriteClip{}.overlaps(s.current,size(s.speed),size(s.speed)))s.speed=0;
        }
    }
    void render(std::span<std::uint32_t> pixels,const StageBackground::Palette &palette,unsigned phase)const{
        const auto &c=palette[color];const auto rgb=unsigned(c[0])<<16|unsigned(c[1])<<8|c[2];
        for(const auto &s:stars)if(s.speed){
            const auto position=motion_lerp(s.previous,s.current,phase);
            const int extent=size(s.speed);
            blit_motion_sprite(pixels,phase==motion_subpixels?position.snapped():position,
                extent,extent,[&](int,int){return rgb;});
        }
    }
};
}
