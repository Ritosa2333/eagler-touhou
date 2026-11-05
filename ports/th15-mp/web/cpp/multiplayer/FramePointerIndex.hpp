#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

namespace th15::multiplayer {
// Capture builds a fresh index each frame. Retain the contiguous table across
// ring reuse instead of visiting and allocating individual hash nodes.
template<class Key> class FramePointerIndex {
    struct Entry {const Key* key=nullptr;std::uint32_t value=0;};
    std::vector<Entry> entries_;
    std::size_t count_=0;
    static std::size_t hash(const Key* key)noexcept{
        auto bits=reinterpret_cast<std::uintptr_t>(key);
        bits^=bits>>16;bits*=0x7feb352du;bits^=bits>>15;bits*=0x846ca68bu;bits^=bits>>16;
        return std::size_t(bits);
    }
    void grow(){
        auto previous=std::move(entries_);entries_.resize(previous.size()*2);count_=0;
        for(const auto& entry:previous)if(entry.key)insert(entry.key,entry.value);
    }
public:
    inline static constexpr auto missing=std::numeric_limits<std::uint32_t>::max();
    void reset(std::size_t count){
        std::size_t capacity=2;while(capacity/2<count)capacity*=2;
        if(entries_.size()<capacity)entries_.assign(capacity,{});
        else std::fill(entries_.begin(),entries_.end(),Entry{});
        count_=0;
    }
    bool insert(const Key* key,std::uint32_t value){
        if(!key)return false;
        if(entries_.empty())reset(1);
        const auto mask=entries_.size()-1;auto at=hash(key)&mask;
        while(entries_[at].key){if(entries_[at].key==key)return false;at=(at+1)&mask;}
        if(count_>=entries_.size()/2){grow();return insert(key,value);}
        entries_[at]={key,value};
        ++count_;return true;
    }
    std::uint32_t find(const Key* key)const noexcept{
        if(!key||entries_.empty())return missing;
        const auto mask=entries_.size()-1;auto at=hash(key)&mask;
        while(entries_[at].key){if(entries_[at].key==key)return entries_[at].value;at=(at+1)&mask;}
        return missing;
    }
    std::size_t retained_bytes()const noexcept{return entries_.capacity()*sizeof(Entry);}
};
}
