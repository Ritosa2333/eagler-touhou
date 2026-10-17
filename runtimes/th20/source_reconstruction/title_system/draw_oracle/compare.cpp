// Original three complete page entries execute, including native CRT formatting,
// localtime and ASCII queue writes. Only dynamic Japanese text scheduling is a
// paired trace boundary; this fixture does not claim new GDI/GPU validation.
#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../rank_entry.hpp"
#include "../replay_save.hpp"
#include "../player_data.hpp"
#include "../practice_data.hpp"
#include "../../program_entry/program_entry.hpp"
#include "../../stone_menu/stone.hpp"
#include "../../startup_scene/startup.hpp"
#include "source_hashes.hpp"
#include <cstdarg>
#include <ctime>
#include <memory>
namespace ti=th20::source::title;namespace tx=th20::source::text;namespace pr=th20::source::progress;namespace gs=th20::source::game_session;namespace rp=th20::source::replay;namespace pe=th20::source::program_entry;namespace sm=th20::source::stone_menu;
namespace th20::source::text {Renderer* renderer;}
namespace th20::source::progress {SaveManager* manager;}
namespace th20::source::stone_menu {StoneMenuInf* controller;}
namespace th20::source::program_entry {WindowStatePrefix window_state;}
template<class T>T& at(unsigned va){return *reinterpret_cast<T*>(mapped_image_base+va-0x400000);}
template<class R,class...A>R cpu(unsigned va,void* self,A...args){return reinterpret_cast<R(__thiscall*)(void*,A...)>(mapped_image_base+va-0x400000)(self,args...);}
std::vector<unsigned> dynamic_trace;
void capture(tx::Renderer& r,const th20::source::sprite::Vec3& p,const char* value){
 auto append=[&](const void* p,unsigned n){const auto* b=static_cast<const unsigned char*>(p);for(unsigned i=0;i<n;++i)dynamic_trace.push_back(b[i]);};
 append(&p,sizeof(p));append(&r.color,0x44);const auto size=std::strlen(value);dynamic_trace.push_back(static_cast<unsigned>(size));append(value,static_cast<unsigned>(size));
}
void __cdecl dynamic_native(tx::Renderer* r,const th20::source::sprite::Vec3* p,const char* format,unsigned length,...){
 (void)length;char buffer[4096];va_list args;va_start(args,length);vsprintf_s(buffer,sizeof(buffer),format,args);va_end(args);capture(*r,*p,buffer);
}
namespace th20::source::text {
void Renderer::write_text(const sprite::Vec3& p,const char* format,...){char buffer[4096];va_list args;va_start(args,format);vsprintf_s(buffer,sizeof(buffer),format,args);va_end(args);capture(*this,p,buffer);}
void Renderer::write_text_literal(const sprite::Vec3& p,const char* value){capture(*this,p,value);}
}
void hook(unsigned va,void* target){auto* p=reinterpret_cast<unsigned char*>(mapped_image_base+va-0x400000);p[0]=0xe9;*reinterpret_cast<int*>(p+1)=reinterpret_cast<unsigned char*>(target)-p-5;FlushInstructionCache(GetCurrentProcess(),p,5);}
auto bytes(const void* p,unsigned n){const auto* b=static_cast<const unsigned char*>(p);return std::vector<unsigned char>(b,b+n);}
unsigned case_id=0,entry_id=0,passed=0,failed=0;std::string error_details;
void prepare_case(){FloatingEnvironment::prepare();const unsigned mode=(case_id/2048)%4;const unsigned short control=static_cast<unsigned short>(0x37f|(mode<<10));__asm fldcw control
 _mm_setcsr(0x1f80|(mode<<13));}
void check(const char* label,const void* expected,const void* actual,std::size_t size){
 if(std::memcmp(expected,actual,size)==0){++passed;return;}++failed;if(failed<20){const auto* e=static_cast<const unsigned char*>(expected);const auto* a=static_cast<const unsigned char*>(actual);std::size_t offset=0;while(offset<size&&e[offset]==a[offset])++offset;std::cerr<<std::hex<<entry_id<<std::dec<<" case="<<case_id<<" "<<label<<" offset="<<std::hex<<offset<<" expected="<<unsigned(e[offset])<<" actual="<<unsigned(a[offset])<<std::dec<<"\n";}}
