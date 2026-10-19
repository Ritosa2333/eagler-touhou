#pragma once
#include "Gui.hpp"
namespace th10 {
struct DialogueEnvironment;
struct MessageInstruction {
    u16 time;u8 opcode,length;
    template<class T> T argument(u32 index=0) const noexcept {T value;std::memcpy(&value,reinterpret_cast<const u8*>(this)+4+index*sizeof(T),sizeof(T));return value;}
    const u8* payload() const noexcept {return reinterpret_cast<const u8*>(this)+4;}
    MessageInstruction* next() noexcept {return reinterpret_cast<MessageInstruction*>(reinterpret_cast<u8*>(this)+4+length);}
};
enum class DialogueAnimationFile {Player,Boss,Interface,Text,MusicCaption};
struct Dialogue {
    i32 id;
    Timer elapsed;u32 elapsed_flags;
    Timer script_time;u32 script_time_flags;
    Timer wait;u32 wait_flags;
    u32 portraits[2],text_box,lines[2],name_tag;
    u32 reserved_058;
    MessageInstruction* instruction;
    Vec3 line_positions[2];
    i32 blocking_frames;
    u32 flags,next_line,speaker,colors[2];
    void initialize(MessageInstruction* script,DialogueEnvironment& environment);
    void release(const AnmRegistry& registry) noexcept;
    static void start(i32 id,DialogueEnvironment& environment);
    i32 update(DialogueEnvironment& environment);
    i32 tick(DialogueEnvironment& environment);
    static char* decode(const u8* source,char* destination) noexcept;
};
static_assert(sizeof(Dialogue)==0x90&&offsetof(Dialogue,instruction)==0x5c);
struct DialogueEnvironment {
    Gui* gui;
    GameEconomy* game;
    AnmRegistry* registry;
    const u32* keys;
    const u16* pressed;
    float* rate;
    char* decoded_text;
    virtual Dialogue* allocate()=0;
    virtual u32 create_animation(DialogueAnimationFile file,i32 script)=0;
    virtual void bind_sprite(AnmVm& vm,DialogueAnimationFile file,i32 sprite,bool explicit_file)=0;
    virtual void draw_text(AnmVm* vm,u32 color,const char* text)=0;
    virtual void clear_projectiles_and_enemies()=0;
    virtual void play_sound(i32 id)=0;
    virtual void start_music()=0;
    virtual void fade_music(float seconds)=0;
    virtual void complete_stage()=0;
};
}
