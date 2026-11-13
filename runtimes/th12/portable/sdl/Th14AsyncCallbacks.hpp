#pragma once
#include <emscripten.h>
#include <emscripten/eventloop.h>
#include <cstdint>
// Used only by TH14's GLES Application translation unit. Keep these exports
// distinct from the optional WebGPU host and its archive integration.
using Th14AsyncWork=void(*)(void*);
using Th14AsyncFrame=EM_BOOL(*)(double,void*);
EM_JS(void,th14_async_schedule_impl,(Th14AsyncWork callback,void* data,int delay),{
    Module['th14AsyncRuntime'].schedule(callback,data,delay);
});
EM_JS(int,th14_async_request_frames_impl,(Th14AsyncFrame callback,void* data),{
    return Module['th14AsyncRuntime'].startFrames(callback,data);
});
// The JS host serializes asynchronous ccalls outside the SDK event loop.
// Own each scheduled callback, and each frame loop until its final return,
// so unrelated pthread mailbox callbacks cannot exit the live runtime.
inline void th14_async_schedule(Th14AsyncWork callback,void* data,int delay){
    emscripten_runtime_keepalive_push();
    th14_async_schedule_impl(callback,data,delay);
}
inline int th14_async_request_frames(Th14AsyncFrame callback,void* data){
    emscripten_runtime_keepalive_push();
    return th14_async_request_frames_impl(callback,data);
}
extern "C" {
EMSCRIPTEN_KEEPALIVE void th14_async_invoke(std::uint32_t callback,std::uint32_t data){reinterpret_cast<void(*)(void*)>(std::uintptr_t(callback))(reinterpret_cast<void*>(std::uintptr_t(data)));emscripten_runtime_keepalive_pop();}
EMSCRIPTEN_KEEPALIVE int th14_async_frame(std::uint32_t callback,double time,std::uint32_t data){const int again=reinterpret_cast<EM_BOOL(*)(double,void*)>(std::uintptr_t(callback))(time,reinterpret_cast<void*>(std::uintptr_t(data)));if(!again)emscripten_runtime_keepalive_pop();return again;}
}
