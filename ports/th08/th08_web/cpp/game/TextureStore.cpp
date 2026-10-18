#include "TextureStore.hpp"
namespace th08 {
u32 TextureStore::insert(TexturePixels&& image,u32 priority,bool render_target) {
    if(!image.width||!image.height||image.pixels.empty())return 0;
    auto record=std::make_unique<TextureRecord>();
    record->image=std::move(image);record->priority=priority;record->render_target=render_target;
    records.push_back(std::move(record));++live;
    return records.size();
}
TextureRecord* TextureStore::get(u32 handle) noexcept { return handle&&handle<=records.size()?records[handle-1].get():nullptr; }
const TextureRecord* TextureStore::get(u32 handle) const noexcept { return handle&&handle<=records.size()?records[handle-1].get():nullptr; }
bool TextureStore::retain(u32 handle) noexcept { auto* record=get(handle);if(!record)return false;++record->references;return true; }
void TextureStore::release(u32 handle) noexcept { auto* record=get(handle);if(record&&!--record->references){records[handle-1].reset();--live;} }
void TextureStore::changed(u32 handle) noexcept { if(auto* record=get(handle))++record->revision; }
}
