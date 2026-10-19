#include "Startup.hpp"
#include <cstdlib>
#include <new>
using namespace th10;
#define STARTUP_EXPORT(name) extern "C" __attribute__((export_name(name)))
STARTUP_EXPORT("startup_create") browser::Startup* startup_create(browser::GameState* state,browser::AnimationEngine* engine,browser::FileSystem* files,browser::Audio* audio){auto* bytes=std::malloc(sizeof(browser::Startup));if(!bytes)return nullptr;auto* startup=new(bytes)browser::Startup(*state,*engine,*files,*audio);if(!startup->initialize()){startup->~Startup();std::free(startup);return nullptr;}return startup;}
STARTUP_EXPORT("startup_destroy") void startup_destroy(browser::Startup* startup){if(startup){startup->~Startup();std::free(startup);}}
STARTUP_EXPORT("startup_advance_loading") void startup_advance_loading(browser::Startup* startup){startup->advance_loading();}
STARTUP_EXPORT("startup_value") StartupScreen* startup_value(browser::Startup* startup){return startup->value;}
STARTUP_EXPORT("startup_common") browser::Common* startup_common(browser::Startup* startup){return startup->shared;}
STARTUP_EXPORT("startup_scores") browser::Scores* startup_scores(browser::Startup* startup){return startup->scores;}
STARTUP_EXPORT("startup_error") i32 startup_error(browser::Startup* startup){return startup->error;}
