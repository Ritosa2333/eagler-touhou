#include "TextureJournal.hpp"
#include "../platform/BrowserRuntime.hpp"
#include "../platform/PlatformDevices.hpp"
#include "../game/Presentation.hpp"

namespace th08::multiplayer {
bool TextureJournal::Bind(BrowserRuntime& value){
    if(runtime||value.app.textures.journal||!touhou::sdl::current())return false;
    runtime=&value;value.app.textures.journal=this;failed=false;return true;
}
bool TextureJournal::MayMutate(){
    if(failed)return false;
    return restoring||open!=Invalid||frames.empty()||presentation::render_only?true:Fail();
}
bool TextureJournal::BeginFrame(u32 number){
    if(!runtime||failed||open!=Invalid||frames.size()>=History||
       (!frames.empty()&&number!=frames.back().number+1))return Fail();
    runtime->flush();
    frames.emplace_back();auto& f=frames.back();f.number=open=number;
    f.slots=runtime->app.textures.records.size();f.live=runtime->app.textures.live;
    f.surfaces=runtime->surfaces;f.captured=runtime->captured;f.capture_failed=runtime->capture_failed;
    const auto& c=runtime->pending_capture;f.capture_target=c.target;f.source=c.source;f.destination=c.destination;f.triangle=c.triangle;
    f.graphics=touhou::sdl::current()->state;
    // The backbuffer can be the source of a later pause/capture; save it before
    // any speculative draw, not after a wrong branch has painted over it.
    return Touch(runtime->back,true);
}
bool TextureJournal::Touch(u32 handle,bool pixels){
    if(!runtime||!MayMutate())return false;
    if(restoring||frames.empty())return true;
    // A render-only pass can update text/capture textures after the logical
    // frame closes. Its before-images belong to that last frame's output,
    // never to a new simulation tick or an untracked resource mutation.
    if(open==Invalid&&!presentation::render_only)return Fail();
    auto& f=frames.back();auto& store=runtime->app.textures;
    if(!handle||handle>store.records.size()||!store.records[handle-1])return Fail();
    if(handle>f.slots)return true; // this frame owns the entire new texture
    auto found=f.images.find(handle);
    if(found==f.images.end()){
        auto owner=store.records[handle-1];Image saved;
        saved.owner=owner;saved.references=owner->references;saved.revision=owner->revision;
        saved.priority=owner->priority;saved.target=owner->render_target;
        found=f.images.emplace(handle,std::move(saved)).first;
    }
    auto& saved=found->second;
    if(pixels&&!saved.has_pixels){
        const auto size=saved.owner->image.pixels.size();
        if(size>32*1024*1024||f.bytes>32*1024*1024-size)return Fail();
        graphics_device().read(handle);
        saved.pixels=saved.owner->image;saved.has_pixels=true;f.bytes+=size;
    }
    return true;
}
bool TextureJournal::EndFrame(){if(failed||open==Invalid)return false;open=Invalid;return true;}
bool TextureJournal::UndoTo(u32 number){
    if(!runtime||failed||open!=Invalid)return false;
    auto it=frames.begin();while(it!=frames.end()&&it->number!=number)++it;
    if(it==frames.end())return false;
    runtime->flush();restoring=true;auto& store=runtime->app.textures;
    while(!frames.empty()&&frames.back().number>=number){
        auto& f=frames.back();
        for(std::size_t slot=f.slots;slot<store.records.size();++slot)graphics_device().invalidate_texture(u32(slot+1));
        store.records.resize(f.slots);
        for(auto& [handle,image]:f.images){
            store.records[handle-1]=image.owner;auto& restored=*image.owner;
            restored.references=image.references;restored.revision=image.revision;
            restored.priority=image.priority;restored.render_target=image.target;
            if(image.has_pixels)restored.image=image.pixels;
            graphics_device().invalidate_texture(handle);
        }
        store.live=f.live;runtime->surfaces=f.surfaces;
        runtime->pending_capture={f.capture_target,f.source,f.destination,f.triangle};
        runtime->captured=f.captured;runtime->capture_failed=f.capture_failed;
        touhou::sdl::current()->state=f.graphics;frames.pop_back();
    }
    restoring=false;return true;
}
void TextureJournal::DiscardBefore(u32 frame){if(open==Invalid)while(!frames.empty()&&frames.front().number<frame)frames.pop_front();}
void TextureJournal::Clear(){
    if(runtime&&runtime->app.textures.journal==this)runtime->app.textures.journal=nullptr;
    frames.clear();runtime=nullptr;open=Invalid;failed=restoring=false;
}
std::size_t TextureJournal::BytesForFrame(u32 number)const{for(const auto& f:frames)if(f.number==number)return f.bytes;return 0;}
}
