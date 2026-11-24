#include "game/EnemyDropEmission.hpp"
#include "game/ItemHost.hpp"
#include "engine/Random.hpp"
#include <bit>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>

float readFloat(){std::uint32_t bits=0;std::cin>>bits;return std::bit_cast<float>(bits);}
void writePosition(th128::enemy::DropPosition position){for(float value:{position.x,position.y,position.z})std::cout<<' '<<std::bit_cast<std::uint32_t>(value);}
int main(int argc,char** argv){
    assert(argc==2);std::ifstream file(argv[1],std::ios::binary);assert(file);
    const std::vector<unsigned char> animation{std::istreambuf_iterator<char>(file),{}};
    th128::item::ItemHost items;std::string error;assert(items.load(animation,error));
    th128::enemy::DropState authored;authored.words[0]=1;authored.words[2]=2;authored.width=authored.height=24;
    th128::enemy::AdditionalDropOperation real(authored,{10,100,.1f});th128::Random random;random.seed=0x1234;
    unsigned calls=0;
    assert(real.run([&]{return random.next32();},[&](const th128::enemy::DropEmission& birth,std::string& reason){
        assert(birth.kind==2&&birth.angle==-1.5707963705062866f&&birth.speed==2.200000047683716f);
        ++calls;return items.spawn(birth.kind,{birth.position.x,birth.position.y,birth.position.z},birth.angle,birth.speed,reason);
    },error));
    assert(calls==2&&items.snapshots().size()==2&&random.calls==10);
    assert(real.state().words[0]==1&&real.state().words[2]==0&&real.state().width==24);
    unsigned repeated=0;assert(!real.run([&]{++repeated;return random.next32();},[](const auto&,auto&){return true;},error)&&repeated==0);
    th128::enemy::AdditionalDropOperation interleaved(authored,{10,100,.1f});th128::Random shared;shared.seed=0;
    unsigned itemCalls=0;
    assert(interleaved.run([&]{return shared.next32();},[&](const auto&,auto&){
        assert(shared.calls==(itemCalls==0?4u:10u));++itemCalls;shared.next32();return true;
    },error));
    assert(itemCalls==2&&shared.calls==14);
    th128::enemy::DropState capped; capped.words[2]=601;
    th128::enemy::AdditionalDropOperation rejected(capped,{0,0,0});unsigned rejectedRandom=0;
    assert(!rejected.run([&]{++rejectedRandom;return 0u;},[](const auto&,auto&){return true;},error)&&rejectedRandom==0);
    char command=0;
    while(std::cin>>command){
        unsigned seed=0;std::cin>>seed;th128::Random source;source.seed=std::uint16_t(seed);
        if(command=='E'){
            auto state=authored;state.width=readFloat();state.height=readFloat();
            th128::enemy::AdditionalDropOperation operation(state,{10,100,.1f});
            th128::enemy::DropEmission captured;unsigned attempts=0;
            const auto stop=[&](const th128::enemy::DropEmission& emission,std::string& reason){captured=emission;++attempts;reason="required native item boundary";return false;};
            assert(!operation.run([&]{return source.next32();},stop,error)&&attempts==1);
            assert(error=="required native item boundary");
            assert(!operation.run([&]{return source.next32();},stop,error)&&attempts==1);
            std::cout<<captured.kind<<' '<<source.seed<<' '<<source.calls;writePosition(captured.position);
            std::cout<<' '<<std::bit_cast<std::uint32_t>(captured.angle)<<' '<<std::bit_cast<std::uint32_t>(captured.speed)<<'\n';
        }else if(command=='A'){
            const float angle=readFloat();const auto result=th128::enemy::advanceDropAngle(angle,source.next32());assert(result);
            std::cout<<std::bit_cast<std::uint32_t>(*result)<<' '<<source.seed<<' '<<source.calls<<'\n';
        }else if(command=='G'){
            const auto angle=readFloat(),width=readFloat(),height=readFloat();
            const auto result=th128::enemy::scatterDrop({10,100,.1f},angle,width,height,source.next32());assert(result);
            std::cout<<source.seed<<' '<<source.calls;writePosition(*result);std::cout<<'\n';
        }else if(command=='Z'){
            th128::enemy::DropState empty;empty.words[10]=99;
            th128::enemy::AdditionalDropOperation operation(empty,{10,100,.1f});unsigned births=0;
            assert(operation.run([&]{return source.next32();},[&](const auto&,auto&){++births;return true;},error)&&births==0);
            assert(operation.state().words[10]==0);std::cout<<source.seed<<' '<<source.calls<<'\n';
        }else return 2;
    }
}
