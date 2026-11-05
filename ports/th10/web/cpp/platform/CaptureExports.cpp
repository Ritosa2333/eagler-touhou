#include "Captures.hpp"
#include <new>
#include <cstdlib>
using namespace th10;
#define CAPTURE_EXPORT(name) extern "C" __attribute__((export_name(name)))
CAPTURE_EXPORT("captures_create") browser::Captures* captures_create(browser::AnimationEngine* engine){auto* bytes=std::malloc(sizeof(browser::Captures));return bytes?new(bytes)browser::Captures(*engine):nullptr;}
CAPTURE_EXPORT("captures_destroy") void captures_destroy(browser::Captures* captures){if(captures){captures->~Captures();std::free(captures);}}
CAPTURE_EXPORT("captures_process") void captures_process(browser::Captures* captures){captures->process();}
CAPTURE_EXPORT("captures_copy_surface") i32 captures_copy_surface(browser::Captures* captures,void* destination,const TextureRect* target,void* source,const TextureRect* region,u32 filter){return captures->copy_surface(destination,target,source,region,filter);}
