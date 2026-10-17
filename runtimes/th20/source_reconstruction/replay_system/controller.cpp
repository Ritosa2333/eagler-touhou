#include "replay.hpp"
#include "../startup_scene/startup.hpp"
namespace th20::source::startup::unrecovered {runtime::CallbackOwner* owner_005c60fc=nullptr;}
namespace th20::source::replay {
ReplayInf* controller() noexcept{return static_cast<ReplayInf*>(startup::unrecovered::owner_005c60fc);}
}
