#include "GameObjectResources.hpp"
namespace th10 {
// 0x41acb0 / 0x4055a0 / 0x40ae50. Embedded constructors do not leave any
// state behind: the original then zeros the full allocation and sets flag 2.
void GameObjectResources::initialize(ItemManager& value) const noexcept {std::memset(&value,0,sizeof(value));value.flags=2;*environment.items=&value;}
void GameObjectResources::initialize(Bomb& value) const noexcept {std::memset(&value,0,sizeof(value));value.flags=2;*environment.bomb=&value;}
void GameObjectResources::initialize(GameEffects& value) const noexcept {std::memset(&value,0,sizeof(value));value.flags=2;*environment.effects=&value;}
// 0x41ad90 / 0x4055c0 / 0x40ae70. Bomb callbacks start enabled.
i32 GameObjectResources::start(ItemManager& items){auto& env=environment;items.update_entry=(*env.chain)->add(env.item_update,&items,21,false,false,*env.callbacks);items.draw_entry=(*env.chain)->add(env.item_draw,&items,25,true,false,*env.callbacks);return 0;}
i32 GameObjectResources::start(Bomb& bomb){auto& env=environment;bomb.update_entry=(*env.chain)->add(env.bomb_update,&bomb,17,false,true,*env.callbacks);bomb.draw_entry=(*env.chain)->add(env.bomb_draw,&bomb,34,true,true,*env.callbacks);return 0;}
i32 GameObjectResources::start(GameEffects& effects){auto& env=environment;effects.animations=env.load_effect_animations();if(!effects.animations){env.report_effect_error();return -1;}effects.update_entry=(*env.chain)->add(env.effects_update,&effects,23,false,false,*env.callbacks);effects.draw_entry=(*env.chain)->add(env.effects_draw,&effects,32,true,false,*env.callbacks);return 0;}
// 0x41adf0. Remove the callbacks and global owner before freeing embedded
// animation geometry in reverse order, matching the array destructor.
void GameObjectResources::shutdown(ItemManager& items){auto& env=environment;(*env.chain)->remove_locked(items.update_entry,*env.callbacks);(*env.chain)->remove_locked(items.draw_entry,*env.callbacks);*env.items=nullptr;const auto release=[&](Item& item){if(item.animation.geometry)env.release_geometry(item.animation.geometry);item.animation.geometry=nullptr;};for(i32 i=2047;i>=0;--i)release(items.faith[i]);for(i32 i=149;i>=0;--i)release(items.regular[i]);}
// 0x405620 / 0x40af00. Shared ANM files remain owned by the animation manager.
void GameObjectResources::shutdown(Bomb& bomb){auto& env=environment;(*env.chain)->remove_locked(bomb.update_entry,*env.callbacks);(*env.chain)->remove_locked(bomb.draw_entry,*env.callbacks);*env.bomb=nullptr;}
void GameObjectResources::shutdown(GameEffects& effects){auto& env=environment;(*env.chain)->remove_locked(effects.update_entry,*env.callbacks);(*env.chain)->remove_locked(effects.draw_entry,*env.callbacks);*env.effects=nullptr;}
// 0x41aed0 / 0x4056b0 / 0x40af90.
void* GameObjectResources::create(GameObjectKind kind){auto& env=environment;
    if(kind==GameObjectKind::Items){auto* value=static_cast<ItemManager*>(env.allocate(sizeof(ItemManager)));if(!value)return nullptr;initialize(*value);if(start(*value)){shutdown(*value);env.delete_object(value);return nullptr;}return value;}
    if(kind==GameObjectKind::Bomb){auto* value=static_cast<Bomb*>(env.allocate(sizeof(Bomb)));if(!value)return nullptr;initialize(*value);if(start(*value)){shutdown(*value);env.delete_object(value);return nullptr;}return value;}
    auto* value=static_cast<GameEffects*>(env.allocate(sizeof(GameEffects)));if(!value)return nullptr;initialize(*value);if(start(*value)){shutdown(*value);env.delete_object(value);return nullptr;}return value;
}
}
