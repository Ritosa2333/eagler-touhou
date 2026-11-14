#include "SpellTh14Visuals.hpp"
namespace th11 {
bool spell_th14_create_visuals(AnmManager& manager,EnemyAnimations& files,AnmResource& ascii,AnmResource& text,AnmResource& effect,
    i32 stage,i32 section,i32 id,i32 timeout,Vec3 boss,u32* titles,u32& circle,u32& background,
    const std::function<bool(u32)>& title_delivery,const std::function<bool(i32)>& sound_delivery){
#if PORT_TITLE == 14
    if(stage<1||stage>7||!titles)return false;
    // 4d5988 + stage*cc: +3c/+40/+44/+48, then +60/+64/+68/+6c.
    const i32 descriptors[7][8]={{3,11,3,19,4,7,4,10},{3,23,3,31,-1,-1,-1,-1},
        {3,14,3,21,3,14,3,21},{3,9,3,16,3,9,4,12},{3,9,3,16,-1,-1,-1,-1},
        {3,15,3,22,-1,-1,-1,-1},{3,13,3,19,4,9,4,13}};
    const auto* row=descriptors[stage-1];
    const bool alternate=(section<6&&row[4]!=-1)||(u32(id)-52<=11);
    const auto* bg=row+(alternate?4:0);
    // Retail would dereference slot -1 for an invalid stage/spell combination.
    for(i32 n:{0,2})if(bg[n]!=-1&&(u32(bg[n])>=files.resources.size()||!files.resources[bg[n]].file))return false;
    if(bg[0]<0)return false;
    const float zero=0;
    // Retail41cd1b/41cdc3/41ce70 copy each resource template directly;
    // these inline factories preserve its layer before the first live update.
    auto create=[&](AnmResource& resource,i32 script,u16 file,u32* handle){auto* vm=manager.create(resource,script,file,u32(-1),false,false,nullptr,nullptr,nullptr,false,&zero);if(handle)*handle=vm?vm->id:0;return vm;};
    if(!create(ascii,0,2,titles)||!create(text,2,0,titles+1)||!create(ascii,1,2,titles+2))return false;
    if(!title_delivery(titles[1])||!sound_delivery(33))return false;
    auto* vm=create(effect,13,8,&circle);if(!vm)return false;vm->position=boss;
    for(i32 script:{11,12}){AnmVm* found=nullptr;for(auto* node=&vm->child;node;node=node->next)if(node->value&&node->value!=vm&&node->value->script_index==script){found=node->value;break;}
        if(!found)return false;found->integers[2]=timeout;}
    if(!create(effect,20,8,nullptr))return false;
    for(i32 n:{0,2})if(bg[n]!=-1){auto& file=files.resources[bg[n]];if(!create(*file.file,bg[n+1],file.id,n?nullptr:&background))return false;}
    return true;
#else
    (void)manager;(void)files;(void)ascii;(void)text;(void)effect;(void)stage;(void)section;(void)id;(void)timeout;(void)boss;(void)titles;(void)circle;(void)background;(void)title_delivery;(void)sound_delivery;return false;
#endif
}
}
