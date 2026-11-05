#include "AnmFrame.hpp"
namespace th10 {
// 0x448860. World instances are grouped into 19 stable draw lists after their
// callbacks and scripts run. Retain the registry successor before callbacks.
i32 AnmManager::update_world(AnmFrameEnvironment& env){
    AnmVm* tails[19];for(u32 index=0;index<19;++index){tails[index]=&draw_layers[index];tails[index]->draw_next=nullptr;}
    auto* node=registry.world_head;while(node){auto& vm=*node->value;node=node->next;
        if(vm.flags&0x04000000)remove(vm,*env.allocation);
        else{if(vm.update_callback)env.callback(vm.update_callback,vm);if(env.update(vm))remove(vm,*env.allocation);else{
            const auto layer=vm.owner_tag;if(layer>=19)__builtin_trap();tails[layer]->draw_next=&vm;tails[layer]=&vm;vm.draw_next=nullptr;
        }}++processed_count;
    }return 1;
}
// 0x448900. UI update resets the shared count; world update adds to it.
i32 AnmManager::update_ui(AnmFrameEnvironment& env){
    auto* tail=&draw_layers[19];tail->draw_next=nullptr;processed_count=0;auto* node=registry.ui_head;
    while(node){auto& vm=*node->value;node=node->next;
        if(vm.flags&0x04000000)remove(vm,*env.allocation);
        else{if(vm.update_callback)env.callback(vm.update_callback,vm);if(env.update(vm))remove(vm,*env.allocation);else{tail->draw_next=&vm;tail=&vm;vm.draw_next=nullptr;}}
        ++processed_count;
    }return 1;
}
// 0x448980. Read the successor after drawing, since a custom draw callback can
// change that link. A callback's deletion marker takes effect next traversal.
i32 AnmManager::draw_layer(u32 layer,AnmFrameEnvironment& env){
    if(layer>=20)__builtin_trap();auto* vm=draw_layers[layer].draw_next;
    while(vm){if(!(vm->flags&0x04000000)){if(vm->draw_callback)env.callback(vm->draw_callback,*vm);env.draw(*vm);}vm=vm->draw_next;}return 1;
}
}
