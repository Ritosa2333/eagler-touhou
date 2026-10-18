#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../pause.hpp"
#include "../menu_support.hpp"
namespace p=th20::source::pause;namespace gs=th20::source::game_session;namespace gp=th20::source::gameplay;namespace sp=th20::source::sprite;
template<class R,class... A>R cpu(unsigned va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
alignas(p::PauseInf) unsigned char pause_storage[sizeof(p::PauseInf)];auto& object=*reinterpret_cast<p::PauseInf*>(pause_storage);
alignas(gp::GameController) unsigned char game_storage[sizeof(gp::GameController)];auto& game=*reinterpret_cast<gp::GameController*>(game_storage);
std::vector<std::uint32_t> trace;gs::Session* session;int poll_count,dialogue,finished;const char music_name[]="stage.wav";
void event(unsigned id,unsigned arg=0){trace.insert(trace.end(),{id,arg});}
struct Host final:p::Services {
 gp::GameController& game()override{return ::game;}gs::Session& session()override{return *::session;}
 std::uint32_t& input_latch()override{return *reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1b8858);}
 std::uint32_t graphics_flags()override{return *reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c5888);}
 int& replay_selection()override{return *reinterpret_cast<int*>(mapped_image_base+0x1afcfc);}
 float clock_scale()override{return *reinterpret_cast<float*>(mapped_image_base+0x1aefe4);}
 void set_clock_scale(float value)override{*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=value;event(1,std::bit_cast<unsigned>(value));}
 bool pressed(unsigned mask)override{event(2,mask);return (*reinterpret_cast<unsigned*>(mapped_image_base+0x1b88c0)&mask)!=0;}
 void select_scene(int value,bool guarded)override{event(3,value);event(4,guarded);}
 void update_playtime()override{event(5);}void reset_playtime_origin()override{event(6);}
 sp::AnimationFile* hud_file()override{event(7);return reinterpret_cast<sp::AnimationFile*>(0x34567890);}
 unsigned spawn_panel(sp::AnimationFile&,int script)override{event(8,script);return 0x34560000u+script;}
 void delete_animation(unsigned& handle)override{event(9,handle);handle=0;}
 void interrupt_animation(unsigned& handle,int value)override{event(10,handle);event(11,value);}
 void capture_background(p::PauseInf& o)override{event(12);o.background_handle=0xabc123;}
 void capture_practice_background(p::PauseInf& o)override{event(13);o.background_handle=0xabc456;}
 void stop_effects()override{event(14);}void effect(int id)override{event(15,id);}void pause_music()override{event(16);}
 int poll_audio()override{event(17);return poll_count-->0;}
 const char* music_name()override{event(18);return ::music_name;}double music_position()override{event(19);return 12.125;}
 void play_game_over_music()override{event(20);}
 bool dialogue_present()override{event(21);return dialogue!=0;}void show_dialogue(bool visible)override{event(22,visible);}void show_hud_message(bool visible)override{event(23,visible);}void hide_hud_numbers()override{event(24);}
 bool replay_finished()override{event(25);return finished!=0;}void mark_replay_finished()override{event(26);finished=1;}
} host;
void __fastcall set_clock(void*,void*,float value){host.set_clock_scale(value);}int __fastcall pressed(void*,void*,unsigned mask){return host.pressed(mask);}
void __fastcall select_scene(void*,void*,int value){host.select_scene(value,true);}void __fastcall playtime(void*,void*){host.update_playtime();}void __fastcall reset_time(void*,void*){host.reset_playtime_origin();}
sp::AnimationFile* __fastcall hud_file(void*,void*){return host.hud_file();}
unsigned* __fastcall spawn(void* self,void*,unsigned* out,const char*,int script,const sp::Vec3*){*out=script==0x57?0xabc456:host.spawn_panel(*reinterpret_cast<sp::AnimationFile*>(self),script);return out;}
void __fastcall erase(unsigned* handle,void*){host.delete_animation(*handle);}void __fastcall interrupt1(unsigned* handle,void*){host.interrupt_animation(*handle,1);}void __fastcall interrupt3(unsigned* handle,void*){host.interrupt_animation(*handle,3);}
void __fastcall capture(p::PauseInf* o,void*){host.capture_background(*o);}
void __fastcall capture_practice(void*,void*,unsigned,int,int,int,int){event(13);}
void __fastcall stop(void*,void*){host.stop_effects();}void __fastcall effect(void*,void*,int id,int){host.effect(id);}void __fastcall pause_music(void*,void*){host.pause_music();}int __fastcall poll(void*,void*){return host.poll_audio();}
const char* __fastcall track(void*,void*){return host.music_name();}double __fastcall position(void*,void*){return host.music_position();}
void __cdecl queue_track(int,const char*){host.play_game_over_music();}void __cdecl play_track(int,int){} // native audio start is paired within observed event20
int __fastcall dialogue_query(void*,void*){return host.dialogue_present();}void* __fastcall dialogue_get(void*,void*){return reinterpret_cast<void*>(1);}
void __fastcall dialogue_hide(void*,void*){host.show_dialogue(false);}void __fastcall dialogue_show(void*,void*){host.show_dialogue(true);}void __fastcall hud_hide(void*,void*){host.show_hud_message(false);}void __fastcall hud_show(void*,void*){host.show_hud_message(true);}void __fastcall numbers_hide(void*,void*){host.hide_hud_numbers();}
int __fastcall replay_query(void*,void*){return host.replay_finished();}void __fastcall replay_mark(void*,void*){host.mark_replay_finished();}
int __cdecl copy_name(char* out,std::size_t size,const char* value){return strcpy_s(out,size,value);}
namespace th20::source::pause {void update_menu(PauseInf&,Services&){event(27);}}
void __fastcall menu_boundary(p::PauseInf*,void*){event(27);}
int wmain(int argc,wchar_t** argv){try{
 if(argc!=3)throw std::runtime_error("Usage: pause_compare ORIGINAL REPORT");const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");Mapping image(bytes,th20::parse_pe(bytes));mapped_image_base=image.address();session=reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568);
 auto hook=[&](unsigned va,void* target){auto* p=reinterpret_cast<unsigned char*>(mapped_image_base+va-0x400000);p[0]=0xe9;*reinterpret_cast<unsigned*>(p+1)=reinterpret_cast<unsigned>(target)-reinterpret_cast<unsigned>(p+5);FlushInstructionCache(GetCurrentProcess(),p,5);};
 const std::pair<unsigned,void*> hooks[]={{0x4292a0,&set_clock},{0x419c00,&pressed},{0x4a0fb0,&select_scene},{0x4bce10,&playtime},{0x4bd6e0,&reset_time},{0x4b8250,&hud_file},{0x49e080,&spawn},{0x44fcd0,&erase},{0x479040,&interrupt1},{0x486350,&interrupt3},{0x4e60c0,&capture},{0x4e6600,&capture_practice},{0x4ba440,&stop},{0x426d70,&effect},{0x4e60a0,&pause_music},{0x4277f0,&poll},{0x4b8090,&track},{0x4e6710,&position},{0x4d9a70,&queue_track},{0x4d9b50,&play_track},{0x478160,&dialogue_query},{0x4b8070,&dialogue_get},{0x4b8b30,&dialogue_hide},{0x4b8880,&dialogue_show},{0x4b8810,&hud_hide},{0x4b87f0,&hud_show},{0x4e6b30,&numbers_hide},{0x4e6790,&replay_query},{0x4e69d0,&replay_mark},{0x548610,&copy_name},{0x4e2260,&menu_boundary}};
 for(auto [va,target]:hooks)hook(va,target);
 new(&object.cursor)th20::source::menu::Cursor;new(&object.name_cursor)th20::source::menu::Cursor;
 *reinterpret_cast<void**>(mapped_image_base+0x1ba828)=&game;*reinterpret_cast<void**>(mapped_image_base+0x1b889c)=reinterpret_cast<void*>(mapped_image_base+0x1b88b0);
 std::mt19937 random(0x4e5bd0);unsigned passed=0,failed=0;std::vector<std::string> failures;
 auto check=[&](const char* label,unsigned test,const void* e,const void* a,std::size_t size){if(!std::memcmp(e,a,size)){++passed;return;}++failed;if(failures.size()<24){std::ostringstream out;out<<label<<" test="<<test;for(std::size_t i=0;i<size;++i)if(static_cast<const unsigned char*>(e)[i]!=static_cast<const unsigned char*>(a)[i]){out<<" offset="<<std::hex<<i<<" expected="<<unsigned(static_cast<const unsigned char*>(e)[i])<<" actual="<<unsigned(static_cast<const unsigned char*>(a)[i]);break;}failures.push_back(out.str());}};
 th20::source::scheduler::Node node{};
 for(unsigned test=0;test<14336;++test){
  for(unsigned i=0;i<sizeof(object);++i)if(i<0x30||i>=0xc8)pause_storage[i]=std::uint8_t(random());
  for(auto& b:game_storage)b=std::uint8_t(random());game.update_node=(test%2)?&node:nullptr;node.flags=random();game.restart_mode=(test/6)%3;session->mode=(test/18)%4;session->flags=random();object.age.flags&=3;object.secondary_age.flags&=3;object.saved_clock_scale=float(int(test%9)-4)/4.f;object.state=(test/72)%5;object.cursor.excluded.clear();
  const float scale=float((test%7)+1)/4.f;std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1aefe4),&scale,4);host.input_latch()=random();host.replay_selection()=int(test%3)-1;*reinterpret_cast<unsigned*>(mapped_image_base+0x1b88c0)=(test%2)?0x100:0;
  poll_count=test%4;dialogue=(test/4)%2;finished=(test/8)%2;const auto initial_poll=poll_count,initial_finished=finished;const auto initial_input=host.input_latch();const auto initial_selection=host.replay_selection();
  std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));std::array<unsigned char,sizeof(game)> game_before;std::memcpy(game_before.data(),&game,sizeof(game));trace.clear();
  const unsigned operation=test%7;const unsigned addresses[]{0x4e5d60,0x4e5bd0,0x4e5f30,0x4e58f0,0x4e58a0,0x4e20d0,0x4e59e0};cpu<void>(addresses[operation],&object);
  const auto expected_trace=trace;const auto expected_input=host.input_latch();const auto expected_selection=host.replay_selection();const auto expected_scale=host.clock_scale();const auto expected_finished=finished;
  std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));std::array<unsigned char,sizeof(game)> expected_game;std::memcpy(expected_game.data(),&game,sizeof(game));
  std::memcpy(&object,before.data(),sizeof(object));std::memcpy(&game,game_before.data(),sizeof(game));*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=scale;host.input_latch()=initial_input;host.replay_selection()=initial_selection;poll_count=initial_poll;finished=initial_finished;trace.clear();
  switch(operation){case 0:p::open_pause(object,host);break;case 1:p::finish_game(object,host);break;case 2:p::finish_replay(object,host);break;case 3:p::restore_after_pause(object,host);break;case 4:p::restore_after_result(object,host);break;case 5:p::update(object,host);break;case 6:p::finish_practice(object,host);break;}
  check("object",test,expected.data(),&object,sizeof(object));check("game",test,expected_game.data(),&game,sizeof(game));check("input",test,&expected_input,&host.input_latch(),4);check("selection",test,&expected_selection,&host.replay_selection(),4);const auto actual_scale=host.clock_scale();check("scale",test,&expected_scale,&actual_scale,4);check("replay_finished",test,&expected_finished,&finished,4);const auto es=expected_trace.size(),as=trace.size();check("trace_size",test,&es,&as,4);if(es==as)check("trace",test,expected_trace.data(),trace.data(),es*4);
 }
 std::ofstream out(argv[2]);out<<"{\"original_sha256\":"<<th20::json_string(expected_sha)<<",\"passed\":"<<passed<<",\"failed\":"<<failed<<",\"failures\":[";for(unsigned i=0;i<failures.size();++i){if(i)out<<",";out<<th20::json_string(failures[i]);std::cerr<<failures[i]<<"\n";}out<<"]}\n";std::cout<<"passed="<<passed<<" failed="<<failed<<"\n";return failed?1:0;
 }catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 2;}}
