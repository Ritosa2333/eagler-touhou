// Development entry points for the independent browser file platform.
#include "FileSystem.hpp"
#include <new>
#include <cstdlib>
using namespace th10;
#ifdef TH_SDL3
#define FILE_IMPORT(name) extern "C"
#else
#define FILE_IMPORT(name) extern "C" __attribute__((import_module("th10_files"),import_name(name)))
#endif
FILE_IMPORT("open") u32 browser_open(const char*,u32);
FILE_IMPORT("close") void browser_close(u32);
FILE_IMPORT("size") u32 browser_size(u32);
FILE_IMPORT("seek") u32 browser_seek(u32,i32,u32);
FILE_IMPORT("read") u32 browser_read(u32,u8*,u32);
FILE_IMPORT("write") u32 browser_write(u32,const u8*,u32);
FILE_IMPORT("list") u32 browser_list(const char*,const char*,u32,char*,u32);
namespace {
struct Host final : browser::FileHost {
    u32 open(const char* name,bool write) override {return browser_open(name,write);}
    void close(u32 handle) override {browser_close(handle);}
    u32 size(u32 handle) override {return browser_size(handle);}
    u32 seek(u32 handle,i32 offset,u32 origin) override {return browser_seek(handle,offset,origin);}
    u32 read(u32 handle,u8* bytes,u32 length) override {return browser_read(handle,bytes,length);}
    u32 write(u32 handle,const u8* bytes,u32 length) override {return browser_write(handle,bytes,length);}
    u32 list(const char* directory,const char* pattern,u32 index,char* name,u32 capacity) override {return browser_list(directory,pattern,index,name,capacity);}
} host;
}
#define FILE_EXPORT(name) extern "C" __attribute__((export_name(name)))
FILE_EXPORT("files_allocate") void* files_allocate(u32 length){return std::malloc(length);}
FILE_EXPORT("files_free") void files_free(void* bytes){std::free(bytes);}
FILE_EXPORT("files_create") browser::FileSystem* files_create(){auto* bytes=std::malloc(sizeof(browser::FileSystem));return bytes?new(bytes)browser::FileSystem(host):nullptr;}
FILE_EXPORT("files_destroy") void files_destroy(browser::FileSystem* files){if(files){files->~FileSystem();std::free(files);}}
FILE_EXPORT("files_attach") u32 files_attach(browser::FileSystem* files,const char* name){return files->attach_archive(name);}
FILE_EXPORT("files_count") u32 files_count(browser::FileSystem* files){return files->resources.count;}
FILE_EXPORT("files_name") const char* files_name(browser::FileSystem* files,u32 index){return index<static_cast<u32>(files->resources.count)?files->resources.entries[index].name:nullptr;}
FILE_EXPORT("files_size") u32 files_size(browser::FileSystem* files,u32 index){return index<static_cast<u32>(files->resources.count)?files->resources.entries[index].size:0;}
FILE_EXPORT("files_load") u8* files_load(browser::FileSystem* files,const char* name,u32* size,u32 external){return ResourceFiles{*files}.load(name,size,external!=0);}
FILE_EXPORT("files_save") i32 files_save(browser::FileSystem* files,const char* name,const u8* bytes,u32 size){return ResourceFiles{*files}.save(name,bytes,size);}
FILE_EXPORT("files_lock_depth") u32 files_lock_depth(browser::FileSystem* files){return files->depth;}