int wmain(int argc,wchar_t** argv){try{
 if(argc!=3&&argc!=4)throw std::runtime_error("Usage: title_draw_compare ORIGINAL.exe REPORT.json [NATIVE_SAVE_SLOWDOWN_HEX]");
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
 AddVectoredExceptionHandler(1,[](PEXCEPTION_POINTERS p)->LONG{if(p->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION)std::cerr<<"AV entry="<<std::hex<<entry_id<<" case="<<std::dec<<case_id<<" va="<<std::hex<<(p->ContextRecord->Eip-mapped_image_base+0x400000)<<" address="<<p->ExceptionRecord->ExceptionInformation[1]<<std::endl;return EXCEPTION_CONTINUE_SEARCH;});
 const auto image_bytes=th20::read_file(argv[1]);if(sha256(image_bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");const auto info=th20::parse_pe(image_bytes);Mapping image(image_bytes,info);mapped_image_base=image.address();
 for(const auto& item:info.imports)for(const auto* name:{L"kernel32.dll",L"gdi32.dll",L"user32.dll"})if(auto dll=GetModuleHandleW(name))if(auto entry=GetProcAddress(dll,item.name.c_str())){*reinterpret_cast<FARPROC*>(mapped_image_base+item.iat_rva)=entry;break;}
 at<HANDLE>(0x5e5990)=GetProcessHeap();for(unsigned i=0;i<22;++i){auto* lock=reinterpret_cast<unsigned*>(mapped_image_base+0x1c0240+i*0x30);std::memset(lock,0,0x30);lock[0]=0x101;lock[10]=GetCurrentThreadId();lock[11]=1;}
 _tzset();long timezone=0,bias=0;int daylight=0;_get_timezone(&timezone);_get_dstbias(&bias);_get_daylight(&daylight);if(daylight)throw std::runtime_error("Native timezone fixture requires non-DST host");at<long>(0x5e5898)=timezone;at<int>(0x5e589c)=0;at<long>(0x5e58a0)=bias;at<unsigned>(0x5e58b4)=1;
 hook(0x46c210,reinterpret_cast<void*>(&dynamic_native));
 auto title_memory=std::make_unique<unsigned char[]>(sizeof(ti::TitleInf)),renderer_memory=std::make_unique<unsigned char[]>(sizeof(tx::Renderer)),manager_memory=std::make_unique<unsigned char[]>(sizeof(pr::SaveManager)),stone_memory=std::make_unique<unsigned char[]>(sizeof(sm::StoneMenuInf));
 auto& o=*reinterpret_cast<ti::TitleInf*>(title_memory.get());auto& r=*reinterpret_cast<tx::Renderer*>(renderer_memory.get());auto& manager=*reinterpret_cast<pr::SaveManager*>(manager_memory.get());auto& stone=*reinterpret_cast<sm::StoneMenuInf*>(stone_memory.get());
 tx::renderer=at<tx::Renderer*>(0x5c0698)=&r;pr::manager=at<pr::SaveManager*>(0x5c6108)=&manager;sm::controller=at<sm::StoneMenuInf*>(0x5c6120)=&stone;
 auto replay_memory=std::make_unique<unsigned char[]>(sizeof(rp::ReplayInf)*25),user_memory=std::make_unique<unsigned char[]>(sizeof(rp::UserHeader)*25);auto* replays=reinterpret_cast<rp::ReplayInf*>(replay_memory.get());auto* users=reinterpret_cast<rp::UserHeader*>(user_memory.get());
 th20::source::startup::unrecovered::owner_005c60fc=&replays[0];at<void*>(0x5c60fc)=&replays[0];for(unsigned i=0;i<25;++i)replays[i].user=&users[i];
 for(unsigned i=0;i<88;++i)std::snprintf(stone.names[i],256,"Stone %02u \x82\xa0",i);
 gs::Player selected{};gs::session.contexts[0].current_player=&selected;at<gs::Session>(0x5ba568).contexts[0].current_player=&selected;
 if(argc==4){
  entry_id=0x5277f0;o.phase=2;o.cursor.current=0;o.metadata[0]=&replays[0];strcpy_s(reinterpret_cast<char*>(&users[0]),10,"ORIGINAL");users[0].timestamp=1767225600;users[0].finished_stage=1;users[0].fields_d0[0]=std::wcstoul(argv[3],nullptr,16);r.line_count=0;r.scale_x=r.scale_y=1;r.font_width=9;r.color=r.field_1a1c8=0xffffffff;r.shadow_color=0xff000000;pe::window_state.scale=at<float>(0x5b8818)=1;
  std::ofstream report(argv[2]);report<<"original_sha256="<<expected_sha<<"\nslowdown_bits="<<std::hex<<users[0].fields_d0[0]<<std::dec<<"\n"<<std::flush;FloatingEnvironment::prepare();cpu<void>(entry_id,&o);report<<"native_returned lines="<<r.line_count<<"\n";for(int i=0;i<r.line_count;++i)report<<r.lines[i].text<<"\n";return 0;
 }
 std::mt19937 rng(0x522e2052);constexpr unsigned entries[]{0x522e20,0x5277f0,0x5257f0};constexpr std::int64_t dates[]{0,946684800,1593561600,1767225600,1893456000};
 for(unsigned entry:entries)for(case_id=0;case_id<8192;++case_id){entry_id=entry;for(unsigned i=0;i<sizeof(o);++i)title_memory[i]=static_cast<unsigned char>(rng());o.phase=(case_id%5==0)?int(case_id%8)-1:2;if(entry==0x5277f0&&case_id%2)o.phase=3;
  o.cursor.current=int(case_id%10);o.cursor56e8.current=int(case_id%91);o.cursor108.current=int((case_id/7)%3);o.cursorbc.current=int((case_id/3)%5);o.word56e4=(case_id/11)%2;o.word56e0=case_id%9;std::memcpy(ti::entered_name(o),"NAME    ",9);o.words5734[1]=case_id%25;o.age.current=int(case_id%17)-3;o.age.current_f=float(o.age.current)+float(case_id%4)*.25f;o.words58d8[8]=case_id%2;
  selected.fields_00[2]=(case_id/5)%2;selected.fields_00[3]=(case_id/13)%9;gs::session.player_table.field_1e0=at<gs::Session>(0x5ba568).player_table.field_1e0=(case_id/17)%5;
  if(entry==0x5257f0){o.cursor.current=int(case_id%16);selected.fields_00[2]=o.cursor.current/8;selected.fields_00[3]=o.cursor.current%8;int count=0;for(auto difficulty:ti::practice_data::difficulties)if(difficulty==o.cursorbc.current)++count;o.cursor108.current%=((count+9)/10);}
  auto* profile=pr::current_profile();auto& fallback=pr::fallback_profile();
  for(unsigned j=0;j<50;++j){auto* record=profile->bytes+0x18+j*40;pr::write<std::uint64_t>(record,0,(static_cast<std::uint64_t>(rng())<<32)|rng());record[8]=(case_id+j)%9;record[9]=static_cast<unsigned char>((case_id+j)%10);std::snprintf(reinterpret_cast<char*>(record+10),10,"N%07u",j);pr::write<std::int64_t>(record,24,dates[(case_id+j)%std::size(dates)]);pr::write<float>(record,32,float((case_id+j)%1000)/8.f);}
  for(unsigned j=0;j<113;++j){auto* record=profile->bytes+0xb08+j*0xe0;auto* common=fallback.bytes+0xb08+j*0xe0;std::snprintf(reinterpret_cast<char*>(common),160,"Card %03u \x82\xa0",j+1);pr::write<unsigned>(common,0xc8,(case_id+j)%3);pr::write<int>(record,0xc0,rng());pr::write<int>(record,0xc8,rng());}
  pr::write<int>(profile->bytes,0x76a8,rng());pr::write<std::uint64_t>(profile->bytes,0x76b0,(static_cast<std::uint64_t>(rng())<<32)|rng());for(unsigned j=0;j<5;++j)pr::write<int>(profile->bytes,0x76b8+j*4,rng());
  for(unsigned j=0;j<25;++j){auto& user=users[j];std::snprintf(reinterpret_cast<char*>(&user),10,"R%07u",j);user.timestamp=dates[(case_id+j)%std::size(dates)];user.fields_d0[2]=(case_id+j)%2;user.stones[0]=(case_id+j)%9;user.difficulty=(case_id+j)%5;user.finished_stage=(case_id+j)%9;constexpr unsigned slows[]{0,0x3e800000,0x3f800001,0x3f800002,0x3f800004,0x3f800005,0x3f800006,0x3dcccccd};pr::write<unsigned>(&user,0xd0,slows[(case_id+j)%std::size(slows)]);o.metadata[j]=(case_id+j)%3?&replays[j]:nullptr;}
  std::memset(&r,0x67,sizeof(r));r.line_count=case_id%4?int(case_id%40):int(299+case_id%22);r.color=rng();r.field_1a1c8=rng();r.shadow_color=rng();r.scale_x=.75f;r.scale_y=1.25f;r.rotation=.125f;r.font_width=9;for(auto& word:r.fields_1a1d4)word=rng();r.fields_1a1d4[3]=(case_id/17)%14;
  pe::window_state.scale=at<float>(0x5b8818)=float(case_id%5+1)*.5f;
  const auto before_renderer=bytes(&r,sizeof(r)),before_title=bytes(&o,sizeof(o)),before_profile=bytes(profile,sizeof(*profile)),before_fallback=bytes(&fallback,sizeof(fallback)),before_users=bytes(users,sizeof(rp::UserHeader)*25);
  dynamic_trace.clear();prepare_case();int result=1;if(entry==0x5257f0)result=cpu<int>(entry,&o);else cpu<void>(entry,&o);const auto expected_renderer=bytes(&r,sizeof(r)),expected_title=bytes(&o,sizeof(o)),expected_profile=bytes(profile,sizeof(*profile)),expected_fallback=bytes(&fallback,sizeof(fallback)),expected_users=bytes(users,sizeof(rp::UserHeader)*25);const auto expected_trace=dynamic_trace;
  std::memcpy(&r,before_renderer.data(),sizeof(r));std::memcpy(&o,before_title.data(),sizeof(o));std::memcpy(profile,before_profile.data(),sizeof(*profile));std::memcpy(&fallback,before_fallback.data(),sizeof(fallback));std::memcpy(users,before_users.data(),sizeof(rp::UserHeader)*25);
  dynamic_trace.clear();prepare_case();int actual=1;if(entry==0x522e20)ti::draw_rank_entry(o);else if(entry==0x5277f0)ti::draw_replay_save(o);else actual=ti::draw_player_data_detail(o,r,*profile,fallback,stone.names[(o.cursor.current%8)*4]);
  check("return",&result,&actual,4);check("renderer",expected_renderer.data(),&r,sizeof(r));check("title",expected_title.data(),&o,sizeof(o));check("profile",expected_profile.data(),profile,sizeof(*profile));check("fallback",expected_fallback.data(),&fallback,sizeof(fallback));check("users",expected_users.data(),users,sizeof(rp::UserHeader)*25);const auto expected_size=expected_trace.size(),actual_size=dynamic_trace.size();check("dynamic_count",&expected_size,&actual_size,sizeof(actual_size));if(expected_size==actual_size)check("dynamic_trace",expected_trace.data(),dynamic_trace.data(),expected_size*4);
  if(failed&&case_id==2){auto folder=std::filesystem::path(argv[2]).parent_path();std::ofstream a(folder/(std::to_string(entry)+"_original.bin"),std::ios::binary),b(folder/(std::to_string(entry)+"_source.bin"),std::ios::binary);a.write(reinterpret_cast<const char*>(expected_renderer.data()),sizeof(r));b.write(reinterpret_cast<const char*>(&r),sizeof(r));}
 }
 std::ofstream report(argv[2]);report<<"{\"status\":\""<<(failed?"failed":"passed")<<"\",\"original_sha256\":\""<<expected_sha<<"\",\"passed\":"<<passed<<",\"failed\":"<<failed<<",\"entries\":[\"00522e20\",\"005277f0\",\"005257f0\"],\"scenarios\":24576,\"boundary\":\"46c210 dynamic CP932 text scheduling paired trace only; original ASCII formatting, localtime and Renderer queue execute\",\"source_hashes\":";for(auto* part:title_draw_hashes)report<<part;report<<"}\n";std::cout<<"passed="<<passed<<" failed="<<failed<<"\n";return failed?1:0;
 }catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 2;}}
