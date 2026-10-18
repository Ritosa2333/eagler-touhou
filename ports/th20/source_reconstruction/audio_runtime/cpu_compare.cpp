// This is an isolated test executable. Production libraries neither map nor
// call the original image. Original instructions are not patched in this test.
#define wmain unused_original_oracle_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "test_buffer.hpp"
#include <memory>
#include "audio_source_hashes.hpp"

namespace audio=th20::source::audio;
namespace platform=th20::source::platform;
namespace runtime=th20::source::runtime;
namespace {
template<class R,class... A> R original(std::uint32_t rva,void* self,A... arguments) {
    return reinterpret_cast<R(__thiscall*)(void*,A...)>(mapped_image_base+rva)(self,arguments...);
}
LONG WINAPI diagnostic(EXCEPTION_POINTERS* error) {
    std::fprintf(stderr,"Audio oracle exception %08lx at %08lx; original base %08x\n",error->ExceptionRecord->ExceptionCode,error->ContextRecord->Eip,mapped_image_base);
    return EXCEPTION_CONTINUE_SEARCH;
}
void WINAPI test_sleep(DWORD) {} // OS wait duration is outside these state-only checks.
DWORD WINAPI test_time() {return 10000;}
}
int wmain(int argc,wchar_t** argv) {
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);AddVectoredExceptionHandler(1,diagnostic);
    try {
        if(argc!=3) throw std::runtime_error("Usage: th20_audio_cpu_compare VERIFIED_TH20.exe REPORT.json");
        const std::filesystem::path input(argv[1]),report(argv[2]);
        if(std::filesystem::weakly_canonical(input)==std::filesystem::weakly_canonical(report)) throw std::runtime_error("Report cannot replace original EXE");
        const auto bytes=th20::read_file(input);if(sha256(bytes)!=expected_sha) throw std::runtime_error("Original hash mismatch");
        const auto pe=th20::parse_pe(bytes);Mapping mapping(bytes,pe);mapped_image_base=mapping.address();
        for(const auto& item:pe.imports) {
            std::uintptr_t address=0;
            if(item.name=="Sleep") address=reinterpret_cast<std::uintptr_t>(test_sleep);
            else if(item.name=="timeGetTime") address=reinterpret_cast<std::uintptr_t>(test_time);
            else if(item.name=="GetCurrentThreadId" || item.name=="AcquireSRWLockExclusive" || item.name=="ReleaseSRWLockExclusive"
                || item.name=="HeapAlloc" || item.name=="HeapFree" || item.name=="GetLastError" || item.name=="SetFilePointer"
                || item.name=="ReadFile" || item.name=="CloseHandle" || item.name=="CreateFileW" || item.name=="MultiByteToWideChar")
                address=reinterpret_cast<std::uintptr_t>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"),item.name.c_str()));
            if(address) *reinterpret_cast<std::uintptr_t*>(mapped_image_base+item.iat_rva)=address;
        }
        *reinterpret_cast<HANDLE*>(mapped_image_base+0x1e5990)=GetProcessHeap();
        original<void>(0x52e00,reinterpret_cast<void*>(mapped_image_base+0x1c0240));
        *reinterpret_cast<std::uint64_t*>(mapped_image_base+0x1b67d0)=0;
        *reinterpret_cast<double*>(mapped_image_base+0x1b8838)=0;
        runtime::MemoryResource resource;auto* previous=runtime::set_default_resource(&resource);
        *reinterpret_cast<std::uintptr_t*>(mapped_image_base+0x1e4d28)=reinterpret_cast<std::uintptr_t>(&resource);
        std::map<std::string,unsigned> counts;std::vector<std::string> failures;unsigned failed=0;
        auto check=[&](const char* label,bool value) {++counts[label];if(!value) {++failed;if(failures.size()<30) failures.push_back(std::string(label)+" #"+std::to_string(counts[label]));}};
        std::mt19937 rng(0x20a04268);auto fill=[&](void* p,std::size_t n) {auto* b=static_cast<std::uint8_t*>(p);while(n--) *b++=static_cast<std::uint8_t>(rng());};
        check("effect_definitions_90",std::memcmp(audio::effect_definitions,reinterpret_cast<void*>(mapped_image_base+0x1ae6e8),sizeof audio::effect_definitions)==0);
        for(unsigned n=0;n<72;++n) {
            const auto p=*reinterpret_cast<const char**>(mapped_image_base+0x1aedf0+n*4);
            check("effect_filenames_72",std::strcmp(audio::effect_filenames[n],p)==0);
        }
        platform::Configuration config{};runtime::Log log;
        audio::Context services{config,log,{},[]{return 10.0;},nullptr};
        auto sound=std::make_unique<audio::SoundInf>();sound->context=&services;
        auto raw=std::make_unique<std::array<std::uint8_t,0x57e8>>();
        original<void>(0x25d70,raw->data());
        for(unsigned i=0;i<90;++i) {
            const auto actual=*reinterpret_cast<std::uintptr_t*>(raw->data()+0x1994+i*24+8);
            const auto wanted=reinterpret_cast<std::uintptr_t>(sound->effects[i].definition);
            const auto index=(actual-mapped_image_base-0x1ae6e8)/20;
            check("effect_constructor_definition_binding",index<90 && audio::effect_definitions[index].id==static_cast<int>(i));
            std::memcpy(raw->data()+0x1994+i*24+8,&wanted,4);
        }
        check("sound_constructor_all_original_fields",std::memcmp(raw->data(),sound.get(),0x57e8)==0);
        const auto synchronize=[&] {std::memcpy(raw->data(),sound.get(),0x57e8);};
        TestBuffer buffer;
        std::array<std::uint8_t,8192> samples{};fill(samples.data(),samples.size());
        audio::TrackFormat format{};strcpy_s(format.name,"track01.wav");format.total_bytes=4096;format.loop_start=256;
        format.format.wFormatTag=WAVE_FORMAT_PCM;format.format.nChannels=2;format.format.nSamplesPerSec=44100;format.format.wBitsPerSample=16;format.format.nBlockAlign=4;
        auto* reader=new audio::WaveReader;reader->open_memory(samples.data(),samples.size(),&format,0);
        audio::MusicStream stream(*sound,buffer.as_buffer(),1024,reader,256);
        std::array<std::uint8_t,0xa8> stream_raw{};
        for(unsigned n=0;n<2000;++n) {
            audio::EffectRequest a,b;fill(&a,sizeof a);b=a;original<void>(0x25ce0,&a);audio::initialize(b);
            check("request_constructor",std::memcmp(&a,&b,sizeof a)==0);
            audio::Command ca,cb;fill(&ca,sizeof ca);cb=ca;original<void>(0x25fc0,&ca);audio::initialize(cb);
            check("command_constructor",std::memcmp(&ca,&cb,sizeof ca)==0);
            audio::EffectChannel ea,eb;fill(&ea,sizeof ea);eb=ea;original<void>(0x25d20,&ea);audio::initialize(eb);
            check("channel_constructor",std::memcmp(&ea,&eb,sizeof ea)==0);
            for(auto& command:sound->commands) audio::initialize(command);
            const unsigned used=n%32;for(unsigned i=0;i<used;++i) {sound->commands[i].type=1+static_cast<int>(rng()%9);sound->commands[i].stage=static_cast<int>(rng());}
            synchronize();const int type=static_cast<int>(rng()%10),argument=static_cast<int>(rng());
            const std::string name(n%255,static_cast<char>('a'+n%26));
            original<void>(0x28c90,raw->data(),type,argument,name.c_str());sound->enqueue(type,argument,name.c_str());
            check("enqueue_31_slots_and_sentinel",std::memcmp(raw->data(),sound.get(),0x57e8)==0);
            for(auto& request:sound->requests) {fill(&request,sizeof request);request.id=-1;}
            const unsigned pending=n%13;const int id=static_cast<int>(rng()%90),pan=static_cast<int>(rng());
            for(unsigned i=0;i<pending;++i) {sound->requests[i].id=static_cast<int>(rng()%90);sound->requests[i].count=static_cast<int>(rng()%65)-3;}
            if(pending && n%3==0) sound->requests[pending-1].id=id;
            synchronize();original<void>(0x26d70,raw->data(),id,pan);sound->request_effect(id,pan);
            check("request_merge_full_negative_count",std::memcmp(raw->data(),sound.get(),0x57e8)==0);
            const int stop_id=n%5?static_cast<int>(rng()%90):-1;
            synchronize();buffer.calls.clear();original<void>(0x28890,raw->data(),stop_id);const auto stop_calls=buffer.calls;
            buffer.calls.clear();sound->stop_effects(stop_id);
            check("stop_request_merge_or_stop_all",stop_calls==buffer.calls && std::memcmp(raw->data(),sound.get(),0x57e8)==0);
            sound->music_level=static_cast<int>(rng()%101);sound->effect_level=static_cast<int>(rng()%101);
            *reinterpret_cast<int*>(mapped_image_base+0x1c000c)=sound->music_level;*reinterpret_cast<int*>(mapped_image_base+0x1c0010)=sound->effect_level;
            ea=sound->effects[id];ea.buffer=buffer.as_buffer();ea.pan=pan;eb=ea;
            buffer.calls.clear();original<void>(0x26ef0,&ea,pan);const auto expected=buffer.calls;buffer.calls.clear();audio::play_effect(eb,pan,sound->effect_level);
            check("effect_play_volume_and_call_order",ea.pan==eb.pan && expected==buffer.calls);
            const int attenuation=static_cast<int>(rng()%12000)-10000;
            std::memcpy(stream_raw.data(),&stream,0xa8);
            buffer.calls.clear();original<void>(0x5b9b0,stream_raw.data(),attenuation);const auto music_calls=buffer.calls;
            buffer.calls.clear();stream.set_volume(attenuation);check("music_volume",music_calls==buffer.calls);
            stream.fade_mode=static_cast<int>(rng()%6);stream.fade_remaining=static_cast<int>(rng()%200)-3;stream.fade_duration=1+static_cast<int>(rng()%180);
            std::memcpy(stream_raw.data(),&stream,0xa8);const unsigned mode=1+n%4;
            const std::uint32_t addresses[]={0,0x5a1a0,0x5a0c0,0x5a050,0x5a130};
            buffer.calls.clear();const auto result=original<int>(addresses[mode],stream_raw.data());const auto fade_calls=buffer.calls;
            buffer.calls.clear();const auto recovered_result=stream.tick_fade(mode);
            check("four_fade_modes_and_returns",result==recovered_result && std::memcmp(stream_raw.data(),&stream,0xa8)==0 && fade_calls==buffer.calls);
            const std::uint32_t position_bits=rng();float position;std::memcpy(&position,&position_bits,4);
            sound->effects[id].buffer=buffer.as_buffer();synchronize();buffer.calls.clear();original<void>(0x29090,raw->data(),id,position_bits);
            const auto pan_calls=buffer.calls;buffer.calls.clear();sound->set_effect_pan(id,position);
            check("pan_float_bits_including_nan",pan_calls==buffer.calls);
            sound->effects[id].buffer=nullptr;
            audio::WaveReader wa,wb;wa.open_memory(samples.data(),samples.size(),&format,n%3);wb=wa;
            const auto offset=rng()%4000;wa.memory_current=wb.memory_current=samples.data()+offset;
            std::array<std::uint8_t,1024> outputa{},outputb{};std::uint32_t receiveda=0xdeadbeef,receivedb=receiveda;const auto requested=rng()%1024;
            const auto read_a=original<HRESULT>(0x5aff0,&wa,outputa.data(),requested,&receiveda);const auto read_b=wb.read(outputb.data(),requested,&receivedb);
            check("memory_read_state_bytes_and_result",read_a==read_b && receiveda==receivedb && outputa==outputb && std::memcmp(&wa,&wb,sizeof wa)==0);
            const bool loop=n%2;const auto reset_a=original<HRESULT>(0x5b5f0,&wa,static_cast<std::uint32_t>(loop),rng());const auto reset_b=wb.reset(loop,0,0);
            check("memory_reset_ignores_position",reset_a==reset_b && std::memcmp(&wa,&wb,sizeof wa)==0);
        }
        sound->device_owner=reinterpret_cast<audio::DeviceOwner*>(1);sound->direct_sound=reinterpret_cast<IDirectSound8*>(1);
        for(unsigned n=0;n<1000;++n) {
            config.flags=rng();config.value_7e=static_cast<std::uint8_t>(rng());config.value_7f=static_cast<std::uint8_t>(rng());
            std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1c4f08),&config,sizeof config);
            synchronize();const auto ra=original<int>(0x28560,raw->data()),rb=sound->apply_configuration();
            check("configuration_volumes_and_requests",ra==rb && std::memcmp(raw->data(),sound.get(),0x57e8)==0);
            config.value_76=n%2;config.value_75=1;std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1c4f08),&config,sizeof config);
            for(auto& command:sound->commands) audio::initialize(command);
            for(auto& request:sound->requests) {audio::initialize(request);request.id=-1;}
            const unsigned count=n%12;for(unsigned i=0;i<count;++i) {auto& q=sound->requests[i];q.id=i;q.count=static_cast<int>(rng()%6)-1;for(int j=0;j<q.count;++j) q.pans[j]=static_cast<int>(rng());sound->effects[i].buffer=buffer.as_buffer();}
            sound->commands[0].type=static_cast<int>(n%10);sound->stream=nullptr;
            if(sound->commands[0].type==1 || sound->commands[0].type==2) sound->commands[0].type=0; // stream loading tested separately
            synchronize();*reinterpret_cast<int*>(mapped_image_base+0x1c0010)=sound->effect_level;
            buffer.calls.clear();const auto expected_return=original<int>(0x277f0,raw->data());const auto expected_calls=buffer.calls;
            buffer.calls.clear();const auto actual_return=sound->poll();
            check("command_null_stream_and_effect_dispatch",expected_return==actual_return && expected_calls==buffer.calls && std::memcmp(raw->data(),sound.get(),0x57e8)==0);
            for(auto& effect:sound->effects) effect.buffer=nullptr;
        }
        sound->device_owner=nullptr;sound->direct_sound=nullptr;
        std::array<audio::TrackFormat,4> tracks{};strcpy_s(tracks[0].name,"a.wav");strcpy_s(tracks[1].name,"b.wav");strcpy_s(tracks[2].name,"c.wav");sound->track_formats=tracks.data();synchronize();
        for(const char* name:{"a.wav","b.wav","c.wav","d.wav","d/a/b.wav","d\\a\\c.wav","d/a\\b.wav"})
            check("track_name_lookup_and_fallback",original<unsigned>(0x28420,raw->data(),name)==sound->find_track(name));
        sound->track_formats=nullptr;
        for(unsigned n=0;n<256;++n) {
            *reinterpret_cast<int*>(mapped_image_base+0x1c000c)=sound->music_level;
            stream.playing=n%2;stream.paused=(n/2)%2;stream.paused_at=1.25;stream.paused_duration=2.5;
            stream.started_at=2.0;stream.fade_mode=static_cast<int>(n%5);stream.fade_remaining=17;stream.fade_duration=30;
            reader->file_mode=0;reader->file=nullptr;reader->memory_mode=1;reader->saved_position=rng();
            const auto wave_before=*reader;std::memcpy(stream_raw.data(),&stream,0xa8);
            buffer.status=0;buffer.calls.clear();HRESULT a=0,b=0;
            const auto action=n%3;
            if(action==0) a=original<HRESULT>(0x5add0,stream_raw.data(),0u,1u,0u);
            if(action==1) a=original<HRESULT>(0x5acf0,stream_raw.data());
            if(action==2) a=original<HRESULT>(0x5bbe0,stream_raw.data());
            const auto after=*reader;const auto calls=buffer.calls;*reader=wave_before;buffer.calls.clear();
            if(action==0) b=stream.play(0,1,0);
            if(action==1) b=stream.pause();
            if(action==2) b=stream.resume();
            check("play_pause_resume_clock_and_state",a==b && calls==buffer.calls && std::memcmp(&after,reader,sizeof *reader)==0 && std::memcmp(stream_raw.data(),&stream,0xa8)==0);
            format.total_bytes=2*44100*4;format.loop_start=44100*4;
            stream.started_at=10.0-static_cast<double>(n)/8;stream.paused_duration=0;
            std::memcpy(stream_raw.data(),&stream,0xa8);
            const auto original_time=original<double>(0x5bd10,stream_raw.data());const auto source_time=stream.playback_seconds();
            check("playback_clock_and_loop_seconds",std::memcmp(&original_time,&source_time,sizeof source_time)==0);
        }
        format.total_bytes=4096;format.loop_start=256;
        for(unsigned n=0;n<512;++n) {
            for(auto& command:sound->commands) audio::initialize(command);
            for(auto& request:sound->requests) {audio::initialize(request);request.id=-1;}
            config.flags=n%2?16:0;config.value_75=1;config.value_76=0;
            sound->stream=&stream;sound->device_owner=reinterpret_cast<audio::DeviceOwner*>(1);
            sound->direct_sound=reinterpret_cast<IDirectSound8*>(1);sound->track_formats=&format;
            stream.playing=n%2;stream.paused=n%3==0;stream.busy=n%4==0;
            stream.fade_mode=2;stream.fade_remaining=50;stream.fade_duration=100;
            reader->open_memory(samples.data(),samples.size(),&format,0);reader->file_mode=0;
            auto& command=sound->commands[0];command.type=2+static_cast<int>(n%7);command.stage=static_cast<int>((n/7)%22);
            command.argument=-1;strcpy_s(command.name,"track01.wav");
            // Stage 1 recreates COM objects; stage 3 of shutdown destroys the
            // stream. Those lifecycle operations have separate backend tests.
            if(command.type==2 && command.stage==1) stream.busy=1;
            if(command.type==4 && (command.stage==2 || command.stage==3)) command.stage=4;
            if(command.type==2 && config.flags && n%4) command.argument=0;
            if(command.type==2 && command.argument==0 && command.stage==0) command.stage=1;
            const auto before=*reader;std::memcpy(stream_raw.data(),&stream,0xa8);
            const auto source_vptr=*reinterpret_cast<std::uintptr_t*>(&stream);
            *reinterpret_cast<std::uintptr_t*>(stream_raw.data())=mapped_image_base+0x16e748;
            synchronize();*reinterpret_cast<void**>(raw->data()+0x57c4)=stream_raw.data();
            *reinterpret_cast<void**>(mapped_image_base+0x1bfff4)=stream_raw.data();
            *reinterpret_cast<int*>(mapped_image_base+0x1c000c)=sound->music_level;
            std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1c4f08),&config,sizeof config);
            buffer.status=0;buffer.calls.clear();const auto a=original<int>(0x277f0,raw->data());const auto after=*reader;const auto calls=buffer.calls;const auto contents=buffer.bytes;
            *reader=before;buffer.calls.clear();const auto b=sound->poll();
            *reinterpret_cast<void**>(raw->data()+0x57c4)=&stream;*reinterpret_cast<std::uintptr_t*>(stream_raw.data())=source_vptr;
            check("active_stream_command_stages",a==b && calls==buffer.calls && contents==buffer.bytes && std::memcmp(&after,reader,sizeof *reader)==0
                && std::memcmp(raw->data(),sound.get(),0x57e8)==0 && std::memcmp(stream_raw.data(),&stream,0xa8)==0);
        }
        sound->stream=nullptr;sound->device_owner=nullptr;sound->direct_sound=nullptr;sound->track_formats=nullptr;
        for(unsigned n=0;n<1000;++n) {
            const auto reset_wave=[&] {
                reader->open_memory(samples.data(),samples.size(),&format,0);
                format.total_bytes=128+rng()%3072;format.loop_start=rng()%(format.total_bytes-1);
                format.format.wBitsPerSample=n%2?8:16;
                reader->memory_size=format.total_bytes;reader->memory_current=samples.data()+rng()%format.total_bytes;
            };
            reset_wave();const auto before_wave=*reader;const bool loop=n%2;
            const auto position=rng()%4096;std::memcpy(stream_raw.data(),&stream,0xa8);
            fill(buffer.bytes.data(),buffer.bytes.size());const auto before_bytes=buffer.bytes;
            buffer.status=0;buffer.method_result=S_OK;buffer.calls.clear();
            const auto ra=original<HRESULT>(0x5a240,stream_raw.data(),buffer.as_buffer(),static_cast<int>(loop),position);
            const auto expected_wave=*reader;const auto expected_bytes=buffer.bytes;const auto expected_calls=buffer.calls;
            *reader=before_wave;buffer.bytes=before_bytes;buffer.calls.clear();const auto rb=stream.fill(buffer.as_buffer(),loop,position);
            check("fill_buffer_bytes_loop_and_silence",ra==rb && expected_bytes==buffer.bytes && expected_calls==buffer.calls && std::memcmp(&expected_wave,reader,sizeof *reader)==0);
            reset_wave();const auto prior=*reader;
            stream.silence=n%3==0;stream.next_write=(n%4)*256;stream.last_play_cursor=rng()%1024;stream.played_bytes=rng();
            buffer.play_cursor=rng()%1024;buffer.write_cursor=rng()%1024;buffer.status=n%5==0?DSBSTATUS_BUFFERLOST:0;
            std::memcpy(stream_raw.data(),&stream,0xa8);const auto status=buffer.status;
            buffer.calls.clear();original<void>(0x5a580,stream_raw.data(),static_cast<int>(loop));
            const auto next_wave=*reader;const auto next_bytes=buffer.bytes;const auto next_calls=buffer.calls;
            *reader=prior;buffer.status=status;buffer.bytes=before_bytes;buffer.calls.clear();stream.handle_notification(loop);
            // Compare only the notification block for skip paths; other bytes
            // belong to earlier fills and are preserved independently.
            const bool wrote=std::any_of(next_calls.begin(),next_calls.end(),[](const BufferCall& c){return c.operation==11;});
            bool same_bytes=true;
            if(wrote) for(const auto& call:next_calls) if(call.operation==11)
                same_bytes=same_bytes && std::memcmp(next_bytes.data()+call.a,buffer.bytes.data()+call.a,call.b)==0;
            check("notification_cursor_wrap_restore_and_loop",next_calls==buffer.calls && same_bytes && std::memcmp(&next_wave,reader,sizeof *reader)==0 && std::memcmp(stream_raw.data(),&stream,0xa8)==0);
            reset_wave();reader->looped=n%2;const auto reset_before=*reader;
            stream.silence=1;stream.next_write=512;stream.last_play_cursor=91;stream.played_bytes=999;stream.retained_94=37;
            std::memcpy(stream_raw.data(),&stream,0xa8);buffer.calls.clear();buffer.status=0;
            const auto reset_result=original<HRESULT>(0x5b490,stream_raw.data(),position);const auto reset_after=*reader;const auto reset_calls=buffer.calls;
            *reader=reset_before;buffer.calls.clear();const auto source_reset=stream.reset(position);
            check("stream_reset_fields_and_reader",reset_result==source_reset && reset_calls==buffer.calls && std::memcmp(&reset_after,reader,sizeof *reader)==0 && std::memcmp(stream_raw.data(),&stream,0xa8)==0);
        }
        const auto file_path=report.parent_path()/"oracle_wave_input.bin";
        {std::ofstream file(file_path,std::ios::binary);file.write(reinterpret_cast<const char*>(samples.data()),samples.size());}
        const auto filename=file_path.string();
        format.file_offset=128;format.total_bytes=4096;format.loop_start=320;
        *reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c0004)=64;
        for(unsigned n=0;n<256;++n) {
            audio::WaveReader wa,wb;
            const auto opened=original<HRESULT>(0x5ab40,&wa,filename.c_str(),&format,1u);
            const auto cpp_opened=wb.open_file(filename.c_str(),&format,1,64);
            const auto same_reader=[&] {
                const auto handle=wa.file;wa.file=wb.file;const bool equal=std::memcmp(&wa,&wb,sizeof wa)==0;wa.file=handle;return equal;
            };
            check("file_open_and_initial_seek",opened==cpp_opened && same_reader() && wa.tell()==wb.tell());
            const bool loop=n%2;const auto position=rng()%8192;
            const auto reset_a=original<HRESULT>(0x5b5f0,&wa,static_cast<unsigned>(loop),position),reset_b=wb.reset(loop,position,64);
            check("file_reset_single_wrap_and_loop",reset_a==reset_b && same_reader() && wa.tell()==wb.tell());
            std::array<std::uint8_t,512> a{},b{};std::uint32_t ca=0,cb=0;
            const auto read_a=original<HRESULT>(0x5aff0,&wa,a.data(),512u,&ca),read_b=wb.read(b.data(),512,&cb);
            check("file_read_remaining_and_bytes",read_a==read_b && ca==cb && a==b && same_reader() && wa.tell()==wb.tell());
            const auto close_a=original<HRESULT>(0x59a30,&wa),close_b=wb.close();
            check("file_close_state",close_a==close_b && same_reader());
        }
        runtime::set_default_resource(previous);
        unsigned total=0;for(auto [name,count]:counts) total+=count;
        std::ofstream output(report);output<<"{\n\"status\":"<<th20::json_string(failed?"failed":"passed")<<",\"original_sha256\":"<<th20::json_string(expected_sha)<<",\"cases\":"<<total<<",\"failed\":"<<failed<<",\n\"coverage\":{";
        bool first=true;for(auto [name,count]:counts) {if(!first)output<<',';first=false;output<<th20::json_string(name)<<':'<<count;}output<<"},\n\"failures\":[";
        first=true;for(auto& failure:failures){if(!first)output<<',';first=false;output<<th20::json_string(failure);}output<<"],\n\"source_sha256\":{";
        first=true;for(const auto& binding:audio_source_hashes) {if(!first)output<<',';first=false;output<<th20::json_string(binding.name)<<':'<<th20::json_string(binding.hash);}
        output<<"},\n\"scope\":\"Isolated original CPU calls; recorded DirectSound boundary. No full-game, driver timing or audible-output equivalence claim.\"}\n";
        std::cout<<total<<" audio CPU comparisons, "<<failed<<" failed\n";return failed?1:0;
    } catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 2;}
}
