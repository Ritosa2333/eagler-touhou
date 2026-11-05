#pragma once
#include <algorithm>
#include <cstdint>
#include <map>
#include <vector>

namespace th075::multiplayer {
// Output history is deliberately outside world snapshots. Play a predicted
// event immediately, match it during correction, and stop obsolete voices.
// Matching across the corrected interval also handles an event moving a few
// frames as the corrected attack/hit timing changes (Giuroll sound.rs).
template<class Sound> class RollbackAudio {
    std::map<std::uint32_t,std::vector<Sound>> heard;
    std::vector<Sound> candidates;
    std::uint32_t first=0,confirmed=0;
    bool correcting=false;
    void prune(){if(correcting)return;while(!heard.empty()&&heard.begin()->first+4<confirmed)heard.erase(heard.begin());}
public:
    void Reset(){heard.clear();candidates.clear();first=confirmed=0;correcting=false;}
    void Begin(std::uint32_t frame){
        first=correcting?std::min(first,frame):frame;correcting=true;
        for(auto it=heard.lower_bound(frame);it!=heard.end();){candidates.insert(candidates.end(),it->second.begin(),it->second.end());it=heard.erase(it);}
    }
    template<class Play> void Submit(std::uint32_t frame,const std::vector<Sound>& sounds,Play play){
        for(const auto& sound:sounds){const auto match=std::find(candidates.begin(),candidates.end(),sound);
            if(correcting&&match!=candidates.end())candidates.erase(match);else play(sound);
        }
        heard[frame]=sounds;
    }
    template<class Stop> void Finish(Stop stop){
        for(const auto& sound:candidates){bool retained=false;
            for(auto it=heard.lower_bound(first>4?first-4:0);it!=heard.end()&&!retained;++it)
                retained=std::find(it->second.begin(),it->second.end(),sound)!=it->second.end();
            if(!retained)stop(sound);
        }
        candidates.clear();correcting=false;prune();
    }
    void Confirm(std::uint32_t frame){confirmed=std::max(confirmed,frame);prune();}
};
}
