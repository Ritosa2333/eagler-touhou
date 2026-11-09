#include "TouchMotionTrack.hpp"
#include <unordered_map>

namespace th09::touch {
namespace {
u32 coordinate_bits(const MotionTrack::Entry& entry,bool y){
    return MotionTrack::word(reinterpret_cast<const u8*>(y?&entry.y:&entry.x));
}
}

// v5 uses one dictionary per axis and track. Common coordinates take one
// byte, and code zero introduces a literal with its original float bits.
void MotionTrack::pack_dictionary(const std::vector<Entry>& track,std::vector<u8>& out){
    if(track.empty())return;
    u32 next=0;
    for(const auto& entry:track){
        replay_compression::append_varint(out,entry.tick-next);
        next=entry.tick+1;
    }
    for(bool y:{false,true}){
        std::unordered_map<u32,u32> frequency;
        for(const auto& entry:track)++frequency[coordinate_bits(entry,y)];
        std::vector<std::pair<u32,u32>> ranked(frequency.begin(),frequency.end());
        const auto count=std::min<std::size_t>(dictionary_entries,ranked.size());
        std::partial_sort(ranked.begin(),ranked.begin()+count,ranked.end(),[](const auto& a,const auto& b){
            return a.second!=b.second?a.second>b.second:a.first<b.first;
        });
        ranked.resize(count);
        std::sort(ranked.begin(),ranked.end());
        out.push_back(u8(count));
        u32 previous=0;
        for(const auto& [bits,uses]:ranked){
            replay_compression::append_varint(out,bits-previous);
            previous=bits;
        }
        for(const auto& entry:track){
            const auto bits=coordinate_bits(entry,y);
            const auto it=std::lower_bound(ranked.begin(),ranked.end(),bits,[](const auto& value,u32 key){return value.first<key;});
            if(it!=ranked.end()&&it->first==bits)out.push_back(u8(it-ranked.begin()+1));
            else{out.push_back(0);append_word(out,bits,4);}
        }
    }
}

bool MotionTrack::unpack_dictionary(std::vector<Entry>& track,const u8*& cursor,const u8* end){
    if(track.empty())return true;
    std::uint64_t next=0;
    for(auto& entry:track){
        u32 gap;
        if(!replay_compression::read_varint(cursor,end,gap)||next+gap>0xffffffffu)return false;
        entry.tick=u32(next+gap);next+=std::uint64_t(gap)+1;
    }
    for(bool y:{false,true}){
        if(cursor==end)return false;
        const u32 count=*cursor++;
        if(count>dictionary_entries||count>track.size())return false;
        std::array<u32,dictionary_entries+1> dictionary{};
        for(u32 i=1;i<=count;++i){
            u32 delta;
            if(!replay_compression::read_varint(cursor,end,delta)||(i>1&&!delta)||delta>0xffffffffu-dictionary[i-1])return false;
            dictionary[i]=dictionary[i-1]+delta;
        }
        for(auto& entry:track){
            if(cursor==end)return false;
            const auto code=*cursor++;
            if(code>count||(!code&&end-cursor<4))return false;
            const u32 bits=code?dictionary[code]:take_word(cursor,4);
            std::memcpy(y?&entry.y:&entry.x,&bits,4);
        }
    }
    return true;
}
}
