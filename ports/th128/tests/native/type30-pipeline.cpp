#include "game/BulletManager.hpp"
#include "game/BulletVisuals.hpp"
#include "game/BulletUpdateHost.hpp"
#include "game/BulletCollisionRules.hpp"
#include "game/CancellationEffects.hpp"
#include "game/IceContact.hpp"
#include "engine/Random.hpp"
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace th128::bullet;

static std::vector<unsigned char> read(const char* path) {
    std::ifstream input(path,std::ios::binary);
    if(!input)throw std::runtime_error(std::string("missing ")+path);
    return {std::istreambuf_iterator<char>(input),{}};
}
static void require(bool condition,const std::string& message) {
    if(!condition)throw std::runtime_error(message);
}
static std::uint32_t wordAt(const std::vector<unsigned char>& data,std::size_t offset) {
    return std::uint32_t(data[offset])|(std::uint32_t(data[offset+1])<<8)
        |(std::uint32_t(data[offset+2])<<16)|(std::uint32_t(data[offset+3])<<24);
}
int main(int argc,char** argv) {
    try {
        require(argc==5,"usage: type30 profile.bin bullet.anm native-jump-table.bin native-dispatch-code.bin");
        const auto profileBytes=read(argv[1]),animation=read(argv[2]);
        const auto jumpTable=read(argv[3]),dispatchCode=read(argv[4]);
        require(profileBytes.size()==288,"type30 profile length");
        require(jumpTable.size()==40&&dispatchCode.size()==185,"native fixture length");
        require(wordAt(profileBytes,16)==373u&&wordAt(profileBytes,0x110)==9u,
            "type30 raw row0 373 and mode9 unchanged");
        require(wordAt(jumpTable,24)==0x409846u&&wordAt(jumpTable,32)==0x409873u
            &&wordAt(jumpTable,36)==0x40987fu,"native jump table entry6/8/9");
        require(dispatchCode[0xa3]==0xc7u&&dispatchCode[0xa4]==0x83u
            &&wordAt(dispatchCode,0xa5)==0x0a1cu&&wordAt(dispatchCode,0xa9)==0x173u,
            "native entry8 writes371 to +a1c");
        require(dispatchCode[0xaf]==0xc7u&&dispatchCode[0xb0]==0x83u
            &&wordAt(dispatchCode,0xb1)==0x0a1cu&&wordAt(dispatchCode,0xb5)==0x17du,
            "native entry9 writes381 to +a1c");

        std::string error;
        Profile profile;
        require(profile.loadTyped(30,profileBytes.data(),profileBytes.size(),error),error);
        require(profile.script==285&&profile.secondaryScript==286,"type30 script/secondary");
        require(profile.tail.hitboxSize==4.f&&profile.tail.auxiliarySize==22.f
            &&profile.tail.value==2&&profile.tail.freezePolicy==1,"type30 geometry/freeze");
        require(profile.identitySpriteRemap(),"type30 identity remap");
        require(profile.cancellationScript(6)==381&&profile.cancellationScript(0)==381,
            "type30 color6/color0 cached cancellation381");
        {
            auto corrupt=profileBytes;corrupt[0]=42;
            require(!profile.loadTyped(30,corrupt.data(),corrupt.size(),error),"type30 wrong script");
            corrupt=profileBytes;corrupt[0x11c]=0;
            require(!profile.loadTyped(30,corrupt.data(),corrupt.size(),error),"type30 wrong secondary");
            corrupt=profileBytes;corrupt[0x110]=6;
            require(!profile.loadTyped(30,corrupt.data(),corrupt.size(),error),"type30 wrong cancellation mode");
            corrupt=profileBytes;corrupt[0x104]=corrupt[0x105]=corrupt[0x106]=corrupt[0x107]=0;
            require(!profile.loadTyped(30,corrupt.data(),corrupt.size(),error),"type30 wrong hitbox");
            require(!profile.loadTyped(29,profileBytes.data(),profileBytes.size(),error),"type30 type mismatch");
        }

        BulletVisuals visuals;
        require(visuals.load(animation.data(),animation.size(),error),error);
        th128::Random random;random.seed=0x1234;
        visuals.setRandomSources([&]{return random.next32();},[&]{return random.next32();});
        std::unordered_map<std::uint32_t,BulletVisuals::Visual> animations;
        std::uint32_t constructorCalls=0,updateCalls=0,lifetimeColor0=0,lifetimeColor6=0;
        Manager manager;
        require(manager.setConstructor([&](const Birth& birth,std::string& constructorError) {
            ++constructorCalls;
            if(birth.type!=30||birth.script!=285||birth.cancellationScript!=381) {
                constructorError="manager birth profile mismatch";return false;
            }
            BulletVisuals::CreateInfo info;
            info.type=30;info.color=birth.color;info.script=birth.script;
            info.secondaryScript=286;info.identitySpriteRemap=true;
            info.spriteRemap={0,0,0,0};
            info.birthInterrupt=birth.state.animationInterrupt;
            auto visual=visuals.create(info,constructorError);
            if(!visual)return false;
            if(!animations.emplace(birth.id,std::move(*visual)).second) {
                constructorError="duplicate type30 visual identity";return false;
            }
            return true;
        }),"type30 manager constructor");
        require(manager.setUpdater([&](Birth& birth,bool& removed,std::string& updateError) {
            ++updateCalls;
            auto visual=animations.find(birth.id);
            if(visual==animations.end()){updateError="missing manager-owned visual";return false;}
            if(!updateBulletWithKnownMisses(birth,visuals,visual->second,{0,1000,0},0,removed,updateError))
                return false;
            if(birth.color==0)lifetimeColor0=birth.lifeClock;
            else if(birth.color==6)lifetimeColor6=birth.lifeClock;
            return true;
        }),"type30 manager updater");
        auto emitter=resetEmitter();
        emitter.type=30;emitter.color=6;emitter.count=1;emitter.aimMode=2;
        emitter.speed=emitter.endSpeed=1.5f;
        require(manager.emit(emitter,profile,{0,400,0},0,error),error);
        emitter.color=0;
        require(manager.emit(emitter,profile,{0,400,0},0,error),error);
        require(manager.activeCount()==2&&constructorCalls==2,"type30 manager births color6/color0");
        require(manager.tick(error),error);
        require(updateCalls==2&&lifetimeColor0==1&&lifetimeColor6==1,
            "canonical manager tick must update both manager-owned births through real visuals");
        const auto births=manager.snapshots();
        require(births.size()==2&&births[0].cancellationScript==381&&births[1].cancellationScript==381,
            "manager snapshots retain cached381 for both colors");
        std::size_t color0Slot=0;
        std::uint32_t color0Id=0;
        for(const auto& birth:births)if(birth.color==0){color0Slot=birth.slot;color0Id=birth.id;}
        require(manager.release(color0Slot),"release color0 manager birth");
        animations.erase(color0Id);
        require(manager.activeCount()==1,"one canonical color6 birth retained");

        th128::effects::CancellationEffects effects;
        require(effects.load(animation.data(),animation.size(),error),error);
        int randomCalls=0;
        require(effects.setGameRandom([&]{++randomCalls;return 0x12345678u;}),"type30 cancellation RNG");
        require(manager.cancelAll([&](Birth& birth,std::string& cancelError) {
            require(birth.cancellationScript==381,"manager cancellation must use cached381");
            if(!effects.spawn(birth.cancellationScript,birth.state.position.x,birth.state.position.y,
                              birth.state.position.z,9,cancelError,true))
                return false;
            return true;
        },error),error);
        require(effects.rootCount()==1&&effects.nodeCount()==4,"type30 canonical cancellation381/380 family");
        bool root381=false;
        std::size_t child380=0;
        for(const auto& node:effects.snapshots()) {
            if(node.state.script==381&&node.parent==0)root381=true;
            if(node.state.script==380)++child380;
        }
        require(root381&&child380==3,"type30 exact root381/three380");
        int retirement=0;
        for(;retirement<80;++retirement) {
            require(effects.tick(error),error);
            if(effects.rootCount()==0)break;
        }
        require(retirement<80&&effects.rootCount()==0&&effects.nodeCount()==0,
            "type30 cancellation authored retirement");
        require(animations.size()==1&&!animations.begin()->second.faulted(),
            "canonical type30 visual remains healthy");
        std::printf("PASS type30 canonical production Manager/constructor/visual/UpdateHost/cancel381 rng=%d retirement=%d\n",
            randomCalls,retirement+1);
    }catch(const std::exception& error) {
        std::fprintf(stderr,"%s\n",error.what());return 1;
    }
}
