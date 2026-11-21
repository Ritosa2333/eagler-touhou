#include "game/BulletManager.hpp"
#include "game/BulletTickRules.hpp"
#include "game/BulletVisuals.hpp"
#include "game/CancellationEffects.hpp"
#include "engine/Random.hpp"
#include <bit>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>

std::vector<unsigned char> read(const char* path){std::ifstream file(path,std::ios::binary);assert(file);return {(std::istreambuf_iterator<char>(file)),{}};}
float readFloat(){std::uint32_t bits=0;std::cin>>bits;return std::bit_cast<float>(bits);}
void scalar(float value){std::cout<<' '<<std::bit_cast<std::uint32_t>(value);}
int main(int argc,char** argv){
    assert(argc==3||argc==4);using namespace th128::bullet;
    const int selectedType=argc==4?20:18,script=selectedType==20?74:72,color=selectedType==20?6:3,cancelScript=selectedType==20?381:375;
    const int displaySprite=selectedType==20?329:326,profileValue=selectedType==20?2:1;
    const float hitbox=selectedType==20?7.f:8.5f;
    const auto bytes=read(argv[1]),animation=read(argv[2]);std::string error;
    Profile profile;assert(profile.loadTyped(selectedType,bytes.data(),bytes.size(),error));
    assert(profile.type==selectedType&&profile.script==script&&profile.tail.type==selectedType&&profile.tail.hitboxSize==hitbox&&profile.tail.auxiliarySize==22&&profile.tail.value==profileValue);
    assert(profile.tail.freezePolicy==0);
    assert(profile.remap(color,1)==displaySprite&&profile.cancellationScript(color)==cancelScript);
    assert(!profile.loadTyped(19,bytes.data(),bytes.size(),error)&&profile.type==selectedType&&profile.script==script);
    assert(!profile.loadTyped(selectedType,bytes.data(),bytes.size()-1,error)&&profile.type==selectedType);
    auto corrupt=bytes;corrupt[0]=43;
    assert(!profile.loadTyped(selectedType,corrupt.data(),corrupt.size(),error)&&profile.script==script);
    Manager manager;unsigned births=0;
    assert(manager.setConstructor([&](const Birth& birth,std::string&){++births;assert(birth.type==selectedType&&birth.script==script&&birth.cancellationScript==cancelScript);return true;}));
    auto emitter=resetEmitter();emitter.type=selectedType;emitter.color=color;emitter.count=8;emitter.aimMode=2;
    emitter.speed=emitter.endSpeed=1.5;emitter.extension0.type=2;emitter.extension0.parameterA=1;
    assert(manager.emit(emitter,profile,{0,400,0},0,error)&&births==8&&manager.activeCount()==8);
    emitter.type=8;assert(!manager.emit(emitter,profile,{0,400,0},0,error)&&births==8);
    BulletVisuals visuals;assert(visuals.load(animation.data(),animation.size(),error));
    th128::anm::Registry referenceBank;assert(referenceBank.loadBank(0,animation.data(),animation.size(),error));
    for(const int interrupt:{0,8}){
        BulletVisuals::CreateInfo info;info.type=selectedType;info.color=color;info.script=script;info.birthInterrupt=interrupt;
        for(int index=0;index<4;++index)info.spriteRemap[std::size_t(index)]=*profile.remap(color,index);
        auto visual=visuals.create(info,error);assert(visual&&visual->primary.state().sprite==displaySprite);
        th128::anm::VM reference;th128::anm::Runtime runtime;
        runtime.remapSprite=[&](int index){return info.spriteRemap.at(std::size_t(index));};
        assert(reference.bind(referenceBank,0,script,runtime));if(interrupt)reference.interrupt(interrupt);reference.tick(runtime);
        for(int tick=0;tick<90;++tick){
            const auto* actual=visuals.animationState(*visual);assert(actual);
            const auto& expected=reference.state();
            assert(actual->sprite==expected.sprite&&actual->integers==expected.integers&&actual->clock==expected.clock&&actual->ticks==expected.ticks
                &&actual->sx==expected.sx&&actual->sy==expected.sy&&actual->primary==expected.primary&&actual->flags==expected.flags
                &&actual->held==expected.held&&actual->ended==expected.ended&&actual->deleteRequested==expected.deleteRequested);
            assert(visuals.advance(*visual,1,error));reference.tick(runtime);
        }
    }
    th128::effects::CancellationEffects effects;assert(effects.load(animation.data(),animation.size(),error));
    th128::Random effectRandom,referenceRandom;effectRandom.seed=referenceRandom.seed=0x1234;
    assert(effects.setGameRandom([&]{return effectRandom.next32();}));
    const auto effect=effects.spawn(cancelScript,320,416,.1f,23,error);
    if(!effect||effects.nodeCount()!=1){std::cerr<<"type18 effect: "<<error<<" nodes="<<effects.nodeCount()<<'\n';return 7;}
    th128::anm::Scene scene;assert(scene.registry.loadBank(0,animation.data(),animation.size(),error));
    scene.runtime.gameRandom32=[&]{return referenceRandom.next32();};
    scene.runtime.unsupported=[&](int,int,int,const char* reason){error=reason;};
    assert(scene.bind(1,1,0,cancelScript,th128::anm::Membership::Primary,320,416,{},false,nullptr,0,0,23)&&error.empty());
    bool childrenObserved=false;
    for(int tick=0;tick<70;++tick){
        const auto snapshots=effects.snapshots();assert(snapshots.size()==scene.nodes().size()&&effectRandom.calls==referenceRandom.calls);
        if(snapshots.size()>1)childrenObserved=true;
        for(const auto& snapshot:snapshots){const auto found=scene.nodes().find(snapshot.node);assert(found!=scene.nodes().end());const auto* node=found->second.get();
            assert(snapshot.state.script==node->vm.state().script&&snapshot.state.sprite==node->vm.state().sprite
                &&snapshot.state.clock==node->vm.state().clock&&snapshot.state.primary==node->vm.state().primary
                &&snapshot.state.x==node->vm.state().x&&snapshot.state.y==node->vm.state().y);}
        assert(effects.tickPrimary(error));scene.tickPrimary();scene.collect();assert(error.empty());
    }
    assert(effects.nodeCount()==0&&scene.nodes().empty()&&childrenObserved);
    const auto moving=effects.spawn(cancelScript,320,416,.1f,23,error);assert(moving);
    assert(effects.initializeNativeDrift(*moving,{.5f,1.5f,0},1,{0,0,0},error));
    assert(!effects.initializeNativeDrift(*moving,{.5f,1.5f,0},1,{0,0,0},error));
    for(int tick=0;tick<30;++tick)assert(effects.tickPrimary(error));
    const auto moved=effects.snapshots();const auto movingRoot=std::find_if(moved.begin(),moved.end(),[&](const auto& snapshot){return snapshot.handle==*moving&&snapshot.parent==0;});
    assert(movingRoot!=moved.end()&&movingRoot->world.x==325&&movingRoot->world.y==431);
    assert(effects.remove(*moving));
    if(selectedType==18)for(int rootScript=337;rootScript<=367;rootScript+=2){
        th128::effects::CancellationEffects colored;assert(colored.load(animation.data(),animation.size(),error));
        th128::Random coloredRandom,directRandom;coloredRandom.seed=directRandom.seed=0x4321;
        colored.setGameRandom([&]{return coloredRandom.next32();});
        const auto root=colored.spawn(rootScript,320,416,.1f,23,error);
        if(!root){std::cerr<<"required colored cancellation "<<rootScript<<": "<<error<<'\n';return 8;}
        assert(colored.initializeNativeDrift(*root,{.5f,1.5f,0},1,{0,0,0},error));
        th128::anm::Scene direct;assert(direct.registry.loadBank(0,animation.data(),animation.size(),error));
        direct.runtime.gameRandom32=[&]{return directRandom.next32();};
        assert(direct.bind(1,1,0,rootScript,th128::anm::Membership::Primary,320,416,{},false,nullptr,0,0,23));
        direct.find(1)->vm.interpolatePosition(5,15,0,30,6);
        bool childSeen=false;
        for(int frame=0;frame<65;++frame){
            const auto nodes=colored.snapshots();assert(nodes.size()==direct.nodes().size()&&coloredRandom.calls==directRandom.calls);
            for(const auto& snapshot:nodes){const auto found=direct.nodes().find(snapshot.node);assert(found!=direct.nodes().end());
                const auto& state=found->second->vm.state();
                assert(snapshot.state.script==state.script&&snapshot.state.primary==state.primary&&snapshot.state.x==state.x&&snapshot.state.y==state.y);
                if(snapshot.parent){assert(snapshot.state.script==rootScript-1);childSeen=true;}}
            assert(colored.tickPrimary(error));direct.tickPrimary();direct.collect();
        }
        assert(childSeen&&colored.nodeCount()==0&&direct.nodes().empty());
    }
    char command=0;
    while(std::cin>>command){
        if(command=='S'){
            int color=0,extension=0;std::uint32_t flags=0;std::cin>>color>>extension>>flags;
            auto source=resetEmitter();source.type=selectedType;source.color=color;source.extension0.type=extension;source.extension0.parameterA=1;
            const auto result=spawnTailPrefix(source,profile.tail,{-128,100,.1f},{.5,1.5,0},flags);assert(result);
            std::cout<<result->flags<<' '<<result->motionState<<' '<<result->animationInterrupt<<' '<<result->extensionIndex<<' '<<result->profileValue;
            for(const float value:{result->hitboxSize,result->auxiliarySize,result->position.x,result->position.y,result->position.z,
                result->velocity.x,result->velocity.y,result->velocity.z})scalar(value);
            std::cout<<'\n';
        }else if(command=='T'){
            TickInput input;std::cin>>input.type>>input.motionState>>input.age>>input.lifeClock;input.lifeTime=readFloat();
            std::cin>>input.integer0>>input.flags>>input.activeExtensions>>input.grace>>input.cooldown>>input.hasSprite;
            input.spriteWidth=readFloat();input.spriteHeight=readFloat();input.clock=readFloat();std::cin>>input.extensionListEmpty;
            input.position={readFloat(),readFloat(),readFloat()};input.velocity={readFloat(),readFloat(),readFloat()};
            const auto result=preAnimationTick(input);assert(result);const auto& state=result->next;
            std::cout<<int(result->continuation)<<' '<<result->boundary<<' '<<result->previousLifeClock<<' '<<state.motionState<<' '<<state.lifeClock;
            scalar(state.lifeTime);std::cout<<' '<<state.flags<<' '<<state.grace<<' '<<state.cooldown;
            for(const float value:{state.position.x,state.position.y,state.position.z,state.velocity.x,state.velocity.y,state.velocity.z})scalar(value);
            std::cout<<'\n';
        }else return 2;
    }
}
