#include "style.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../gameplay/enemy.hpp"
#include "../program_entry/program_entry.hpp"
#include <atomic>
#include <cstring>
namespace th20::source::bullet {
std::int32_t __cdecl remap_sprite(sprite::Animation* animation,std::int32_t id){
    const auto& b=*reinterpret_cast<const Bullet*>(animation->field_5c8);const auto& s=styles[b.field_4c];
    if(recovered::signed_bits(s.colors[0].words[0])>=0)return recovered::signed_bits(s.colors[b.field_4e].words[id]);
    return id;
}
//4383a0: restart an existing VM without clearing the owner/remap callbacks.
//AnimationFile+5c is an atomic load (4292c0/40c420), not a vector-empty test.
void restart_animation(sprite::AnimationFile& file,sprite::Animation& a,std::int32_t script){
    if(!file.scripts[script]||std::atomic_ref(file.fields_5c[0]).load()!=0){std::memset(&a,0,sizeof(a));return;}
    a.base.field_440=static_cast<std::uint16_t>(script);a.base.fields_10_28[2]=a.base.fields_10_28[3]=file.id;
    a.base.flags[2]=(a.base.flags[2]&~0xc3u)|2u;a.base.fields_10_28[5]=script;a.base.fields_10_28[6]=0;
    recovered::timer_set(a.timer_4d8,0);recovered::timer_set(a.timer_4c8,0);a.base.flags[0]&=~0x10000u;a.field_5dc=0;
    sprite::execute_animation(a);++program_entry::sprite_controller->field_b8;
    if((a.base.flags[2]&3u)==2)a.base.flags[2]=(a.base.flags[2]&~3u)|1u;
}
namespace unrecovered {
void change_bullet_style(Bullet& b,const Command& op){
    auto& a=*b.animation;
    if(!(op.words[4]&0x8000u)){
        b.field_4c=static_cast<std::int16_t>(op.words[4]);b.field_4e=static_cast<std::int16_t>(op.words[5]&0x7fffu);
        b.size.x=b.size.y=bullet_radius(op.i(4));const auto& s=styles[b.field_4c];b.field_44=s.draw_group;
        sprite::reset_animation_state(a);a.field_5e0=reinterpret_cast<std::uintptr_t>(&remap_sprite);a.field_5c8=reinterpret_cast<std::uintptr_t>(&b);
        auto& owner=*static_cast<Controller*>(b.context->primary_owner);restart_animation(*owner.file,a,recovered::signed_bits(styles[op.i(4)].script));
        b.flags|=16;a.base.flags[2]=(a.base.flags[2]&~0x3000000u)|0x1000000u;
        auto& sprites=*program_entry::sprite_controller;sprite::request_animation_deletion(sprites,b.animation_handle);
        if(styles[op.i(4)].child_script)sprite::spawn_named_animation(sprites,*owner.file,b.animation_handle,"bullet",recovered::signed_bits(styles[op.i(4)].child_script),&b.position,0,-1,0);
        switch(s.cancel_type){
        case 0:b.cancel_script=b.field_4e*2+6;break;
        case 1:{constexpr std::int32_t scripts[8]={6,10,14,18,22,26,30,36};b.cancel_script=b.field_4e<8?scripts[b.field_4e]:0;break;}
        case 2:b.cancel_script=-1;b.flags|=16;break;
        case 3:b.cancel_script=18;break;
        case 4:b.cancel_script=8;break;
        case 5:b.cancel_script=14;break;
        case 6:b.cancel_script=b.field_4e<16?recovered::signed_bits(s.colors[b.field_4e].words[3]):0;b.flags|=16;break;
        case 7:b.cancel_script=0x107;b.flags|=16;break;
        case 8:b.cancel_script=0x10a;b.flags|=16;break;
        case 9:b.cancel_script=0x10d;b.flags|=16;break;
        case 10:b.cancel_script=0x116;b.flags|=16;break;
        }
        if(op.words[5]&0x8000u)sprite::set_animation_interrupt(a,2);
    }else{
        sprite::reset_animation_state(a);auto& enemy=*static_cast<gameplay::EnemyController*>(b.context->objects_04[1]);
        auto& file=*enemy.services->existing_animation(enemy,op.words[4]&0x7fffu);restart_animation(file,a,op.words[5]&0x7fffu);
        a.base.flags[2]=(a.base.flags[2]&~0x3000000u)|0x1000000u;
    }
}
}
}
