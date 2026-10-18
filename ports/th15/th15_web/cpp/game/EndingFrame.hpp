#pragma once
#include "EndingScript.hpp"
#include "FrameScheduler.hpp"
namespace th15 {
struct EndingDestination {virtual ~EndingDestination()=default;virtual bool ending_destination(i32)=0;};
class EndingFrame {
 EndingScript& script;EndingDestination& host;FrameScheduler& scheduler;
 std::array<FrameCallback,2> callbacks;EndingInput input;u32 system_flags=0;
public:
 i32 frames=0;std::string error;
 EndingFrame(EndingScript&,EndingDestination&,FrameScheduler&);~EndingFrame();
 void controls(const EndingInput& value,u32 flags=0)noexcept{input=value;system_flags=flags;}
 i32 update();
};
}
