#include "SpellCard.hpp"
namespace th10 {
void SpellCard::initialize(SpellCard** current) noexcept {std::memset(this,0,sizeof(*this));flags=2;*current=this;}
i32 SpellCard::attach(SpellEnvironment& env){
    update_entry=(*env.chain)->add(env.update_callback,this,22,false,false,*env.callbacks);
    background_entry=(*env.chain)->add(env.background_callback,this,14,true,false,*env.callbacks);
    foreground_entry=(*env.chain)->add(env.foreground_callback,this,37,true,false,*env.callbacks);
    if(!(elapsed_flags&1)){elapsed.rate=env.rate;elapsed_flags|=1;}elapsed.initialize(-1);return 0;
}
void SpellCard::activate() noexcept {if(update_entry)update_entry->flags|=2;if(background_entry)background_entry->flags|=2;}
void SpellCard::release(SpellEnvironment& env){
    for(auto& id:title_animations)env.registry->delete_and_clear(id);
    (*env.chain)->remove_locked(update_entry,*env.callbacks);(*env.chain)->remove_locked(background_entry,*env.callbacks);(*env.chain)->remove_locked(foreground_entry,*env.callbacks);*env.current=nullptr;
    for(u32 i=5;i>0;--i)if(auto*& geometry=record_digits[i-1].geometry){env.release_memory(geometry);geometry=nullptr;}
    for(u32 i=8;i>0;--i)if(auto*& geometry=bonus_digits[i-1].geometry){env.release_memory(geometry);geometry=nullptr;}
    for(u32 i=2;i>0;--i)if(auto*& geometry=backgrounds[i-1].geometry){env.release_memory(geometry);geometry=nullptr;}
}
SpellCard* SpellCard::create(SpellEnvironment& env){auto* spell=env.allocate();if(!spell)return nullptr;spell->initialize(env.current);if(spell->attach(env)){spell->release(env);env.release_memory(spell);return nullptr;}return spell;}
// 0x409c00. The record counter is excluded while replaying; score and the
// captured/failed notification follow their normal gameplay paths.
void SpellCard::finish(SpellEnvironment& env){
    if(!(spell_flags&1))return;(*env.stage)->draw_flags|=1;for(u32 id:title_animations)env.registry->interrupt(id,1);spell_flags&=~1u;env.registry->delete_and_clear(circle_animation);
    if(spell_flags&2){env.game->add_score(bonus);env.show_bonus(bonus);if(*env.replay_mode!=1){auto& selected=env.record(number,false);if(selected.captures<99999)++selected.captures;auto& combined=env.record(number,true);if(combined.captures<99999)++combined.captures;}env.play_sound(45);}
    else{env.registry->delete_and_clear(*env.notification_animation);*env.notification_animation=env.create_animation(SpellAnimationFile::Interface,72);}
}
}
