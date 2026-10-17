#include "Common.hpp"
#include <cstdlib>
#include <new>
using namespace th10;
#define COMMON_EXPORT(name) extern "C" __attribute__((export_name(name)))
COMMON_EXPORT("common_create") browser::Common* common_create(browser::AnimationEngine* engine){
    auto* bytes=std::malloc(sizeof(browser::Common));if(!bytes)return nullptr;auto* common=new(bytes)browser::Common(*engine);
    if(!common->initialize()){common->~Common();std::free(common);return nullptr;}return common;
}
COMMON_EXPORT("common_destroy") void common_destroy(browser::Common* common){if(common){common->~Common();std::free(common);}}
COMMON_EXPORT("common_state") CommonResources* common_state(browser::Common* common){return common->value;}
COMMON_EXPORT("common_enable") void common_enable(browser::Common* common){common->value->enable();}
COMMON_EXPORT("common_queue") void common_queue(browser::Common* common,const char* text,const Vec3* position,u32 early){common->value->queue(text,*position,early!=0);}
COMMON_EXPORT("common_small") void common_small(browser::Common* common){common->value->mark_small();}
COMMON_EXPORT("common_draw") i32 common_draw(browser::Common* common,u32 early){return common->draw(early!=0);}
