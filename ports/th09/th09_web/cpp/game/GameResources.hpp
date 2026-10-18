#pragma once
#include "AnmResource.hpp"
#include <array>
#include <memory>
namespace th09 {
enum class AnimationFile:u32 {text,ascii,menu,capture,background,left_character,right_character,extra_portraits,bullets,enemies,front,staff,count};
constexpr AnimationFile character_animation(i32 side){return side?AnimationFile::right_character:AnimationFile::left_character;}
struct CharacterResources {
    const char *shots,*animation,*enemies,*story_messages,*versus_messages,*alternate_animation;
    i32 attack_portrait=0,boss_background=0,dialogue_portrait=0,dialogue_sprite=0,focus_effect=0,attack_limit=0,attack_limit_per_difficulty=0;
};
struct BackgroundResources {const char* animation;const char* model;};
const CharacterResources* character_resources(u32 character)noexcept;
const BackgroundResources* background_resources(u32 background)noexcept;
struct ResourceReader {virtual ~ResourceReader()=default;virtual bool read(const char* name,std::vector<u8>&)=0;
    // Returns pending, ready, or failure without exposing platform jobs to game logic.
    virtual i32 prepare(const char*){return 1;}virtual void cancel_prepare(){}};
struct TextureAllocation {u32 handle=0,width=0,height=0;};
struct ResourceTextures {
    virtual ~ResourceTextures()=default;
    // Embedded THTX bytes, external image bytes or an empty source, according
    // to the descriptor. The platform owns decoding and graphics allocation.
    virtual TextureAllocation create(const AnmTextureSource&,const u8*,u32 size)=0;
    virtual void destroy(u32 handle)=0;
};
class GameResources {
    struct Entry {std::string name;std::unique_ptr<AnmResource> animation;std::vector<u32> textures;u32 uploaded=0;};
    ResourceReader& reader;ResourceTextures& textures;AnmExecutor& executor;
    std::array<Entry,u32(AnimationFile::count)> entries;
    struct Pending {AnimationFile slot;Entry entry;};std::vector<Pending> pending;
    bool parse(Entry&,AnimationFile);bool upload_one(Entry&);
    void release(Entry&);
public:
    std::string error;
    GameResources(ResourceReader& r,ResourceTextures& t,AnmExecutor& e):reader(r),textures(t),executor(e){}
    ~GameResources(){cancel_preload();for(auto& entry:entries)release(entry);}
    GameResources(const GameResources&)=delete;
    GameResources& operator=(const GameResources&)=delete;
    bool load(AnimationFile,const char* name);
    bool load_common();
    void preload(AnimationFile,const char*);bool warm_one();void cancel_preload();
    u32 pending_resources()const{return u32(pending.size());}
    bool load_match(i32 background,const i32 characters[2],const bool alternate[2],bool extra_portraits);
    AnmLoaded* animation(AnimationFile);
    bool read(const char* name,std::vector<u8>& bytes){return reader.read(name,bytes);}
    bool start(AnimationFile,AnmVm&,i32 script,bool reset_position=true);
    bool sprite(AnimationFile,AnmVm&,i32 index);
    bool advance(AnmVm& vm){return executor.execute(vm);}
};
}
