#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../key_config.hpp"
namespace kc=th20::source::key_config;namespace in=th20::source::input;namespace sp=th20::source::sprite;
template<class R,class... A>R cpu(unsigned va,void* self,A... args){return reinterpret_cast<R(__thiscall*)(void*,A...)>(mapped_image_base+va-0x400000)(self,args...);}
template<class T>T& at(unsigned va){return *reinterpret_cast<T*>(mapped_image_base+va-0x400000);}
alignas(kc::KeyConfigInf) unsigned char storage[sizeof(kc::KeyConfigInf)];auto& object=*reinterpret_cast<kc::KeyConfigInf*>(storage);
alignas(in::Controller) unsigned char input_bytes[sizeof(in::Controller)];auto& input=*reinterpret_cast<in::Controller*>(input_bytes);
std::vector<unsigned> trace;void event(unsigned id,unsigned value=0){trace.insert(trace.end(),{id,value});}
struct Host final:kc::Environment {
 Host():Environment(::input,at<th20::source::platform::Configuration>(0x5c4f08),at<th20::source::platform::KeyBindings>(0x5b9eb0),&at<float>(0x5aefe4)){}
 bool pressed(int slot,unsigned bits)override{if(slot<0)return false;const auto* b=at<in::ButtonState*>(0x5b889c+slot*4);return b&&(b->pressed&bits)!=0;}
 bool repeated(int slot,unsigned bits)override{if(slot<0)return false;const auto* b=at<in::ButtonState*>(0x5b889c+slot*4);return b&&((b->pressed|b->repeat8)&bits)!=0;}
 void play_effect(int id)override{event(1,id);}void rebuild_devices()override{event(2);}void retire(kc::KeyConfigInf&)override{event(3);}
};
void __fastcall effect(void*,void*,int id,int){event(1,id);}void __fastcall rebuild(void*,void*){event(2);}void __fastcall retire(void*,void*){event(3);}
int wmain(int argc,wchar_t** argv){try{
 if(argc!=3)throw std::runtime_error("Usage: key_config_compare ORIGINAL REPORT");auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");Mapping image(bytes,th20::parse_pe(bytes));mapped_image_base=image.address();
 AddVectoredExceptionHandler(1,[](PEXCEPTION_POINTERS p)->LONG{if(p->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION)std::cerr<<"ip="<<std::hex<<p->ContextRecord->Eip-mapped_image_base+0x400000<<" address="<<p->ExceptionRecord->ExceptionInformation[1]<<std::endl;return EXCEPTION_CONTINUE_SEARCH;});
 auto hook=[&](unsigned va,void* target){auto* p=reinterpret_cast<unsigned char*>(mapped_image_base+va-0x400000);p[0]=0xe9;*reinterpret_cast<unsigned*>(p+1)=reinterpret_cast<unsigned>(target)-reinterpret_cast<unsigned>(p+5);};hook(0x426d70,&effect);hook(0x420d80,&rebuild);hook(0x4217c0,&retire);
 new(&object.cursor)th20::source::menu::Cursor;
 int history[2][4]{};int* blocks[2][8]{};blocks[0][0]=history[0];blocks[1][0]=history[1];
 for(unsigned i=0;i<2;++i){auto& d=i?object.cursor.secondary_history:object.cursor.history;d.blocks=blocks[i];d.block_count=8;d.first=0;d.size=0;}
 in::ButtonState buttons[4]{};for(unsigned i=0;i<4;++i)at<void*>(0x5b889c+i*4)=&buttons[i];at<void*>(0x5b8898)=&input;Host host;
 std::mt19937 random(0x4c6230);unsigned passed=0,failed=0;std::vector<std::string> failures;
 auto check=[&](const char* label,unsigned test,const void* e,const void* a,std::size_t size){if(!std::memcmp(e,a,size)){++passed;return;}++failed;if(failures.size()<30){std::ostringstream out;out<<label<<" case="<<test;for(std::size_t i=0;i<size;++i)if(static_cast<const unsigned char*>(e)[i]!=static_cast<const unsigned char*>(a)[i]){out<<" offset="<<std::hex<<i<<" expected="<<unsigned(static_cast<const unsigned char*>(e)[i])<<" actual="<<unsigned(static_cast<const unsigned char*>(a)[i]);break;}failures.push_back(out.str());}};
 for(unsigned test=0;test<32768;++test){
  for(unsigned i=0;i<sizeof(object);++i)if(i<0x14||i>=0x60)storage[i]=std::uint8_t(random());
  for(auto& v:input_bytes)v=std::uint8_t(random());input.device_count=3;input.selected[0]=random()%3;input.selected[1]=random()%3;for(unsigned i=0;i<3;++i){input.devices[i].kind=i;std::memset(input.devices[i].raw,0,256);for(unsigned k=0;k<4;++k)input.devices[i].raw[random()%256]=0x80;}
  object.selected_slot=0;object.selected_devices[0]=input.selected[0];object.selected_devices[1]=-1;object.state=test%5;object.phase=(test/5)%5;object.refresh_devices=test%2;
  object.cursor.count=object.state==2?(input.devices[input.selected[0]].kind==0?9:6):3;object.cursor.current=random()%object.cursor.count;object.cursor.previous=random();object.cursor.minimum=0;object.cursor.wrapping=1;object.cursor.excluded.clear();object.cursor.history.size=object.cursor.secondary_history.size=test%2;object.cursor.history.first=object.cursor.secondary_history.first=0;history[0][0]=0;history[1][0]=3;
  th20::recovered::timer_set(object.age,int(random()%35)-1);th20::recovered::timer_set(object.transition_age,int(random()%30));th20::recovered::timer_set(object.selection_age,int(random()%9));at<float>(0x5aefe4)=float(random()%9)/4.f;
  for(auto& b:buttons){b.pressed=random()&0x801ff;b.repeat8=random()&0xf0;}for(auto& v:at<th20::source::platform::KeyBindings>(0x5b9eb0).keyboard)v=std::uint16_t(random());
  std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));std::array<unsigned char,sizeof(input)> input_before;std::memcpy(input_before.data(),&input,sizeof(input));const auto c_before=host.configuration;int h_before[2][4];std::memcpy(h_before,history,sizeof(history));trace.clear();const int er=cpu<int>(0x4c5790,&object);
  const auto et=trace;const auto ec=host.configuration;std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));std::array<unsigned char,sizeof(input)> expected_input;std::memcpy(expected_input.data(),&input,sizeof(input));int eh[2][4];std::memcpy(eh,history,sizeof(history));
  std::memcpy(&object,before.data(),sizeof(object));std::memcpy(&input,input_before.data(),sizeof(input));std::memcpy(history,h_before,sizeof(history));host.configuration=c_before;trace.clear();const int ar=kc::update(object,host);
  check("object",test,expected.data(),&object,sizeof(object));check("input",test,expected_input.data(),&input,sizeof(input));check("configuration",test,&ec,&host.configuration,sizeof(ec));check("cursor_history",test,eh,history,sizeof(history));check("return",test,&er,&ar,4);const auto es=et.size(),as=trace.size();check("trace_size",test,&es,&as,4);if(es==as)check("trace",test,et.data(),trace.data(),es*4);
 }
 std::ofstream out(argv[2]);out<<"{\"original_sha256\":"<<th20::json_string(expected_sha)<<",\"passed\":"<<passed<<",\"failed\":"<<failed<<",\"failures\":[";for(unsigned i=0;i<failures.size();++i){if(i)out<<",";out<<th20::json_string(failures[i]);std::cerr<<failures[i]<<"\n";}out<<"]}\n";std::cout<<"passed="<<passed<<" failed="<<failed<<"\n";return failed?1:0;
 }catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 2;}}
