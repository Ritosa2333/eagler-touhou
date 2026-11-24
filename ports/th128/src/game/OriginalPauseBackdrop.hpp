#pragma once
#include "animation/AnmScene.hpp"
#include <array>
#include <cmath>
#include <limits>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>
namespace th128::pause {
struct OriginalBackdropCapture {
    std::uint64_t epoch=0,sourceFrame=0;
    int bank=6,entry=1,width=512,height=512,format=3,filter=2;
    std::array<int,4> source{128,16,384,448},destination{0,0,128,128};
};
struct OriginalBackdropDraw {
    unsigned id=0,parent=0;
    int bank=6,priority=70;
    anm::State state;
    anm::SpriteInfo sprite;
    float x=0,y=0,z=0;
};
class OriginalPauseBackdrop {
    anm::Scene scene_;
    OriginalBackdropCapture capture_;
    bool loaded_=false,active_=false,closing_=false;
    unsigned ticks_=0;
    std::string failure_;
    static constexpr unsigned Root=1,Shade=2;
    [[noreturn]] void fail(const std::string& reason){if(failure_.empty())failure_=reason;throw std::runtime_error(failure_);}
    void checked(){
        if(!failure_.empty())fail(failure_);
        const auto bank=scene_.registry.bank(6);
        for(const auto& [identity,node]:scene_.nodes()){
            const auto& pose=node->vm.state();
            if(node->vm.faulted()||node->customGeometry||node->beforeTick||node->onInterrupt)
                fail("unowned original backdrop ANM callback or opcode");
            if(pose.bank!=6||pose.spriteBank!=6||pose.sprite<0||!bank||std::size_t(pose.sprite)>=bank->sprites.size())
                fail("original backdrop text sprite binding unavailable");
            if(pose.deleteRequested||pose.ended)node->alive=false;
        }
        scene_.collect();active_=scene_.find(Root)!=nullptr;
    }
    bool compose(const anm::Node& node,OriginalBackdropDraw& draw,unsigned depth=0)const{
        if(depth>=64)return false;
        const auto& pose=node.vm.state();draw.state=pose;
        draw.x=pose.x+pose.engineX+pose.offsetX;draw.y=pose.y+pose.engineY+pose.offsetY;draw.z=pose.z+pose.engineZ+pose.offsetZ;
        if(node.parent){
            const auto parent=scene_.nodes().find(node.parent);OriginalBackdropDraw inherited;
            if(parent==scene_.nodes().end()||!compose(*parent->second,inherited,depth+1))return false;
            draw.x+=inherited.x;draw.y+=inherited.y;draw.z+=inherited.z;
            draw.state.sx*=inherited.state.sx;draw.state.sy*=inherited.state.sy;
            if(pose.mode==1)draw.state.rotation+=inherited.state.rotation;
        }
        return std::isfinite(draw.x)&&std::isfinite(draw.y)&&std::isfinite(draw.z);
    }
public:
    void load(std::span<const std::uint8_t> bytes){
        if(loaded_||active_)throw std::runtime_error("backdrop resource owner already loaded");
        std::string error;if(!scene_.registry.loadBank(6,bytes.data(),bytes.size(),error))fail(error);
        const auto bank=scene_.registry.bank(6);
        if(!bank||bank->scripts.size()<=82||bank->sprites.size()<=83
            ||bank->sprites[79].textureEntry!=1||bank->sprites[79].width!=128||bank->sprites[79].height!=128
            ||bank->sprites[80].textureEntry!=1||bank->sprites[80].width!=384||bank->sprites[80].height!=448
            ||bank->sprites[83].textureEntry!=2||bank->sprites[83].width!=384||bank->sprites[83].height!=448)
            fail("original text75 backdrop and text82 shade required");
        scene_.runtime.instructionBudget=4096;
        scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
        scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
        scene_.runtime.gameRandom32=[this]{fail("unowned backdrop game RNG");return 0u;};
        scene_.runtime.displayRandom32=[this]{fail("unowned backdrop display RNG");return 0u;};
        scene_.runtime.unsupported=[this](int,int script,int opcode,const char* reason){
            fail("backdrop script="+std::to_string(script)+" opcode="+std::to_string(opcode)+": "+reason);
        };
        loaded_=true;
    }
    void open(std::uint64_t epoch,std::uint64_t sourceFrame){
        if(!loaded_||active_||!epoch||epoch<=capture_.epoch)throw std::runtime_error("backdrop entry requires fresh exclusive epoch");
        if(!failure_.empty())fail(failure_);
        scene_.reset();capture_={};capture_.epoch=epoch;capture_.sourceFrame=sourceFrame;ticks_=0;closing_=false;
        if(!scene_.bind(Shade,2,6,82,anm::Membership::Embedded,0,0,{},false,nullptr,0,0,29)
            ||!scene_.bind(Root,1,6,75,anm::Membership::Primary,0,0,{},false,nullptr,0,0,32))
            fail("original text backdrop constructor rejected");
        checked();
    }
    void tick(){
        if(!active_)throw std::runtime_error("backdrop tick without active owner");
        scene_.tickPrimary();scene_.tickSecondary();++ticks_;checked();
    }
    void close(){
        if(!active_||closing_)throw std::runtime_error("backdrop IRQ1 requires active nonclosing owner");
        scene_.interrupt(Root,1);closing_=true;
    }
    void commit(){scene_.reset();active_=false;closing_=false;}
    bool active()const{return active_;}
    bool closing()const{return closing_;}
    unsigned ticks()const{return ticks_;}
    const OriginalBackdropCapture& capture()const{return capture_;}
    std::vector<OriginalBackdropDraw> draws()const{
        std::vector<OriginalBackdropDraw> rows;if(!active_)return rows;
        const auto bank=scene_.registry.bank(6);
        for(const auto membership:{anm::Membership::Primary,anm::Membership::Secondary})
            for(const auto* node:scene_.chain(membership)){
                const auto& pose=node->vm.state();
                if(!node->alive||!pose.visible||!(pose.flags&2u)||pose.deleteRequested||pose.sprite<0)continue;
                OriginalBackdropDraw draw;draw.id=node->id;draw.parent=node->parent;draw.sprite=bank->sprites[std::size_t(pose.sprite)];
                if(compose(*node,draw))rows.push_back(draw);
            }
        return rows;
    }
    OriginalBackdropDraw shade()const{
        if(!loaded_)throw std::runtime_error("shade without original text resource");
        const auto bank=scene_.registry.bank(6);OriginalBackdropDraw draw;bool found=false;std::uint32_t color=0xffffffffu;
        for(const auto& [identity,node]:scene_.nodes())if(node->alive){
            const auto& pose=node->vm.state();
            if(pose.script==80)color=pose.primary|0xff000000u;
            if(pose.script==82){
                draw.id=node->id;draw.parent=node->parent;draw.sprite=bank->sprites[std::size_t(pose.sprite)];
                found=compose(*node,draw);draw.priority=52;
            }
        }
        if(!found)throw std::runtime_error("separate shade82 owner unavailable");
        draw.state.primary=color;return draw;
    }
};
}
