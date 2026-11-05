#include "LaserManager.hpp"
#include <new>
namespace th11 {
LaserManager::LaserManager(AnmResource& resource,AnmEnvironment& animations,LaserWorld& world,u16 file_id):resource(resource),animations(animations),world(world),file_id(file_id){sync();}
LaserManager::~LaserManager(){clear();}
i32 LaserManager::issue_id()noexcept{next_id=wrapping_add(next_id,1);if(next_id<0x10000)next_id=0x10000;return next_id;}
void LaserManager::append(LaserState& b)noexcept{b.previous=tail;b.next=nullptr;tail->next=&b;tail=&b;++active_count;}
void LaserManager::release(LaserState& b)noexcept{
    b.previous->next=b.next;if(b.next)b.next->previous=b.previous;if(tail==&b)tail=b.previous;--active_count;
    if(b.type==0)delete reinterpret_cast<LaserLine*>(&b);else delete reinterpret_cast<LaserInfinite*>(&b);
}
void LaserManager::clear()noexcept{while(sentinel.next)release(*sentinel.next);}
i32 LaserManager::spawn(const LaserLineParameters& p){
    if(!updating)sync();if(active_count>=capacity)return 0;const auto id=issue_id();
    auto* l=new(std::nothrow) LaserLine{};if(!l){last_error=-2;return 0;}
    l->base.initialize(&rate);l->base.id=id;append(l->base);
    if(!laser_line_initialize(*l,p,resource,animations,*this,file_id)){last_error=-2;release(l->base);return 0;}return id;
}
i32 LaserManager::spawn(const LaserInfiniteParameters& p){
    if(!updating)sync();if(active_count>=capacity)return 0;const auto id=issue_id();
    auto* l=new(std::nothrow) LaserInfinite{};if(!l){last_error=-2;return 0;}
    l->base.initialize(&rate);l->base.type=1;l->base.id=id;append(l->base);
    if(!laser_infinite_initialize(*l,p,resource,animations,*this,file_id)){last_error=-2;release(l->base);return 0;}return id;
}
bool LaserManager::spawn_line(const LaserLineParameters& p){if(active_count>=capacity)return true;return spawn(p)!=0;}
bool LaserManager::change_appearance(LaserLine& l,i32 script){return laser_line_appearance(l,script,resource,animations,file_id);}
bool LaserManager::cut(LaserLine& l,Vec3 p,Vec3 size,bool reward,bool skip){return laser_line_cut_rectangle(l,p,size,reward,skip,*this)>=0;}
bool LaserManager::cut_infinite(LaserInfinite& l,Vec3 p,Vec3 size,bool reward,bool skip){return laser_infinite_cut_rectangle(l,p,size,reward,skip,*this)>=0;}
bool LaserManager::update(bool paused,bool frozen){
    if(paused)return true;sync();last_error=0;updating=true;const float saved_rate=rate,saved_anm_rate=animations.rate;if(frozen){rate=0;animations.rate=0;}
    for(auto* b=first();b;){
        auto* next=b->next;
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
    for(auto* b=first();b;){auto* next=b->next;if(b->state!=1){const bool ok=b->type==0?laser_line_draw(*reinterpret_cast<LaserLine*>(b),renderer):laser_infinite_draw(*reinterpret_cast<LaserInfinite*>(b),renderer);if(!ok){last_error=-2;return false;}}b=next;}return true;
}
void LaserManager::mark_all()noexcept{for(auto* b=first();b;b=b->next)if(b->state!=1&&!b->marked)b->marked=1;}
LaserState* LaserManager::find(i32 id)const noexcept{if(id)for(auto* b=first();b;b=b->next)if(b->id==id)return b;return nullptr;}
bool LaserManager::cancel_id(i32 id){
    sync();for(auto* b=find(id);b;b=find(id)){
        const auto result=b->type==0?laser_line_cancel(*reinterpret_cast<LaserLine*>(b),true,false,*this):laser_infinite_cancel(*reinterpret_cast<LaserInfinite*>(b),true,false,*this);
        if(result<0){last_error=result;return false;}b->id=0;
    }return true;
}
bool LaserManager::update_warning(float angular_delta){
    sync();for(auto* b=first();b;b=b->next)if(b->type==1&&!laser_infinite_warning(*reinterpret_cast<LaserInfinite*>(b),angular_delta,*this)){last_error=-2;return false;}return true;
}
bool LaserManager::cancel_all(bool reward,bool skip){
    sync();for(auto* b=first();b;){auto* next=b->next;if(b->state!=1){const auto result=b->type==0?laser_line_cancel(*reinterpret_cast<LaserLine*>(b),reward,skip,*this):laser_infinite_cancel(*reinterpret_cast<LaserInfinite*>(b),reward,skip,*this);if(result<0){last_error=result;return false;}}b=next;}return true;
}
i32 LaserManager::cancel_rectangle(Vec3 center,Vec3 size,bool reward){
    sync();last_center=center;last_size=size;i32 total=0;
    for(auto* b=first();b;){auto* next=b->next;if(b->state!=1&&b->rectangle_enabled){const auto count=b->type==0?laser_line_cut_rectangle(*reinterpret_cast<LaserLine*>(b),center,size,reward,true,*this):laser_infinite_cut_rectangle(*reinterpret_cast<LaserInfinite*>(b),center,size,reward,true,*this);if(count<0)return last_error=count;total=wrapping_add(total,count);}b=next;}return total;
}
i32 LaserManager::cancel_circle(Vec3 center,float radius,u32 rewards,bool skip){
    sync();last_center=center;i32 total=0;
    for(auto* b=first();b;){auto* next=b->next;if(b->state!=1){const auto count=b->type==0?laser_line_cut_circle(*reinterpret_cast<LaserLine*>(b),center,radius,rewards,skip,*this):laser_infinite_cut_circle(*reinterpret_cast<LaserInfinite*>(b),center,radius,rewards,skip,*this);if(count<0)return last_error=count;total=wrapping_add(total,count);}b=next;}return total;
}
}
