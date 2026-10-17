// Selected original functions only, without starting the game/CRT entry.
#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../stone.hpp"
#include "../draw.hpp"
#include "../../sprite_renderer/menu_animation.hpp"
#include "../../program_entry/program_entry.hpp"
#include <map>
namespace sm=th20::source::stone_menu;namespace menu=th20::source::menu;namespace sc=th20::source::scheduler;namespace sp=th20::source::sprite;
namespace th20::source::program_entry {sc::State* function_controller=nullptr;sc::Environment scheduler_environment;}
template<class R,class... A>R cpu(std::uint32_t va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
using Event=std::array<unsigned,3>;std::vector<Event> events;sp::AnimationFile file;std::string resource;bool missing_file=false;
void __fastcall select_boundary(void*,void*,int index){events.push_back({1,unsigned(index),0});}
sp::AnimationFile* __fastcall load_boundary(void*,void*,int index,const char* name){if(std::strcmp(name,"stone.anm"))throw std::logic_error("unexpected ANM resource");events.push_back({2,unsigned(index),0});return missing_file?nullptr:&file;}
void __cdecl error_boundary(void*,const char*){events.push_back({3,0,0});}
void __fastcall interrupt_boundary(std::uint32_t* handle,void*,int event){events.push_back({4,*handle,unsigned(event)});}
void __fastcall delete_boundary(std::uint32_t* handle,void*){events.push_back({5,*handle,0});*handle=0;}
void __fastcall unload_boundary(void*,void*,int index){events.push_back({6,unsigned(index),0});}
void* __cdecl resource_boundary(const char* name,void*,int){if(std::strcmp(name,"stonetext.txt"))throw std::logic_error("unexpected text resource");events.push_back({7,0,0});auto* result=HeapAlloc(GetProcessHeap(),0,resource.size()+1);std::memcpy(result,resource.c_str(),resource.size()+1);return result;}
void __fastcall resource_free_boundary(void*,void*,void* allocation){HeapFree(GetProcessHeap(),0,allocation);}
struct Fixture final:sm::Environment {
    void select_view(int index) override{select_boundary(nullptr,nullptr,index);}
    sp::AnimationFile* load_animation(int index,const char* name) override{return load_boundary(nullptr,nullptr,index,name);}
    std::string text_resource(const char*) override{events.push_back({7,0,0});return resource;}
    void load_error() override{error_boundary(nullptr,nullptr);}
    void interrupt(std::uint32_t handle,int event) override{interrupt_boundary(&handle,nullptr,event);}
    void request_delete(std::uint32_t& handle) override{delete_boundary(&handle,nullptr);}
    void unload_animation(int index) override{unload_boundary(nullptr,nullptr,index);}
} host;
namespace th20::source::stone_menu {
Environment& environment(){return host;}
}
// The original text parser's standard stringstream operations use the host's
// real C++ library. String erase/remove, label search, branches, fixed-buffer
// writes, and all game state stay in the selected original body.
std::map<void*,std::unique_ptr<std::istringstream>> streams;
unsigned stream_vtable[2]{};
void* __fastcall stream_construct(void* self,void*,const std::pmr::string* content,unsigned,int){streams[self]=std::make_unique<std::istringstream>(std::string(content->data(),content->size()));*static_cast<unsigned**>(self)=stream_vtable;return self;}
void* __cdecl stream_getline(void* self,std::pmr::string* line){std::string value;std::getline(*streams.at(self),value);line->assign(value.data(),value.size());return self;}
bool __fastcall stream_good(void* self,void*){return !streams.at(self)->fail();}
void __fastcall stream_destroy(void* self,void*){streams.erase(self);}
namespace {
std::array<std::uint8_t,0x1a360> text_storage;
using DrawEvent=std::array<unsigned,16>;std::vector<DrawEvent> draw_events;
unsigned selected[4],available[9],used[9];bool unlocked[9];int character=0,difficulty=0;float scale=1;sp::Vec3 origin;
unsigned bits(float value){unsigned result;std::memcpy(&result,&value,4);return result;}
void text_word(unsigned offset,unsigned value){std::memcpy(text_storage.data()+offset,&value,4);}
int __fastcall selection_boundary(void*,void*,int slot,int){return selected[slot];}
unsigned __fastcall available_boundary(void*,void*,unsigned index){return available[index];}
unsigned __fastcall used_boundary(void*,void*,unsigned index){return used[index];}
bool __fastcall unlocked_boundary(void*,void*,int,unsigned index){return unlocked[index];}
void __fastcall origin_boundary(unsigned* handle,void*,sp::Vec3* out){*out=origin;DrawEvent e{};e[0]=1;e[1]=*handle;draw_events.push_back(e);}
void __fastcall position_boundary(unsigned* handle,void*,const sp::Vec3* value){DrawEvent e{};e[0]=2;e[1]=*handle;e[2]=bits(value->x);e[3]=bits(value->y);e[4]=bits(value->z);draw_events.push_back(e);}
void __fastcall scale_boundary(unsigned* handle,void*,float x,float y){DrawEvent e{};e[0]=3;e[1]=*handle;e[2]=bits(x);e[3]=bits(y);draw_events.push_back(e);}
void __cdecl text_boundary(void*,const sp::Vec3* position,const char* content,unsigned){DrawEvent e{};e[0]=4;e[1]=bits(position->x);e[2]=bits(position->y);e[3]=bits(position->z);unsigned hash=2166136261;for(auto* at=reinterpret_cast<const unsigned char*>(content);*at;++at){hash=(hash^*at)*16777619;}e[4]=hash;constexpr unsigned offsets[]{0x1a1bc,0x1a1c0,0x1a1c4,0x1a1c8,0x1a1e4,0x1a1f4,0x1a1f8};for(unsigned i=0;i<std::size(offsets);++i)std::memcpy(e.data()+5+i,text_storage.data()+offsets[i],4);draw_events.push_back(e);}
struct DrawFixture final:sm::DrawEnvironment {
    int character() override{return ::character;}int difficulty() override{return ::difficulty;}
    int selected_profile(int slot) override{return selected[slot];}unsigned stone_count(unsigned i) override{return available[i];}unsigned used_stone_count(unsigned i) override{return used[i];}bool extra_unlocked(unsigned i) override{return unlocked[i];}
    float screen_scale() override{return scale;}
    sp::Vec3 animation_position(unsigned& h) override{sp::Vec3 p;origin_boundary(&h,nullptr,&p);return p;}
    void set_animation_position(unsigned h,const sp::Vec3& p) override{position_boundary(&h,nullptr,&p);}
    void set_animation_scale(unsigned h,float x,float y) override{scale_boundary(&h,nullptr,x,y);}
    void text_style(unsigned x,unsigned y) override{text_word(0x1a1f4,x);text_word(0x1a1f8,y);}
    void text_field(sm::TextField f,unsigned value) override{text_word(static_cast<unsigned>(f),value);}
    void clear_ascii_lines() override{text_word(0x1a1bc,0);}
    void write_text(const sp::Vec3& p,const char* value) override{text_boundary(nullptr,&p,value,static_cast<unsigned>(std::strlen(value)));}
} draw_host;
}
namespace th20::source::stone_menu {DrawEnvironment& draw_environment(){return draw_host;}}
sp::SpriteData sprite_descriptor;sp::Animation* active_animation=nullptr;std::vector<unsigned> immediate_events;
bool frame_lookup=false;std::array<sp::Animation,256> frame_animations;unsigned frame_next=16;
sp::SpriteData* __fastcall descriptor_boundary(void*,void*,int){return &sprite_descriptor;}
sp::Animation* __cdecl animation_boundary(unsigned handle){return frame_lookup?(handle>0&&handle<frame_animations.size()?&frame_animations[handle]:nullptr):(handle?active_animation:nullptr);}
int __fastcall execute_boundary(sp::Animation* animation,void*){immediate_events.push_back(animation->index);animation->base.field_43c+=animation->base.field_438;return 0;}
namespace th20::source::sprite {
SpriteData& current_sprite(Controller&,const Animation&){return sprite_descriptor;}
Animation* find_animation(Controller&,unsigned handle) noexcept{return animation_boundary(handle);}
Animation* resolve_animation_handle(Controller&,unsigned& handle) noexcept{auto* value=animation_boundary(handle);if(!value)handle=0;return value;}
int execute_animation(Animation& animation){return execute_boundary(&animation,nullptr);}
}
#include "update_fixture.hpp"
int wmain(int argc,wchar_t** argv){try{
    if(argc!=4)throw std::runtime_error("Usage: th20_stone_menu_cpu_compare ORIGINAL.exe stonetext.txt REPORT.json");const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");const auto info=th20::parse_pe(bytes);Mapping image(bytes,info);mapped_image_base=image.address();
    AddVectoredExceptionHandler(1,[](PEXCEPTION_POINTERS p)->LONG{if(p->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION)std::cerr<<"access violation VA="<<std::hex<<p->ContextRecord->Eip-mapped_image_base+0x400000<<" address="<<p->ExceptionRecord->ExceptionInformation[1]<<std::dec<<std::endl;return EXCEPTION_CONTINUE_SEARCH;});
    for(const auto& import:info.imports)if(auto address=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),import.name.c_str()))*reinterpret_cast<FARPROC*>(mapped_image_base+import.iat_rva)=address;
    *reinterpret_cast<HANDLE*>(mapped_image_base+0x1e5990)=GetProcessHeap();cpu<void>(0x452e00,reinterpret_cast<void*>(mapped_image_base+0x1c0240));sc::State scheduler;sc::initialize_state(scheduler);th20::source::program_entry::function_controller=&scheduler;*reinterpret_cast<void**>(mapped_image_base+0x1b66d8)=&scheduler;
    auto hook=[&](unsigned va,void* target){auto* p=reinterpret_cast<std::uint8_t*>(mapped_image_base+va-0x400000);p[0]=0xe9;*reinterpret_cast<unsigned*>(p+1)=reinterpret_cast<unsigned>(target)-reinterpret_cast<unsigned>(p+5);FlushInstructionCache(GetCurrentProcess(),p,5);};
    hook(0x4776a0,reinterpret_cast<void*>(&select_boundary));hook(0x44ee50,reinterpret_cast<void*>(&load_boundary));hook(0x454150,reinterpret_cast<void*>(&error_boundary));hook(0x44ef90,reinterpret_cast<void*>(&interrupt_boundary));hook(0x44fcd0,reinterpret_cast<void*>(&delete_boundary));hook(0x44c430,reinterpret_cast<void*>(&unload_boundary));hook(0x410aa0,reinterpret_cast<void*>(&resource_boundary));hook(0x41f670,reinterpret_cast<void*>(&resource_free_boundary));
    hook(0x5142b0,reinterpret_cast<void*>(&stream_construct));hook(0x515500,reinterpret_cast<void*>(&stream_getline));hook(0x516420,reinterpret_cast<void*>(&stream_good));hook(0x516530,reinterpret_cast<void*>(&stream_destroy));
    std::mt19937 random(0x519960);unsigned passed=0,failed=0;std::vector<std::string> failures;
    auto check=[&](const char* name,unsigned test,const void* a,const void* b,std::size_t size){if(!std::memcmp(a,b,size)){++passed;return;}++failed;if(failures.size()<30){std::ostringstream out;out<<name<<" test="<<test;for(std::size_t i=0;i<size;++i)if(static_cast<const unsigned char*>(a)[i]!=static_cast<const unsigned char*>(b)[i]){out<<" +0x"<<std::hex<<i<<" expected="<<unsigned(static_cast<const unsigned char*>(a)[i])<<" actual="<<unsigned(static_cast<const unsigned char*>(b)[i]);break;}failures.push_back(out.str());}};
    auto check_events=[&](const char* name,unsigned test,const auto& expected){const auto ec=expected.size(),ac=events.size();check(name,test,&ec,&ac,sizeof(ec));if(ec==ac)check(name,test,expected.data(),events.data(),ac*sizeof(Event));};
    std::cerr<<"cursor constructors\n";
    alignas(menu::Cursor) std::array<std::uint8_t,sizeof(menu::Cursor)> raw;std::array<std::uint8_t,sizeof(menu::Cursor)> expected_cursor;
    auto normalize_cursor=[&](void* expected,menu::Cursor& actual){auto* p=static_cast<std::uint8_t*>(expected);std::memcpy(p+0x10,&actual.excluded,4);std::memcpy(p+0x20,&actual.history.proxy,4);std::memcpy(p+0x34,&actual.secondary_history.proxy,4);};
    for(unsigned test=0;test<128;++test){std::memset(raw.data(),test%2?0xa5:0,raw.size());auto& cursor=*reinterpret_cast<menu::Cursor*>(raw.data());cpu<void>(0x4aef00,&cursor);expected_cursor=raw;const bool valid=cursor.history.proxy&&cursor.history.proxy->container==&cursor.history&&!cursor.history.proxy->first_iterator&&cursor.secondary_history.proxy&&cursor.secondary_history.proxy->container==&cursor.secondary_history&&!cursor.secondary_history.proxy->first_iterator,yes=true;check("original_cursor_owning_proxies",test,&yes,&valid,1);cpu<void>(0x4afa70,&cursor);std::memset(raw.data(),test%2?0xa5:0,raw.size());::new(raw.data())menu::Cursor;normalize_cursor(expected_cursor.data(),cursor);check("cursor_constructor",test,expected_cursor.data(),raw.data(),raw.size());cursor.~Cursor();}
    std::cerr<<"cursor selection and moves\n";
    menu::Cursor cursor;
    for(unsigned test=0;test<8192;++test){cursor.current=int(random()%60)-20;cursor.previous=int(random());cursor.count=int(random()%33);cursor.minimum=0;cursor.wrapping=test%2;cursor.excluded.clear();if(cursor.count>2)for(int i=1;i<cursor.count-1;++i)if(random()%3==0)cursor.excluded.push_back(i);const int choice=int(random()%100)-40;const auto before=cursor.current;const int er=cpu<int>(0x4bed50,&cursor,choice),ec=cursor.current;cursor.current=before;const int ar=cursor.select(choice);check("select_return",test,&er,&ar,4);check("select_current",test,&ec,&cursor.current,4);
        cursor.minimum=cursor.count>1?int(random()%unsigned(cursor.count-1)):0;cursor.excluded.clear();if(cursor.count>2&&cursor.wrapping)for(int i=cursor.minimum+1;i<cursor.count-1;++i)if(random()%3==0)cursor.excluded.push_back(i);cursor.current=cursor.count>0?cursor.minimum:before;const int step=test%2?-1:1;const auto bc=cursor.current;const int em=cpu<int>(0x4bfa00,&cursor,step),mc=cursor.current;cursor.current=bc;const int am=cursor.move(step);check("move_return",test,&em,&am,4);check("move_current",test,&mc,&cursor.current,4);}
    std::cerr<<"StoneMenu constructors\n";
    auto* owner=static_cast<sm::StoneMenuInf*>(VirtualAlloc(nullptr,sizeof(sm::StoneMenuInf),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));std::array<std::uint8_t,sizeof(sm::StoneMenuInf)> before,expected;
    for(unsigned test=0;test<128;++test){std::memset(owner,test%2?0xa5:0,sizeof(*owner));cpu<void>(0x515b90,owner);std::memcpy(expected.data(),owner,expected.size());cpu<void>(0x516170,owner);std::memset(owner,test%2?0xa5:0,sizeof(*owner));::new(static_cast<void*>(owner))sm::StoneMenuInf;std::memcpy(expected.data(),owner,4);normalize_cursor(expected.data()+0x28,owner->category);normalize_cursor(expected.data()+0x74,owner->selection);check("whole_constructor",test,expected.data(),owner,sizeof(*owner));owner->~StoneMenuInf();}
    ::new(static_cast<void*>(owner))sm::StoneMenuInf;
    std::cerr<<"animation shutdown order\n";
    for(unsigned test=0;test<2048;++test){for(auto& handle:owner->animation_handles)handle=random();owner->visible=int(random());owner->state=int(random());std::memcpy(before.data(),owner,before.size());events.clear();cpu<void>(test%2?0x519840:0x5198d0,owner);std::memcpy(expected.data(),owner,expected.size());const auto ee=events;std::memcpy(owner,before.data(),before.size());events.clear();if(test%2)sm::hide(*owner,host);else sm::clear(*owner,host);check("shutdown_state",test,expected.data(),owner,sizeof(*owner));check_events("shutdown_events",test,ee);}
    std::cerr<<"full initialization and text parser\n";const auto stock_bytes=th20::read_file(argv[2]);const std::string stock(reinterpret_cast<const char*>(stock_bytes.data()),stock_bytes.size());
    for(unsigned test=0;test<96;++test){resource=test%4==0?stock:test%4==1?"#comment\r\n@REIMU_MAIN_RED\r\nName\n# comment\nD1\n\nD2\n@REIMU_WIDE_RED\nWide\n\\\nignored":test%4==2?"@MARISA_MAIN_GREEN2_EX\nEX name\n1\n2\n3\n4\n5\nignored\n@REIMU_NARROW_RED\nAnother\n":"@REIMU_MAIN_RED\nN\fame\r\nD\fesc\n";missing_file=test%12==11;owner->select_context(0);owner->update_node=owner->draw_node=nullptr;for(auto& line:owner->names)std::memset(line,0x35,sizeof(line));for(auto& group:owner->descriptions)for(auto& line:group)std::memset(line,0x73,sizeof(line));std::memcpy(before.data(),owner,before.size());events.clear();const int er=cpu<int>(0x519960,owner,test%2);std::memcpy(expected.data(),owner,expected.size());sc::Node eu{},ed{};if(owner->update_node){eu=*owner->update_node;ed=*owner->draw_node;}const auto ee=events;
        if(owner->update_node)cpu<void>(0x4124b0,&scheduler,owner->update_node);if(owner->draw_node)cpu<void>(0x4124b0,&scheduler,owner->draw_node);
        std::memcpy(owner,before.data(),before.size());events.clear();const int ar=sm::initialize(*owner,test%2,host);auto* eo=reinterpret_cast<sm::StoneMenuInf*>(expected.data());eo->context=owner->context;eo->update_node=owner->update_node;eo->draw_node=owner->draw_node;check("initialize_return",test,&er,&ar,4);check("initialize_parser_all_fixed_buffers",test,expected.data(),owner,sizeof(*owner));check_events("initialize_resources",test,ee);
        if(owner->update_node){eu.callback=owner->update_node->callback;eu.link.value=owner->update_node;ed.callback=owner->draw_node->callback;ed.link.value=owner->draw_node;check("enabled_update_registration",test,&eu,owner->update_node,sizeof(eu));check("enabled_draw_registration",test,&ed,owner->draw_node,sizeof(ed));sc::remove(scheduler,th20::source::program_entry::scheduler_environment,owner->update_node);sc::remove(scheduler,th20::source::program_entry::scheduler_environment,owner->draw_node);owner->update_node=owner->draw_node=nullptr;}
    }
    std::cerr<<"draw text, descriptions, selection list and animation geometry\n";
    hook(0x464100,reinterpret_cast<void*>(&selection_boundary));hook(0x4bd610,reinterpret_cast<void*>(&available_boundary));hook(0x51b970,reinterpret_cast<void*>(&used_boundary));hook(0x51bc10,reinterpret_cast<void*>(&unlocked_boundary));hook(0x44cc70,reinterpret_cast<void*>(&origin_boundary));hook(0x4645e0,reinterpret_cast<void*>(&position_boundary));hook(0x4506b0,reinterpret_cast<void*>(&scale_boundary));hook(0x46c210,reinterpret_cast<void*>(&text_boundary));
    *reinterpret_cast<void**>(mapped_image_base+0x1c0698)=text_storage.data();std::array<unsigned,3> player{};*reinterpret_cast<void**>(mapped_image_base+0x1ba568+0x24)=player.data();
    for(unsigned test=0;test<3200;++test){
        owner->category.current=test%5;owner->state=(test/5)%8;owner->age.current=int((test/40)%31)-3;owner->saved_selection=int(random()%9);owner->selection_position={float(int(random()%3000)-1000)/7.f,float(int(random()%3000)-1000)/7.f,float(int(random()%3000)-1000)/7.f};origin={float(int(random()%3000)-1000)/7.f,float(int(random()%3000)-1000)/7.f,float(int(random()%3000)-1000)/7.f};for(auto& h:owner->animation_handles)h=random();
        character=test%2;player[2]=character;difficulty=test%6;*reinterpret_cast<int*>(mapped_image_base+0x1ba7d0)=difficulty;scale=float(test%7+1)/3.f;*reinterpret_cast<float*>(mapped_image_base+0x1b8818)=scale;
        for(auto& choice:selected)choice=random()%9;if(difficulty==4)selected[0]%=8;
        for(unsigned i=0;i<9;++i){available[i]=random()%30;used[i]=random()%(available[i]+1);unlocked[i]=(random()%2)!=0;}
        for(unsigned i=0;i<88;++i){sprintf_s(owner->names[i],"Name_%u",i);for(unsigned l=0;l<5;++l)if(random()%3==0)owner->descriptions[i][l][0]=0;else sprintf_s(owner->descriptions[i][l],"Description_%u_%u",i,l);}
        for(auto& byte:text_storage)byte=static_cast<std::uint8_t>(random());std::memcpy(before.data(),owner,before.size());const auto before_text=text_storage;draw_events.clear();FloatingEnvironment::prepare();const int er=cpu<int>(0x5189c0,owner);std::memcpy(expected.data(),owner,expected.size());const auto ee=draw_events;const auto et=text_storage;
        std::memcpy(owner,before.data(),before.size());text_storage=before_text;draw_events.clear();FloatingEnvironment::prepare();const int ar=sm::draw(*owner,draw_host);check("draw_return",test,&er,&ar,4);check("draw_whole_owner",test,expected.data(),owner,sizeof(*owner));check("draw_text_state",test,et.data(),text_storage.data(),et.size());const auto ec=ee.size(),ac=draw_events.size();check("draw_event_count",test,&ec,&ac,sizeof(ec));if(ec==ac)check("draw_event_data",test,ee.data(),draw_events.data(),ac*sizeof(DrawEvent));
    }
    std::cerr<<"sprite float subrectangles and recursive child search\n";
    hook(0x437ca0,reinterpret_cast<void*>(&descriptor_boundary));hook(0x44cd00,reinterpret_cast<void*>(&animation_boundary));hook(0x42b5d0,reinterpret_cast<void*>(&execute_boundary));
    sp::Animation original_animation,source_animation;
    for(unsigned test=0;test<4096;++test){
        for(auto& byte:reinterpret_cast<std::array<std::uint8_t,sizeof(original_animation)>&>(original_animation))byte=static_cast<std::uint8_t>(random());source_animation=original_animation;
        auto f=[&]{return float(int(random()%10000)-5000)/13.f;};sprite_descriptor.left=f();sprite_descriptor.top=f();sprite_descriptor.u0=f();sprite_descriptor.v0=f();sprite_descriptor.texture_extent_1c=f();sprite_descriptor.texture_extent_20=f();sprite_descriptor.scale_50=f();sprite_descriptor.scale_54=f();const float x=f(),y=f(),w=f(),h=f();
        FloatingEnvironment::prepare();cpu<void>(0x438f10,&original_animation,x,y,w,h);FloatingEnvironment::prepare();sp::set_animation_texture_rectangle(source_animation,sprite_descriptor,x,y,w,h);check("float_texture_rectangle",test,&original_animation,&source_animation,sizeof(source_animation));
    }
    std::array<sp::Animation,16> tree{};
    for(unsigned test=0;test<2048;++test){
        for(auto& a:tree){std::memset(&a,0,sizeof(a));a.base.field_440=static_cast<unsigned short>(random()%7);a.links[3].value=&a;}
        tree[0].links[3].next=&tree[1].links[2];tree[1].links[2].value=&tree[1];tree[1].links[2].next=&tree[2].links[2];tree[2].links[2].value=&tree[2];
        tree[1].links[3].next=&tree[3].links[2];tree[3].links[2].value=&tree[3];tree[3].links[2].next=&tree[4].links[2];tree[4].links[2].value=&tree[4];
        tree[4].links[3].next=&tree[5].links[2];tree[5].links[2].value=&tree[5];if(test%3==0)tree[0].base.field_440=static_cast<unsigned short>(-2);
        const int script=test%7==0?-1:int(random()%9),occurrence=int(random()%5);auto* expected_child=cpu<sp::Animation*>(0x44c6b0,tree.data(),script,occurrence);auto* actual_child=sp::find_animation_child(tree[0],script,occurrence);check("recursive_child_pointer",test,&expected_child,&actual_child,4);
    }
    // A small controller reference is never accessed by these helper bodies;
    // both original and source lookup boundaries resolve the same actual tree.
    auto* unused_controller=reinterpret_cast<sp::Controller*>(tree.data());
    for(unsigned test=0;test<1024;++test){std::memset(tree.data(),0,sizeof(tree));for(unsigned i=0;i<8;++i){tree[i].index=i;tree[i].links[3].value=&tree[i];if(i<7)tree[i].links[3].next=&tree[i+1].links[3];}active_animation=&tree[0];const auto before_tree=tree;const unsigned handle=test%13==0?0:7,event=random();immediate_events.clear();using Immediate=void(__cdecl*)(unsigned,int);reinterpret_cast<Immediate>(mapped_image_base+0x4efc0)(handle,event);const auto expected_tree=tree;const auto ee=immediate_events;tree=before_tree;immediate_events.clear();sp::execute_animation_interrupt(*unused_controller,handle,event);check("immediate_interrupt_tree",test,expected_tree.data(),tree.data(),sizeof(tree));const auto ec=ee.size(),ac=immediate_events.size();check("immediate_interrupt_count",test,&ec,&ac,sizeof(ec));if(ec==ac)check("immediate_interrupt_order",test,ee.data(),immediate_events.data(),ac*4);}
    #include "update_cases.inc"
    owner->~StoneMenuInf();VirtualFree(owner,0,MEM_RELEASE);
    std::ofstream report(argv[3],std::ios::binary);report<<"{\n  \"module\": \"stone_menu\",\n  \"original_sha256\": \""<<expected_sha<<"\",\n  \"passed\": "<<passed<<",\n  \"failed\": "<<failed<<",\n  \"failures\": [";for(std::size_t i=0;i<failures.size();++i){if(i)report<<',';report<<'\"'<<failures[i]<<'\"';}report<<"]\n}\n";std::cout<<"passed="<<passed<<" failed="<<failed<<'\n';for(auto& failure:failures)std::cerr<<failure<<'\n';return failed?1:0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 2;}}
