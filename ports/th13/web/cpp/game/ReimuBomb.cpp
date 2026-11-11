#include "StageBattle.hpp"
#include "StageMath.hpp"
#include <algorithm>
namespace th13 {
void PlayerBombOrb::step(i32 age,i32 index,float player_x,float player_y,const StageEnemy* target) {
    const float old_x=x,old_y=y;
    const i32 launch=90+index*10;
    if(age<launch) {
        if(age<90)radius+=1;
        angle=wrap_angle(float(double(angle)+double(.10471976f)));
        // 40b590 initializes mover+24 to0x3d490fdb. 4736a0 adds this
        // radial growth in addition to40ae30's first90 updates of +1.
        radius+=.04908738657832146f;
        float dx,dy;polar_xy(angle,radius,dx,dy);
        x=player_x+dx;y=player_y+dy;
    } else {
        if(age==launch) {
            angle=wrap_angle(float(std::atan2(double(velocity_y),double(velocity_x))));
            const float squared=float(double(velocity_x)*velocity_x+double(velocity_y)*velocity_y);
            speed=float(std::sqrt(double(squared)));
        } else if(target) {
            const float dx=target->position[0]-x,dy=target->position[1]-y;
            const float wanted=float(std::atan2(double(dy),double(dx)));
            double delta=double(wanted)-angle;
            if(delta>double(3.1415927f))delta=double(wanted)-float(angle+6.2831855f);
            else if(-delta>double(3.1415927f))delta=double(wanted)-float(angle-6.2831855f);
            const float turn=float(delta);
            if(std::abs(turn)>=.7853982f)speed=std::max(1.f,float(double(speed)-double(.7f)));
            else if(std::abs(turn)<.2617994f)speed=std::min(8.f,float(double(speed)+double(.2f)));
            angle=wrap_angle(wrap_angle(float(float(turn*.1f)+angle)));
        } else if(x<-160||x>160||y<32||y>416)speed*=.9f;
        float dx,dy;polar_xy(angle,speed,dx,dy);x+=dx;y+=dy;
    }
    x=round001(x);y=round001(y);velocity_x=x-old_x;velocity_y=y-old_y;
}
}
