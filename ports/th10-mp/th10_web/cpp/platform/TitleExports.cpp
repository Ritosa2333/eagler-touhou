#include "Title.hpp"
#include "Startup.hpp"
#include <cstdlib>
#include <new>
using namespace th10;
#define TITLE_EXPORT(name) extern "C" __attribute__((export_name(name)))
TITLE_EXPORT("title_create") browser::Title* title_create(browser::GameState* state,browser::AnimationEngine* engine,browser::Common* common,browser::Fonts* fonts,browser::Input* input,browser::Audio* audio,browser::Scores* scores,browser::ScreenEffects* effects){auto* bytes=std::malloc(sizeof(browser::Title));if(!bytes)return nullptr;auto* title=new(bytes)browser::Title(*state,*engine,*common,*fonts,*input,*audio,*scores,*effects);if(!title->initialize()){title->~Title();std::free(title);return nullptr;}return title;}
TITLE_EXPORT("title_destroy") void title_destroy(browser::Title* title){if(title){title->~Title();std::free(title);}}
TITLE_EXPORT("title_value") TitleMenu* title_value(browser::Title* title){return title->value;}
TITLE_EXPORT("title_advance_loading") void title_advance_loading(browser::Title* title){title->advance_loading();}
TITLE_EXPORT("title_input") void title_input(browser::Title* title,u32 buttons){title->input.player_profiles[0].input.update_raw(buttons);}
TITLE_EXPORT("title_create_from_startup") browser::Title* title_create_from_startup(browser::GameState* state,browser::AnimationEngine* engine,browser::Startup* startup,browser::Fonts* fonts,browser::Input* input,browser::Audio* audio,browser::ScreenEffects* effects){
    if(!startup->shared||!startup->scores)return nullptr;auto* bytes=std::malloc(sizeof(browser::Title));if(!bytes)return nullptr;
    auto* title=new(bytes)browser::Title(*state,*engine,*startup->shared,*fonts,*input,*audio,*startup->scores,*effects);title->startup=startup->value;
    if(!title->initialize()){title->~Title();std::free(title);return nullptr;}return title;
}
