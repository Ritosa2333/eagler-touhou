#include "AnmLibrary.hpp"
#ifdef TH_ENABLE_THCRAP
#include "RuntimeOverride.hpp"
#endif
#include <algorithm>
#include <cmath>
#include <cstring>
namespace th08 {
#ifdef TH_NATIVE_PLATFORM
bool sdl_decode_rgba(const u8*,u32,u32&,u32&,std::vector<u8>&);
#endif
#if defined(TH_NATIVE_PLATFORM)&&defined(TH_ENABLE_THCRAP)
namespace {
enum class RgbaAlphaState{Empty,Opaque,Mixed};
// Exact RGBA rect analysis and blit_blend port of th07 AnmManager, which in turn
// mirrors upstream thcrap_tsa anm.cpp. The replacement PNG is a coordinate-space
// patch over the embedded atlas, never a resized backing texture.
RgbaAlphaState analyze_rgba(const std::vector<u8>& pixels,i32 stride,i32 left,i32 top,i32 width,i32 height){
    bool zero=false,opaque=false;
    for(i32 y=top;y<top+height;++y){
        const u8* row=pixels.data()+size_t(y)*size_t(stride)+size_t(left)*4;
        for(i32 x=0;x<width;++x,row+=4){
            if(row[3]==0)zero=true;else if(row[3]==255)opaque=true;else return RgbaAlphaState::Mixed;
            if(zero&&opaque)return RgbaAlphaState::Mixed;
        }
    }
    return opaque?RgbaAlphaState::Opaque:RgbaAlphaState::Empty;
}
void blend_rgba_over_opaque(u8* destination,const u8* source,i32 pixels){
    for(i32 x=0;x<pixels;++x,destination+=4,source+=4){
        const i32 source_alpha=source[3],destination_weight=255-source_alpha;
        destination[0]=u8((destination[0]*destination_weight+source[0]*source_alpha)>>8);
        destination[1]=u8((destination[1]*destination_weight+source[1]*source_alpha)>>8);
        destination[2]=u8((destination[2]*destination_weight+source[2]*source_alpha)>>8);
        destination[3]=u8(std::min<i32>(destination[3]+source_alpha,255));
    }
}
// Overrides ship as external PNGs whose repository path equals the ANM's
// embedded texture name (data/title/select01.png). The patch is composited into
// the original atlas at the sprite rectangles that share its origin; sprites
// outside a smaller patch retain their embedded THTX pixels.
bool load_override(const AnmResource& resource,u32 index,bool reduced,TexturePixels& image){
    const auto& source=resource.textures()[index];
    if(source.name.empty()||source.empty||!source.embedded)return false;
    // The overlay ASCII atlas is remapped by thcrap's ascii_vpatchf. This port
    // keeps the original index-to-glyph layout, so replacing the atlas would
    // corrupt every overlay string; the EAS1 table already covers its text.
    if(source.name=="data/ascii/ascii.png")return false;
    std::vector<u8> bytes;
    if(!RuntimeOverride::Read(source.name.c_str(),bytes))return false;
    u32 patch_width=0,patch_height=0;std::vector<u8> patch;
    if(!sdl_decode_rgba(bytes.data(),u32(bytes.size()),patch_width,patch_height,patch))return false;
    if(!image.width||!image.height)return false;
    std::vector<u8> target=image.rgba();
    if(patch_width==0||patch_height==0||target.size()!=u64(image.width)*image.height*4)return false;
    const auto& rects=resource.sprite_rects();
    bool patched=false;
    for(u32 sprite=source.first_sprite;sprite<source.first_sprite+source.sprite_count&&sprite<rects.size();++sprite){
        const auto& rect=rects[sprite];
        const i32 left=i32(std::lround(rect.x)),top=i32(std::lround(rect.y));
        const i32 width=i32(std::lround(rect.width)),height=i32(std::lround(rect.height));
        if(left<0||top<0||width<=0||height<=0||u32(left+width)>image.width||u32(top+height)>image.height)return false;
        if(u32(left)>=patch_width||u32(top)>=patch_height)continue;
        const i32 copy_width=std::min(width,i32(patch_width)-left),copy_height=std::min(height,i32(patch_height)-top);
        if(analyze_rgba(patch,patch_width*4,left,top,copy_width,copy_height)==RgbaAlphaState::Empty)continue;
        const auto destination_alpha=analyze_rgba(target,i32(image.width)*4,left,top,copy_width,copy_height);
        for(i32 y=top;y<top+copy_height;++y){
            u8* destination_row=target.data()+size_t(y)*image.width*4+size_t(left)*4;
            const u8* source_row=patch.data()+size_t(y)*patch_width*4+size_t(left)*4;
            if(destination_alpha==RgbaAlphaState::Opaque)blend_rgba_over_opaque(destination_row,source_row,copy_width);
            else std::memcpy(destination_row,source_row,size_t(copy_width)*4);
        }
        patched=true;
    }
    if(!patched)return false;
    return image.from_rgba(target.data(),image.width,image.height,image.format);
}
}
#else
namespace { bool load_override(const AnmResource&,u32,bool,TexturePixels&){return false;} }
#endif
AnmLibrary::~AnmLibrary() { for(i32 i=0;i<256;++i)release(i);clear_preloads(); }
void AnmLibrary::clear_preloads(){
    // Active entries refer to their pristine template. Release them before
    // dropping the cache; ordinary scene release keeps resident textures alive.
    for(i32 i=0;i<256;++i)if(files[i]&&files[i]->prepared)release(i);
    for(const auto& entry:prepared)for(u32 handle:entry->resident)textures.release(handle);
    prepared.clear();prepared_bytes=0;
}
AnmLoaded* AnmLibrary::get(i32 index) noexcept { return index>=0&&index<256&&files[index]?&files[index]->resource.view():nullptr; }
const AnmResource* AnmLibrary::resource(i32 index) const noexcept { return index>=0&&index<256&&files[index]?&files[index]->resource:nullptr; }
void AnmLibrary::release(i32 index) {
    if(index<0||index>=256||!files[index])return;
    for(auto& entry:files[index]->textures)textures.release(entry.texture);
    files[index].reset();
}
AnmLoaded* AnmLibrary::load(i32 index,Archive& archive,const char* name,bool deferred) {
    if(index<0||index>=25)return nullptr;
    release(index);
    std::vector<u8> bytes;
    if(!archive.read(name,bytes))return nullptr;
    return load(index,bytes.data(),bytes.size(),deferred);
}
AnmLoaded* AnmLibrary::load(i32 index,const u8* bytes,u32 size,bool deferred) {
    if(index<0||index>=25)return nullptr;
    release(index);
    auto entry=std::make_unique<Entry>();
    for(const auto& candidate:prepared)if(candidate->force_16bit==force_16bit&&candidate->resource.data().size()==size&&!std::memcmp(candidate->resource.data().data(),bytes,size)){entry->prepared=candidate.get();break;}
    if(entry->prepared){entry->resource.clone_from(entry->prepared->resource,index);++cache_hits;}
    else if(!entry->resource.load(index,bytes,size))return nullptr;
    // Every original TH08 ANM embeds THTX pixels or allocates an empty atlas.
    for(const auto& source:entry->resource.textures())if(!source.embedded&&!source.empty)return nullptr;
    entry->textures.resize(entry->resource.textures().size());
    auto& view=entry->resource.view();view.textures=entry->textures.data();
    files[index]=std::move(entry);
    if(deferred) view.numberEntriesToBeLoaded=1;
    else for(u32 i=0;i<files[index]->textures.size();++i)if(!materialize(*files[index],i)){release(index);return nullptr;}
    return get(index);
}
bool AnmLibrary::materialize(Entry& entry,u32 index) {
    if(index>=entry.textures.size())return false;
    const auto& source=entry.resource.textures()[index];
    if(entry.prepared){
        const u32 handle=entry.prepared->resident[index];auto* resident=textures.get(handle);
        // Only an idle static texture may be borrowed. Text/capture atlases and
        // simultaneous scene instances always retain independent writable data.
        if(resident&&resident->references==1){
            const auto& pristine=entry.prepared->pixels[index];
            if(resident->revision!=entry.prepared->resident_revision[index]||resident->image.pixels!=pristine.pixels){resident->image=pristine;textures.changed(handle);entry.prepared->resident_revision[index]=resident->revision;}
            textures.retain(handle);textures.release(entry.textures[index].texture);
            entry.textures[index].texture=handle;++texture_cache_hits;
            return entry.resource.configure_texture(index,handle,resident->image.width,resident->image.height);
        }
    }
    TexturePixels image;
    if(entry.prepared)image=entry.prepared->pixels[index];
    else if(source.embedded) {
        if(!image.from_anm(entry.resource.data().data()+source.pixel_offset-16,source.pixel_size+16,source.format,force_16bit))return false;
        load_override(entry.resource,index,force_16bit,image);
    } else {
        // Original empty atlases bypass GetAnmFormat's force-16-bit override.
        u32 format=TexturePixels::anm_format(source.format,false);if(source.format==0)format=21;
        if(!image.create(source.width,source.height,format))return false;
    }
    const u32 width=image.width,height=image.height,handle=textures.insert(std::move(image),source.priority,source.empty);
    if(!handle)return false;
    textures.release(entry.textures[index].texture);entry.textures[index].texture=handle;
    return entry.resource.configure_texture(index,handle,width,height);
}
bool AnmLibrary::preload(const u8* bytes,u32 size){
    const bool reduced=force_16bit;{
        for(const auto& entry:prepared)if(entry->force_16bit==reduced&&entry->resource.data().size()==size&&!std::memcmp(entry->resource.data().data(),bytes,size))return true;
        auto entry=std::make_unique<Prepared>();entry->force_16bit=reduced;if(!entry->resource.load(0,bytes,size))return false;
        u32 cost=size;
        for(u32 i=0;i<entry->resource.textures().size();++i){
            const auto& source=entry->resource.textures()[i];
            TexturePixels image;
            if(source.embedded){
                if(!image.from_anm(entry->resource.data().data()+source.pixel_offset-16,source.pixel_size+16,source.format,reduced))return false;
                load_override(entry->resource,i,reduced,image);
            }
            else if(source.empty){u32 format=TexturePixels::anm_format(source.format,false);if(source.format==0)format=21;if(!image.create(source.width,source.height,format))return false;}
            else return false;
            cost+=image.pixels.size();entry->pixels.push_back(std::move(image));
        }
        // Bound the shared/menu and opening-stage cache on mobile devices.
        if(prepared_bytes+cost>32*1024*1024)return false;
        for(u32 i=0;i<entry->pixels.size();++i){
            const auto& source=entry->resource.textures()[i];u32 handle=0;
            if(!source.empty){auto pixels=entry->pixels[i];handle=textures.insert(std::move(pixels),source.priority);}
            entry->resident.push_back(handle);entry->resident_revision.push_back(handle?textures.get(handle)->revision:0);if(handle)textures.prepare(handle);
        }
        prepared_bytes+=cost;prepared.push_back(std::move(entry));
    }return true;
}
bool AnmLibrary::postload(i32 index) {
    auto* view=get(index);if(!view)return false;
    if(view->numberEntriesToBeLoaded==0)return true;
    const u32 entry=u32(view->numberEntriesToBeLoaded-1);
    if(!materialize(*files[index],entry)){view->numberEntriesToBeLoaded=0;return false;}
    view->numberEntriesToBeLoaded=entry+1<files[index]->textures.size()?i32(entry+2):0;
    return true;
}
bool AnmLibrary::service() {
    for(i32 i=0;i<256;++i)if(files[i]&&files[i]->resource.view().numberEntriesToBeLoaded&&!postload(i))return false;
    return true;
}
bool AnmLibrary::start(i32 index,i32 script,AnmVm& vm,AnmExecutor& executor,bool reset_position) {
    auto* entry=get(index);
    if(!entry||script<0||u32(script)>=files[index]->resource.script_count())return false;
    vm.scriptIndex=i16(script);
    if(reset_position) { vm.pos={};vm.pos2={};vm.fontWidth=vm.fontHeight=15; }
    executor.start(*entry,vm,entry->scripts[script]);
    return true;
}
bool AnmLibrary::has_texture(const AnmVm& vm) const noexcept {
    if(!vm.loadedSprite||vm.loadedSprite->anmIdx<0||vm.loadedSprite->anmIdx>=256)return false;
    const auto& file=files[vm.loadedSprite->anmIdx];
    return file && file->textures.data()!=nullptr;
}
}
