#include "option_frame.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/menu_animation.hpp"
namespace th20::source::player_entity {
namespace {
struct GameOptionFrame final:OptionFrameServices {
    PowerServices& power() override{return power_services();}
    ShotCallbackEnvironment& callbacks() override{return shot_callback_environment();}
    void execute_interrupt(std::uint32_t handle,int event) override{sprite::execute_animation_interrupt(*program_entry::sprite_controller,handle,event);}
};
}
OptionFrameServices& option_frame_services(){static GameOptionFrame result;return result;}
int update_option(Player& player,Option& option){return update_option(player,option,option_frame_services());}
}
