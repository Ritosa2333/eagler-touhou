#include "GameConfig.hpp"
namespace th17 {
namespace {template<class T>void put(u8* p,T value){std::memcpy(p,&value,sizeof value);}u32 word(const u8* p){u32 value;std::memcpy(&value,p,4);return value;}}
void GameConfig::reset(const u8* controller)noexcept{
 bytes.fill(0);put<u32>(bytes.data(),0x170002);constexpr u16 defaults[]={0,1,2,3,65535,65535,65535,65535,65535,0};std::memcpy(bytes.data()+4,controller?controller:reinterpret_cast<const u8*>(defaults),20);
 put<u16>(bytes.data()+0x18,600);put<u32>(bytes.data()+0x1a,0x1000258);put<u32>(bytes.data()+0x1e,0x2000501);
 put<u32>(bytes.data()+0x22,0x2005064);put<u32>(bytes.data()+0x28,0x100);put<u32>(bytes.data()+0x2c,0x80000000);put<u32>(bytes.data()+0x30,0x80000000);
}
bool GameConfig::open(const u8* p,u32 n)noexcept{if(!p||n!=bytes.size()||word(p)!=0x170002)return false;constexpr u8 limits[]={2,3,2,6,3,3};for(u32 i=0;i<6;i++)if(p[0x1c+i]>=limits[i])return false;std::memcpy(bytes.data(),p,n);return true;}
}
