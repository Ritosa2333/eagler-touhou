#pragma once
#include <array>
#include <cstdint>
#include <optional>
namespace soew {
// The original shot effect is also requested on some cooldown frames.
// With autofire, sound immediately, then group the actual emitted volleys.
class ShotSoundCadence {
    static constexpr unsigned volleys_per_sound=3;
    bool autofire_=false;
    unsigned phase_=0;
public:
    void set_autofire(bool enabled){if(autofire_!=enabled){autofire_=enabled;reset();}}
    void reset(){phase_=0;}
    std::uint64_t requests(std::uint64_t original,bool emitted_volley){
        if(!autofire_)return original;
        if(!emitted_volley)return 0;
        const bool play=phase_==0;
        phase_=(phase_+1)%volleys_per_sound;
        return play;
    }
};

// ReC98/th02/snd/se_data.c and se.cpp, not TH03's different priorities.
struct SoundEffects {
    static constexpr std::array<unsigned,33> priorities={0,0,32,9,2,16,5,6,48,8,9,5,16,10,17,12,32,32,16};
    static constexpr std::array<unsigned,33> holds={0,0,36,16,4,16,8,10,48,80,24,17,11,4,80,16,48,74,32};
    unsigned current=255,frame=0;
    bool enabled=true;
    std::uint64_t requests=0,dispatched=0;
    ShotSoundCadence shot_cadence;
    void request(unsigned id){++requests;if(!enabled||id>=33)return;if(current==255)current=id;else if(priorities[current]<=priorities[id]){current=id;frame=0;}}
    std::optional<unsigned> update(){if(!enabled||current==255)return {};std::optional<unsigned> result;if(frame==0){result=current;++dispatched;}++frame;if(holds[current]<frame){frame=0;current=255;}return result;}
    std::optional<unsigned> force(unsigned id){current=255;frame=0;request(id);return update();}
};
}
