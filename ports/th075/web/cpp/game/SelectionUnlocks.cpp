#include "CharacterSelectScene.hpp"
namespace th075 {
u8 SelectionUnlocks::character_available(i8 character,bool story)const{
    u8 value=1;const auto f=[&](u32 index){return storyFlags[index];};
    if(!story){switch(character){
        case 6:value=f(0)|f(20)|f(40)|f(1)|f(21)|f(41)|f(5)|f(25)|f(45);break;
        case 7:value=f(2)|f(22)|f(42)|f(4)|f(24)|f(44);break;
        case 8:value=0;for(u32 i=0;i<6;i++)value|=f(20+i)|f(40+i);break;
        case 9:case 10:value=0;for(u32 i=0;i<6;i++)value|=f(40+i);break;
    }}else{switch(character){
        case 6:case 7:for(u32 i=0;i<6;i++)value&=f(40+i);break;
        case 8:value=f(46)&f(47);break;case 9:value=f(48);break;case 10:value=0;break;
    }}return value;
}
u8 SelectionUnlocks::background_available(i8 background)const{return i32(background)%10<6?1:storyFlags[u8(background)+200];}
}
