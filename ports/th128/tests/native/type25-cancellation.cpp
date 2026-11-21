#include "animation/AnmScene.hpp"
#include "game/CancellationEffects.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <set>
#include <string>
#include <vector>

using th128::anm::Bank;
using th128::anm::Membership;
using th128::anm::asWord;
using th128::effects::CancellationEffects;
using Handle=CancellationEffects::Handle;
using Bytes=std::vector<std::uint8_t>;

static void check(bool condition,const std::string& message){
    if(!condition){std::cerr<<message<<'\n';std::exit(1);}
}
static void near(float actual,float expected,const std::string& message){
    check(std::isfinite(actual)&&std::abs(actual-expected)<0.001f,message);
}
static void put16(Bytes& bytes,size_t offset,std::uint16_t value){
    bytes.at(offset)=std::uint8_t(value);bytes.at(offset+1)=std::uint8_t(value>>8);
}
static void put32(Bytes& bytes,size_t offset,std::uint32_t value){
    for(size_t byte=0;byte<4;++byte)bytes.at(offset+byte)=std::uint8_t(value>>(byte*8));
}
struct Instruction{int opcode,time;std::vector<std::uint32_t> words;std::uint16_t mask=0;};
static void append(Bytes& bytes,const std::vector<Instruction>& program){
    for(const auto& command:program){
        const size_t offset=bytes.size(),length=8+command.words.size()*4;
        bytes.resize(offset+length);
        put16(bytes,offset,std::uint16_t(command.opcode));put16(bytes,offset+2,std::uint16_t(length));
        put16(bytes,offset+4,std::uint16_t(command.time));put16(bytes,offset+6,command.mask);
        for(size_t index=0;index<command.words.size();++index)
            put32(bytes,offset+8+index*4,command.words[index]);
    }
    const size_t end=bytes.size();bytes.resize(end+8);put16(bytes,end,65535);
}
static Bytes synthetic(const std::vector<Instruction>& root,const std::vector<Instruction>& child){
    constexpr size_t ScriptCount=372,TableEnd=64+8+ScriptCount*8;
    Bytes bytes(TableEnd+4+40);
    put32(bytes,0,7);put16(bytes,4,2);put16(bytes,6,ScriptCount);put16(bytes,10,64);put16(bytes,12,64);
    put32(bytes,16,TableEnd);bytes[TableEnd]='t';
    for(size_t sprite=0;sprite<2;++sprite){
        const size_t offset=TableEnd+4+sprite*20;
        put32(bytes,64+sprite*4,std::uint32_t(offset));
        put32(bytes,offset,std::uint32_t(sprite));put32(bytes,offset+12,asWord(32.f));put32(bytes,offset+16,asWord(32.f));
    }
    for(size_t script=0;script<ScriptCount;++script){
        put32(bytes,72+script*8,std::uint32_t(script));put32(bytes,76+script*8,std::uint32_t(bytes.size()));
        append(bytes,script==371?root:script==370?child:std::vector<Instruction>{{2,0,{}}});
    }
    return bytes;
}
static bool load(CancellationEffects& effects,const Bytes& bytes,std::string& error){
    return effects.load(bytes.data(),bytes.size(),error);
}
static Handle spawn(CancellationEffects& effects,int script=371){
    std::string error;
    const auto handle=effects.spawn(script,332,40,5,23,error);
    check(handle&&*handle&&error.empty(),"spawn failed: "+error);
    return *handle;
}
static CancellationEffects::Snapshot snapshot(const CancellationEffects& effects,Handle handle,int script){
    for(const auto& value:effects.snapshots())
        if(value.handle==handle&&value.state.script==script)return value;
    check(false,"missing snapshot");return {};
}
struct Sequence{
    std::vector<std::uint32_t> values;size_t at=0;
    std::uint32_t next(){return at<values.size()?values[at++]:0u;}
};
static void compare(const CancellationEffects& effects,const th128::anm::Scene& reference,uint32_t& calls,
                    uint32_t& referenceCalls,const std::string& phase){
    check(calls==referenceCalls,"RNG divergence "+phase);
    check(effects.nodeCount()==reference.nodes().size(),"node count divergence "+phase);
    for(const auto& value:effects.snapshots()){
        const auto found=reference.nodes().find(value.node);
        check(found!=reference.nodes().end(),"reference node missing "+phase);
        const auto& expected=found->second->vm.state();
        check(value.state.script==expected.script&&value.state.ticks==expected.ticks&&
              value.state.clock==expected.clock&&value.state.sprite==expected.sprite&&
              value.state.layer==expected.layer&&value.state.integers==expected.integers&&
              value.state.primary==expected.primary&&asWord(value.state.rotation)==asWord(expected.rotation)&&
              asWord(value.state.sx)==asWord(expected.sx)&&asWord(value.state.sy)==asWord(expected.sy),
              "state divergence "+phase);
        check(value.membership==found->second->membership&&value.parent==found->second->parent,
              "ownership divergence "+phase);
    }
}
static void actualCases(const char* path){
    std::ifstream file(path,std::ios::binary);
    check(bool(file),"cannot open actual bullet.anm");
    const Bytes bytes{std::istreambuf_iterator<char>(file),{}};
    CancellationEffects effects;std::string error;
    check(load(effects,bytes,error),error);
    check(!effects.spawn(370,0,0,0,23,error),"child370 must not be a spawnable root");
    check(!effects.spawn(369,0,0,0,23,error),"unproven 369 root must stay rejected");
    check(!effects.spawn(373,0,0,0,23,error),"unproven 373 root must stay rejected");
    check(!effects.spawn(371,0,0,0,31,error),"invalid layer must stay rejected");
    check(!effects.spawn(371,std::numeric_limits<float>::infinity(),0,0,23,error),"invalid pose must stay rejected");
    std::vector<std::uint32_t> values(4096,0u);values[0]=3u;values[7]=2u;values[14]=4u;
    Sequence actualSequence{values},referenceSequence{values};
    std::uint32_t calls=0,referenceCalls=0;
    effects.setGameRandom([&]{++calls;return actualSequence.next();});
    const auto handle=spawn(effects);
    check(effects.rootCount()==1&&effects.nodeCount()==1,"first random 3 must delay child creation");
    check(!effects.initializeNativeDrift(handle,{1,2,3},.5f,{0,0,0},error),"clock .5 must fail");
    check(!effects.initializeNativeDrift(handle,{1,2,3},1.f,{1,0,0},error),"nonzero global start must fail");
    const auto callsBeforeDrift=calls;
    check(effects.initializeNativeDrift(handle,{1,2,3},1.f,{0,0,0},error),error);
    check(calls==callsBeforeDrift,"drift initializer must not read RNG");
    th128::anm::Scene reference;
    check(reference.registry.loadBank(0,bytes.data(),bytes.size(),error),error);
    reference.runtime.gameRandom32=[&]{++referenceCalls;return referenceSequence.next();};
    reference.runtime.unsupported=[&](int,int,int,const char* reason){check(false,reason);};
    check(reference.bind(handle,handle,0,371,Membership::Primary,332,40,{},false,nullptr,0,0,23,false,5),
          "reference bind failed");
    compare(effects,reference,calls,referenceCalls,"birth");
    size_t maximumNodes=effects.nodeCount(),endPass=0,previousChildren=0;
    std::vector<int> birthPasses;
    for(int pass=1;pass<=70;++pass){
        check(effects.tick(error),error);
        reference.tickPrimary();reference.collect();
        compare(effects,reference,calls,referenceCalls,"tick "+std::to_string(pass));
        maximumNodes=std::max(maximumNodes,effects.nodeCount());
        const auto snapshots=effects.snapshots();
        size_t children=0,rootCount=0;
        for(const auto& value:snapshots){
            check(value.handle==handle,"effect owner divergence");
            check(std::isfinite(value.world.x)&&std::isfinite(value.world.y)&&std::isfinite(value.world.z)&&
                  std::isfinite(value.world.sx)&&std::isfinite(value.world.sy)&&std::isfinite(value.world.rotation),
                  "non-finite world pose");
            if(value.state.script==371){++rootCount;check(value.parent==0&&value.state.layer==23,"root pose/layer");
                check(!value.sprite,"invisible root must not select a sprite");}
            if(value.state.script==370){
                ++children;
                check(value.parent!=0&&value.state.layer==15,"child parent/layer");
                check(value.state.sprite>=580&&value.state.sprite<=587,"child sprite domain");
                check(value.sprite&&value.sprite->width==32&&value.sprite->height==32,"child sprite metadata");
                check(value.world.sx>=0.f&&value.world.sx<=1.0001f,
                      "child scale domain "+std::to_string(value.world.sx));
            }
        }
        check(rootCount<=1&&children<=3,"child/root budget");
        if(children>previousChildren){birthPasses.push_back(pass);previousChildren=children;}
        if(pass==9){
            check(rootCount==1&&children==3&&effects.nodeCount()==4,"three random-delayed children by pass 9");
            check(maximumNodes<=4,"reservation budget exceeded");
        }
        if(pass==30){
            check(children==3,"all three children must survive to the 40-frame tail");
            const auto root=snapshot(effects,handle,371);
            near(root.state.x,10.f,"drift x");near(root.state.y,20.f,"drift y");near(root.state.z,30.f,"drift z");
        }
        if(pass==42)check(children==3,"first child must still serve pass 42");
        if(pass==43)check(children==2,"first child retires at pass 43");
        if(pass==45)check(children==1,"second child retires at pass 45");
        if(effects.nodeCount()==0){
            check(pass==49,"random delays 3/2/4 plus 40-frame tail must retire at pass 49");
            endPass=pass;break;
        }
    }
    check((birthPasses==std::vector<int>{3,5,9}),"op26/op75 delays must birth children on passes 3, 5 and 9");
    check(endPass==49,"staggered tail cleanup pass");
    check(effects.rootCount()==0&&effects.nodeCount()==0&&effects.snapshots().empty(),"cleanup must empty scene");
    check(calls==24,"root/three children must consume exactly 24 game RNG reads");
    check(maximumNodes==4,"root plus exactly three children must fill reservation 4");
    const auto callsAtEnd=calls;
    check(effects.tick(error),error);
    check(calls==callsAtEnd&&effects.nodeCount()==0,"no RNG or nodes after cleanup");
    CancellationEffects immediate;std::uint32_t immediateCalls=0;
    check(load(immediate,bytes,error),error);
    immediate.setGameRandom([&]{++immediateCalls;return 0u;});
    const auto immediateHandle=spawn(immediate);
    check(immediate.rootCount()==1&&immediate.nodeCount()==4,"zero RNG must birth all three children in the bind tick");
    check(immediateCalls==21,"birth must consume root3 plus six reads per child");
    const auto immediateRoot=snapshot(immediate,immediateHandle,371);
    check(immediateRoot.state.ticks==1&&immediateRoot.state.clock==-39&&
          !immediateRoot.state.ended&&!immediateRoot.state.deleteRequested,
          "op75(40) must leave the root alive at clock -39, not ended in its bind tick");
    std::set<int> sprites;size_t immediateMax=immediate.nodeCount();
    for(int pass=1;pass<=40;++pass){
        check(immediate.tick(error),error);
        immediateMax=std::max(immediateMax,immediate.nodeCount());
        for(const auto& value:immediate.snapshots()){
            check(value.handle==immediateHandle,"owner divergence");
            if(value.state.script==370){
                check(value.parent!=0&&value.state.sprite>=580&&value.state.sprite<=587,"child sprite service");
                sprites.insert(value.state.sprite);
                check(value.state.ticks>0,"child ticks");
            }
        }
        if(pass==39){
            const auto root=snapshot(immediate,immediateHandle,371);
            check(root.state.clock==0&&!root.state.ended,"root must survive the 40th pass gate");
            size_t children=0;
            for(const auto& value:immediate.snapshots())if(value.state.script==370){
                ++children;check(value.state.clock==40&&!value.state.ended,"child must stay served through pass 39");
            }
            check(children==3,"three children must remain registered before the final pass");
        }
        if(pass==40){
            check(immediate.nodeCount()==0&&immediate.rootCount()==0&&immediate.snapshots().empty(),
                  "root and last child must retire together on pass 40");
        }
    }
    check(immediateMax==4,"zero-RNG reservation must stay at four nodes");
    check(sprites.size()==8&&*sprites.begin()==580&&*sprites.rbegin()==587,
          "child must render the authored 580..587 sequence");
    check(immediateCalls==24,"complete effect must consume 24 game RNG reads");
    th128::anm::Scene zeroReference;
    check(zeroReference.registry.loadBank(0,bytes.data(),bytes.size(),error),error);
    zeroReference.runtime.gameRandom32=[] {return 0u;};
    zeroReference.runtime.unsupported=[&](int,int,int,const char* reason){check(false,reason);};
    constexpr Handle zeroOwner=77;
    check(zeroReference.bind(zeroOwner,zeroOwner,0,371,Membership::Primary,332,40,{},false,nullptr,0,0,23,false,5),
          "zero reference bind failed");
    check(zeroReference.nodes().size()==4,"zero reference must birth three children");
    for(int pass=1;pass<=39;++pass){zeroReference.tickPrimary();zeroReference.collect();
        check(zeroReference.nodes().size()==4,"children must remain registered through pass 39");}
    zeroReference.tickPrimary();
    size_t endedChildren=0,endedRoots=0;
    for(const auto& [id,node]:zeroReference.nodes()){
        if(node->vm.state().script==370){
            check(node->vm.state().ended&&node->vm.state().clock==40&&node->vm.state().ticks==40,
                  "child must execute its own op1 in the root's final pass, before collection");
            ++endedChildren;
        }else if(node->vm.state().script==371){
            check(node->vm.state().ended&&node->vm.state().clock==0&&node->vm.state().ticks==40,
                  "root must execute op1 on pass 40");
            ++endedRoots;
        }
    }
    check(endedChildren==3&&endedRoots==1,"root and all three children must end in the same pass");
    zeroReference.collect();
    check(zeroReference.nodes().empty(),"joint retirement must leave no nodes");
    const auto valid=synthetic({{88,0,{370}},{1,0,{}}},{{1,0,{}}});
    CancellationEffects accepted;
    check(load(accepted,valid,error),error);
    check(!load(accepted,synthetic({{88,0,{370}},{88,0,{999}},{1,0,{}}},{{1,0,{}}}),error)&&
          error.find("type25 cancellation children")!=std::string::npos,"unproven child target must reject");
    check(!load(accepted,synthetic({{88,0,{370}},{88,0,{370}},{88,0,{370}},{88,0,{370}},{1,0,{}}},
                                   {{1,0,{}}}),error)&&
          error.find("type25 cancellation children")!=std::string::npos,"more than three children must reject");
    check(!load(accepted,synthetic({{88,0,{370}},{102,0,{0,0}},{1,0,{}}},{{1,0,{}}}),error)&&
          error.find("unsupported opcode=102")!=std::string::npos,"unknown opcode must reject");
    check(!load(accepted,synthetic({{88,0,{370}},{1,0,{}}},{{88,0,{371}},{1,0,{}}}),error)&&
          error.find("child-of-child")!=std::string::npos,"child-of-child must reject");
    check(!load(accepted,synthetic({{88,0,{370}},{4,0,{0,0}},{1,0,{}}},{{1,0,{}}}),error)&&
          error.find("type25 cancellation loop")!=std::string::npos,"root loop must reject");
    std::cout<<"type25 cancellation 371/370 actual + strict synthetic cases passed\n";
}
int main(int argc,char** argv){
    check(argc==2,"usage: type25-cancellation <actual bullet.anm>");
    actualCases(argv[1]);
}
