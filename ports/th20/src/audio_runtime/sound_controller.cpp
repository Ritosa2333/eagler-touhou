#include "music_stream.hpp"
#include <cstring>
#include <stdexcept>
#include <xmmintrin.h>
#ifdef TH_SDL3
#include "platform/Files.hpp"
#include "platform/Time.hpp"
#endif

namespace th20::source::audio {
namespace {
template<class T> void destroy_allocated(T*& object) {
    if(!object) return;
    object->~T();
    {std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));::operator delete(object);}
    object=nullptr;
}
void copy_name(char* target,std::size_t capacity,const char* source) {
    if(strcpy_s(target,capacity,source)!=0) throw std::length_error("Audio name exceeds original fixed buffer");
}
std::uint32_t notification_bytes(const TrackFormat& track) {
    const auto alignment=track.format.nBlockAlign;
    if(!alignment) throw std::domain_error("Music block alignment is zero");
    const auto bytes=(track.format.nSamplesPerSec*4u*alignment)>>4;
    return bytes-bytes%alignment;
}
}
SoundInf::~SoundInf()=default; // 0x426030 destroys the pmr vector; shutdown is explicit.
int SoundInf::load_formats(const char* name) {
    if(!context || !context->read_resource) throw std::logic_error("Audio requires a resource reader");
    auto bytes=context->read_resource(name);
    track_formats=nullptr;
    if(!bytes) return -1;
    track_formats=static_cast<TrackFormat*>(runtime::allocate_bytes(bytes->size()));
    if(!track_formats) return -1;
    std::memcpy(track_formats,bytes->data(),bytes->size());return 0;
}
int SoundInf::apply_configuration() {
    for(auto& request:requests) request.id=-1;
    if(!device_owner) return -1;
    if(!direct_sound) return 0;
    if(!context) throw std::logic_error("Audio configuration service missing");
    music_level=static_cast<std::int8_t>(context->configuration.value_7e);
    effect_level=static_cast<std::int8_t>(context->configuration.value_7f);
    std::int32_t attenuation;
    if(!effect_level) attenuation=-10000;
    else {
        const float inverse=1.0f-static_cast<float>(music_level)/100.0f;
        const float square=inverse*inverse;
        const float value=5000.0f*(1.0f-square*square);
        attenuation=static_cast<std::int32_t>(static_cast<std::uint32_t>(_mm_cvtt_ss2si(_mm_set_ss(value)))-5000u);
    }
    retained_57e4=static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::uint32_t>(attenuation)*9u)/10);
    return 0;
}
bool SoundInf::uses_preload() const {
    if(!context) throw std::logic_error("Audio configuration service missing");
    return (context->configuration.flags&0x10)!=0;
}
void SoundInf::initialize(HWND target,Context& services) {
    context=&services;
    for(auto& request:requests) {audio::initialize(request);request.id=-1;}
    device_owner=new DeviceOwner;
    if(FAILED(device_owner->initialize(target,DSSCL_PRIORITY,2,44100,16))) {
        // 0x56dc8c: exact CP932 bytes, matching Log's byte buffer.
        runtime::log_printf(context->log,"DirectSound \x83\x49\x83\x75\x83\x57\x83\x46\x83\x4e\x83\x67\x82\xcc\x8f\x89\x8a\xfa\x89\xbb\x82\xaa\x8e\xb8\x94\x73\x82\xb5\x82\xbd\x82\xe6\r\n");
        destroy_allocated(device_owner);return;
    }
    direct_sound=device_owner->device;notify_thread=nullptr;
#ifndef TH_SDL3
    WAVEFORMATEX format{};format.wFormatTag=WAVE_FORMAT_PCM;format.nChannels=2;format.nSamplesPerSec=44100;
    format.nAvgBytesPerSec=176400;format.nBlockAlign=4;format.wBitsPerSample=16;
    DSBUFFERDESC description{};description.dwSize=sizeof description;description.dwFlags=0x8008;
    description.dwBufferBytes=0x8000;description.lpwfxFormat=&format;
    if(FAILED(direct_sound->CreateSoundBuffer(&description,&silent_buffer,nullptr))) return;
    void* first=nullptr;void* second=nullptr;DWORD first_bytes=0,second_bytes=0;
    if(FAILED(silent_buffer->Lock(0,0x8000,&first,&first_bytes,&second,&second_bytes,0))) return;
    std::memset(first,0,first_bytes); // original clears only the first lock region
    silent_buffer->Unlock(first,first_bytes,second,second_bytes);silent_buffer->Play(0,0,DSBPLAY_LOOPING);
#endif
    // The software mixer needs no keep-alive silence buffer and no service
    // timer: refills are driven by mixer-side notification crossings.
#ifdef TH_SDL3
    web::audio::set_notification_handler([](std::uint32_t,void* userdata){
        auto& sound=*static_cast<SoundInf*>(userdata);
        if(sound.stream && sound.stream->playing) {sound.stream->busy=1;sound.stream->handle_notification(true);sound.stream->busy=0;}
    },this);
#endif
    music_level=effect_level=100;
#ifndef TH_SDL3
    SetTimer(target,0,250,nullptr);
#endif
    window=target;
    for(unsigned i=0;i<72;++i) if(load_wave(i,effect_filenames[i])!=0) {
        runtime::log_printf(context->log,"error : Sound \x83\x74\x83\x40\x83\x43\x83\x8b\x82\xaa\x93\xc7\x82\xdd\x8d\x9e\x82\xdf\x82\xc8\x82\xa2 \x83\x66\x81\x5b\x83\x5e\x82\xf0\x8a\x6d\x94\x46 %s\r\n",effect_filenames[i]);return;
    }
    for(auto& effect:effects) if(create_effect(effect)!=0) return;
    std::memset(source_buffers,0,sizeof source_buffers); // ownership transferred into effect channels
    runtime::log_printf(context->log,"DirectSound \x82\xcd\x90\xb3\x8f\xed\x82\xc9\x8f\x89\x8a\xfa\x89\xbb\x82\xb3\x82\xea\x82\xdc\x82\xb5\x82\xbd\r\n");
}
std::size_t SoundInf::find_track(const char* name) const {
    if(!track_formats) throw std::logic_error("Music format resource has not been loaded");
    const char* separator=std::strrchr(name,'/');if(!separator) separator=std::strrchr(name,'\\');
    char basename[128];copy_name(basename,sizeof basename,separator?separator+1:name);
    std::size_t index=0;
    while(track_formats[index].name[0] && std::strcmp(track_formats[index].name,basename)!=0) ++index;
    // 0x428540 explicitly replaces an unmatched sentinel index with zero.
    return track_formats[index].name[0]?index:0;
}
void SoundInf::free_preload(unsigned index) {
    auto& entry=preloaded.at(index);
    if(entry.allocation) {runtime::release_bytes(entry.allocation);entry.allocation=nullptr;}
}
void SoundInf::preload(unsigned index,const char* name) {
    if(index>=16) throw std::out_of_range("Music preload slot");
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(2));
    if(preloaded.size()<=index) preloaded.resize(index+1);
    auto& entry=preloaded[index];
    if(entry.allocation && std::strcmp(name,track_names[index])==0) return;
    copy_name(track_names[index],256,name);
    if(!uses_preload() || !device_owner) return;
    free_preload(index);
    auto& track=track_formats[find_track(name)];
    auto* bytes=static_cast<std::uint8_t*>(runtime::allocate_bytes(track.preload_bytes));
    if(!bytes) return;
