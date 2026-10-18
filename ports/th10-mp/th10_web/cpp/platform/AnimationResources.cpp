#include "AnimationResources.hpp"
#include "../game/RuntimeOverride.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#if defined(TH_NATIVE_PLATFORM)&&defined(TH_ENABLE_THCRAP)
namespace th10 { bool sdl_decode_rgba(const u8*,u32,u32&,u32&,std::vector<u8>&); }
#endif
namespace th10::browser {
namespace {
u32 address(const void* value){return static_cast<u32>(reinterpret_cast<uintptr_t>(value));}
template<class T>T read_le(const u8* bytes){T value;std::memcpy(&value,bytes,sizeof(value));return value;}
}
AnimationResources::AnimationResources(FileSystem& f,GraphicsDevice& d,AnmManager& m,u32 flags):files(f),device(d),manager(m),display(flags),textures(d,display){registry=&m.registry;loader_flags=&loading;}
AnimationResources::~AnimationResources(){
#ifdef TH_NATIVE_PLATFORM
    for(const auto& entry:prepared)for(const auto& texture:entry.textures)if(texture.resident)device.release_resource(texture.resident);
#endif
}
AnmFile* AnimationResources::allocate_file(){return static_cast<AnmFile*>(std::malloc(sizeof(AnmFile)));}
void* AnimationResources::allocate_bytes(u32 size){return std::malloc(size);}
void AnimationResources::release_file(AnmFile* file){std::free(file);}
void AnimationResources::release_bytes(void* bytes){std::free(bytes);}
u8* AnimationResources::read_file(const char* name,bool external,u32* size){
#ifdef TH_NATIVE_PLATFORM
    u32 n=0;auto* data=ResourceFiles{files}.load(name,&n,external);if(size)*size=n;if(!external)last_read_size=n;return data;
#else
    return ResourceFiles{files}.load(name,size,external);
#endif
}
void AnimationResources::release_texture(void* texture){device.release_resource(texture);}
i32 AnimationResources::materialize(AnmFile& file,i32 texture,i32 sprite,i32 script,const AnmChunk* chunk){return file.materialize(texture,sprite,script,chunk,*this);}
// thcrap ships PNG replacements for the bitmaps embedded in an ANM. Read the
// override through the same /thcrap/th10 namespace used by the rest of the
// pack, decode it to RGBA and blend it over the uploaded surface at each
// sprite rectangle. The surface may use any ANM format, so the region is
// copied to a temporary RGBA8 buffer, composited there and converted back.
bool AnimationResources::override_embedded(AnmTexture& texture,const char* texture_name,const AnmChunk* chunk){
#if defined(TH_NATIVE_PLATFORM)&&defined(TH_ENABLE_THCRAP)
    if(!texture_name||!*texture_name||!chunk)return false;
    std::string name(texture_name);for(char& c:name)if(c=='\\')c='/';
    // Only the overlay font atlas must keep its original index-to-glyph layout;
    // other "ascii/" assets (e.g. the pause menu) are ordinary textures.
    if(name.size()>=10&&name.compare(name.size()-10,10,"ascii/ascii.png")==0)return false;
    std::vector<u8> file;
    if(!RuntimeOverride::Read(name.c_str(),file))return false;
    u32 patch_width=0,patch_height=0;std::vector<u8> patch;
    if(!sdl_decode_rgba(file.data(),static_cast<u32>(file.size()),patch_width,patch_height,patch))return false;
    if(!patch_width||!patch_height)return false;
    auto* surface=textures.get_surface(texture.handle);if(!surface)return false;
    const auto description=textures.describe_surface(surface);const auto lock=textures.lock_surface(surface);
    bool patched=false;
    if(lock.pixels&&description.width&&description.height){
        PixelSurface output{description.format,description.width,description.height,lock.pitch,lock.pixels};
        std::vector<u8> region;
        const auto* records=reinterpret_cast<const u8*>(chunk)+sizeof(AnmChunk);
        for(i32 index=0;index<chunk->sprite_count;++index,records+=4){
            const auto* record=reinterpret_cast<const u8*>(chunk)+read_le<u32>(records);
            const i32 left=static_cast<i32>(std::lround(read_le<float>(record+4)));
            const i32 top=static_cast<i32>(std::lround(read_le<float>(record+8)));
            const i32 width=static_cast<i32>(std::lround(read_le<float>(record+12)));
            const i32 height=static_cast<i32>(std::lround(read_le<float>(record+16)));
            if(left<0||top<0||width<=0||height<=0)continue;
            const i32 copy_width=std::min(width,static_cast<i32>(patch_width)-left);
            const i32 copy_height=std::min(height,static_cast<i32>(patch_height)-top);
            if(copy_width<=0||copy_height<=0)continue;
            if(left+copy_width>static_cast<i32>(description.width)||top+copy_height>static_cast<i32>(description.height))continue;
            const TextureRect destination_region{left,top,left+copy_width,top+copy_height};
            const TextureRect source_region{0,0,copy_width,copy_height};
            region.assign(static_cast<std::size_t>(copy_width)*static_cast<std::size_t>(copy_height)*4,0);
            PixelSurface temporary{21,static_cast<u32>(copy_width),static_cast<u32>(copy_height),copy_width*4,region.data()};
            if(PixelCopy::copy(temporary,source_region,lock.pixels,description.format,lock.pitch,destination_region))continue;
            // thcrap's default "auto" mode analyzes the alpha of both the
            // replacement and the destination sprite. A fully opaque
            // destination is alpha-blended onto; anything else is fully
            // overwritten so transparent patch pixels erase the original.
            bool replacement_empty=true,destination_opaque=true;
            for(i32 y=0;y<copy_height;++y){
                const u8* source=patch.data()+(static_cast<std::size_t>(top+y)*patch_width+left)*4;
                const u8* destination=region.data()+static_cast<std::size_t>(y)*static_cast<std::size_t>(copy_width)*4;
                for(i32 x=0;x<copy_width;++x,source+=4,destination+=4){
                    if(source[3])replacement_empty=false;
                    if(destination[3]!=255)destination_opaque=false;
                }
            }
            if(replacement_empty)continue;
            for(i32 y=0;y<copy_height;++y){
                u8* destination=region.data()+static_cast<std::size_t>(y)*static_cast<std::size_t>(copy_width)*4;
                const u8* source=patch.data()+(static_cast<std::size_t>(top+y)*patch_width+left)*4;
                for(i32 x=0;x<copy_width;++x,destination+=4,source+=4){
                    if(!destination_opaque){
                        destination[0]=source[2];destination[1]=source[1];destination[2]=source[0];destination[3]=source[3];
                        continue;
                    }
                    const i32 alpha=source[3];if(!alpha)continue;const i32 weight=255-alpha;
                    destination[0]=static_cast<u8>((destination[0]*weight+source[2]*alpha)>>8);
                    destination[1]=static_cast<u8>((destination[1]*weight+source[1]*alpha)>>8);
                    destination[2]=static_cast<u8>((destination[2]*weight+source[0]*alpha)>>8);
                    destination[3]=alpha==255?u8(255):static_cast<u8>(std::min<i32>(destination[3]+alpha,255));
                }
            }
            if(PixelCopy::copy(output,destination_region,region.data(),21,copy_width*4,source_region))continue;
            patched=true;
        }
    }
    textures.unlock_surface(surface);textures.release_surface(surface);
    return patched;
#else
    (void)texture;(void)texture_name;(void)chunk;return false;
#endif
}
void AnimationResources::wait_for_loading(AnmManager& owner,AnmFile&){owner.process_loading(*this);}
void AnimationResources::set_priority(void* texture,u32 priority){device.resource_priority(texture,priority);}
void AnimationResources::preload(void* texture){device.preload_resource(texture);}
AnmTextureDimensions AnimationResources::dimensions(void* texture){void* surface=textures.get_surface(texture);const auto desc=textures.describe_surface(surface);textures.release_surface(surface);return {desc.width,desc.height};}
}
