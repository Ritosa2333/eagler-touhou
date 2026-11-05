#pragma once
#include "RecordStore.hpp"
namespace th15 {
struct TitleKeyboard {i32 format=0;std::array<u8,256> keys{};};
class TitleCheat {
 RecordStore& records;std::array<u8,256> current{},previous{};
public:
 i32 sequence=0,idle=0;std::string error;explicit TitleCheat(RecordStore& r):records(r){}
 bool update(u32 pressed,const TitleKeyboard&,bool& unlocked);
 const std::array<u8,256>& raw_current()const noexcept{return current;}
 const std::array<u8,256>& raw_previous()const noexcept{return previous;}
};
}
