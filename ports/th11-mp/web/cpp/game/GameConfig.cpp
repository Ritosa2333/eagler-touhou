#include "GameConfig.hpp"
namespace th11 {
void GameConfig::reset(const u8* controller)noexcept{
    // CRT initializer 48abf0 runs before 420d80 copies the controller table.
    // Comparing only the zero-filled PE image would bind every action to 0.
    constexpr i16 defaults[]={0,1,2,4,-1,-1,-1,-1,3};
    std::array<u8,18> bindings{};std::memcpy(bindings.data(),controller?controller:reinterpret_cast<const u8*>(defaults),18);
    bytes={};const u32 signature=0x110003,position=0x80000000;const u16 threshold=600;
    std::memcpy(bytes.data(),&signature,4);std::memcpy(bytes.data()+4,bindings.data(),18);
    std::memcpy(bytes.data()+0x16,&threshold,2);std::memcpy(bytes.data()+0x18,&threshold,2);
    bytes[0x1b]=1;bytes[0x1c]=1;bytes[0x1f]=2;bytes[0x20]=100;bytes[0x21]=80;bytes[0x23]=2;
    std::memcpy(bytes.data()+0x28,&position,4);std::memcpy(bytes.data()+0x2c,&position,4);bytes[0x39]=1;
}
bool GameConfig::open(const u8* data,u32 size)noexcept{
    if(!data||size!=bytes.size())return false;u32 signature;std::memcpy(&signature,data,4);
    if(signature!=0x110003||data[0x1a]>=2||data[0x1b]>=3||data[0x1c]>=2||data[0x1d]>=4||data[0x1e]>=3||data[0x1f]>=3)return false;
    std::memcpy(bytes.data(),data,bytes.size());return true;
}
}
