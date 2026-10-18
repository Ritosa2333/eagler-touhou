#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_THREADING
#define MINIAUDIO_IMPLEMENTATION
#include "../../../portable/sdl/third_party/miniaudio.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

struct Mixer {
    ma_engine engine{};ma_audio_buffer source{};ma_sound sound{};
    Mixer(const std::vector<float>& pcm,ma_uint32 rate,ma_uint32 channels,bool noPitch){
        auto config=ma_engine_config_init();config.noDevice=MA_TRUE;config.channels=2;config.sampleRate=44100;config.defaultVolumeSmoothTimeInPCMFrames=0;
        assert(ma_engine_init(&config,&engine)==MA_SUCCESS);
        auto input=ma_audio_buffer_config_init(ma_format_f32,channels,pcm.size()/channels,pcm.data(),nullptr);input.sampleRate=rate;
        assert(ma_audio_buffer_init(&input,&source)==MA_SUCCESS);
        assert(ma_sound_init_from_data_source(&engine,&source,MA_SOUND_FLAG_NO_SPATIALIZATION|(noPitch?MA_SOUND_FLAG_NO_PITCH:0),nullptr,&sound)==MA_SUCCESS);
        ma_data_source_set_loop_point_in_pcm_frames(&source,512,4096);ma_sound_set_looping(&sound,MA_TRUE);ma_sound_start(&sound);
    }
    ~Mixer(){ma_sound_uninit(&sound);ma_audio_buffer_uninit(&source);ma_engine_uninit(&engine);}
    void command(unsigned step){
        if(step==4){ma_sound_set_pan_mode(&sound,ma_pan_mode_balance);ma_sound_set_pan(&sound,.6f);}
        if(step==8)ma_sound_set_volume(&sound,.4f);
        if(step==12)assert(ma_sound_seek_to_pcm_frame(&sound,1000)==MA_SUCCESS);
        if(step==16)ma_sound_stop(&sound);
        if(step==18)ma_sound_start(&sound);
        if(step==24){ma_sound_set_volume(&sound,.75f);ma_sound_set_pan(&sound,-.25f);}
        if(step==28)assert(ma_sound_seek_to_pcm_frame(&sound,0)==MA_SUCCESS);
    }
    void read(std::vector<float>& pcm){ma_uint64 count=0;assert(ma_engine_read_pcm_frames(&engine,pcm.data(),pcm.size()/2,&count)==MA_SUCCESS);assert(count==pcm.size()/2);}
};
int main(){
    for(auto rate:{44100u,48000u,22050u})for(auto channels:{1u,2u}){
        std::vector<float> input(4096*channels);
        for(unsigned f=0;f<4096;++f)for(unsigned c=0;c<channels;++c)input[f*channels+c]=.25f*float(std::sin(double(f)*(c?.077:.063)))+.0625f*float(int(f%17)-8)/8.f;
        Mixer original(input,rate,channels,false),candidate(input,rate,channels,true);
        std::size_t differing=0,total=0;double error=0,energy=0;float maxError=0;ma_uint64 maxCursorDelta=0;
        for(unsigned step=0;step<40;++step){
            original.command(step);candidate.command(step);
            const unsigned frames=step%3==0?127:step%3==1?513:1024;
            std::vector<float> a(frames*2),b(frames*2);original.read(a);candidate.read(b);
            // At unity rate the optimized mixer must deliver the unfiltered
            // source PCM. The old pitch filter has a transient/phase response,
            // so old/new unity-rate PCM is intentionally not a bitwise oracle.
            if(step==0&&rate==44100)for(unsigned f=0;f<frames;++f)for(unsigned c=0;c<2;++c)assert(b[f*2+c]==input[f*channels+std::min(c,channels-1)]);
            for(std::size_t i=0;i<a.size();++i){assert(std::isfinite(b[i]));differing+=a[i]!=b[i];const float d=a[i]-b[i];error+=double(d)*d;energy+=double(a[i])*a[i];maxError=std::max(maxError,std::fabs(d));if(step==16||step==17)assert(a[i]==0&&b[i]==0);}
            ma_uint64 x=0,y=0;assert(ma_sound_get_cursor_in_pcm_frames(&original.sound,&x)==MA_SUCCESS);assert(ma_sound_get_cursor_in_pcm_frames(&candidate.sound,&y)==MA_SUCCESS);maxCursorDelta=std::max(maxCursorDelta,x>y?x-y:y-x);total+=a.size();
        }
        std::printf("audio rate=%u channels=%u differing=%zu/%zu max_error=%.9g relative_rms=%.9g cursor_delta=%llu\n",rate,channels,differing,total,double(maxError),std::sqrt(error/std::max(energy,1e-30)),static_cast<unsigned long long>(maxCursorDelta));
        assert(maxCursorDelta==0);
        if(rate!=44100)assert(differing==0);
    }
}
