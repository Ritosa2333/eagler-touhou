#include "game/IceHost.hpp"
#include "engine/Random.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
using Bytes=std::vector<std::uint8_t>;
Bytes read(const char* file){std::ifstream input(file,std::ios::binary);assert(input);return {std::istreambuf_iterator<char>(input),{}};}
void require(bool success,const std::string& error){if(!success){std::cerr<<error<<'\n';std::exit(1);}}
int main(int count,char** arguments){
    assert(count==4);const auto ice=read(arguments[1]),bullet=read(arguments[2]),masks=read(arguments[3]);
    th128::bullet::BulletVisuals visuals;std::string error;require(visuals.load(bullet.data(),bullet.size(),error),error);
    std::map<std::uint32_t,th128::bullet::BulletVisuals::Visual> animations;
    std::vector<th128::bullet::Birth> live;
    for(std::uint32_t identifier=1;identifier<=4;++identifier){
        th128::bullet::BulletVisuals::CreateInfo info;info.spriteRemap={0,0,0,0};
        auto visual=visuals.create(info,error);require(bool(visual),error);animations.emplace(identifier,*visual);
        if(identifier==1)animations.at(identifier).primary.control().flags|=0x40000000u;
        th128::bullet::Birth birth;birth.id=identifier;birth.slot=identifier-1;birth.state.flags=1;
        birth.angle=.4f;
        birth.state.position={identifier==1?50.f:identifier==2?60.f:identifier==3?150.f:0.f,200,0};
        birth.state.hitboxSize=4;birth.state.auxiliarySize=10;birth.state.freezePolicy=identifier==4?1:0;live.push_back(birth);
    }
    std::vector<int> sounds;unsigned consumed=0,damage=0,expired=0,completed=0,random=0;
    th128::ice::Completion completion;
    th128::ice::Services services;
    th128::Random display;
    services.random=[&]{++random;return 0xffffffffu;};
    services.displayRandom=[&]{return display.next32();};
    services.visual=[&](std::uint32_t identifier){const auto found=animations.find(identifier);return found==animations.end()?nullptr:&found->second;};
    services.consume=[&](const auto& birth,std::string&){assert(animations.erase(birth.id)==1);++consumed;return true;};
    services.sound=[&](int identifier,float,std::string&){sounds.push_back(identifier);return true;};
    services.damage=[&](th128::ice::Position,float radius,std::string&){assert(radius==38);++damage;return true;};
    services.expire=[&](th128::ice::Position,std::string&){++expired;return true;};
    services.complete=[&](th128::ice::Completion value,std::string&){completion=value;++completed;return true;};
    th128::ice::Host host;require(host.configure(ice,masks,visuals,services,error),error);
    require(host.charge(1,{0,200,0},error),error);const auto charge=host.draws();assert(!charge.empty());
    require(host.charge(20,{10,200,0},error),error);const auto movedCharge=host.draws();
    assert(movedCharge.size()==charge.size());for(std::size_t index=0;index<charge.size();++index)assert(movedCharge[index].x==charge[index].x+10);
    require(host.charge(0,{10,200,0},error),error);
    require(host.activate(10000,{0,200,0},error),error);assert(host.statistics().busy==1&&host.statistics().objects==1);
    const auto original=animations.at(1).primary.state();
    const auto tick=[&]{std::vector<th128::bullet::Birth> current;for(const auto& birth:live)if(animations.contains(birth.id))current.push_back(birth);
        require(host.tick(current,error),error);require(host.tickAnimations(error),error);};
    tick();assert(consumed==1&&host.statistics().objects==2&&host.statistics().pixels>0);
    const auto frozen=host.draws();const auto clone=std::find_if(frozen.begin(),frozen.end(),[](const auto& draw){return draw.bank==0;});
    assert(clone!=frozen.end()&&clone->state.layer==13&&(clone->state.flags&0x80000u));assert(clone->state.script==original.script);
    assert(clone->state.rotation==*th128::bullet::directionalDrawAngle(.4f,original.rotation,original.flags));
    tick();tick();assert(consumed==1);tick();assert(consumed==2);
    assert(animations.contains(3)&&animations.contains(4));assert(host.statistics().pixels>0);
    const auto before=host.statistics();const auto draws=host.draws();assert(draws.size()>2);
    for(int repeat=0;repeat<10;++repeat){assert(host.draws().size()==draws.size());assert(host.objects().size()==before.objects);}
    while(!completed&&host.statistics().elapsed<=101)tick();
    assert(completed==1&&consumed==2&&damage==2&&expired==2);assert(completion.power==10000&&completion.pixels>0&&completion.percent>0);
    assert(host.statistics().busy==0&&host.statistics().objects==0&&host.statistics().completions==1);
    assert(sounds.front()==47&&std::count(sounds.begin(),sounds.end(),69)==2&&std::count(sounds.begin(),sounds.end(),71)==1);
    require(host.activate(3000,{0,200,0},error),error);for(int elapsed=0;elapsed<20;++elapsed)tick();assert(completed==2);
    assert(completion.pixels==0&&random>3);
    require(host.activate(10000,{0,400,0},error),error);for(int elapsed=0;elapsed<180;++elapsed)tick();
    assert(completed==3);assert(std::count(sounds.begin(),sounds.end(),71)==3);
    for(const auto& draw:host.draws())assert(draw.state.script!=33&&draw.state.script!=34);
    th128::ice::Host rejected;auto missing=services;missing.complete={};assert(!rejected.configure(ice,masks,visuals,missing,error));
    th128::ice::Host failing;auto failure=services;failure.consume=[](const auto&,std::string& reason){reason="real consumption failed";return false;};
    require(failing.configure(ice,masks,visuals,failure,error),error);require(failing.activate(3000,{150,200,0},error),error);
    assert(!failing.tick(std::span(live).subspan(2,1),error)&&error=="real consumption failed");assert(!failing.tick({},error));
    animations.clear();live.clear();consumed=damage=expired=completed=0;
    th128::bullet::BulletVisuals::CreateInfo largeInfo;largeInfo.type=18;largeInfo.color=3;largeInfo.script=72;largeInfo.spriteRemap={371,326,0,375};
    auto largeVisual=visuals.create(largeInfo,error);require(bool(largeVisual),error);require(visuals.interrupt(*largeVisual,2,error)&&visuals.advance(*largeVisual,1,error),error);animations.emplace(91,*largeVisual);
    th128::bullet::Birth large;large.id=91;large.type=18;large.lifeClock=0;large.state.flags=1;large.state.position={0,200,0};
    large.state.profileValue=1;large.state.freezePolicy=0;large.state.hitboxSize=8.5f;large.state.auxiliarySize=22;
    auto largeServices=services;largeServices.damage=[](th128::ice::Position,float radius,std::string&){assert(radius==float(22.*3.799999952316284));return true;};
    th128::ice::Host largeHost;require(largeHost.configure(ice,masks,visuals,largeServices,error),error);require(largeHost.activate(10000,{0,200,0},error),error);
    require(largeHost.tick(std::span(&large,1),error),error);assert(consumed==1&&largeHost.statistics().converted==1&&largeHost.statistics().pixels>1000);
    const auto largeIce=largeHost.objects();assert(std::any_of(largeIce.begin(),largeIce.end(),[](const auto& item){return item.mode==0&&item.value==22&&item.maskSelector==4;}));
    const auto largeDraws=largeHost.draws();assert(std::any_of(largeDraws.begin(),largeDraws.end(),[](const auto& draw){return draw.bank==0&&draw.state.script==72&&draw.state.sprite==371;}));
    std::cout<<"native ice ownership and delayed propagation, cloned primary VM, mask, shatter and required completion passed\n";
}
