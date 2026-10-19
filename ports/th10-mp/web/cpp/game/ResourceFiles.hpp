#pragma once
#include "ResourceArchive.hpp"
namespace th10 {
struct ResourceFileEnvironment : CodecMemory {
    ResourceArchive* archive;u8* lock_depth;u32* current_handle;
    virtual void enter()=0;
    virtual void leave()=0;
    virtual u32 open(const char* name,bool write)=0;
    virtual u32 length(u32 handle)=0;
    virtual void read(u32 handle,u8* output,u32 count,u32* actual)=0;
    virtual void write(u32 handle,const u8* input,u32 count,u32* actual)=0;
    virtual void close(u32 handle)=0;
    virtual void discard_error(const char* filename)=0;
    virtual void read_archive(const char* name,u8* output)=0;
};
struct ResourceFiles {
    ResourceFileEnvironment& environment;
    u8* load(const char* name,u32* size,bool external);
    bool exists(const char* name);
    i32 save(const char* name,const u8* bytes,u32 size,u32 initial_actual=0);
    i32 open(const char* name,bool write);
    i32 write(const u8* bytes,u32 length,u32 initial_actual);
    u8* read(u32 length);
    void close();
private:
    void lock();void unlock();
};
}
