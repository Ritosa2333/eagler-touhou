#include "EffectManager.hpp"
namespace th09 {
namespace {constexpr i32 scripts[]={28,33,34,35,36,32,86,63,87,85,88,89,90,91,92,93,88,88,88,53,28,29,30,31,88,88,88,88,88,88,88,88,88,88,88,88,88,88};}
EffectManager::EffectManager(EffectServices& s,i32 field,u32 n,u32 fixed):services(s),side(field),capacity(n),reserved_slots(fixed),actors(n+fixed+1){}
bool EffectManager::initialize(EffectActor& a,EffectKind kind,const Vec3& p,const Vec3& extra,u32 color){
    if(u32(kind)>=std::size(scripts))return false;
    before_write(a);a=EffectActor{};a.side=side;a.kind=kind;a.active=1;a.position=p;a.arguments=extra;a.animation=std::make_unique<AnmVm>();
    services.start_animation(*a.animation,side,false,scripts[u32(kind)]);a.animation->flags|=0x2000;a.animation->color1.d3dColor=i32(color);a.animation->pos2={};
    if(!initialize_effect(a,services))a.active=0;return true;
}
EffectActor* EffectManager::create(EffectKind kind,const Vec3& p,const Vec3& extra,i32 number,u32 color){
    for(u32 scanned=0;scanned<capacity;++scanned){auto& a=actors[cursor];cursor=(cursor+1)%capacity;if(a.active)continue;if(!initialize(a,kind,p,extra,color))return nullptr;if(--number==0)return &a;}
    before_write(actors[capacity+reserved_slots]);return &actors[capacity+reserved_slots];
}
EffectActor* EffectManager::slotted(EffectKind kind,const Vec3& p,u32 slot,u32 color){if(slot>=reserved_slots)return nullptr;auto& a=actors[capacity+slot];if(!initialize(a,kind,p,{},color))return nullptr;a.slot=i32(slot);return &a;}
void EffectManager::clear(){for(auto& a:actors){before_write(a);a=EffectActor{};}for(auto& l:draw_lists)l.clear();}
void EffectManager::update(u32 flags,u32 first_field_flags){
    if((flags&0x1800)||(first_field_flags&1))return;services.coordinate_side=side;for(auto& l:draw_lists)l.clear();count=0;
    for(u32 i=0;i<capacity+reserved_slots;++i){auto& a=actors[i];if(!a.active){a.animation.reset();a.burst.reset();a.colors.clear();a.texture.clear();continue;}++count;
        if(!update_effect(a,services)){a.active=0;continue;}
        if(!a.animation||services.advance_animation(*a.animation)){a.active=0;continue;}
        a.time.tick(services.timing);if(!a.hidden)draw_lists[a.upper_layer?2:a.layer?1:0].push_back(&a);
    }frame=wrapping_add(frame,1);
}
void EffectManager::draw(i32 layer){
    if(layer<0||layer>2)return;services.coordinate_side=side;services.begin_layer(side,layer);
    for(auto a:draw_lists[layer]){if(draw_effect(*a,services))continue;if(!a->animation)continue;auto& vm=*a->animation;
        if(side==2){vm.pos=a->position;vm.pos.z=0;}else{vm.pos=services.geometry[side].to_screen(a->position);vm.pos.z=.08f;}
        vm.pos.x+=vm.pos2.x;vm.pos.y+=vm.pos2.y;vm.pos.z+=vm.pos2.z;services.draw_animation(vm);}
}
}