#ifdef TH_SDL3
    const auto file=web::files::open(music_file,false);
    if(!file) {runtime::release_bytes(bytes);return;}
    web::files::seek(file,static_cast<std::int32_t>(track.file_offset),0);
    web::files::read(file,bytes,track.preload_bytes);
    web::files::close(file);
#else
    wchar_t path[262]{};MultiByteToWideChar(932,0,music_file,-1,path,260);
    HANDLE file=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0x08000080,nullptr);
    if(file==INVALID_HANDLE_VALUE) {runtime::release_bytes(bytes);return;}
    SetFilePointer(file,static_cast<LONG>(track.file_offset),nullptr,FILE_BEGIN);
    DWORD received=0;ReadFile(file,bytes,track.preload_bytes,&received,nullptr);CloseHandle(file);
#endif
    entry.format=&track;entry.allocation=entry.current=bytes;entry.size=track.preload_bytes;
}
int SoundInf::load_track(std::int32_t index) {
    if(!device_owner || !context || !context->configuration.value_75 || !direct_sound) return -1;
    if(index<0 || index>=16) throw std::out_of_range("Music preload slot");
    if(!uses_preload()) return reopen_track(track_names[index]);
    if(preloaded.size()<=static_cast<unsigned>(index)) preloaded.resize(index+1);
    auto& entry=preloaded[index];if(!entry.allocation) return -1;
    copy_name(current_track,256,track_names[index]);
    const auto chunk=notification_bytes(*entry.format);
#ifdef TH_SDL3
    notification=reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(web::audio::create_event()));
