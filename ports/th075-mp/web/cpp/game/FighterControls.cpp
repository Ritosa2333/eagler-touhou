#include "FighterControls.hpp"
#include <algorithm>
namespace th075 {
static i32 held_axis(i32 previous,bool negative,bool positive){
    if(negative){if(previous>0)previous=0;return increment_wrapped(previous,-1);}
    if(positive){if(previous<0)previous=0;return increment_wrapped(previous,1);}return 0;
}
void ControlCounts::from_held(u8 value){
    horizontal=held_axis(horizontal,value&1,value&2);vertical=held_axis(vertical,value&4,value&8);
    for(u32 i=0;i<4;i++)buttons[i]=value&(16u<<i)?increment_wrapped(buttons[i],1):0;
}
u8 ControlCounts::held()const{
    u8 value=u8((horizontal<0)|((horizontal>0)<<1)|((vertical<0)<<2)|((vertical>0)<<3));
    for(u32 i=0;i<4;i++)if(buttons[i]!=0)value|=u8(16u<<i);return value;
}
u8 ControlCounts::edges(i8 facing)const{
    const i32 x=facing==1?horizontal:(horizontal==std::numeric_limits<i32>::min()?horizontal:-horizontal);
    u8 value=u8((x<0)|((x>0)<<1)|((vertical<0)<<2)|((vertical>0)<<3));
    for(u32 i=0;i<4;i++)if(buttons[i]==1)value|=u8(16u<<i);return value;
}
void InputHistory::push(u8 value){std::move(frames.begin()+1,frames.end(),frames.begin());frames.back()=value;}
static bool command_token(char token,i8 facing,u8& value){
    switch(token){
        case '1':value=9;break;case '2':value=8;break;case '3':value=10;break;
        case '4':value=1;break;case '5':case 'N':value=0;break;case '6':value=2;break;
        case '7':value=5;break;case '8':value=4;break;case '9':value=6;break;
        case 'A':value=16;break;case 'B':value=32;break;case 'C':value=64;break;case 'D':value=128;break;
        case 'L':value=facing>0?1:2;break;case 'R':value=facing>0?2:1;break;case 'X':value=240;break;
        default:return false;
    }return true;
}
u8 InputHistory::match(const char* pattern,u32 length,u32 window,i8 facing)const{
    if(!pattern||!length||length>20||!window||window>frames.size())return 0;
    std::array<u8,20> sequence{};for(u32 i=0;i<length;i++)if(!command_token(pattern[i],facing,sequence[i]))return 0;
    u32 index=0;for(u32 i=frames.size()-window;i<frames.size();i++){
        const u8 value=sequence[index],buttons=frames[i]&0xf0;
        const bool matched=value<16?(frames[i]&15)==value:value==240?buttons!=0:buttons==value;
        if(matched&&++index==length){if(value!=240)return 1;for(u8 button=0;button<4;button++)if(buttons&(16u<<button))return button+1;}
    }return 0;
}
u8 FighterControls::player_frame(const ControlCounts& input,i8 facing,i16 action){
    raw=input;current=raw;
    bool pressed=false;for(auto b:current.buttons)pressed|=b==1;
    if(action>299&&pressed){bufferTimer=10;buffered.horizontal=current.horizontal;buffered.vertical=current.vertical;for(u32 i=0;i<4;i++)buffered.buttons[i]=current.buttons[i]==1;}
    if(action<300||bufferTimer<1){buffered.clear();bufferTimer=0;}
    if(bufferTimer>0)--bufferTimer;
    history.push(current.edges(facing));return raw.held();
}
}
