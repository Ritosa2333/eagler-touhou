#include "BulletFlow.hpp"
namespace th08 {
bool BulletFlow::setup(){
    // The original records the requested name but always loads etama.anm.
    state.animation=context.initial?resources.load(6,"etama.anm"):resources.get(6);
    if(context.initial)effects.base_animation=state.animation;
    if(!state.animation||!system.initialize(*state.animation,random))return false;items.reset();return true;
}
void BulletFlow::release(){if(context.release_resources)resources.release(6);}
bool BulletFlow::attach(Chain& owner,const char* requested){
    detach();state.reset();filename=requested?requested:"";state.filename=filename.c_str();chain=&owner;
    calculation.set_callback([](void* p){return static_cast<BulletFlow*>(p)->system.update()?JobResult::Continue:JobResult::Error;});calculation.argument=this;
    calculation.added=[](void* p){return static_cast<BulletFlow*>(p)->setup()?0:-1;};calculation.deleted=[](void* p){static_cast<BulletFlow*>(p)->release();return 0;};if(owner.add(&calculation,14))return false;
    drawing.set_callback([](void* p){return static_cast<BulletFlow*>(p)->system.draw()?JobResult::Continue:JobResult::Error;});drawing.argument=this;owner.add(&drawing,13,true);return true;
}
void BulletFlow::detach(){if(chain){chain->cut(&calculation);chain->cut(&drawing);chain=nullptr;}}
}
