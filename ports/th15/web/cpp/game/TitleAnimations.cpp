#include "TitleAnimations.hpp"
namespace th15 {
bool TitleAnimations::check(bool ok){if(!ok&&error.empty())error=animations.error.empty()?"Title animation unavailable":animations.error;return ok;}
bool TitleAnimations::create(i32 script){state.handles[script]=animations.create(title_bank,script,-1,0);return check(state.handles[script]!=0);}
bool TitleAnimations::prompt(){if(state.handles[203])return true;state.handles[203]=animations.create(ascii_bank,19,-1,0);return check(state.handles[203]!=0);}
bool TitleAnimations::interrupt(i32 script,i32 label,bool immediate){return check(animations.interrupt(state.handles[script],label,immediate));}
bool TitleAnimations::retire(i32 script){const bool ok=interrupt(script,1);state.handles[script]=0;return ok;}
bool TitleAnimations::child_interrupt(i32 root,i32 script,i32 label,bool immediate){
    auto* vm=animations.registry.find(state.handles[root]);if(!vm){state.handles[root]=0;return true;}
    auto* child=animations.registry.find_child_script(*vm,script,0);return !child||check(animations.interrupt(animations.registry.handle(*child),label,immediate));
}
bool TitleAnimations::hide_child(i32 root,i32 script){
    auto* vm=animations.registry.find(state.handles[root]);if(!vm){state.handles[root]=0;return true;}
    auto* child=animations.registry.find_child_script(*vm,script,0);return !child||check(animations.pause(animations.registry.handle(*child),true));
}
}
