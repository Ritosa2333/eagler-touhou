#include "AnimalSpirits.hpp"
#include <cmath>
namespace th17 {
AnimalSpirits::~AnimalSpirits(){clear();}
bool AnimalSpirits::clear(){for(auto& e:entries)if(!animations.retire(e.state.animation)){error=animations.error;return false;}entries.clear();return true;}
bool AnimalSpirits::adjust_direction(AnimalSpiritState& s,float angle){
    u32 attempts=0;
    for(;;){s.velocity={float(std::cos(double(angle))),float(std::sin(double(angle))),0};s.position.z=0;bool retry=false;
        for(const auto& entry:entries){const auto& other=entry.state;if(&other==&s)continue;const float dx=float(other.position.x-s.position.x),dy=float(other.position.y-s.position.y),distance=float(float(dx*dx)+float(dy*dy));
            if(distance>=576.f)continue;const float dot=float(float(other.velocity.x*s.velocity.x)+float(other.velocity.y*s.velocity.y));if(dot<.96f)continue;
            attempts++;angle=float(angle+3.883222f);if(attempts<=10){retry=true;break;}}
        if(!retry)return true;
    }
}
u32 AnimalSpirits::spawn(i32 kind,const Vec3& p,float angle,bool practice){
    error.clear();if(entries.size()>=40)return 0;if(kind<1||kind>17){error="Invalid animal spirit kind";return 0;}
    entries.push_back({++generation,{}});auto& entry=entries.back();auto& s=entry.state;s.age.set(0);s.outside.set(0);
    const bool rotating=kind>=15&&kind<=17;s.animation=animations.create(resource,rotating?kind-15:kind-1,-1,0);if(!s.animation){error=animations.error;entries.pop_back();return 0;}
    if(rotating){s.kind=kind-14;s.flags=(s.flags&~4u)|2;s.cycle.set(240);}else{s.kind=kind;if(kind<=3&&!animations.interrupt(s.animation,7)){error=animations.error;return 0;}}
    s.position=p;if(auto* vm=animations.registry.find(s.animation))vm->visual.translation=p;if(!adjust_direction(s,angle))return 0;
    if(kind>=8&&kind<=14){if(!practice&&!host.trophy(kind+25)){error="Animal spirit trophy service failed";return 0;}if(!host.sound(74)){error="Animal spirit audio failed";return 0;}}
    else if(!host.sound(59)){error="Animal spirit audio failed";return 0;}return entry.id;
}
bool AnimalSpirits::advance(AnimalSpiritState& s,const Vec3& player,float rate,bool& finished){
    finished=false;
    if(s.flags&1){s.velocity={float(float(player.x-s.position.x)*.25f),float(float(player.y-s.position.y)*.25f),float(float(player.z-s.position.z)*.25f)};s.position={float(s.position.x+s.velocity.x),float(s.position.y+s.velocity.y),float(s.position.z+s.velocity.z)};
        if(s.age.current>=10){if(!animations.retire(s.animation)){error=animations.error;return false;}finished=true;return true;}}
    else{
        float dx=float(s.position.x-player.x),dy=float(s.position.y-player.y),distance=float(float(dx*dx)+float(dy*dy));
        if(distance<576.f){if(!host.sound(63)||!host.collect_spirit(s.kind)){error="Animal spirit collection failed";return false;}s.flags|=1;if(!animations.interrupt(s.animation,1,true)){error=animations.error;return false;}s.age.set(0);}
        dx=float(player.x-s.position.x);dy=float(player.y-s.position.y);distance=float(float(dx*dx)+float(dy*dy));const float step=distance<=3600.f?.5f:1.f;
        if(s.flags&2){
            if(distance<=3600.f){if(!(s.flags&4)&&!animations.interrupt(s.animation,8)){error=animations.error;return false;}if(s.cycle.current<60)s.cycle.set(90);s.flags|=4;}
            else{s.cycle.decrement(&rate);if(s.flags&4){if(!animations.interrupt(s.animation,s.cycle.current<61?2:3)){error=animations.error;return false;}}s.flags&=~4u;}
            if(s.cycle.current==60){if(!animations.interrupt(s.animation,2)){error=animations.error;return false;}}
            else if(s.cycle.current<1){s.cycle.set(180);s.kind=s.kind%3+1;if(!animations.retire(s.animation)){error=animations.error;return false;}s.animation=animations.create(resource,s.kind-1,-1,0);if(!s.animation){error=animations.error;return false;}if(s.age.current>7799&&!animations.interrupt(s.animation,9,true)){error=animations.error;return false;}if(!host.sound(78)){error="Animal spirit rotation audio failed";return false;}}
        }
        if(std::fabs(s.position.x)>208.f||std::fabs(float(s.position.y-224.f))>240.f){s.outside.tick(&rate);if(s.outside.current>119||s.age.current>129){if(!animations.retire(s.animation)){error=animations.error;return false;}finished=true;return true;}}
        else s.outside.set(0);
        s.position={float(s.position.x+float(step*s.velocity.x)),float(s.position.y+float(step*s.velocity.y)),float(s.position.z+float(step*s.velocity.z))};
        if(s.age.current==7800&&!animations.interrupt(s.animation,9,true)){error=animations.error;return false;}
        if(s.age.current<8400){bool reflected=false;
            if(s.position.x<=-180.f&&s.velocity.x<0){s.velocity.x=float(s.velocity.x*-1.f);reflected=true;}
            if(s.position.x>=180.f&&s.velocity.x>0){s.velocity.x=float(s.velocity.x*-1.f);reflected=true;}
            if(s.position.y<=128.f&&s.velocity.y<0){s.velocity.y=float(s.velocity.y*-1.f);reflected=true;}
            if(s.position.y>=384.f&&s.velocity.y>0){s.velocity.y=float(s.velocity.y*-1.f);reflected=true;}
            if(reflected){for(float* value:{&s.velocity.x,&s.velocity.y})if(std::fabs(*value)<.2f){const bool positive=*value>0;const float unit=random.unit();*value=positive?float(double(unit)*double(.3f)+double(.5f)):float(double(unit)*double(-.3f)-double(.5f));}
                const float angle=s.velocity.x==0.f&&s.velocity.y==0.f?1.5707963705062866f:float(std::atan2(double(s.velocity.y),double(s.velocity.x)));if(!adjust_direction(s,angle))return false;}
        }
    }
    if(auto* vm=animations.registry.find(s.animation))vm->visual.translation=s.position;s.age.tick(&rate);return true;
}
bool AnimalSpirits::update(const Vec3& player,float rate){
    error.clear();animations.rate=rate;
    for(auto at=entries.begin();at!=entries.end();){bool finished=false;if(!advance(at->state,player,rate,finished))return false;if(finished)at=entries.erase(at);else ++at;}return true;
}
}
