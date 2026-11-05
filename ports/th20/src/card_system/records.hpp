#pragma once
#include "../progress_state/manager.hpp"
#include <stdexcept>
namespace th20::source::card {
inline std::uint8_t* record(progress::Profile& profile,int index){
    if(index<0||index>=113)throw std::out_of_range("Card record outside original 113 entries");
    return reinterpret_cast<std::uint8_t*>(&profile)+0xb08+index*0xe0;
}
inline int record_count(const std::uint8_t* record,unsigned offset,unsigned mode){return progress::read<int>(record,offset+mode*4);}
inline void increment_record(std::uint8_t* record,unsigned offset,unsigned mode){const int old=record_count(record,offset,mode);if(old<99999)progress::write(record,offset+mode*4,old+1);}
}
