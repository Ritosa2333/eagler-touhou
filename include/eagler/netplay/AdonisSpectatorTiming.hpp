#pragma once
#include <eagler/netplay/AdonisStartup.hpp>
#include <vector>
namespace Netplay {
struct AdonisSpectatorTiming {
    unsigned game=0,mode=0,delay=0,prediction=0,fullDelay=0;
    std::uint64_t sessionId=0;std::uint32_t gameplayAbi=0,rttP95Us=0,lost=0,samples=0;
    bool automatic=false;
    bool Valid()const{
        return (game==8||game==10)&&(mode==1||mode==2)&&sessionId&&gameplayAbi&&delay<=9&&prediction<=2&&
            rttP95Us>0&&rttP95Us<=1'000'000&&samples>=96&&samples<=120&&lost<=72&&
            fullDelay==std::max(1u,((rttP95Us/2)*60+999999)/1000000)&&
            (mode!=1||!prediction)&&prediction<=fullDelay-(automatic?1u:0u)&&
            (!automatic||delay==fullDelay-prediction);
    }
    std::vector<std::uint8_t> Encode()const{
        if(!Valid())return {};std::vector<std::uint8_t> out(40);
        out[0]='E';out[1]=game==8?'8':'A';out[2]='T';out[3]='M';out[4]=1;out[5]=mode;out[6]=delay;out[7]=prediction;
        const auto put=[&](unsigned at,std::uint64_t value,unsigned n=4){for(unsigned i=0;i<n;++i)out[at+i]=std::uint8_t(value>>(8*i));};
        put(8,fullDelay);put(12,sessionId,8);put(20,gameplayAbi);put(24,rttP95Us);put(28,lost);put(32,automatic);put(36,samples);return out;
    }
    static bool IsPacket(const std::uint8_t* p,std::size_t n){return p&&n>=4&&p[0]=='E'&&(p[1]=='8'||p[1]=='A')&&p[2]=='T'&&p[3]=='M';}
    static bool Decode(const std::uint8_t* p,std::size_t n,AdonisSpectatorTiming& result){
        if(!IsPacket(p,n)||n!=40||p[4]!=1)return false;
        const auto get=[&](unsigned at,unsigned n=4){std::uint64_t value=0;for(unsigned i=0;i<n;++i)value|=std::uint64_t(p[at+i])<<(i*8);return value;};
        AdonisSpectatorTiming next;next.game=p[1]=='8'?8:10;next.mode=p[5];next.delay=p[6];next.prediction=p[7];
        next.fullDelay=get(8);next.sessionId=get(12,8);next.gameplayAbi=get(20);next.rttP95Us=get(24);next.lost=get(28);
        if(get(32)>1)return false;next.automatic=get(32);next.samples=get(36);
        if(!next.Valid())return false;result=next;return true;
    }
};
inline AdonisSpectatorTiming MakeAdonisSpectatorTiming(const AdonisStartup& startup,const SessionConfig& config,bool automatic){
    AdonisSpectatorTiming result;const auto choice=startup.Selected();
    result.game=config.gameId;result.sessionId=config.sessionId;result.gameplayAbi=config.gameplayAbi;
    result.delay=choice.delay;result.prediction=choice.prediction;result.fullDelay=choice.fullDelay;result.automatic=automatic;
    result.mode=unsigned(startup.Mode());
    result.samples=120;for(unsigned p=0;p<config.playerCount;++p){const auto& r=startup.ReportFor(p);
        result.rttP95Us=std::max(result.rttP95Us,r.p95Us);result.samples=std::min(result.samples,r.received);result.lost+=r.lost;}
    return result;
}
}
