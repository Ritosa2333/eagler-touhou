#include "LaserManager.hpp"
#include <cstring>
namespace th10 {
// 0x40cf90. Id lookup includes lasers already marked for deletion.
EnemyLaser* LaserManager::find(u32 id) const noexcept {for(auto* laser=sentinel.next;laser;laser=laser->next)if(laser->id==id)return laser;return nullptr;}
// 0x41c030, 0x41c5b0, 0x41c680.
void EnemyLaser::initialize(const float* rate) noexcept {std::memset(this,0,sizeof(*this));timer.rate=rate;timer_flags=1;timer.initialize(-1);}
void StraightLaser::initialize(u32 methods,const float* rate) noexcept {
    base.initialize(rate);base.original_virtual_table=methods;std::memset(&parameters,0,sizeof(parameters));beam.clear();tip.clear();
}
void TimedLaser::initialize(u32 methods,const float* rate) noexcept {
    base.initialize(rate);base.original_virtual_table=methods;std::memset(&parameters,0,sizeof(parameters));parameters.growth_speed=8.f;beam.clear();tip.clear();
}
// 0x41c510. A non-supported kind still consumes an id, unless the pool is full.
u32 LaserManager::create(i32 kind,const void* parameters,LaserEnvironment& env){
    if(count>=256)return 0;if(++last_id==0)last_id=1;EnemyLaser* laser;
    if(kind==0){auto* storage=static_cast<StraightLaser*>(env.allocate(sizeof(StraightLaser)));storage->initialize(env.straight_methods,env.default_rate);laser=&storage->base;}
    else if(kind==1){auto* storage=static_cast<TimedLaser*>(env.allocate(sizeof(TimedLaser)));storage->initialize(env.timed_methods,env.default_rate);laser=&storage->base;}
    else return last_id;
    laser->id=last_id;laser->previous=tail;tail->next=laser;count=wrapping_add(count,1);tail=laser;env.initialize_laser(*laser,parameters);return last_id;
}
// 0x41c330. Update and destruction callbacks may change the object's links.
i32 LaserManager::update(LaserEnvironment& env){
    auto* laser=sentinel.next;
    while(laser){
        auto* next=laser->next;bool remove=false;
        if(laser->delete_wait){laser->delete_wait=static_cast<u8>(laser->delete_wait+1);remove=laser->delete_wait>=2;}
        if(remove||laser->state==1||env.update_laser(*laser)!=0){
            env.destroy_laser(*laser);count=wrapping_add(count,-1);laser->previous->next=laser->next;
            if(laser->next)laser->next->previous=laser->previous;if(tail==laser)tail=laser->previous;env.release(laser);
        }else laser->timer.tick();
        laser=next;
    }
    return 1;
}
i32 LaserManager::draw(LaserEnvironment& env){for(auto* laser=sentinel.next;laser;){auto* next=laser->next;if(laser->state!=1)env.draw_laser(*laser);laser=next;}return 1;}
// 0x417770. The stage reset removes every beam. The original leaves the
// manager's count and tail untouched; its destructor may change the links.
void LaserManager::clear(LaserEnvironment& env){for(auto* laser=sentinel.next;laser;){auto* next=laser->next;env.destroy_laser(*laser);laser->previous->next=laser->next;if(laser->next)laser->next->previous=laser->previous;env.release(laser);laser=next;}}
// 0x41c7d0: deletion is deferred until the next update, with byte wrap retained.
i32 LaserManager::schedule_deletion() noexcept {for(auto* laser=sentinel.next;laser;laser=laser->next)if(laser->state!=1&&!laser->delete_wait)laser->delete_wait=1;return 0;}
i32 LaserManager::cancel_all(i32 convert,LaserEnvironment& env){for(auto* laser=sentinel.next;laser;){auto* next=laser->next;if(laser->state!=1)env.cancel_laser(*laser,convert);laser=next;}return 1;}
i32 LaserManager::cancel_rectangle(const Vec3& center,const Vec3& size,i32 convert,LaserEnvironment& env){
    auto* laser=sentinel.next;last_cancel_position=center;last_cancel_size=size;i32 result=0;
    while(laser){auto* next=laser->next;if(laser->state!=1)result=wrapping_add(result,env.cancel_rectangle(*laser,center,size,convert));laser=next;}return result;
}
i32 LaserManager::cancel_circle(const Vec3& center,float radius,i32 convert,LaserEnvironment& env){
    auto* laser=sentinel.next;last_cancel_position=center;i32 result=0;
    while(laser){auto* next=laser->next;if(laser->state!=1)result=wrapping_add(result,env.cancel_circle(*laser,center,radius,convert));laser=next;}return result;
}
// 0x41c880. Both concrete laser classes share the same geometric query.
i32 LaserManager::intersections(const Vec3& center,float margin) const noexcept {
    i32 result=0;for(auto* laser=sentinel.next;laser;laser=laser->next)if(laser->state!=1)result=wrapping_add(result,laser->intersects(center,margin));return result;
}
// 0x41c480 / 0x41c4e0: pause disables updates; a separate flag freezes time only.
i32 LaserManager::tick(u32 flags,float& global_rate,LaserEnvironment& env){
    if(flags&0x405)return 1;if(flags&2){const float saved=global_rate;global_rate=0;const i32 result=update(env);global_rate=saved;return result;}return update(env);
}
i32 LaserManager::render(u32 flags,LaserEnvironment& env){return flags&4?1:draw(env);}
}
