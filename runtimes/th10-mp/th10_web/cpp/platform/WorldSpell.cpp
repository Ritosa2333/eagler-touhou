#include "../game/CallbackNames.hpp"
#include "World.hpp"
#ifdef TH_ENABLE_THPRAC
#include "../game/PracticeRuntime.hpp"
#endif
namespace th10::browser {
namespace {
struct Spell final:SpellEnvironment {
    World& w;explicit Spell(World& world):w(world){current=&w.actors.spell;stage=&w.backgrounds.current;game=&w.state.game;replay_mode=w.state.replay?&w.state.replay->mode:nullptr;player_position=w.actors.player?&w.actors.player->position:nullptr;registry=&w.engine.manager.registry;rate=&w.engine.speed;chain=&w.chain;callbacks=&w.engine.callback_environment;update_callback=callback_id::SpellUpdate;background_callback=callback_id::SpellBackground;foreground_callback=callback_id::SpellForeground;notification_animation=w.actors.gui?&w.actors.gui->notification:nullptr;}
#ifdef TH_ENABLE_THPRAC
    // F4 time lock (0x408d93): hold the spell bonus decay.
    bool time_locked() const override{return practice_time_lock(w.state.practice);}
#endif
    SpellCard* allocate() override{return static_cast<SpellCard*>(std::malloc(sizeof(SpellCard)));}
    void release_memory(void* bytes) override{std::free(bytes);}
    const Vec3& boss_position() override{return w.actors.enemies->bosses[0]->state.current.position;}
    SpellRecord& record(i32 id,bool combined) override{return w.scores.data->characters[combined?6:game->character*3+game->shot_type].spells[id];}
    AnmFile& file(SpellAnimationFile kind){switch(kind){case SpellAnimationFile::Interface:return *w.actors.gui->animations;case SpellAnimationFile::Effects:return *w.common.value->effects;case SpellAnimationFile::Text:return *w.common.value->text_animations;case SpellAnimationFile::Bullets:return *w.actors.bullets->animation_file;case SpellAnimationFile::Boss:return *w.actors.enemies->animation_files[2];}__builtin_trap();}
    i32 update_animation(AnmVm& vm) override{return vm.update(w.engine);}
    void draw_animation(AnmVm& vm) override{w.engine.draw(vm);}
    void initialize_animation(AnmVm& vm,SpellAnimationFile kind,i32 script,bool embedded) override{if(embedded)initialize_embedded_animation(file(kind),vm,script,w.engine,w.engine.manager.started_scripts);else file(kind).initialize_script(vm,script,w.engine,w.engine.manager.started_scripts);}
    void bind_digit(AnmVm& vm,i32 sprite) override{file(SpellAnimationFile::Interface).bind_sprite(vm,sprite);}
    u32 create_animation(SpellAnimationFile kind,i32 script) override{return w.animation(file(kind),script);}
    void draw_name(AnmVm* vm,const char* name) override{w.text(*vm,0xffffff,name,TextAlignment::Right);}
    void play_sound(i32 id) override{w.sound(id);}
    void show_bonus(i32 bonus) override{w.hud->notify(0,bonus);}
};
}
bool World::create_spell(){Spell env(*this);return SpellCard::create(env)!=nullptr;}
void World::destroy_spell(SpellCard* spell){Spell env(*this);spell->release(env);std::free(spell);}
i32 World::update_spell(){Spell env(*this);return actors.spell->update(env);}
i32 World::draw_spell(bool foreground){Spell env(*this);return foreground?actors.spell->draw_digits(env):actors.spell->draw_backgrounds(env);}
void World::start_spell(i32 id,i32 name_id,const char* name,i32 frames){Spell env(*this);actors.spell->start(id,name_id,name,frames,env);}
void World::finish_spell(){Spell env(*this);actors.spell->finish(env);}
}
