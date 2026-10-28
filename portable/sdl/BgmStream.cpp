// Streams signed 16-bit interleaved PCM out of thbgm.dat at a byte offset,
// converted to f32 on read. Looping is left to miniaudio's data-source loop
// points, exactly like the upstream decoder path; the reader thread only
// prefetches the frames that playback will ask for next.
#include "BgmStream.hpp"
#include <algorithm>
#include <cstring>
namespace th12host::host {
namespace {constexpr size_t ChunkFrames=8192,RingChunks=12;} // 32 KB reads, ~2.2 s ahead
BgmReader::BgmReader(FILE* f):file(f){
#ifndef __EMSCRIPTEN__
    thread=std::thread([this]{run();});
#endif
}
BgmReader::~BgmReader(){{std::lock_guard<std::mutex> lock(mutex);quit=true;}wake.notify_all();if(thread.joinable())thread.join();}
void BgmReader::start(u64 offset_,u64 frames_,u64 loop_start_,u32 channels_,u64 frame){
    {std::lock_guard<std::mutex> lock(mutex);
        offset=offset_;frames=frames_;loop_start=std::min(loop_start_,frames_);channels=std::max(1u,channels_);next=std::min(frame,frames_);
        ring.assign(ChunkFrames*RingChunks*channels,0);first=size=0;++generation;active=frames>0;failed=false;}
    wake.notify_all();
}
void BgmReader::stop(){std::lock_guard<std::mutex> lock(mutex);active=false;++generation;first=size=0;}
size_t BgmReader::take(i16* out,size_t count){
#ifdef __EMSCRIPTEN__
    size_t done=0;
    while(done<count&&active&&!failed){
        if(next>=frames)next=loop_start;
        const auto n=std::min<u64>(count-done,frames-next);if(!n)break;
        if(std::fseek(file,long(offset+next*channels*2),SEEK_SET)!=0){failed=true;break;}
        const size_t got=std::fread(out+done*channels,channels*2,size_t(n),file);done+=got;next+=got;
        if(got<n){failed=true;break;}
    }
    return done;
#else
    std::unique_lock<std::mutex> lock(mutex);size_t done=0;
    while(done<count){
        if(!size){if(!active||failed)break;ready.wait(lock);continue;}
        const size_t n=std::min({size/channels,count-done,(ring.size()-first)/channels})*channels;
        std::memcpy(out+done*channels,ring.data()+first,n*sizeof(i16));first=(first+n)%ring.size();size-=n;done+=n/channels;
        wake.notify_one();
    }
    return done;
#endif
}
void BgmReader::run(){
    std::unique_lock<std::mutex> lock(mutex);
    while(!quit){
        if(!active||failed||ring.size()-size<ChunkFrames*channels){wake.wait(lock);continue;}
        // Read without the lock; a start() or stop() meanwhile discards the chunk.
        const u32 g=generation,ch=channels;const u64 at=next,n=std::min<u64>(ChunkFrames,frames-at),byte=offset+at*ch*2;
        chunk.resize(size_t(n*ch));lock.unlock();
        bool ok=byte==position||std::fseek(file,long(byte),SEEK_SET)==0;
        const size_t got=ok?std::fread(chunk.data(),ch*2,size_t(n),file):0;position=ok?byte+got*ch*2:~0ull;
        lock.lock();
        if(g!=generation)continue;
        for(size_t i=0;i<got*ch;){const size_t at_end=(first+size)%ring.size(),m=std::min(got*ch-i,ring.size()-at_end);
            std::memcpy(ring.data()+at_end,chunk.data()+i,m*sizeof(i16));size+=m;i+=m;}
        if(got<n)failed=true;
        next=at+got;if(next>=frames)next=loop_start;
        ready.notify_all();
    }
}

ma_result pcm_read(ma_data_source* source,void* out,ma_uint64 count,ma_uint64* read){
    auto& s=*static_cast<PcmStream*>(source);if(read)*read=0;
    const u64 n=std::min<u64>(count,s.frames-std::min(s.cursor,s.frames));if(!n||!s.reader)return MA_AT_END;
    s.scratch.resize(size_t(n*s.channels));
    const size_t got=s.reader->take(s.scratch.data(),size_t(n));
    auto* pcm=static_cast<float*>(out);for(size_t i=0;i<got*s.channels;++i)pcm[i]=float(s.scratch[i])/32768.f;
    // Report MA_AT_END together with the final frames: miniaudio's looping
    // treats a later empty MA_AT_END read as "no audio" and stops the loop.
    s.cursor+=got;if(read)*read=got;return got&&s.cursor<s.frames?MA_SUCCESS:MA_AT_END;
}
ma_result pcm_seek(ma_data_source* source,ma_uint64 frame){
    auto& s=*static_cast<PcmStream*>(source);if(frame>s.frames||!s.reader)return MA_INVALID_ARGS;
    // The loop back from the end is already prefetched; any other jump restarts the reader.
    const bool in_order=frame==s.cursor||(s.cursor==s.frames&&frame==s.loop_start);
    if(!in_order)s.reader->start(s.offset,s.frames,s.loop_start,s.channels,frame);
    s.cursor=frame;return MA_SUCCESS;
}
ma_result pcm_format(ma_data_source* source,ma_format* format,ma_uint32* channels,ma_uint32* rate,ma_channel* map,size_t cap){
    auto& s=*static_cast<PcmStream*>(source);if(format)*format=ma_format_f32;if(channels)*channels=s.channels;if(rate)*rate=s.rate;
    if(map)ma_channel_map_init_standard(ma_standard_channel_map_default,map,cap,s.channels);return MA_SUCCESS;
}
ma_result pcm_cursor(ma_data_source* source,ma_uint64* cursor){*cursor=static_cast<PcmStream*>(source)->cursor;return MA_SUCCESS;}
ma_result pcm_length(ma_data_source* source,ma_uint64* length){*length=static_cast<PcmStream*>(source)->frames;return MA_SUCCESS;}
const ma_data_source_vtable pcm_vtable{pcm_read,pcm_seek,pcm_format,pcm_cursor,pcm_length,nullptr,0};
}
