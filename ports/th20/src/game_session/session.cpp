#include "session.hpp"
#include <algorithm>
#include <cstring>
namespace th20::source::game_session {
void construct_context(Context& c) noexcept {std::memset(&c,0,sizeof(c));}
void construct_player(Player& p) noexcept {
    std::fill_n(p.fields_00,11,0u);std::fill_n(p.bytes_2c,4,std::uint8_t(0));std::fill_n(p.fields_30,29,0u);
    p.fields_30[1]=400;p.fields_30[2]=100;p.fields_30[4]=10000;p.fields_30[6]=5000;
    p.fields_30[8]=100;p.fields_30[9]=1;p.fields_30[26]=0xffffffffu;
    p.bytes_a4[0]=p.bytes_a4[1]=0;p.fields_a8[0]=p.fields_a8[1]=0;p.byte_b0=0;
    std::fill_n(p.fields_b4,15,0u);p.fields_b4[1]=0xffffffffu;p.fields_b4[8]=2;
}
void construct_player_table(PlayerTable& p) noexcept {
    for(auto& player:p.players)construct_player(player);
    p.field_1e0=1;p.field_1e4=0;p.continue_count=0;std::fill_n(p.fields_1ec,14,0u);
    p.fields_1ec[5]=p.fields_1ec[6]=p.fields_1ec[13]=0xffffffffu;
}
void construct_session(Session& s) noexcept {
    for(auto& context:s.contexts)construct_context(context);
    s.field_60=s.field_64=s.field_68=s.flags=0;s.mode=0;std::fill_n(s.fields_74,4,0u);
    construct_player_table(s.player_table);s.field_2b0=0;s.field_2b8=0;s.field_2bc=1;
}
Session::Session(){construct_session(*this);}
Session session;
Context& context(std::int32_t index) noexcept {return session.contexts[index];}
Player& player(std::int32_t index) noexcept {return session.player_table.players[index];}
std::uint32_t& flags() noexcept {return session.flags;}
std::int32_t& mode() noexcept {return session.mode;}
runtime::CallbackOwner*& overlay_owner(std::int32_t index) noexcept {return context(index).overlay_owner;}
runtime::CallbackOwner*& primary_owner(std::int32_t index) noexcept {return context(index).primary_owner;}
void bind_default_player() noexcept {context(0).current_player=&player(0);}
void set_flag0(Session& s,std::uint32_t value) noexcept {s.flags=(s.flags&~1u)|(value&1);}
void set_flag1(Session& s,std::uint32_t value) noexcept {s.flags=(s.flags&~2u)|((value&1)<<1);}
void clear_game_mode_flags() noexcept {set_flag0(session,0);set_flag1(session,0);}
void add_continue_count(PlayerTable& table,std::int32_t delta) noexcept {
    const auto bits=static_cast<std::uint32_t>(table.continue_count)+static_cast<std::uint32_t>(delta);
    std::int32_t value;std::memcpy(&value,&bits,4);table.continue_count=std::clamp(value,0,9);
}
void increment_continue_count() noexcept {add_continue_count(session.player_table,1);}
}
