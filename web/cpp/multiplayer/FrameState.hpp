#pragma once
#include "../game/FrameScheduler.hpp"
#include <array>
#include <functional>
#include <string>
#include <vector>

namespace th15::multiplayer {
class FrameState {
    struct Entry {
        FrameCallback* original=nullptr;
        FrameCallback::Function run=nullptr,initialize=nullptr,cleanup=nullptr;
        void* owner=nullptr;
        bool enabled=false;
        i32 priority=0;
        FramePass pass=FramePass::Update;
    };
    std::vector<Entry> entries_;
    std::array<std::vector<size_t>,2> order_;
    FrameCallback* continuation_=nullptr;
    bool closing_=false,ready_=false;
public:
    using CallbackResolver=std::function<FrameCallback*(FrameCallback*)>;
    using OwnerResolver=std::function<void*(void*)>;
    mutable std::string error;
    bool capture(const FrameScheduler&);
    // Detach before replacing owners. Undo invokes no initialization, cleanup,
    // registration or presentation effects from the discarded future.
    static void detach(FrameScheduler&)noexcept;
    bool restore(FrameScheduler&,const CallbackResolver& callbacks={},const OwnerResolver& owners={})const;
    size_t size()const noexcept{return entries_.size();}
};
}
