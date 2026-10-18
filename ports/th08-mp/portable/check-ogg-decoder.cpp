// Exercise the same file decoder and loop API as TH08 AudioHost in Wasm.
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
#include <cstring>
extern "C" __attribute__((export_name("check"))) int check(unsigned frames,unsigned loop){
    ma_decoder d{};auto c=ma_decoder_config_init(ma_format_s16,2,44100);
    if(ma_decoder_init_file("/track.ogg",&c,&d)!=MA_SUCCESS)return 1;
    struct Close{ma_decoder* d;~Close(){ma_decoder_uninit(d);}}close{&d};
    ma_uint64 length=0,n=0;if(ma_decoder_get_length_in_pcm_frames(&d,&length)!=MA_SUCCESS||length!=frames)return 2;
    short expected[2048]{},actual[2048]{},block[8192]{};ma_uint64 total=0;bool sound=false;
    while(total<length){if(ma_decoder_read_pcm_frames(&d,block,4096,&n)!=MA_SUCCESS&&n==0)return 3;total+=n;for(unsigned i=0;i<n*2;i++)sound|=block[i]!=0;}
    if(total!=length||!sound)return 4;
    if(ma_decoder_seek_to_pcm_frame(&d,length-512)!=MA_SUCCESS)return 5;
    if(ma_decoder_read_pcm_frames(&d,expected,512,&n)!=MA_SUCCESS||n!=512)return 6;
    if(ma_decoder_seek_to_pcm_frame(&d,loop)!=MA_SUCCESS)return 7;
    if(ma_decoder_read_pcm_frames(&d,expected+1024,512,&n)!=MA_SUCCESS||n!=512)return 8;
    ma_data_source_set_loop_point_in_pcm_frames(&d,loop,length);
    ma_data_source_set_looping(&d,MA_TRUE);
    if(ma_decoder_seek_to_pcm_frame(&d,length-512)!=MA_SUCCESS)return 9;
    if(ma_data_source_read_pcm_frames(&d,actual,1024,&n)!=MA_SUCCESS||n!=1024)return 10;
    if(std::memcmp(actual,expected,sizeof(actual)))return 11;
    return 0;
}
