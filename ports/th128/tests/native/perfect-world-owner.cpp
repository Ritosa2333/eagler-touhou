#include "game/PerfectFreezeHost.hpp"
#include "game/IceHost.hpp"
#include "game/PlayerHitHost.hpp"
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
static th128::bullet::Birth birth(const th128::bullet::Profile& profile,std::uint32_t id,std::uint32_t slot,float x,float y){
    th128::bullet::Birth value;value.id=id;value.slot=slot;value.type=profile.type;value.color=3;
    value.script=profile.script;value.state.position={x,y,0};value.state.flags=1;value.state.motionState=1;
    value.state.hitboxSize=profile.tail.hitboxSize;value.state.auxiliarySize=profile.tail.auxiliarySize;
    value.state.freezePolicy=profile.tail.freezePolicy;value.angle=.4f;return value;
}
int main(int argc,char** argv){
    assert(argc==7);std::string error;
    const auto ice=read(argv[1]),bullet=read(argv[2]),masks=read(argv[3]),pl00=read(argv[6]);
    th128::bullet::Profile type25,type13;
    require(type25.loadTyped(25,read(argv[4]).data(),288,error),error);
    require(type13.loadTyped(13,read(argv[5]).data(),288,error),error);
    th128::bullet::BulletVisuals visuals;require(visuals.load(bullet.data(),bullet.size(),error),error);
    std::map<std::uint32_t,th128::bullet::BulletVisuals::Visual> animations;
    const auto add=[&](std::uint32_t id,const th128::bullet::Profile& profile){
        th128::bullet::BulletVisuals::CreateInfo info;info.type=profile.type;info.color=0;info.script=profile.script;
        info.secondaryScript=profile.secondaryScript?std::optional<int>(profile.secondaryScript):std::nullopt;
        info.spriteRemap={0,0,0,0};info.identitySpriteRemap=profile.identitySpriteRemap();
        auto value=visuals.create(info,error);require(bool(value),error);animations.emplace(id,*value);
    };
    add(1,type25);add(2,type13);add(6,type13);add(7,type25);

    th128::player::PlayerHitHost player;
    const th128::player::hit::Timer zero{-1,0,0,0x4b6f58,1};
    const th128::player::HitHostSnapshot snapshot{{1,0,0},zero,zero,zero,{true,0},{0,0,0},{true,0}};
    require(player.configure(pl00.data(),pl00.size(),snapshot,
        [](int,th128::player::hit::Position,int,std::string&){return true;},
        [](int,std::string&){return true;},error),error);

    std::uint32_t consumed=0,perfectCompletions=0,initialSettlements=0,initialPixels=0;
    th128::ice::Services iceServices;th128::Random display;
    iceServices.random=[]{return 0u;};iceServices.displayRandom=[&]{return display.next32();};
    iceServices.visual=[&](std::uint32_t id){const auto found=animations.find(id);return found==animations.end()?nullptr:&found->second;};
    iceServices.consume=[&](const th128::bullet::Birth& value,std::string&){require(animations.erase(value.id)==1,"consume missing visual");++consumed;return true;};
    iceServices.sound=[](int,float,std::string&){return true;};
    iceServices.damage=[](th128::ice::Position,float,std::string&){return true;};
    iceServices.expire=[](th128::ice::Position,std::string&){return true;};
    iceServices.complete=[](th128::ice::Completion,std::string&){return true;};
    iceServices.perfectComplete=[&](th128::ice::Completion value,std::string&){require(value.power==12000,"perfect callback power");++perfectCompletions;return true;};
    th128::ice::Host iceHost;require(iceHost.configure(ice,masks,visuals,iceServices,error),error);

    int protectCalls=0,freezeCalls=0;
    th128::perfect::Services services;
    services.protect=[&](int ticks,std::string& reason){++protectCalls;return ticks==th128::perfect::ProtectionTicks&&player.assignPerfectProtection200(reason);};
    std::vector<th128::bullet::Birth> shot{birth(type25,1,0,50,200),birth(type13,2,1,51,200)};
    services.freezeBullets=[&](const th128::perfect::BulletRequest&,th128::perfect::BulletResult& result,std::string& reason){
        ++freezeCalls;th128::ice::PerfectFreezeResult frozen;
        if(!iceHost.freezePerfect(shot,frozen,reason))return false;
        require(frozen.completed&&frozen.frozen==2&&frozen.skipped==0,"perfect ice preflight/result");
        ++initialSettlements;initialPixels=frozen.pixels;
        result={frozen.frozen,0,frozen.skipped,true};return true;
    };
    th128::perfect::Context context;context.perfectCharge=30000;context.x=320;context.y=416;context.radius=2;
    context.systemPresent=true;context.systemMode3c=0;context.busy=0;context.playerState=1;
    context.mode=th128::perfect::Mode::Normal;context.lasers=th128::perfect::LaserContext::Empty;context.laserCount=0;
    th128::perfect::Host host;require(host.loadNativeEffect(ice,error),error);
    require(host.configure(context,services,error),error);
    auto pressed=context;pressed.keys=th128::perfect::XKeyBit;pressed.xPressed=true;
    require(host.activate(pressed,error),error);
    require(protectCalls==1&&host.status().active&&host.status().charge==20000,"activation/stock");
    pressed.perfectCharge=host.status().charge;
    const auto protection=player.snapshot().protection;
    require(protection.previous==199&&protection.current==200&&protection.fractional==200.f&&
        (protection.flags&1u)&&player.snapshot().gate.counter438dc==200,"native protection assignment");
    require(!host.effectSnapshots().empty(),"native ice effect root");
    require(host.tick(true,error)&&host.status().phase==0,"paused activation tick does not advance");

    for(int index=0;index<119;++index)require(host.tick(false,error),error);
    require(freezeCalls==0&&consumed==0&&initialSettlements==0,"no bullet mutation before phase120");
    require(host.tick(false,error),error);
    require(freezeCalls==1&&initialSettlements==1&&consumed==2&&initialPixels>0,"phase120 initial perfect batch is settled once");
    require(host.status().frozen==2&&host.status().cleared==0&&host.status().immune==0,"perfect status frozen/cleared/immune");

    for(int frame=0;frame<3;++frame)require(iceHost.tick({},error),error);
    th128::bullet::Birth late=birth(type13,6,2,50,200);
    require(iceHost.tick(std::span(&late,1),error),error);
    require(consumed==3&&perfectCompletions==1,"later ordinary contact propagates through perfectComplete once");
    th128::bullet::Birth policy1=birth(type25,7,3,51,200);
    const auto before=consumed;require(iceHost.tick(std::span(&policy1,1),error),error);
    require(consumed==before&&animations.contains(7),"ordinary policy1 remains immune");
    for(int index=0;index<80;++index)require(host.tick(false,error),error);
    require(!host.status().active&&host.status().protectionTicks==0,"protection expiry ends Perfect");
    std::string ignored;
    require(!host.activate(pressed,ignored),"X latch remains held without keyup");
    auto released=context;released.xPressed=false;released.keys=0;
    require(!host.activate(released,ignored),"release resets latch only");
    require(host.activate(pressed,ignored),"fresh X press after keyup re-arms");
    require(host.shutdown(ignored),"shutdown");

    th128::perfect::Host denied;require(denied.loadNativeEffect(ice,error),error);
    auto low=context;low.perfectCharge=5000;require(denied.configure(low,services,error),error);
    auto lowPressed=pressed;lowPressed.perfectCharge=5000;
    require(!denied.activate(lowPressed,error)&&!denied.status().failed&&!denied.status().lastDenied.empty(),"insufficient stock is a nonfatal denial");

    th128::perfect::Host unknown;auto unknownContext=context;unknownContext.lasers=th128::perfect::LaserContext::Unknown;
    require(!unknown.configure(unknownContext,services,error),"unknown laser context rejected");
    th128::perfect::Host missing;require(missing.loadNativeEffect(ice,error),error);
    th128::perfect::Services missingServices=services;missingServices.freezeBullets={};missingServices.protect={};
    require(!missing.configure(context,missingServices,error),"missing services rejected");
    std::cout<<"PASS perfect world owner: protection, phase120 once, consume, callback, policy1, expiry, rearm, denial\n";
}
