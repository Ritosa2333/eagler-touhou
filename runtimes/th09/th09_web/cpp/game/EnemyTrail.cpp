#include "EnemyTrail.hpp"
#include "EclVm.hpp"
namespace th09 {
bool EnemyTrail::prepare_uv(const AnmVm& vm,i32 count){
    if(count<=2)return true;if(count>i32(vertices.size())||!vm.loadedSprite)return false;
    const auto& sprite=*vm.loadedSprite;const float start=vm.uvScrollPos.x+sprite.uvEnd.x;
    const float dy=(sprite.uvEnd.x-sprite.uvStart.x)/float((count+1)/2-1);
    float value=start;
    for(i32 i=0;i<count;i+=2){auto& v=vertices[i];v.uv={value,sprite.uvStart.y+vm.uvScrollPos.y};v.rhw=1;v.color=u32(vm.color1.d3dColor);value-=dy;}
    value=start;for(i32 i=1;i<count;i+=2){auto& v=vertices[i];v.uv={value,sprite.uvEnd.y+vm.uvScrollPos.y};v.rhw=1;v.color=u32(vm.color1.d3dColor);value-=dy;}
    return true;
}
bool EnemyTrail::command(EclVm& vm,const EclInstruction& i){
    flags=u8(vm.raw(i,0));length=i16(vm.integer(i,1));collision_length=i16(vm.integer(i,2));interval=i16(vm.integer(i,3));
    if(flags&8){if(!interval)return false;return prepare_uv(vm.animation.layers[0],(i32(length)/interval)*2);}return true;
}
}
