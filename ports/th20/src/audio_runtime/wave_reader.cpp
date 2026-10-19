#include "wave_reader.hpp"
#include <cstring>
#ifdef TH_SDL3
#include "platform/Files.hpp"
namespace th20::web::audio {
bool music_is_enabled();
void track_reader(void*);
void* current_reader();
}
#endif
#include <stdexcept>

namespace th20::source::audio {
WaveReader::WaveReader() noexcept {std::memset(this,0,sizeof *this);}
WaveReader::~WaveReader() {close();}
HRESULT WaveReader::open_file(const char* path,TrackFormat* format,std::uint32_t mode,std::uint32_t base_offset) {
    file_mode=mode;memory_mode=0;
    if(file_mode!=1 || !path) return E_INVALIDARG;
#ifdef TH_SDL3
    file=reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(web::files::open(path,false)));
    if(!file) return E_FAIL;
    web::audio::track_reader(this);
#else
    wchar_t wide[262]{};
    if(!MultiByteToWideChar(932,0,path,-1,wide,260)) return E_INVALIDARG;
    file=CreateFileW(wide,GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0x08000080,nullptr);
    if(file==INVALID_HANDLE_VALUE) return E_FAIL;
#endif
    track=format;filename=path;reset(false,0,base_offset);initial_remaining=remaining;looped=0;return S_OK;
}
HRESULT WaveReader::open_memory(const std::uint8_t* bytes,std::uint32_t size,TrackFormat* format,int flag) {
    track=format;memory_size=size;memory_start=bytes;memory_current=memory_start;memory_mode=1;
    return flag==1?S_OK:E_NOTIMPL; // original returns E_NOTIMPL after initializing when flag=0
}
HRESULT WaveReader::reopen(TrackFormat* format,std::uint32_t position,std::uint32_t base_offset) {
    if(memory_mode) return E_FAIL;
#ifdef TH_SDL3
    // The browser file host uses zero for a closed handle. The Win32 sentinel
    // is INVALID_HANDLE_VALUE; checking only that value leaves a closed OGG
    // stream permanently silent after a resource change.
    if(!file) open_file(filename,format,1,base_offset);
#else
    if(file==INVALID_HANDLE_VALUE) open_file(filename,format,1,base_offset);
#endif
#ifdef TH_SDL3
    if(!file) return E_FAIL;
#else
    if(file==INVALID_HANDLE_VALUE) return E_FAIL;
#endif
    looped=0;track=format;reset(false,position,base_offset);initial_remaining=remaining;return S_OK;
}
HRESULT WaveReader::read(void* output,std::uint32_t requested,std::uint32_t* received) {
    if(memory_mode) {
        if(!memory_current) return CO_E_NOTINITIALIZED;
        if(received) *received=0;
        const auto consumed=static_cast<std::uint32_t>(memory_current-memory_start);
        if(consumed>memory_size) throw std::out_of_range("Audio memory cursor exceeds allocation");
        if(requested>memory_size-consumed) requested=memory_size-consumed;
#ifdef TH_SDL3
        if(!web::audio::music_is_enabled()) {std::memset(output,track->format.wBitsPerSample==8?0x80:0,requested);memory_current+=requested;if(received)*received=requested;return S_OK;}
#endif
        std::memcpy(output,memory_current,requested);memory_current+=requested;
        if(received) *received=requested;return S_OK;
    }
    if(!file) return CO_E_NOTINITIALIZED;
    if(!output || !received) return E_INVALIDARG;
    const auto amount=requested>remaining?remaining:requested;
    remaining-=amount;
#ifdef TH_SDL3
    if(!web::audio::music_is_enabled()) {
        // Music-disabled keeps stream timing; the game reads silence.
        std::memset(output,track->format.wBitsPerSample==8?0x80:0,amount);*received=amount;return S_OK;
    }
    const auto actual=web::files::read(reinterpret_cast<std::uintptr_t>(file),output,amount);*received=actual;
#else
    DWORD actual=0;ReadFile(file,output,amount,&actual,nullptr);*received=actual;
#endif
    return S_OK; // original ignores the BOOL return; failed I/O's byte count is outside the proven domain
}
HRESULT WaveReader::reset(bool loop,std::uint32_t position,std::uint32_t base_offset) {
    if(memory_mode) {
        memory_current=memory_start;
        if(static_cast<std::int32_t>(track->total_bytes)>0) memory_size=track->total_bytes;
        if(loop && static_cast<std::int32_t>(track->loop_start)>0) memory_current+=track->loop_start;
        return S_OK; // original memory path ignores position
    }
    if(file==INVALID_HANDLE_VALUE || !file) return CO_E_NOTINITIALIZED;
    if(!loop || static_cast<std::int32_t>(track->loop_start)<1) {
        if(track->total_bytes<=position) position-=track->total_bytes-track->loop_start;
#ifdef TH_SDL3
        web::files::seek(reinterpret_cast<std::uintptr_t>(file),static_cast<std::int32_t>(base_offset+track->file_offset+position),0);
#else
        SetFilePointer(file,static_cast<LONG>(base_offset+track->file_offset+position),nullptr,FILE_BEGIN);
#endif
        remaining=track->total_bytes-position;
    } else {
#ifdef TH_SDL3
        web::files::seek(reinterpret_cast<std::uintptr_t>(file),static_cast<std::int32_t>(base_offset+track->file_offset+track->loop_start),0);
#else
        SetFilePointer(file,static_cast<LONG>(base_offset+track->file_offset+track->loop_start),nullptr,FILE_BEGIN);
#endif
        remaining=track->total_bytes-track->loop_start;
    }
    return S_OK;
}
HRESULT WaveReader::close() {
    if(file_mode==1) {
#ifdef TH_SDL3
        web::files::close(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(file)));file=nullptr;
        if(web::audio::current_reader()==this) web::audio::track_reader(nullptr);
#else
        CloseHandle(file);file=INVALID_HANDLE_VALUE;
#endif
    }
    return S_OK;
}
std::uint32_t WaveReader::tell() const {
#ifdef TH_SDL3
    return web::files::tell(reinterpret_cast<std::uintptr_t>(file));
#else
    return SetFilePointer(file,0,nullptr,FILE_CURRENT);
#endif
}
}

#ifdef TH_SDL3
namespace th20::source::audio {
void reopen_music_stream_file() {
    // Drop the streaming handle; WaveReader::reopen reopens lazily against
    // the fresh bytes the launcher injected into MEMFS.
    if(auto* reader=static_cast<WaveReader*>(web::audio::current_reader())) reader->close();
}
}
#endif
