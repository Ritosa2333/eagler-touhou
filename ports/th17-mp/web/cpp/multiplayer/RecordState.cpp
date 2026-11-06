#include "FrameImage.hpp"
#include "../game/SpellRecords.hpp"

namespace th17 {
struct SpellRecords::Checkpoint {
    decltype(loadouts) personal;
    decltype(aggregate) total;
    explicit Checkpoint(const SpellRecords& records):personal(records.loadouts),total(records.aggregate){}
};

void SpellRecords::save_rollback(multiplayer::FrameImage& image){
    auto saved=rollback_record.lock();
    if(!saved){
        saved=std::make_shared<const Checkpoint>(*this);
        rollback_record=saved;
    }
    image.restore_with([this,saved=std::move(saved)]{
        if(rollback_record.lock()!=saved){
            loadouts=saved->personal;
            aggregate=saved->total;
            rollback_record=saved;
        }
        return true;
    });
}
}
