#include "AnmResources.hpp"
namespace th10 {
// 0x4470c0. Keep the allocated slot on read/validation failure, as the original
// resource owner later releases it. Texture bytes are separate allocations.
AnmFile* AnmManager::open(i32 slot,const char* filename,AnmResourceEnvironment& env){
    if(slot<0||slot>=33){env.report(AnmResourceError::InvalidSlot);return nullptr;}
    auto* data=env.read_file(filename,false,nullptr);auto* file=env.allocate_file();__builtin_memset(file,0,sizeof(*file));files[slot]=file;if(!data)return nullptr;
    file->file_index=slot;file->loaded=data;__builtin_memcpy(file->name,filename,std::strlen(filename)+1);
    const auto* chunk=reinterpret_cast<const AnmChunk*>(data);i32 sprites=chunk->sprite_count,scripts=chunk->script_count;u32 textures=1;
    while(chunk->next_offset){chunk=chunk->next();sprites=wrapping_add(sprites,chunk->sprite_count);scripts=wrapping_add(scripts,chunk->script_count);++textures;}
    file->texture_count=textures;file->textures=static_cast<AnmTexture*>(env.allocate_bytes(textures*sizeof(AnmTexture)));__builtin_memset(file->textures,0,textures*sizeof(AnmTexture));
    file->sprites=static_cast<AnmSprite*>(env.allocate_bytes(static_cast<u32>(sprites)*sizeof(AnmSprite)));file->scripts=static_cast<AnmInstruction**>(env.allocate_bytes(static_cast<u32>(scripts)*4));file->script_count=scripts;file->sprite_count=sprites;
    chunk=reinterpret_cast<const AnmChunk*>(data);i32 index=0;for(;;){if(file->prepare_chunk(index,chunk,env)<0)return nullptr;if(!chunk->next_offset)return file;chunk=chunk->next();++index;}
}
// 0x447280. The platform pumps its loader at this blocking resource barrier.
// An already-open slot returns immediately even if its upload is in progress.
AnmFile* AnmManager::load(i32 slot,const char* name,AnmResourceEnvironment& env){
    if(slot<0||slot>=33){env.report(AnmResourceError::InvalidSlot);return nullptr;}if(files[slot])return files[slot];
#ifdef TH_NATIVE_PLATFORM
    if(auto* ready=env.prepared_file(slot,name)){files[slot]=ready;return ready;}
#endif
    auto* file=open(slot,name,env);if(!file)return nullptr;file->unavailable=1;
    do{if(*env.loader_flags&128)return file;env.wait_for_loading(*this,*file);}while(file->unavailable);return file;
}
// 0x4472e0.
i32 AnmFile::prepare_chunk(i32 index,const AnmChunk* chunk,AnmResourceEnvironment& env){
    if(!chunk){env.report(AnmResourceError::MissingHeader);return -1;}if(chunk->version!=4){env.report(AnmResourceError::InvalidVersion);return -1;}
    if(!chunk->embedded_texture&&*chunk->texture_name()!='@'){
        u32 size;auto* bytes=env.read_file(chunk->texture_name(),true,&size);if(!bytes){env.report(AnmResourceError::MissingTexture);return -1;}textures[index].source_size=size;textures[index].source=bytes;
    }
    return 1;
}
// 0x4473c0. Complete one entry at a time, retaining aggregate sprite/script
// offsets while scanning the linked file headers.
bool AnmFile::complete_next(AnmResourceEnvironment& env){
    const auto* chunk=reinterpret_cast<const AnmChunk*>(loaded);i32 texture=0,sprite=0,script=0;bool completed=false;
    for(;;){
        if(static_cast<u32>(texture)==unavailable-1){if(env.materialize(*this,texture,sprite,script,chunk)<0){unavailable=0;return false;}completed=true;}
        sprite=wrapping_add(sprite,chunk->sprite_count);script=wrapping_add(script,chunk->script_count);++texture;
        if(!chunk->next_offset){unavailable=0;return true;}chunk=chunk->next();
        if(static_cast<u32>(texture)==unavailable||completed){++unavailable;return true;}
    }
}
// 0x4493e0. Mark both registries; actual VM destruction occurs on update.
void AnmRegistry::discard_file(AnmFile* file) const noexcept {
    ListNode<AnmVm>* heads[]={world_head,ui_head};for(auto* head:heads)for(auto* node=head;node;node=node->next)if(node->value->animation_file==file)node->value->flags|=0x04000000;
}
// 0x447810. The loaded-file pointer gates the entire destruction sequence.
void AnmFile::release(AnmResourceEnvironment& env){
    if(!loaded)return;env.registry->discard_file(this);
    for(i32 index=0;index<texture_count;++index){auto& texture=textures[index];if(texture.handle){env.release_texture(texture.handle);texture.handle=nullptr;}if(texture.source){env.release_bytes(texture.source);texture.source=nullptr;}}
    if(textures){env.release_bytes(textures);textures=nullptr;}if(sprites){env.release_bytes(sprites);sprites=nullptr;}if(scripts){env.release_bytes(scripts);scripts=nullptr;}if(extra_data){env.release_bytes(extra_data);extra_data=nullptr;}if(loaded){env.release_bytes(loaded);loaded=nullptr;}
}
// 0x4477d0 / 0x447790.
void AnmManager::unload(i32 slot,AnmResourceEnvironment& env){if(slot<0||slot>=33||!files[slot])return;files[slot]->release(env);env.release_file(files[slot]);files[slot]=nullptr;}
bool AnmManager::resources_ready() const noexcept {for(auto* file:files)if(file&&(file->discard_request||file->unavailable))return false;return true;}
// 0x447700. A stale deletion marker does not dereference the released slot;
// the executable does so on that invalid path, which would fault on Windows.
i32 AnmManager::process_loading(AnmResourceEnvironment& env){
    for(i32 slot=0;slot<33;++slot)if(auto* file=files[slot]){if(file->discard_request)unload(slot,env);else if(file->unavailable)return file->complete_next(env)?0:-1;}return 0;
}
// 0x447940. Preserve input aliasing and each float storage rounding boundary.
void AnmFile::set_sprite(i32 index,const AnmSprite& source) noexcept {
    sprites[index]=source;auto& sprite=sprites[index];
    sprite.u0=Scalar::div(sprite.left,sprite.texture_width);sprite.u1=Scalar::div(sprite.right,sprite.texture_width);
    sprite.v0=Scalar::div(sprite.top,sprite.texture_height);sprite.v1=Scalar::div(sprite.bottom,sprite.texture_height);
    sprite.width=((number(sprite.right)-number(sprite.left))/number(source.scale_x)).to_float();sprite.height=((number(sprite.bottom)-number(sprite.top))/number(source.scale_y)).to_float();
}
}
