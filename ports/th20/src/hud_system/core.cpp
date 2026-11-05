#include "hud.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include <stdexcept>
namespace th20::source::hud {
FrontInf* controller=nullptr;
FrontInf::FrontInf():life_handles{},bomb_handles{},number_handles{},life_animations{},bomb_animations{},number_animations{},handle_90(0),score_handles{},notice_handles{},handle_c8(0),handle_cc(0),handles_d0{},handles_f8{},fields_11c{},background_handle(0),age{},secondary_draw(nullptr),field_158(0),field_15c(0),score(0),field_168(0),field_16c(0),stage_file(nullptr),fields_174{},pairs{},secondary_age{},field_1b8(0),collecting(0),message_data(nullptr),message_index(0),field_1c8(0),field_1cc(0),panels{},field_2d0(0){flags&=~0x3fffu;controller=this;}
}
