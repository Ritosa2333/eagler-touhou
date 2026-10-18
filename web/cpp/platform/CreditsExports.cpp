#include "Credits.hpp"
#include <new>
using namespace th10;
#define CREDITS_EXPORT(name) extern "C" __attribute__((export_name(name)))
CREDITS_EXPORT("credits_create") browser::Credits* credits_create(browser::World* world,browser::Captures* captures){auto* bytes=std::malloc(sizeof(browser::Credits));if(!bytes)return nullptr;auto* credits=new(bytes)browser::Credits(*world,*captures);if(!credits->initialize()){credits->~Credits();std::free(credits);return nullptr;}return credits;}
CREDITS_EXPORT("credits_destroy") void credits_destroy(browser::Credits* credits){if(credits){credits->~Credits();std::free(credits);}}
CREDITS_EXPORT("credits_advance_loading") void credits_advance_loading(browser::Credits* credits){credits->advance_loading();}
CREDITS_EXPORT("credits_value") Ending* credits_value(browser::Credits* credits){return credits->value;}
CREDITS_EXPORT("credits_error") i32 credits_error(browser::Credits* credits){return credits->error;}
