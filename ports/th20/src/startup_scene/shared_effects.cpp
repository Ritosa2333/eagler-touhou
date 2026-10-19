#include "../effect_system/effect.hpp"
namespace th20::source::startup::unrecovered {
runtime::CallbackOwner* create_resource_0049e0c0(int index){return effects::create_controller(index);}
void release_resource_0049deb0(int index){effects::destroy_controller(index);}
}
namespace th20::source::gameplay::unrecovered {
bool effects_ready(){return effects::controller(0)->ready!=0;} //437520(0)->438520 readsEffectInf+38
}
