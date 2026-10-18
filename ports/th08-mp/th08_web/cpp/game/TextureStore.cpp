#include "TextureStore.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/TextureJournal.hpp"
#endif
namespace th08 {
u32 TextureStore::insert(TexturePixels&& image,u32 priority,bool render_target) {
    if(!image.width||!image.height||image.pixels.empty())return 0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(journal&&!journal->MayMutate())return 0;
    auto record=std::make_shared<TextureRecord>();
#else
    auto record=std::make_unique<TextureRecord>();
#endif
    record->image=std::move(image);record->priority=priority;record->render_target=render_target;
    records.push_back(std::move(record));++live;
    return records.size();
}
TextureRecord* TextureStore::get(u32 handle) noexcept { return handle&&handle<=records.size()?records[handle-1].get():nullptr; }
const TextureRecord* TextureStore::get(u32 handle) const noexcept { return handle&&handle<=records.size()?records[handle-1].get():nullptr; }
bool TextureStore::retain(u32 handle) noexcept {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(journal&&!journal->Touch(handle,false))return false;
#endif
    auto* record=get(handle);if(!record)return false;++record->references;return true;
}
void TextureStore::release(u32 handle) noexcept {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(journal&&get(handle)&&!journal->Touch(handle,false))return;
#endif
    auto* record=get(handle);if(record&&!--record->references){records[handle-1].reset();--live;}
}
void TextureStore::changed(u32 handle) noexcept {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(journal&&!journal->Touch(handle,false))return;
#endif
    if(auto* record=get(handle))++record->revision;
}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
bool TextureStore::before_write(u32 handle){return !journal||journal->Touch(handle,true);}
#endif
}
