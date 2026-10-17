#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../options.hpp"
#include "../../audio_runtime/audio.hpp"
#include "../../text_renderer/menu_style.hpp"
namespace op=th20::source::options;namespace tx=th20::source::text;namespace sp=th20::source::sprite;namespace au=th20::source::audio;
template<class R,class... A>R cpu(unsigned va,void* self,A... args){return reinterpret_cast<R(__thiscall*)(void*,A...)>(mapped_image_base+va-0x400000)(self,args...);}
template<class T>T& at(unsigned va){return *reinterpret_cast<T*>(mapped_image_base+va-0x400000);}
std::vector<unsigned> trace;void event(unsigned id,unsigned a=0){trace.insert(trace.end(),{id,a});}
alignas(op::OptionInf) unsigned char storage[sizeof(op::OptionInf)];auto& object=*reinterpret_cast<op::OptionInf*>(storage);
alignas(au::SoundInf) unsigned char sound_bytes[sizeof(au::SoundInf)];auto& sound=*reinterpret_cast<au::SoundInf*>(sound_bytes);
namespace th20::source::audio {void SoundInf::enqueue(int type,int value,const char* name){event(type+100,value);event(200,unsigned(std::strcmp(name,"SetVol")));}}
struct Host final:op::Environment {
 Host():Environment(at<th20::source::platform::Configuration>(0x5c4f08),at<int>(0x5b87e4),&at<float>(0x5aefe4)){}
 bool key_config_active()override{return at<unsigned>(0x5c4d28)!=0;}
 bool pressed(unsigned mask)override{return (at<unsigned>(0x5b88c0)&mask)!=0;}
 bool repeated(unsigned mask)override{return ((at<unsigned>(0x5b88b8)|at<unsigned>(0x5b88c0))&mask)!=0;}
 void play_effect(int id)override{event(1,id);}void screen_change_effect()override{event(2);}
 void apply_volume()override{event(3);}void reset_device()override{event(4);}
 void create_key_config(const sp::Vec3& p)override{event(5,std::bit_cast<unsigned>(p.x));event(6,std::bit_cast<unsigned>(p.y));event(7,std::bit_cast<unsigned>(p.z));}
 void save_configuration()override{event(8);}void retire(op::OptionInf&)override{event(9);}
};
void __fastcall effect(void*,void*,int id,int){event(1,id);}void* __cdecl fade(int,int,int,int,int){event(2);return nullptr;}
void __fastcall apply(void*,void*){event(3);}void __fastcall reset(void*,void*,unsigned){event(4);}
void* __cdecl key_config(const sp::Vec3* p){event(5,std::bit_cast<unsigned>(p->x));event(6,std::bit_cast<unsigned>(p->y));event(7,std::bit_cast<unsigned>(p->z));return nullptr;}
void __fastcall save(void*,void*){event(8);}void __fastcall retire(void*,void*){event(9);}
void __fastcall enqueue(void*,void*,int type,int value,const char* name){sound.enqueue(type,value,name);}
int wmain(int argc,wchar_t** argv){try{
 if(argc!=3)throw std::runtime_error("Usage: options_compare ORIGINAL REPORT");auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");Mapping image(bytes,th20::parse_pe(bytes));mapped_image_base=image.address();
 auto hook=[&](unsigned va,void* target){auto* p=reinterpret_cast<unsigned char*>(mapped_image_base+va-0x400000);p[0]=0xe9;*reinterpret_cast<unsigned*>(p+1)=reinterpret_cast<unsigned>(target)-reinterpret_cast<unsigned>(p+5);};
 const std::pair<unsigned,void*> hooks[]={{0x426d70,&effect},{0x425090,&fade},{0x4e0fc0,&apply},{0x41de00,&reset},{0x4c7cb0,&key_config},{0x41aa70,&save},{0x4217c0,&retire},{0x428c90,&enqueue}};
 std::array<unsigned char,5> volume_code;std::memcpy(volume_code.data(),reinterpret_cast<void*>(mapped_image_base+0xe0fc0),5);
 for(auto [va,target]:hooks)hook(va,target);
 at<void*>(0x5b88a4)=reinterpret_cast<void*>(mapped_image_base+0x1b88b0);new(&object.cursor)th20::source::menu::Cursor;Host host;
 std::mt19937 random(0x4dfe00);unsigned passed=0,failed=0;std::vector<std::string> failures;
 auto check=[&](const char* label,unsigned test,const void* e,const void* a,std::size_t size){if(!std::memcmp(e,a,size)){++passed;return;}++failed;if(failures.size()<30){std::ostringstream out;out<<label<<" case="<<test;for(std::size_t i=0;i<size;++i)if(static_cast<const unsigned char*>(e)[i]!=static_cast<const unsigned char*>(a)[i]){out<<" offset="<<std::hex<<i<<" expected="<<unsigned(static_cast<const unsigned char*>(e)[i])<<" actual="<<unsigned(static_cast<const unsigned char*>(a)[i]);break;}failures.push_back(out.str());}};
 for(unsigned test=0;test<32768;++test){
  for(unsigned i=0;i<sizeof(object);++i)if(i<0x14||i>=0x60)storage[i]=std::uint8_t(random());
  object.cursor.current=random()%6;object.cursor.previous=int(random());object.cursor.count=6;object.cursor.minimum=0;object.cursor.wrapping=1;object.cursor.excluded.clear();object.state=test%7;
  th20::recovered::timer_set(object.age,int(random()%86)-4);th20::recovered::timer_set(object.selection_age,int(random()%10));th20::recovered::timer_set(object.key_config_age,int(random()%32));
  object.allow_escape=test%2;at<unsigned>(0x5c4d28)=test%31==0?1:0;at<float>(0x5aefe4)=float(random()%9)/4.f;
  at<unsigned>(0x5b88c0)=random()&0x801ff;at<unsigned>(0x5b88b8)=random()&0xf0;host.configuration.value_7e=std::uint8_t(random());host.configuration.value_7f=std::uint8_t(random());host.configuration.scale_choice=random()%7;host.display_mode=int(random()%12)-1;
  std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));const auto c_before=host.configuration;const auto m_before=host.display_mode;trace.clear();int er=cpu<int>(0x4dfe00,&object);
  const auto et=trace;const auto ec=host.configuration;const auto em=host.display_mode;std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));
  std::memcpy(&object,before.data(),sizeof(object));host.configuration=c_before;host.display_mode=m_before;trace.clear();int ar=op::update(object,host);
  check("update_object",test,expected.data(),&object,sizeof(object));check("configuration",test,&ec,&host.configuration,sizeof(ec));check("display_mode",test,&em,&host.display_mode,4);check("return",test,&er,&ar,4);const auto es=et.size(),as=trace.size();check("trace_size",test,&es,&as,4);if(es==as)check("trace",test,et.data(),trace.data(),es*4);
 }
 // Full original 4e0fc0 body; the asynchronous command enqueue is an explicit paired boundary.
 std::memcpy(reinterpret_cast<void*>(mapped_image_base+0xe0fc0),volume_code.data(),5);
 for(unsigned test=0;test<65536;++test){
  host.configuration.value_7e=std::uint8_t(test);host.configuration.value_7f=std::uint8_t(test>>8);sound.music_level=at<int>(0x5c000c)=11;sound.effect_level=at<int>(0x5c0010)=22;sound.retained_57e4=at<unsigned>(0x5c0014)=33;
  trace.clear();cpu<void>(0x4e0fc0,&object);const auto et=trace;int expected[3]={at<int>(0x5c000c),at<int>(0x5c0010),at<int>(0x5c0014)};
  trace.clear();op::apply_volume(host.configuration,sound);check("volume",test,expected,&sound.music_level,12);const auto es=et.size(),as=trace.size();check("volume_trace_size",test,&es,&as,4);if(es==as)check("volume_trace",test,et.data(),trace.data(),es*4);
 }
 std::ofstream out(argv[2]);out<<"{\"original_sha256\":"<<th20::json_string(expected_sha)<<",\"passed\":"<<passed<<",\"failed\":"<<failed<<",\"failures\":[";for(unsigned i=0;i<failures.size();++i){if(i)out<<",";out<<th20::json_string(failures[i]);std::cerr<<failures[i]<<"\n";}out<<"]}\n";std::cout<<"passed="<<passed<<" failed="<<failed<<"\n";return failed?1:0;
 }catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 2;}}

