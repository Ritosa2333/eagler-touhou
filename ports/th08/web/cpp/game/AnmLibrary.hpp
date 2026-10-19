#pragma once
#include "AnmResource.hpp"
#include "Archive.hpp"
#include "TextureStore.hpp"
#include <array>
namespace th08 {
struct AnmTextureEntry { u32 texture=0; const u8* external_data=nullptr; u32 external_size=0; };
static_assert(sizeof(AnmTextureEntry)==12);
class AnmLibrary {
public:
    explicit AnmLibrary(TextureStore& textures):textures(textures){}
    ~AnmLibrary();
    bool force_16bit=false;
    // Preparing a pristine template never runs an animation or consumes RNG.
    bool preload(const u8*,u32);
    u32 preload_count()const{return prepared.size();}
    u32 preload_hits()const{return cache_hits;}
    u32 resident_hits()const{return texture_cache_hits;}
    void clear_preloads();
    AnmLoaded* load(i32 index,const u8* bytes,u32 size,bool deferred=false);
    AnmLoaded* load(i32 index,Archive& archive,const char* name,bool deferred=false);
    bool service();
    bool postload(i32 index);
    void release(i32 index);
    AnmLoaded* get(i32 index) noexcept;
    const AnmResource* resource(i32 index) const noexcept;
    bool start(i32 index,i32 script,AnmVm& vm,AnmExecutor& executor,bool reset_position=true);
    bool has_texture(const AnmVm& vm) const noexcept;
private:
    struct Prepared {AnmResource resource;std::vector<TexturePixels> pixels;std::vector<u32> resident;mutable std::vector<u32> resident_revision;bool force_16bit=false;};
    struct Entry { AnmResource resource; std::vector<AnmTextureEntry> textures;const Prepared* prepared=nullptr; };
    std::vector<std::unique_ptr<Prepared>> prepared;u32 prepared_bytes=0,cache_hits=0,texture_cache_hits=0;
    std::array<std::unique_ptr<Entry>,256> files;
    TextureStore& textures;
    bool materialize(Entry& entry,u32 index);
};
}