#else
    notification=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    // Original parameter is an unused HWND; source carries its owning SoundInf.
    notify_thread=CreateThread(nullptr,0,notification_thread,this,0,&notify_thread_id);
#endif
    auto* reader=new WaveReader;reader->open_memory(entry.current,entry.size,entry.format,0);
    if(FAILED(create_music_stream(*this,reader,0x10100,GUID_NULL,16,chunk,notification))) return -1;
    preloaded_index=index;return 0;
}
int SoundInf::reopen_track(const char* name) {
    if(!stream) return -1;
    stream->reopen(&track_formats[find_track(name)],0);copy_name(current_track,256,name);return 0;
}
int SoundInf::start_stream(const char* name) {
    copy_name(music_file,256,name);
    if(!device_owner || !direct_sound) return -1;
    stop_stream();
    if(!track_formats) throw std::logic_error("Music format resource has not been loaded");
    const auto chunk=notification_bytes(track_formats[0]);
#ifdef TH_SDL3
    notification=reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(web::audio::create_event()));
#else
    notification=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    notify_thread=CreateThread(nullptr,0,notification_thread,this,0,&notify_thread_id);
#endif
    auto* reader=new WaveReader;
    auto result=reader->open_file(music_file,track_formats,1,retained_57d4);
    if(FAILED(result)) {destroy_allocated(reader);return -1;}
    return FAILED(create_music_stream(*this,reader,0x10100,GUID_NULL,16,chunk,notification))?-1:0;
}
void SoundInf::stop_stream() {
    if(!stream) return;
    stream->stop(true);
#ifdef TH_SDL3
    web::audio::close_event(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(notification)));notification=nullptr;
#else
    if(notify_thread) {
        PostThreadMessageW(notify_thread_id,WM_QUIT,0,0);
        while(WaitForSingleObject(notify_thread,256)!=WAIT_OBJECT_0) PostThreadMessageW(notify_thread_id,WM_QUIT,0,0);
        CloseHandle(notify_thread);CloseHandle(notification);notify_thread=nullptr;
    }
