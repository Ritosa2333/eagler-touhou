#include "ItemManager.hpp"
#include "EnemyState.hpp"
#include "AnmRenderer.hpp"
#include <cmath>
namespace th11 {
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
ItemManager::ItemManager(AnmResource& r,AnmEnvironment& a,ItemWorld& w,u16 id)
    :ItemManager(r,a,w,id,false){}
ItemManager::ItemManager(AnmResource& r,AnmEnvironment& a,ItemWorld& w,u16 id,bool defer_storage)
    :resource(r),animations(a),world(w),file_id(id){
    if(!defer_storage)initialize_storage();
}
void ItemManager::initialize_storage(){
    if(storage)return;
    storage.reset(new ItemState[capacity]);
    reset();
}
#else
ItemManager::ItemManager(AnmResource& r,AnmEnvironment& a,ItemWorld& w,u16 id):storage(new ItemState[capacity]),resource(r),animations(a),world(w),file_id(id){reset();}
#endif
void ItemManager::reset()noexcept{std::memset(storage.get(),0,sizeof(ItemState)*capacity);active_count=0;last_error=0;
#if PORT_TITLE == 16
    th16_spawn_count=th16_cancel_count=th16_generation=0;th16_cancel_free=th16_ordinary_free=nullptr;th16_slowdown=1;th16_attract_timer=0;
    for(u32 n=0;n<ordinary_capacity;++n){storage[n].th16_free_next=th16_ordinary_free;th16_ordinary_free=&storage[n];}
    for(u32 n=ordinary_capacity;n<capacity;++n){storage[n].th16_free_next=th16_cancel_free;th16_cancel_free=&storage[n];}
#endif
#if PORT_TITLE == 14
    th14_spawn_count=th14_cancel_count=th14_generation=0;th14_cancel_free=nullptr;
    th14_ordinary_free=nullptr;
    for(u32 n=0;n<ordinary_capacity;++n){storage[n].th14_free_next=th14_ordinary_free;th14_ordinary_free=&storage[n];}
    for(u32 n=ordinary_capacity;n<capacity;++n){storage[n].th14_free_next=th14_cancel_free;th14_cancel_free=&storage[n];}
#endif
}
bool ItemManager::bind(ItemState& item,i32 script){
#if PORT_TITLE == 14
    const bool bound=item.vm.copy_script(resource,script,file_id,&animations.rate);
#else
    const bool bound=item.vm.bind_script(resource,script,file_id,&animations.rate);
#endif
    if(!bound){last_error=1;return false;}
    // The caller sets 16x16 before the original bind, whose second reset clears it.
    if(item.vm.update(animations)<0){last_error=2;return false;}return true;
}
i32 ItemManager::spawn(i32 type,Vec3 position,u32 color,float angle,float speed){
#if PORT_TITLE == 16
    if((type>=9&&type<=14)||type==16)return spawn_cancel_th16(type,position,angle,speed);
    if((type>=1&&type<=8)||type==15)return spawn_ordinary_th16(type,position,angle,speed);
    last_error=1611;return -2;
#endif
#if PORT_TITLE == 14
    if(type==9||type==10||type==11)return spawn_cancel_th14(type,position,angle,speed);
    if((type>=1&&type<=8)||type==12)return spawn_ordinary_th14(type,position,angle,speed);
    last_error=140;return -2; // Ordinary two-VM item allocation is separate in TH14.
#endif
    if(last_error)return -2;
    if(type==8){
        auto& item=storage[ordinary_capacity+cancel_cursor];++cancel_spawn_count;
        if(!item.state){
            item.delay=cancel_spawn_count<256?i32(cancel_cursor%4):cancel_spawn_count<512?i32(cancel_cursor%8+4):cancel_spawn_count<1024?i32(cancel_cursor%16+8):i32(cancel_cursor%32+16);
            item.state=5;item.type=item.display_type=8;item.position=position;
            const auto v=polar(angle,speed);item.velocity={v.x,v.y,0};item.lifetime.set(0,&animations.rate);item.reserved_44c=0;
        }
        cancel_cursor=(cancel_cursor+1)%cancel_capacity;return 0;
    }
    ItemState* selected=nullptr;for(u32 i=0;i<ordinary_capacity;++i)if(!storage[i].state){selected=&storage[i];break;}
    if(!selected)return 0;auto& item=*selected;item.state=1;item.position=position;
    if(!(item.position.x>-192))item.position.x=-192;else if(item.position.x>192)item.position.x=192;
    const auto v=polar(angle,speed);item.velocity={v.x,v.y,0};item.lifetime.set(0,&animations.rate);item.reserved_44c=0;
    if(player.power>=player.max_power&&(type==1||type==4||type==10||type==11))type=9;
    if(type==5&&(!world.effect(item.position,0x73)||!world.sound(0x34,item.position.x))){last_error=3;return -2;}
    item.type=item.display_type=type;if(type==10)item.display_type=1;else if(type==11)item.display_type=4;
    if(!bind(item,item.display_type+0x60))return -2;item.vm.color=color;return 0;
}
bool ItemManager::activate(ItemState& item){
#if PORT_TITLE == 16
    return activate_th16(item);
#else
    item.state=2;return bind(item,kPortTitle==14?item.type+94:item.type+0x60);
#endif
}
void ItemManager::move(ItemState& item){
    const auto axis=[&](float p,float v){return float(double(p)+float(double(v)*animations.rate));};
    item.position={axis(item.position.x,item.velocity.x),axis(item.position.y,item.velocity.y),axis(item.position.z,item.velocity.z)};
}
void ItemManager::home(ItemState& item){
    const float angle=aim_angle(item.position,player.position);const auto v=polar(angle,item.attraction_speed);item.velocity.x=v.x;item.velocity.y=v.y;move(item);
    if(item.attraction_speed<12)item.attraction_speed=float(double(item.attraction_speed)+double(.2f));
    if(player.state==4){item.state=1;item.velocity.x=item.velocity.y=0;}
}
void ItemManager::attract_all(){for(u32 i=0;i<capacity;++i)if(storage[i].state==1){storage[i].state=3;storage[i].attraction_speed=player.attraction_speed;}}
bool ItemManager::convert_power_items(){
    for(u32 i=0;i<ordinary_capacity;++i){auto& item=storage[i];if(item.state&&(item.type==1||item.type==4||item.type==10||item.type==11)){
        const Vec3 position=item.position;item.state=0;
        if(spawn(9,position)<0||!world.effect(position,0x73)){last_error=4;return false;}
    }}return true;
}
bool ItemManager::update(){
#if PORT_TITLE == 14
    return update_th14();
#endif
    if(last_error)return false;active_count=cancel_spawn_count=0;bool convert=false;
#if PORT_TITLE == 16
    th16_cancel_count=0;
#endif
    for(u32 i=0;i<capacity;++i){auto& item=storage[i];if(!item.state)continue;
#if PORT_TITLE == 16
        if((item.type>=1&&item.type<=14)||item.type==16){if(!update_item_th16(item))return false;continue;}
        last_error=1614;return false;
#endif
        if(item.state==5){item.delay=wrapping_sub(item.delay,1);if(item.delay<0&&!activate(item))return false;continue;}
        if(item.state==1){
            const bool fall=(player.state==2||player.state==4||((item.lifetime.current<40||player.communication<10000)&&player.position.y>=128))&&!player.force_attract;
            if(fall){move(item);item.velocity.y=float(double(item.velocity.y)+double(animations.rate)*double(.03f));
                if(item.velocity.y>=0)item.velocity.x=0;if(item.velocity.y>2)item.velocity.y=2;if(item.position.y>472){item.state=0;continue;}
            }else{item.state=3;item.attraction_speed=player.attraction_speed;home(item);}
        }else if(item.state==2){
            move(item);item.velocity.y=float(double(item.velocity.y)+double(animations.rate)*double(.03f));
            if(item.velocity.y>=0){item.state=3;item.attraction_speed=player.attraction_speed;home(item);}
            else if(item.position.y>472){item.state=0;if(!world.rank_delta(-4)){last_error=5;return false;}continue;}
        }else if(item.state==3||item.state==4)home(item);
        if(player.state!=2){
            if(player.pickup.contains(item.position)){
                if(!world.collect(item,convert)){last_error=6;return false;}item.state=0;
                if(!world.sound(0x14,item.position.x)){last_error=7;return false;}continue;
            }
            if(item.state!=3&&item.state!=4&&(player.focused?player.focused_attract:player.unfocused_attract).contains(item.position)){
                item.state=4;item.attraction_speed=float(double(player.attraction_speed)/3);
            }
        }
        if(item.vm.update(animations)<0){last_error=8;return false;}item.lifetime.tick();++active_count;
    }
    return !convert||convert_power_items();
}
bool ItemManager::scatter(EnemyDrop& drops,Vec3 position){
#if PORT_TITLE == 14
    return scatter_th14(drops,position);
#endif
    auto& rng=animations.script_rng;constexpr double pi=3.1415927410125732421875,half_pi=1.57079637050628662109375;
    float angle=float(double(rng.signed_unit())*pi);
    for(i32 type=1;type<=11;++type)for(i32 n=0;n<drops.counts[type-1];++n){
        const Vec2 offset{float(std::cos(double(angle))*drops.spread.x),float(std::sin(double(angle))*drops.spread.y)};
        const float radius=float(double(rng.unit())*.5+.5);
        const Vec3 p{float(double(position.x)+float(double(radius)*offset.x)),float(double(position.y)+float(double(radius)*offset.y)),float(double(position.z)+0.)};
        if(spawn(type,p)<0)return false;
        const float random_angle=float(double(rng.signed_unit())*pi);
        angle=normalize_angle(float(double(angle)+half_pi+double(random_angle)*.25));
    }
    for(auto& n:drops.counts)n=0;return true;
}
bool ItemManager::drop(EnemyDrop& drops,Vec3 position){
    if((kPortTitle==14?drops.primary!=0:drops.primary>0)&&spawn(drops.primary,position)<0)return false;
    if(!scatter(drops,position))return false;drops.primary=0;return true;
}
bool ItemManager::draw(AnmRenderer& renderer){
#if PORT_TITLE == 14
    return draw_th14(renderer);
#endif
    for(u32 i=0;i<capacity;++i){auto& item=storage[i];if(!item.state)continue;auto& vm=item.vm;
        vm.script_position={float((double(item.position.x)+32)+192),float(double(item.position.y)+16),item.position.z};
        if(vm.script_position.y<8){
            const float distance=float(double(vm.script_position.y)-8);vm.script_position.y=24;
            const u8 alpha=distance>=32?255:u8(truncate_int((double(distance)*.03125)*255));
            vm.color=(vm.color&0xffffff)|(u32(alpha)<<24);
            if(vm.sprite_index!=item.display_type+0x162){vm.resource=&resource;if(!vm.bind_sprite(item.display_type+0x161))return false;}
        }else if(vm.sprite_index!=item.display_type+0x159){
            vm.resource=&resource;if(!vm.bind_sprite(item.display_type+0x158))return false;vm.color|=0xff000000;
        }
        if(renderer.draw(vm)==-2)return false;
    }return true;
}
}
