#include "EffectFlow.hpp"
namespace th08 {
bool EffectFlow::setup(){
    effects.reset();effects.state.base_animation=resources.get(6);background.dialogue_state=0;background.spell_vm_count=2;
    if(context.keep_resources){effects.state.stage_animation=resources.get(9);return true;}
    constexpr const char* stages[]{"eff01.anm","eff02.anm","eff03.anm","eff04a.anm","eff04b.anm","eff05.anm","eff06.anm","eff07.anm","eff08.anm"};
    constexpr const char* words[]{"eff09sk.anm","eff09ym.anm","eff09al.anm","eff09rm.anm","eff09yy.anm","eff09yk.anm"};
    const char* path=nullptr;
    if(context.practice&&context.spell>=216){if(context.spell<222)path=words[context.spell-216];}
    else if(context.stage>=0&&context.stage<9)path=stages[context.stage];
    if(!path)return false;effects.state.stage_animation=resources.load(9,path);return effects.state.stage_animation!=nullptr;
}
void EffectFlow::release(){effects.release();if(!context.keep_resources)resources.release(9);}
bool EffectFlow::attach(Chain& owner){
    detach();effects.reset();chain=&owner;
    calculation.set_callback([](void* p){return static_cast<EffectFlow*>(p)->effects.update();});calculation.argument=this;
    calculation.added=[](void* p){return static_cast<EffectFlow*>(p)->setup()?0:-1;};calculation.deleted=[](void* p){static_cast<EffectFlow*>(p)->release();return 0;};
    if(owner.add(&calculation,13))return false;
    drawing.set_callback([](void* p){return static_cast<EffectFlow*>(p)->effects.draw();});drawing.argument=this;owner.add(&drawing,12,true);return true;
}
void EffectFlow::detach(){if(chain){chain->cut(&calculation);chain->cut(&drawing);chain=nullptr;}}
}
