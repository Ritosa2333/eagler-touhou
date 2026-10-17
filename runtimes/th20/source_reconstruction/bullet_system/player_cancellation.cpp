#include "player_cancellation.hpp"
namespace th20::source::bullet {
void cancel_near_circle(Controller& owner,const sprite::Vec3& center,float radius,int drop_mode){
    scheduler::Iterator iterator(owner.active.sentinel.next);
    for(;iterator.current;iterator.advance()){
        auto& value=*reinterpret_cast<Bullet*>(iterator.current->value);
        if((value.state==1||value.state==2)&&in_circle(value.position,center,value.size.x/2.f+radius)){
            cancel(value,drop_mode);
            ++owner.cancel_counter;
        }
    }
}
}
