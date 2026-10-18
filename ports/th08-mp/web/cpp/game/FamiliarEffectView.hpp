#pragma once
#include "AnmExecutor.hpp"
#include "EffectState.hpp"
#include "PresentationVisual.hpp"
#include <unordered_map>

namespace th08 {
// Viewer-local ANM state. These copies never enter the effect pool, Replay,
// shared RNG or rollback journal. Authored interrupts advance on fixed ticks.
class FamiliarEffectView {
    struct Sample {
        AnmVm animation{};
        presentation::VisualSample previous;
        i32 age=-1,form=-1;
        bool alive=false;
    };
    Rng random{};
    AnmExecutor animations{random};
    std::unordered_map<const EffectState*,Sample> samples;
public:
    void reset(){samples.clear();}
    void update(const EffectState& source,i32 form,const FrameTiming& timing){
        if(!source.active||(source.kind!=32&&source.kind!=33)||form<0){samples.erase(&source);return;}
        auto& sample=samples[&source];
        const bool fresh=sample.form<0||source.age.current<=sample.age||
            sample.animation.anmFile!=source.anmFile||sample.animation.beginningOfScript!=source.beginningOfScript;
        if(fresh){sample.animation=source;sample.form=-1;sample.alive=true;}
        sample.previous.capture(sample.animation);
        // A destruction interrupt outranks a form change. Host form interrupts
        // 1/2 do not restart the local ring's eight-frame transition.
        if(source.pendingInterrupt&&source.pendingInterrupt!=1&&source.pendingInterrupt!=2)
            sample.animation.pendingInterrupt=source.pendingInterrupt;
        else if(sample.form!=form)sample.animation.pendingInterrupt=form?2:1;
        sample.form=form;sample.age=source.age.current;
        animations.timing=timing;
        sample.alive=!animations.execute(sample.animation);
    }
    bool contains(const EffectState& source)const{
        const auto found=samples.find(&source);
        return found!=samples.end()&&source.active&&(source.kind==32||source.kind==33)&&source.age.current>=found->second.age&&
            source.age.current<=found->second.age+1&&source.anmFile==found->second.animation.anmFile&&
            source.beginningOfScript==found->second.animation.beginningOfScript;
    }
    bool apply(const EffectState& source,EffectState& draw)const{
        if(!contains(source))return false;
        const auto& sample=samples.find(&source)->second;
        static_cast<AnmVm&>(draw)=sample.animation;
        if(!sample.alive)draw.visible=false;
        sample.previous.apply(sample.animation,draw,presentation::world_alpha,
            presentation::VisualSample::Attributes|presentation::VisualSample::Offset);
        return true;
    }
};
}
