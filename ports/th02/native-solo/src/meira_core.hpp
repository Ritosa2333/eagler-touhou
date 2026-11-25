#pragma once
#include "boss_damage.hpp"
#include "enemy_effects.hpp"
#include "enemy_math.hpp"
#include "collision_display.hpp"
#include <functional>
namespace soew {
// th02_main.asm11600..13361. Starts after the preboss dialogue; scene/end
// dialogue, palette changes, tile invalidation and stage advance are caller-owned.
struct MeiraCore {
    struct Effect {
        std::uint8_t flag=0,group=0;
        int left=0,top=0;
        std::uint8_t angle=0,age=0,speed=0,tiny_pattern=0,delay=0;
    };
    struct Point {int left=0,top=0;};
    struct EffectDraw {int left=0,top=0;unsigned pattern=0;bool tiny=false;};
    // These globals are NOT initialized by meira_init. Supplying zero is an
    // explicit caller choice, not an assertion that the original resets them.
    struct Inherited {
        std::uint16_t phase_frame=0,death_frame=0;
        std::array<Effect,40> effects{};
        std::array<std::array<Point,3>,2> clones{};
        std::uint8_t marker_pattern=0,effect_parity=0,trail_group=0,trail_speed=0,direction=0;
        std::uint16_t trail_delay=0,stream_speed=0,aimed_speed=0;
        int origin_x=0,origin_y=0,destination_x=0,destination_y=0;
        std::uint16_t travel_step=0;
    };
    struct Hooks {
        unsigned damage_divisor=1;
        std::function<void(const EnemyBulletRequest&)> bullet;
        std::function<unsigned(int,int,int,int)> damage;
        std::function<void(unsigned)> score,sound,invincibility;
        std::function<void()> hit;
        std::function<void(int,int,int,int,bool)> spark;
        std::function<void(int,int,int)> explosion;
        std::function<unsigned()> random8,random16;
        std::function<void(unsigned)> special_turns;
        CollisionFrame *collision_display=nullptr;
    };
    std::array<int,2> left{192,192},top{48,48};
    std::uint16_t phase_frame=0,death_frame=0;
    BossDamage damage_taken;
    unsigned rank=1,state=0,attack_index=0,pattern=141,draw_pattern=141;
    unsigned special_turns_max=2,death_pattern=0;
    std::uint8_t special_angle=0,marker_pattern=0,effect_parity=0,trail_group=0,trail_speed=0,direction=0;
    std::uint16_t trail_delay=0,stream_speed=0,aimed_speed=0,travel_step=0;
    int origin_x=0,origin_y=0,destination_x=0,destination_y=0;
    int render_y=48,target_x=-1,target_y=-1;
    bool dying=false,done=false,visible=false,hit_flash=false,death_zoom=false,clones_enabled=false;
    std::array<Effect,40> effects{};
    std::array<std::array<Point,3>,2> clones{};
    std::array<EffectDraw,40> effect_draws{};
    std::array<Point,3> clone_draws{};
    unsigned effect_draw_count=0,clone_draw_count=0;

