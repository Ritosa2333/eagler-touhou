#include "ScreenEffects.hpp"
namespace th17 {
ScreenEffects::~ScreenEffects(){clear();}
void ScreenEffects::clear(){for(auto& entry:entries)scheduler->remove(entry->callback);entries.clear();reset_offsets();}
void ScreenEffects::collect(){for(auto next=entries.begin();next!=entries.end();)if((*next)->retired)next=entries.erase(next);else ++next;}
u32 ScreenEffects::count()const noexcept{u32 count=0;for(const auto& e:entries)if(!e->retired)count++;return count;}
bool ScreenEffects::attach(FrameScheduler& target){if(scheduler==&target)return true;for(auto& e:entries){scheduler->remove(e->callback);if(!e->retired){e->callback.run=update_entry;if(target.add(e->callback,FramePass::Update,20)<0){error="Screen effect scheduling failed";return false;}}}scheduler=&target;return true;}
bool ScreenEffects::submit(std::unique_ptr<Entry> entry){entry->owner=this;auto& callback=entry->callback;callback.owner=entry.get();callback.run=update_entry;callback.cleanup=retire_entry;callback.enabled=true;if(scheduler->add(callback,FramePass::Update,20)<0){error="Screen effect scheduling failed";return false;}entries.push_back(std::move(entry));return true;}
bool ScreenEffects::shake(ScreenShakeSpec spec){collect();auto entry=std::make_unique<Entry>();entry->shake=std::make_unique<ScreenShake>(game_random,spec);return submit(std::move(entry));}
bool ScreenEffects::nudge(ScreenNudgeSpec spec){collect();auto entry=std::make_unique<Entry>();entry->nudge=std::make_unique<ScreenNudge>(visual_random,spec);return submit(std::move(entry));}
i32 ScreenEffects::retire_entry(void* p){auto& e=*static_cast<Entry*>(p);e.owner->scheduler->remove(e.callback);e.retired=true;return 0;}
i32 ScreenEffects::update_entry(void* p){
    auto& e=*static_cast<Entry*>(p);auto& owner=*e.owner;const i32 result=e.shake?e.shake->update(owner.context):e.nudge->update(owner.context);
    // A paused/unavailable game's shake callback succeeds without writing the
    // camera globals. Recopying its last sample would undo the end-of-draw reset
    // and could also overwrite a different effect that ran earlier this frame.
    const bool writes_offset=result==1&&(!e.shake||(owner.context.game_available&&!(owner.context.game_flags&0x77)));
    if(writes_offset){if(e.shake){owner.world_offset=e.shake->world_offset;owner.screen_offset=e.shake->screen_offset;}else{owner.world_offset=e.nudge->world_offset;owner.screen_offset=e.nudge->screen_offset;}}return result;
}
}
