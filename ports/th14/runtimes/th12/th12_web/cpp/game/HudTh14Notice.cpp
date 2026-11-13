#include "Hud.hpp"
namespace th11 {
#if PORT_TITLE == 14
bool Hud::notice_th14(i32 type,i32 value){
    // Retail 434430. All roots append to the normal ANM registry, after
    // their first update with zero position/rotation and flag 200 set.
    if(type<0||type>6)return false;
    if(type==5)return true;
    auto erase=[&](u32& id){
        if(auto* vm=animations.find(id)){
            vm->flags|=0x4000000;
            if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)n->value->flags|=0x4000000;
        }
        id=0;
    };
    const float zero=0;
    // Retail4348fe copies the template without an intervening layer store.
    auto make=[&](AnmResource& resource,i32 script,u16 file,u32& id){
        auto* vm=animations.create(resource,script,file,u32(-1),false,false,nullptr,nullptr,nullptr,false,&zero);
        id=vm?vm->id:0;return vm;
    };
    if(type>=2&&type<=4){erase(item_notice);return make(front,59+type,5,item_notice)!=nullptr;}
    erase(spell_notice);
    if(!make(front,type==0?59:type==1?60:64,5,spell_notice))return false;
    if(type==6)return true;
    if(type==0){
        i32 remainder=value,divisor=10000000;bool significant=false;
        for(i32 i=0;i<8;++i){
            erase(bonus_digits[i]);auto* vm=make(text,4+i,2,bonus_digits[i]);if(!vm)return false;
            const i32 digit=remainder/divisor;remainder%=divisor;divisor/=10;
            significant=significant||digit!=0;
            if(!vm->bind_sprite(239+digit))return false;
            auto visibility=[&](AnmVm& node){if(significant)node.flags|=2;else node.flags&=~2u;};
            visibility(*vm);if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)visibility(*n->value);
        }
        for(i32 i=0;i<2;++i){
            erase(bonus_marks[i]);
            if(value>=(i?1000:1000000)){
                auto* vm=make(text,12+i,2,bonus_marks[i]);if(!vm||!vm->bind_sprite(253))return false;
            }
        }
    }
    // Failure leaves earlier bonus digits/marks alive. Neither result path
    // closes the previous time animation before replacing its handle.
    show_spell_time=true;return make(front,95,5,spell_time_animation)!=nullptr;
}
#endif
}
