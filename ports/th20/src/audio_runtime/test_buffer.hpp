#pragma once
#include "music_stream.hpp"
#include <array>
#include <vector>

// Test-only COM interface implementation. Both the unmodified original CPU
// routines and recovered C++ call this same documented DirectSound boundary.
struct BufferCall {
    unsigned operation;std::uint32_t a=0,b=0,c=0;
    bool operator==(const BufferCall&) const=default;
};
struct TestBuffer {
    std::uintptr_t* vtable;
    std::vector<BufferCall> calls;
    std::vector<std::uint8_t> bytes;
    DWORD status=0,play_cursor=0,write_cursor=0;
    HRESULT method_result=S_OK;
    explicit TestBuffer(std::size_t capacity=4096):vtable(table().data()),bytes(capacity) {}
    IDirectSoundBuffer* as_buffer() {return reinterpret_cast<IDirectSoundBuffer*>(this);}
    static TestBuffer& get(void* self) {return *static_cast<TestBuffer*>(self);}
    static ULONG WINAPI release(void* self) {get(self).calls.push_back({2});return 1;}
    static HRESULT WINAPI position(void* self,DWORD* play,DWORD* write) {
        auto& s=get(self);s.calls.push_back({4,play?1u:0u,write?1u:0u});if(play)*play=s.play_cursor;if(write)*write=s.write_cursor;return s.method_result;
    }
    static HRESULT WINAPI get_status(void* self,DWORD* status) {
        auto& s=get(self);s.calls.push_back({9});*status=s.status;return s.method_result;
    }
    static HRESULT WINAPI lock(void* self,DWORD offset,DWORD size,void** first,DWORD* first_size,void** second,DWORD* second_size,DWORD flags) {
        auto& s=get(self);s.calls.push_back({11,offset,size,flags});
        if(FAILED(s.method_result))return s.method_result;
        if(offset>s.bytes.size() || size>s.bytes.size()-offset) return E_INVALIDARG;
        *first=s.bytes.data()+offset;*first_size=size;if(second)*second=nullptr;if(second_size)*second_size=0;return S_OK;
    }
    static HRESULT WINAPI play(void* self,DWORD zero,DWORD priority,DWORD flags) {
        auto& s=get(self);s.calls.push_back({12,zero,priority,flags});return s.method_result;
    }
    static HRESULT WINAPI set_position(void* self,DWORD value) {auto& s=get(self);s.calls.push_back({13,value});return s.method_result;}
    static HRESULT WINAPI volume(void* self,LONG value) {auto& s=get(self);s.calls.push_back({15,static_cast<std::uint32_t>(value)});return s.method_result;}
    static HRESULT WINAPI pan(void* self,LONG value) {auto& s=get(self);s.calls.push_back({16,static_cast<std::uint32_t>(value)});return s.method_result;}
    static HRESULT WINAPI stop(void* self) {auto& s=get(self);s.calls.push_back({18});return s.method_result;}
    static HRESULT WINAPI unlock(void* self,void*,DWORD first,void*,DWORD second) {auto& s=get(self);s.calls.push_back({19,first,second});return s.method_result;}
    static HRESULT WINAPI restore(void* self) {auto& s=get(self);s.calls.push_back({20});s.status&=~DSBSTATUS_BUFFERLOST;return s.method_result;}
    static std::array<std::uintptr_t,21>& table() {
        static std::array<std::uintptr_t,21> entries=[] {
            std::array<std::uintptr_t,21> v{};
            v[2]=reinterpret_cast<std::uintptr_t>(release);v[4]=reinterpret_cast<std::uintptr_t>(position);
            v[9]=reinterpret_cast<std::uintptr_t>(get_status);v[11]=reinterpret_cast<std::uintptr_t>(lock);
            v[12]=reinterpret_cast<std::uintptr_t>(play);v[13]=reinterpret_cast<std::uintptr_t>(set_position);
            v[15]=reinterpret_cast<std::uintptr_t>(volume);v[16]=reinterpret_cast<std::uintptr_t>(pan);
            v[18]=reinterpret_cast<std::uintptr_t>(stop);v[19]=reinterpret_cast<std::uintptr_t>(unlock);
            v[20]=reinterpret_cast<std::uintptr_t>(restore);return v;
        }();return entries;
    }
};
