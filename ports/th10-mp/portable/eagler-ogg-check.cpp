// Check the exact Vorbis/miniaudio source contract used by AudioHost.
// The fixture is private and is supplied by the runner; it is never copied
// into the repository.
#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_WAV
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_THREADING
#include "sdl/third_party/stb_vorbis.h"
#define MINIAUDIO_IMPLEMENTATION
#include "sdl/third_party/miniaudio.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

namespace {
using Samples=std::vector<int16_t>;
bool decode(const char* path,ma_uint64 expected,Samples& output){
    ma_decoder decoder{};auto config=ma_decoder_config_init(ma_format_s16,2,44100);
    if(ma_decoder_init_file(path,&config,&decoder)!=MA_SUCCESS)return false;
    struct Guard{ma_decoder& value;~Guard(){ma_decoder_uninit(&value);}} guard{decoder};
    ma_uint64 length=0;if(ma_decoder_get_length_in_pcm_frames(&decoder,&length)!=MA_SUCCESS||length!=expected)return false;
    output.resize(size_t(length)*2);ma_uint64 done=0;
    while(done<length){ma_uint64 actual=0;const auto result=ma_decoder_read_pcm_frames(&decoder,output.data()+size_t(done)*2,std::min<ma_uint64>(4096,length-done),&actual);if((result!=MA_SUCCESS&&result!=MA_AT_END)||!actual)return false;done+=actual;}
    return done==length;
}
bool read_at(const char* path,ma_uint64 frame,ma_uint64 count,const Samples& expected){
    ma_decoder decoder{};auto config=ma_decoder_config_init(ma_format_s16,2,44100);
    if(ma_decoder_init_file(path,&config,&decoder)!=MA_SUCCESS)return false;
    struct Guard{ma_decoder& value;~Guard(){ma_decoder_uninit(&value);}} guard{decoder};
    if(ma_decoder_seek_to_pcm_frame(&decoder,frame)!=MA_SUCCESS)return false;Samples actual(size_t(count)*2);ma_uint64 done=0;
    while(done<count){ma_uint64 n=0;if(ma_decoder_read_pcm_frames(&decoder,actual.data()+size_t(done)*2,count-done,&n)!=MA_SUCCESS||!n)return false;done+=n;}
    return std::memcmp(actual.data(),expected.data()+size_t(frame)*2,actual.size()*sizeof(int16_t))==0;
}
int verify(const char* path,unsigned frames,unsigned loop){
    if(!frames||loop>=frames)return 1;Samples stream,full;
    if(!decode(path,frames,stream)||!decode(path,frames,full))return 2;
    if(stream!=full)return 3;
    const auto tail=std::min<unsigned>(512,frames);if(!read_at(path,frames-tail,tail,stream)||!read_at(path,loop,tail,stream))return 4;
    ma_audio_buffer buffer{};auto buffer_config=ma_audio_buffer_config_init(ma_format_s16,2,frames,full.data(),nullptr);buffer_config.sampleRate=44100;
    if(ma_audio_buffer_init(&buffer_config,&buffer)!=MA_SUCCESS)return 5;
    struct BufferGuard{ma_audio_buffer& value;~BufferGuard(){ma_audio_buffer_uninit(&value);}} buffer_guard{buffer};
    if(ma_data_source_set_loop_point_in_pcm_frames(&buffer,loop,frames)!=MA_SUCCESS||ma_data_source_set_looping(&buffer,MA_TRUE)!=MA_SUCCESS)return 6;
    if(ma_audio_buffer_seek_to_pcm_frame(&buffer,frames-tail)!=MA_SUCCESS)return 7;
    Samples actual(size_t(tail+tail)*2);ma_uint64 read=0;if(ma_data_source_read_pcm_frames(&buffer,actual.data(),tail+tail,&read)!=MA_SUCCESS||read!=tail+tail)return 8;
    for(unsigned i=0;i<tail+tail;++i){const auto source=i<tail?frames-tail+i:loop+i-tail;if(std::memcmp(actual.data()+size_t(i)*2,stream.data()+size_t(source)*2,2*sizeof(int16_t)))return 9;}
    return 0;
}
}
extern "C" __attribute__((export_name("check"))) int check(unsigned frames,unsigned loop){return verify("/bgm-ogg/track.ogg",frames,loop);}
extern "C" __attribute__((export_name("check_late_missing"))) int check_late_missing(){ma_decoder decoder{};auto config=ma_decoder_config_init(ma_format_s16,2,44100);return ma_decoder_init_file("/bgm-ogg/late.ogg",&config,&decoder)==MA_SUCCESS?1:0;}
extern "C" __attribute__((export_name("check_late"))) int check_late(unsigned frames,unsigned loop){return verify("/bgm-ogg/late.ogg",frames,loop);}
