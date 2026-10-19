#include "replay.hpp"
#include "data_strings.hpp"
#include "../archive/resource_manager.hpp"
#include "../progress_state/compression.hpp"
#include "../progress_state/records.hpp"
#ifdef TH_SDL3
#include "platform/Files.hpp"
#endif
#include "../gameplay/player_state.hpp"
#include "../platform_services/services.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/frame_statistics.hpp"
#include "../platform_window/platform_window.hpp"
#include <ctime>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <filesystem>
#include <stdexcept>
#include <emmintrin.h>
namespace th20::source::replay {
int prepare_save(ReplayInf& o,int mode){_time64(&o.user->timestamp);o.user->finished_stage=mode?recovered::signed_bits(static_cast<unsigned>(mode)+7u):gameplay::player_state::stage(game_session::session.player_table);return 0;}
namespace {
template<class F>void chunks(ReplayInf& o,int stage,F f){for(auto* link=o.recordings[stage].next;link;link=link->next)f(*reinterpret_cast<RecordingChunk*>(link->value));}
void append(Bytes& bytes,const void* data,std::size_t count){const auto* start=static_cast<const std::uint8_t*>(data);bytes.insert(bytes.end(),start,start+count);}
void format(Bytes& output,const char* pattern,...){char text[60000];va_list args;va_start(args,pattern);const int count=vsprintf_s(text,sizeof(text),pattern,args);va_end(args);if(count<0)throw std::runtime_error("Replay USER text formatting failed");append(output,text,static_cast<unsigned>(count));}
Bytes user_text(ReplayInf& o,int first,int last){
    Bytes bytes(12,0);progress::write(bytes.data(),0,0x52455355u);
    format(bytes,data::s_00573530,data::s_00573524);format(bytes,data::s_00573554,data::s_0057354c);format(bytes,data::s_00573564,reinterpret_cast<const char*>(o.user));
    std::tm date{};_localtime64_s(&date,&o.user->timestamp);format(bytes,data::s_00573570,date.tm_year%100,date.tm_mon+1,date.tm_mday,date.tm_hour,date.tm_min);
    const auto character=o.user->fields_d0[2]*9u+o.user->stones[0];if(character>=18u||static_cast<unsigned>(o.user->difficulty)>=5u)throw std::out_of_range("Replay USER original string table index");
    //509a81 really multiplies character by9, indexing across contiguous literal tables.
    format(bytes,data::s_00573590,data::characters[character]);format(bytes,data::s_0057359c,data::ranks[o.user->difficulty]);
    if(o.user->finished_stage>7)format(bytes,first==7?data::s_005735a8:data::s_005735bc);
    else if(first==last){if(first==7)format(bytes,data::s_005735d0);else format(bytes,data::s_005735e0,first);}
    else format(bytes,data::s_005735ec,first,last);
    format(bytes,data::s_00573600,progress::read<std::uint64_t>(o.user,0x18));format(bytes,data::s_00573610,static_cast<double>(progress::read<float>(o.user,0xd0)));
    bytes.push_back(0);bytes.resize((bytes.size()+3u)&~3u,0);progress::write(bytes.data(),4,static_cast<std::uint32_t>(bytes.size()));return bytes;
}
Bytes comment_text(){Bytes bytes(12,0);progress::write(bytes.data(),0,0x52455355u);bytes[8]=1;format(bytes,data::s_00573624);bytes.push_back(0);bytes.resize((bytes.size()+3u)&~3u,0);progress::write(bytes.data(),4,static_cast<std::uint32_t>(bytes.size()));return bytes;}
#ifdef TH_SDL3
void write(std::uint32_t handle,const void* bytes,DWORD count){if(!handle)return;const auto written=web::files::write(handle,bytes,count);if(written!=count)web::files::close(handle);}
#else
void write(HANDLE handle,const void* bytes,DWORD count){if(handle==INVALID_HANDLE_VALUE)return;DWORD written=0;WriteFile(handle,bytes,count,&written,nullptr);if(written!=count)CloseHandle(handle);}
#endif
}
void save(ReplayInf& o,const char* filename,const char* name,int,int append_terminator){
    strcpy_s(reinterpret_cast<char*>(o.user),9,name);for(std::size_t n=std::strlen(name);n<8;++n)reinterpret_cast<char*>(o.user)[n]=' ';
    if(!(o.flags&1u)&&append_terminator){auto& chunk=*reinterpret_cast<RecordingChunk*>(o.active_chunk->value);if(chunk.append(0xffff,0xffff,0xffff))o.active_chunk=o.add_recording_chunk(o.active_stage);}
    int first=0,last=0,count=0;std::uint32_t size=0x100;
    for(int i=0;i<8;++i)if(auto* record=o.stages[i]){
        if(!first)first=i;last=i;if(!(o.flags&1u))record->data_bytes=0;size+=0x2a0;
        chunks(o,i,[&](RecordingChunk& chunk){const unsigned bytes=chunk.frame_count()*6u+chunk.fps_count();size+=bytes;if(!(o.flags&1u)){record->data_bytes+=bytes;record->frame_count+=chunk.frame_count();}});++count;
    }
    o.user->fields_d0[1]=count;progress::write(o.user,0x18,gameplay::player_state::score(game_session::player(0)));
    const auto& stats=*static_cast<platform_window::FrameStatistics*>(platform_window::unrecovered::scheduler_object_005c4a00);
    const double ratio=_mm_cvtsd_f64(_mm_div_sd(_mm_set_sd(stats.actual_frames),_mm_set_sd(stats.target_frames)));
    // A run that consumed an unlimited drag is saved with a 100% processing-drop
    // rate, matching the Launcher touch protocol and the th08/th10 ports.
    const float slow=platform::unlimited_touch_used()?100.f:
        recovered::add32(100.f,-recovered::mul32(static_cast<float>(ratio),100.f));progress::write(o.user,0xd0,slow);
    Bytes unpacked;unpacked.reserve(size);append(unpacked,o.user,0x100);
    for(int i=0;i<8;++i)if(auto* record=o.stages[i]){append(unpacked,record,0x2a0);chunks(o,i,[&](RecordingChunk& chunk){append(unpacked,chunk.inputs,chunk.frame_count()*6u);});chunks(o,i,[&](RecordingChunk& chunk){append(unpacked,chunk.fps,chunk.fps_count());});}
    Bytes packed;resources::with_shared_dictionary([&](auto& dictionary){progress::LzssEncoder encoder(dictionary);packed=encoder.encode(unpacked);});
    const auto length=static_cast<std::uint32_t>(packed.size());progress::encrypt(packed,{0x7d,0x3a,0x100,length});progress::encrypt(packed,{0x5c,0xe1,0x400,length});
    o.header->unpacked_size=static_cast<unsigned>(unpacked.size());o.header->packed_size=length;o.header->field_0c=length+0x30u;
    const auto path=(std::filesystem::path(program_entry::window_state.user_data_directory)/"replay"/filename).string();
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(2));
#ifdef TH_SDL3
    const auto file=web::files::open(path.c_str(),true);
    write(file,o.header,0x30);write(file,packed.data(),length);
    auto metadata=user_text(o,first,last);write(file,metadata.data(),static_cast<DWORD>(metadata.size()));auto comment=comment_text();write(file,comment.data(),static_cast<DWORD>(comment.size()));
    if(file)web::files::close(file);
#else
    wchar_t wide_path[MAX_PATH+2]{};MultiByteToWideChar(932,0,path.c_str(),-1,wide_path,MAX_PATH);
    HANDLE file=CreateFileW(wide_path,GENERIC_WRITE,FILE_SHARE_READ,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(file==INVALID_HANDLE_VALUE){wchar_t* message=nullptr;FormatMessageW(0x1300,nullptr,GetLastError(),0x400,reinterpret_cast<LPWSTR>(&message),0,nullptr);LocalFree(message);}
    write(file,o.header,0x30);write(file,packed.data(),length);
    auto metadata=user_text(o,first,last);write(file,metadata.data(),static_cast<DWORD>(metadata.size()));auto comment=comment_text();write(file,comment.data(),static_cast<DWORD>(comment.size()));
    if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);
#endif
    o.flags|=1u;
}
}
