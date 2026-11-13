#include "LaserManager.hpp"
#include <new>
namespace th11 {
LaserManager::LaserManager(AnmResource& resource,AnmEnvironment& animations,LaserWorld& world,u16 file_id):resource(resource),animations(animations),world(world),file_id(file_id){sync();}
LaserManager::~LaserManager(){clear();}
i32 LaserManager::issue_id()noexcept{next_id=wrapping_add(next_id,1);if(next_id<0x10000)next_id=0x10000;return next_id;}
void LaserManager::append(LaserState& b)noexcept{b.previous=tail;b.next=nullptr;tail->next=&b;tail=&b;++active_count;}
void LaserManager::release(LaserState& b)noexcept{
    b.previous->next=b.next;if(b.next)b.next->previous=b.previous;if(tail==&b)tail=b.previous;--active_count;
#if PORT_TITLE == 16
    if(b.type==0){delete reinterpret_cast<Th16LaserLine*>(&b);return;}
#endif
#if PORT_TITLE == 14
    if(b.type==0){delete reinterpret_cast<Th14LaserLine*>(&b);return;}
    if(b.type==1){delete reinterpret_cast<Th14LaserInfinite*>(&b);return;}
    if(b.type==3){delete reinterpret_cast<Th14LaserShell*>(&b);return;}
    if(b.type==2){delete reinterpret_cast<Th14LaserCurve*>(&b);return;}
#endif
    if(b.type==0)delete reinterpret_cast<LaserLine*>(&b);else delete reinterpret_cast<LaserInfinite*>(&b);
}
void LaserManager::clear()noexcept{while(sentinel.next)release(*sentinel.next);}
i32 LaserManager::spawn(const LaserLineParameters& p){
#if PORT_TITLE == 14 || PORT_TITLE == 16
    (void)p;last_error=-2;return 0;
#else
    if(!updating)sync();if(active_count>=capacity)return 0;const auto id=issue_id();
    auto* l=new(std::nothrow) LaserLine{};if(!l){last_error=-2;return 0;}
    l->base.initialize(&rate);l->base.id=id;append(l->base);
    if(!laser_line_initialize(*l,p,resource,animations,*this,file_id)){last_error=-2;release(l->base);return 0;}return id;
#endif
}
i32 LaserManager::spawn(const LaserInfiniteParameters& p){
#if PORT_TITLE == 14 || PORT_TITLE == 16
    // TH14 kind 1 owns a different request/ANM object. Old-title callers
    // must not insert an incompatible object into the title-specific list.
    (void)p;last_error=-2;return 0;
#else
    if(!updating)sync();if(active_count>=capacity)return 0;const auto id=issue_id();
    auto* l=new(std::nothrow) LaserInfinite{};if(!l){last_error=-2;return 0;}
    l->base.initialize(&rate);l->base.type=1;l->base.id=id;append(l->base);
    if(!laser_infinite_initialize(*l,p,resource,animations,*this,file_id)){last_error=-2;release(l->base);return 0;}return id;
#endif
}
bool LaserManager::spawn_line(const LaserLineParameters& p){if(active_count>=capacity)return true;return spawn(p)!=0;}
bool LaserManager::change_appearance(LaserLine& l,i32 script){return laser_line_appearance(l,script,resource,animations,file_id);}
bool LaserManager::cut(LaserLine& l,Vec3 p,Vec3 size,bool reward,bool skip){return laser_line_cut_rectangle(l,p,size,reward,skip,*this)>=0;}
bool LaserManager::cut_infinite(LaserInfinite& l,Vec3 p,Vec3 size,bool reward,bool skip){return laser_infinite_cut_rectangle(l,p,size,reward,skip,*this)>=0;}
bool LaserManager::update(bool paused,bool frozen){
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    if(paused)return true;
    //43a6dd zeros the global before entering43a570. sync copies port
    //world fields; preserve that separate cached rate on return.
    const float saved_anm_rate=animations.rate;if(frozen)animations.rate=0;
    sync();last_error=0;updating=true;const float saved_rate=rate;
    if(frozen)rate=0;
#else
    if(paused)return true;sync();last_error=0;updating=true;const float saved_rate=rate,saved_anm_rate=animations.rate;if(frozen){rate=0;animations.rate=0;}
#endif
    for(auto* b=first();b;){
        auto* next=b->next;
#if PORT_TITLE == 16
        if(b->type==0){auto& laser=*reinterpret_cast<Th16LaserLine*>(b);auto& flags=laser.native_flags;
            if(flags&6){flags=(flags&~6u)|((flags+2u)&6u);if((flags&6)>=4){release(*b);b=next;continue;}}
            if(b->state==1){release(*b);b=next;continue;}
            if(flags&8){if(laser_th16_line_collision(laser,*this,true)<0){last_error=-2;break;}b=next;continue;}
            const i32 result=laser_th16_line_update(laser,animations,*this);
            if(result<0){last_error=result;break;}if(result)release(*b);else{b->lifetime.tick();flags|=1;b->rectangle_enabled=1;}
            b=next;continue;
        }
        last_error=-2;break;
#endif
#if PORT_TITLE == 14
        if(b->type==0){auto& laser=*reinterpret_cast<Th14LaserLine*>(b);auto& flags=laser.native_flags;
            if(flags&6){flags=(flags&~6u)|((flags+2u)&6u);if((flags&6)>=4){release(*b);b=next;continue;}}
            if(b->state==1){release(*b);b=next;continue;}
            if(flags&8){if(laser_th14_line_collision(laser,*this,true)<0){last_error=-2;break;}b=next;continue;}
            const i32 result=laser_th14_line_update(laser,animations,*this);
            if(result<0){last_error=result;break;}if(result)release(*b);else{b->lifetime.tick();flags|=1;b->rectangle_enabled=1;}
            b=next;continue;
        }
        if(b->type==1){auto& laser=*reinterpret_cast<Th14LaserInfinite*>(b);auto& flags=laser.native_flags;
            if(flags&6){flags=(flags&~6u)|((flags+2u)&6u);if((flags&6)>=4){release(*b);b=next;continue;}}
            if(b->state==1){release(*b);b=next;continue;}
            if(flags&8){if(laser_th14_infinite_collision(laser,*this,true)<0){last_error=-2;break;}b=next;continue;}
            const i32 result=laser_th14_infinite_update(laser,animations,*this);
            if(result<0){last_error=result;break;}if(result)release(*b);else{b->lifetime.tick();flags|=1;b->rectangle_enabled=1;}
            b=next;continue;
        }
        if(b->type==2){auto& curve=*reinterpret_cast<Th14LaserCurve*>(b);auto& flags=curve.native_flags;
            if(flags&6){flags=(flags&~6u)|((flags+2u)&6u);if((flags&6)>=4){release(*b);b=next;continue;}}
            if(b->state==1){release(*b);b=next;continue;}
            if(flags&8){if(laser_th14_curve_collision(curve,*this,true)<0){last_error=-2;break;}b=next;continue;}
            const i32 result=laser_th14_curve_update(curve,animations,*this);
            if(result<0){last_error=result;break;}if(result)release(*b);else{b->lifetime.tick();flags|=1;b->rectangle_enabled=1;}
            b=next;continue;
        }
        if(b->type==3){auto& shell=*reinterpret_cast<Th14LaserShell*>(b);auto& flags=shell.native_flags;
            if(flags&6){flags=(flags&~6u)|((flags+2u)&6u);if((flags&6)>=4){release(*b);b=next;continue;}}
            if(b->state==1){release(*b);b=next;continue;}
            if(flags&8){b=next;continue;}
            b->lifetime.tick();flags|=1;b->rectangle_enabled=1;b=next;continue;
        }
#endif
        if((b->marked&&++b->marked>=2)||b->state==1)release(*b);
        else{
            const i32 result=b->type==0?laser_line_update(*reinterpret_cast<LaserLine*>(b),animations,*this):laser_infinite_update(*reinterpret_cast<LaserInfinite*>(b),animations,*this);
            if(result<0){last_error=result;break;}
            if(result)release(*b);else{b->lifetime.tick();b->rectangle_enabled=1;}
        }
        b=next;
    }
    rate=saved_rate;animations.rate=saved_anm_rate;updating=false;return last_error==0;
}
bool LaserManager::draw(AnmRenderer& renderer,bool paused){
    if(paused)return true;
    for(auto* b=first();b;){auto* next=b->next;
#if PORT_TITLE == 16
        if(b->type==0){if(b->state!=1&&!laser_th16_line_draw(*reinterpret_cast<Th16LaserLine*>(b),renderer)){last_error=-2;return false;}b=next;continue;}
        last_error=-2;return false;
#endif
#if PORT_TITLE == 14
        if(b->type==0){if(b->state!=1&&!laser_th14_line_draw(*reinterpret_cast<Th14LaserLine*>(b),renderer)){last_error=-2;return false;}b=next;continue;}
        if(b->type==1){if(b->state!=1&&!laser_th14_infinite_draw(*reinterpret_cast<Th14LaserInfinite*>(b),renderer)){last_error=-2;return false;}b=next;continue;}
        if(b->type==2){if(b->state!=1&&!laser_th14_curve_draw(*reinterpret_cast<Th14LaserCurve*>(b),renderer)){last_error=-2;return false;}b=next;continue;}
#endif
        if(b->state!=1&&b->type!=3){const bool ok=b->type==0?laser_line_draw(*reinterpret_cast<LaserLine*>(b),renderer):laser_infinite_draw(*reinterpret_cast<LaserInfinite*>(b),renderer);if(!ok){last_error=-2;return false;}}b=next;}return true;
}
void LaserManager::mark_all()noexcept{for(auto* b=first();b;b=b->next)if(b->state!=1&&!b->marked){
#if PORT_TITLE == 16
    if(b->type==0){auto& flags=reinterpret_cast<Th16LaserLine*>(b)->native_flags;if(!(flags&6))flags|=2;continue;}
#endif
#if PORT_TITLE == 14
    if(b->type==0){auto& flags=reinterpret_cast<Th14LaserLine*>(b)->native_flags;if(!(flags&6))flags|=2;continue;}
    if(b->type==1){auto& flags=reinterpret_cast<Th14LaserInfinite*>(b)->native_flags;if(!(flags&6))flags|=2;continue;}
    if(b->type==3){auto& flags=reinterpret_cast<Th14LaserShell*>(b)->native_flags;if(!(flags&6))flags|=2;continue;}
    if(b->type==2){auto& flags=reinterpret_cast<Th14LaserCurve*>(b)->native_flags;if(!(flags&6))flags|=2;continue;}
#endif
    b->marked=1;}}
LaserState* LaserManager::find(i32 id)const noexcept{if(id)for(auto* b=first();b;b=b->next)if(b->id==id)return b;return nullptr;}
bool LaserManager::cancel_object(LaserState& b,bool reward,bool skip){
#if PORT_TITLE == 16
    if(b.type==0){const auto result=laser_th16_line_cancel(*reinterpret_cast<Th16LaserLine*>(&b),reward,skip,*this);if(result<0){last_error=result;return false;}return true;}
    last_error=-2;return false;
#endif
#if PORT_TITLE == 14
    if(b.type==0){const auto result=laser_th14_line_cancel(*reinterpret_cast<Th14LaserLine*>(&b),reward,skip,*this);if(result<0){last_error=result;return false;}return true;}
    if(b.type==1){const auto result=laser_th14_infinite_cancel(*reinterpret_cast<Th14LaserInfinite*>(&b),reward,skip,*this);if(result<0){last_error=result;return false;}return true;}
    if(b.type==3){if(!skip){auto& flags=reinterpret_cast<Th14LaserShell*>(&b)->native_flags;flags=(flags&~4u)|2;}return true;}
    if(b.type==2){const auto result=laser_th14_curve_cancel(*reinterpret_cast<Th14LaserCurve*>(&b),reward,skip,*this);if(result<0){last_error=result;return false;}return true;}
#endif
    const auto result=b.type==0?laser_line_cancel(*reinterpret_cast<LaserLine*>(&b),reward,skip,*this):laser_infinite_cancel(*reinterpret_cast<LaserInfinite*>(&b),reward,skip,*this);
    if(result<0){last_error=result;return false;}return true;
}
bool LaserManager::cancel_id(i32 id){
    sync();for(auto* b=find(id);b;b=find(id)){
        if(!cancel_object(*b,true,false))return false;b->id=0;
    }return true;
}
bool LaserManager::update_warning(float angular_delta){
#if PORT_TITLE == 14
    (void)angular_delta;for(auto* b=first();b;b=b->next)if(b->type==1&&b->state==3){last_error=-2;return false;}return true;
#else
    sync();for(auto* b=first();b;b=b->next)if(b->type==1&&!laser_infinite_warning(*reinterpret_cast<LaserInfinite*>(b),angular_delta,*this)){last_error=-2;return false;}return true;
#endif
}
bool LaserManager::cancel_all(bool reward,bool skip){
    sync();for(auto* b=first();b;){auto* next=b->next;if(b->state!=1&&!cancel_object(*b,reward,skip))return false;b=next;}return true;
}
i32 LaserManager::cancel_rectangle(Vec3 center,Vec3 size,bool reward){
    sync();last_center=center;last_size=size;i32 total=0;
    for(auto* b=first();b;){auto* next=b->next;if(b->type!=3&&b->state!=1&&b->rectangle_enabled){
#if PORT_TITLE == 16
        if(b->type==0){const i32 count=th16_line_cut(*reinterpret_cast<Th16LaserLine*>(b),center,{size.x,size.y},0,reward?1u:0u,true);if(count<0)return last_error=count;total=wrapping_add(total,count);b=next;continue;}
        return last_error=-2;
#endif
#if PORT_TITLE == 14
        if(b->type==0){const i32 count=th14_line_cut(*reinterpret_cast<Th14LaserLine*>(b),center,{size.x,size.y},0,reward?1u:0u,true);if(count<0)return last_error=count;total=wrapping_add(total,count);b=next;continue;}
        if(b->type==1){if(b->protection){b=next;continue;}return last_error=-2;}
        if(b->type==2){if(b->protection){b=next;continue;}return last_error=-2;}
#endif
        const auto count=b->type==0?laser_line_cut_rectangle(*reinterpret_cast<LaserLine*>(b),center,size,reward,true,*this):laser_infinite_cut_rectangle(*reinterpret_cast<LaserInfinite*>(b),center,size,reward,true,*this);if(count<0)return last_error=count;total=wrapping_add(total,count);}b=next;}return total;
}
i32 LaserManager::cancel_circle(Vec3 center,float radius,u32 rewards,bool skip){
    sync();last_center=center;i32 total=0;
    for(auto* b=first();b;){auto* next=b->next;if(b->type!=3&&b->state!=1){
#if PORT_TITLE == 16
        if(b->type==0){auto& l=*reinterpret_cast<Th16LaserLine*>(b);std::vector<std::array<u8,0x358>> fragments;
            const i32 count=laser_th16_line_cut_circle(l,center,radius,rewards,skip,fragments,*this);if(count<0)return last_error=count;
            for(const auto& request:fragments){if(active_count>=capacity)break;if(!spawn_th16_line(request.data()))return last_error=-2;}
            total=wrapping_add(total,count);b=next;continue;}
        return last_error=-2;
#endif
#if PORT_TITLE == 14
        if(b->type==0){auto& l=*reinterpret_cast<Th14LaserLine*>(b);std::vector<std::array<u8,0x354>> fragments;
            const i32 count=laser_th14_line_cut_circle(l,center,radius,rewards,skip,fragments,*this);if(count<0)return last_error=count;
            for(const auto& request:fragments){if(active_count>=capacity)break;if(!spawn_th14_line(request.data()))return last_error=-2;}
            total=wrapping_add(total,count);b=next;continue;}
        if(b->type==1){auto& l=*reinterpret_cast<Th14LaserInfinite*>(b);std::vector<std::array<u8,0x354>> fragments;
            const i32 count=laser_th14_infinite_cut_circle(l,center,radius,rewards,skip,fragments,*this);if(count<0)return last_error=count;
            for(const auto& request:fragments){if(active_count>=capacity)break;if(!spawn_th14_line(request.data()))return last_error=-2;}
            total=wrapping_add(total,count);b=next;continue;}
        if(b->type==2){const i32 count=laser_th14_curve_cut_circle(*reinterpret_cast<Th14LaserCurve*>(b),center,radius,rewards,skip,*this);if(count<0)return last_error=count;total=wrapping_add(total,count);b=next;continue;}
#endif
        const auto count=b->type==0?laser_line_cut_circle(*reinterpret_cast<LaserLine*>(b),center,radius,rewards,skip,*this):laser_infinite_cut_circle(*reinterpret_cast<LaserInfinite*>(b),center,radius,rewards,skip,*this);if(count<0)return last_error=count;total=wrapping_add(total,count);}b=next;}return total;
}
#if PORT_TITLE == 16
i32 LaserManager::spawn_th16_line(const u8* request){
    last_error=0;if(!updating)sync();if(active_count>=capacity)return 0;const i32 id=issue_id();auto* laser=new(std::nothrow) Th16LaserLine{};
    if(!laser){last_error=-2;return 0;}laser->base.initialize(&rate);laser->base.id=id;append(laser->base);
    if(!laser_th16_line_initialize(*laser,request,resource,animations,*this,file_id)){last_error=-2;release(laser->base);return 0;}return id;
}
bool LaserManager::th16_line_appearance(Th16LaserLine& l,i32 script){return l.body.bind_script(resource,script,file_id,&animations.rate)&&l.body.update(animations)>=0;}
i32 LaserManager::th16_line_cut(Th16LaserLine& l,Vec3 center,Vec2 size,float angle,u32 rewards,bool skip){
    std::vector<std::array<u8,0x358>> fragments;const i32 count=laser_th16_line_cut_rectangle(l,center,size,angle,rewards,skip,fragments,*this);if(count<0)return last_error=count;
    for(const auto& request:fragments){if(active_count>=capacity)break;if(!spawn_th16_line(request.data()))return last_error=-2;}return count;
}
bool LaserManager::reset_th16(){
    sync();for(auto* b=first();b;){auto* next=b->next;b->offscreen_timer.set(0,&rate);b->protection=0;if(!cancel_object(*b,true,false))return false;b=next;}return true;
}
#endif
#if PORT_TITLE == 14
i32 LaserManager::th14_line_cut(Th14LaserLine& l,Vec3 center,Vec2 size,float angle,u32 rewards,bool skip){
    std::vector<std::array<u8,0x354>> fragments;const i32 count=laser_th14_line_cut_rectangle(l,center,size,angle,rewards,skip,fragments,*this);if(count<0)return last_error=count;
    for(const auto& request:fragments){if(active_count>=capacity)break;if(!spawn_th14_line(request.data()))return last_error=-2;}
    return count;
}
i32 LaserManager::th14_infinite_cut(Th14LaserInfinite& l,Vec3 center,Vec2 size,float angle,u32 rewards,bool skip){
    std::vector<std::array<u8,0x354>> fragments;const i32 count=laser_th14_infinite_cut_rectangle(l,center,size,angle,rewards,skip,fragments,*this);if(count<0)return last_error=count;
    for(const auto& request:fragments){if(active_count>=capacity)break;if(!spawn_th14_line(request.data()))return last_error=-2;}return count;
}
i32 LaserManager::th14_curve_cut(Th14LaserCurve& curve,Vec3 center,Vec2 size,float angle,u32 rewards,bool skip){
    std::vector<std::array<u8,0x354>> fragments;const i32 count=laser_th14_curve_cut_rectangle(curve,center,size,angle,rewards,skip,fragments,*this);if(count<0)return last_error=count;
    for(const auto& request:fragments){if(active_count>=capacity)break;if(!spawn_th14_curve(request.data(),&curve.path.nodes))return last_error=-2;}return count;
}
i32 LaserManager::cancel_rectangle_th14(Vec3 center,Vec2 size,float angle,u32 rewards,bool skip_infinite){
    sync();last_center=center;last_size={size.x,size.y,0};
    i32 total=0;for(auto* b=first();b;){auto* next=b->next;
        if(!(skip_infinite&&b->type==1)&&b->type!=3&&b->state!=1&&b->rectangle_enabled&&!b->protection){
            if(b->type==0){const i32 count=th14_line_cut(*reinterpret_cast<Th14LaserLine*>(b),center,size,angle,rewards,true);if(count<0)return last_error=count;total=wrapping_add(total,count);b=next;continue;}
            if(b->type==1){
                const i32 count=th14_infinite_cut(*reinterpret_cast<Th14LaserInfinite*>(b),center,size,angle,rewards,true);if(count<0)return last_error=count;
                total=wrapping_add(total,count);b=next;continue;
            }
            if(b->type!=2)return last_error=-2;
            const i32 count=th14_curve_cut(*reinterpret_cast<Th14LaserCurve*>(b),center,size,angle,rewards,true);if(count<0)return last_error=count;
            total=wrapping_add(total,count);
        }b=next;
    }return total;
}
bool LaserManager::freeze_th14(bool value){
    for(auto* b=first();b;b=b->next){
        auto& flags=b->type==0?reinterpret_cast<Th14LaserLine*>(b)->native_flags:b->type==1?reinterpret_cast<Th14LaserInfinite*>(b)->native_flags:
            b->type==2?reinterpret_cast<Th14LaserCurve*>(b)->native_flags:reinterpret_cast<Th14LaserShell*>(b)->native_flags;
        flags=value?flags|8u:flags&~8u;
    }return true;
}
i32 LaserManager::spawn_th14_line(const u8* request){
    last_error=0;if(!updating)sync();if(active_count>=capacity)return 0;const i32 id=issue_id();auto* laser=new(std::nothrow) Th14LaserLine{};
    if(!laser){last_error=-2;return 0;}laser->base.initialize(&rate);laser->base.id=id;append(laser->base);
    if(!laser_th14_line_initialize(*laser,request,resource,animations,*this,file_id)){last_error=-2;release(laser->base);return 0;}return id;
}
i32 LaserManager::spawn_th14_infinite(const u8* request){
    last_error=0;if(!updating)sync();if(active_count>=capacity)return 0;const i32 id=issue_id();auto* laser=new(std::nothrow) Th14LaserInfinite{};
    if(!laser){last_error=-2;return 0;}laser->base.initialize(&rate);laser->base.type=1;laser->base.id=id;append(laser->base);
    if(!laser_th14_infinite_initialize(*laser,request,resource,animations,*this,file_id)){last_error=-2;release(laser->base);return 0;}return id;
}
bool LaserManager::reset_th14(){
    sync();for(auto* b=first();b;){auto* next=b->next;b->offscreen_timer.set(0,&rate);b->protection=0;
        if(!cancel_object(*b,true,false))return false;b=next;
    }return true;
}
i32 LaserManager::spawn_th14_curve(const u8* request,const std::vector<Th14CurveNode>* inherited){
    last_error=0;if(!updating)sync();if(active_count>=capacity)return 0;const i32 id=issue_id();auto* curve=new(std::nothrow) Th14LaserCurve{};
    if(!curve){last_error=-2;return 0;}curve->base.initialize(&rate);curve->base.id=id;
    if(!laser_th14_curve_initialize(*curve,request,resource,animations,*this,file_id,inherited)){delete curve;last_error=-2;return 0;}
    append(curve->base);return id;
}
i32 LaserManager::spawn_th14_shell(const u8* request){
    last_error=0;if(!updating)sync();if(active_count>=capacity)return 0;const i32 id=issue_id();auto* shell=new(std::nothrow) Th14LaserShell{};
    if(!shell){last_error=-2;return 0;}auto& b=shell->base;b.initialize(&rate);b.state=b.type=3;
    std::memcpy(shell->request,request,0x354);std::memcpy(&b.position,request,12);std::memcpy(&b.width,request+0x18,4);std::memcpy(&b.length,request+0x20,4);
    std::memcpy(&b.id,request+0x28,4);std::memcpy(&b.protection,request+0x2c,4);for(auto& value:shell->history)value=b.length;
    append(b);return id;
}
#endif
}
