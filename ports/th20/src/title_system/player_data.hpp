#pragma once
#include "selection.hpp"
#include "../progress_state/manager.hpp"
#include <array>
#include <span>
namespace th20::source::title {
struct DataUnlockState {
    std::array<std::uint8_t,256> pressed{},previous{},current{};
    std::uint32_t matched=0,idle=0;
};
static_assert(sizeof(DataUnlockState)==0x308);
extern DataUnlockState data_unlock_state; //5c6248..5c654c
class UnlockEnvironment {
public:virtual ~UnlockEnvironment()=default;
    virtual int read_keyboard(std::span<std::uint8_t,256>)=0;
    virtual void unlock_progress()=0;
    virtual void sound(int)=0;
};
UnlockEnvironment& data_unlock_environment();
void advance_data_unlock(DataUnlockState&,UnlockEnvironment&); //shared524c10/52a8a0 body
void unlock_all_data(progress::SaveManager&,state::Random&); //51e1c0
class DataEnvironment {
public:
    SelectionEnvironment& selection;
    explicit DataEnvironment(SelectionEnvironment& s):selection(s){}
    virtual ~DataEnvironment()=default;
    virtual void open_stones()=0;
    virtual void select_stone(int)=0;
    virtual void hide_stones()=0;
    virtual void update_unlock_sequence()=0;
};
DataEnvironment& data_environment();
int card_count_by_difficulty(int); //4886b0
int update_player_data_menu(TitleInf&,SelectionEnvironment&); //5265a0
int update_player_data_detail(TitleInf&,DataEnvironment&); //524c10
int draw_player_data_detail(TitleInf&,text::Renderer&,const progress::Profile& selected,const progress::Profile& fallback,const char* stone_name); //5257f0
}
