#include "../game/CallbackNames.hpp"
#include "World.hpp"
#include "HintData.hpp"
namespace th10::browser {
namespace {
struct Hints final:StageHintsEnvironment,HintRecordingEnvironment {
    World& w;u32 output=0xffffffff;
    explicit Hints(World& world):w(world){current=&w.actors.hints;StageHintsEnvironment::game=HintRecordingEnvironment::game=&w.state.game;player=&w.actors.player;registry=&w.engine.manager.registry;text_animations=&w.common.value->text_animations;chain=&w.chain;callbacks=&w.engine.callback_environment;update_callback=callback_id::HintsUpdate;draw_callback=callback_id::HintsDraw;enabled=&w.state.configuration.music_mode;rate=&w.engine.speed;const auto& data=hint_data(w.state.chinese);section_names=data.sections;alignment_names=data.alignments;default_file=data.default_file;extra_file=data.extra_file;std::memcpy(save_comments,data.comments,sizeof(save_comments));save_separator=data.separator;}
    ~Hints(){end_file();}
    void* allocate(u32 bytes) override{return std::malloc(bytes);}
    void release_object(void* p) override{std::free(p);}
    void* allocate_bytes(u32 bytes) override{return std::malloc(bytes);}
    void free_bytes(void* p) override{std::free(p);}
    StageHint* allocate_hint() override{return static_cast<StageHint*>(std::malloc(sizeof(StageHint)));}
    u8* read_file(const char* name,u32& bytes) override{return ResourceFiles{w.scores.files}.load(name,&bytes,true);}
    u32 create_animation(AnmFile& file,i32 script,const Vec3& p) override{return w.engine.manager.create_at(file,script,p,true,AnimationPlacement::WorldBack,w.engine,w.engine);}
    void draw_text(AnmVm& vm,i32 align,u32 color,const char* text) override{const TextAlignment alignments[]={TextAlignment::Center,TextAlignment::Left,TextAlignment::Right};w.text(vm,color,text,alignments[align]);}
    void make_directory(const char*) override{} // The player's file store uses relative path keys.
    bool begin_file(const char* name) override{end_file();output=w.scores.files.host.open(name,true);return output!=0xffffffff;}
    void write_text(const char* text,bool) override{if(output==0xffffffff)return;const auto bytes=static_cast<u32>(std::strlen(text));if(w.scores.files.host.write(output,reinterpret_cast<const u8*>(text),bytes)!=bytes)end_file();}
    void end_file() override{if(output!=0xffffffff){w.scores.files.host.close(output);output=0xffffffff;}}
    HintDate local_time() override{const auto date=w.calendar.local_date(w.calendar.timestamp());return {date.year+1900,date.month+1,date.day,date.hour,date.minute};}
};
}
bool World::create_hints(){Hints env(*this);return StageHints::create(env)!=nullptr;}
void World::destroy_hints(StageHints* hints){Hints env(*this);hints->shutdown(env);std::free(hints);}
i32 World::update_hints(){Hints env(*this);return actors.hints->update(env);}
void World::record_hint(const char* text,const Vec3& position,bool caution){Hints env(*this);if(caution)actors.hints->record_caution(text,position,env);else actors.hints->record(text,position,env);}
}
