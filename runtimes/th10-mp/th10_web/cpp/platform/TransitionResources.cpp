#include "AnimationResources.hpp"
#ifdef TH_NATIVE_PLATFORM
namespace th10::browser {
// TH10 uses title.anm for the title, Music Room and result menus. Keep decoded
// pixels and parsed tables; each scene gets its own writable textures and VMs.
bool AnimationResources::preload_transition(u32 index){
    static const char* names[]={"title.anm","title_v.anm","bullet.anm","front.anm","pl00.anm","pl01.anm"};if(index>=6)return false;
    for(const auto& entry:prepared)if(entry.name==names[index]&&entry.display==display)return true;
    constexpr i32 scratch=32;if(manager.files[scratch])return false;
    auto* file=manager.open(scratch,names[index],*this);if(!file){manager.unload(scratch,*this);return false;}
    PreparedAnimation entry;entry.name=names[index];entry.display=display;entry.raw.assign(file->loaded,file->loaded+last_read_size);
    file->unavailable=1;while(file->unavailable)if(!file->complete_next(*this)){manager.unload(scratch,*this);return false;}
    u32 cost=entry.raw.size();const auto* chunk=reinterpret_cast<const AnmChunk*>(file->loaded);
    for(i32 i=0;i<file->texture_count;++i){
        auto* surface=textures.get_surface(file->textures[i].handle);PreparedTexture image;image.description=textures.describe_surface(surface);const auto mapped=textures.lock_surface(surface);
        image.pitch=mapped.pitch;image.bytes_per_pixel=file->textures[i].bytes_per_pixel;image.pixels.assign(mapped.pixels,mapped.pixels+mapped.pitch*image.description.height);cost+=image.pixels.size();
        textures.unlock_surface(surface);textures.release_surface(surface);
        // Keep static GPU textures resident like the reference runtimes. '@'
        // text/capture atlases must start with independent pristine contents.
        if(*chunk->texture_name()!='@'){image.resident=file->textures[i].handle;device.retain_resource(image.resident);preload(image.resident);image.revision=device.resource_revision(image.resident);}
        entry.textures.push_back(std::move(image));if(chunk->next_offset)chunk=chunk->next();
    }
    entry.sprites.assign(file->sprites,file->sprites+file->sprite_count);
    for(const auto& sprite:entry.sprites){u32 texture=0;while(texture<u32(file->texture_count)&&file->textures[texture].handle!=sprite.texture)++texture;entry.sprite_textures.push_back(texture);}
    for(i32 i=0;i<file->script_count;++i)entry.script_offsets.push_back(reinterpret_cast<const u8*>(file->scripts[i])-file->loaded);
    manager.unload(scratch,*this);if(prepared_bytes+cost>32*1024*1024){for(const auto& texture:entry.textures)if(texture.resident)device.release_resource(texture.resident);return false;}
    prepared_bytes+=cost;prepared.push_back(std::move(entry));return true;
}
AnmFile* AnimationResources::prepared_file(i32 slot,const char* name){
    PreparedAnimation* source=nullptr;for(auto& entry:prepared)if(entry.name==name&&entry.display==display){source=&entry;break;}if(!source)return nullptr;
    auto* file=allocate_file();*file={};file->file_index=slot;std::strncpy(file->name,name,sizeof(file->name)-1);
    file->loaded=static_cast<u8*>(allocate_bytes(source->raw.size()));std::memcpy(file->loaded,source->raw.data(),source->raw.size());
    file->texture_count=source->textures.size();file->sprite_count=source->sprites.size();file->script_count=source->script_offsets.size();
    file->textures=static_cast<AnmTexture*>(allocate_bytes(file->texture_count*sizeof(AnmTexture)));std::memset(file->textures,0,file->texture_count*sizeof(AnmTexture));
    for(i32 i=0;i<file->texture_count;++i){auto& image=source->textures[i];auto& texture=file->textures[i];
        if(image.resident){
            const u32 refs=device.retain_resource(image.resident);
            if(refs==2){
                if(device.resource_revision(image.resident)!=image.revision){
                    auto* surface=textures.get_surface(image.resident);const auto mapped=textures.lock_surface(surface);
                    for(u32 y=0;y<image.description.height;++y)std::memcpy(mapped.pixels+y*mapped.pitch,image.pixels.data()+y*image.pitch,std::min<u32>(mapped.pitch,image.pitch));
                    textures.unlock_surface(surface);textures.release_surface(surface);image.revision=device.resource_revision(image.resident);
                }
                texture.handle=image.resident;texture.bytes_per_pixel=image.bytes_per_pixel;++resident_hits;continue;
            }
            device.release_resource(image.resident);
        }
        if(textures.create_surface_texture(texture,image.description.width,image.description.height,image.description.format)){file->release(*this);release_file(file);return nullptr;}
        texture.bytes_per_pixel=image.bytes_per_pixel;auto* surface=textures.get_surface(texture.handle);const auto mapped=textures.lock_surface(surface);
        for(u32 y=0;y<image.description.height;++y)std::memcpy(mapped.pixels+y*mapped.pitch,image.pixels.data()+y*image.pitch,std::min<u32>(mapped.pitch,image.pitch));
        textures.unlock_surface(surface);textures.release_surface(surface);preload(texture.handle);
    }
    file->sprites=static_cast<AnmSprite*>(allocate_bytes(file->sprite_count*sizeof(AnmSprite)));
    for(i32 i=0;i<file->sprite_count;++i){file->sprites[i]=source->sprites[i];file->sprites[i].file_index=slot;file->sprites[i].texture=file->textures[source->sprite_textures[i]].handle;}
    file->scripts=static_cast<AnmInstruction**>(allocate_bytes(file->script_count*sizeof(AnmInstruction*)));
    for(i32 i=0;i<file->script_count;++i)file->scripts[i]=reinterpret_cast<AnmInstruction*>(file->loaded+source->script_offsets[i]);
    ++cache_hits;return file;
}
}
#endif
