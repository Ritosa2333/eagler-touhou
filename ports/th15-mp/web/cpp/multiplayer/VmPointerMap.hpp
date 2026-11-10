#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace th15 { class AnmVm; }
namespace th15::multiplayer {
// A restoration knows its full graph size before inserting. Keep that scratch
// in one contiguous allocation instead of allocating a hash node for every VM.
class VmPointerMap {
    struct Entry {const AnmVm* key=nullptr;AnmVm* value=nullptr;};
    std::vector<Entry> entries_;
    static size_t hash(const AnmVm* key)noexcept{
        auto bits=reinterpret_cast<std::uintptr_t>(key);
        bits^=bits>>16;bits*=0x7feb352du;bits^=bits>>15;bits*=0x846ca68bu;bits^=bits>>16;
        return size_t(bits);
    }
public:
    explicit VmPointerMap(size_t count){
        size_t capacity=2;while(capacity/2<count)capacity*=2;
        entries_.resize(capacity);
    }
    void emplace(const AnmVm* key,AnmVm* value)noexcept{
        const auto mask=entries_.size()-1;auto at=hash(key)&mask;
        while(entries_[at].key&&entries_[at].key!=key)at=(at+1)&mask;
        if(!entries_[at].key)entries_[at]={key,value};
    }
    AnmVm* find(const AnmVm* key)const noexcept{
        if(!key)return nullptr;
        const auto mask=entries_.size()-1;auto at=hash(key)&mask;
        while(entries_[at].key){if(entries_[at].key==key)return entries_[at].value;at=(at+1)&mask;}
        return nullptr;
    }
};
}
