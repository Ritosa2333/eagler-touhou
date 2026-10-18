#include "GameConfiguration.hpp"
#include "Binary.hpp"
namespace th09 {
InputBindings GameConfiguration::default_bindings(){return {{{0,1,2,4,-1,-1,-1,-1,3}},{{{90,88,16,27,38,40,37,39,17},{44,45,42,1,200,208,203,205,29}}}};}
void GameConfiguration::reset(){
    bytes.fill(0);const auto bindings=default_bindings();for(u32 side=0;side<2;++side)std::memcpy(bytes.data()+side*54,&bindings,54);
    const u32 version=0x90003;const u16 threshold=600;std::memcpy(bytes.data()+0xa4,&version,4);std::memcpy(bytes.data()+0xa8,&threshold,2);std::memcpy(bytes.data()+0xaa,&threshold,2);
    bytes[0xae]=bytes[0xaf]=bytes[0xb0]=bytes[0xb8]=1;bytes[0xb3]=bytes[0xb9]=2;bytes[0xba]=100;bytes[0xbb]=80;
}
bool GameConfiguration::load(const u8* p,u32 size){
    if(!p||size!=bytes.size())return false;u32 version;std::memcpy(&version,p+0xa4,4);
    constexpr u8 limits[]{3,2,3,2,6,2,3,3};if(version!=0x90003)return false;
    for(u32 n=0;n<8;++n)if(p[0xac+n]>=limits[n])return false;
    std::memcpy(bytes.data(),p,bytes.size());return true;
}
void GameConfiguration::apply(TitleSettings& s)const{
    std::memcpy(s.bindings.data(),bytes.data(),108);s.extra_lives=bytes[0xac];s.windowed=bytes[0xad];s.music_mode=bytes[0xae];s.effects=bytes[0xaf];s.difficulty=bytes[0xb0];s.screen_mode=bytes[0xb1];s.frameskip=bytes[0xb2];
    for(u32 n=0;n<2;++n){s.auto_focus[n]=bytes[0xb4+n];s.devices[n]=bytes[0xb7+n];}s.music_volume=bytes[0xba];s.sound_volume=bytes[0xbb];
}
bool GameConfiguration::capture(const TitleSettings& s){
    const auto previous=bytes;std::memcpy(bytes.data(),s.bindings.data(),108);bytes[0xac]=s.extra_lives;bytes[0xad]=s.windowed;bytes[0xae]=s.music_mode;bytes[0xaf]=s.effects;bytes[0xb0]=s.difficulty;bytes[0xb1]=s.screen_mode;bytes[0xb2]=s.frameskip;
    for(u32 n=0;n<2;++n){bytes[0xb4+n]=s.auto_focus[n];bytes[0xb7+n]=s.devices[n];}bytes[0xba]=s.music_volume;bytes[0xbb]=s.sound_volume;return previous!=bytes;
}
}
