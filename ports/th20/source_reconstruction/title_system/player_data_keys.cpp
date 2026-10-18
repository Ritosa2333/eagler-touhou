#include "player_data.hpp"
#include "player_data_constants.hpp"
namespace th20::source::title {
void advance_data_unlock(DataUnlockState& state,UnlockEnvironment& e){
    state.previous=state.current;int mode=e.read_keyboard(state.current);
    if(mode==2){
        constexpr unsigned scan_codes[26]={0x1e,0x30,0x2e,0x20,0x12,0x21,0x22,0x23,0x17,0x24,0x25,0x26,0x32,0x31,0x18,0x19,0x10,0x13,0x1f,0x14,0x16,0x2f,0x11,0x2d,0x15,0x2c};
        state.pressed.fill(0);for(unsigned i=0;i<26;++i)state.pressed[scan_codes[i]]=state.current[0x41+i];state.current=state.pressed;mode=1;
    }
    if(mode==1){
        for(unsigned i=0;i<256;++i)state.pressed[i]=(state.current[i]^state.previous[i])&state.current[i];
        if(state.matched>=12){e.unlock_progress();e.sound(17);state.matched=0;}
        else if(state.pressed[player_data_constants::unlock_keys[state.matched]]&0x80){++state.matched;state.idle=0;}
        else {std::uint8_t any=0;for(unsigned i=0;i<0x39;++i)any|=state.pressed[i];if(any&0x80)state.matched=0;}
    }
    ++state.idle;if(recovered::signed_bits(state.idle)>300)state.matched=state.idle=0;
}
}
