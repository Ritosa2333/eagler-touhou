#include "BulletVisuals.hpp"
#include "Arithmetic.hpp"
namespace th09 {
namespace {
// TH09 1.50a static script/color tables at 0x48e628, 0x4a0f18 and 0x4a0f58.
constexpr i32 scripts[23][5]={
    {0,18,19,20,15},{1,21,22,23,16},{2,21,22,23,16},{3,21,22,23,16},{4,21,22,23,16},{5,21,22,23,16},{6,21,22,23,16},
    {7,24,24,24,17},{8,24,24,24,17},{9,24,24,24,17},{25,27,27,27,26},
    {96,21,22,23,16},{97,21,22,23,16},{98,21,22,23,16},{99,24,24,24,17},{100,24,24,24,17},
    {101,21,22,23,16},{102,21,22,23,16},{103,24,24,24,17},{104,24,24,24,17},{105,24,24,24,17},{106,21,22,23,16},{107,24,24,24,17}
};
constexpr i32 small_colors[16]={0,1,1,1,1,2,2,2,2,3,3,3,4,4,4,0};
constexpr i32 medium_colors[8]={0,1,1,2,2,3,4,0};
}
bool BulletVisuals::initialize(){
    for(u32 i=0;i<appearances.size();++i){
        auto& a=appearances[i];
        for(u32 n=0;n<5;++n){const i32 script=scripts[i][n];if(script<0||u32(script)>=file.scriptCount)return false;
            auto& vm=a.animations[n];std::memset(&vm,0,sizeof(vm));vm.scriptIndex=i16(script);executor.start(file,vm,file.scripts[script]);vm.zWriteDisabled=true;}
        auto& body=a.animations[0];if(!body.loadedSprite||executor.invalid)return false;
        a.base_sprite=body.activeSpriteIndex;body.baseSpriteIndex=body.activeSpriteIndex;
        const float height=body.loadedSprite->heightPx;a.height=u8(truncate(height));float hit;
        if(height<=8){hit=4;a.draw_group=5;}
        else if(height<=16){
            switch(scripts[i][0]){case 2:case 4:case 5:case 6:case 96:case 97:case 98:case 101:case 102:case 106:hit=4;a.draw_group=4;break;
            default:hit=6;a.draw_group=3;break;}
        }else if(height<=32){
            switch(scripts[i][0]){case 8:case 103:case 104:case 105:case 107:hit=5;a.draw_group=2;break;
            case 9:case 99:case 100:hit=8;a.draw_group=1;break;default:hit=10;a.draw_group=1;break;}
        }else {hit=24;a.draw_group=0;}
        a.hitbox.x=hit;a.hitbox.y=hit;
    }
    return true;
}
void BulletVisuals::copy_metadata(Bullet& b,const BulletAppearance& a){b.hitbox=a.hitbox;b.base_sprite=a.base_sprite;b.template_flags=a.flags;b.source_height=a.height;b.draw_group=a.draw_group;}
void BulletVisuals::refresh(Bullet& b,u32 index){
    const auto& v=instances[index][0];b.animation_height=v.spriteSize.y;b.has_body_script=v.currentInstruction!=nullptr;
    if(v.loadedSprite){b.cull_width=v.loadedSprite->widthPx;b.cull_height=v.loadedSprite->heightPx;}
}
bool BulletVisuals::change_color(AnmVm& vm,i32 base,i32 color,float height){
    if(vm.activeSpriteIndex==base+color)return true;
    i32 sprite=base+color;
    if(height<=16){if(color<0||color>=16)return false;sprite=base+small_colors[color];}
    else if(height<=32){if(color<0||color>=8)return false;sprite=base+medium_colors[color];}
    return file.SetSprite(&vm,sprite)==0;
}
bool BulletVisuals::prepare(Bullet& b,u32 index,i32 type,i32 color,u32 flags){
    if(index>=instances.size()||type<0||u32(type)>=appearances.size())return false;
    if(checkpoint)checkpoint->BeforeBytes(&instances[index],sizeof(instances[index]));
    const auto& a=appearances[type];auto& v=instances[index];v[0]=a.animations[0];v[4]=a.animations[4];copy_metadata(b,a);
    const i32 sprite=a.animations[0].activeSpriteIndex+color;
    if(v[0].activeSpriteIndex!=sprite&&file.SetSprite(&v[0],sprite))return false;
    if(!v[0].loadedSprite)return false;
    const float height=v[0].loadedSprite->heightPx;
    if(!change_color(v[4],a.animations[4].activeSpriteIndex,color,height))return false;
    const u32 phase=flags&2?1:flags&4?2:flags&8?3:0;
    if(phase){v[phase]=a.animations[phase];if(!change_color(v[phase],a.animations[phase].activeSpriteIndex,color,height))return false;}
    refresh(b,index);return true;
}
bool BulletVisuals::change_type(Bullet& b,u32 index,i32 type,i32 color){
    if(index>=instances.size()||type<0||u32(type)>=appearances.size())return false;
    if(checkpoint)checkpoint->BeforeBytes(&instances[index],sizeof(instances[index]));
    const auto& a=appearances[type];instances[index]=a.animations;copy_metadata(b,a);
    if(file.SetSprite(&instances[index][0],a.animations[0].activeSpriteIndex+color))return false;refresh(b,index);return true;
}
bool BulletVisuals::set_sprite(Bullet& b,u32 index,i32 sprite){if(index>=instances.size())return false;if(checkpoint)checkpoint->BeforeBytes(&instances[index],sizeof(instances[index]));if(file.SetSprite(&instances[index][0],sprite))return false;refresh(b,index);return true;}
bool BulletVisuals::advance(Bullet& b,u32 index,BulletAnimation animation){
    if(index>=instances.size())return true;if(checkpoint)checkpoint->BeforeBytes(&instances[index],sizeof(instances[index]));const bool finished=executor.execute(instances[index][u32(animation)]);if(animation==BulletAnimation::body)refresh(b,index);return finished;
}
}