#endif
    destroy_allocated(stream);
}
#ifndef TH_SDL3
DWORD WINAPI SoundInf::notification_thread(void* parameter) {
    auto& sound=*static_cast<SoundInf*>(parameter);bool finished=false;
    while(!finished) {
        const auto event=MsgWaitForMultipleObjects(1,&sound.notification,FALSE,INFINITE,0x1cbf);
        if(!sound.stream) finished=true;
        if(event==WAIT_OBJECT_0) {
            if(sound.stream && sound.stream->playing) {
                sound.stream->busy=1;sound.stream->handle_notification(true);sound.stream->busy=0;
            }
        } else if(event==WAIT_OBJECT_0+1) {
            MSG message{};while(PeekMessageW(&message,nullptr,0,0,PM_REMOVE)) if(message.message==WM_QUIT) finished=true;
        }
    }
    return 0;
}
#endif
int SoundInf::shutdown() {
    if(track_formats) {runtime::release_bytes(track_formats);track_formats=nullptr;}
    for(auto& effect:effects) release_effect(effect);
    if(device_owner) {
#ifndef TH_SDL3
        KillTimer(window,1);
#endif
        stop_stream();direct_sound=nullptr;
        if(silent_buffer) {silent_buffer->Stop();silent_buffer->Release();silent_buffer=nullptr;}
#ifdef TH_SDL3
        web::audio::set_notification_handler(nullptr,nullptr);
#endif
        destroy_allocated(stream);destroy_allocated(device_owner);
        for(unsigned i=0;i<preloaded.size();++i) free_preload(i);
    }
    return 0;
}
int SoundInf::poll() {
    std::unique_lock<std::recursive_mutex> initial(runtime::shared_locks().slot(11));
    if(!device_owner) return 0; // original returns zero from its unlocking helper
    initial.unlock();
    Command* current=commands;
    for(bool again=true;again;) {
        again=false;bool dequeue=true;
        std::unique_lock<std::recursive_mutex> lock(runtime::shared_locks().slot(11));
        auto advance=[&] {current->stage=static_cast<std::int32_t>(static_cast<std::uint32_t>(current->stage)+1);dequeue=false;};
        switch(current->type) {
        case 1:
            if(!uses_preload()) {preload(current->argument,current->name);again=true;}
            else if(current->stage!=0) advance();
            else {stop_stream();preload(current->argument,current->name);again=true;}
            break;
        case 2:
            if(!uses_preload() || current->argument<0) {
                if(!stream) break;
                switch(current->stage) {
                case 0: stream->stop(false);advance();break;
                case 1:
                    if(stream->busy) {dequeue=false;break;}
                    copy_name(queued_track,256,current->argument<0?current->name:track_names[current->argument]);
                    current->argument=static_cast<int>(find_track(queued_track));
                    stream->recreate(&track_formats[current->argument]);advance();break;
                case 2: stream->reopen(&track_formats[current->argument],0);advance();break;
                case 3: {
                    auto* target=stream->buffer(0);stream->reset(0);current->argument=stream->wave->track->total_bytes!=0;
                    if(SUCCEEDED(stream->fill(target,current->argument!=0,0))) advance();break;
                }
                case 4: stream->play(0,1,0);advance();break;
                default: if(current->stage<=6) advance();break;
                }
            } else {
                switch(current->stage) {
                case 0: if(load_track(current->argument)==0) advance();break;
                case 2: if(!stream || SUCCEEDED(stream->reset(0))) advance();break;
                case 5: {
                    auto* target=stream->buffer(0);current->argument=stream->wave->track->total_bytes!=0;
                    if(SUCCEEDED(stream->fill(target,current->argument!=0,0))) advance();break;
                }
                case 7: stream->play(0,1,0);advance();break;
                default: if(current->stage<=19) advance();break;
                }
            }
            break;
        case 3:
            if(stream) {if(current->stage==0) stream->stop(true);if(current->stage!=1) advance();}break;
        case 4:
            if(stream) {
                switch(current->stage) {
                case 0: stream->stop(true);break;
#ifdef TH_SDL3
                // No refill thread exists; the stream tears down immediately.
                case 1: web::audio::close_event(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(notification)));notification=nullptr;destroy_allocated(stream);break;
#else
                case 1: if(!notify_thread) goto command_done;PostThreadMessageW(notify_thread_id,WM_QUIT,0,0);break;
                case 2:
                    if(WaitForSingleObject(notify_thread,256)==WAIT_OBJECT_0) notify_thread=nullptr;
                    else {PostThreadMessageW(notify_thread_id,WM_QUIT,0,0);current->stage-=1;}
                    break;
                case 3: CloseHandle(notify_thread);CloseHandle(notification);notify_thread=nullptr;destroy_allocated(stream);break;
#endif
                case 10: goto command_done;
                }
                advance();
            }
            break;
        case 5: if(stream) stream->fade_out(static_cast<float>(current->argument));break;
        case 6: case 7:
            if(context->configuration.value_75==1 && stream) {
                if(stream->busy) {dequeue=false;break;}
                if(current->type==6) stream->pause();else stream->resume();
            }
            break;
        case 8: if(stream) stream->set_volume(0);break;
        case 9: if(stream) stream->replace_track(&track_formats[find_track(current->name)]);break;
        default: dequeue=false;break;
        }
command_done:
        if(dequeue) {
            unsigned copied=0;
            for(;copied<31 && current->type!=0;++copied,++current) *current=*(current+1);
            lock.unlock();
#ifdef TH_SDL3
            web::time::sleep(1);
#else
            Sleep(1);
#endif
            // Deliberately retain the advanced current pointer when preload
            // restarts the loop. 0x428125 jumps back without resetting it.
        } else again=false;
    }
    if(context->configuration.value_76) {
        std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(11));
        for(auto& request:requests) {
            if(request.id<0) break;
            const auto id=request.id;request.id=-1;
            if(request.count<0) {stop_effect(effects[id]);request.count=0;}
            else {
                std::uint32_t sum=0;for(int i=0;i<request.count;++i) sum+=static_cast<std::uint32_t>(request.pans[i]);
                auto pan=static_cast<std::int32_t>(sum);if(request.count>0) pan/=request.count;
                request.count=0;play_effect(effects[id],pan,effect_level);
            }
        }
    }
    return commands[0].type;
}
}
