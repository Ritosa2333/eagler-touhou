#include "ReplayFiles.hpp"
#include <cstdlib>
#include <new>
using namespace th10;
#ifdef TH_SDL3
extern "C" void local_date(i32,ReplayDate*);
extern "C" i32 current_timestamp();
#else
extern "C" __attribute__((import_module("th10_time"),import_name("local_date"))) void local_date(i32,ReplayDate*);
extern "C" __attribute__((import_module("th10_time"),import_name("timestamp"))) i32 current_timestamp();
#endif
namespace {struct Calendar final : browser::ReplayCalendar {ReplayDate local_date(i32 timestamp) override {ReplayDate result{};::local_date(timestamp,&result);return result;}i32 timestamp() override{return current_timestamp();}} calendar;}
namespace th10::browser {ReplayCalendar& default_calendar(){return calendar;}}
#define REPLAY_EXPORT(name) extern "C" __attribute__((export_name(name)))
REPLAY_EXPORT("replay_document_create") browser::ReplayDocument* replay_document_create(browser::FileSystem* files,u32 flags){auto* bytes=std::malloc(sizeof(browser::ReplayDocument));return bytes?new(bytes)browser::ReplayDocument(*files,flags):nullptr;}
REPLAY_EXPORT("replay_document_destroy") void replay_document_destroy(browser::ReplayDocument* document){if(document){document->~ReplayDocument();std::free(document);}}
REPLAY_EXPORT("replay_document_load") i32 replay_document_load(browser::ReplayDocument* document,const char* name){return document->load(name);}
REPLAY_EXPORT("replay_document_value") Replay* replay_document_value(browser::ReplayDocument* document){return &document->value;}
REPLAY_EXPORT("replay_document_allocate") u8* replay_document_allocate(browser::ReplayDocument* document,u32 bytes){return document->allocate_bytes(bytes);}
REPLAY_EXPORT("replay_document_allocations") u32 replay_document_allocations(browser::ReplayDocument* document){return document->memory.count;}
REPLAY_EXPORT("replay_writer_create") browser::ReplayWriter* replay_writer_create(browser::FileSystem* files,GameEconomy* game,const double* active,const double* total,u32 chinese){auto* bytes=std::malloc(sizeof(browser::ReplayWriter));return bytes?new(bytes)browser::ReplayWriter(*files,calendar,*game,*active,*total,chinese!=0):nullptr;}
REPLAY_EXPORT("replay_writer_destroy") void replay_writer_destroy(browser::ReplayWriter* writer){if(writer){writer->~ReplayWriter();std::free(writer);}}
REPLAY_EXPORT("replay_writer_save") i32 replay_writer_save(browser::ReplayWriter* writer,Replay* replay,const char* file,const char* name){return writer->save(*replay,file,name);}
REPLAY_EXPORT("replay_writer_allocations") u32 replay_writer_allocations(browser::ReplayWriter* writer){return writer->memory.count;}
