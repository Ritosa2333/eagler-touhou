#pragma once
#include "animation/AnmScene.hpp"
#include <cmath>
#include <limits>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>
namespace th128::pause {
struct HelpDraw {unsigned id=0,parent=0;int bank=46;anm::State state;anm::SpriteInfo sprite;float x=0,y=0,z=0;};
class OriginalHelp {
    anm::Scene scene_;
    bool loaded_=false,active_=false,closing_=false;
    int selected_=0,page_=-1;
    unsigned age_=0;
    std::string failure_;
    std::vector<int> sounds_;
    float originX_=0;
    [[noreturn]] void fail(const std::string& message){if(failure_.empty())failure_=message;throw std::runtime_error(failure_);}
    void checked(){
        if(!failure_.empty())fail(failure_);
        for(const auto& [id,node]:scene_.nodes()){
            if(node->vm.faulted()||node->customGeometry||node->onInterrupt||node->beforeTick)fail("original help ANM owner failed");
            if(node->vm.state().deleteRequested||node->vm.state().ended)node->alive=false;
        }
        scene_.collect();
    }
    void bind(unsigned key,int script,unsigned owner){
        if(!scene_.bind(key,owner,46,script,anm::Membership::Primary,originX_,0,{},false,nullptr,0,0,31))fail("original help script binding failed");
        checked();
    }
    void topics(){
        for(unsigned index=0;index<9;++index){
            bind(100+index,int(index),1);
            scene_.interrupt(100+index,int(index)==selected_?2:3);
        }
    }
    void page(){
        for(unsigned index=0;index<9;++index)scene_.interrupt(100+index,1);
        scene_.remove(109);bind(109,9,2);page_=selected_;age_=0;
    }
public:
    void load(std::span<const std::uint8_t> bytes){
        if(loaded_||active_)fail("help bank replacement during active ownership");
        std::string error;if(!scene_.registry.loadBank(46,bytes.data(),bytes.size(),error))fail(error);
        const auto bank=scene_.registry.bank(46);
        if(!bank||bank->scripts.size()!=10||bank->sprites.size()!=10)fail("original help0..9 programs required");
        scene_.runtime.instructionBudget=4096;
        scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
        scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
        scene_.runtime.unsupported=[this](int,int,int,const char* reason){fail(reason);};
        loaded_=true;
    }
    bool loaded() const{return loaded_;}
    void open(float originX=0){if(!loaded_||active_)fail("help entry without available resource");scene_.reset();originX_=originX;sounds_.clear();active_=true;closing_=false;selected_=0;page_=-1;age_=0;topics();}
    void tick(std::uint32_t pressed){
        if(!active_)fail("help tick without active menu");
        scene_.tickPrimary();scene_.tickSecondary();checked();++age_;
        if(closing_){if(age_>=20)active_=false;return;}
        if(age_<20)return;
        if(page_<0){
            const auto before=selected_;
            if(pressed&0x10u)selected_=(selected_+8)%9;
            else if(pressed&0x20u)selected_=(selected_+1)%9;
            if(before!=selected_){
                scene_.interrupt(100+unsigned(before),3);scene_.interrupt(100+unsigned(selected_),2);sounds_.push_back(10);
            }
            if(pressed&0x102u){
                for(unsigned index=0;index<9;++index)scene_.interrupt(100+index,1);
                closing_=true;age_=0;sounds_.push_back(9);
            }else if(pressed&0x80201u){sounds_.push_back(7);page();}
        }else{
            if((pressed&0x20u)&&selected_<8){++selected_;sounds_.push_back(10);sounds_.push_back(7);page();}
            else if((pressed&0x10u)&&selected_>0){--selected_;sounds_.push_back(10);sounds_.push_back(7);page();}
            else if(pressed&0x80303u){scene_.interrupt(109,1);page_=-1;age_=0;topics();sounds_.push_back(9);}
        }
    }
    bool active() const{return active_;}
    int selection() const{return selected_;}
    int pageIndex() const{return page_;}
    const std::vector<int>& sounds()const{return sounds_;}
    std::vector<HelpDraw> draws()const{
        std::vector<HelpDraw> rows;const auto bank=scene_.registry.bank(46);if(!active_||!bank)return rows;
        for(const auto membership:{anm::Membership::Primary,anm::Membership::Secondary})for(const auto* node:scene_.chain(membership)){
            const auto& pose=node->vm.state();
            if(!node->alive||!pose.visible||!(pose.flags&2u)||pose.sprite<0||std::size_t(pose.sprite)>=bank->sprites.size())continue;
            rows.push_back({node->id,node->parent,46,pose,bank->sprites[std::size_t(pose.sprite)],pose.x+pose.engineX+pose.offsetX,pose.y+pose.engineY+pose.offsetY,pose.z+pose.engineZ+pose.offsetZ});
        }
        return rows;
    }
};
}
