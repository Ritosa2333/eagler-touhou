#pragma once
#include "TexturePixels.hpp"
#include <memory>
namespace th08 {
struct TextureRecord {
    TexturePixels image;
    u32 references=1,revision=1,priority=0;
    bool render_target=false;
};
class TextureStore {
public:
    using PrepareCallback=void(*)(void*,u32);
    PrepareCallback prepare_callback=nullptr;void* prepare_context=nullptr;
    void prepare(u32 handle){if(prepare_callback)prepare_callback(prepare_context,handle);}
    u32 insert(TexturePixels&& image,u32 priority=0,bool render_target=false);
    TextureRecord* get(u32 handle) noexcept;
    const TextureRecord* get(u32 handle) const noexcept;
    bool retain(u32 handle) noexcept;
    void release(u32 handle) noexcept;
    void changed(u32 handle) noexcept;
    u32 live_count() const noexcept { return live; }
private:
    std::vector<std::unique_ptr<TextureRecord>> records;
    u32 live=0;
};
}
