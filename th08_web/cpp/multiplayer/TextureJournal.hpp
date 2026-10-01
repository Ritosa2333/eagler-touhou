#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Texture rollback is multiplayer-only
#endif
#include "../game/TextureStore.hpp"
#include "../game/TextureResample.hpp"
#include "Renderer.hpp"
#include <deque>
#include <map>

namespace th08 {class BrowserRuntime;namespace multiplayer {
#ifdef TH_MULTIPLAYER_FIXTURES
bool fixture_texture_coalesce(bool enabled);
bool fixture_back_metadata_only(bool enabled);
#endif
// Lazy before-write images plus pinned native texture lifetimes. Device caches
// are invalidated on restore, never serialized as GL handles.
class TextureJournal {
public:
    static constexpr u32 History=8,Invalid=~u32(0);
    ~TextureJournal(){Clear();}
    bool Bind(BrowserRuntime&);
    bool BeginFrame(u32,bool extend=false);
    bool EndFrame();
    bool UndoTo(u32);
    void DiscardBefore(u32);
    void Clear();
    bool Touch(u32 handle,bool pixels);
    bool SetBackMetadataOnly(bool enabled){if(!frames.empty()||open!=Invalid)return false;back_metadata_only=enabled;return true;}
    bool BackMetadataOnly()const{return back_metadata_only;}
    bool MayMutate();
    bool Failed()const{return failed;}
    std::size_t BytesForFrame(u32)const;
#ifdef TH_MULTIPLAYER_FIXTURES
    u32 DiagnosticBackRestores()const{return diagnostic_back_restores;}
    u32 DiagnosticBackSkipped()const{return diagnostic_back_skipped;}
#endif
private:
    struct Image {
        std::shared_ptr<TextureRecord> owner;
        u32 references=0,revision=0,priority=0;
        bool target=false,has_pixels=false;
        u32 gpu_image=0;
        TexturePixels pixels;
    };
    struct Frame {
        u32 number=0,end=0,live=0;std::size_t slots=0,bytes=0;
        std::map<u32,Image> images;
        std::map<i32,u32> surfaces;
        u32 capture_target=0;TextureRect source{},destination{};
        bool triangle=false,captured=false,capture_failed=false;
        touhou::sdl::State graphics;
    };
    BrowserRuntime* runtime=nullptr;
    std::deque<Frame> frames;
    u32 open=Invalid;bool failed=false,restoring=false;
    // Only rollback metadata is authoritative for the onscreen backbuffer.
    bool back_metadata_only=true;
#ifdef TH_MULTIPLAYER_FIXTURES
    u32 diagnostic_back_restores=0,diagnostic_back_skipped=0;
#endif
    bool Fail(){failed=true;return false;}
    void ReleaseImages(Frame&);
};
}}
