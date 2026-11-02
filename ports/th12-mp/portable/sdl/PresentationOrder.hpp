// Reusable frame-local lookup and ordering, shared by native/Web presentation.
#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <vector>
namespace th12host {
// First registration wins, matching the original draw schedule Map. Epochs
// release old identities without clearing/reallocating buckets every frame.
class FrameLookup {
    struct Slot {std::uint32_t key=0,index=0;std::uint64_t epoch=0;};
    std::vector<Slot> slots;std::uint64_t epoch=0;
    std::size_t bucket(std::uint32_t key)const{
        key^=key>>16;key*=0x7feb352du;key^=key>>15;return key&(slots.size()-1);
    }
public:
    static constexpr std::uint32_t missing=std::numeric_limits<std::uint32_t>::max();
    template<class Records> void build(const Records* records,std::size_t count){
        std::size_t size=16;while(size<count*2)size*=2;
        if(slots.size()<size){slots.assign(size,{});epoch=0;}
        if(++epoch==0){for(auto& slot:slots)slot.epoch=0;epoch=1;}
        for(std::uint32_t i=0;i<count;++i){
            std::size_t at=bucket(records[i].id);
            while(slots[at].epoch==epoch&&slots[at].key!=records[i].id)at=(at+1)&(slots.size()-1);
            if(slots[at].epoch!=epoch)slots[at]={records[i].id,i,epoch};
        }
    }
    std::uint32_t find(std::uint32_t key)const{
        if(slots.empty())return missing;std::size_t at=bucket(key);
        while(slots[at].epoch==epoch){if(slots[at].key==key)return slots[at].index;at=(at+1)&(slots.size()-1);}
        return missing;
    }
};
// The original Web adapter's priority buckets: ordered runs are concatenated,
// and only buckets whose callback order reverses need a comparison sort.
class PriorityBuckets {
    struct Bucket {std::vector<std::uint32_t> indices;bool unsorted=false;};
    std::array<Bucket,101> buckets;
public:
    template<class Priority,class Order> void sort(std::vector<std::uint32_t>& indices,Priority priority,Order order){
        const auto less=[&](auto a,auto b){return order(a)!=order(b)?order(a)<order(b):a<b;};
        for(auto& b:buckets){b.indices.clear();b.unsorted=false;}
        for(auto index:indices){
            const auto p=priority(index);
            if(p<0||p>=int(buckets.size())){
                std::sort(indices.begin(),indices.end(),[&](auto a,auto b){return priority(a)!=priority(b)?priority(a)<priority(b):less(a,b);});return;
            }
            auto& b=buckets[std::size_t(p)];
            if(!b.indices.empty()&&less(index,b.indices.back()))b.unsorted=true;
            b.indices.push_back(index);
        }
        indices.clear();for(auto& b:buckets){
            if(b.unsorted)std::sort(b.indices.begin(),b.indices.end(),less);
            indices.insert(indices.end(),b.indices.begin(),b.indices.end());
        }
    }
};
}
