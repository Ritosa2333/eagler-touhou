#include "World.hpp"
#include "Title.hpp"
#include <new>
using namespace th10;
#define WORLD_EXPORT(name) extern "C" __attribute__((export_name(name)))
WORLD_EXPORT("world_create") browser::World* world_create(browser::GameState* state,browser::AnimationEngine* engine,browser::Common* common,browser::Fonts* fonts,browser::Input* input,browser::Audio* audio,browser::Scores* scores,browser::ScreenEffects* effects){auto* bytes=std::malloc(sizeof(browser::World));return bytes?new(bytes)browser::World(*state,*engine,*common,*fonts,*input,*audio,*scores,*effects):nullptr;}
WORLD_EXPORT("world_destroy") void world_destroy(browser::World* world){if(world){world->~World();std::free(world);}}
WORLD_EXPORT("world_start") i32 world_start(browser::World* world,i32 mode,i32 new_game){world->new_game=new_game;return world->start(mode);}
WORLD_EXPORT("world_select_stage") i32 world_select_stage(browser::World* world,i32 stage,i32 character,i32 shot,i32 difficulty){if(stage<1||stage>7||character<0||character>1||shot<0||shot>2||difficulty<0||difficulty>4)return 0;auto& state=world->state;state.game.stage=stage;state.game.character=character;state.game.shot_type=shot;state.game.difficulty=difficulty;state.current_stage=&browser::menu_data(state.chinese).stages[stage];return 1;}
WORLD_EXPORT("world_input") void world_input(browser::World* world,u32 buttons){world->input.player_profiles[0].input.update_raw(buttons);}
WORLD_EXPORT("world_advance_loading") void world_advance_loading(browser::World* world){world->advance_loading();}
WORLD_EXPORT("world_stop") void world_stop(browser::World* world){world->stop_session();}
WORLD_EXPORT("world_error") i32 world_error(browser::World* world){return world->error;}
WORLD_EXPORT("world_actor") void* world_actor(browser::World* world,i32 kind){auto& a=world->actors;switch(kind){case 0:return a.session;case 1:return a.player;case 2:return a.enemies;case 3:return a.bullets;case 4:return a.items;case 5:return a.lasers;case 6:return a.gui;case 7:return a.results;case 8:return a.popups;case 9:return a.hints;case 10:return a.effects;case 11:return a.bomb;case 12:return a.spell;case 13:return world->state.replay;case 14:return world->backgrounds.current;case 15:return world->backgrounds.previous;default:return nullptr;}}
WORLD_EXPORT("world_frame_timing") void world_frame_timing(browser::World* world,float fps,double active,double total){world->measured_fps=fps;world->state.active_time=active;world->state.total_time=total;}
WORLD_EXPORT("world_bind_title") void world_bind_title(browser::World* world,browser::Title* title){title->results=&world->results_services();}
WORLD_EXPORT("world_save_replay") bool world_save_replay(browser::World* world,const char* file,const char* name){if(!world||!file||!name||!world->state.replay||world->state.replay->mode!=0)return false;world->finish_replay(0);world->save_replay(file,name);return world->error==0;}
