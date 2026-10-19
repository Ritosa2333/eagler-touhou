#include "music_stream.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <xmmintrin.h>

namespace th20::source::audio {
namespace {
std::int32_t wrap_sub(std::int32_t a,std::uint32_t b) {return static_cast<std::int32_t>(static_cast<std::uint32_t>(a)-b);}
std::int32_t wrap_mul(std::int32_t a,std::uint32_t b) {return static_cast<std::int32_t>(static_cast<std::uint32_t>(a)*b);}
void release_buffers(MusicStream& stream) {
    if(stream.buffers) {
        for(unsigned i=0;i<stream.buffer_count;++i) if(stream.buffers[i]) stream.buffers[i]->Release();
        runtime::release_bytes(stream.buffers);stream.buffers=nullptr;
    }
}
void destroy_wave(WaveReader* wave) {
    if(!wave) return;
    wave->~WaveReader();std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));::operator delete(wave);
}
std::uint32_t base_offset(const MusicStream& stream) {return stream.sound->retained_57d4;}// 0x5c0004
double read_clock(MusicStream& stream) {
    if(!stream.sound->context || !stream.sound->context->read_clock) throw std::logic_error("Music clock service missing");
    return stream.sound->context->read_clock();
}
HRESULT notifications(SoundBuffer* buffer,unsigned count,std::uint32_t size,HANDLE event) {
#ifdef TH_SDL3
    // One native buffer owns both surfaces; no separate notify object exists.
    std::vector<DSBPOSITIONNOTIFY> positions(count);
    for(unsigned i=0;i<count;++i) {positions[i].dwOffset=size*i-1+size;positions[i].hEventNotify=event;}
    return buffer->SetNotificationPositions(count,positions.data());
#else
    IDirectSoundNotify* notify=nullptr;
    auto result=buffer->QueryInterface(IID_IDirectSoundNotify,reinterpret_cast<void**>(&notify));
    if(FAILED(result)) return result;
    if(!notify) return E_POINTER;
    std::vector<DSBPOSITIONNOTIFY> positions(count);
    for(unsigned i=0;i<count;++i) {positions[i].dwOffset=size*i-1+size;positions[i].hEventNotify=event;}
    result=notify->SetNotificationPositions(count,positions.data());notify->Release();return result;
#endif
}
}
MusicStream::MusicStream(SoundInf& parent,SoundBuffer* initial,std::uint32_t bytes,WaveReader* reader,std::uint32_t interval)
    :retained_04(0),buffers(nullptr),buffer_size(bytes),wave(reader),buffer_count(1),fade_remaining(0),fade_duration(0),fade_mode(0),
     play_priority(0),play_flags(0),retained_2c(0),retained_30(0),retained_34(0),started_at(0),paused_at(0),paused_duration(0),retained_50(0),
     playing(0),paused(0),description{},owner(nullptr),last_play_cursor(0),played_bytes(0),next_write(0),retained_94(0),silence(0),
     notification_size(interval),notification_event(nullptr),busy(0),sound(&parent) {
    buffers=static_cast<SoundBuffer**>(runtime::allocate_bytes(sizeof *buffers));
    if(!buffers) throw std::bad_alloc();
    buffers[0]=initial;fill(initial,false,0);initial->SetCurrentPosition(0);
}
MusicStream::~MusicStream() {release_buffers(*this);destroy_wave(wave);wave=nullptr;}
HRESULT MusicStream::restore(SoundBuffer* target,BOOL* restored) {
    if(!target) return CO_E_NOTINITIALIZED;
    if(restored) *restored=FALSE;
#ifdef TH_SDL3
    return target->Restore(); // software buffers are never lost
#else
    DWORD status=0;auto result=target->GetStatus(&status);if(FAILED(result)) return result;
    if(status&DSBSTATUS_BUFFERLOST) {
        do {
            result=target->Restore();if(result==DSERR_BUFFERLOST) Sleep(10);
            result=target->Restore();
        } while(result!=S_OK);
        if(restored) *restored=TRUE;
    }
    return S_OK;
#endif
}
SoundBuffer* MusicStream::buffer(unsigned index) const {return buffers && index<buffer_count?buffers[index]:nullptr;}
SoundBuffer* MusicStream::free_buffer() {
    if(!buffers) return nullptr;
    unsigned index=0;
    for(;index<buffer_count;++index) if(buffers[index]) {
        DWORD status=0;buffers[index]->GetStatus(&status);if(!(status&DSBSTATUS_PLAYING)) break;
    }
    if(index==buffer_count) index=static_cast<unsigned>(std::rand())%buffer_count;
    return buffers[index];
}
HRESULT MusicStream::fill(SoundBuffer* target,bool loop,std::uint32_t position) {
    if(!target) return CO_E_NOTINITIALIZED;
    auto result=restore(target,nullptr);if(FAILED(result)) return result;
    void* pointer=nullptr;DWORD size=0;
    result=target->Lock(0,buffer_size,&pointer,&size,nullptr,nullptr,0);if(FAILED(result)) return result;
    wave->reset(false,position,base_offset(*this));
    std::uint32_t received=0;result=wave->read(pointer,size,&received);if(FAILED(result)) return result;
    const int silent=wave->track->format.wBitsPerSample==8?0x80:0;
    if(received==0) std::memset(pointer,silent,size);
    else if(received<size) {
        if(!loop) std::memset(static_cast<std::uint8_t*>(pointer)+received,silent,size-received);
        else for(std::uint32_t used=received;used<size;used+=received) {
            result=wave->reset(true,0,base_offset(*this));if(FAILED(result)) return result;
            result=wave->read(static_cast<std::uint8_t*>(pointer)+used,size-used,&received);if(FAILED(result)) return result;
            if(!received) throw std::runtime_error("Zero-byte audio loop cannot fill buffer");
        }
    }
    target->Unlock(pointer,size,nullptr,0);return S_OK;
}
HRESULT MusicStream::reset_all_positions() {
    if(!buffers) return CO_E_NOTINITIALIZED;
    HRESULT result=0;for(unsigned i=0;i<buffer_count;++i) result|=buffers[i]->SetCurrentPosition(0);return result;
}
HRESULT MusicStream::reset(std::uint32_t position) {
    if(!buffers || !buffers[0] || !wave) return CO_E_NOTINITIALIZED;
    last_play_cursor=played_bytes=next_write=retained_94=silence=0;
    BOOL restored=FALSE;auto result=restore(buffers[0],&restored);if(FAILED(result)) return result;
    if(restored) {result=fill(buffers[0],false,position);if(FAILED(result)) return result;}
    wave->reset(wave->looped==1,position,base_offset(*this));
    return buffers[0]->SetCurrentPosition(0);
}
HRESULT MusicStream::handle_notification(bool loop) {
    if(!buffers || !wave) return CO_E_NOTINITIALIZED;
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(12));
    DWORD play_cursor=0,write_cursor=0;buffers[0]->GetCurrentPosition(&play_cursor,&write_cursor);
    const auto delta=write_cursor-notification_size;
    if(!((next_write<delta || write_cursor<=next_write)
        && (static_cast<std::int32_t>(delta)>=0 || next_write<buffer_size-notification_size))) return S_OK;
    BOOL restored=FALSE;auto result=restore(buffers[0],&restored);if(FAILED(result)) return result;
    if(restored) return fill(buffers[0],false,0);
    void* first=nullptr;void* second=nullptr;DWORD size=0,second_size=0;
    result=buffers[0]->Lock(next_write,notification_size,&first,&size,&second,&second_size,0);if(FAILED(result)) return result;
    if(second) return E_UNEXPECTED; // valid notification blocks do not wrap
    const int silent=wave->track->format.wBitsPerSample==8?0x80:0;
    std::uint32_t received=size;
    if(!silence) {result=wave->read(first,size,&received);if(FAILED(result)) return result;}
    else std::memset(first,silent,size);
    if(received<size) {
        wave->looped=1;
        if(!loop) {std::memset(static_cast<std::uint8_t*>(first)+received,silent,size-received);silence=1;}
        else for(std::uint32_t used=received;used<size;used+=received) {
            result=wave->reset(true,0,base_offset(*this));if(FAILED(result)) return result;
            result=wave->read(static_cast<std::uint8_t*>(first)+used,size-used,&received);if(FAILED(result)) return result;
            if(!received) throw std::runtime_error("Zero-byte audio loop cannot refill buffer");
        }
    }
    buffers[0]->Unlock(first,size,nullptr,0);
    result=buffers[0]->GetCurrentPosition(&play_cursor,nullptr);if(FAILED(result)) return result;
    played_bytes+=(play_cursor<last_play_cursor?buffer_size-last_play_cursor:0u-last_play_cursor)+play_cursor;
    last_play_cursor=play_cursor;next_write=(next_write+size)%buffer_size;return S_OK;
}
HRESULT MusicStream::set_volume(std::int32_t attenuation) {return buffers[0]->SetVolume(music_volume(attenuation,sound->music_level));}
HRESULT MusicStream::stop(bool close_file) {
    if(!buffers) return CO_E_NOTINITIALIZED;
    HRESULT result=0;playing=paused=0;
    for(unsigned i=0;i<buffer_count;++i) {result|=buffers[i]->Stop();result|=buffers[i]->SetCurrentPosition(0);}
    fade_mode=0;if(close_file) wave->close();return result;
}
HRESULT MusicStream::play(DWORD priority,DWORD flags,std::uint32_t position) {
    if(!buffers) return CO_E_NOTINITIALIZED;
    auto* target=free_buffer();if(!target) return CO_E_NOTINITIALIZED;
    BOOL restored=FALSE;auto result=restore(target,&restored);if(FAILED(result)) return result;
    if(restored) {result=fill(target,false,position);if(FAILED(result)) return result;reset_all_positions();}
    fade_mode=fade_remaining=fade_duration=0;set_volume(0);playing=1;play_priority=priority;play_flags=flags;retained_30=0;
    started_at=read_clock(*this);retained_50=paused_duration=paused_at=0;
    return target->Play(0,priority,flags);
}
HRESULT MusicStream::pause() {
    if(!buffers) return CO_E_NOTINITIALIZED;
    if(!playing) {paused=0;return CO_E_NOTINITIALIZED;}
    playing=0;paused=1;const auto result=buffers[0]->Stop();paused_at=read_clock(*this);
    wave->saved_position=wave->tell()-wave->track->file_offset;wave->close();return result;
}
HRESULT MusicStream::resume() {
    if(!buffers || !paused) return CO_E_NOTINITIALIZED;
    paused=0;reopen(wave->track,wave->saved_position);playing=1;
    paused_duration=(read_clock(*this)-paused_at)+paused_duration;
    return buffers[0]->Play(0,play_priority,play_flags);
}
HRESULT MusicStream::reopen(TrackFormat* format,std::uint32_t position) {return wave->reopen(format,position,base_offset(*this));}
HRESULT MusicStream::recreate(TrackFormat* format) {
    playing=0;release_buffers(*this);
    buffers=static_cast<SoundBuffer**>(runtime::allocate_bytes(buffer_count*sizeof *buffers));
    if(!buffers) throw std::bad_alloc();std::memset(buffers,0,buffer_count*sizeof *buffers);
    auto copy=description;copy.lpwfxFormat=&format->format;
    for(unsigned i=0;i<buffer_count;++i) {
        auto result=owner->device->CreateSoundBuffer(&copy,&buffers[i],nullptr);if(FAILED(result)) return result;
        result=notifications(buffers[i],16,notification_size,notification_event);if(FAILED(result)) return result;
    }
    return S_OK;
}
void MusicStream::fade_out(float seconds) {
    fade_mode=1;const float frames=seconds*60.0f;fade_remaining=fade_duration=_mm_cvtt_ss2si(_mm_set_ss(frames));
}
int MusicStream::tick_fade(unsigned mode) {
    if(fade_mode!=static_cast<int>(mode)) return 0;
    fade_remaining=wrap_sub(fade_remaining,1);
    if(fade_remaining<1) {fade_mode=0;if(mode==1) buffers[0]->Stop();return 1;}
    if(fade_duration==0) throw std::domain_error("Audio fade duration cannot be zero while active");
    const int factor=(mode==1 || mode==2)?5000:1000;
    const auto quotient=wrap_mul(fade_remaining,static_cast<unsigned>(factor))/fade_duration;
    const auto attenuation=(mode==1 || mode==4)?wrap_sub(quotient,static_cast<unsigned>(factor)):wrap_sub(0,static_cast<unsigned>(quotient));
    set_volume(attenuation);return 0;
}
void update_stream(SoundInf& sound) {if(sound.stream) for(unsigned mode:{1u,2u,4u,3u}) sound.stream->tick_fade(mode);}
double MusicStream::playback_seconds() {
    if(!wave||!wave->track) {std::fprintf(stderr,"playback_seconds: wave=%p track=%p\n",static_cast<void*>(wave),wave?static_cast<void*>(wave->track):nullptr);throw std::logic_error("Music stream has no wave track");}
    auto& f=*wave->track;
    const double now=read_clock(*this);
    double elapsed=now-(started_at+paused_duration);
    const double limit=((static_cast<double>(static_cast<std::int32_t>(f.total_bytes))/(static_cast<double>(f.format.nSamplesPerSec)/8.0))
        /static_cast<double>(f.format.wBitsPerSample))/static_cast<double>(f.format.nChannels);
    const double loop=((static_cast<double>(static_cast<std::int32_t>(f.total_bytes))-static_cast<double>(static_cast<std::int32_t>(f.loop_start)))
        /static_cast<double>(f.format.nSamplesPerSec))/(static_cast<double>(f.format.wBitsPerSample)/8.0)/static_cast<double>(f.format.nChannels);
    if(limit<=elapsed && !(loop>0)) throw std::domain_error("Invalid non-progressing audio loop");
    while(limit<=elapsed) elapsed-=loop;return elapsed;
}
HRESULT MusicStream::seek_seconds(double seconds) {
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(12));
    buffers[0]->Stop();const auto& f=*wave->track;
    const auto samples=static_cast<std::int64_t>(static_cast<double>(f.format.nSamplesPerSec)*seconds);
    std::uint32_t position=static_cast<std::uint32_t>(f.format.nChannels)*(f.format.wBitsPerSample>>3)*static_cast<std::uint32_t>(samples)-f.format.nBlockAlign;
    if(static_cast<std::int32_t>(position)<0) position=0;
    recreate(wave->track);reset(position);wave->initial_remaining=wave->remaining;
    fill(buffers[0],wave->track->total_bytes!=0,position);play(play_priority,play_flags,position);started_at-=seconds;return S_OK;
}
HRESULT MusicStream::replace_track(TrackFormat* format) {const auto seconds=playback_seconds();wave->track=format;seek_seconds(seconds);return S_OK;}
HRESULT create_music_stream(SoundInf& sound,WaveReader* reader,DWORD flags,const GUID& algorithm,unsigned count,std::uint32_t size,HANDLE event) {
    if(!sound.device_owner || !sound.device_owner->device) return CO_E_NOTINITIALIZED;
    DSBUFFERDESC description{};description.dwSize=sizeof description;description.dwFlags=flags|0x18188;
    description.dwBufferBytes=size*count;description.lpwfxFormat=&reader->track->format;description.guid3DAlgorithm=algorithm;
    SoundBuffer* buffer=nullptr;
    auto result=sound.device_owner->device->CreateSoundBuffer(&description,&buffer,nullptr);if(FAILED(result)) {destroy_wave(reader);return result;}
    result=notifications(buffer,count,size,event);if(FAILED(result)) {buffer->Release();destroy_wave(reader);return result;}
    auto* stream=new MusicStream(sound,buffer,description.dwBufferBytes,reader,size);
    sound.stream=stream;stream->description=description;stream->owner=sound.device_owner;stream->notification_event=event;stream->busy=0;return S_OK;
}
}
