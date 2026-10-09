#include "BulletManager.hpp"
#include "BulletLaunch.hpp"
#include "AnmRenderer.hpp"
#include "Movement.hpp"
namespace th11 {
namespace {
i32 sprite_callback(AnmVm& vm,i32 frame){
    const auto& b=*reinterpret_cast<BulletState*>(reinterpret_cast<u8*>(&vm)-offsetof(BulletState,vm));
    const auto& a=bullet_appearances[b.sprite];if(a.sprites[0]<0)return frame;
    const i32 index=b.color*3+frame;return index>=0&&index<48?a.sprites[index]:-1;
}
bool outside(const Vec3& p,double half_width,double half_height,double top){
    return !(double(p.x)+half_width>-192&&double(p.x)-half_width<192&&double(p.y)+half_height>top&&double(p.y)-half_height<448);
}
void move(BulletState& b,float rate,bool half){
    const auto step=[&](float v){const float delta=float(double(v)*rate);return half?float(double(delta)*.5):delta;};
    b.position.x=float(double(b.position.x)+step(b.velocity.x));b.position.y=float(double(b.position.y)+step(b.velocity.y));b.position.z=float(double(b.position.z)+step(b.velocity.z));
}
}
BulletManager::BulletManager(AnmResource& r,AnmEnvironment& a,BulletWorld& w,u16 id):storage(new BulletState[capacity]),resource(r),animations(a),world(w),file_id(id){
    std::memset(storage.get(),0,sizeof(BulletState)*capacity);rate=animations.rate;
}
bool BulletManager::appearance(BulletState& b,i32 type,i32 color,bool interrupt){
    if(u32(type)>=bullet_appearances.size()||color<0||color>=16)return false;
    const auto& a=bullet_appearances[type];if(a.effect_mode==1&&color>=8)return false;
    b.sprite=i16(type);b.color=i16(color);b.hitbox={a.hitbox,a.hitbox};
    if(!b.vm.bind_script(resource,a.script,file_id,&animations.rate))return false;
    b.vm.sprite_callback=sprite_callback;b.vm.reserved_430=u32(uintptr_t(&b));
    // Original bind executes the first animation frame before the spawn interrupt.
    if(b.vm.update(animations)<0)return false;
    b.flags&=~0x10u;
    switch(a.effect_mode){case 0:b.cancel_effect=color*2+2;break;case 1:b.cancel_effect=bullet_cancel_scripts[color];break;case 2:b.cancel_effect=-1;b.flags|=0x10;break;case 3:b.cancel_effect=14;break;case 4:b.cancel_effect=4;break;}
    if(interrupt)b.vm.pending_interrupt=2;return true;
}
void BulletManager::reset()noexcept{
#ifdef TH11_MULTIPLAYER
    multiplayer_grazed.fill(0);
#endif
    std::memset(storage.get(),0,sizeof(BulletState)*capacity);
    heads.fill(nullptr);tails.fill(nullptr);cursor=active_count=fire_depth=0;last_error=0;
}
bool BulletManager::change_appearance(BulletState& b,i32 type,i32 color,bool interrupt){return appearance(b,type,color,interrupt);}
i32 BulletManager::spawn(const BulletEmitter& e,i32 index,i32 layer,float aim)noexcept{
    if(last_error)return -2;u32 slot=cursor,visited=0;
    while(storage[slot].state!=0){slot=(slot+1)%capacity;if(++visited==capacity)return 1;}
    auto& b=storage[slot];BulletLaunch launch;
#ifdef TH11_MULTIPLAYER
    multiplayer_grazed[slot]=0;
#endif
    const bool accepted=bullet_launch(e,index,layer,aim,animations.script_rng,player,exclusion_squared,launch);
    b.position=launch.position;b.speed=launch.speed;b.angle=launch.angle;if(!accepted)return -1;
    b.flags|=1;b.state=1;b.lifetime.set(0,&rate);b.auxiliary_timer.set(0,&rate);
    b.velocity.x=launch.velocity.x;b.velocity.y=launch.velocity.y;b.active_transforms=e.flags;
    b.reserved_4c0=0;b.flags=(b.flags&~12u)|2u;
    if(!appearance(b,e.sprite,e.color,false)){last_error=1;return -2;}
    b.draw_group=bullet_appearances[e.sprite].group;b.transform_sound=e.transform_sound;b.offscreen_grace=10;
    if(e.transforms[0].opcode==2){b.vm.pending_interrupt=i16(e.transforms[0].c+7);b.state=2;
        b.position.x=float(double(b.position.x)-float(double(b.velocity.x)*4));b.position.y=float(double(b.position.y)-float(double(b.velocity.y)*4));b.position.z=float(double(b.position.z)-float(double(b.velocity.z)*4));
    }else b.vm.pending_interrupt=2;
    std::memcpy(b.transforms,e.transforms,sizeof b.transforms);b.emitter_flags=e.flags;b.active_transforms=0;b.transform_index=e.transform_start;
    if(bullet_start_transforms(b,*this)<0){last_error=2;return -2;}
    if(b.vm.update(animations)<0){last_error=3;return -2;}
    cursor=(slot+1)%capacity;return 0;
}
bool BulletManager::fire(const BulletEmitter& e){
    if(last_error||fire_depth>=64){last_error=4;return false;}++fire_depth;
#ifdef TH11_MULTIPLAYER
    if(world.multiplayer_world())player=world.multiplayer_target(e.position);
#endif
    const float aim=bullet_aim(e.position,player);bool full=false;
    for(i32 layer=0;layer<e.layers&&!full;++layer)for(i32 index=0;index<e.count;++index){const i32 result=spawn(e,index,layer,aim);if(result==-2){--fire_depth;return false;}if(result==1){full=true;break;}}
    --fire_depth;if((e.flags&128)&&!sound(e.fire_sound,e.position.x,true)){last_error=5;return false;}return true;
}
void BulletManager::release(BulletState& b)noexcept{b.state=0;b.lifetime.set(0,&rate);b.auxiliary_timer.set(0,&rate);}
bool BulletManager::cancel(BulletState& b){
    if(b.state!=1&&b.state!=2)return true;b.state=3;b.vm.pending_interrupt=1;
    if(outside(b.position,8,8,0)){b.flags|=8;return true;}
    if(b.cancel_effect>=0&&!world.effect(b.position,b.cancel_effect))return false;b.lifetime.set(0,&rate);return true;
}
bool BulletManager::cancel_all(bool skip_delayed,const BulletCancelContext& context){
    if(context.protected_spell())return true;
    for(u32 i=0;i<capacity;++i){auto& b=storage[i];
        if(b.state&&b.state!=3&&(!skip_delayed||!b.collision_delay)&&!cancel(b))return false;
    }return true;
}
bool BulletManager::cancel_rectangle(bool reward,const BulletCancelContext& context){
    // 40b3a0 marks removal without changing the bullet's state or timers.
    if(context.protected_spell())return true;
    constexpr u32 small[]={0xff808080,0xffff1010,0xffff1010,0xff801080,0xff801080,0xff1010ff,0xff1010ff,0xff108080,0xff108080,0xff10ff10,0xff10ff10,0xff10ff10,0xff808010,0xff808010,0xff808010,0xff808080};
    constexpr u32 medium[]={0xff808080,0xffff1010,0xff801080,0xff1010ff,0xff108080,0xff10ff10,0xff808010,0xff808080};
    constexpr u32 large[]={0xffff1010,0xff1010ff,0xff10ff10,0xff808010};
    const float hx=float(double(cancel_size.x)*.5),hy=float(double(cancel_size.y)*.5);
    const float left=float(double(cancel_center.x)-hx),right=float(double(cancel_center.x)+hx),top=float(double(cancel_center.y)-hy),bottom=float(double(cancel_center.y)+hy);
    for(u32 j=0;j<capacity;++j){auto& b=storage[j];if(!b.state||b.state==3)continue;
        const float l=float(double(b.position.x)-double(b.hitbox.x)*.5),r=float(double(b.position.x)+double(b.hitbox.x)*.5);
        const float t=float(double(b.position.y)-double(b.hitbox.y)*.5),v=float(double(b.position.y)+double(b.hitbox.y)*.5);
        if(!(left<=r&&l<=right&&top<=v&&t<=bottom))continue;
        b.flags|=8;if(reward&&!world.cancel_reward(b.position))return false;
        if(b.vm.sprite){const float h=b.vm.sprite->width;const u32 count=h>32?4:h>16?8:16;if(u32(b.color)>=count)return false;
            const u32 color=(h>32?large:h>16?medium:small)[b.color];if(!world.colored_effect(b.position,93,color))return false;
        }else if(!world.effect(b.position,93))return false;
        b.reserved_4bc=0;
    }return true;
}
bool BulletManager::cancel_circle(const Vec3& center,float radius,bool reward,bool skip_delayed,const BulletCancelContext& context){
    if(context.protected_spell())radius=float(double(radius)/3);
    for(u32 i=0;i<capacity;++i){auto& b=storage[i];
        if(!b.state||b.state==3||(skip_delayed&&b.collision_delay))continue;
        const float dx=float(double(b.position.x)-center.x),dy=float(double(b.position.y)-center.y);
        const float r=float(double(b.hitbox.x)*.5+radius),distance=float(double(dx)*dx+double(dy)*dy);
        if(!(double(distance)>double(r)*r)){
            if(!cancel(b))return false;
            if(reward&&!outside(b.position,2,2,0)&&!world.cancel_reward(b.position))return false;
        }
    }return world.cancel_enemies(center,radius,reward);
}
bool BulletManager::convert_circle(const Vec3& center,float radius,bool reward,const BulletCancelContext& context){
    // 40b210: Reimu-B converts both normal and spawning bullets, but skips
    // collision-delayed bullets. Conversion precedes the optional point item.
    if(context.protected_spell())radius=float(double(radius)/3);
    for(u32 i=0;i<capacity;++i){auto& b=storage[i];
        if(!b.state||b.state==3||b.collision_delay)continue;
        const float dx=float(double(b.position.x)-center.x),dy=float(double(b.position.y)-center.y);
        const float r=float(double(b.hitbox.x)*.5+radius),distance=float(double(dx)*dx+double(dy)*dy);
        if(!(double(distance)>double(r)*r)){
            if(!cancel(b))return false;
            if(!outside(b.position,2,2,0)){
                const float angle=normalize_angle(float(double(b.angle)+3.1415927410125732421875));
                if(!world.cancel_shot(b.position,angle))return false;
                if(reward&&!world.cancel_reward(b.position))return false;
            }
        }
    }return world.convert_enemies(center,radius,reward);
}
bool BulletManager::cancel_beam(const Vec3& center,float half_width,bool reward,const BulletCancelContext& context){
    // 40b0e0: a vertical half-plane strip, not the rectangular 40b3a0 path.
    if(context.protected_spell())half_width=float(double(half_width)/3);
    for(u32 i=0;i<capacity;++i){auto& b=storage[i];
        if(!b.state||b.state==3||b.collision_delay)continue;
        if(!(double(center.x)-half_width<=b.position.x&&b.position.x<=double(center.x)+half_width&&b.position.y<=center.y))continue;
        if(!cancel(b))return false;
        if(reward&&!outside(b.position,2,2,0)&&!world.cancel_reward(b.position))return false;
    }return world.cancel_enemy_beam(center,half_width,reward);
}
i32 BulletManager::update_one(BulletState& b)noexcept{
#ifdef TH11_MULTIPLAYER
    if(world.multiplayer_world())player=world.multiplayer_target(b.position);
#endif
    if(b.flags&8){release(b);return -1;}
    if(b.state==2){move(b,rate,true);if(b.vm.integers[0])b.state=1;}
    else if(b.state==3)move(b,rate,true);
    if(b.state==1){
        if(bullet_start_transforms(b,*this)<0)return -2;
        constexpr u32 order[]={1,4,8,0x10,0x40,0x20,0x100,0x800000,0x2000000,0x8000000,0x1000};
        for(const auto op:order)if((b.active_transforms&op)&&bullet_motion(b,op,*this)<0)return -2;
        move(b,rate,false);
        if(b.flags&2){i32 collision=0;
#ifdef TH11_MULTIPLAYER
            if(world.multiplayer_world()){if(!world.multiplayer_bullet_collision(b,multiplayer_grazed[&b-storage.get()],collision))return -2;}
            else
#endif
            collision=world.collision(b);if(collision<0)return -2;
            if(collision==1){b.state=3;b.vm.pending_interrupt=1;if(b.cancel_effect>=0&&!world.effect(b.position,b.cancel_effect))return -2;}
            else if(collision==2&&!(b.flags&4)
#ifdef TH11_MULTIPLAYER
                    &&!world.multiplayer_world()
#endif
            ){if(!world.register_graze())return -2;b.flags|=4;
                if(!world.effect(b.position,156)||!world.reward_graze()||!sound(28,b.position.x,true))return -2;}
        }
    }
    if(b.vm.sprite){for(const auto op:{0x20000u,0x40000u})if((b.active_transforms&op)&&bullet_motion(b,op,*this)<0)return -2;
        if(b.offscreen_grace<1&&outside(b.position,double(b.vm.sprite->width)*.5,double(b.vm.sprite->height)*.5,-64)){release(b);return -1;}}
    if(b.collision_delay)b.collision_delay=wrapping_sub(b.collision_delay,1);if(b.offscreen_grace>0)--b.offscreen_grace;
    const i32 result=b.vm.update(animations);if(result<0)return -2;if(result){release(b);return -1;}return 0;
}
bool BulletManager::update(bool paused)noexcept{
    if(last_error)return false;rate=animations.rate;active_count=0;heads.fill(nullptr);tails.fill(nullptr);
    for(u32 i=0;i<capacity;++i){auto& b=storage[i];if(!b.state)continue;
        if(!paused){const i32 result=update_one(b);if(result==-2){last_error=6;return false;}if(result)continue;}
        if(u32(b.draw_group)>=heads.size()){last_error=7;return false;}
        const u32 group=b.draw_group;if(tails[group])tails[group]->draw_next=&b;else heads[group]=&b;tails[group]=&b;b.draw_next=nullptr;++active_count;b.lifetime.tick();
    }return true;
}
bool BulletManager::draw_group(u32 group,AnmRenderer& renderer){
    if(group>=heads.size())return false;
    for(auto* b=heads[group];b;b=b->draw_next){
        b->vm.script_position={float((double(b->position.x)+32)+192),float(double(b->position.y)+16),b->position.z};
        if(b->vm.flags&0x8000000){b->vm.rotation.z=normalize_angle(float(double(b->angle)+1.57079637050628662109375));b->vm.flags|=4;}
        if(renderer.draw(b->vm)==-2)return false;
    }return true;
}
}
