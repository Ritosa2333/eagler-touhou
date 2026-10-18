#pragma once
#include "power.hpp"
#include "shot_callbacks.hpp"
namespace th20::source::player_entity {
class OptionFrameServices {
public:
    virtual ~OptionFrameServices()=default;
    virtual PowerServices& power()=0;
    virtual ShotCallbackEnvironment& callbacks()=0;
    virtual void execute_interrupt(std::uint32_t handle,int event)=0; //44efc0, interrupt then execute child VM
};
OptionFrameServices& option_frame_services();
int update_option(Player&,Option&,OptionFrameServices&); //4fa860, ECX Player / stack Option
int update_option(Player&,Option&);
}
