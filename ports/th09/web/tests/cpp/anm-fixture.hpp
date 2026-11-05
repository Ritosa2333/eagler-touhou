#pragma once
#include "../../cpp/game/AnmExecutor.hpp"
#include "../../cpp/game/AnmResource.hpp"
#include <vector>
namespace anm_test {
using namespace th09;
struct Fixture {
    Rng rng;AnmExecutor executor{rng};AnmVm vm;AnmLoaded file;
    AnmLoadedSprite sprites[16]{};std::vector<u8> script;
    AnmResource resource;
    Fixture(){file.anmIdx=7;file.rawData=sprites;file.sprites=sprites;file.spriteCount=16;
        for(u32 i=0;i<16;++i){auto& s=sprites[i];s.anmIdx=7;s.texture=i+1;s.width=256;s.height=128;s.widthPx=16+i*3;s.heightPx=24+i*5;s.scaleFactor={1,1};s.uvEnd={0.75f,0.5f};}
    }
    void start(const u8* bytes,u32 size,float rate,u32 flags){script.assign(bytes,bytes+size);vm=AnmVm();rng={0x1234,0,0};executor.executed=0;executor.invalid=false;executor.timing={rate,bool(flags&32)};executor.start(file,vm,reinterpret_cast<AnmRawInstr*>(script.data()));}
    bool load(const u8* bytes,u32 size){if(!resource.load(7,bytes,size))return false;file=resource.view();return true;}
    bool start_index(u32 index,float rate){if(index>=resource.script_count())return false;vm=AnmVm();rng={0x1234,0,0};executor.executed=0;executor.invalid=false;executor.timing={rate,false};executor.start(file,vm,file.scripts[index]);return true;}
};
}
