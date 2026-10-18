#pragma once
#include "AnmManager.hpp"
namespace th10 {
struct AnmFrameEnvironment {
    AnmAllocationEnvironment* allocation;
    virtual void callback(u32 token,AnmVm& vm)=0;
    virtual i32 update(AnmVm& vm)=0;
    virtual void draw(AnmVm& vm)=0;
};
}
