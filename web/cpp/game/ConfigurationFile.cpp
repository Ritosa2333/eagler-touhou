#include "ConfigurationFile.hpp"
namespace th08 {
bool load_configuration(GameConfiguration& output,const u8* bytes,u32 size,const u8* wave,u32 wave_size){
    if(wave){u32 header[4]{};if(wave_size<sizeof(header))return false;std::memcpy(header,wave,sizeof(header));if(header[0]!=fourcc('Z','W','A','V')||header[1]!=1||header[2]!=0x800)return false;}
    GameConfiguration value;std::memset(&value,0,sizeof(value));if(bytes&&size>=sizeof(value))std::memcpy(&value,bytes,sizeof(value));
    if(!bytes||size!=sizeof(value)||value.version!=0x80001||value.lives>=7||value.bombs>=4||value.color16>=2||value.music>=3||value.difficulty>=6||value.sounds>=2||value.windowed>=2||value.frameskip>=3||value.effects>=3||value.slow_mode>=2||value.shot_slow>=2){
        std::memset(&value,0,sizeof(value));constexpr i16 controller[]{0,1,2,4,-1,-1,-1,-1,3};std::memcpy(value.controller,controller,sizeof(controller));
        value.version=0x80001;value.pad_x=value.pad_y=600;value.lives=2;value.bombs=3;value.music=wave?1:2;value.sounds=1;value.difficulty=1;value.effects=2;value.music_volume=100;value.sound_volume=80;
    }
    value.options=(value.options|1u)&~128u;output=value;return true;
}
}
