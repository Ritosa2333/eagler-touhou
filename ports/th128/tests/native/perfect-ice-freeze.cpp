#include "game/IceHost.hpp"
#include "game/IceContact.hpp"
#include "engine/Random.hpp"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
using Bytes=std::vector<std::uint8_t>;
static Bytes read(const char* file){std::ifstream input(file,std::ios::binary);assert(input);return {std::istreambuf_iterator<char>(input),{}};}
static void require(bool ok,const std::string& error){if(!ok){std::cerr<<error<<'\n';std::exit(1);}}
static th128::bullet::Birth makeBirth(const th128::bullet::Profile& profile,std::uint32_t id,std::uint32_t slot,
                                      int color,float x,float y,std::uint32_t flags=1){
    th128::bullet::Birth birth;birth.id=id;birth.slot=slot;birth.type=profile.type;birth.color=color;
    birth.script=profile.script;birth.state.position={x,y,0};birth.state.flags=flags;
    birth.state.motionState=1;birth.state.hitboxSize=profile.tail.hitboxSize;
    birth.state.auxiliarySize=profile.tail.auxiliarySize;birth.state.freezePolicy=profile.tail.freezePolicy;
    birth.angle=.4f;return birth;
}
int main(int count,char** arguments){
    assert(count==6);const auto ice=read(arguments[1]),bullet=read(arguments[2]),masks=read(arguments[3]);
    th128::bullet::Profile type25,type13;
    std::string error;
    require(type25.loadTyped(25,read(arguments[4]).data(),288,error),error);
    require(type13.loadTyped(13,read(arguments[5]).data(),288,error),error);
    assert(type25.tail.freezePolicy==1&&type13.tail.freezePolicy==0);
    th128::bullet::BulletVisuals visuals;require(visuals.load(bullet.data(),bullet.size(),error),error);
    std::map<std::uint32_t,th128::bullet::BulletVisuals::Visual> animations;
    const auto addVisual=[&](std::uint32_t id,const th128::bullet::Profile& profile){
        th128::bullet::BulletVisuals::CreateInfo info;info.type=profile.type;info.color=0;info.script=profile.script;
        info.secondaryScript=profile.secondaryScript?std::optional<int>(profile.secondaryScript):std::nullopt;
        info.spriteRemap={0,0,0,0};info.identitySpriteRemap=profile.identitySpriteRemap();
        auto visual=visuals.create(info,error);require(bool(visual),error);animations.emplace(id,*visual);
    };
    addVisual(1,type25);addVisual(2,type13);addVisual(6,type13);addVisual(8,type13);addVisual(9,type25);
    std::uint32_t consumed=0,expired=0,completed=0,perfectCompletions=0;std::vector<int> sounds;
    th128::ice::Completion perfectCompletion;
    th128::ice::Services services;
    th128::Random display;
    services.random=[]{return 0u;};services.displayRandom=[&]{return display.next32();};
    services.visual=[&](std::uint32_t id){const auto found=animations.find(id);return found==animations.end()?nullptr:&found->second;};
    services.consume=[&](const th128::bullet::Birth& birth,std::string&){assert(animations.erase(birth.id)==1);++consumed;return true;};
    services.sound=[&](int identifier,float,std::string&){sounds.push_back(identifier);return true;};
    services.damage=[](th128::ice::Position,float,std::string&){return true;};
    services.expire=[&](th128::ice::Position,std::string&){++expired;return true;};
    services.complete=[&](th128::ice::Completion,std::string&){++completed;return true;};
    services.perfectComplete=[&](th128::ice::Completion value,std::string&){perfectCompletion=value;++perfectCompletions;return true;};
    th128::ice::Host host;require(host.configure(ice,masks,visuals,services,error),error);
    assert(host.statistics().objects==0&&consumed==0);
    assert(!th128::ice::canFreezeBullet(1,1,60,false)&&th128::ice::canFreezeBullet(1,1,60,true));
    assert(th128::ice::canFreezeBullet(1,0,0,false));
    assert(th128::ice::selectIceMask(1).disposition==th128::ice::IceMaskDisposition::Known);
    assert(th128::ice::selectIceMask(6).disposition==th128::ice::IceMaskDisposition::Unsupported);
    std::vector<th128::bullet::Birth> bullets{
        makeBirth(type25,1,0,0,50,200),makeBirth(type13,2,1,3,51,200),
        makeBirth(type25,3,2,0,70,200,0),makeBirth(type13,4,3,3,1000,200)};
    th128::ice::PerfectFreezeResult result;
    require(host.freezePerfect(bullets,result,error),error);
    assert(result.completed&&result.frozen==2&&result.skipped==2&&result.pixels>0);
    const auto maskProfile=th128::ice::selectIceMask(1);const auto maskBytes=maskProfile.height*maskProfile.wordsPerRow*4;
    std::vector<std::uint8_t> expectedMask(th128::ice::IceMaskBytes,0);
    for(const float x:{50.f,51.f})assert(th128::ice::updateIceMask(expectedMask,
        {std::span<const std::uint8_t>(masks).subspan(0,maskBytes),maskProfile.height,maskProfile.wordsPerRow},
        {x,200,0},th128::ice::IceMaskOperation::Merge));
    assert(result.pixels==*th128::ice::countIceMask(expectedMask));
    assert(consumed==2&&completed==0&&sounds.empty()&&perfectCompletions==0);
    assert(!animations.contains(1)&&!animations.contains(2)&&animations.contains(6));
    assert(host.statistics().busy==0&&host.statistics().objects==2&&host.statistics().completions==0);
    assert(!host.activate(3000,{50,200,0},error)&&error.find("perfect freeze batch")!=std::string::npos);
    for(int frame=0;frame<3;++frame)require(host.tick({},error),error);
    th128::bullet::Birth late=makeBirth(type13,8,6,3,50,200);
    require(host.tick(std::span(&late,1),error),error);
    assert(consumed==3&&host.statistics().objects==3&&!animations.contains(8)&&perfectCompletions==1);
    assert(perfectCompletion.power==12000&&perfectCompletion.pixels>0&&perfectCompletion.percent>0);
    assert(std::count(sounds.begin(),sounds.end(),69)==1);
    th128::bullet::Birth ordinaryImmune=makeBirth(type25,9,7,0,51,200);
    require(host.tick(std::span(&ordinaryImmune,1),error),error);
    assert(consumed==3&&host.statistics().objects==3&&perfectCompletions==1&&animations.contains(9));
    th128::bullet::Birth far=makeBirth(type13,10,8,3,1000,200);
    require(host.tick(std::span(&far,1),error),error);
    assert(consumed==3&&host.statistics().objects==3&&perfectCompletions==1);
    const auto objects=host.objects();assert(objects.size()==3);
    size_t selector1=0;
    for(const auto& object:objects){
        assert(object.mode==0&&object.lifetime.current>=194&&object.lifetime.current<=200&&object.maskSelector>=1&&object.maskSelector<=5);
        assert(object.position.x==50||object.position.x==51);
        if(object.maskSelector==1)++selector1;
    }
    assert(selector1>=2);
    const auto clones=host.draws();
    assert(std::any_of(clones.begin(),clones.end(),[](const auto& draw){return draw.bank==0&&draw.state.layer==13&&(draw.state.flags&0x80000u);}));
    for(int frame=0;frame<30;++frame)require(host.tickAnimations(error),error);
    for(int tick=0;tick<260&&host.statistics().objects;++tick)require(host.tick({},error),error);
    assert(host.statistics().objects==0&&expired==3&&completed==0&&perfectCompletions==1);
    std::vector<th128::bullet::Birth> second{makeBirth(type13,6,4,3,80,200)};
    th128::ice::PerfectFreezeResult secondResult;
    require(host.freezePerfect(second,secondResult,error),error);
    assert(secondResult.completed&&secondResult.frozen==1&&consumed==4);
    th128::ice::Host missing;require(missing.configure(ice,masks,visuals,services,error),error);
    th128::ice::PerfectFreezeResult missingResult;
    std::vector<th128::bullet::Birth> missingBullet{makeBirth(type13,7,5,3,90,200)};
    assert(!missing.freezePerfect(missingBullet,missingResult,error)&&error.find("visual")!=std::string::npos);
    assert(!missingResult.completed&&missing.statistics().objects==0);
    addVisual(1,type25);addVisual(2,type13);
    th128::ice::Host failing;std::uint32_t attempts=0;auto failingServices=services;
    failingServices.consume=[&](const th128::bullet::Birth&,std::string&){++attempts;return false;};
    require(failing.configure(ice,masks,visuals,failingServices,error),error);
    th128::ice::PerfectFreezeResult failedResult;
    std::vector<th128::bullet::Birth> failingBullets{makeBirth(type25,1,0,0,50,200),makeBirth(type13,2,1,3,60,200)};
    assert(!failing.freezePerfect(failingBullets,failedResult,error)&&error.find("consume")!=std::string::npos);
    assert(!failedResult.completed&&attempts==1&&failing.statistics().objects>=1);
    assert(!failing.freezePerfect(failingBullets,failedResult,error));
    addVisual(10,type25);addVisual(11,type13);addVisual(12,type13);addVisual(13,type13);
    th128::ice::Host staged;require(staged.configure(ice,masks,visuals,services,error),error);
    std::vector<th128::bullet::Birth> stagedFirst{makeBirth(type25,10,7,0,50,200)};
    std::vector<th128::bullet::Birth> stagedSecond{makeBirth(type13,11,8,3,51,200)};
    th128::ice::PerfectFreezeResult stagedResult;
    require(staged.freezePerfect(stagedFirst,stagedResult,error),error);
    th128::ice::PerfectFreezeResult overlappingResult;
    assert(!staged.freezePerfect(stagedSecond,overlappingResult,error)&&
        error.find("exclusive empty ice batch")!=std::string::npos&&!overlappingResult.completed);
    th128::ice::Host noCallback;auto noCallbackServices=services;noCallbackServices.perfectComplete={};
    require(noCallback.configure(ice,masks,visuals,noCallbackServices,error),error);
    std::vector<th128::bullet::Birth> noCallbackFirst{makeBirth(type13,12,9,3,50,200)};
    th128::ice::PerfectFreezeResult noCallbackResult;
    require(noCallback.freezePerfect(noCallbackFirst,noCallbackResult,error),error);
    const auto consumedBeforeLate=consumed;std::vector<th128::bullet::Birth> noCallbackLate{makeBirth(type13,13,10,3,50,200)};
    assert(!noCallback.tick(noCallbackLate,error)&&error.find("perfectComplete")!=std::string::npos);
    assert(consumed==consumedBeforeLate&&animations.contains(13)&&noCallback.statistics().objects==1);
    std::cout<<"perfect ice freeze: type25 override, type13 freeze, clone/consume once, selector1, 200 lifetime, expiry and sticky failures passed\n";
}
