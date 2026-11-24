#include "game/BulletUpdateHost.hpp"
#include <cassert>
#include <fstream>
#include <iterator>
#include <iostream>
#include <unordered_map>
#include <vector>

std::vector<unsigned char> read(const char* path){
    std::ifstream stream(path,std::ios::binary);assert(stream);
    return {(std::istreambuf_iterator<char>(stream)),{}};
}

int main(int argc,char** argv){
    using namespace th128::bullet;
    assert(argc==3);
    const auto profileBytes=read(argv[1]),animationBytes=read(argv[2]);
    Profile profile;BulletVisuals visuals;Manager manager;std::string error;
    assert(profile.load(profileBytes.data(),profileBytes.size(),error));
    assert(visuals.load(animationBytes.data(),animationBytes.size(),error));
    std::unordered_map<unsigned,BulletVisuals::Visual> animations;
    const auto configure=[&]{
        assert(manager.setConstructor([&](const Birth& birth,std::string& reason){
            BulletVisuals::CreateInfo info;info.type=8;info.color=10;info.script=43;info.birthInterrupt=birth.state.animationInterrupt;
            for(std::size_t index=0;index<4;++index)info.spriteRemap[index]=*profile.remap(10,int(index));
            auto visual=visuals.create(info,reason);if(!visual)return false;
            animations.emplace(birth.id,std::move(*visual));return true;
        }));
        assert(manager.setUpdater([&](Birth& birth,bool& removed,std::string& reason){
            auto found=animations.find(birth.id);assert(found!=animations.end());
            const auto success=updateBulletWithKnownMisses(birth,visuals,found->second,{0,400,0},2,removed,reason);
            if(removed)animations.erase(found);return success;
        }));
    };
    configure();
    auto emitter=resetEmitter();emitter.type=8;emitter.color=10;emitter.speed=1;emitter.endSpeed=1;
    emitter.positionX=-128;emitter.positionY=0;
    emitter.extension0.type=2;emitter.extension0.parameterA=1;emitter.extension0.control=1;
    assert(manager.emit(emitter,profile,{0,400,0},0,error));
    for(unsigned tick=0;tick<25;++tick){
        if(!manager.tick(error)){std::cerr<<"body tick "<<tick<<": "<<error<<'\n';return 2;}
    }
    const auto first=manager.snapshots().at(0);
    assert(first.age==25&&first.state.motionState==1&&first.state.position.y>12);
    assert(visuals.animationState(animations.at(first.id))->ticks==27);
    manager.reset();animations.clear();configure();
    emitter.positionX=.125f;emitter.positionY=359;
    assert(manager.emit(emitter,profile,{0,400,0},0,error));
    for(unsigned tick=0;tick<8;++tick)assert(manager.tick(error));
    assert(!manager.tick(error)&&error.find("graze")!=std::string::npos);
    const auto stopped=manager.snapshots().at(0);
    assert(stopped.age==8&&(stopped.state.flags&4U)==0);
    assert(visuals.animationState(animations.at(stopped.id))->ticks==10);
    manager.reset();animations.clear();configure();
    emitter.positionX=0;
    assert(manager.emit(emitter,profile,{0,400,0},0,error));
    for(unsigned tick=0;tick<8;++tick)assert(manager.tick(error));
    assert(!manager.tick(error)&&error.find("graze")!=std::string::npos);
    assert(manager.snapshots().at(0).age==8);
    manager.reset();animations.clear();
    unsigned grazeEvents=0;
    assert(manager.setConstructor([&](const Birth& birth,std::string& reason){
        BulletVisuals::CreateInfo info;info.type=8;info.color=10;info.script=43;info.birthInterrupt=8;
        for(std::size_t index=0;index<4;++index)info.spriteRemap[index]=*profile.remap(10,int(index));
        auto visual=visuals.create(info,reason);if(!visual)return false;
        animations.emplace(birth.id,std::move(*visual));return true;
    }));
    assert(manager.setUpdater([&](Birth& birth,bool& removed,std::string& reason){
        return updateBulletWithKnownMisses(birth,visuals,animations.at(birth.id),{0,400,0},2,removed,reason,
            [&grazeEvents](const Birth&,std::string&){++grazeEvents;return true;});
    }));
    emitter.positionX=.125f;emitter.positionY=359;
    assert(manager.emit(emitter,profile,{0,400,0},0,error));
    for(unsigned tick=0;tick<12;++tick)assert(manager.tick(error));
    assert(grazeEvents==1&&(manager.snapshots().at(0).state.flags&4U));
    manager.reset();animations.clear();
    unsigned hits=0,cancellations=0;
    visuals.setRandomSources([]{return 0x80000000U;},{});
    assert(manager.setConstructor([&](const Birth& birth,std::string& reason){
        BulletVisuals::CreateInfo info;info.type=8;info.color=10;info.script=43;info.birthInterrupt=8;
        for(std::size_t index=0;index<4;++index)info.spriteRemap[index]=*profile.remap(10,int(index));
        auto visual=visuals.create(info,reason);if(!visual)return false;
        animations.emplace(birth.id,std::move(*visual));return true;
    }));
    assert(manager.setUpdater([&](Birth& birth,bool& removed,std::string& reason){
        return updateBulletWithKnownMisses(birth,visuals,animations.at(birth.id),{0,400,0},2,removed,reason,{},
            [&](const Birth&,std::string&)->std::optional<int>{++hits;return 1;},
            [&](const Birth& hit,bool full,std::string&){
                ++cancellations;assert(!full&&hit.state.motionState==3&&hit.cancellationScript==357);
                assert(animations.at(hit.id).primary.diagnostics().pendingInterrupt==1);return true;
            });
    }));
    emitter.positionX=.125f;emitter.positionY=399;
    assert(manager.emit(emitter,profile,{0,400,0},0,error));
    for(unsigned tick=0;tick<10;++tick)assert(manager.tick(error));
    assert(hits==1&&cancellations==1&&manager.snapshots().at(0).state.motionState==3);
    struct ContactResult {bool success;Birth bullet;int ticks;int pending;std::string error;int sprite;int clock;bool held;bool ended;};
    const auto contactCase=[&](bool full,HitCallback hit,CancellationCallback cancellation){
        manager.reset();animations.clear();
        assert(manager.setConstructor([&](const Birth& birth,std::string& reason){
            BulletVisuals::CreateInfo info;info.type=8;info.color=10;info.script=43;info.birthInterrupt=birth.state.animationInterrupt;
            for(std::size_t index=0;index<4;++index)info.spriteRemap[index]=*profile.remap(10,int(index));
            auto visual=visuals.create(info,reason);if(!visual)return false;animations.emplace(birth.id,std::move(*visual));return true;
        }));
        assert(manager.setUpdater([&,full,hit=std::move(hit),cancellation=std::move(cancellation)](Birth& birth,bool& removed,std::string& reason){
            if(birth.age==(full?0:8)){birth.grace=3;birth.cooldown=2;}
            return updateBulletWithKnownMisses(birth,visuals,animations.at(birth.id),{0,400,0},2,removed,reason,{},hit,cancellation);
        }));
        emitter.extension0.type=full?0:2;emitter.positionX=.125f;emitter.positionY=399;
        assert(manager.emit(emitter,profile,{0,400,0},0,error));
        bool success=true;
        for(unsigned step=0;step<(full?1U:9U);++step)if(!(success=manager.tick(error)))break;
        const auto bullet=manager.snapshots().at(0);
        const auto* state=visuals.animationState(animations.at(bullet.id));
        return ContactResult{success,bullet,state->ticks,
            animations.at(bullet.id).primary.diagnostics().pendingInterrupt,error,state->sprite,state->clock,state->held,state->ended};
    };
    unsigned unexpectedCancellation=0;
    const auto noCancellation=[&](const Birth&,bool,std::string&){++unexpectedCancellation;return true;};
    const auto suppressedHalf=contactCase(false,[](const Birth&,std::string&)->std::optional<int>{return 0;},noCancellation);
    assert(suppressedHalf.success&&suppressedHalf.bullet.age==9&&suppressedHalf.bullet.state.motionState==2
        &&suppressedHalf.ticks==11&&suppressedHalf.pending==0&&suppressedHalf.bullet.grace==2&&suppressedHalf.bullet.cooldown==1);
    assert(suppressedHalf.sprite==328&&suppressedHalf.clock==18&&!suppressedHalf.held&&!suppressedHalf.ended);
    const auto suppressedFull=contactCase(true,[](const Birth&,std::string&)->std::optional<int>{return 0;},noCancellation);
    assert(suppressedFull.success&&suppressedFull.bullet.age==1&&suppressedFull.ticks==3&&suppressedFull.pending==0);
    assert(suppressedFull.sprite==138&&suppressedFull.held&&suppressedFull.clock==48&&!suppressedFull.ended);
    assert(unexpectedCancellation==0);
    for(const auto invalidReturn: {2, -1}){
        const auto invalid=contactCase(false,[invalidReturn](const Birth&,std::string&)->std::optional<int>{return invalidReturn;},noCancellation);
        assert(!invalid.success&&invalid.bullet.age==8&&invalid.bullet.state.motionState==2&&invalid.ticks==10&&invalid.pending==0
            &&invalid.bullet.lifeClock==9&&invalid.bullet.grace==3&&invalid.bullet.cooldown==2);
    }
    const auto incomplete=contactCase(false,[](const Birth&,std::string&)->std::optional<int>{return std::nullopt;},noCancellation);
    assert(!incomplete.success&&incomplete.bullet.age==8&&incomplete.ticks==10&&incomplete.pending==0);
    const auto accepted=[](const Birth&,std::string&)->std::optional<int>{return 1;};
    const auto missing=contactCase(false,accepted,{});
    assert(!missing.success&&missing.bullet.state.motionState==3&&missing.pending==1&&missing.ticks==10
        &&missing.bullet.lifeClock==9&&missing.bullet.grace==3&&missing.bullet.cooldown==2);
    const auto rejected=contactCase(false,accepted,[](const Birth& birth,bool full,std::string& reason){
        assert(birth.state.motionState==3&&!full);reason="required cancellation rejected";return false;
    });
    assert(!rejected.success&&rejected.bullet.age==8&&rejected.ticks==10&&rejected.pending==1&&rejected.bullet.grace==3);
    unsigned fullCalls=0;
    const auto completedFull=contactCase(true,accepted,[&](const Birth& birth,bool full,std::string&){
        assert(full&&birth.state.motionState==3&&birth.cancellationScript==357);++fullCalls;return true;
    });
    assert(completedFull.success&&fullCalls==1&&completedFull.bullet.age==1&&completedFull.bullet.lifeClock==1
        &&completedFull.ticks==3&&completedFull.pending==0&&completedFull.bullet.grace==2&&completedFull.bullet.cooldown==1
        &&completedFull.bullet.state.position.x==suppressedFull.bullet.state.position.x
        &&completedFull.bullet.state.position.y==suppressedFull.bullet.state.position.y);
    manager.reset();animations.clear();configure();unsigned extensionSounds=0;
    emitter.positionX=.125f;emitter.positionY=100;emitter.extension0.type=2;emitter.extension0.parameterA=1;
    ExtensionRaw extension;extension.type=16;extension.parameterA=2;extension.parameterB=1;extension.parameterC=2;extension.parameterS=-999;
    assert(setExtendedExtension(emitter,1,extension));
    assert(manager.setUpdater([&](Birth& birth,bool& removed,std::string& reason){
        return updateBulletWithKnownMisses(birth,visuals,animations.at(birth.id),{0,400,0},2,removed,reason,{},{},{},
            [&](int identifier,std::string&){assert(identifier==40);++extensionSounds;return true;});
    }));
    assert(manager.emit(emitter,profile,{0,400,0},0,error));
    bool activeExtensionObserved=false;
    for(int tick=0;tick<30;++tick){
        if(!manager.tick(error)){std::cerr<<"extension host tick="<<tick<<": "<<error<<'\n';return 6;}
        const auto state=manager.snapshots().at(0);
        if(state.extension16.active){activeExtensionObserved=true;assert(state.state.activeExtensions==16);}
    }
    assert(activeExtensionObserved&&extensionSounds==1&&manager.snapshots()[0].extension16.count==1
        &&!manager.snapshots()[0].extension16.active&&manager.snapshots()[0].state.activeExtensions==0
        &&manager.snapshots()[0].state.extensionIndex==2&&manager.snapshots()[0].lifeClock==30);
    manager.reset();animations.clear();configure();unsigned rejectedSounds=0;
    assert(manager.setUpdater([&](Birth& birth,bool& removed,std::string& reason){
        return updateBulletWithKnownMisses(birth,visuals,animations.at(birth.id),{0,400,0},2,removed,reason,{},{},{},
            [&](int,std::string& failure){++rejectedSounds;failure="required extension sound rejected";return false;});
    }));
    assert(manager.emit(emitter,profile,{0,400,0},0,error));
    for(int tick=0;tick<30;++tick)if(!manager.tick(error))break;
    assert(error=="required extension sound rejected"&&rejectedSounds==1);
    const auto retainedClock=manager.snapshots()[0].lifeClock;
    assert(!manager.tick(error)&&rejectedSounds==1&&manager.snapshots()[0].lifeClock==retainedClock);
    Birth reentered=manager.snapshots()[0];reentered.extensionFailure.clear();
    reentered.state.motionState=1;reentered.state.flags=0x11;reentered.state.activeExtensions=16;
    reentered.extension16.active=true;reentered.extension16.time=reentered.extension16.period;reentered.extension16.fraction=float(reentered.extension16.time);
    auto& reenteredVisual=animations.at(reentered.id);unsigned recursiveSounds=0;bool removed=false;
    ExtensionSound recursive;
    recursive=[&](int,std::string& reason){
        ++recursiveSounds;
        if(recursiveSounds<2)updateBulletWithKnownMisses(reentered,visuals,reenteredVisual,{0,400,0},2,removed,reason,{},{},{},recursive);
        reason.clear();return true;
    };
    assert(!updateBulletWithKnownMisses(reentered,visuals,reenteredVisual,{0,400,0},2,removed,error,{},{},{},recursive));
    assert(recursiveSounds==1&&error.find("reenter")!=std::string::npos);
    manager.reset();animations.clear();configure();unsigned type4Sounds=0;
    extension.type=4;extension.parameterR=.0166666675f;extension.parameterS=-999;extension.parameterA=60;
    assert(setExtendedExtension(emitter,1,extension));
    assert(manager.setUpdater([&](Birth& birth,bool& retired,std::string& reason){
        return updateBulletWithKnownMisses(birth,visuals,animations.at(birth.id),{0,400,0},2,retired,reason,{},{},{},
            [&](int identifier,std::string&){assert(identifier==40&&birth.state.extensionIndex==1&&birth.state.activeExtensions==4);
                ++type4Sounds;return true;});
    }));
    assert(manager.emit(emitter,profile,{0,400,0},0,error));
    for(int tick=0;tick<30;++tick)assert(manager.tick(error));
    assert(type4Sounds==1&&manager.snapshots()[0].extension4.active&&manager.snapshots()[0].state.extensionIndex==2);
    return 0;
}
