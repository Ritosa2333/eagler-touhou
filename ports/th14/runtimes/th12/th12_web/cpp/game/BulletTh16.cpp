#include "BulletTh16.hpp"
#include "EclContext.hpp"
#include "BulletManager.hpp"
#include "Movement.hpp"
namespace th11 {
#include "Th16BulletTables.inc"
#if PORT_TITLE == 16
namespace {
i32 th16_sprite(AnmVm& vm,i32 frame){
    const auto& b=*reinterpret_cast<BulletState*>(reinterpret_cast<u8*>(&vm)-offsetof(BulletState,vm));
    if(u32(b.sprite)>=44)return -1;const auto& a=th16_bullet_appearances[b.sprite];if(a.sprites[0]<0)return frame;
    const i32 index=b.color*4+frame;return u32(index)<64?a.sprites[index]:-1;
}
}
i32 BulletManager::spawn_th16(const Th16BulletEmitter& e,i32 index,i32 layer,float aim)noexcept{
    if(last_error)return -2;if(!th16_free)return 1;
    auto& b=*th16_free;th16_free=b.th16_free_next;b.th16_free_next=nullptr;b.th16_active_previous=nullptr;b.th16_active_next=th16_active;
    if(th16_active)th16_active->th16_active_previous=&b;th16_active=&b;
    BulletLaunch launch{};const bool accepted=bullet_th16_launch(e,index,layer,aim,animations.visual_rng,player,exclusion_squared,launch);
    b.position=launch.position;b.speed=launch.speed;b.angle=launch.angle;b.flags|=1;b.state=1;b.lifetime.set(0,&rate);b.auxiliary_timer.set(0,&rate);
    if(!accepted){release(b);return -1;}
    if(!bullet_th16_bind_metadata(b,e,launch,&rate)){last_error=1601;return -2;}
    const auto& a=th16_bullet_appearances[e.sprite];if(!b.vm.bind_script(resource,a.script,file_id,&animations.rate)){last_error=1602;return -2;}
    b.vm.sprite_callback=th16_sprite;b.vm.reserved_430=u32(uintptr_t(&b));b.vm.flags2=(b.vm.flags2&~0x80000u)|0x40000u;
    const auto& first=e.transforms[e.transform_start];if(first.opcode==2){if(i16(first.p)!=1)b.vm.interrupt(i16(first.p)+7);}else b.vm.interrupt(2);
    struct StartServices:Th16BulletStepServices{
        BulletManager& manager;explicit StartServices(BulletManager& m):manager(m){}
        i32 collision(BulletState&)override{return -2;}i32 animation(AnmVm&)override{return -2;}
        bool sound(i32 id)override{return manager.sound(id,0,false);}
        bool positional_sound(i32 id,float x)override{return manager.sound(id,x,true);}
        bool fire(const Th16BulletEmitter& e)override{return manager.fire_th16(e);}
        bool cancel(BulletState& b)override{return manager.cancel_th16(b);}
        bool change_appearance(BulletState& b,i32 type,u32 color)override{return manager.change_appearance(b,type,color&0x7fff,(color&0x8000)!=0);}
    } services(*this);
    if(bullet_th16_start_transforms(b,&rate,nullptr,nullptr,&player,&services)==-2){last_error=1603;return -2;}
    if(b.vm.update(animations)==-2){last_error=1604;return -2;}return 0;
}
bool BulletManager::fire_th16(const Th16BulletEmitter& e){
    if(last_error||fire_depth>=64){last_error=1605;return false;}++fire_depth;
    struct Services:Th16FireServices{
        BulletManager& manager;explicit Services(BulletManager& m):manager(m){}
        i32 spawn(const Th16BulletEmitter& e,i32 index,i32 layer,float aim)override{return manager.spawn_th16(e,index,layer,aim);}
        bool sound(i32 id,float x)override{return manager.sound(id,x,true);}
    } services(*this);
    const bool result=bullet_th16_fire(e,player,services);--fire_depth;if(!result&&!last_error)last_error=1606;return result;
}
i32 BulletManager::collision_th16(BulletState& b){
    struct Services:Th16BulletCollisionServices {
        BulletWorld& world;explicit Services(BulletWorld& w):world(w){}
        i32 probe(const BulletState& b)override{return world.collision(b);}
        bool effect(const Vec3& p,i32 script,const Vec3& v)override{return world.th16_hit_effect(p,script,v);}
        bool graze(const Vec3& p)override{return world.th16_graze(p);}
    } services(world);
    const i32 result=bullet_th16_collision(b,rate,services);
    if(result==-2)last_error=1607;return result;
}
bool BulletManager::cancel_th16(BulletState& b,u32 rewards){
    struct Services:Th16CancelServices{
        BulletManager& manager;BulletWorld& world;explicit Services(BulletManager& m,BulletWorld& w):manager(m),world(w){}
        i32 animation(AnmVm& vm)override{return vm.update(manager.animations);}
        bool effect(const Vec3& p,i32 id)override{return world.effect(p,id);}
        bool sound(i32 id,float x)override{return manager.sound(id,x,true);}
        bool reward(Vec3 p,u32 value)override{return bullet_th16_cancel_reward(p,value,manager.th16_reward_count);}
    } services(*this,world);
    const bool ok=bullet_th16_cancel(b,&rate,services,rewards);if(!ok&&!last_error)last_error=1612;return ok;
}
bool BulletManager::cancel_circle_th16(const Vec3& center,float radius,bool delayed,u32 rewards){
    for(auto* b=th16_active;b;b=b->th16_active_next)if(bullet_th16_circle_match(*b,center,radius,delayed)&&!cancel_th16(*b,rewards))return false;
    return true;
}
bool BulletManager::cancel_all_th16(){
    // Retail416f40 scans all2000 slots, including state4 re-cancellation.
    for(u32 n=0;n<capacity;++n){auto& b=storage[n];if(b.state&&b.state!=3&&!cancel_th16(b))return false;}return true;
}
i32 BulletManager::update_th16(BulletState& b)noexcept{
    struct Services:Th16BulletStepServices{
        BulletManager& manager;explicit Services(BulletManager& m):manager(m){}
        i32 collision(BulletState& b)override{return manager.collision_th16(b);}
        i32 animation(AnmVm& vm)override{return vm.update(manager.animations);}
        bool sound(i32 id)override{return manager.sound(id,0,false);}
        bool positional_sound(i32 id,float x)override{return manager.sound(id,x,true);}
        Vec3 player_position()const override{return manager.player;}
        Vec2 bounce_bounds()const override{return manager.th16_bounce_bounds;}
        bool fire(const Th16BulletEmitter& e)override{return manager.fire_th16(e);}
        bool cancel(BulletState& b)override{return manager.cancel_th16(b);}
        bool change_appearance(BulletState& b,i32 type,u32 color)override{return manager.change_appearance(b,type,color&0x7fff,(color&0x8000)!=0);}
    } services(*this);
    const i32 result=bullet_th16_step(b,rate,services);if(result==-1)release(b);if(result==-2&&!last_error)last_error=1608;return result;
}
#endif
bool bullet_th16_cancel_reward(Vec3 p,u32 rewards,u32& count)noexcept{
    if(!rewards)return true;
    if(-192.f>=p.x+32.f||p.x-32.f>=192.f||0.f>=p.y+32.f||p.y-32.f>=448.f)return true;
    ++count;return rewards==1||rewards==3;
}
bool bullet_th16_cancel(BulletState& b,const float* rate,Th16CancelServices& services,u32 rewards){
#if PORT_TITLE == 16
    // Retail416840 primary VM path. Secondary VMs are
    // absent in the measured44-style catalog; factories remain service boundaries.
    b.vm.interrupt(1);if(services.animation(b.vm)==-2)return false;
    if(!(b.flags&0x200)){
        if(b.cancel_effect>=0&&!services.effect(b.position,b.cancel_effect))return false;
        if(!services.sound(71,b.position.x))return false;
        if(rewards&&!services.reward(b.position,rewards))return false;
    }
    const float r=rate?*rate:1.f;
    b.position.x=float(b.position.x+float(float(b.velocity.x*r)*.5f));
    b.position.y=float(b.position.y+float(float(b.velocity.y*r)*.5f));
    b.position.z=float(b.position.z+float(float(b.velocity.z*r)*.5f));
    b.state=4;b.lifetime.set(0,rate);return true;
#else
    (void)b;(void)rate;(void)services;(void)rewards;return false;
#endif
}
bool bullet_th16_circle_match(const BulletState& b,const Vec3& center,float radius,bool delayed)noexcept{
    if((b.state!=1&&b.state!=2)||(delayed&&b.collision_delay))return false;
    const float dx=float(b.position.x-center.x),dy=float(b.position.y-center.y),r=float(float(b.hitbox.x*.5f)+radius);
    return float(r*r)>=float(float(dy*dy)+float(dx*dx));
}
i32 bullet_th16_collision(BulletState& b,float rate,Th16BulletCollisionServices& services){
    // Complete4124b0 primary-VM consumer. The measured44-style table has
    // no secondary VM. Player hit, effect allocation and graze are services.
    b.vm.th13_native_flags&=~0x60000u;b.vm.script_position={0,0,0};
    if(!(b.flags&2)||!(b.hitbox.x>0))return 0;
    const i32 result=services.probe(b);if(result<0)return -2;
    if(result==1&&!b.collision_delay){
        b.state=3;b.vm.interrupt(1);
        if(b.cancel_effect>=0){
            const Vec3 velocity{float(float(b.velocity.x*rate)*10.f),float(float(b.velocity.y*rate)*10.f),float(float(b.velocity.z*rate)*10.f)};
            if(!services.effect(b.position,b.cancel_effect,velocity))return -2;
        }
    }else if(result==2&&!(b.flags&4)){
        if(!services.graze(b.position))return -2;b.flags|=4;
    }
    return result;
}
void bullet_th16_hit_effect_setup(AnmVm& vm,const Vec3& velocity,const float* rate)noexcept{
    // Full4173f0, all four vectors and own timer reset are observable.
    auto& p=vm.position_interpolation;p.start[0]=p.start[1]=p.start[2]=0;
    p.end[0]=velocity.x;p.end[1]=velocity.y;p.end[2]=velocity.z;
    std::memset(p.initial_tangent,0,sizeof(p.initial_tangent));std::memset(p.final_tangent,0,sizeof(p.final_tangent));
    p.duration=30;p.mode=InterpolationMode::Decelerate4;p.timer.set(0,rate);
}
bool bullet_th16_fire(const Th16BulletEmitter& e,const Vec3& player,Th16FireServices& services){
    // Complete414da0 loops, excluding actual allocation and sound queues.
    bool full=false;const float aim=bullet_aim(e.position,player);
    for(i32 layer=0;layer<e.layers&&!full;++layer)for(i32 index=0;index<e.count;++index){
        const i32 result=services.spawn(e,index,layer,aim);if(result==-2)return false;if(result==1){full=true;break;}
    }
    return !(e.flags&0x20)||services.sound(e.fire_sound,e.position.x);
}
void bullet_th16_clock(Timer& t,float rate)noexcept{
    t.previous=t.current;
    if(rate>.99f&&rate<1.01f){t.current=wrapping_add(t.current,1);t.fractional=float(t.fractional+1.f);}
    else{t.fractional=float(t.fractional+rate);t.current=truncate_int(t.fractional);}
}
namespace {
// Own415bb0 and four ordered wall consumers415a10/415ae0/4158d0/415790.
i32 th16_bounce(BulletState& b,Th16BulletStepServices& services)noexcept{
    auto& slot=b.motion[8];const auto bounds=services.bounce_bounds();
    const bool manager_width=!(bounds.x<=0.f);
    const float outer_w=manager_width?bounds.x:slot.vector.x,outer_h=manager_width?bounds.y:slot.vector.y;
    if(!(outer_w*-.5f>=b.position.x+0.f||b.position.x>=outer_w*.5f||224.f-outer_h*.5f>=b.position.y+0.f||b.position.y>=224.f+outer_h*.5f))return 0;
    const float w=manager_width?bounds.x:slot.vector.x,h=!(bounds.y<=0.f)?bounds.y:slot.vector.y;
    bool reflected=false;const bool count_only=(slot.duration&0x10)!=0;
    const auto vertical=[&](bool top){
        if(top?b.position.y<224.f-h*.5f:b.position.y>=224.f+h*.5f){
            reflected=true;if(!count_only){b.angle=normalize_angle(-b.angle);b.position.y=top?(448.f-h)-b.position.y:(448.f+h)-b.position.y;}
        }
    };
    const auto horizontal=[&](bool left){
        if(left?b.position.x<w*-.5f:b.position.x>=w*.5f){
            reflected=true;if(!count_only){b.angle=normalize_angle(-b.angle-3.1415927410125732f);b.angle=normalize_angle(normalize_angle(b.angle+0.f));b.position.x=left?-w-b.position.x:w-b.position.x;}
        }
    };
    if(slot.duration&1)vertical(true);if(slot.duration&2)vertical(false);if(slot.duration&8)horizontal(false);if(slot.duration&4)horizontal(true);
    if(slot.a>-990.f)b.speed=slot.a; // Own494778 retains this older sentinel.
    const auto velocity=polar(b.angle,b.speed);b.velocity.x=velocity.x;b.velocity.y=velocity.y;
    if(reflected){slot.count=wrapping_add(slot.count,1);if(b.transform_sound>=0&&!services.sound(b.transform_sound))return -2;}
    if(slot.count>=slot.limit){b.active_transforms&=~0x40u;return 1;}return 0;
}
// Own4162d0; descriptor413d1f permits later non-concurrent transforms.
i32 th16_offscreen_hold(BulletState& b)noexcept{
    auto& slot=b.motion[6];slot.timer.advance(-1);bool complete=false;
    if(slot.duration){
        if(!b.vm.sprite)return -2;const float width=b.vm.sprite->width,height=b.vm.sprite->height;
        if(!(b.position.x+width*.5f> -192.f&&b.position.x-width*.5f<192.f&&b.position.y+height*.5f>0.f&&b.position.y-height*.5f<448.f)){
            const auto direction=polar(b.angle,1.f);const float left=(-384.f-width)*.5f,right=(width+384.f)*.5f,top=224.f-(height+448.f)*.5f,bottom=(height+448.f)*.5f+224.f;
            float negative=-999.f,positive=-999.f;
            for(const auto p:std::array<Vec2,4>{Vec2{left,top},Vec2{right,top},Vec2{left,bottom},Vec2{right,bottom}}){
                const float x=p.x-b.position.x,y=p.y-b.position.y,length=std::sqrt(x*x+y*y),inverse=length>0.f?1.f/length:0.f,nx=x*inverse,ny=y*inverse;
                const float cross=direction.x*ny-direction.y*nx,dot=direction.x*nx+direction.y*ny;
                if(cross<=0.f&&dot>negative&&dot>=0.f)negative=dot;if(cross>=0.f&&dot>positive&&dot>=0.f)positive=dot;
            }
            complete=negative< -998.f||positive< -998.f;
        }
    }
    if(complete||slot.timer.current<=0){b.active_transforms^=0x100u;return 1;}return 0;
}
}
i32 bullet_th16_step(BulletState& b,float rate,Th16BulletStepServices& services)noexcept{
#if PORT_TITLE == 16
    // Full411e70's zero-ETEX state1/2 paths. Collision and VM update have
    // explicit service boundaries; other ETEX/lifecycle bodies remain gaps.
    bullet_th16_clock(b.auxiliary_timer,b.auxiliary_timer.rate?*b.auxiliary_timer.rate:1.f);
    if(b.flags&8)return -1;
    if(b.active_transforms&~0x8020015du)return -2;
    if(b.state<1||b.state>4)return -2;
    const auto move=[&](bool half){
        const auto delta=[&](float v){const float step=float(v*rate);return half?float(step*.5f):step;};
        b.position.x=float(b.position.x+delta(b.velocity.x));b.position.y=float(b.position.y+delta(b.velocity.y));b.position.z=float(b.position.z+delta(b.velocity.z));
    };
    bool animate_only=false;
    if(b.state==3)move(true);
    if(b.state==2){
        move(true);
        if(b.lifetime.current>=8){const i32 hit=services.collision(b);if(hit<0)return -2;if(hit==1)animate_only=true;}
        if(!b.vm.integers[0])animate_only=true;
        if(!animate_only)b.state=1;
    }
    if(!animate_only&&b.state==1){
        // Retail412014 retries the remaining program when a motion ends.
        for(u32 budget=0;;++budget){
            const Vec3 player=services.player_position();if(budget>=64||bullet_th16_start_transforms(b,&rate,nullptr,nullptr,&player,&services)==-2)return -2;
            if(!b.active_transforms)break;
            if(b.active_transforms&~0x8020015du)return -2;
            i32 ended=0;if(b.active_transforms&1)ended+=bullet_th16_motion_one(b,rate);if(b.active_transforms&4)ended+=bullet_th16_motion_four(b,rate);
            if(b.active_transforms&0x200000u)ended+=bullet_th16_motion_target_speed(b,rate);
            if(b.active_transforms&8)ended+=bullet_th16_motion_eight(b,rate);
            if(b.active_transforms&16){const i32 result=bullet_th16_motion_sixteen(b,rate,services);if(result<0)return -2;ended+=result;}
            if(b.active_transforms&0x40){const i32 result=th16_bounce(b,services);if(result<0)return -2;ended+=result;}
            if(b.active_transforms&0x100){const i32 result=th16_offscreen_hold(b);if(result<0)return -2;ended+=result;}
            if(b.active_transforms&0x80000000u){auto& timer=b.motion[3].timer;if(timer.current<=0){b.active_transforms^=0x80000000u;++ended;}else{timer.previous=timer.current;const float speed=timer.rate?*timer.rate:1.f;timer.fractional=timer.fractional-(speed>.99f&&speed<1.01f?1.f:speed);timer.current=truncate_int(timer.fractional);}}
            if(b.collision_delay)b.collision_delay=wrapping_sub(b.collision_delay,1);
            if(!ended)break;
        }
        if(!(b.flags&0x200)){move(false);if(services.collision(b)<0)return -2;}
    }
    if(b.vm.sprite&&!(b.active_transforms&0x100)&&b.offscreen_grace<1){
        const float half_x=float(float(b.vm.sprite->width*b.th16_size)*.5f),half_y=float(float(b.vm.sprite->height*b.th16_size)*.5f);
        if(float(b.position.x+half_x)<=-192.f||float(b.position.x-half_x)>=192.f||float(b.position.y+half_y)<=-64.f||float(b.position.y-half_y)>=448.f)return -1;
    }
    if(b.collision_delay)b.collision_delay=wrapping_sub(b.collision_delay,1);
    if(b.offscreen_grace>0)--b.offscreen_grace;
    if(!(b.flags&0x200)){const i32 result=services.animation(b.vm);if(result==-2)return -2;if(result)return -1;}
    return 0;
#else
    (void)b;(void)rate;(void)services;return -2;
#endif
}
i32 bullet_th16_start_transforms(BulletState& b,const float* rate,i32* sound,bool* positional,const Vec3* player,Th16BulletStepServices* services)noexcept{
#if PORT_TITLE == 16
    if(sound)*sound=-1;
    if(positional)*positional=false;
    const auto audio=[&](i32 id,bool position){if(services)return position?services->positional_sound(id,b.position.x):services->sound(id);if(sound)*sound=id;if(positional)*positional=position;return true;};
    // Own retail413860 dispatch and4139cb initializer. Other ETEX stays explicit.
    if(b.transform_index<0)return -2;
    u32 budget=0;
    while(b.transform_index<18){
        if(++budget>4096||b.transform_index<0)return -2;
        const auto& t=b.th16_transforms[b.transform_index];const u32 op=u32(t.opcode);
        if(!op||(!t.concurrent&&(b.active_transforms&~0x100u))||(op&b.active_transforms))return 0;
        if(op==1){b.active_transforms|=1;b.motion[0].timer.set(0,rate);b.motion[0].vector.z=0;}
        else if(op==4){
            b.active_transforms|=4;auto& motion=b.motion[2];motion.a=t.a;
            if(t.b<=-999990.f)motion.b=b.angle;
            else{float angle=t.b;if(angle>=999990.f){if(!player)return -2;const float dx=player->x-b.position.x,dy=player->y-b.position.y;angle=float(float(dx==0&&dy==0?1.5707963705062866:std::atan2(double(dy),double(dx)))+t.c);}motion.b=normalize_angle(angle);}
            motion.timer.set(0,rate);motion.duration=t.p;
            motion.vector.x=float(std::cos(double(motion.b))*double(motion.a));motion.vector.y=float(std::sin(double(motion.b))*double(motion.a));
            if(b.transform_index&&b.transform_sound>=0&&!audio(b.transform_sound,false))return -2;
        }
        else if(op==8){b.active_transforms|=8;auto& motion=b.motion[1];motion.a=t.a;motion.b=t.b;motion.duration=t.p;motion.timer.set(0,rate);if(b.transform_index&&b.transform_sound>=0&&!audio(b.transform_sound,false))return -2;}
        else if(op==0x100){b.active_transforms|=op;auto& slot=b.motion[6];slot.timer.set(t.p,rate);slot.duration=t.q;}
        else if(op==0x40){auto& slot=b.motion[8];b.active_transforms|=op;slot.a=t.a;slot.vector.x=(t.q&0x20)?t.b:384.f;slot.vector.y=(t.q&0x20)?t.c:448.f;slot.limit=t.p;slot.count=0;slot.duration=t.q;}
        else if(op==0x200){if(!services||!services->change_appearance(b,t.p,u32(t.q)))return -2;}
        else if(op==0x200000u){
            // Own4145b9 / 4151e0: target speed becomes a vector acceleration.
            auto& motion=b.motion[5];motion.a=(t.a-b.speed)/float(t.p);
            if(t.b<=-999990.f)motion.b=b.angle;
            else{float angle=t.b;if(angle>=999990.f){if(!player)return -2;const float dx=player->x-b.position.x,dy=player->y-b.position.y;angle=float(float(dx==0&&dy==0?1.5707963705062866:std::atan2(double(dy),double(dx)))+t.c);}motion.b=normalize_angle(angle);}
            motion.timer.set(0,rate);motion.duration=t.p;
            motion.vector.x=float(std::cos(double(motion.b))*double(motion.a));motion.vector.y=float(std::sin(double(motion.b))*double(motion.a));b.active_transforms|=op;
            if(b.transform_index&&b.transform_sound>=0&&!audio(b.transform_sound,false))return -2;
        }
        else if(op==16){
            // Own413aeb: modes2/3 use the geometry saved by ETEX800000.
            // Modes5..7 additionally consume the visual RNG and remain gaps.
            if(t.r<0||t.r>4)return -2;
            auto& motion=b.motion[4];motion.a=t.b<=-999990.f?b.speed:t.b;
            if(t.a<=-999990.f)motion.b=b.angle;
            else{float angle=t.a;if(angle>=9999990.f)angle=b.motion[7].b;else if(angle>=999990.f){if(!player)return -2;const float dx=player->x-b.position.x,dy=player->y-b.position.y;angle=float(float(dx==0&&dy==0?1.5707963705062866:std::atan2(double(dy),double(dx)))+t.c);}motion.b=normalize_angle(angle);}
            if(t.r==2){if(!player)return -2;motion.b=normalize_angle(bullet_aim(b.motion[7].vector,*player)+motion.b);}
            else if(t.r==3)motion.b=normalize_angle(b.motion[7].b+motion.b);
            motion.timer.set(0,rate);motion.duration=t.p;motion.limit=t.q;motion.count=0;
            // These components hold opaque mode words, not a geometric vector.
            motion.vector.x=float_bits(u32(t.r));motion.vector.y=float_bits(u32(t.s));b.active_transforms|=16;
        }
        else if(op==8192){
            // Own41403b constructs a whole emitter and skips its metadata row.
            if(b.transform_index>=17||!services)return -2;
            const auto& metadata=b.th16_transforms[b.transform_index+1];Th16BulletEmitter emitter{};
            emitter.position=b.position;emitter.count=i16(t.r);emitter.layers=i16(t.s);emitter.aim=i16(t.p);emitter.transform_start=t.q;emitter.transform_sound=-1;
            if(t.a<=-999990.f)emitter.angle=b.angle;
            else{float angle=t.a;if(angle>=999990.f){if(!player)return -2;const float dx=player->x-b.position.x,dy=player->y-b.position.y;angle=float(dx==0&&dy==0?1.5707963705062866:std::atan2(double(dy),double(dx)));}emitter.angle=normalize_angle(angle);}
            emitter.spread=t.b;emitter.speed=t.c<=-999990.f?b.speed:t.c;emitter.slow_speed=t.d;emitter.sprite=metadata.p;emitter.color=metadata.q;
            std::memcpy(emitter.transforms,b.th16_transforms,sizeof emitter.transforms);
            ++b.transform_index;if(!services->fire(emitter))return -2;++b.transform_index;
            if(metadata.r&&!services->cancel(b))return -2;
            continue;
        }
        else if(op==32768){b.extra=t.p;}
        else if(op==0x800000u){b.motion[7].a=b.speed;b.motion[7].b=b.angle;b.motion[7].vector=b.position;}
        else if(op==65536){
            // Own4141da uses the descriptor's shared loop counter at+c64.
            if(t.q<=0){b.transform_index=t.p;continue;}
            if(!b.reserved_4c0){b.reserved_4c0=t.q;b.transform_index=t.p;continue;}
            if(b.reserved_4c0!=1){--b.reserved_4c0;b.transform_index=t.p;continue;}
            b.reserved_4c0=0;
        }
        else if(op==2048){if(!audio(t.p,true))return -2;}
        else if(op==0x80000000u){if(t.p>0){b.active_transforms|=op;b.motion[3].timer.set(t.p,rate);}}
        else return -2;
        ++b.transform_index;
    }
    return 0;
#else
    (void)b;(void)rate;return -2;
#endif
}
i32 bullet_th16_motion_four(BulletState& b,float rate)noexcept{
    auto& motion=b.motion[2];if(motion.timer.current>=motion.duration){b.active_transforms&=~4u;return 1;}
    b.speed=(motion.a*rate)+b.speed;
    b.velocity.x=(motion.vector.x*rate)+b.velocity.x;b.velocity.y=(motion.vector.y*rate)+b.velocity.y;b.velocity.z=(motion.vector.z*rate)+b.velocity.z;
    if(std::abs(b.velocity.x)>.0001f||std::abs(b.velocity.y)>.0001f){b.angle=normalize_angle(float(std::atan2(double(b.velocity.y),double(b.velocity.x))));const float magnitude=(b.velocity.x*b.velocity.x)+(b.velocity.y*b.velocity.y);b.speed=float(std::sqrt(double(magnitude)));}
    bullet_th16_clock(motion.timer,motion.timer.rate?*motion.timer.rate:rate);return 0;
}
i32 bullet_th16_motion_target_speed(BulletState& b,float rate)noexcept{
    auto& motion=b.motion[5];if(motion.timer.current>=motion.duration){b.active_transforms&=~0x200000u;return 1;}
    b.speed=(motion.a*rate)+b.speed;
    b.velocity.x=(motion.vector.x*rate)+b.velocity.x;b.velocity.y=(motion.vector.y*rate)+b.velocity.y;b.velocity.z=(motion.vector.z*rate)+b.velocity.z;
    if(std::abs(b.velocity.x)>.0001f||std::abs(b.velocity.y)>.0001f)b.angle=normalize_angle(float(std::atan2(double(b.velocity.y),double(b.velocity.x))));
    bullet_th16_clock(motion.timer,motion.timer.rate?*motion.timer.rate:rate);return 0;
}
i32 bullet_th16_motion_sixteen(BulletState& b,float rate,Th16BulletStepServices& services)noexcept{
    // Complete415570 for the measured modes0/1/4.
    auto& motion=b.motion[4];float speed=b.speed;
    if(motion.timer.current>=motion.duration){
        if(b.transform_sound>=0&&!services.sound(b.transform_sound))return -2;
        motion.count=wrapping_add(motion.count,1);const u32 mode=float_bits(motion.vector.x);
        if(mode==0)b.angle=normalize_angle(b.angle+motion.b);
        else if(mode==1){const auto player=services.player_position();const float dx=player.x-b.position.x,dy=player.y-b.position.y;b.angle=normalize_angle(float(float(dx==0&&dy==0?1.5707963705062866:std::atan2(double(dy),double(dx)))+motion.b));}
        else if(mode>=2&&mode<=4)b.angle=normalize_angle(motion.b);else return -2;
        b.speed=speed=motion.a;motion.timer.set(0,motion.timer.rate);
        if(motion.count>=motion.limit){b.velocity.x=float(std::cos(double(b.angle))*double(speed));b.velocity.y=float(std::sin(double(b.angle))*double(speed));b.active_transforms&=~16u;return 1;}
    }else speed=b.speed-((motion.timer.fractional*b.speed)/float(motion.duration));
    b.velocity.x=float(std::cos(double(b.angle))*double(speed));b.velocity.y=float(std::sin(double(b.angle))*double(speed));
    bullet_th16_clock(motion.timer,motion.timer.rate?*motion.timer.rate:rate);return 0;
}
i32 bullet_th16_motion_eight(BulletState& b,float rate)noexcept{
#if PORT_TITLE == 16
    auto& motion=b.motion[1];if(motion.timer.current>=motion.duration){b.active_transforms&=~8u;return 1;}
    b.angle=normalize_angle(normalize_angle((motion.b*rate)+b.angle));b.speed=(motion.a*rate)+b.speed;
    b.velocity.x=float(std::cos(double(b.angle))*double(b.speed));b.velocity.y=float(std::sin(double(b.angle))*double(b.speed));
    bullet_th16_clock(motion.timer,motion.timer.rate?*motion.timer.rate:rate);return 0;
#else
    (void)b;(void)rate;return -2;
#endif
}
i32 bullet_th16_motion_one(BulletState& b,float rate)noexcept{
#if PORT_TITLE == 16
    // Complete414ec0: SSE ramp followed by x87 polar vector and own timer.
    auto& t=b.motion[0].timer;if(t.current>16){b.active_transforms^=1;return 1;}
    const float ramp=float(5.f-float(float(t.fractional*5.f)*.0625f));
    const float speed=float(ramp+b.speed);
    b.velocity.x=float(std::cos(double(b.angle))*double(speed));b.velocity.y=float(std::sin(double(b.angle))*double(speed));
    bullet_th16_clock(t,t.rate?*t.rate:rate);return 0;
#else
    (void)b;(void)rate;return -2;
#endif
}
bool bullet_th16_change_appearance(BulletState& b,i32 type,u32 packed,Th16BulletAppearanceServices& services)noexcept{
#if PORT_TITLE == 16
    // Own413dfa..413fe4; initial script update precedes origin selection.
    if(u32(type)>=44||u32(packed&0x7fff)>=16)return false;
    const auto& a=th16_bullet_appearances[type];const i32 color=i32(packed&0x7fff);
    if(a.secondary||(a.effect==1&&color>=8))return false;
    b.sprite=i16(type);b.color=i16(color);b.hitbox={a.hitbox,a.hitbox};b.draw_group=a.group;
    if(!services.reset_primary()||!services.bind_primary(a.script))return false;
    b.flags|=0x10;b.vm.flags2=(b.vm.flags2&~0x80000u)|0x40000;
    if(!services.reset_secondary())return false;
    switch(a.effect){
    case 0:b.cancel_effect=color*2+4;break;case 1:b.cancel_effect=th16_bullet_cancel_scripts[color];break;
    case 2:b.cancel_effect=-1;break;case 3:b.cancel_effect=16;break;case 4:b.cancel_effect=6;break;case 5:b.cancel_effect=12;break;
    case 6:b.cancel_effect=a.sprites[color*4+3];break;
    case 7:b.cancel_effect=260;break;case 8:b.cancel_effect=263;break;case 9:b.cancel_effect=266;break;case 10:b.cancel_effect=275;break;
    }
    return !(packed&0x8000)||services.interrupt_primary();
#else
    (void)b;(void)type;(void)packed;(void)services;return false;
#endif
}
#if PORT_TITLE == 16
bool BulletManager::appearance_th16(BulletState& b,i32 type,u32 packed)noexcept{
    struct Services:Th16BulletAppearanceServices {
        BulletManager& manager;BulletState& bullet;
        Services(BulletManager& m,BulletState& b):manager(m),bullet(b){}
        bool reset_primary()override{bullet.vm.initialize();bullet.vm.sprite_callback=th16_sprite;bullet.vm.reserved_430=u32(uintptr_t(&bullet));return true;}
        bool bind_primary(i32 script)override{
            if(!bullet.vm.bind_script(manager.resource,script,manager.file_id,&manager.animations.rate))return false;
            bullet.vm.sprite_callback=th16_sprite;bullet.vm.reserved_430=u32(uintptr_t(&bullet));
            return bullet.vm.update(manager.animations)>=0;
        }
        bool reset_secondary()override{return true;} // Retail44-style catalog has no secondary script.
        bool interrupt_primary()override{if(bullet.vm.th16_callbacks[3])return false;bullet.vm.interrupt(2);return true;}
    } services(*this,b);
    return bullet_th16_change_appearance(b,type,packed,services);
}
#endif
bool bullet_th16_bind_metadata(BulletState& b,const Th16BulletEmitter& e,const BulletLaunch& launch,const float* rate)noexcept{
#if PORT_TITLE == 16
    // Retail412ed2..4137cb metadata. Real resource binding and updates are
    // separate consumers; only zero ETEX / leading appearance delay is ready.
    if(u32(e.sprite)>=44||u32(e.color)>=16||u32(e.transform_start)>=18)return false;
    const auto& a=th16_bullet_appearances[e.sprite];if(a.secondary)return false;
    const auto& t=e.transforms[e.transform_start];
    if(t.opcode!=0&&t.opcode!=1&&t.opcode!=2)return false;
    b.position=launch.position;b.velocity={launch.velocity.x,launch.velocity.y,0};b.speed=launch.speed;b.angle=launch.angle;
    b.flags=((b.flags|1u)&~12u)|2u|0x10u;b.state=1;b.collision_delay=0;b.th16_size=1;
    b.sprite=i16(e.sprite);b.color=i16(e.color);b.draw_group=a.group;b.hitbox={a.hitbox,a.hitbox};
    b.offscreen_grace=5;b.transform_sound=e.transform_sound;b.emitter_flags=e.flags;b.active_transforms=0;b.reserved_4c0=0;
    b.extra=0;b.th16_script_state=0;b.transform_index=e.transform_start;
    b.lifetime.set(0,rate);b.auxiliary_timer.set(0,rate);std::memcpy(b.th16_transforms,e.transforms,sizeof b.th16_transforms);
    switch(a.effect){
    case 0:b.cancel_effect=e.color*2+4;break;case 1:if(e.color>=8)return false;b.cancel_effect=th16_bullet_cancel_scripts[e.color];break;
    case 2:b.cancel_effect=-1;break;case 3:b.cancel_effect=16;break;case 4:b.cancel_effect=6;break;
    case 6:b.cancel_effect=a.sprites[e.color*4+3];break;
    case 7:b.cancel_effect=260;break;case 8:b.cancel_effect=263;break;case 9:b.cancel_effect=266;break;case 10:b.cancel_effect=275;break;
    default:return false;
    }
    if(t.opcode==2){b.state=2;b.position.x-=b.velocity.x*4.f;b.position.y-=b.velocity.y*4.f;b.position.z-=b.velocity.z*4.f;++b.transform_index;}
    return true;
#else
    (void)b;(void)e;(void)launch;(void)rate;return false;
#endif
}
}
