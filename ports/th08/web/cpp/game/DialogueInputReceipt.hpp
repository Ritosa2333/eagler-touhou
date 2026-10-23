#pragma once
#include <cstdint>
#include <utility>

namespace th08 {
// Copy of the exact input sample passed through GuiFlow's Dialogue update.
// The later Replay-recording publisher may replace DialogueContext inputs for
// the next tick; it cannot rewrite this value receipt.
class DialogueInputReceipt {
public:
    void reset() noexcept {
        current_=previous_=0;
        sequence_=0;
        valid_=false;
    }

    void invalidate() noexcept { valid_=false; }

    template<class Update>
    void consume(std::uint16_t current,std::uint16_t previous,Update&& update) {
        std::forward<Update>(update)();
        current_=current;
        previous_=previous;
        if(++sequence_==0)++sequence_;
        valid_=true;
    }

    std::uint16_t current()const noexcept{return current_;}
    std::uint16_t previous()const noexcept{return previous_;}
    std::uint32_t sequence()const noexcept{return sequence_;}
    bool valid()const noexcept{return valid_;}

private:
    std::uint16_t current_=0,previous_=0;
    std::uint32_t sequence_=0;
    bool valid_=false;
};
}
