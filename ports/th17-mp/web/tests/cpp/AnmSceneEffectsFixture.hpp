#pragma once
#include "../../cpp/game/AnmSceneEffects.hpp"
#include "../../cpp/game/AnmOverlay.hpp"
struct AnmSceneEffectsFixture {AnmSceneEffects effects;explicit AnmSceneEffectsFixture(AnmManagerFixture& a,i32 bank):effects(a.manager,a.environment.game_rng,a.random,bank){}};
API(anm_scene_effects_create) AnmSceneEffectsFixture* anm_scene_effects_create(AnmManagerFixture* a,i32 bank){return new AnmSceneEffectsFixture(*a,bank);}
API(anm_scene_effects_delete) void anm_scene_effects_delete(AnmSceneEffectsFixture* p){delete p;}
API(anm_scene_effects_configure) u32 anm_scene_effects_configure(AnmSceneEffectsFixture* p,AnmVm* vm,i32 kind){return p->effects.configure(*vm,kind);}
API(anm_scene_effects_gather) AnmGatherEffect* anm_scene_effects_gather(AnmVm* vm){return vm->geometry.gather.get();}
API(anm_scene_effects_panel) AnmVm* anm_scene_effects_panel(AnmVm* vm,u32 index){return vm&&vm->geometry.overlay&&index<5?vm->geometry.overlay->panels[index].get():nullptr;}
