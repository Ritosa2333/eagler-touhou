#include "AnimationResources.hpp"
#include "../game/TriangleCoefficients.hpp"
#include "TextureResample.hpp"
#include <new>
#include <cstdlib>
using namespace th10;
#define TEXTURE_EXPORT(name) extern "C" __attribute__((export_name(name)))
TEXTURE_EXPORT("textures_filter_table") u8* textures_filter_table(u32 source,u32 destination,u32 wrap){return TriangleCoefficients::create(source,destination,wrap!=0);}
TEXTURE_EXPORT("textures_resample") i32 textures_resample(PixelSurface* output,const TextureRect* destination,const PixelSurface* input,const TextureRect* source,u32 wrap){return browser::TextureResample::triangle(*output,*destination,*input,*source,wrap&1,wrap&2);}
TEXTURE_EXPORT("textures_copy_pixels") i32 textures_copy_pixels(PixelSurface* destination,const TextureRect* target,const u8* source,u32 format,i32 pitch,const TextureRect* region){return browser::PixelCopy::copy(*destination,*target,source,format,pitch,*region);}
TEXTURE_EXPORT("textures_embedded") i32 textures_embedded(browser::GraphicsDevice* device,AnmTexture* texture,const u8* source,i32 width,i32 height,i32 format,u32 flags){browser::Textures platform(*device,flags);return texture->create_embedded(source,width,height,format,platform);}
TEXTURE_EXPORT("textures_empty") i32 textures_empty(browser::GraphicsDevice* device,AnmTexture* texture,i32 width,i32 height,i32 format,u32 flags){browser::Textures platform(*device,flags);return texture->create_empty(width,height,format,platform);}
TEXTURE_EXPORT("animations_create") browser::AnimationResources* animations_create(browser::FileSystem* files,browser::GraphicsDevice* device,AnmManager* manager,u32 display){auto* bytes=std::malloc(sizeof(browser::AnimationResources));return bytes?new(bytes)browser::AnimationResources(*files,*device,*manager,display):nullptr;}
TEXTURE_EXPORT("animations_destroy") void animations_destroy(browser::AnimationResources* resources){if(resources){for(i32 i=0;i<33;++i)resources->manager.unload(i,*resources);resources->~AnimationResources();std::free(resources);}}
TEXTURE_EXPORT("animations_load") AnmFile* animations_load(browser::AnimationResources* resources,i32 slot,const char* name){return resources->manager.load(slot,name,*resources);}
TEXTURE_EXPORT("animations_unload") void animations_unload(browser::AnimationResources* resources,i32 slot){resources->manager.unload(slot,*resources);}
TEXTURE_EXPORT("animations_error") i32 animations_error(browser::AnimationResources* resources){return resources->last_error;}
