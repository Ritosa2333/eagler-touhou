#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../replay.hpp"
#include "../input.hpp"
namespace rp=th20::source::replay;
template<class R,class...A>R cpu(unsigned va,void* self,A...args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
int wmain(int argc,wchar_t**argv){try{
    if(argc!=3)throw std::runtime_error("Usage: replay_compare ORIGINAL.exe REPORT.json");auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");const auto info=th20::parse_pe(bytes);Mapping original(bytes,info);mapped_image_base=original.address();
    std::mt19937 random(0x507560);unsigned passed=0,failed=0;std::vector<std::string> failures;
    auto fill=[&](void* value,unsigned size){auto* at=static_cast<unsigned char*>(value);for(unsigned i=0;i<size;++i)at[i]=static_cast<unsigned char>(random());};
    auto check=[&](const char* what,unsigned test,const void* expected,const void* actual,unsigned size){if(!std::memcmp(expected,actual,size)){++passed;return;}++failed;if(failures.size()<25){std::ostringstream detail;detail<<what<<" test="<<test;for(unsigned i=0;i<size;++i)if(static_cast<const unsigned char*>(expected)[i]!=static_cast<const unsigned char*>(actual)[i]){detail<<" offset="<<std::hex<<i<<" expected="<<unsigned(static_cast<const unsigned char*>(expected)[i])<<" actual="<<unsigned(static_cast<const unsigned char*>(actual)[i]);break;}failures.push_back(detail.str());}};
    for(unsigned test=0;test<4096;++test){
        alignas(8) unsigned char e[0x2a0],a[0x2a0];
        fill(e,sizeof(e));std::memcpy(a,e,sizeof(a));cpu<void*>(0x507480,e);new(a)rp::FileHeader;check("file_header_constructor",test,e,a,sizeof(rp::FileHeader));
        fill(e,sizeof(e));std::memcpy(a,e,sizeof(a));cpu<void*>(0x507360,e);new(a)rp::UserHeader;check("user_header_constructor",test,e,a,sizeof(rp::UserHeader));
        fill(e,sizeof(e));std::memcpy(a,e,sizeof(a));cpu<void*>(0x5077d0,e);new(a)rp::StageRecord;check("stage_record_constructor",test,e,a,sizeof(rp::StageRecord));
        fill(e,sizeof(e));std::memcpy(a,e,sizeof(a));cpu<void*>(0x5076b0,e);auto* cursor=new(a)rp::PlaybackCursor;auto* expected=reinterpret_cast<rp::PlaybackCursor*>(e);expected->link.value=reinterpret_cast<th20::source::scheduler::Node*>(a);check("playback_cursor_constructor",test,e,a,sizeof(rp::PlaybackCursor));cursor->~PlaybackCursor();
    }
    for(unsigned test=0;test<16384;++test){
        th20::source::input::ButtonState e,a;fill(&e,sizeof(e));a=e;if(test%2){cpu<void>(0x5091d0,&e);rp::reset_input(a);check("replay_input_reset",test,&e,&a,sizeof(e));}else{cpu<void>(0x50a4d0,&e);rp::update_input(a);check("replay_input_update",test,&e,&a,sizeof(e));}
    }
    auto em=std::make_unique<unsigned char[]>(sizeof(rp::RecordingChunk)),am=std::make_unique<unsigned char[]>(sizeof(rp::RecordingChunk));
    for(unsigned test=0;test<512;++test){
        fill(em.get(),sizeof(rp::RecordingChunk));std::memcpy(am.get(),em.get(),sizeof(rp::RecordingChunk));cpu<void*>(0x507720,em.get());auto* a=new(am.get())rp::RecordingChunk;auto* e=reinterpret_cast<rp::RecordingChunk*>(em.get());e->input_cursor=a->input_cursor;e->fps_cursor=a->fps_cursor;e->link.value=a->link.value;check("recording_chunk_constructor",test,e,a,sizeof(*a));
        const unsigned position=test%3==0?35999:random()%35999;e->input_cursor=e->inputs+position;a->input_cursor=a->inputs+position;const auto x=static_cast<std::uint16_t>(random()),y=static_cast<std::uint16_t>(random()),z=static_cast<std::uint16_t>(random());const bool er=cpu<bool>(0x509e40,e,x,y,z),ar=a->append(x,y,z);check("append_frame_result",test,&er,&ar,1);check("append_frame_bytes",test,e->inputs,a->inputs,sizeof(e->inputs));const auto ec=e->input_cursor-e->inputs,ac=a->input_cursor-a->inputs;check("append_frame_cursor",test,&ec,&ac,4);
        const unsigned fps_position=test%3==0?1199:random()%36000;e->fps_cursor=e->fps+fps_position;a->fps_cursor=a->fps+fps_position;const auto value=static_cast<std::uint8_t>(random());const bool ef=cpu<bool>(0x509ee0,e,value),af=a->append_fps(value);check("append_fps_result",test,&ef,&af,1);check("append_fps_bytes",test,e->fps,a->fps,sizeof(e->fps));a->~RecordingChunk();
    }
    std::ofstream out(argv[2]);out<<"{\"original_sha256\":"<<th20::json_string(expected_sha)<<",\"passed\":"<<passed<<",\"failed\":"<<failed<<",\"failures\":[";for(unsigned i=0;i<failures.size();++i){if(i)out<<",";out<<th20::json_string(failures[i]);std::cerr<<failures[i]<<"\n";}out<<"]}\n";std::cout<<"passed="<<passed<<" failed="<<failed<<"\n";return failed?1:0;
}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 2;}}
