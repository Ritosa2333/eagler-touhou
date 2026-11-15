#pragma once
#include "animation/AnmScene.hpp"
#include "game/OriginalHelpHost.hpp"
#include "game/OriginalPauseBackdrop.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace th128::pause {
enum class Kind {Pause,GameOver,Replay};
enum class Action {None,Resume,Continue,Title,ReplaySave,Manual,Restart};
struct Input {std::uint32_t held=0,pressed=0;};
struct Resources {int score=0,motivation=0,motivationFlag=0,perfect=0,continues=0;};
inline Resources continueResources(Resources value){
    if(value.score<0||value.score>999999999||value.perfect<0||value.perfect>30000||value.continues<0||value.continues>9)
        throw std::runtime_error("invalid original continue resources");
    value.score=0;value.motivation=20000;value.motivationFlag=0;value.continues=std::min(value.continues+1,9);
    return value;
}
struct State {
    Kind kind=Kind::Pause;Action action=Action::None;
    bool loaded=false,active=false,ready=false,confirm=false,closing=false,manual=false;
    int helpSelection=0,helpPage=-1;
    int selection=0,rows=5,confirmSelection=0;
    unsigned ticks=0,serial=0;
    std::array<bool,5> disabled{};
};
struct Draw {unsigned id=0,parent=0;int bank=5;anm::State state;anm::SpriteInfo sprite;float x=0,y=0,z=0;};
class Host {
    anm::Scene scene_;
    OriginalHelp help_;
    OriginalPauseBackdrop backdrop_;
    bool backdropLoaded_=false;
    unsigned helpSoundCursor_=0;
    State state_;
    std::vector<int> sounds_;
    std::string failure_;
    unsigned age_=0;
    bool executing_=false,confirmationLeaving_=false,confirmationAccepted_=false,helpEntering_=false;
    Action requested_=Action::None;
    static constexpr unsigned Root=1;
    [[noreturn]] void fail(const std::string& reason){if(failure_.empty())failure_=reason;throw std::runtime_error(failure_);}
    void checked(){
        if(!failure_.empty())throw std::runtime_error(failure_);
        const auto bank=scene_.registry.bank(5);
        for(const auto& [identity,node]:scene_.nodes()){
            const auto& pose=node->vm.state();
            if(node->vm.faulted()||node->customGeometry||node->beforeTick||node->onInterrupt)
                fail("unimplemented original pause ANM callback or instruction");
            if(pose.sprite>=0&&(!bank||pose.spriteBank!=5||std::size_t(pose.sprite)>=bank->sprites.size()))
                fail("original pause sprite binding unavailable");
            if(pose.deleteRequested||pose.ended)node->alive=false;
        }
        scene_.collect();
    }
    void sound(int identifier){sounds_.push_back(identifier);}
    void signal(int code){scene_.interrupt(Root,code);checked();}
    Action selectedAction() const{
        if(state_.selection==0)return state_.kind==Kind::GameOver?Action::Continue:Action::Resume;
        if(state_.selection==1)return Action::Title;
        if(state_.selection==2)return state_.kind==Kind::Replay?Action::Restart:Action::ReplaySave;
        if(state_.selection==3)return Action::Manual;
        if(state_.selection==4)return Action::Restart;
        throw std::runtime_error("unowned original pause selection");
    }
    void select(int delta){
        const int before=state_.selection;
        for(int count=0;count<state_.rows;++count){
            state_.selection=(state_.selection+delta+state_.rows)%state_.rows;
            if(!state_.disabled[std::size_t(state_.selection)])break;
        }
        if(before!=state_.selection){signal(7+state_.selection);sound(10);}
    }
    void beginClose(Action action){
        if(backdropLoaded_&&backdrop_.active()&&!backdrop_.closing()&&action!=Action::ReplaySave)backdrop_.close();
        confirmationLeaving_=false;requested_=action;state_.closing=true;state_.ready=false;state_.confirm=false;age_=0;
        if(action==Action::ReplaySave&&state_.kind==Kind::GameOver){
            for(const auto& [identity,node]:scene_.nodes())if(node->alive&&node->vm.state().script==106)scene_.interruptNode(identity,6);
            scene_.drawEnabled(Root,false);state_.action=action;
        }
        else signal(1);
    }
    void leaveConfirm(bool accepted){
        bool found=false;
        for(const auto& [identity,node]:scene_.nodes())if(node->alive&&node->vm.state().script==(accepted?97:98)){
            scene_.interruptNode(identity,6);found=true;
        }
        if(!found)fail("original confirmation choice glyph owner unavailable");
        checked();confirmationLeaving_=true;confirmationAccepted_=accepted;
        state_.confirm=false;state_.ready=false;age_=0;
    }
    void beginConfirm(){
        state_.confirm=true;state_.confirmSelection=1;state_.ready=false;age_=0;
    }
    void beginHelp(){
        if(!help_.loaded())fail("original manual resource owner unavailable");
        const int script=state_.kind==Kind::GameOver?107:121;bool found=false;
        for(const auto& [identity,node]:scene_.nodes())if(node->alive&&node->vm.state().script==script){
            scene_.interruptNode(identity,6);found=true;
        }
        if(!found)fail("original selected manual glyph owner unavailable");
        checked();helpEntering_=true;state_.ready=false;age_=0;
    }
    bool compose(const anm::Node& node,Draw& draw,unsigned depth=0) const{
        if(depth>=64)return false;
        const auto& pose=node.vm.state();draw.state=pose;
        draw.x=pose.x+pose.engineX+pose.offsetX;draw.y=pose.y+pose.engineY+pose.offsetY;draw.z=pose.z+pose.engineZ+pose.offsetZ;
        if(node.parent){
            const auto found=scene_.nodes().find(node.parent);Draw parent;
            if(found==scene_.nodes().end()||!compose(*found->second,parent,depth+1))return false;
            draw.x+=parent.x;draw.y+=parent.y;draw.z+=parent.z;
            draw.state.sx*=parent.state.sx;draw.state.sy*=parent.state.sy;
            if(pose.mode==1)draw.state.rotation+=parent.state.rotation;
        }
        return std::isfinite(draw.x)&&std::isfinite(draw.y)&&std::isfinite(draw.z);
    }
public:
    void load(std::span<const std::uint8_t> bytes){
        if(state_.active||executing_||state_.loaded)fail("original pause bank requires a fresh host");
        std::string error;
        if(!scene_.registry.loadBank(5,bytes.data(),bytes.size(),error))fail(error);
        const auto bank=scene_.registry.bank(5);
        if(!bank||bank->scripts.size()<=121)fail("original front pause99 and GameOver109 scripts required");
        scene_.runtime.instructionBudget=4096;
        scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
        scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
        scene_.runtime.gameRandom32=[this]{fail("unowned pause game RNG");return 0u;};
        scene_.runtime.displayRandom32=[this]{fail("unowned pause display RNG");return 0u;};
        scene_.runtime.unsupported=[this](int,int script,int opcode,const char* reason){fail("pause script="+std::to_string(script)+" opcode="+std::to_string(opcode)+": "+reason);};
        state_.loaded=true;
    }
    void loadManual(std::span<const std::uint8_t> bytes){if(state_.active||executing_)fail("manual bank requires a fresh game owner");help_.load(bytes);}
    void loadBackdrop(std::span<const std::uint8_t> bytes){if(state_.active||executing_||backdropLoaded_)fail("backdrop resource requires fresh pause owner");backdrop_.load(bytes);backdropLoaded_=true;}
    bool backdropLoaded()const{return backdropLoaded_;}
    const OriginalPauseBackdrop& backdrop()const{return backdrop_;}
    void open(bool gameOver,int continues,bool replayAvailable,std::uint64_t sourceFrame=0,bool playback=false){
        if(!state_.loaded||state_.active||executing_||continues<0||continues>9)fail("invalid original pause entry");
        const auto serial=state_.serial+1;state_={};state_.loaded=true;state_.active=true;state_.serial=serial;
        state_.kind=playback?Kind::Replay:gameOver?Kind::GameOver:Kind::Pause;
        state_.rows=playback?3:5;
        state_.disabled[0]=gameOver&&continues>=5;
        state_.disabled[2]=!playback&&(!replayAvailable||continues>0);
        state_.selection=state_.disabled[0]?1:0;
        age_=0;requested_=Action::None;confirmationLeaving_=false;confirmationAccepted_=false;helpEntering_=false;sounds_.clear();scene_.reset();
        if(backdropLoaded_)backdrop_.open(serial,sourceFrame);
        if(!gameOver){
            if(!scene_.bind(Root,1,5,playback?100:99,anm::Membership::Primary,0,0,{},false,nullptr,0,0,25))
                fail("original pause constructor rejected");
            signal(3);sound(14);
        }
        checked();
    }
    void tick(Input input){
        if(!state_.active||executing_)fail("original pause tick without active owner");
        if(!failure_.empty())fail(failure_);
        executing_=true;
        const auto navigation=input.pressed|(input.held&0xf0u);
        try{
            scene_.tickPrimary();scene_.tickSecondary();checked();++state_.ticks;++age_;
            if(backdropLoaded_&&backdrop_.active())backdrop_.tick();
            if(state_.manual){
                help_.tick(navigation);
                while(helpSoundCursor_<help_.sounds().size())sound(help_.sounds()[helpSoundCursor_++]);
                state_.helpSelection=help_.selection();state_.helpPage=help_.pageIndex();
                if(!help_.active()){state_.manual=false;state_.ready=true;state_.helpPage=-1;scene_.drawEnabled(Root,true);}
            }else if(helpEntering_){
                if(age_==20){
                    scene_.drawEnabled(Root,false);help_.open();helpSoundCursor_=0;
                    state_.manual=true;state_.helpPage=-1;state_.helpSelection=0;helpEntering_=false;
                }
            }else if(state_.closing){
                if(age_>=12)state_.action=requested_;
            }else if(confirmationLeaving_){
                if(age_>=20){
                    confirmationLeaving_=false;
                    if(confirmationAccepted_){
                        const auto action=selectedAction();
                        if(action==Action::ReplaySave){
                            requested_=action;state_.closing=true;state_.action=action;age_=0;scene_.drawEnabled(Root,false);
                        }else beginClose(action);
                    }else{signal(7+state_.selection);state_.ready=true;age_=0;}
                }
            }else if(state_.confirm&&!state_.ready){
                if(age_==20)signal(14);
                if(age_>=30){signal(15+state_.confirmSelection);state_.ready=true;}
            }else if(!state_.ready){
                if(age_>=10){
                    if(state_.kind==Kind::GameOver){
                        if(!scene_.bind(Root,1,5,109,anm::Membership::Primary,0,0,{},false,nullptr,0,0,25))
                            fail("original GameOver constructor rejected");
                        signal(3);
                    }
                    signal(7+state_.selection);state_.ready=true;
                }
            }else if(state_.confirm){
                if(navigation&0x30u){state_.confirmSelection^=1;signal(15+state_.confirmSelection);sound(10);}
                if(input.pressed&0x100u){sound(9);beginClose(Action::Resume);}
                else if(input.pressed&2u){
                    sound(9);
                    if(state_.confirmSelection==0){state_.confirmSelection=1;signal(16);}
                    else leaveConfirm(false);
                }else if(input.pressed&0x80201u){
                    const bool accepted=state_.confirmSelection==0;sound(accepted?7:9);leaveConfirm(accepted);
                }
            }else{
                if(navigation&0x10u)select(-1);
                else if(navigation&0x20u)select(1);
                if((input.pressed&0x102u)&&state_.kind!=Kind::GameOver){sound(7);beginClose(Action::Resume);}
                else if(input.pressed&0x80201u){
                    if(state_.disabled[std::size_t(state_.selection)])sound(9);
                    else{
                        sound(7);const auto action=selectedAction();
                        if(action==Action::Manual)beginHelp();
                        else if(action==Action::Resume||action==Action::Continue||state_.kind==Kind::GameOver)beginClose(action);
                        else beginConfirm();
                    }
                }
            }
        }catch(...){executing_=false;throw;}
        executing_=false;
    }
    void replayPresentationTick(){
        if(!state_.active||state_.action!=Action::ReplaySave||!state_.closing||executing_)
            throw std::runtime_error("hidden replay presentation requires its active front owner");
        executing_=true;
        try{scene_.tickPrimary();scene_.tickSecondary();checked();++state_.ticks;if(backdropLoaded_&&backdrop_.active())backdrop_.tick();}
        catch(...){executing_=false;throw;}
        executing_=false;
    }
    void returnFromReplay(){
        if(!state_.active||state_.kind!=Kind::GameOver||state_.action!=Action::ReplaySave||!state_.closing||executing_)
            throw std::runtime_error("replay return requires the retained GameOver front owner");
        scene_.drawEnabled(Root,true);signal(7+state_.selection);
        state_.action=Action::None;state_.closing=false;state_.ready=true;
        requested_=Action::None;age_=0;
    }
    void commit(Action action){
        if(!state_.active||state_.action==Action::None||state_.action!=action||executing_)throw std::runtime_error("unowned original pause action commit");
        state_.active=false;state_.ready=false;state_.action=Action::None;state_.closing=false;scene_.reset();if(backdropLoaded_)backdrop_.commit();
    }
    const State& state() const{return state_;}
    const std::vector<int>& sounds() const{return sounds_;}
    const std::string& error() const{return failure_;}
    std::vector<Draw> draws() const{
        std::vector<Draw> result;const auto bank=scene_.registry.bank(5);
        if(!state_.active||!bank)return result;
        if(state_.manual){for(const auto& row:help_.draws())result.push_back({row.id,row.parent,row.bank,row.state,row.sprite,row.x,row.y,row.z});return result;}
        for(const auto membership:{anm::Membership::Primary,anm::Membership::Secondary})
            for(const auto* node:scene_.chain(membership)){
                const auto& pose=node->vm.state();
                if(!node->alive||!pose.visible||!(pose.flags&2u)||pose.deleteRequested||pose.sprite<0||std::size_t(pose.sprite)>=bank->sprites.size())continue;
                Draw draw;draw.id=node->id;draw.parent=node->parent;draw.sprite=bank->sprites[std::size_t(pose.sprite)];
                if(compose(*node,draw))result.push_back(draw);
            }
        return result;
    }
};
}
