#include "pause.hpp"
#include "../gameplay/subsystems.hpp"
#include "../startup_scene/startup.hpp"
#include <stdexcept>
namespace th20::source::startup::unrecovered {extern runtime::CallbackOwner* owner_005c60bc;}
namespace th20::source::gameplay::unrecovered {
runtime::CallbackOwner*& owner(Owner which){
    switch(which){
    case Owner::global_005c60bc:return startup::unrecovered::owner_005c60bc;
    case Owner::global_005c60fc:return startup::unrecovered::owner_005c60fc;
    default:throw std::invalid_argument("Game owner must use its recovered typed subsystem");
    }
}
}
