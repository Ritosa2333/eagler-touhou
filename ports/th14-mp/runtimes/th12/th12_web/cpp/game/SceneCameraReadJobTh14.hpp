#pragma once
#include "SceneCamera.hpp"
#include "StagedLoaderPrototype.hpp"
#include <cstring>
#include <type_traits>
namespace th11 {
// A resumable retail-order75-word read. The destination remains private until
// Ready; subsequent reads observe the live source, not a cached whole camera.
class SceneCameraReadJobTh14 {
    SceneCamera snapshot_{};
    u32 copied_=0;
    bool failed_=false,running_=false;
    static u32 offset(u32 index)noexcept{
        return index<18?index*4:index<21?offsetof(SceneCamera,target_offset)+(index-18)*4:
            index==21?offsetof(SceneCamera,fov):index==22?offsetof(SceneCamera,render_width):
            index==23?offsetof(SceneCamera,render_height):offsetof(SceneCamera,view)+(index-24)*4;
    }
public:
    static_assert(sizeof(SceneCamera)==300&&std::is_trivially_copyable<SceneCamera>::value);
    bool reset()noexcept{if(running_)return false;snapshot_=SceneCamera{};copied_=0;failed_=false;return true;}
    u32 copied_words()const noexcept{return copied_;}
    bool failed()const noexcept{return failed_;}
    const SceneCamera& snapshot()const noexcept{return snapshot_;}
    static u32 source_offset(u32 index)noexcept{return index<75?offset(index):~u32(0);}
    template<class BeforeRead> prototype::Work advance(const SceneCamera& source,BeforeRead&& before_read){
        return advance(source,before_read,[&source](u32,u32 at,u32& word){
            std::memcpy(&word,reinterpret_cast<const u8*>(&source)+at,4);
            return prototype::Work::Ready;
        });
    }
    // A host may physically capture the live source on an independent worker.
    // The result stays private until all75 reads are available. The existing
    // synchronous overload retains its original source and observer semantics.
    template<class BeforeRead,class ReadWord> prototype::Work advance(const SceneCamera& source,BeforeRead&& before_read,ReadWord&& read_word){
        using Work=prototype::Work;
        if(failed_)return Work::Failed;
        if(copied_==75)return Work::Ready;
        if(running_)return Work::Waiting;
        struct Guard{bool& value;explicit Guard(bool& v):value(v){value=true;}~Guard(){value=false;}}guard(running_);
        while(copied_<75){
            const auto permission=before_read(copied_);
            if(permission!=Work::Ready){if(permission==Work::Failed)failed_=true;return permission;}
            const auto at=offset(copied_);u32 word;
            const auto result=read_word(copied_,at,word);
            if(result!=Work::Ready){if(result==Work::Failed)failed_=true;return result;}
            std::memcpy(reinterpret_cast<u8*>(&snapshot_)+at,&word,4);++copied_;
        }
        return Work::Ready;
    }
};
}
