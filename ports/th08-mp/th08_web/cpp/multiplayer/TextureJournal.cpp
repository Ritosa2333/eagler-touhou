#include "TextureJournal.hpp"
#include "../platform/BrowserRuntime.hpp"
#include "../platform/PlatformDevices.hpp"
#include "../game/Presentation.hpp"

namespace th08::multiplayer {
#ifdef TH_MULTIPLAYER_FIXTURES
namespace {bool texture_coalesce_enabled=true,back_metadata_only_enabled=false;}
bool fixture_texture_coalesce(bool enabled){texture_coalesce_enabled=enabled;return texture_coalesce_enabled;}
bool fixture_back_metadata_only(bool enabled){back_metadata_only_enabled=enabled;return back_metadata_only_enabled;}
#endif
bool TextureJournal::Bind(BrowserRuntime& value){
    if(runtime||value.app.textures.journal||!touhou::sdl::current())return false;
    runtime=&value;value.app.textures.journal=this;failed=false;
#ifdef TH_MULTIPLAYER_FIXTURES
    back_metadata_only=back_metadata_only_enabled;
#endif
    return true;
}
bool TextureJournal::MayMutate(){
    if(failed)return false;
    return restoring||open!=Invalid||frames.empty()||presentation::render_only?true:Fail();
}
bool TextureJournal::BeginFrame(u32 number,bool extend){
    if(!runtime||failed||open!=Invalid||(!extend&&frames.size()>=History)||
       (extend&&frames.empty())||(!frames.empty()&&number!=frames.back().end+1))return Fail();
    runtime->flush();
    if(extend){frames.back().end=open=number;return true;}
    frames.emplace_back();auto& f=frames.back();f.number=f.end=open=number;
    f.slots=runtime->app.textures.records.size();f.live=runtime->app.textures.live;
    f.surfaces=runtime->surfaces;f.captured=runtime->captured;f.capture_failed=runtime->capture_failed;
    const auto& c=runtime->pending_capture;f.capture_target=c.target;f.source=c.source;f.destination=c.destination;f.triangle=c.triangle;
    f.graphics=touhou::sdl::current()->state;
    // The backbuffer can be the source of a later pause/capture; save it before
    // any speculative draw, not after a wrong branch has painted over it.
    return Touch(runtime->back,!back_metadata_only);
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
    if(pixels&&handle==runtime->back&&back_metadata_only)return true;
    if(pixels&&!saved.has_pixels){
        const auto size=saved.owner->image.pixels.size();
        if(size>32*1024*1024||f.bytes>32*1024*1024-size)return Fail();
        // Preserve the backbuffer on the GPU. A synchronous full-screen
        // readback here stalls every forward AND resimulated logical frame.
        // Other textures can have CPU writers, so retain their byte snapshots.
        if(handle==runtime->back){
            saved.gpu_image=touhou::sdl::current()->save_color(handle);
            if(!saved.gpu_image)return Fail();
        }else{
            graphics_device().read(handle);saved.pixels=saved.owner->image;
        }
        saved.has_pixels=true;f.bytes+=size;
    }
    return true;
}
bool TextureJournal::EndFrame(){if(failed||open==Invalid)return false;open=Invalid;return true;}
bool TextureJournal::UndoTo(u32 number){
    if(!runtime||failed||open!=Invalid)return false;
    auto it=frames.begin();while(it!=frames.end()&&it->number!=number)++it;
    if(it==frames.end())return false;
    runtime->flush();restoring=true;auto& store=runtime->app.textures;
    // Every frame saves the backbuffer before speculative drawing. When its
    // handle and owner are stable, only the target frame's before-image can
    // survive this undo; restoring newer full images would immediately be
    // overwritten by the next older one.
    const u32 back=runtime->back;
    const auto target_back=it->images.find(back);
    bool coalesce_back=target_back!=it->images.end()&&target_back->second.gpu_image;
#ifdef TH_MULTIPLAYER_FIXTURES
    coalesce_back=coalesce_back&&texture_coalesce_enabled;
#endif
    if(coalesce_back){
        const auto owner=target_back->second.owner;
        for(auto probe=it;probe!=frames.end();++probe){
            const auto found=probe->images.find(back);
            if(found==probe->images.end()||!found->second.gpu_image||found->second.owner!=owner){coalesce_back=false;break;}
        }
    }
    while(!frames.empty()&&frames.back().number>=number){
        auto& f=frames.back();
        for(std::size_t slot=f.slots;slot<store.records.size();++slot)graphics_device().invalidate_texture(u32(slot+1));
        store.records.resize(f.slots);
        for(auto& [handle,image]:f.images){
            store.records[handle-1]=image.owner;auto& restored=*image.owner;
            restored.references=image.references;restored.revision=image.revision;
            restored.priority=image.priority;restored.render_target=image.target;
            if(image.gpu_image){
                const bool skip=coalesce_back&&handle==back&&f.number!=number;
#ifdef TH_MULTIPLAYER_FIXTURES
                if(handle==back){if(skip)++diagnostic_back_skipped;else ++diagnostic_back_restores;}
#endif
                if(!skip&&!touhou::sdl::current()->restore_color(handle,image.gpu_image)){restoring=false;return Fail();}
            }else{
                if(image.has_pixels)restored.image=image.pixels;
                // A metadata-only backbuffer keeps its current GPU image. Its
                // CPU copy is not an image of this historical frame; forcing
                // an upload here would replace the visible target with it.
                if(handle==back&&back_metadata_only)touhou::sdl::current()->adopt_color_revision(handle,restored.revision);
                else graphics_device().invalidate_texture(handle);
            }
        }
        store.live=f.live;runtime->surfaces=f.surfaces;
        runtime->pending_capture={f.capture_target,f.source,f.destination,f.triangle};
        runtime->captured=f.captured;runtime->capture_failed=f.capture_failed;
        touhou::sdl::current()->state=f.graphics;ReleaseImages(f);frames.pop_back();
    }
    restoring=false;return true;
}
void TextureJournal::ReleaseImages(Frame& frame){if(auto* renderer=touhou::sdl::current())for(auto& [handle,image]:frame.images)renderer->discard_color(image.gpu_image);}
void TextureJournal::DiscardBefore(u32 frame){if(open==Invalid)while(!frames.empty()&&frames.front().end<frame){ReleaseImages(frames.front());frames.pop_front();}}
void TextureJournal::Clear(){
    if(runtime&&runtime->app.textures.journal==this)runtime->app.textures.journal=nullptr;
    for(auto& frame:frames)ReleaseImages(frame);
    frames.clear();runtime=nullptr;open=Invalid;failed=restoring=false;
#ifdef TH_MULTIPLAYER_FIXTURES
    diagnostic_back_restores=diagnostic_back_skipped=0;
#endif
}
std::size_t TextureJournal::BytesForFrame(u32 number)const{for(const auto& f:frames)if(f.number<=number&&number<=f.end)return f.bytes;return 0;}
}
