#pragma once
namespace th15::multiplayer {
// A speculative frame may mutate its captured world, but scene factories and
// external writes wait for actual inputs. The frame owner restores on a fence.
struct FrameFence {
    inline static bool tentative=false,blocked=false;
    static bool allow()noexcept{if(tentative){blocked=true;return false;}return true;}
    struct Scope {
        bool previous;
        explicit Scope(bool predicted):previous(tentative){tentative=predicted;blocked=false;}
        ~Scope(){tentative=previous;}
    };
};
}
