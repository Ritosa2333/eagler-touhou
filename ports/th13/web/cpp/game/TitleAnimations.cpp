#include "TitleAnimations.hpp"
namespace th13 {
bool TitleAnimations::check(bool ok){if(!ok&&error.empty())error=animations.error.empty()?"Title animation unavailable":animations.error;return ok;}
bool TitleAnimations::create(i32 script){state.handles[script]=animations.create(title_bank,script,-1,0);return check(state.handles[script]!=0);}
// 逐实例平移覆盖（行条/大行条按行位落点；AnmManager::create translation 语义，
// 位置 = translation + 脚本 ins_400 位置，见 AnmTransforms）。
bool TitleAnimations::create_at(i32 script,const Vec3& translation){state.handles[script]=animations.create(title_bank,script,-1,2,translation);return check(state.handles[script]!=0);}
bool TitleAnimations::interrupt(i32 script,i32 label,bool immediate){return check(animations.interrupt(state.handles[script],label,immediate));}
bool TitleAnimations::handle_interrupt(u32 handle,i32 label,bool immediate){return check(animations.interrupt(handle,label,immediate));}
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
