// Real Windows/DirectSound integration test. Generated PCM samples are silent;
// no original executable is required, loaded, or invoked by this target.
#include "music_stream.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <chrono>
#include <cstring>

namespace a=th20::source::audio;
namespace rt=th20::source::runtime;
namespace p=th20::source::platform;
void require(bool condition,const char* message) {if(!condition) throw std::runtime_error(message);}
int wmain(int argc,wchar_t** argv) {
    if(argc!=2) return 2;
    try {
        const std::filesystem::path output=argv[1];std::filesystem::create_directories(output);
        const auto pcm_path=output/"silent_music.pcm";
        std::vector<std::uint8_t> pcm(44100*4,0);
        {std::ofstream file(pcm_path,std::ios::binary);file.write(reinterpret_cast<const char*>(pcm.data()),pcm.size());}
        std::vector<std::uint8_t> wav(44+4096,0);
        const auto put=[&](unsigned at,std::uint32_t value){std::memcpy(wav.data()+at,&value,4);};
        std::memcpy(wav.data(),"RIFF",4);put(4,static_cast<std::uint32_t>(wav.size()-8));std::memcpy(wav.data()+8,"WAVEfmt ",8);put(16,16);
        WAVEFORMATEX format{};format.wFormatTag=WAVE_FORMAT_PCM;format.nChannels=2;format.nSamplesPerSec=44100;
        format.nAvgBytesPerSec=176400;format.nBlockAlign=4;format.wBitsPerSample=16;
        std::memcpy(wav.data()+20,&format,16);std::memcpy(wav.data()+36,"data",4);put(40,4096);
        a::TrackFormat records[2]{};strcpy_s(records[0].name,"silent.wav");records[0].preload_bytes=records[0].total_bytes=static_cast<std::uint32_t>(pcm.size());
        records[0].loop_start=44100*2;records[0].format=format;
        std::vector<std::uint8_t> metadata(sizeof records);std::memcpy(metadata.data(),records,sizeof records);
        p::Configuration config{};config.value_75=config.value_76=1;config.value_7e=100;config.value_7f=80;
        rt::Log log;const auto origin=std::chrono::steady_clock::now();
        a::Context services{config,log,[&](const char* name)->std::optional<std::vector<std::uint8_t>> {
            if(std::strcmp(name,"thbgm.fmt")==0) return metadata;
            return wav;
        },[&] {return std::chrono::duration<double>(std::chrono::steady_clock::now()-origin).count();},nullptr};
        WNDCLASSW wc{};wc.lpfnWndProc=DefWindowProcW;wc.hInstance=GetModuleHandleW(nullptr);wc.lpszClassName=L"TH20SourceAudioSmoke";
        const auto atom=RegisterClassW(&wc);require(atom!=0,"Register hidden audio test window");
        const auto window=CreateWindowExW(0,wc.lpszClassName,L"",WS_OVERLAPPED,0,0,32,32,nullptr,nullptr,wc.hInstance,nullptr);
        require(window!=nullptr,"Create hidden audio test window");services.graphics_window=window;
        auto sound=std::make_unique<a::SoundInf>();sound->context=&services;
        require(sound->load_formats("thbgm.fmt")==0,"Load source music format bytes");
        sound->initialize(window,services);require(sound->device_owner && sound->direct_sound && sound->silent_buffer,"Initialize actual DirectSound");
        for(const auto& effect:sound->effects) require(effect.buffer!=nullptr,"Create all 90 actual effect buffers");
        require(sound->apply_configuration()==0 && sound->effect_level==80,"Apply actual sound configuration");
        sound->request_effect(0,0);sound->request_effect(0,400);sound->poll();
        DWORD status=0;sound->effects[0].buffer->GetStatus(&status);require(status&DSBSTATUS_PLAYING,"Play generated silent sound effect");
        const auto path=pcm_path.string();require(sound->start_stream(path.c_str())==0 && sound->stream,"Create file streaming buffer and thread");
        require(SUCCEEDED(sound->stream->play(0,DSBPLAY_LOOPING,0)),"Play actual streaming buffer");
        Sleep(320);require(SUCCEEDED(sound->stream->pause()),"Pause stream and close file");
        require(SUCCEEDED(sound->stream->resume()),"Resume stream and reopen file");
        require(SUCCEEDED(sound->stream->seek_seconds(0.125)),"Recreate DirectSound buffers and seek");
        sound->stop_stream();require(!sound->stream && !sound->notify_thread,"Join streaming notification thread");
        config.flags|=16;sound->preload(0,"silent.wav");require(sound->preloaded[0].allocation!=nullptr,"Preload actual PCM file");
        require(sound->load_track(0)==0 && sound->stream,"Create memory streaming buffer");
        require(SUCCEEDED(sound->stream->play(0,DSBPLAY_LOOPING,0)),"Play memory stream");Sleep(50);
        sound->stop_stream();require(sound->shutdown()==0,"Shutdown buffers and device");
        require(!sound->device_owner && !sound->silent_buffer && !sound->track_formats && !sound->preloaded[0].allocation,"Release all owned sound resources");
        DestroyWindow(window);UnregisterClassW(wc.lpszClassName,wc.hInstance);
        std::ofstream report(output/"backend_validation.json");report<<"{\"status\":\"passed\",\"backend\":\"real Win32 DirectSound8\",\"effects_created\":90,\"file_stream\":true,\"memory_stream\":true,\"pause_resume_seek\":true,\"thread_join\":true,\"samples\":\"generated silent PCM\",\"original_exe_used\":false}\n";
        std::cout<<"DirectSound8 silent backend integration passed\n";return 0;
    } catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
}
