// Selected original functions only; the game entry and CRT are never started.
#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../score.hpp"
#include "../../program_entry/program_entry.hpp"
#include <cstdarg>
namespace ss=th20::source::small_score;namespace gs=th20::source::game_session;namespace state=th20::source::state;namespace sc=th20::source::scheduler;namespace sp=th20::source::sprite;
namespace th20::source::program_entry {sc::State* function_controller=nullptr;sc::Environment scheduler_environment;}
namespace th20::source::sprite {int execute_animation(Animation&){throw std::logic_error("Unexpected ANM execution outside SmallScore fixture boundary");}}
template<class R,class... A>R cpu(std::uint32_t va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
using Event=std::array<std::uint32_t,16>;
std::vector<Event> events;
std::array<std::uint8_t,0x1a360> text_storage{};
sp::AnimationFile file;
sp::SpriteData descriptor;
bool fog_option=false;
unsigned bits(float value){unsigned result;std::memcpy(&result,&value,4);return result;}
void record(unsigned kind,int a=0,int b=0){Event event{};event[0]=kind;event[1]=a;event[2]=b;events.push_back(event);}
void __fastcall select_boundary(void*,void*,int view){record(1,view);}
void __fastcall layer_boundary(void*,void*,int layer,int view){record(2,layer,view);}
void __fastcall fog_boundary(void*,void*){record(3);}
void __fastcall sprite_boundary(sp::Animation* animation,void*,int index){record(4,index);animation->base.fields_10_28[4]=index;}
sp::SpriteData* __fastcall descriptor_boundary(const sp::Animation* animation,void*){descriptor.extent_4c=float(animation->base.fields_10_28[4]%37)+.125f;return &descriptor;}
void __fastcall draw_boundary(void*,void*,const sp::Animation* animation){record(5,animation->base.fields_10_28[4]);auto& e=events.back();e[2]=bits(animation->vector_5bc.x);e[3]=bits(animation->vector_5bc.y);e[4]=bits(animation->vector_5bc.z);e[5]=bits(animation->base.vector_70.x);e[6]=animation->base.field_490;e[7]=animation->base.flags[1];}
void text_word(unsigned offset,unsigned value){std::memcpy(text_storage.data()+offset,&value,4);}
void __fastcall initialize_boundary(sp::AnimationFile* source,void*,sp::Animation* animation,int index){record(6,index);animation->base.fields_10_28[3]=source->id;animation->base.fields_10_28[4]=index;}
void text_event(ss::TextLine kind,const sp::Vec3& position,double multiplier,int value){record(7,static_cast<int>(kind));auto& e=events.back();e[2]=bits(position.x);e[3]=bits(position.y);e[4]=bits(position.z);std::memcpy(e.data()+5,&multiplier,8);e[7]=value;constexpr unsigned offsets[]{0x1a1c0,0x1a1e0,0x1a1ec,0x1a1f4,0x1a1f8};for(unsigned i=0;i<5;++i)std::memcpy(e.data()+8+i,text_storage.data()+offsets[i],4);}
void __cdecl text_boundary(void*,const sp::Vec3* position,const char* format,...){va_list args;va_start(args,format);if(!std::strcmp(format,"BONUS %.1f"))text_event(ss::TextLine::multiplier,*position,va_arg(args,double),0);else if(!std::strcmp(format,"%d"))text_event(ss::TextLine::integer,*position,0,va_arg(args,int));else if(!std::strcmp(format,"NO BONUS"))text_event(ss::TextLine::no_bonus,*position,0,0);else throw std::logic_error("Unexpected original SmallScore format");va_end(args);}
struct Fixture final:ss::Environment {
    sp::AnimationFile& text_file() override{return file;}
    void initialize_sprite(sp::AnimationFile& f,sp::Animation& a,int i) override{initialize_boundary(&f,nullptr,&a,i);}
    void select_view(int v) override{select_boundary(nullptr,nullptr,v);}
    void configure_layer(int l,int v) override{layer_boundary(nullptr,nullptr,l,v);}
    bool fog_configuration() override{return fog_option;}
    void disable_fog() override{fog_boundary(nullptr,nullptr);}
    void set_sprite(sp::Animation& a,int i) override{sprite_boundary(&a,nullptr,i);}
    float sprite_height(const sp::Animation& a) override{return descriptor_boundary(&a,nullptr)->extent_4c;}
    void draw_sprite(sp::Animation& a) override{draw_boundary(nullptr,nullptr,&a);}
    void text_vertical_alignment(unsigned v) override{text_word(0x1a1e0,v);}
    void text_horizontal_alignment(unsigned v) override{text_word(0x1a1ec,v);}
    void text_color(unsigned v) override{text_word(0x1a1c0,v);}
    void text_style(unsigned a,unsigned b) override{text_word(0x1a1f4,a);text_word(0x1a1f8,b);}
    void write_text(ss::TextLine kind,const sp::Vec3& p,float m,int v) override{text_event(kind,p,static_cast<double>(m),v);}
} host;
namespace th20::source::small_score {Environment& environment(){return host;}}
int wmain(int argc,wchar_t** argv){try{
    if(argc!=3)throw std::runtime_error("Usage: th20_small_score_cpu_compare ORIGINAL.exe REPORT.json");const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");const auto info=th20::parse_pe(bytes);Mapping image(bytes,info);mapped_image_base=image.address();
    AddVectoredExceptionHandler(1,[](PEXCEPTION_POINTERS p)->LONG{if(p->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION)std::cerr<<"access violation VA="<<std::hex<<p->ContextRecord->Eip-mapped_image_base+0x400000<<" address="<<p->ExceptionRecord->ExceptionInformation[1]<<std::dec<<std::endl;return EXCEPTION_CONTINUE_SEARCH;});
    for(const auto& import:info.imports)if(auto address=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),import.name.c_str()))*reinterpret_cast<FARPROC*>(mapped_image_base+import.iat_rva)=address;
    *reinterpret_cast<HANDLE*>(mapped_image_base+0x1e5990)=GetProcessHeap();cpu<void>(0x452e00,reinterpret_cast<void*>(mapped_image_base+0x1c0240));sc::State scheduler;sc::initialize_state(scheduler);th20::source::program_entry::function_controller=&scheduler;*reinterpret_cast<void**>(mapped_image_base+0x1b66d8)=&scheduler;
    auto hook=[&](unsigned va,void* target){auto* p=reinterpret_cast<std::uint8_t*>(mapped_image_base+va-0x400000);p[0]=0xe9;*reinterpret_cast<unsigned*>(p+1)=reinterpret_cast<unsigned>(target)-reinterpret_cast<unsigned>(p+5);FlushInstructionCache(GetCurrentProcess(),p,5);};
    hook(0x4776a0,reinterpret_cast<void*>(&select_boundary));hook(0x44f3d0,reinterpret_cast<void*>(&layer_boundary));hook(0x4dda60,reinterpret_cast<void*>(&fog_boundary));hook(0x470ed0,reinterpret_cast<void*>(&sprite_boundary));hook(0x437cd0,reinterpret_cast<void*>(&descriptor_boundary));hook(0x43f550,reinterpret_cast<void*>(&draw_boundary));hook(0x4708e0,reinterpret_cast<void*>(&initialize_boundary));hook(0x46c990,reinterpret_cast<void*>(&text_boundary));
    *reinterpret_cast<void**>(mapped_image_base+0x1c0698)=text_storage.data();*reinterpret_cast<void**>(text_storage.data()+0x1a244)=&file;file.id=17;
    std::mt19937 random(0x510710);unsigned passed=0,failed=0;std::vector<std::string> failures;
    auto check=[&](const char* name,unsigned test,const void* a,const void* b,std::size_t size){if(!std::memcmp(a,b,size)){++passed;return;}++failed;if(failures.size()<30){std::ostringstream out;out<<name<<" test="<<test;for(std::size_t i=0;i<size;++i)if(static_cast<const unsigned char*>(a)[i]!=static_cast<const unsigned char*>(b)[i]){out<<" +0x"<<std::hex<<i<<" expected="<<unsigned(static_cast<const unsigned char*>(a)[i])<<" actual="<<unsigned(static_cast<const unsigned char*>(b)[i]);break;}failures.push_back(out.str());}};
    auto check_events=[&](const char* name,unsigned test,const auto& expected){const auto ec=expected.size(),ac=events.size();check(name,test,&ec,&ac,sizeof(ec));if(ec==ac)check(name,test,expected.data(),events.data(),ac*sizeof(Event));};
    auto coordinate=[&]{return float(int(random()%10000)-5000)/17.f;};
    for(unsigned test=0;test<4096;++test){ss::Entry expected,actual;for(auto& byte:reinterpret_cast<std::array<std::uint8_t,sizeof(expected)>&>(expected))byte=static_cast<std::uint8_t>(random());actual=expected;cpu<void>(0x50fd50,&expected);ss::construct_entry(actual);check("entry_constructor",test,&expected,&actual,sizeof(actual));}
    auto* fixture=static_cast<ss::SmallScoreInf*>(VirtualAlloc(nullptr,sizeof(ss::SmallScoreInf),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));std::array<std::uint8_t,sizeof(ss::SmallScoreInf)> before,expected;
    for(unsigned test=0;test<128;++test){std::memset(fixture,test%2?0xa5:0,sizeof(*fixture));cpu<void>(0x50fdd0,fixture);std::memcpy(expected.data(),fixture,expected.size());std::memset(fixture,test%2?0xa5:0,sizeof(*fixture));::new(static_cast<void*>(fixture))ss::SmallScoreInf;std::memcpy(expected.data(),fixture,4);check("whole_constructor",test,expected.data(),fixture,sizeof(*fixture));}
    for(unsigned test=0;test<4096;++test){for(auto& byte:before)byte=static_cast<std::uint8_t>(random());std::memcpy(fixture,before.data(),before.size());fixture->next_slot=test%19;std::memcpy(before.data(),fixture,before.size());const sp::Vec3 position{coordinate(),coordinate(),coordinate()};const int value=test%13==0?0:static_cast<int>(random());const unsigned color=random();FloatingEnvironment::prepare();cpu<void>(0x510710,fixture,&position,value,color);std::memcpy(expected.data(),fixture,expected.size());std::memcpy(fixture,before.data(),before.size());FloatingEnvironment::prepare();ss::spawn(*fixture,position,value,color);check("spawn_whole_owner",test,expected.data(),fixture,sizeof(*fixture));}
    std::array<std::uint8_t,0x620> player{};gs::context(0).objects_04[0]=player.data();
    for(unsigned test=0;test<4096;++test){std::memset(fixture,0,sizeof(*fixture));::new(static_cast<void*>(fixture))ss::SmallScoreInf;fixture->select_context(0);fixture->view_index=test%2;const sp::Vec3 player_position{coordinate(),coordinate(),coordinate()};std::memcpy(player.data()+0x614,&player_position,12);
        for(unsigned i=0;i<18;++i){auto& entry=fixture->entries[i];for(auto& byte:reinterpret_cast<std::array<std::uint8_t,sizeof(entry)>&>(entry))byte=static_cast<std::uint8_t>(random());entry.active=(test+i)%3!=0;entry.position={coordinate(),coordinate(),coordinate()};entry.speed=coordinate()/30;entry.age={int(random()),int(test%76)-8,float(int(test%76)-8),random()};entry.length=(test+i)%11;for(auto& digit:entry.digits)digit=random()%11;entry.multiplier=coordinate();}
        state::clock_scale=float(test%8)/4;*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=state::clock_scale;std::memcpy(before.data(),fixture,before.size());events.clear();FloatingEnvironment::prepare();const int er=cpu<int>(0x50ff70,fixture);std::memcpy(expected.data(),fixture,expected.size());const auto ee=events;std::memcpy(fixture,before.data(),before.size());events.clear();FloatingEnvironment::prepare();const int ar=ss::update(*fixture,host);check("update_return",test,&er,&ar,4);check("update_whole_owner",test,expected.data(),fixture,sizeof(*fixture));check_events("update_events",test,ee);
        fog_option=test%2;*reinterpret_cast<unsigned*>(mapped_image_base+0x1c4d40+0x24c)=fog_option?4:0;std::memcpy(before.data(),fixture,before.size());const auto before_text=text_storage;events.clear();FloatingEnvironment::prepare();const int ed=cpu<int>(0x510110,fixture);std::memcpy(expected.data(),fixture,expected.size());const auto de=events;const auto et=text_storage;
        std::memcpy(fixture,before.data(),before.size());text_storage=before_text;events.clear();FloatingEnvironment::prepare();const int ad=ss::draw(*fixture,host);check("draw_return",test,&ed,&ad,4);check("draw_whole_owner",test,expected.data(),fixture,sizeof(*fixture));check("draw_text_state",test,et.data(),text_storage.data(),text_storage.size());check_events("draw_glyph_and_text_events",test,de);
    }
    for(unsigned test=0;test<128;++test){std::memset(fixture,0,sizeof(*fixture));::new(static_cast<void*>(fixture))ss::SmallScoreInf;std::memcpy(before.data(),fixture,before.size());const auto index=test%2;events.clear();const int er=cpu<int>(0x510640,fixture,index);std::memcpy(expected.data(),fixture,expected.size());sc::Node expected_update=*fixture->update_node,expected_draw=*fixture->draw_node;const auto expected_events=events;cpu<void>(0x50fea0,fixture);std::array<std::uint8_t,sizeof(*fixture)> shutdown;std::memcpy(shutdown.data(),fixture,shutdown.size());
        std::memcpy(fixture,before.data(),before.size());events.clear();const int ar=fixture->initialize(index,host);auto normalize=[&](auto& bytes){auto* object=reinterpret_cast<ss::SmallScoreInf*>(bytes.data());object->context=fixture->context;object->update_node=fixture->update_node;object->draw_node=fixture->draw_node;};normalize(expected);normalize(shutdown);expected_update.callback=fixture->update_node->callback;expected_update.link.value=fixture->update_node;expected_draw.callback=fixture->draw_node->callback;expected_draw.link.value=fixture->draw_node;
        check("initialize_return",test,&er,&ar,4);check("initialize_whole_owner",test,expected.data(),fixture,sizeof(*fixture));check("initialize_update_node",test,&expected_update,fixture->update_node,sizeof(sc::Node));check("initialize_draw_node",test,&expected_draw,fixture->draw_node,sizeof(sc::Node));check_events("initialize_script",test,expected_events);fixture->~SmallScoreInf();std::memcpy(shutdown.data(),fixture,4);check("destructor_whole_owner",test,shutdown.data(),fixture,sizeof(*fixture));const bool empty=scheduler.update.sentinel.next==nullptr&&scheduler.draw.sentinel.next==nullptr,yes=true;check("destructor_scheduler_empty",test,&yes,&empty,1);
    }
    std::ofstream report(argv[2],std::ios::binary);report<<"{\n  \"module\": \"small_score\",\n  \"original_sha256\": \""<<expected_sha<<"\",\n  \"passed\": "<<passed<<",\n  \"failed\": "<<failed<<",\n  \"failures\": [";for(std::size_t i=0;i<failures.size();++i){if(i)report<<',';report<<'\"'<<failures[i]<<'\"';}report<<"]\n}\n";std::cout<<"passed="<<passed<<" failed="<<failed<<'\n';for(auto& failure:failures)std::cerr<<failure<<'\n';return failed?1:0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 2;}}

