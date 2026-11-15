#pragma once
#include "animation/AnmScene.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <limits>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace th128::laser::cancel {
struct OwnedWholeLine {
    std::uint32_t vtable=0x4a2c1c,state=2,guard5b4=0;
    int type=20,color=6;
    float x=0,y=0,z=0,angle=0,length=0;
};
struct ColoredEffectRequest {float x=0,y=0,z=0;int script=16,layer=23;};
enum class WholeLineResult { GuardedUnchanged, RetiredWithoutEffects, RetiredAfterOwnedEffects };
struct WholeLineCompletion {WholeLineResult disposition;std::uint32_t roots=0;};

class LaserWholeLineEffectsOwner {
public:
    LaserWholeLineEffectsOwner()=default;
    LaserWholeLineEffectsOwner(const LaserWholeLineEffectsOwner&)=delete;
    LaserWholeLineEffectsOwner& operator=(const LaserWholeLineEffectsOwner&)=delete;
    LaserWholeLineEffectsOwner(LaserWholeLineEffectsOwner&&)=delete;
    LaserWholeLineEffectsOwner& operator=(LaserWholeLineEffectsOwner&&)=delete;

    bool configure(const void* bytes,std::size_t size,std::function<std::uint32_t()> gameRandom32,std::string& error){
        error.clear();
        if(configured_||executing_||!bytes||!size||size>8u*1024u*1024u||!gameRandom32){error="laser colored FX requires a fresh real asset/RNG owner";return false;}
        anm::Registry candidate;
        if(!candidate.loadBank(0,static_cast<const std::uint8_t*>(bytes),size,error))return false;
        const auto bank=candidate.bank(0);
        if(!bank||!programs(*bank)){error="laser color6 requires exact original scripts15/16 and sprite342";return false;}
        scene_.registry=std::move(candidate);random_=std::move(gameRandom32);
        scene_.runtime.instructionBudget=4096;
        scene_.runtime.gameRandom32=[this]{return random_();};
        scene_.runtime.unsupported=[this](int,int script,int opcode,const char* why){
            if(failure_.empty())failure_="laser FX script="+std::to_string(script)+" opcode="+std::to_string(opcode)+": "+why;
        };
        configured_=true;return true;
    }

    static std::optional<std::vector<ColoredEffectRequest>> prepare(const OwnedWholeLine& line,std::string& error){
        error.clear();
        if(line.vtable!=0x4a2c1c||line.state!=2||line.type!=20||line.color!=6
            ||!std::isfinite(line.length)||line.length<0||line.length>160
            ||!std::isfinite(line.angle)||std::abs(line.angle)>3.1415927410125732f
            ||!std::isfinite(line.x)||!std::isfinite(line.y)||!std::isfinite(line.z)
            ||std::abs(line.x)>65536||std::abs(line.y)>65536||std::abs(line.z)>65536){
            error="laser colored FX is outside proven original type0/type20/color6/max128 domain";return std::nullopt;
        }
        std::vector<ColoredEffectRequest> result;
        if(line.length<=16)return result;
        const float directionX=float(std::cos(double(line.angle))*8.);
        const float directionY=float(std::sin(double(line.angle))*8.);
        const float stepX=float(double(directionX)*2.),stepY=float(double(directionY)*2.);
        float horizontal=float(double(line.x)+directionX),vertical=float(double(line.y)+directionY);
        float distance=8;
        do{
            if(result.size()>=9){error="laser colored FX original sample budget";return std::nullopt;}
            result.push_back({horizontal,vertical,line.z,16,23});
            horizontal=float(double(horizontal)+stepX);vertical=float(double(vertical)+stepY);
            distance=float(double(distance)+16.);
        }while(double(line.length)>double(distance)+8.);
        return result;
    }

    std::optional<WholeLineCompletion> cancel(OwnedWholeLine& line,bool honorGuard,std::string& error){
        error.clear();
        if(!configured_||executing_||!failure_.empty()){error=failure_.empty()?"laser FX owner is not ready":failure_;return std::nullopt;}
        const auto requests=prepare(line,error);if(!requests)return std::nullopt;
        if(honorGuard&&line.guard5b4)return WholeLineCompletion{WholeLineResult::GuardedUnchanged,0};
        if(requests->empty()){line.state=1;return WholeLineCompletion{WholeLineResult::RetiredWithoutEffects,0};}
        if(!emitOwnedSamples(*requests,error))return std::nullopt;
        line.state=1;
        return WholeLineCompletion{WholeLineResult::RetiredAfterOwnedEffects,std::uint32_t(requests->size())};
    }

