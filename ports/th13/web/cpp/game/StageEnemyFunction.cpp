#include "StageSim.hpp"
#include "StageMath.hpp"

namespace th13 {
namespace {
// 423d40 returns the signed shortest difference before its caller adds
// the reflection axis. Preserve those stores rather than using pi-angle.
float difference(float target,float angle) {
    if(float(target-angle)>3.1415927f)return float(double(target)-(double(angle)+6.2831855f));
    if(float(angle-target)>3.1415927f)return float(double(target)-(double(angle)-6.2831855f));
    return target-angle;
}
float reflected(float axis,float angle){return wrap_angle(float(double(difference(axis,angle))+axis));}
}
bool StageSim::invoke_enemy_function(StageEnemy& enemy,i32 index) {
    if(index==0)return true;
    if(index==4) {
        if(enemy_absorb_spirits)return enemy_absorb_spirits(enemy);
        error_="Enemy spirit absorption host unavailable";return false;
    }
    if(index==6) {
        // 422e40 saves next before4183f0. Newly emitted bullets are not
        // substituted for the triggering node if its vector reallocates.
        const size_t count=bullets.size();
        for(size_t i=0;i<count;++i) {
            const StageBullet source=bullets[i];
            if(!source.alive||source.native_trigger||std::abs(float(source.y-player_y))>=4)continue;
            StageSpawnArguments args;args.z=source.z;
            std::copy_n(enemy.i_slots,4,args.integers.begin());
            std::copy_n(enemy.f_slots,args.floats.size(),args.floats.begin());
            spawn_enemy("BossCard2_EtAt",source.x,source.y,10000,nullptr,false,false,&args);
            if(!error_.empty())return false;
            auto& bullet=bullets[i];bullet.native_trigger=1;
            if(!cancel_bullet(bullet))return false;
        }
        return true;
    }
    if(index!=1&&index!=2&&index!=3&&index!=5) {
        error_="Unimplemented enemy native function "+std::to_string(index);return false;
    }
    const float left=enemy.position[0]+enemy.f_slots[0],right=enemy.position[0]+enemy.f_slots[1];
    const float top=enemy.position[1]+enemy.f_slots[2],bottom=enemy.position[1]+enemy.f_slots[3];
    for(auto& bullet:bullets) {
        if(!bullet.alive)continue;
        if(index==5) {
            const float dx=player_x-bullet.x,dy=player_y-bullet.y;
            if(!bullet.native_trigger&&double(dx)*dx+double(dy)*dy<
                    double(enemy.f_slots[0])*enemy.f_slots[0]) {
                bullet.native_trigger=1;bullet.ex_cursor=5;
            }
            continue;
        }
        const float old_x=float(double(bullet.x)-double(bullet.vel[0])*2);
        const float old_y=float(double(bullet.y)-double(bullet.vel[1])*2);
        bool changed=false;
        auto bounce_x=[&](float wall){bullet.angle=reflected(-1.5707964f,bullet.angle);
            bullet.x=float(double(wall)+wall-bullet.x);changed=true;};
        auto bounce_y=[&](float wall){bullet.angle=reflected(0,bullet.angle);
            bullet.y=float(double(wall)+wall-bullet.y);changed=true;};
        if(index==1) {
            if(double(left)-8<=old_x&&old_x<=double(right)+8&&
               double(top)-8<=old_y&&old_y<=double(bottom)+8) {
                if(bullet.x<left&&left<=old_x)bounce_x(left);
                if(right<bullet.x&&old_x<=right)bounce_x(right);
                if(bullet.y<top&&top<=old_y)bounce_y(top);
                if(bottom<bullet.y&&old_y<=bottom)bounce_y(bottom);
            }
        } else if(index==2) {
            if(bullet.x<-192&&old_x>=-192)bounce_x(-192);
            if(bullet.x>192&&old_x<=192)bounce_x(192);
            if(bullet.x>enemy.f_slots[0]&&old_x<=enemy.f_slots[0])bounce_x(enemy.f_slots[0]);
            if(bullet.x<enemy.f_slots[1]&&old_x>=enemy.f_slots[1])bounce_x(enemy.f_slots[1]);
        } else {
            const float dx=enemy.f_slots[1]-bullet.x,dy=enemy.f_slots[2]-bullet.y;
            const float px=enemy.f_slots[1]-old_x,py=enemy.f_slots[2]-old_y;
            const float radius2=enemy.f_slots[0]*enemy.f_slots[0];
            if(double(dx)*dx+double(dy)*dy>radius2&&
               float(double(px)*px+double(py)*py)<=radius2) {
                const float tangent=wrap_angle(float(float(std::atan2(double(-dy),double(-dx)))+1.5707964f));
                bullet.angle=reflected(tangent,bullet.angle);changed=true;
            }
        }
        if(changed) {
            polar_xy(bullet.angle,bullet.speed,bullet.vel[0],bullet.vel[1]);
            if(bullet.transform_sound>=0&&enemy_sound&&!enemy_sound(bullet.transform_sound)) {
                error_="Enemy native reflection sound failed";return false;
            }
        }
    }
    return true;
}
}
