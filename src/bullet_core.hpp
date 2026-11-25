#pragma once
#include "enemy_effects.hpp"
#include "enemy_math.hpp"
#include "core_contract.hpp"
#include "collision_display.hpp"
#include <array>
namespace soew {
// Source-backed groups and special motions from ReC98/th02/main/bullet/bullet.cpp.
// Q12.4 positions, first-free150 pool, page carry and original hit rectangles.
// Unknown special values reject explicitly; shared words alias as original unions.
struct BulletCore {
    struct Bullet {
        unsigned flag=0; // free0/alive1/remove2
        EnemyBulletRequest::Kind kind=EnemyBulletRequest::Pellet;
        std::array<std::array<std::uint16_t,2>,2> position{};
        std::array<int,2> velocity{};
        std::uint8_t angle=0,pattern=0,motion=0,speed=0,counter=0;
        unsigned motion_generation=0;
    };
    using BoundaryExit=std::function<void(const Bullet&,const std::array<std::uint16_t,2>&)>;
    std::array<Bullet,150> bullets{};
    int rank=1,playperf=0;
    unsigned stack=0;
    std::uint8_t easy_skip=0;
    std::uint64_t spawned=0,removed=0;
    // Defaults from bullets_and_sparks_init. Each word has several meanings;
    // changing chase_speed also changes gravity_speed/drift_angle, etc.
    struct Special {int u1=1,u2=64,u3=1;} special;
    static bool known_special(unsigned value){return (value>=0x80&&value<=0x85)||value==0x87||value==0x88||value==0xff;}
    explicit BulletCore(int difficulty=1):rank(difficulty),stack(difficulty==2||difficulty==3?1:0){if(rank<0||rank>4)throw std::runtime_error("Invalid bullet rank");}
    static int pixel(std::uint16_t raw){const int value=enemy_s16(raw);return value>=0?value/16:-((-value+15)/16);}
    void begin_frame(unsigned back){
        if(back>1)throw std::runtime_error("Invalid bullet page");
        for(auto &b:bullets){if(b.flag==2){b.flag=0;continue;}if(b.flag==1)b.position[back]=b.position[back^1];}
    }
    void clear(unsigned back){
        // Source bullets_clear first invalidates AGAIN: active coordinates
        // copy front->back, remove flags become free, then alive becomesremove.
        begin_frame(back);for(auto &b:bullets)if(b.flag==1){b.flag=2;++removed;}
    }
    struct GroupAngle {int offset=0;bool aim=true,done=false;int extra_speed=0;};
    static GroupAngle group_angle(unsigned group,unsigned i,unsigned angle,RandomRing &rng){
        GroupAngle g;
        if(group==0x19){g.done=true;return g;}
        if(group>=1&&group<=24){const unsigned family=(group-1)/6,variant=(group-1)%6;const int widen=int(variant%3);g.aim=variant>=3;
            if(family==0){const int step=3+widen*3;g.offset=i==0?step:-step;g.done=i==1;}
            if(family==1){const int step=8+widen*8;g.offset=i==0?0:i==1?step:-step;g.done=i==2;}
            if(family==2){const int step=3+widen*6;g.offset=i==0?step:i==1?-step:i==2?step*3:-step*3;g.done=i==3;}
            if(family==3){const int step=6+widen*6;g.offset=i==0?0:i==1?step:i==2?-step:i==3?step*2:-step*2;g.done=i==4;}
            return g;
        }
        if(group==0x26){g.offset=i==0?14:-14;g.done=i==1;return g;}
        g.aim=false;
        switch(group){
        case 0x20:g.done=true;break;
        case 0x21:g.offset=int(i*64);g.done=i>=3;break;
        case 0x22:g.offset=i==0?0:128;g.done=i!=0;break;
        case 0x23:g.offset=i==0?0:128-int(angle)*2;g.done=i!=0;break;
        case 0x24:g.offset=int(i*32);g.done=i>=7;break;
        case 0x25:g.offset=int(i*16);g.done=i>=15;break;
        case 0x27:g.offset=int(i*8);g.done=i>=31;break;
        case 0x1a:g.offset=rng.next8();g.done=true;break;
        case 0x1b:g.offset=rng.next8();g.done=i>=angle;break;
        case 0x1c:g.offset=rng.next8();g.extra_speed=rng.masked(31);g.done=i>=angle;break;
        default:g.aim=true;break; // Original invalid group fills free pool aimed.
        }
        return g;
    }
    bool add(const EnemyBulletRequest &request,unsigned page,int player_x,int player_y,RandomRing &rng,bool bombing=false){
        if(page>1)throw std::runtime_error("Invalid bullet page");
        const bool is_special=request.kind==EnemyBulletRequest::Bullet16&&request.group>=0x80;
        if(is_special&&!known_special(request.group))return false;
        if(request.speed>255||playperf<-32768||playperf>32767)throw std::runtime_error("Bullet numeric input outside original range");
        if(request.left<24||request.left>416||request.top<0||request.top>383||bombing)return true;
        // int multiplication was16-bit: emulate wrap BEFORE signed divide/48.
        int speed=enemy_s16(request.speed);
        if(!(is_special&&request.group==0x83)){
            speed=enemy_s16(unsigned(speed+enemy_s16(unsigned(speed*playperf))/48));
            speed=enemy_s16(unsigned(speed+(rank==0?-10:rank==3?12:0)));
            if(speed<16){++easy_skip;if(rank==0&&(easy_skip&1))return true;speed=16;}
        }
        unsigned slot=0,stack_i=0,group_i=0;
        for(;;){
            while(slot<bullets.size()){
                auto &b=bullets[slot++];if(b.flag)continue;
                auto angle=is_special?GroupAngle{0,false,true,0}:group_angle(request.group,group_i,request.angle,rng);
                const auto tuned_speed=enemy_s16(unsigned(speed+angle.extra_speed));
                unsigned direction=static_cast<unsigned>(int(request.angle)+angle.offset)&255;
                if(angle.aim){const auto dx=enemy_s16(unsigned(player_x+12-request.left)),dy=enemy_s16(unsigned(player_y+20-request.top));direction=(direction+enemy_atan2(dy,dx))&255;}
                b.motion_generation=unsigned(++spawned);b.flag=1;b.kind=request.kind;b.pattern=request.pattern;b.angle=request.angle;b.counter=0;
                b.motion=request.kind==EnemyBulletRequest::Bullet16?request.group:0;
                if(request.kind==EnemyBulletRequest::Bullet16)b.speed=static_cast<std::uint8_t>(speed);
                b.position[page]={static_cast<std::uint16_t>(request.left*16),static_cast<std::uint16_t>(request.top*16)};
                b.velocity=enemy_vector(direction,tuned_speed);++group_i;
                if(angle.done)break;
            }
            if(stack_i>=stack)break;
            speed=enemy_s16(unsigned(speed));speed=speed>=0?speed/2:-((-speed+1)/2);if(rank==3)speed+=8;if(speed<16)break;
            ++stack_i;group_i=0;
        }
        return true;
    }
    void update_special(Bullet &b,unsigned page,int player_x,int player_y,std::uint32_t frame){
        if(b.kind!=EnemyBulletRequest::Bullet16)return;
        const int x=pixel(b.position[page][0]),y=pixel(b.position[page][1]);
        const int aim_x=player_x+8,aim_y=player_y+8;
        auto wrap=[](int v){return enemy_s16(static_cast<unsigned>(v));};
        auto aim=[&](){return enemy_vector(enemy_atan2(wrap(aim_y-y),wrap(aim_x-x)),b.speed);};
        auto chase=[&](int speed){if(x<aim_x)b.velocity[0]=wrap(b.velocity[0]+speed);else if(x>aim_x)b.velocity[0]=wrap(b.velocity[0]-speed);if(y<aim_y)b.velocity[1]=wrap(b.velocity[1]+speed);else if(y>aim_y)b.velocity[1]=wrap(b.velocity[1]-speed);};
        switch(b.motion){
        case 0x80:chase(enemy_s16(unsigned(special.u1)));break;
        case 0x81:++b.counter;if(int(b.counter)<enemy_s16(unsigned(special.u2)))b.velocity=aim();else b.motion=0;break;
        case 0x82:if((frame&7)==0){b.velocity[0]/=2;b.velocity[1]/=2;if(b.velocity[0]==0&&b.velocity[1]==0){b.velocity=aim();++b.counter;if(int(b.counter)>=enemy_s16(unsigned(special.u3)))b.motion=0;}}break;
        case 0x83:b.velocity[1]=wrap(b.velocity[1]+enemy_s16(unsigned(special.u1)));break;
        case 0x84:case 0x85:
            b.angle=static_cast<std::uint8_t>(b.angle+special.u1);
            if(b.motion==0x84)b.speed=static_cast<std::uint8_t>(b.speed+special.u2);
            b.velocity=enemy_vector(b.angle,b.speed);
            if(b.motion==0x85)chase(enemy_s16(unsigned(special.u2)));
            ++b.counter;if(int(b.counter)>enemy_s16(unsigned(special.u3)))b.motion=0;
            break;
        case 0x87:case 0x88:
            if(b.motion==0x87&&(x<40||x>392)){b.velocity[0]=wrap(-b.velocity[0]);++b.counter;}
            if(y<24||y>360){b.velocity[1]=wrap(-b.velocity[1]);++b.counter;}
            if(int(b.counter)>enemy_s16(unsigned(special.u3)))b.motion=0;
            break;
        default:break;
        }
    }
    bool update(unsigned page,int player_x,int player_y,std::uint32_t stage_frame=0,CollisionFrame *display=nullptr,const BoundaryExit &on_exit={}){
        if(page>1)throw std::runtime_error("Invalid bullet page");
        const CollisionPoint player{player_x,player_y};
        if(display)display->sample_player(CollisionSample::Projectile,player);
        const auto pellet_window=bullet_hit_window(player,true),bullet_window=bullet_hit_window(player,false);
        const bool capture_bullets=display&&display->mode==HitboxMode::All;
        bool hit=false;
        for(auto &b:bullets){if(b.flag!=1)continue;const auto old=b.position[page];const auto step=b.velocity;
            for(unsigned c=0;c<2;++c)b.position[page][c]=static_cast<std::uint16_t>(b.position[page][c]+step[c]);
            update_special(b,page,player_x,player_y,stage_frame);
            const int x=pixel(b.position[page][0]),y=pixel(b.position[page][1]);
            const bool left=x<=20,right=x>=416,top=y<=0,bottom=y>=384;
            bool erase=left||right||top||bottom;
            if(erase&&on_exit&&((left&&step[0]<0)||(right&&step[0]>0)||(top&&step[1]<0)||(bottom&&step[1]>0)))on_exit(b,old);
            if(!erase){const bool small=b.kind==EnemyBulletRequest::Pellet;
                if(capture_bullets)display->bullet({x,y},small);
                if((small?pellet_window:bullet_window).contains({x,y})){erase=true;hit=true;}
            }
            if(erase){b.flag=2;++removed;}
        }
        return hit;
    }
};
}
