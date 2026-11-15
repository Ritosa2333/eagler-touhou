#pragma once
#include "AnmVm.hpp"
#include "BulletSprites.hpp"
#include "BulletVisual.hpp"
namespace th19 {
// 52b8e0 / 52fca0 initialize descriptor body and independent color+58 origin.
struct LaserVisual {
    AnmVm body,origin,tip;
    int frame=-1,type=-1,color=-1;
    void seek(int target,int bulletType,int bulletColor,const std::unordered_map<i32,AnmScriptLoc>&scripts){
        if(type!=bulletType||color!=bulletColor||target<frame){
            body={};origin={};tip={};frame=-1;type=bulletType;color=bulletColor;
        }
        if(frame<0){
            auto bind=[&](AnmVm&vm,int id){auto it=scripts.find(id);if(it==scripts.end())return;
                vm.bind_entry(it->second.stream,it->second.len,&scripts);vm.tick();
                vm.interrupt(2);vm.tick();};
            bind(body,nativeBulletAnimation(type));bind(origin,58+color);
            // Constructors 52b8e0 / 52fca0 override ANM +4bb after
            // initialization and interrupt2: both beams and origins add light.
            body.blend_mode=1;origin.blend_mode=1;
            // 52fca0: third VM is not interrupted at creation.
            auto it=scripts.find((type<18||type==38?93:85)+color);
            if(it!=scripts.end()){tip.bind_entry(it->second.stream,it->second.len,&scripts);tip.tick();}
            frame=0;
        }
        while(frame<target){body.tick();origin.tick();tip.tick();++frame;}
    }
    int bodySprite()const{return nativeBulletFrameSprite(type,color,body.sprite);}
};
}
