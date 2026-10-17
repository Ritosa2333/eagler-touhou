#include "hud.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../gameplay/enemy_frame.hpp"
#include "../item_system/rewards.hpp"
#include "../startup_scene/startup.hpp"
namespace th20::source::gameplay::unrecovered {
runtime::CallbackOwner* create_004b9090(){return hud::create();}
void reset_hud_owner(){(void)hud::initialize_stage(*hud::controller);}
void* boss_hud_005c06a4(){return hud::controller;}
namespace {class HudBombObserver final:public player_state::BombObserver {void update(int a,int b,int c) override{hud::set_bombs(*hud::controller,a,b,c);}};}
player_state::BombObserver* bomb_observer(){static HudBombObserver value;return hud::controller?&value:nullptr;}
}
namespace th20::source::item::unrecovered {
void hud_bombs_004b8650(void* owner,int a,int b,int c){hud::set_bombs(*static_cast<hud::FrontInf*>(owner),a,b,c);}
void hud_lives_004b8bf0(void* owner,int a,int b,int c){hud::set_lives(*static_cast<hud::FrontInf*>(owner),a,b,c);}
void hud_notice_004b90e0(void* owner,int a,int b){hud::notify(*static_cast<hud::FrontInf*>(owner),a,b);}
}
namespace th20::source::startup::unrecovered {
int initialize_resource_004b5900(){(void)hud::load_shared();return 0;} //literal original wrapper return
void release_resource_004b6560(){hud::unload_shared();}
}
