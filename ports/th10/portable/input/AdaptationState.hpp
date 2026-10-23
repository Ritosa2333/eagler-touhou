#pragma once
#include <cstdint>

namespace th10::adaptation {
using Word=std::uint32_t;

// The ordinary stage keeps its real `main` enemy: Player::update requires a
// nonempty enemy manager before it consumes Shoot. This gate stops that owner
// before EnemyState::update, including the initial call made by spawn().
struct ControllerIdentity {
    Word owner=0,program=0,instruction=0,mainInstruction=0,timeBits=0,stackTop=0;
};
struct SceneGate {
    Word status=0,generation=0,blockedCalls=0,executedCalls=0,unexpectedOwners=0;
    bool pending=false,active=false,firstBlocked=false,updateStarted=false,spawningMain=false;
    ControllerIdentity initial{};
    bool arm(bool eligible) noexcept {
        if(!eligible||status||pending||active)return false;
        ++generation;status=1;pending=true;return true;
    }
    void begin(bool eligible) noexcept {
        if(!pending)return;
        pending=false;status=eligible?2:3;active=eligible;
    }
    bool intercept(const ControllerIdentity& actual,Word managerCount) noexcept {
        if(!active)return false;
        updateStarted=true;++blockedCalls;
        if(!initial.owner){
            if(!spawningMain||managerCount||!actual.owner||!actual.program||
               !actual.instruction||actual.instruction!=actual.mainInstruction||actual.timeBits||actual.stackTop){
                ++unexpectedOwners;return true;
            }
            initial=actual;firstBlocked=true;
        }else if(actual.owner!=initial.owner||actual.program!=initial.program||
                 actual.instruction!=initial.instruction||actual.mainInstruction!=initial.mainInstruction||
                 actual.timeBits!=initial.timeBits||actual.stackTop!=initial.stackTop)++unexpectedOwners;
        return true;
    }
    void note_execution() noexcept {if(active)++executedCalls;}
    void note_unexpected_spawn() noexcept {if(active)++unexpectedOwners;}
};

// Versioned post-consumer ABI; indices match the shared STG action reader.
// TH10 inventory units are actual power / 20. Counters observe the production
// Bomb start; they do not grant power or change the player's update order.
struct State {
    SceneGate scene;
    Word hostFrames=0,consumerSequence=0,bombsUsed=0,bombsUsedStage=0;
    Word actions[20]{1};
    Word dialogueSequence=0,dialogueCurrent=0,dialoguePrevious=0,dialoguePressed=0;
    Word dialogueRequestSerial=0,dialogueRequestStatus=0,dialogueEntry=~0u,dialogueSubroutine=~0u;
    bool dialogueValid=false;
    void reset_actions() noexcept {consumerSequence=bombsUsed=bombsUsedStage=0;for(auto& v:actions)v=0;actions[0]=1;
        dialogueSequence=dialogueCurrent=dialoguePrevious=dialoguePressed=dialogueRequestSerial=dialogueRequestStatus=0;
        dialogueEntry=dialogueSubroutine=~0u;dialogueValid=false;}
    void publish(Word frame,Word stage,Word current,Word previous,Word focused,
                 Word fireCurrent,Word firePrevious,Word shots,Word bombActive,
                 Word triggered,Word bombType,Word bombTimer,Word power,Word life) noexcept {
        const Word values[]{1,255,1,++consumerSequence,frame,stage,current,previous,focused,
            fireCurrent,firePrevious,shots,bombActive,triggered,bombType,bombTimer,
            power/20,bombsUsed,bombsUsedStage,life};
        for(unsigned i=0;i<20;++i)actions[i]=values[i];
    }
    void publish_paused(Word stage,Word life) noexcept {
        for(auto& v:actions)v=0;actions[0]=1;actions[1]=129;actions[2]=2;
        actions[4]=hostFrames;actions[5]=stage;actions[19]=life;
    }
};
}
