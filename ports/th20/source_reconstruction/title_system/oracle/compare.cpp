#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../title.hpp"
#include "../background.hpp"
#include "../music.hpp"
#include "../stage_select.hpp"
#include "../loadout.hpp"
#include "../replay_menu.hpp"
#include "../rank_entry.hpp"
#include "../replay_save.hpp"
#include "../practice.hpp"
#include "../player_data.hpp"
#include "../stones.hpp"
#include "../frame.hpp"
#include "../../pause_system/menu_support.hpp"
#include "../../gameplay/player_state.hpp"
#include "source_hashes.hpp"
#include "lifecycle_fixture.hpp"
namespace ti=th20::source::title;namespace gs=th20::source::game_session;namespace mn=th20::source::menu;
template<class R,class... A>R cpu(unsigned va,void* self,A... args){return reinterpret_cast<R(__thiscall*)(void*,A...)>(mapped_image_base+va-0x400000)(self,args...);}
template<class T>T& at(unsigned va){return *reinterpret_cast<T*>(mapped_image_base+va-0x400000);}
alignas(ti::TitleInf) unsigned char storage[sizeof(ti::TitleInf)];auto& object=*reinterpret_cast<ti::TitleInf*>(storage);
std::vector<unsigned> trace;void event(unsigned id,unsigned value=0){trace.insert(trace.end(),{id,value});}
bool extra,notice;unsigned notice_data[64]{};
bool exists(unsigned h){return (h&1)!=0;}
void animation(ti::TitleInf& o,int index,int event_id,bool clear){event(3,index);event(4,event_id);if(clear)o.handles[index]=0;}
struct Host final:ti::MainEnvironment {
 Host():MainEnvironment(at<gs::Session>(0x5ba568),at<int>(0x5b0a60),at<int>(0x5c6140)){}
 bool pressed(unsigned mask)override{return (at<unsigned>(0x5b88c0)&mask)!=0;}
 bool repeated(unsigned mask)override{return ((at<unsigned>(0x5b88c0)|at<unsigned>(0x5b88b8))&mask)!=0;}
 bool extra_unlocked()override{return extra;}
 bool notice_present()override{return at<void*>(0x5c5b38)!=nullptr;}
 bool notice_finished()override{return notice_data[0x90/4]!=0;}
 bool notice_pending()override{return notice;}
 void create_notice()override{event(10);at<void*>(0x5c5b38)=notice_data;}
 void retire_notice()override{event(11);at<void*>(0x5c5b38)=nullptr;}
 void sound(int id)override{event(1,id);}
 void spawn(ti::TitleInf& o,int i)override{event(2,i);o.handles[i]=1001+i*2;}
 void interrupt(ti::TitleInf& o,int i,int e,bool clear)override{animation(o,i,e,clear);}
 bool exists(unsigned h)override{return ::exists(h);}
 bool decoration_exists(ti::TitleInf& o)override{if(::exists(o.handle474))return true;o.handle474=0;return false;}
 void interrupt_handle(unsigned h,int e)override{event(5,h);event(6,e);}
 void spawn_decoration(ti::TitleInf& o)override{event(7);o.handle474=2001;}
};
int __fastcall extra_query(void*,void*){return extra;}int __fastcall pending(void*,void*){return notice;}
int __fastcall pop_notice(void*,void*){return 4;}
void* __cdecl create_notice(int){event(10);at<void*>(0x5c5b38)=notice_data;return notice_data;}
void __cdecl retire_notice(void*){event(11);at<void*>(0x5c5b38)=nullptr;}
void __fastcall sound(void*,void*,int id,int){event(1,id);}
void __fastcall spawn(ti::TitleInf* o,void*,int i){event(2,i);o->handles[i]=1001+i*2;}
void __fastcall activate(ti::TitleInf* o,void*,int i){animation(*o,i,2,false);}
void __fastcall deactivate(ti::TitleInf* o,void*,int i){animation(*o,i,3,false);}
void __fastcall erase(ti::TitleInf* o,void*,int i){animation(*o,i,1,true);}
int __fastcall handle_exists(unsigned* h,void*){if(exists(*h))return 1;*h=0;return 0;}
int __fastcall indexed_exists(ti::TitleInf* o,void*,int i){return exists(o->handles[i]);}
void __fastcall interrupt1(unsigned* h,void*){event(5,*h);event(6,1);}
unsigned* __fastcall decoration(void*,void*,unsigned* out,const char*,int script,int,void*){event(script==19?40:7);*out=script==19?4001:2001;return out;}
void __fastcall mesh_initialize(void*,void*,float,float,float,float){event(30);}
void __fastcall mesh_commit(void*,void*){event(31);}
#include "selection_fixture.hpp"
int wmain(int argc,wchar_t** argv){try{
 if(argc!=3)throw std::runtime_error("Usage: title_compare ORIGINAL REPORT");auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");Mapping image(bytes,th20::parse_pe(bytes));mapped_image_base=image.address();
 AddVectoredExceptionHandler(1,[](PEXCEPTION_POINTERS p)->LONG{if(p->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION)std::cerr<<"ip="<<std::hex<<p->ContextRecord->Eip-mapped_image_base+0x400000<<" address="<<p->ExceptionRecord->ExceptionInformation[1]<<std::endl;return EXCEPTION_CONTINUE_SEARCH;});
 auto hook=[&](unsigned va,void* target){auto* p=reinterpret_cast<unsigned char*>(mapped_image_base+va-0x400000);p[0]=0xe9;*reinterpret_cast<unsigned*>(p+1)=reinterpret_cast<unsigned>(target)-reinterpret_cast<unsigned>(p+5);};
 const std::pair<unsigned,void*> hooks[]={{0x52c7b0,&extra_query},{0x52c9f0,&pending},{0x52c320,&pop_notice},{0x4dfb90,&create_notice},{0x4217c0,&retire_notice},{0x426d70,&sound},{0x52cf80,&spawn},{0x52cbf0,&activate},{0x52cc90,&deactivate},{0x52cc30,&erase},{0x47a340,&handle_exists},{0x52c750,&indexed_exists},{0x479040,&interrupt1},{0x450c70,&decoration}};for(auto [va,target]:hooks)hook(va,target);
 new(&object.cursor)mn::Cursor;object.cursor.excluded.reserve(16);
 int history[2][4]{};int* blocks[2][8]{};blocks[0][0]=history[0];blocks[1][0]=history[1];for(unsigned i=0;i<2;++i){auto& d=i?object.cursor.secondary_history:object.cursor.history;d.blocks=blocks[i];d.block_count=8;d.first=0;d.size=0;}
 at<void*>(0x5b889c)=reinterpret_cast<void*>(mapped_image_base+0x1b88b0);Host host;host.session.contexts[0].current_player=&host.session.player_table.players[0];
 std::mt19937 random(0x529e80);unsigned passed=0,failed=0;std::vector<std::string> failures;
 auto check=[&](const char* label,unsigned test,const void* e,const void* a,std::size_t size){if(!std::memcmp(e,a,size)){++passed;return;}++failed;if(failures.size()<30){std::ostringstream out;out<<label<<" case="<<test;for(std::size_t i=0;i<size;++i)if(static_cast<const unsigned char*>(e)[i]!=static_cast<const unsigned char*>(a)[i]){out<<" offset="<<std::hex<<i<<" expected="<<unsigned(static_cast<const unsigned char*>(e)[i])<<" actual="<<unsigned(static_cast<const unsigned char*>(a)[i]);break;}failures.push_back(out.str());}};
 for(unsigned test=0;test<32768;++test){
  for(unsigned i=0;i<sizeof(object);++i)if(i<0x24||i>=0x70)storage[i]=std::uint8_t(random());
  object.state=1;object.previous_state=random()%6;object.phase=test%6;object.cursor.count=10;object.cursor.current=(test/6)%10;object.cursor.previous=random();object.cursor.minimum=0;object.cursor.wrapping=1;object.cursor.excluded.clear();object.cursor.history.size=object.cursor.secondary_history.size=0;object.cursor.history.first=object.cursor.secondary_history.first=0;
  th20::recovered::timer_set(object.age,int((test/60)%140));th20::recovered::timer_set(object.selection_age,random()%9);th20::recovered::timer_set(object.flash_age,random()%32);extra=(test/10)%2;notice=(test/100)%2;at<void*>(0x5c5b38)=test%4?nullptr:notice_data;notice_data[0x90/4]=(test/4)%2;
  at<unsigned>(0x5b88c0)=random()&0x801ff;at<unsigned>(0x5b88b8)=random()&0xf0;at<int>(0x5b0a60)=random()%4;at<int>(0x5c6140)=random()%2;host.session.mode=(test/7)%3;
  std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));const auto session_before=host.session;const auto notice_before=at<void*>(0x5c5b38);int history_before[2][4];std::memcpy(history_before,history,sizeof(history));int exclusions_before[16];std::memcpy(exclusions_before,object.cursor.excluded.data(),64);trace.clear();const int er=cpu<int>(0x529e80,&object);
  const auto et=trace;const auto esession=host.session;const auto enotice=at<void*>(0x5c5b38);std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));int eh[2][4];std::memcpy(eh,history,sizeof(history));int ex[16];std::memcpy(ex,object.cursor.excluded.data(),64);
  std::memcpy(&object,before.data(),sizeof(object));host.session=session_before;at<void*>(0x5c5b38)=notice_before;std::memcpy(history,history_before,sizeof(history));std::memcpy(object.cursor.excluded.data(),exclusions_before,64);trace.clear();const int ar=ti::update_main_menu(object,host);
  check("object",test,expected.data(),&object,sizeof(object));check("session",test,&esession,&host.session,sizeof(esession));check("history",test,eh,history,sizeof(history));check("excluded",test,ex,object.cursor.excluded.data(),64);const auto anotice=at<void*>(0x5c5b38);check("notice",test,&enotice,&anotice,4);check("return",test,&er,&ar,4);const auto es=et.size(),as=trace.size();check("trace_size",test,&es,&as,4);if(es==as)check("trace",test,et.data(),trace.data(),es*4);
 }
 #include "selection_compare.inl"
 #include "loadout_compare.inl"
 #include "replay_compare.inl"
 #include "rank_compare.inl"
 #include "save_compare.inl"
 #include "background_compare.inl"
 #include "music_compare.inl"
 #include "stage_compare.inl"
 #include "practice_compare.inl"
 #include "player_data_compare.inl"
 #include "stones_compare.inl"
 #include "frame_compare.inl"
 #include "lifecycle_compare.inl"
 std::ofstream out(argv[2]);out<<"{\"source_hashes\":";for(const auto* part:title_source_hashes)out<<part;out<<",\"original_sha256\":"<<th20::json_string(expected_sha)<<",\"passed\":"<<passed<<",\"failed\":"<<failed<<",\"failures\":[";for(unsigned i=0;i<failures.size();++i){if(i)out<<",";out<<th20::json_string(failures[i]);std::cerr<<failures[i]<<"\n";}out<<"]}\n";std::cout<<"passed="<<passed<<" failed="<<failed<<"\n";return failed?1:0;
 }catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 2;}}
