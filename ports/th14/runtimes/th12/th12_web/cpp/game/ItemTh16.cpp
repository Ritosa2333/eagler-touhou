#include "ItemManager.hpp"
#include "BulletTh16.hpp"
#include <cmath>
namespace th11 {
#if PORT_TITLE == 16
i32 ItemManager::spawn_ordinary_th16(i32 type,Vec3 p,float angle,float speed,i32 force_attract){
    if(last_error)return -2;if((type<1||type>8)&&type!=15){last_error=1611;return -2;}
    // Own430960 ordinary branch.15 counts a fragment drop and binds as6.
    ++th16_spawn_count;auto* selected=th16_ordinary_free;if(!selected)return 0;auto& i=*selected;
    i.state=1;i.position=p;if(i.position.x<=-192.f)i.position.x=-192.f;else if(i.position.x>=192.f)i.position.x=192.f;
    if(type==15){auto& count=world.item_th16_drop_count();count=wrapping_add(count,1);type=6;}
    const auto v=polar(angle,speed);i.velocity={v.x,v.y,0};i.lifetime.set(0,&animations.rate);
    i.reserved_44c=0;i.attraction_speed=0;i.delay=0;i.type=type;i.display_type=0;
    if((type>=4&&type<=7)&&!world.item_th16_spawn_notice(i.position,type)){last_error=1612;return -2;}
    if(!bind(i,type+111))return -2;
    if(!i.th16_indicator.bind_script(resource,type+133,file_id,&animations.rate)||i.th16_indicator.update(animations)<0){last_error=1613;return -2;}
    i.th16_force_attract=force_attract;i.vm.color=0xffffffff;
    th16_ordinary_free=i.th16_free_next;i.th16_free_next=nullptr;return 0;
}
i32 ItemManager::spawn_cancel_th16(i32 type,Vec3 position,float angle,float speed,i32 force_attract){
    if(last_error)return -2;
    if((type<9||type>14)&&type!=16){last_error=1601;return -2;}
    // Full430960 special branch. The counter advances even with a full pool.
    ++th16_spawn_count;auto* selected=th16_cancel_free;if(!selected)return 0;
    auto& item=*selected;item.th16_generation=th16_generation;
    const i32 count=signed_bits(th16_spawn_count),load=signed_bits(th16_cancel_count);
    item.delay=load>=1024?count%32+16:load>=512?count%16+8:load>=256?count%8+4:count%4;
    item.state=6;item.type=item.display_type=type;item.position=position;
    const auto vector=polar(angle,speed);item.velocity={vector.x,vector.y,0};
    item.lifetime.set(0,&animations.rate);item.reserved_44c=speed;item.th16_angle=angle;item.th16_force_attract=force_attract;
    th16_cancel_free=item.th16_free_next;item.th16_free_next=nullptr;return 0;
}
void ItemManager::release_th16(ItemState& item)noexcept{
    // Own430d90 prepends to the item's original free-list head.
    auto& head=(&item-storage.get())<ordinary_capacity?th16_ordinary_free:th16_cancel_free;
    item.state=0;item.th16_free_next=head;head=&item;
}
bool ItemManager::activate_th16(ItemState& item){
    // Retail430c90: season16 uses selected season+129 and state3.
    // Its season selection is kept separately from player character/loadout.
    if((item.type<9||item.type>14)&&item.type!=16){last_error=1603;return false;}
    item.state=item.type==16?3:2;
    if(!bind(item,item.type==16?th16_season+129:item.type+111))return false;
    item.vm.parent=nullptr;item.th16_indicator.flags&=~1u;
    item.th16_indicator.instruction=nullptr;return true;
}
bool ItemManager::update_item_th16(ItemState& i){
    // Own42f4e0 item bodies. Pickup rewards and VM consumers are separate.
    if(i.state==6){i.delay=wrapping_sub(i.delay,1);return i.delay>=0||activate_th16(i);}
    const auto step=[&](bool slow){
        float x=i.velocity.x*animations.rate,y=i.velocity.y*animations.rate,z=i.velocity.z*animations.rate;
        if(slow){x=x*th16_slowdown;y=y*th16_slowdown;z=z*th16_slowdown;}
        i.position.x=i.position.x+x;i.position.y=i.position.y+y;i.position.z=i.position.z+z;
    };
    const bool attract=(player.state!=2&&player.state!=4&&player.position.y<(player.character==3?148.f:128.f))||player.bomb_attract||player.season_attract||player.spell_attract;
    const auto home_item=[&](){
        if(i.state==4&&(i.type<9||i.type>14))th16_attract_timer=8;
        const float dx=player.position.x-i.position.x,dy=player.position.y-i.position.y;
        const float angle=dx==0&&dy==0?1.5707963705062866f:float(std::atan2(double(dy),double(dx)));
        const auto v=polar(angle,i.attraction_speed);i.velocity.x=v.x;i.velocity.y=v.y;step(false);
        if(i.attraction_speed<12)i.attraction_speed=i.attraction_speed+.2f;
        if(player.state==4){i.state=1;i.velocity.x=i.velocity.y=0;}
    };
    const auto outside=[&](){return i.position.y>472.f||!(std::fabs(i.position.x)<200.f);};
    bool fall_body=i.state==1;
    if(i.state==2){
        step(false);i.velocity.y=i.velocity.y+animations.rate*.03f;
        if(!(i.velocity.y<0)){
            i.attraction_speed=player.attraction_speed;
            if(i.type==16){i.state=1;fall_body=true;}else{i.state=4;home_item();}
        }else if(outside()){release_th16(i);return true;}
    }else if(i.state==3){
        step(false);i.velocity.y=i.velocity.y+animations.rate*.03f;
        i.reserved_44c=i.reserved_44c-.03f;
        const auto v=polar(i.th16_angle,i.reserved_44c);i.velocity.x=v.x;i.velocity.y=v.y;
        if(player.bomb_attract||player.season_attract)i.th16_force_attract=1;
        if(i.reserved_44c<=0){
            i.velocity={};i.reserved_44c=0;i.th16_angle=1.5707963705062866f;
            i.attraction_speed=player.attraction_speed;i.state=i.th16_force_attract?4:1;fall_body=true;
        }else if(outside()){release_th16(i);return true;}
    }else if(i.state==4)home_item();
    else if(i.state==5){
        if(attract){i.attraction_speed=player.attraction_speed;i.state=4;}
        home_item();
    }else if(!fall_body){last_error=1602;return false;}
    if(fall_body){
        if(i.delay>0){i.delay=wrapping_sub(i.delay,1);if(i.delay==0){last_error=1608;return false;}return true;}
        if(attract){i.attraction_speed=player.attraction_speed;i.state=4;home_item();}
        else{
            if(i.lifetime.current>=32&&th16_slowdown<1){last_error=1606;return false;}
            step(true);float acceleration=animations.rate*.03f;acceleration=acceleration*th16_slowdown;
            i.velocity.y=i.velocity.y+acceleration;
            if(i.lifetime.current>=32)i.vm.color=0xffffffff;
            if(!(i.velocity.y<0))i.velocity.x=0;
            if(i.velocity.y>2.f)i.velocity.y=2.f;
            if(outside()){release_th16(i);return true;}
        }
    }
    if(player.state!=2){
        if(player.pickup.contains(i.position)){
            if(!world.item_th16_collect(i)){if(!last_error)last_error=1604;return false;}
            if(!world.sound(37,i.position.x)){last_error=1607;return false;}
            release_th16(i);return true;
        }
        if(i.state!=3&&i.state!=4&&i.state!=5&&i.type==16&&(player.focused?player.focused_attract:player.unfocused_attract).contains(i.position)){
            i.state=5;i.attraction_speed=player.attraction_speed/3.f;
        }
    }
    if((i.vm.flags&1)&&i.vm.update(animations)<0){last_error=1609;return false;}
    if((i.th16_indicator.flags&1)&&i.th16_indicator.update(animations)<0){last_error=1610;return false;}
    bullet_th16_clock(i.lifetime,i.lifetime.rate?*i.lifetime.rate:1.f);++active_count;return true;
}
#endif
}
