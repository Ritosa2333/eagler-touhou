#pragma once
#include "AnmManager.hpp"
#include "EnemyVisualCommands.hpp"
namespace th15 {
// Enemy code sees logical animation services. The resource pool and portable
// draw queues are owned by AnmManager, with no native graphics objects here.
class EnemyAnmHost final:public EnemyVisualHost {
    AnmManager& manager;
    std::array<i32,6> banks{{-1,-1,-1,-1,-1,-1}};bool mapped=false;
public:
    explicit EnemyAnmHost(AnmManager& manager):manager(manager){}
    const std::string* failure()const noexcept override{return manager.error.empty()?nullptr:&manager.error;}
    bool map_resource(i32 slot,i32 id)noexcept{if(slot<0||slot>=i32(banks.size()))return false;banks[u32(slot)]=id;mapped=true;return true;}
    i32 resource_id(i32 slot)const noexcept{return !mapped?slot:slot>=0&&slot<i32(banks.size())?banks[u32(slot)]:-1;}
    AnmVm* find(u32 handle)override{return manager.registry.find(handle);}
    bool create(u32& handle,i32 resource,i32 script,i32 layer)override{handle=manager.create(resource_id(resource),script,layer);return handle!=0;}
    bool retire(u32& handle)override{return manager.retire(handle);}
    bool pause(u32 handle)override{return manager.pause(handle);}
    bool interrupt(u32 handle,i32 label)override{return manager.interrupt(handle,label);}
    bool rebind(u32& handle,i32 script)override{return manager.rebind(handle,script);}
    bool size(u32 handle,Vec2& out)const noexcept override{auto* vm=manager.registry.find(handle);if(!vm)return false;const auto& v=vm->visual;out={float(v.sprite_size.y*v.scale.y),float(v.sprite_size.x*v.scale.x)};return true;}
    bool change_direction(u32& handle,i32 resource,i32 script,i32 layer)override{auto* old=find(handle);const Vec3 position=old?old->variables.position:Vec3{};if(!retire(handle))return false;handle=manager.create(resource_id(resource),script,layer,8,position);return handle!=0;}
};
}
