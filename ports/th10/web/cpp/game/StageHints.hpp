#pragma once
#include "Player.hpp"
#include "UpdateChain.hpp"
#include "AnmRegistry.hpp"
namespace th10 {
struct StageHintsEnvironment;
struct HintRecordingEnvironment;
struct HintName {const char* name;i32 value;};
struct StageHint {
    Vec3 position;
    ListNode<StageHint> node;
    u32 reserved_018;
    char text[68];
    i32 delay,alignment,section,duration,remaining;
    u32 reserved_074[2];
    float scale,width;
    u32 color;
    void initialize() noexcept;
};
static_assert(sizeof(StageHint)==0x88&&offsetof(StageHint,delay)==0x60);
struct StageHints {
    u32 flags,manager_state;
    UpdateChainEntry *update_entry,*draw_entry;
    i32 frames,section_frames;
    ListNode<StageHint> pending[8],retained[8];
    i32 previous_section;
    u32 animations[10];
    Vec3 positions[10];
    float widths[10];
    i32 cursor;
    void initialize(StageHints** current) noexcept;
    i32 start(StageHintsEnvironment& environment);
    void shutdown(StageHintsEnvironment& environment);
    void clear(StageHintsEnvironment& environment);
    i32 display(ListNode<StageHint>* head,bool release,StageHintsEnvironment& environment);
    i32 update(StageHintsEnvironment& environment);
    void decrement_remaining(i32 stage) noexcept;
    StageHint* record(const char* text,const Vec3& position,HintRecordingEnvironment& environment);
    StageHint* record_caution(const char* text,const Vec3& position,HintRecordingEnvironment& environment);
    i32 load(const char* name,bool transient,StageHintsEnvironment& environment);
    i32 save(const char* name,StageHintsEnvironment& environment);
    static StageHints* create(StageHintsEnvironment& environment);
};
static_assert(sizeof(StageHints)==0x1a8&&offsetof(StageHints,animations)==0xdc&&offsetof(StageHints,cursor)==0x1a4);
struct HintText {
    static char* trim(char* text) noexcept;
    static const char* read_line(const char* input,char* output,u32& remaining,u32 capacity) noexcept;
    static void split(const char* text,char* key,char* value) noexcept;
    static i32 lookup(const char* name,const HintName* table,i32 count) noexcept;
    static const char* lookup(i32 value,const HintName* table,i32 count) noexcept;
    static void append(ListNode<StageHint>& node,ListNode<StageHint>& head) noexcept;
};
struct HintDate {i32 year,month,day,hour,minute;};
struct HintRecordingEnvironment {GameEconomy* game;virtual StageHint* allocate_hint()=0;};
struct StageHintsEnvironment {
    StageHints** current;
    GameEconomy* game;
    Player** player;
    AnmRegistry* registry;
    AnmFile** text_animations;
    UpdateChain** chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken update_callback,draw_callback;
    const u8* enabled;
    const float* rate;
    const HintName *section_names,*alignment_names;
    const char *default_file,*extra_file;
    const char* save_comments[3];
    const char* save_separator;
    virtual void* allocate(u32 bytes)=0;
    virtual void release_object(void* object)=0;
    virtual void* allocate_bytes(u32 bytes)=0;
    virtual void free_bytes(void* bytes)=0;
    virtual u8* read_file(const char* name,u32& bytes)=0;
    virtual u32 create_animation(AnmFile& file,i32 script,const Vec3& position)=0;
    virtual void draw_text(AnmVm& animation,i32 alignment,u32 color,const char* text)=0;
    virtual void make_directory(const char* name)=0;
    virtual bool begin_file(const char* name)=0;
    virtual void write_text(const char* text,bool header)=0;
    virtual void end_file()=0;
    virtual HintDate local_time()=0;
};
}
