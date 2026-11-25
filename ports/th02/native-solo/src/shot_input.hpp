#pragma once
#include <deque>
namespace soew {
enum class ShotInputMode { Manual, Autofire12Hz, Fast };
inline constexpr ShotInputMode shot_input_mode(bool autofire,bool fast_autofire,bool dialogue){
    return !autofire||dialogue?ShotInputMode::Manual:fast_autofire?ShotInputMode::Fast:ShotInputMode::Autofire12Hz;
}
// Host-only Z sampler. Never inject OS keys or change original burst logic.
struct ShotInput {
    bool physical=false,sampled=false,blocked=false;
    unsigned phase=0;
    std::deque<bool> edges;
    // Manual edges are sampled in order; never converted into firing commands.
    // Autofire release discards all pending transitions immediately.
    void clear(){physical=sampled=false;phase=0;edges.clear();blocked=true;}
    void event(bool down,ShotInputMode mode){
        if(!down)blocked=false;
        // Only non-repeat WM_KEYDOWN reaches here: a fresh press after a
        // reset is trustworthy even before the next timer release sample.
        if(blocked&&down)blocked=false;
        if(physical==down)return;
        physical=down;
        if(mode!=ShotInputMode::Manual){edges.clear();phase=0;sampled=false;}
        else {if(edges.size()<64)edges.push_back(down);else{edges.clear();edges.push_back(down);}}
    }
    void event(bool down,bool autofire){event(down,autofire?ShotInputMode::Autofire12Hz:ShotInputMode::Manual);}
    bool tick(ShotInputMode mode){
        if(blocked)return false;
        if(mode!=ShotInputMode::Manual){
            edges.clear();if(!physical){phase=0;sampled=false;return false;}
            // Both feed the original burst gate without requesting a volley.
            // The accumulator advances per 17 ms base logic tick; Fast uses
            // two sampled ticks down, two up instead of the 12 Hz waveform.
            if(mode==ShotInputMode::Fast){sampled=phase<2;phase=(phase+1)%4;}
            else {sampled=phase<1000;phase=(phase+408)%2000;}
            return sampled;
        }
        if(!edges.empty()){sampled=edges.front();edges.pop_front();}else sampled=physical;
        return sampled;
    }
    bool tick(bool autofire){return tick(autofire?ShotInputMode::Autofire12Hz:ShotInputMode::Manual);}
};
}
