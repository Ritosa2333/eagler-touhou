#pragma once
#include "ApplicationState.hpp"
#include "TexturePlatform.hpp"
namespace th10 {
#pragma pack(push,1)
struct BitmapFileHeader {u16 type;u32 size;u16 reserved1,reserved2;u32 data_offset;};
#pragma pack(pop)
struct ScreenshotState {
    u32 worker;BitmapFileHeader header;u16 reserved_012;
    u32* bitmap_info;u8* pixels;char filename[260];
};
static_assert(sizeof(BitmapFileHeader)==14&&sizeof(ScreenshotState)==0x120&&offsetof(ScreenshotState,bitmap_info)==0x14&&offsetof(ScreenshotState,filename)==0x1c);
enum class ScreenshotError {UnsupportedFormat,Format16,Allocation};
struct ScreenshotEnvironment {
    ScreenshotState* global;u32* output_handle;
    virtual void sleep(u32 milliseconds)=0;
    virtual void* back_buffer(void* device)=0;
    virtual void* allocate(u32 bytes)=0;
    virtual void free_bytes(void* bytes)=0;
    virtual TextureLock lock_surface(void* surface)=0;
    virtual void unlock_surface(void* surface)=0;
    virtual void release_surface(void* surface)=0;
    virtual u32 begin_writer()=0;
    virtual void report(ScreenshotError error)=0;
    virtual void open_output(const char* filename)=0;
    virtual void write_output(const u8* bytes,u32 length,u32& actual)=0;
    virtual void close_output()=0;
};
struct Screenshot {
    ScreenshotEnvironment& environment;
    i32 capture(ApplicationState& application,ScreenshotState& state,const u32& back_buffer_format,const char* filename);
    void write(u32 initial_actual=0);
};
}
