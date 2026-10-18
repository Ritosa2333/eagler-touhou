#include "ItemManager.hpp"
#include <cmath>
namespace th15 {
namespace {
bool special(i32 kind)noexcept{return kind==9||kind==10||kind==11||kind==13||kind==14||kind==15;}
bool contains(const PlayerBox& box,const Vec3& p)noexcept{return float(p.x+0.f)>=box.minimum.x&&float(p.y+0.f)>=box.minimum.y&&float(p.x-0.f)<=box.maximum.x&&float(p.y-0.f)<=box.maximum.y;}
void translate(ItemState& s,float rate,float scale=1)noexcept{s.position.x=float(s.position.x+float(float(s.velocity.x*rate)*scale));s.position.y=float(s.position.y+float(float(s.velocity.y*rate)*scale));s.position.z=float(s.position.z+float(float(s.velocity.z*rate)*scale));}
void approach(ItemState& s,const Vec3& player,float rate)noexcept{const float dy=float(player.y-s.position.y),dx=float(player.x-s.position.x),angle=dy==0.f&&dx==0.f?1.5707963705062866f:float(std::atan2(double(dy),double(dx)));s.velocity={float(std::cos(double(angle))*double(s.attraction_speed)),float(std::sin(double(angle))*double(s.attraction_speed)),0};translate(s,rate);if(s.attraction_speed<12.f)s.attraction_speed=float(s.attraction_speed+.2f);}
}
bool ItemManager::update(ItemFrameContext& c){
    if(c.character<0||c.character>3){error="Invalid item frame character";return false;}animations.rate=c.rate;cancel_density=0;active_count=0;
    const auto auto_collect=[&]{return (c.player_state!=2&&c.player_state!=4&&c.player.position.y<float(c.character==1?148:128))||(c.bomb_state==1&&c.bomb_frame<60)||c.hud_collect;};
    for(u32 index=0;index<pool_size;index++){
        auto& slot=slots[index];auto& s=slot.state;if(!s.state)continue;
        if(s.state==5){s.delay=wrapping_sub(s.delay,1);if(s.delay<0&&!falling(index+1))return false;continue;}
        bool skip_collection=false;
        if(s.state==1){
            if(s.delay>0){s.delay=wrapping_sub(s.delay,1);if(s.delay<1&&!piece_effect(s))return false;continue;}
            if(auto_collect()){s.state=3;s.attraction_speed=c.attraction_speed;}
            else{translate(s,c.rate,motion_scale);s.velocity.y=float(s.velocity.y+float(float(c.rate*.03f)*motion_scale));
                if(s.age.current>31){if(!slot.body){error="Active item animation unavailable";return false;}auto& body=*slot.body;if(motion_scale>=1.f){body.visual.color=0xffffffff;body.visual.flags=(body.visual.flags&0xc1ffffffu)|4;body.variables.rotation.z=0;}else{const float y=c.visual_random.signed_unit(),x=c.visual_random.signed_unit();body.variables.position={x,y,0};body.visual.color=0xffffa0ff;}}
                if(s.velocity.y>=0)s.velocity.x=0;if(s.velocity.y>2)s.velocity.y=2;if(s.position.y>472){if(!release(index+1))return false;continue;}}
        }else if(s.state==2){translate(s,c.rate);s.velocity.y=float(s.velocity.y+float(c.rate*.03f));if(s.velocity.y>=0){s.state=3;s.attraction_speed=c.attraction_speed;}else{if(s.position.y>472){if(!release(index+1))return false;continue;}skip_collection=s.kind==13||s.kind==14||s.kind==15;}}
        else if(s.state==4){if(auto_collect()){s.state=3;s.attraction_speed=c.attraction_speed;}else{approach(s,c.player.position,c.rate);if(c.player_state==4){s.velocity.x=s.velocity.y=0;s.state=1;}}}
        if(s.state==3){if(!special(s.kind))c.score.collection_timer=8;approach(s,c.player.position,c.rate);if(c.player_state==4){s.velocity.x=s.velocity.y=0;s.state=1;}}
        if(!skip_collection&&c.player_state!=2){
            if(contains(c.bounds.item_near,s.position)){if(!c.collection.award(s)){error=c.collection.error;return false;}if(s.kind!=9&&(!immediate_sound||!immediate_sound(37))){error="Item pickup sound unavailable";return false;}if(!release(index+1))return false;continue;}
            if(s.state!=3&&s.state!=4&&!special(s.kind)&&contains(c.held&8?c.bounds.graze:c.bounds.item_full,s.position)){s.state=4;s.attraction_speed=float(c.attraction_speed/3.f);}
        }
        for(AnmVm* vm:{slot.body.get(),slot.arrow.get()})if(vm&&vm->visual.visible()&&animations.tick_instance(*vm)<0){error=animations.error;return false;}
        s.age.tick(&c.rate);active_count=wrapping_add(active_count,1);
    }
    if(motion_scale<1.f)motion_scale=float(motion_scale+.1f);return true;
}
}
