#include "ending.hpp"
#include "../progress_state/manager.hpp"
#include "../game_session/session.hpp"
#include <stdexcept>
namespace th20::source::ending {
int ending_record(unsigned index){std::lock_guard lock(runtime::shared_locks().slot(20));progress::manager->verify_metadata();if(index>=32)throw std::out_of_range("Ending record outside32 entries");return static_cast<std::int8_t>(progress::manager->current.metadata.bytes[0x16+index]);}
int select_ending(){
    auto& session=game_session::session;game_session::add_continue_count(session.player_table,0);const auto& p=*session.contexts[0].current_player;
    const auto id=session.player_table.continue_count==0?p.fields_00[2]*8u+p.fields_00[3]:p.fields_00[2]+16u;session.field_2b8=id;return recovered::signed_bits(id);
}
unsigned held_frames(const input::ButtonState* buttons,unsigned index){return buttons&&(buttons->current&(1u<<(index&31)))?buttons->held_frames[index]:0;}
}
