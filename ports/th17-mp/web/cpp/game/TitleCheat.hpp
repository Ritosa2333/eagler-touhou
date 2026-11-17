#pragma once
#include "RecordStore.hpp"
namespace th17 {
struct TitleKeyboard {i32 format=0;std::array<u8,256> keys{};};
class TitleCheat {
 RecordStore& records;std::array<u8,256> current{},previous{};
public:
 i32 sequence=0,idle=0;std::string error;explicit TitleCheat(RecordStore& r):records(r){}
 bool update(u32 pressed,const TitleKeyboard&,bool& unlocked,bool trophies=false);
};
}
