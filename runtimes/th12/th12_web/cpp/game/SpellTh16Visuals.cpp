#include "SpellTh16Visuals.hpp"
namespace th11 {
bool spell_th16_create_visuals(AnmManager& manager,EnemyAnimations& files,AnmResource& ascii,AnmResource& text,AnmResource& effect,
    i32 stage,i32 section,i32 variant,i32 timeout,Vec3 boss,u32* titles,u32& circle,u32& background,u32& flags,
    const std::function<bool(u32)>& title_delivery,const std::function<bool(i32)>& sound_delivery){
#if PORT_TITLE == 16
    if(stage<1||stage>7||variant<0||variant>1||!titles)return false;
    // Retail4a22d0 + stage*d4: two 40-byte descriptor groups at +34.
    const i32 rows[7][10]={{3,9,0,3,17,3,9,0,3,17},{3,11,0,3,19,-1,-1,0,-1,-1},
        {3,11,0,3,20,3,11,0,-1,-1},{3,12,0,3,19,4,12,0,4,19},
        {3,12,0,3,19,-1,-1,0,3,20},{3,10,0,3,17,-1,-1,0,-1,-1},
        {3,13,0,3,20,4,9,1,4,13}};
    const auto* row=rows[stage-1];const auto* bg=row+((section<43&&row[6]!=-1)?5:0);
    auto valid=[&](i32 slot){return slot>=0&&u32(slot)<files.resources.size()&&files.resources[slot].file;};
    if(!valid(bg[0])||(row[variant*5+3]!=-1&&!valid(row[variant*5+3])))return false;
    const float zero=0;
    auto create=[&](AnmResource& r,i32 script,u16 file,u32* handle){auto* vm=manager.create(r,script,file,0,false,false,nullptr,nullptr,nullptr,false,&zero);if(handle)*handle=vm?vm->id:0;return vm;};
    if(!create(ascii,0,2,titles)||!create(text,2,0,titles+1)||!create(ascii,1,2,titles+2))return false;
    if(!title_delivery(titles[1])||!sound_delivery(33))return false;
    auto* vm=create(effect,13,8,&circle);if(!vm)return false;vm->position=boss;
    for(i32 script:{11,12}){AnmVm* child=nullptr;for(auto* node=vm->child.next;node;node=node->next)if(node->value&&node->value->script_index==script){child=node->value;break;}if(!child)return false;child->integers[2]=timeout;}
    if(!create(effect,20,8,nullptr))return false;
    auto& file=files.resources[bg[0]];if(!create(*file.file,bg[1],file.id,&background))return false;
    flags=(flags&~0x200u)|((u32(bg[2])&1)<<9);
    if(row[variant*5+3]!=-1){auto& fx=files.resources[row[variant*5+3]];if(!create(*fx.file,row[variant*5+4],fx.id,nullptr))return false;}
    return true;
#else
    (void)manager;(void)files;(void)ascii;(void)text;(void)effect;(void)stage;(void)section;(void)variant;(void)timeout;(void)boss;(void)titles;(void)circle;(void)background;(void)flags;(void)title_delivery;(void)sound_delivery;return false;
#endif
}
}
