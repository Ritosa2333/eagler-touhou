#include "ItemManager.hpp"
#include "AnmRenderer.hpp"
#include "EnemyState.hpp"
#include <cmath>
namespace th11 {
#if PORT_TITLE == 14
i32 ItemManager::spawn_ordinary_th14(i32 type,Vec3 p,float angle,float speed,i32 delay){
    if(last_error)return -2;++th14_spawn_count;auto* selected=th14_ordinary_free;if(!selected)return 0;
    auto& i=*selected;i.state=1;i.position=p;
    if(i.position.x<=-192.f)i.position.x=-192.f;else if(i.position.x>=192.f)i.position.x=192.f;
    if(type==12){auto& count=world.item_th14_drop_counter();count=wrapping_add(count,1);type=count%5==0?4:6;}
    const auto v=polar(angle,speed);i.velocity={v.x,v.y,0};i.lifetime.set(0,&animations.rate);
    i.reserved_44c=0;i.attraction_speed=0;i.delay=delay;i.type=type;i.display_type=0;
    if(!delay&&(type==4||type==5||type==6||type==7)&&!world.item_th14_spawn_notice(i.position,type)){last_error=150;return -2;}
    if(!bind(i,type+94))return -2;
    if(!i.th14_indicator.copy_script(resource,type+105,file_id,&animations.rate)||i.th14_indicator.update(animations)<0){last_error=147;return -2;}
    i.vm.color=0xffffffff;th14_ordinary_free=i.th14_free_next;i.th14_free_next=nullptr;return 0;
}
bool ItemManager::scatter_th14(EnemyDrop& drops,Vec3 position){
    auto& rng=animations.visual_rng;constexpr float pi=3.1415927410125732422f,half_pi=1.5707963705062866211f;
    float angle=rng.signed_unit()*pi;
    // 4215b0 scatters types 1..12, then clears all thirteen count words.
    for(i32 type=1;type<=12;++type)for(i32 n=0;n<drops.counts[type-1];++n){
        const Vec2 offset{float(std::cos(double(angle))*drops.spread.x),float(std::sin(double(angle))*drops.spread.y)};
        const float scaled=rng.unit()*.5f,radius=scaled+.5f;
        const float x=radius*offset.x,y=radius*offset.y;
        const Vec3 p{position.x+x,position.y+y,position.z+0.f};
        if(spawn(type,p)<0)return false;
        const float turn=rng.signed_unit()*pi,bias=angle+half_pi,noise=turn*.25f;
        angle=normalize_angle(bias+noise);
    }
    std::memset(drops.counts,0,13*4);return true;
}
i32 ItemManager::spawn_cancel_th14(i32 type,Vec3 p,float angle,float speed){
    if(last_error)return -2;
    ++th14_spawn_count;auto* selected=th14_cancel_free;if(!selected)return 0;
    auto& item=*selected;item.th14_generation=th14_generation;
    const i32 count=signed_bits(th14_spawn_count),load=signed_bits(th14_cancel_count);
    item.delay=load>=1024?count%32+16:load>=512?count%16+8:load>=256?count%8+4:count%4;
    item.state=5;item.type=item.display_type=type;item.position=p;
    const auto v=polar(angle,speed);item.velocity={v.x,v.y,0};
    item.lifetime.set(0,&animations.rate);item.reserved_44c=0;
    th14_cancel_free=item.th14_free_next;item.th14_free_next=nullptr;return 0;
}
bool ItemManager::cancel_reward_th14(Vec3 p){
    // Retail 4034b0 is signed_unit, not unit. SSE rounds both angle operations.
    const float offset=animations.visual_rng.signed_unit()*.17453292012214660645f;
    const float angle=offset-1.5707963705062866211f;
    return spawn_cancel_th14(10,p,angle,2.2f)==0;
}
bool ItemManager::cancel_reward_th14(Vec3 p,u32 rewards,u32 count,u32 spell_flags){
    // 41ae00 increments the caller's counter before dispatch, including types
    // with no allocation. Type 3 consumes two RNG draws even if the pool is full.
    if(rewards==1)return cancel_reward_th14(p);
    if(rewards==3)return cancel_reward_th14(p)&&cancel_reward_th14(p);
    if(rewards!=2||(spell_flags&1))return true;
    if(signed_bits(count)%5==0&&spawn_ordinary_th14(1,p,-1.5707963705062866211f,2.2f)<0)return false;
    return cancel_reward_th14(p);
}
void ItemManager::release_th14(ItemState& item)noexcept{
    auto& free=(&item-storage.get())<ordinary_capacity?th14_ordinary_free:th14_cancel_free;
    item.state=0;item.th14_free_next=free;free=&item;
}
bool ItemManager::update_th14(){
    if(last_error)return false;active_count=0;th14_cancel_count=0;
    const auto step=[&](ItemState& i){
        const float x=i.velocity.x*animations.rate,y=i.velocity.y*animations.rate,z=i.velocity.z*animations.rate;
        i.position.x=i.position.x+x;i.position.y=i.position.y+y;i.position.z=i.position.z+z;
    };
    const auto home_item=[&](ItemState& i){
        if(i.state==3&&i.type!=9&&i.type!=10&&i.type!=11)world.item_th14_combo_state().timer=8;
        const float dx=player.position.x-i.position.x,dy=player.position.y-i.position.y;
        const float angle=dx==0&&dy==0?1.5707963705062866f:float(std::atan2(double(dy),double(dx)));
        const auto v=polar(angle,i.attraction_speed);i.velocity.x=v.x;i.velocity.y=v.y;step(i);
        if(i.attraction_speed<12.f)i.attraction_speed=i.attraction_speed+.2f;
        if(player.state==4){i.velocity.x=i.velocity.y=0;i.state=1;}
    };
    for(u32 n=0;n<capacity;++n){auto& i=storage[n];if(!i.state)continue;
        const bool ordinary=i.type>=1&&i.type<=8;
        const float line=player.character==1?148.f:128.f;
        const bool attract=(player.state!=2&&player.state!=4&&player.position.y<line)||player.bomb_attract||player.force_attract;
        if(!ordinary&&(i.type<9||i.type>11)){last_error=141;return false;}
        if(i.state==5){
            i.delay=wrapping_sub(i.delay,1);if(i.delay<0&&!activate(i))return false;
            // Activation skips movement, pickup, lifetime and active-count increment.
            continue;
        }
        if(i.state==1){
            if(i.delay>0){i.delay=wrapping_sub(i.delay,1);
                if(!i.delay&&(i.type==4||i.type==5||i.type==6||i.type==7)&&!world.item_th14_spawn_notice(i.position,i.type)){last_error=150;return false;}continue;}
            if(attract){
                i.attraction_speed=player.attraction_speed;i.state=3;home_item(i);
            }
            else{
                step(i);const float acceleration=animations.rate*.03f;i.velocity.y=i.velocity.y+acceleration;
                if(i.velocity.y>=0)i.velocity.x=0;
                const float terminal=player.character==2?1.9f:2.f;if(i.velocity.y>terminal)i.velocity.y=terminal;
                if(i.position.y>472){release_th14(i);continue;}
            }
        }else if(i.state==2){
            step(i);const float acceleration=animations.rate*.03f;i.velocity.y=i.velocity.y+acceleration;
            if(i.velocity.y>=0){i.attraction_speed=player.attraction_speed;i.state=3;home_item(i);}
            else if(i.position.y>472){release_th14(i);continue;}
        }else if(i.state==3)home_item(i);
        else if(i.state==4){if(attract){i.attraction_speed=player.attraction_speed;i.state=3;}home_item(i);}
        else{last_error=142;return false;}
        if(player.state!=2&&player.pickup.contains(i.position)){
            th14_last_collect_type=i.type;bool convert=false;if(!world.collect(i,convert)){last_error=143;return false;}
            if(i.type!=9&&!world.sound(37,i.position.x)){last_error=144;return false;}
            release_th14(i);continue;
        }
        if(player.state!=2&&i.state!=3&&i.state!=4&&ordinary){
            const auto& bounds=player.focused?player.focused_attract:player.unfocused_attract;
            if(bounds.contains(i.position)){i.state=4;i.attraction_speed=player.attraction_speed/3.f;}
        }
        if((i.vm.flags&1)&&i.vm.update(animations)<0){last_error=145;return false;}
        if((i.th14_indicator.flags&1)&&i.th14_indicator.update(animations)<0){last_error=149;return false;}
        i.lifetime.tick();++active_count;
    }
    auto& combo=world.item_th14_combo_state();
    if(combo.timer>0){combo.timer=wrapping_sub(combo.timer,1);if(!combo.timer&&!world.item_th14_combo_settle()){last_error=151;return false;}}
    return true;
}
bool ItemManager::draw_th14(AnmRenderer& renderer){
    // 439640 copies raw playfield coordinates; bound ANM scripts own the origin.
    for(u32 n=0;n<capacity;++n){auto& i=storage[n];if(!i.state||!(i.vm.flags&1)||i.delay>0)continue;
        i.vm.script_position=i.position;
        i.th14_indicator.script_position=i.position;
        if(i.position.y<-8){
            auto& vm=i.th14_indicator;
            if(vm.flags&1){
                const float distance=vm.script_position.y+8.f;vm.script_position.y=8.f;
                const float normalized=distance*.03125f,scaled=normalized*255.f;
                const u8 alpha=distance>=32.f?255:u8(truncate_int(scaled));vm.color=(vm.color&0xffffff)|(u32(alpha)<<24);
                if(renderer.draw(vm)==-2)return false;
            }
            i.display_type=1;continue;
        }
        if(renderer.draw(i.vm)==-2)return false;i.display_type=0;
    }return true;
}
#endif
}
