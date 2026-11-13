#pragma once
#include "AnmManager.hpp"
namespace th11 {
#if PORT_TITLE == 14
// ASCII manager +191ec: 420d20 creates ascii script17 once. 420ca0 and
// 4369b3 broadcast interrupt1, then clear the handle without an extra tick.
struct LoadingTh14 {
    u32 handle=0;
    bool begin(AnmManager& manager,AnmResource& ascii,float x=480,float y=392){
        if(handle)return true;
        const Vec3 position{float(x*2.f),float(y*2.f),0};const float rotation=0;
        auto* vm=manager.create(ascii,17,2,0xffffffffu,false,false,&position,nullptr,nullptr,false,&rotation);
        if(!vm)return false;handle=vm->id;return true;
    }
    void finish(AnmManager& manager){
        if(auto* root=manager.find(handle)){
            root->interrupt(1);
            if(!root->child.previous)for(auto* node=root->child.next;node;node=node->next)node->value->interrupt(1);
        }
        handle=0;
    }
};
#endif
}
