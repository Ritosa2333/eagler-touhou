#include "shot_controller_frame.hpp"
#include "../input/input.hpp"
#include "../gameplay/gameplay.hpp"
#include "../gameplay/enemy.hpp"
#include "../hud_system/hud.hpp"
#include "../stage_clear/stage_clear.hpp"
#include "../overlay_system/overlay.hpp"
#include <bit>
namespace th20::source::player_entity {
namespace {
class GameShotController final:public ShotControllerServices {
public:
    ShotFrameServices& frames() override{return shot_frame_services();}
    bool world_allows_shooting() override {
        if(hud::controller->collecting)return false;
        auto* enemy=static_cast<gameplay::EnemyController*>(game_session::context(0).objects_04[1]);
        if(!enemy||!enemy->field_124||stage_clear::controller())return false;
        return !(gameplay::controller->game_flags&0x80u);
    }
    bool input_blocked() override{return (gameplay::controller->game_flags&0x80000u)!=0;}
    int input_slot(int view) override{return std::bit_cast<int>(view?input::controller->retained_2e34:input::controller->retained_2e30);}
    std::uint32_t held(int slot,std::uint32_t mask) override{if(slot<0)return 0;auto* state=input::button_slot(unsigned(slot));return state?state->retained_298[1]&mask:0;}
    void shoot_weapons(game_session::Context& context,int frame,int secondary,int power) override{overlay::shoot(*static_cast<overlay::WeaponStoneInf*>(context.overlay_owner),frame,secondary,power);}
    void activate_weapons(game_session::Context& context,bool inactive) override{overlay::activate_weapons(*static_cast<overlay::WeaponStoneInf*>(context.overlay_owner),inactive);}
};
}
ShotControllerServices& shot_controller_services(){static GameShotController result;return result;}
void update_shot_controller(ShotController& owner){update_shot_controller(owner,shot_controller_services());}
}