    bool emitOwnedSamples(const std::vector<ColoredEffectRequest>& requests,std::string& error){
        error.clear();
        if(!configured_||executing_||!failure_.empty()){error=failure_.empty()?"laser FX owner is not ready":failure_;return false;}
        if(requests.size()>10){error="native circle/whole FX request budget";return false;}
        for(const auto& request:requests)if(request.script!=16||request.layer!=23||!std::isfinite(request.x)
            ||!std::isfinite(request.y)||request.z!=0||std::abs(request.x)>65536||std::abs(request.y)>65536){
            error="native circle/whole colored effect sample is not owned";return false;
        }
        if(requests.empty())return true;
        if(scene_.nodes().size()+requests.size()*2>512||nextOwner_>std::numeric_limits<std::uint32_t>::max()-requests.size()){
            error="laser FX preflight primary-node/owner budget";return false;
        }
        std::vector<std::uint32_t> pending;
        pending.reserve(requests.size());
        executing_=true;
        try{
            for(const auto& request:requests){
                const auto owner=nextOwner_++;pending.push_back(owner);
                const auto* node=scene_.bind(owner,owner,0,request.script,anm::Membership::Primary,
                    float(double(request.x)+320.),float(double(request.y)+16.),{},false,nullptr,0,0,request.layer,false,request.z);
                if(!node||node->vm.faulted()||!failure_.empty()){
                    if(failure_.empty())failure_="laser colored FX real Scene bind failed";
                    for(const auto created:pending)scene_.removeOwner(created);
                    scene_.collect();executing_=false;error=failure_;return false;
                }
            }
        }catch(...){
            for(const auto created:pending)scene_.removeOwner(created);
            scene_.collect();executing_=false;failure_="laser colored FX owner threw after preflight";error=failure_;return false;
        }
        executing_=false;
        return true;
    }

    bool tickPrimary(std::string& error){
        error.clear();
        if(!configured_||executing_||!failure_.empty()){error=failure_.empty()?"laser FX owner is not ready":failure_;return false;}
        executing_=true;
        try{scene_.tickPrimary();scene_.collect();}
        catch(...){failure_="laser colored FX tick threw";}
        executing_=false;
        if(!failure_.empty()){error=failure_;return false;}
        return true;
    }
    const anm::Scene& scene() const{return scene_;}
private:
    static bool command(const anm::Command& actual,int opcode,int time,unsigned mask,std::initializer_list<std::uint32_t> words){
        return actual.op==opcode&&actual.time==time&&actual.mask==mask
            &&actual.words.size()==words.size()&&std::equal(actual.words.begin(),actual.words.end(),words.begin());
    }
    static bool programs(const anm::Bank& bank){
        if(bank.scripts.size()<=16||bank.sprites.size()<=342)return false;
        if(bank.scripts[15].commands.size()!=16)return false;
        if(!command(bank.scripts[15].commands[0],67,0,0,{1u}))return false;
        if(!command(bank.scripts[15].commands[1],66,0,0,{1u}))return false;
        if(!command(bank.scripts[15].commands[2],68,0,0,{10u}))return false;
        if(!command(bank.scripts[15].commands[3],3,0,0,{342u}))return false;
        if(!command(bank.scripts[15].commands[4],51,0,0,{0u}))return false;
        if(!command(bank.scripts[15].commands[5],23,0,3,{1176260608u,1176268800u,1094713344u}))return false;
        if(!command(bank.scripts[15].commands[6],23,0,3,{1176261632u,1176268800u,1094713344u}))return false;
        if(!command(bank.scripts[15].commands[7],11,0,1,{1176261632u,1103101952u}))return false;
        if(!command(bank.scripts[15].commands[8],49,0,4,{0u,0u,1176266752u}))return false;
        if(!command(bank.scripts[15].commands[9],51,0,0,{128u}))return false;
        if(!command(bank.scripts[15].commands[10],56,0,12,{28u,0u,1176260608u,1176261632u,0u}))return false;
        if(!command(bank.scripts[15].commands[11],60,0,0,{14u,4u,1072902963u,1072902963u}))return false;
        if(!command(bank.scripts[15].commands[12],51,8,0,{20u}))return false;
        if(!command(bank.scripts[15].commands[13],60,14,0,{14u,1u,1061997773u,1061997773u}))return false;
        if(!command(bank.scripts[15].commands[14],1,28,0,{}))return false;
        if(!command(bank.scripts[15].commands[15],-1,0,0,{}))return false;
        if(bank.scripts[16].commands.size()!=4)return false;
        if(!command(bank.scripts[16].commands[0],51,0,0,{0u}))return false;
        if(!command(bank.scripts[16].commands[1],88,0,0,{15u}))return false;
        if(!command(bank.scripts[16].commands[2],1,28,0,{}))return false;
        if(!command(bank.scripts[16].commands[3],-1,0,0,{}))return false;
        return true;
    }
    anm::Scene scene_;
    std::function<std::uint32_t()> random_;
    std::string failure_;
    std::uint32_t nextOwner_=1;
    bool configured_=false,executing_=false;
};
}
