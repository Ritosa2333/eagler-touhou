#pragma once
#include <emscripten.h>
#include <cstdint>
#ifdef TH14_WEBGPU_BACKEND
extern "C" {
void th14_webgpu_schedule(void(*)(void*),void*,int);
int th14_webgpu_request_frames(EM_BOOL(*)(double,void*),void*);
int th14_webgpu_archive_open(const void*,unsigned);
void th14_webgpu_archive_cancel();
void th14_webgpu_archive_schedule(void(*)(void*),void*,const char* const*,unsigned);
int th14_webgpu_archive_status();
// Export trampolines so ccall({async:true}) owns the bottom of every suspended
// callback stack, including indirect loading workers and animation frames.
EMSCRIPTEN_KEEPALIVE void th14_webgpu_invoke(std::uint32_t callback,std::uint32_t data){reinterpret_cast<void(*)(void*)>(std::uintptr_t(callback))(reinterpret_cast<void*>(std::uintptr_t(data)));}
EMSCRIPTEN_KEEPALIVE int th14_webgpu_frame(std::uint32_t callback,double time,std::uint32_t data){return reinterpret_cast<EM_BOOL(*)(double,void*)>(std::uintptr_t(callback))(time,reinterpret_cast<void*>(std::uintptr_t(data)));}
}
#endif
