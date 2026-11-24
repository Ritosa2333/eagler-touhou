#pragma once
#include <cstdint>
#include <optional>

namespace th128::laser::cancel {
struct LineMembership {
    std::uint32_t line=0,head=0,tail=0,sentinel=0,previous=0,next=0;
    std::uint32_t previousNext=0,nextPrevious=0,count=0;
};
struct DeferredLineUnlink {
    std::uint32_t count=0,head=0,tail=0;
    std::uint32_t previous=0,previousNext=0,next=0,nextPrevious=0;
    bool updateNextPrevious=false;
};

inline std::optional<DeferredLineUnlink> prepareDeferredLineUnlink(std::uint32_t vtable,
    std::uint32_t state,const LineMembership& membership){
    if(vtable!=0x4a2c1c||state!=1||!membership.count||membership.count>256
        ||!membership.line||!membership.head||!membership.tail||!membership.sentinel
        ||!membership.previous||membership.line==membership.sentinel
        ||membership.previous==membership.line||membership.next==membership.line
        ||membership.next==membership.sentinel
        ||membership.previousNext!=membership.line
        ||(membership.next&&membership.nextPrevious!=membership.line)
        ||((membership.head==membership.line)!=(membership.previous==membership.sentinel))
        ||((membership.tail==membership.line)!=(membership.next==0))
        ||((membership.count==1)!=(membership.head==membership.line&&membership.tail==membership.line)))
        return std::nullopt;
    return DeferredLineUnlink{membership.count-1,
        membership.head==membership.line?membership.next:membership.head,
        membership.tail==membership.line?membership.previous:membership.tail,
        membership.previous,membership.next,membership.next,membership.previous,membership.next!=0};
}
}
