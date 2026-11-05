#include "GameConfig.hpp"
namespace th15 {
namespace {template<class T>void put(u8* p,T value){std::memcpy(p,&value,sizeof value);}u32 word(const u8* p){u32 value;std::memcpy(&value,p,4);return value;}}
void GameConfig::reset(const u8* controller)noexcept{
    bytes.fill(0);put<u32>(bytes.data(),0x150002);if(controller)std::memcpy(bytes.data()+4,controller,20);
    put<u16>(bytes.data()+0x18,600);put<u32>(bytes.data()+0x1a,0x1000258);put<u16>(bytes.data()+0x1e,0x501);
    bytes[0x21]=2;bytes[0x22]=100;bytes[0x23]=80;put<u16>(bytes.data()+0x24,0x200);
    put<u32>(bytes.data()+0x28,0x100);put<u32>(bytes.data()+0x2c,0x80000000);put<u32>(bytes.data()+0x30,0x80000000);
}
bool GameConfig::open(const u8* data,u32 n)noexcept{if(!data||n!=bytes.size()||word(data)!=0x150002)return false;constexpr u8 limits[]={2,3,2,6,3,3};for(u32 i=0;i<6;i++)if(data[0x1c+i]>=limits[i])return false;std::memcpy(bytes.data(),data,n);return true;}
}
