#include "BulletTemplates.hpp"
namespace th08 {
// Original TH08 1.00d table at 004b4ad8: main, three spawn sizes, cancel.
static constexpr i32 scripts[21][5]{
    {0,18,19,20,15},{1,21,22,23,16},{2,21,22,23,16},{3,21,22,23,16},
    {4,21,22,23,16},{5,21,22,23,16},{6,21,22,23,16},{7,24,24,24,17},
    {8,24,24,24,17},{9,24,24,24,17},{25,27,27,27,26},{106,21,22,23,16},
    {107,21,22,23,16},{108,21,22,23,16},{109,24,24,24,17},{110,24,24,24,17},
    {111,21,22,23,16},{112,21,22,23,16},{113,24,24,24,17},{114,24,24,24,17},{115,24,24,24,17}
};
bool BulletTemplates::load(AnmLoaded& file,Rng& random,const FrameTiming& timing){
    AnmExecutor executor(random);executor.timing=timing;
    for(u32 index=0;index<21;++index){
        auto& type=types[index];
        for(u32 slot=0;slot<5;++slot){
            const i32 script=scripts[index][slot];if(u32(script)>=file.scriptCount)return false;
            auto& vm=type.animation[slot];vm.anmFile=&file;vm.scriptIndex=i16(script);executor.start(file,vm,file.scripts[script]);if(executor.invalid)return false;
        }
        for(auto& vm:type.animation)vm.zWriteDisabled=true;
        auto& main=type.animation[0];main.baseSpriteIndex=main.activeSpriteIndex;if(!main.loadedSprite)return false;
        const float height=main.loadedSprite->heightPx;type.height=u8(Scalar::truncate(height));const i32 script=scripts[index][0];
        float hitbox=24;u8 layer=0;
        if(height<=8){hitbox=4;layer=5;}
        else if(height<=16){
            switch(script){
            case 2:case 4:case 5:case 6:case 106:case 107:case 108:case 111:case 112:hitbox=4;layer=4;break;
            default:hitbox=6;layer=3;break;
            }
        }else if(height<=32){
            switch(script){
            case 8:case 113:case 114:case 115:hitbox=5;layer=2;break;
            case 9:case 109:case 110:hitbox=8;layer=1;break;
            default:hitbox=10;layer=1;break;
            }
        }
        type.hitbox.x=type.hitbox.y=hitbox;type.layer=layer;
    }
    return true;
}
}
