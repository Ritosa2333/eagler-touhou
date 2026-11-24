#include "PerfectHud.hpp"
#include <limits>
#include <utility>

namespace th128::hud {
namespace {
struct Guard {bool& state;explicit Guard(bool& value):state(value){state=true;}~Guard(){state=false;}};
}
bool PerfectHud::fail(std::string reason,std::string& error){if(failure_.empty())failure_=std::move(reason);error=failure_;return false;}
void PerfectHud::runtime(){
    scene_.runtime={};scene_.runtime.instructionBudget=4096;
    scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.gameRandom32=[this]{if(failure_.empty())failure_="unproved HUD game RNG operation";return 0u;};
    scene_.runtime.displayRandom32=[this]{if(failure_.empty())failure_="unproved HUD display RNG operation";return 0u;};
    scene_.runtime.unsupported=[this](int,int script,int opcode,const char* reason){
        if(failure_.empty())failure_="front HUD script="+std::to_string(script)+" opcode="+std::to_string(opcode)+": "+reason;
    };
}
bool PerfectHud::checked(std::string& error){
    if(!failure_.empty())return fail(failure_,error);
    for(const auto& [id,node]:scene_.nodes()){
        if(node->vm.faulted()||node->detached||node->customGeometry)return fail("unimplemented HUD Scene geometry/callback",error);
        const auto& state=node->vm.state();const auto resource=scene_.registry.bank(state.spriteBank);
        if(state.sprite>=0&&(!resource||std::size_t(state.sprite)>=resource->sprites.size()||(state.spriteBank!=5&&state.spriteBank!=11)))return fail("HUD sprite binding unavailable",error);
        if(state.ended||state.deleteRequested)node->alive=false;
    }
    scene_.collect();return true;
}
bool PerfectHud::load(std::span<const std::uint8_t> bytes,std::string& error){
    error.clear();if(executing_||booted_){error="front HUD resource replacement requires a fresh host";return false;}
    anm::Registry candidate;
    if(!candidate.loadBank(5,bytes.data(),bytes.size(),error))return false;
    const auto bank=candidate.bank(5);
    if(!bank||bank->scripts.size()<=47){error="actual front0/47 scripts are required";return false;}
    scene_.registry=std::move(candidate);loaded_=true;runtime();return true;
}
bool PerfectHud::loadScoreFont(std::span<const std::uint8_t> bytes,std::string& error){
    error.clear();if(executing_||scene_.registry.bank(11)){error="score font replacement requires a fresh HUD font owner";return false;}
    return scene_.registry.loadBank(11,bytes.data(),bytes.size(),error);
}
bool PerfectHud::spellResult(int notice,int bonus,std::string& error){
    error.clear();if(!booted_||executing_||!failure_.empty()||notice<7||notice>9||bonus<0
        ||!scene_.registry.bank(11)){error="original spell result requires native notice and owning front/ascii banks";return false;}
    Guard guard(executing_);
    resultTimes_=false;resultSeconds_.reset();
    for(unsigned key=10;key<=21;++key)scene_.remove(key);scene_.collect();
    if(!scene_.bind(10,10,5,170+notice-7,anm::Membership::Primary,0,0,{},false,nullptr,0,0,23))return fail("original victory banner bind failed",error);
    if(notice!=9){
        unsigned divisor=10000000;bool significant=false;
        for(unsigned index=0;index<8;++index){
            const int digit=(unsigned(bonus)/divisor)%10;significant=significant||digit!=0||index==7;
            if(significant&&!scene_.bind(11+index,10,11,9+int(index),anm::Membership::Primary,0,0,[digit](int){return 243+digit;},false,nullptr,0,0,23))
                return fail("original spell bonus digit bind failed",error);
            divisor/=10;
        }
        // Decimal separators use sprite253 with their original delayed ASCII scripts.
        if(bonus>=1000000&&!scene_.bind(19,10,11,17,anm::Membership::Primary,0,0,[](int){return 253;},false,nullptr,0,0,23))
            return fail("original million separator bind failed",error);
        if(bonus>=1000&&!scene_.bind(20,10,11,18,anm::Membership::Primary,0,0,[](int){return 253;},false,nullptr,0,0,23))
            return fail("original thousand separator bind failed",error);
    }
    if(!scene_.bind(21,10,5,80,anm::Membership::Primary,0,0,{},false,nullptr,0,0,23))return fail("original victory time label bind failed",error);
    return checked(error);
}
bool PerfectHud::spellTimes(unsigned frames,std::optional<double> seconds,std::string& error){
    error.clear();if(!scene_.find(21)||frames>0x7fffffffu||(seconds&&(!std::isfinite(*seconds)||*seconds<0||*seconds>1000000000))){error="original spell result time context is invalid";return false;}
    resultTicks_=frames;resultSeconds_=seconds;resultTimes_=true;return true;
}
bool PerfectHud::boot(int value,std::string& error){
    error.clear();if(!loaded_||booted_||executing_||value<0||value>30000||!failure_.empty())return fail("invalid fresh front HUD startup",error);
    {Guard guard(executing_);
    if(!scene_.bind(1,1,5,0,anm::Membership::Primary,0,0,{},false,nullptr,0,0,23)||!checked(error))return fail(error.empty()?"front root0 binding failed":error,error);}
    booted_=true;return charge(value,error);
}
bool PerfectHud::rank(int value,std::string& error){
    error.clear();if(!booted_||executing_||rank_!=-1||value<0||value>4){error="invalid fresh original rank HUD binding";return false;}
    Guard guard(executing_);
    if(!scene_.bind(3,3,5,66+value,anm::Membership::Primary,0,0,{},false,nullptr,0,0,23))return fail("original rank66..70 binding failed",error);
    scene_.interrupt(3,3,true);
    if(!checked(error))return false;
    rank_=value;return true;
}
bool PerfectHud::charge(int value,std::string& error){
    error.clear();if(value<0||value>30000){error="invalid bounded HUD charge";return false;}
    if(!booted_||executing_||!failure_.empty())return fail(failure_.empty()?"unavailable HUD charge operation":failure_,error);
    const auto* root=scene_.find(1);if(!root)return fail("required front root is unavailable",error);
    std::array<unsigned,3> found{};
    for(const auto* child:scene_.children(root->id)){
        const int script=child->vm.state().script;
        if(script<12||script>14)continue;
        ++found[std::size_t(script-12)];
        auto& state=scene_.nodes().at(child->id)->vm.control();
        if(value>=(script-11)*10000)state.flags|=2u;else state.flags&=~2u;
    }
    if(found!=std::array<unsigned,3>{1,1,1})return fail("required native charge child12/13/14 is missing or ambiguous",error);
    return true;
}
bool PerfectHud::notice(std::string& error){return replaceNotice(47,error);}
bool PerfectHud::motivationNotice(std::string& error){return replaceNotice(49,error);}
bool PerfectHud::replaceNotice(int script,std::string& error){
    error.clear();if(!booted_||executing_||!failure_.empty())return fail(failure_.empty()?"unavailable front notification":failure_,error);
    // Original 0x424d6c/type2 and 0x424dca/type4 replace the same field +0x588c.
    Guard guard(executing_);scene_.remove(2);scene_.collect();
    if(!scene_.bind(2,2,5,script,anm::Membership::Primary,0,0,{},false,nullptr,0,0,23))return fail("required front"+std::to_string(script)+" notification bind failed",error);
    return checked(error);
}
bool PerfectHud::tick(std::string& error){
    error.clear();if(!booted_||executing_||!failure_.empty())return fail(failure_.empty()?"unavailable front primary pass":failure_,error);
    Guard guard(executing_);scene_.tickPrimary();return checked(error);
}
std::vector<Snapshot> PerfectHud::snapshots() const {
    std::vector<Snapshot> result;if(!scene_.registry.bank(5))return result;
    for(const auto* node:scene_.chain(anm::Membership::Primary)){
        const auto& state=node->vm.state();const auto bank=scene_.registry.bank(state.spriteBank);std::optional<anm::SpriteInfo> sprite;
        if(bank&&state.sprite>=0&&std::size_t(state.sprite)<bank->sprites.size())sprite=bank->sprites[std::size_t(state.sprite)];
        float x=state.x+state.engineX+state.offsetX,y=state.y+state.engineY+state.offsetY,z=state.z+state.engineZ+state.offsetZ;
        if(node->parent){const auto parent=scene_.nodes().find(node->parent);if(parent!=scene_.nodes().end()){
            const auto& owner=parent->second->vm.state();x+=owner.x+owner.engineX+owner.offsetX;y+=owner.y+owner.engineY+owner.offsetY;z+=owner.z+owner.engineZ+owner.offsetZ;
        }}
        result.push_back({node->id,node->parent,state,sprite,x,y,z});
    }
    if(resultTimes_){
        const auto found=std::find_if(result.begin(),result.end(),[](const auto& r){return r.state.bank==5&&r.state.script==80;});
        const auto* label=found==result.end()?nullptr:&*found;const auto font=scene_.registry.bank(11);
        if(label&&font&&font->sprites.size()>256){
            const auto alpha=label->state.primary&0xff000000u;unsigned index=0;
            const auto append=[&](std::string text,float x,float y,float scale,std::uint32_t color){
                for(const unsigned char code:text){
                    if(code!=' '){const int sprite=code=='.'?254:code=='s'?256:int(code)+195;
                        Snapshot glyph;glyph.id=500000+index++;glyph.state.bank=glyph.state.spriteBank=11;
                        glyph.state.sprite=sprite;glyph.state.script=-1;glyph.state.visible=true;glyph.state.layer=30;glyph.state.mode=0;
                        glyph.state.anchorX=glyph.state.anchorY=1;glyph.state.sx=glyph.state.sy=scale;glyph.state.flags2|=4u;glyph.state.primary=alpha|(color&0xffffffu);
                        glyph.sprite=font->sprites[std::size_t(sprite)];glyph.x=x;glyph.y=y;result.push_back(glyph);}
                    x+=14*scale;
                }
            };
            const auto line=[&](unsigned seconds,unsigned fraction,float y,std::uint32_t color){
                auto integer=std::to_string(std::min(seconds,999u));integer.insert(0,3-integer.size(),' ');integer+='.';
                auto decimal=std::to_string(fraction%100);if(decimal.size()<2)decimal.insert(0,1,'0');decimal+='s';
                append(integer,318,y,1,color);append(decimal,368,y+6,.6000000238418579f,color);
            };
            if(alpha){line(resultTicks_/60,(resultTicks_%60)*100/60,240,0xffffffu);
                if(resultSeconds_){const double seconds=*resultSeconds_;line(unsigned(std::min(seconds,999.)),unsigned(std::floor((seconds-std::floor(seconds))*100)),256,0x808080u);}
            }
        }
    }
    return result;
}
}