    void init(unsigned difficulty,const Inherited& old){
        if(difficulty>4)throw std::runtime_error("Invalid Meira rank");
        *this=MeiraCore{};rank=difficulty;phase_frame=old.phase_frame;death_frame=old.death_frame;
        effects=old.effects;clones=old.clones;marker_pattern=old.marker_pattern;effect_parity=old.effect_parity;
        trail_group=old.trail_group;trail_speed=old.trail_speed;direction=old.direction;trail_delay=old.trail_delay;
        stream_speed=old.stream_speed;aimed_speed=old.aimed_speed;origin_x=old.origin_x;origin_y=old.origin_y;
        destination_x=old.destination_x;destination_y=old.destination_y;travel_step=old.travel_step;
    }
    static int word(int v){return enemy_s16(unsigned(v));}
    static int rolled(int y,unsigned scroll){int v=word(y+int(scroll));return v>=400?v-400:v;}
    // Source bg_render copies front coordinates and only then reclaims flag2.
    void begin_frame(unsigned back){
        if(back>1)throw std::runtime_error("Invalid Meira page");
        left[back]=left[back^1];top[back]=top[back^1];
        if(clones_enabled)clones[back]=clones[back^1];
        for(auto& e:effects)if(e.flag==2)e.flag=0;
    }
    static void sound(const Hooks& h,unsigned n){if(h.sound)h.sound(n);}
    void emit(const Hooks& h,int x,int y,unsigned angle,unsigned group,unsigned speed,bool big=false,unsigned pat=0){
        EnemyBulletRequest r;r.left=word(x);r.top=word(y);r.angle=std::uint8_t(angle);r.group=std::uint8_t(group);
        r.speed=speed;r.pattern=std::uint8_t(pat);r.kind=big?EnemyBulletRequest::Bullet16:EnemyBulletRequest::Pellet;
        if(h.bullet)h.bullet(r);
    }
    bool add_effect(int x,int y,unsigned angle,unsigned group,unsigned speed,unsigned delay){
        for(auto& e:effects)if(!e.flag){e.flag=1;e.left=word(x);e.top=word(y);e.angle=std::uint8_t(angle);
            e.group=std::uint8_t(group);e.age=0;e.speed=std::uint8_t(speed);e.tiny_pattern=marker_pattern;e.delay=std::uint8_t(delay);return true;}
        return false;
    }
    void effects_update(int px,int py,unsigned scroll,const Hooks& h){
        // th02_main.asm12002..12103: waiting marker is itself lethal, with
        // inclusive X endpoints but strict Y endpoints. Animation is not lethal.
        for(auto& e:effects){if(e.flag!=1)continue;
            if(e.delay){effect_draws[effect_draw_count++]={e.left,rolled(e.top,scroll),e.tiny_pattern,true};--e.delay;
                const CollisionBounds bounds{CollisionAxis::closed(word(e.left-16),e.left),CollisionAxis::open(word(e.top-16),e.top)};
                if(h.collision_display)h.collision_display->hazard(CollisionSource::MeiraMarker,bounds,CollisionSample::Current,{px,py});
                if(bounds.contains({px,py})){if(h.hit)h.hit();}continue;}
            if(e.age<32){if(!e.age){sound(h,10);e.left=word(e.left-8);e.top=word(e.top-8);}
                effect_draws[effect_draw_count++]={e.left,rolled(e.top,scroll),26u+e.age/4u,false};++e.age;continue;}
            if((effect_parity&1)<=rank)emit(h,e.left+12,e.top+12,e.angle,e.group,e.speed);
            ++effect_parity;e.flag=2;
        }
    }
    void trail_move(unsigned back,int dx,int dy){
        left[back]=word(left[back]+(direction?-dx:dx));
        if(!direction&&left[back]>368)left[back]=368;
        if(direction&&left[back]<16)left[back]=16;
        top[back]=word(top[back]+dy);
        // Pascal arguments: left,top,angle,group,speed,delay; speed/delay
        // are byte reads despite the overlapping word pushes in the original.
        add_effect(left[back]+12,top[back]+12,0,trail_group,trail_speed,trail_delay);
    }
    void entrance(unsigned back,const Hooks& h){
        const int f=enemy_s16(phase_frame);
        if(f<10)return;
        if(f==10){sound(h,9);pattern=142;return;}
        if(f<50)return;
        if(f==50){sound(h,3);pattern=143;return;}
        if(f<=90){top[back]=word(top[back]+6);return;}
        pattern=141;phase_frame=0;
    }
    void first_slash(unsigned back,int px){
        const int f=enemy_s16(phase_frame),y=top[back];if(f<20)return;
        if(f==20){direction=word(left[back]+16)<px;pattern=144+direction;trail_delay=16;marker_pattern=std::uint8_t(121-direction);trail_group=25;trail_speed=64;return;}
        if(f<=32){trail_move(back,-8,-8);trail_delay+=2;return;}
        if(f<56){pattern=141;return;}
        if(f==56){pattern=145-direction;marker_pattern=std::uint8_t(120+direction);trail_delay=16;return;}
        if(f==57){trail_move(back,8,-8);trail_delay+=2;if(y>48)phase_frame=56;return;}
        if(f<80){pattern=141;return;}
        if(f==80){pattern=149-direction;marker_pattern=std::uint8_t(121-direction);trail_delay=16;return;}
        if(left[back]>32&&left[back]<352){trail_move(back,8,8);trail_delay+=2;return;}
        if(y>48){top[back]=word(top[back]-2);pattern=141;}else phase_frame=0;
    }
    void second_slash(unsigned back,const Hooks& h){
        const int f=enemy_s16(phase_frame);if(f<50)return;
        if(f==50){sound(h,9);pattern=142;trail_group=26;return;}
        if(f<99)return;
        if(f==99){sound(h,3);pattern=149-direction;marker_pattern=std::uint8_t(120+direction);trail_delay=80;return;}
        if(f<120){trail_move(back,-8,8);--trail_delay;return;}
        if(f<136){pattern=144+direction;marker_pattern=std::uint8_t(121-direction);--trail_delay;if(f==135)sound(h,3);return;}
        if(f<156){trail_move(back,-8,-8);--trail_delay;return;}
        if(left[back]!=192){pattern=141;left[back]=word(left[back]+(direction?-2:2));}else phase_frame=0;
    }
    void stream(unsigned back,const Hooks& h){
        const int f=enemy_s16(phase_frame);if(f<50)return;
        if(f==50){sound(h,9);pattern=142;return;}
        if(f<99)return;
        if(f==99){sound(h,3);pattern=143;stream_speed=32;return;}
        if(f<116){if(f&1){emit(h,left[back]+24,top[back]+32,64,rank==0?9:21,stream_speed);stream_speed+=11;}return;}
        pattern=141;phase_frame=0;
    }
    void aimed_stream(unsigned back,const Hooks& h){
        const int f=enemy_s16(phase_frame);if(f<50)return;
        if(f==50){sound(h,9);pattern=142;trail_group=26;return;}
        if(f<99)return;
        if(f==99){sound(h,3);pattern=143;aimed_speed=32;return;}
        if(f<120){if(f&1){emit(h,left[back]+24,top[back]+32,0,rank==0?11:23,aimed_speed);aimed_speed+=11;}
            if(left[back]!=192)left[back]=word(left[back]+(left[back]<192?8:-8));
            return;}
        pattern=141;phase_frame=0;
    }
    void four_slashes(unsigned back,int px,const Hooks& h){
        const int f=enemy_s16(phase_frame);if(f<50)return;
        if(f==50){direction=word(left[back]+16)<px;sound(h,9);pattern=142;trail_group=26;trail_speed=48;return;}
        if(f<99)return;
        if(f==99){sound(h,3);pattern=148+direction;marker_pattern=std::uint8_t(120+direction);trail_delay=20;return;}
        if(f<116){trail_move(back,-8,8);trail_delay+=2;return;}
        if(f<136){pattern=149-direction;marker_pattern=std::uint8_t(121-direction);trail_delay=20;if(f==135)sound(h,3);return;}
        if(f<152){trail_move(back,8,8);trail_delay+=2;return;}
        if(f<172){pattern=145-direction;marker_pattern=std::uint8_t(120+direction);trail_delay=20;if(f==171)sound(h,3);return;}
        if(f<188){trail_move(back,8,-8);trail_delay+=2;return;}
        if(f<208){pattern=144+direction;marker_pattern=std::uint8_t(121-direction);trail_delay=20;if(f==207)sound(h,3);return;}
        if(f<224){trail_move(back,-8,-8);trail_delay+=2;return;}
        pattern=141;phase_frame=0;
    }
    void triple_aimed(unsigned back,const Hooks& h){
        const int f=enemy_s16(phase_frame);if(f<50)return;
        if(f==50){sound(h,9);pattern=142;trail_group=26;return;}
        if(f<99)return;
        if(f==99||f==108||f==116){sound(h,3);pattern=143;emit(h,left[back]+24,top[back]+32,0,rank==0?11:23,74);return;}
        if(f<116){pattern=142;return;}pattern=141;phase_frame=0;
    }
    static unsigned random(const std::function<unsigned()>& fn,unsigned mask){
        if(!fn)throw std::runtime_error("Meira requires the original random-ring input");
        return fn()&mask;
    }
    void bounce_opener(unsigned back,const Hooks& h){
        const int f=enemy_s16(phase_frame);if(f<50)return;
        if(f==50){sound(h,9);pattern=142;special_turns_max=5;if(h.special_turns)h.special_turns(5);return;}
        if(f<99)return;
        pattern=146;
        for(unsigned i=0;i<3;++i)emit(h,left[back]+24,top[back]+24,random(h.random8,255),0x87,54,true,122+(i&1));
        phase_frame=0;clones_enabled=true;
    }
    void clone_render(unsigned back,unsigned scroll){
        for(const auto& p:clones[back])if(p.left!=left[back]&&p.top!=top[back])clone_draws[clone_draw_count++]={p.left,rolled(p.top,scroll)};
    }
    void clone_travel(unsigned back,unsigned scroll,const Hooks& h){
        const int f=enemy_s16(phase_frame);if(f<50)return;
        if(!(phase_frame&7)){clones[back][0]=clones[back][1];clones[back][1]=clones[back][2];clones[back][2]={left[back],top[back]};}
        if(f==50){for(auto& c:clones[back])c={left[back],top[back]};
            destination_x=int(random(h.random16,65535)%320)+32;destination_y=int(random(h.random8,255))+16;
            origin_x=left[back];origin_y=top[back];travel_step=1;clone_render(back,scroll);return;}
        if(f<=114){
            // IMUL's high word is overwritten by CWD: divide the wrapped low
            // signed word, not the full mathematical product (asm12997..13018).
            left[back]=word(origin_x+word(word(destination_x-origin_x)*enemy_s16(travel_step))/64);
            top[back]=word(origin_y+word(word(destination_y-origin_y)*enemy_s16(travel_step))/64);
            ++travel_step;clone_render(back,scroll);return;
        }
        // The source's f==114 sound branch is unreachable after f<=114.
        if(f<164)return;
        pattern=146;for(unsigned i=0;i<2;++i)emit(h,left[back]+24,top[back]+24,random(h.random8,255),0x87,54,true,122+i);
        phase_frame=0;clone_render(back,scroll);
    }
    void attacks(unsigned back,int px,unsigned scroll,const Hooks& h){
        if(state==0){switch(attack_index){case 0:entrance(back,h);break;case 1:first_slash(back,px);break;case 2:second_slash(back,h);break;case 3:stream(back,h);break;}
            if(!phase_frame){attack_index=std::uint8_t(attack_index+1);if(attack_index>3)attack_index=0;
                if(enemy_s16(damage_taken)>700&&attack_index!=1){++state;attack_index=0;}}return;}
        if(state==1){switch(attack_index){case 0:aimed_stream(back,h);break;case 1:four_slashes(back,px,h);break;case 2:triple_aimed(back,h);break;}
            if(!phase_frame){attack_index=std::uint8_t(attack_index+1);if(attack_index>2)attack_index=0;
                if(enemy_s16(damage_taken)>1500){++state;attack_index=0;}}return;}
        if(state==2){if(attack_index==0)bounce_opener(back,h);else if(attack_index==1)clone_travel(back,scroll,h);if(!phase_frame)attack_index=1;}
    }
    void damage_and_render(unsigned back,int px,int py,unsigned scroll,const Hooks& h){
        const int x=left[back],y=top[back];
        const CollisionBounds bounds{CollisionAxis::half_open(x,word(x+32)),CollisionAxis::open(word(y-16),word(y+32))};
        if(h.collision_display)h.collision_display->hazard(CollisionSource::MeiraBody,bounds,CollisionSample::Current,{px,py});
        if(bounds.contains({px,py})){if(h.hit)h.hit();}
        render_y=rolled(y,scroll);const unsigned dealt=h.damage?h.damage(word(x+8),y,48,48):0;
        visible=true;draw_pattern=pattern;
        if(dealt){damage_taken.add(dealt,h.damage_divisor);sound(h,4);hit_flash=true;
            if(enemy_s16(damage_taken)>=2400){dying=true;if(h.score)h.score(30000);if(h.invincibility)h.invincibility(200);}}
    }
    void death(unsigned back,unsigned scroll,const Hooks& h){
        if(h.explosion){h.explosion(word(left[back]+24),word(top[back]+24),enemy_s16(death_frame));
            h.explosion(word(left[back]+24),word(top[back]+24),word(int(death_frame)-24));}
        if(h.spark)h.spark(word(left[back]+48),word(top[back]+48),60,1,false);
        death_pattern=unsigned(10+word(int(death_frame)-32)/10);++death_frame;
        if(enemy_s16(death_frame)>=96){death_frame=0;done=true;return;}
        render_y=rolled(top[back],scroll);visible=true;draw_pattern=pattern;death_zoom=enemy_s16(phase_frame)>=32;
    }
    void update(unsigned back,int player_x,int player_y,unsigned scroll,const Hooks& h){
        if(back>1||scroll>=400)throw std::runtime_error("Invalid Meira frame context");
        visible=false;hit_flash=false;death_zoom=false;effect_draw_count=0;clone_draw_count=0;draw_pattern=pattern;if(done)return;
        ++phase_frame;target_x=word(left[back]+40);target_y=word(top[back]+40);
        if(dying){death(back,scroll,h);if(done)return;}
        else{attacks(back,player_x,scroll,h);damage_and_render(back,player_x,player_y,scroll,h);}
        effects_update(player_x,player_y,scroll,h);
    }
};
}
