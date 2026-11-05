#pragma once
#include <array>
#include <cstdint>
#include <map>
#include <utility>
#include <vector>

namespace th10::multiplayer {
// ANM handles rewind with the native registry. Keep the corresponding visual
// owners at the same checkpoint, including markers already fading out.
class HitboxOwnershipHistory {
public:
    static constexpr std::size_t MaxFrames=14;
    using Owners=std::map<std::uint32_t,std::uint32_t>;
    void Clear(){for(auto& record:records_){record.valid=false;record.owners.clear();}}
    void Capture(std::uint32_t frame,const Owners& owners){
        Record* selected=nullptr;
        for(auto& record:records_)if(record.valid&&record.frame==frame){selected=&record;break;}
        if(!selected)for(auto& record:records_)if(!record.valid){selected=&record;break;}
        if(!selected){selected=&records_[0];for(auto& record:records_)if(record.frame<selected->frame)selected=&record;}
        selected->owners.assign(owners.begin(),owners.end());
        selected->frame=frame;selected->valid=true;
    }
    bool Restore(std::uint32_t frame,Owners& owners){
        const Record* selected=nullptr;
        for(const auto& record:records_)if(record.valid&&record.frame==frame){selected=&record;break;}
        if(!selected)return false;
        owners.clear();owners.insert(selected->owners.begin(),selected->owners.end());
        for(auto& record:records_)if(record.valid&&record.frame>=frame)record.valid=false;
        return true;
    }
private:
    struct Record {std::uint32_t frame=0;bool valid=false;std::vector<std::pair<std::uint32_t,std::uint32_t>> owners;};
    // Reuse each slot's vector capacity during forward play and resimulation.
    std::array<Record,MaxFrames> records_{};
};
}
