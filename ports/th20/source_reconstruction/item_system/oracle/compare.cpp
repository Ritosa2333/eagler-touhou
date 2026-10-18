// Original machine code is restricted to selected-function oracle execution.
#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../item.hpp"
#include "../rewards.hpp"
#include "../collect.hpp"
#include "../../gameplay/player_state.hpp"
#include "../../program_entry/program_entry.hpp"
#include "../../gameplay/gameplay.hpp"
namespace it=th20::source::item;namespace gs=th20::source::game_session;namespace state=th20::source::state;namespace sc=th20::source::scheduler;namespace sp=th20::source::sprite;
namespace th20::source::program_entry {sc::State* function_controller=nullptr;sc::Environment scheduler_environment;}
namespace th20::source::gameplay {GameController* controller=nullptr;}
namespace th20::source::sprite {int execute_animation(Animation&){throw std::logic_error("Unexpected ANM execution outside Item fixture boundary");}}
template<class R,class... A>R cpu(std::uint32_t va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
std::vector<std::array<std::uint32_t,8>> events;it::ItemInf* fixture=nullptr;
void event(unsigned kind,const void* object,int value=0){events.push_back({kind,reinterpret_cast<unsigned>(object)-reinterpret_cast<unsigned>(fixture),static_cast<unsigned>(value),0,0,0,0,0});}
void __fastcall bind_boundary(void*,void*,sp::Animation* animation,int script){event(1,animation,script);}
int __fastcall effect_boundary(it::Item* item,void*){event(2,item,item->type);return 0;}
bool boss_collect=false;
void __fastcall select_boundary(void*,void*,int view){event(3,fixture,view);}
void __fastcall layer_boundary(void*,void*,int layer,int view){event(4,fixture,layer);events.back()[3]=view;}
void __fastcall activate_boundary(it::Item* item,void*){event(5,item);item->state=2;}
void __fastcall collect_boundary(it::Item* item,void*){event(6,item);}
void __fastcall sound_boundary(void*,void*,int sound,float x){event(7,fixture,sound);std::memcpy(&events.back()[3],&x,4);}
void __fastcall update_animation_boundary(sp::Animation* animation,void*){event(8,animation);}
void __fastcall draw_animation_boundary(sp::Animation* animation,void*){event(9,animation);}
void __fastcall move_boundary(std::uint32_t* handle,void*,const sp::Vec3* position){event(10,handle,*handle);std::memcpy(events.back().data()+3,position,12);}
void __fastcall retire_boundary(std::uint32_t* handle,void*){event(11,handle,*handle);*handle=0;}
bool real_collection=false;void collect_real_reward(it::Item&);
struct Fixture final:it::Environment {
    void bind_animation(it::ItemInf&,sp::Animation& animation,int script) override{bind_boundary(nullptr,nullptr,&animation,script);}
    void bind_special_animation(sp::Animation& animation,int script) override{bind_boundary(nullptr,nullptr,&animation,script);}
    void spawn_effect(it::Item& item) override{effect_boundary(&item,nullptr);}
    void bonus_notification() override{auto* overlay=reinterpret_cast<std::uint8_t*>(gs::overlay_owner(0));(*reinterpret_cast<std::uint8_t**>(overlay+0x34))[0x35]=1;}
    void select_view(it::ItemInf& owner) override{select_boundary(nullptr,nullptr,owner.view_index);}
    void configure_layer(it::ItemInf& owner,int layer) override{layer_boundary(nullptr,nullptr,layer,owner.view_index);}
    bool boss_collecting() override{return boss_collect;}
    void activate_special(it::Item& item) override{activate_boundary(&item,nullptr);}
    void collect(it::Item& item) override{if(real_collection)collect_real_reward(item);else collect_boundary(&item,nullptr);}
    void collect_sound(const it::Item& item) override{sound_boundary(nullptr,nullptr,37,item.position.x);}
    void update_animation(sp::Animation& animation) override{update_animation_boundary(&animation,nullptr);}
    void draw_animation(sp::Animation& animation) override{draw_animation_boundary(&animation,nullptr);}
    void move_attachment(std::uint32_t& handle,const sp::Vec3& position) override{move_boundary(&handle,nullptr,&position);}
    void retire_attachment(std::uint32_t& handle) override{retire_boundary(&handle,nullptr);}
};
bool has_hud=false,special_active=false;
void __fastcall bomb_hud_boundary(void*,void*,int count,int fragments,int maximum){event(20,fixture,count);events.back()[3]=fragments;events.back()[4]=maximum;}
void __fastcall life_hud_boundary(void*,void*,int count,int fragments,int maximum){event(21,fixture,count);events.back()[3]=fragments;events.back()[4]=maximum;}
void __fastcall notice_boundary(void*,void*,int type,int value){event(22,fixture,type);events.back()[3]=value;}
void __fastcall simple_sound_boundary(void*,void*,int sound,int value){event(23,fixture,sound);events.back()[3]=value;}
void __fastcall score_boundary(void* owner,void*,const sp::Vec3* position,int amount,unsigned color){event(24,owner,amount);events.back()[3]=color;std::memcpy(events.back().data()+4,position,12);}
void __fastcall refresh_boundary(void* player,void*,int value){event(25,player,value);}
void __cdecl phase_boundary(){event(26,fixture);}
struct RewardFixture final:it::RewardEnvironment {
    bool hud_available() override{return has_hud;}
    void hud_bombs(int a,int b,int c) override{bomb_hud_boundary(nullptr,nullptr,a,b,c);}
    void hud_lives(int a,int b,int c) override{life_hud_boundary(nullptr,nullptr,a,b,c);}
    void hud_notice(int a,int b) override{notice_boundary(nullptr,nullptr,a,b);}
    void sound(int id) override{simple_sound_boundary(nullptr,nullptr,id,0);}
    void sound_at(int id,float x) override{sound_boundary(nullptr,nullptr,id,x);}
    void floating_score(void* owner,const sp::Vec3& p,int amount,unsigned color) override{score_boundary(owner,nullptr,&p,amount,color);}
    void refresh_power(void* p,int value) override{refresh_boundary(p,nullptr,value);}
    bool boss_collecting() override{return boss_collect;}
    bool special_active() override{return ::special_active;}
    void start_special_phase() override{phase_boundary();}
};
RewardFixture reward_host;
void collect_real_reward(it::Item& item){it::collect_item(item,reward_host);}
namespace th20::source::item {RewardEnvironment& reward_environment(){return reward_host;}}
Fixture host;
namespace th20::source::item {Environment& environment(){return host;}}
int wmain(int argc,wchar_t** argv){try{
    AddVectoredExceptionHandler(1,[](PEXCEPTION_POINTERS p)->LONG{if(p->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION)std::cerr<<"access violation VA="<<std::hex<<p->ContextRecord->Eip-mapped_image_base+0x400000<<" address="<<p->ExceptionRecord->ExceptionInformation[1]<<std::dec<<std::endl;return EXCEPTION_CONTINUE_SEARCH;});
    if(argc!=3)throw std::runtime_error("Usage: th20_item_cpu_compare ORIGINAL.exe REPORT.json");const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");const auto info=th20::parse_pe(bytes);Mapping image(bytes,info);mapped_image_base=image.address();
    for(const auto& import:info.imports)if(auto address=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),import.name.c_str()))*reinterpret_cast<FARPROC*>(mapped_image_base+import.iat_rva)=address;
    *reinterpret_cast<HANDLE*>(mapped_image_base+0x1e5990)=GetProcessHeap();cpu<void>(0x452e00,reinterpret_cast<void*>(mapped_image_base+0x1c0240));sc::State scheduler;sc::initialize_state(scheduler);th20::source::program_entry::function_controller=&scheduler;*reinterpret_cast<void**>(mapped_image_base+0x1b66d8)=&scheduler;
    std::mt19937 random(0x4c3c90);unsigned passed=0,failed=0;std::vector<std::string> failures;
    auto check=[&](const char* name,unsigned test,const void* a,const void* b,std::size_t size){if(!std::memcmp(a,b,size)){++passed;return;}++failed;if(failures.size()<30){std::ostringstream out;out<<name<<" test="<<test;for(std::size_t i=0;i<size;++i)if(static_cast<const unsigned char*>(a)[i]!=static_cast<const unsigned char*>(b)[i]){out<<" +0x"<<std::hex<<i<<" expected="<<unsigned(static_cast<const unsigned char*>(a)[i])<<" actual="<<unsigned(static_cast<const unsigned char*>(b)[i]);break;}failures.push_back(out.str());}};
    fixture=static_cast<it::ItemInf*>(VirtualAlloc(nullptr,sizeof(it::ItemInf),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));if(!fixture)throw std::bad_alloc();std::vector<std::uint8_t> before(sizeof(*fixture)),expected(sizeof(*fixture));
    for(unsigned test=0;test<16;++test){std::memset(fixture,test%2?0xa5:0,sizeof(*fixture));cpu<void>(0x4c22e0,fixture);std::memcpy(expected.data(),fixture,expected.size());std::memset(fixture,test%2?0xa5:0,sizeof(*fixture));::new(static_cast<void*>(fixture))it::ItemInf;std::memcpy(expected.data(),fixture,4);check("whole_item_controller_constructor",test,expected.data(),fixture,sizeof(*fixture));}
    for(unsigned test=0;test<64;++test){for(auto& byte:before)byte=static_cast<std::uint8_t>(random());std::memcpy(fixture,before.data(),before.size());cpu<void>(0x4bc220,fixture);std::memcpy(expected.data(),fixture,expected.size());std::memcpy(fixture,before.data(),before.size());fixture->initialize_pool();check("whole_item_pool_reset",test,expected.data(),fixture,sizeof(*fixture));}
    std::memset(fixture,0,sizeof(*fixture));::new(static_cast<void*>(fixture))it::ItemInf;fixture->select_context(0);gs::context(0).objects_04[2]=fixture;gs::context(0).current_player=&gs::player(0);
    std::vector<std::uint8_t> primary(0x286d90);*reinterpret_cast<void**>(primary.data()+0x286d8c)=reinterpret_cast<void*>(0x12345678);gs::context(0).primary_owner=reinterpret_cast<th20::source::runtime::CallbackOwner*>(primary.data());
    std::array<std::uint8_t,0x60> overlay{},auxiliary{};*reinterpret_cast<void**>(overlay.data()+0x34)=auxiliary.data();gs::context(0).overlay_owner=reinterpret_cast<th20::source::runtime::CallbackOwner*>(overlay.data());
    auto* mapped_session=reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568);auto* mapped_random=reinterpret_cast<state::Random*>(mapped_image_base+0x1ba4a8);
    auto hook=[&](unsigned va,void* target){auto* p=reinterpret_cast<std::uint8_t*>(mapped_image_base+va-0x400000);p[0]=0xe9;*reinterpret_cast<unsigned*>(p+1)=reinterpret_cast<unsigned>(target)-reinterpret_cast<unsigned>(p+5);FlushInstructionCache(GetCurrentProcess(),p,5);};hook(0x438380,reinterpret_cast<void*>(&bind_boundary));hook(0x4c42c0,reinterpret_cast<void*>(&effect_boundary));
    auto coordinate=[&]{return float(int(random()%10000)-5000)/17.f;};
    for(unsigned test=0;test<4096;++test){fixture->initialize_pool();fixture->spawn_counter=static_cast<int>(random());fixture->point_counter=test%12;fixture->special_count=static_cast<int>(test%5)*256;fixture->generation=random();fixture->bonus_counter=test%14;gs::player(0).bytes_a4[1]=test%2;gs::player(0).bytes_2c[3]=test%3==0;gs::session.player_table.field_1e0=test%5;auxiliary[0x35]=0;
        state::seed(state::random_streams[0],random());*mapped_random=state::random_streams[0];std::memcpy(mapped_session,&gs::session,sizeof(gs::session));const auto before_random=state::random_streams[0];const auto before_auxiliary=auxiliary;std::memcpy(before.data(),fixture,before.size());const sp::Vec3 position{coordinate(),coordinate(),coordinate()};const auto angle=coordinate()/20,speed=coordinate()/100;const unsigned color=random(),extra=random();const int type=test%17,delay=test%4,sound=-1;
        events.clear();FloatingEnvironment::prepare();auto* expected_pointer=cpu<it::Item*>(0x4c3c90,fixture,type,&position,color,angle,speed,delay,extra,sound);const auto expected_events=events;const auto expected_random=*mapped_random;const auto expected_auxiliary=auxiliary;std::memcpy(expected.data(),fixture,expected.size());
        auto* expected_controller=reinterpret_cast<it::ItemInf*>(expected.data());for(auto& item:expected_controller->pool){auto pointer=reinterpret_cast<std::uintptr_t>(item.context);const auto original_session_address=reinterpret_cast<std::uintptr_t>(mapped_session);if(pointer>=original_session_address&&pointer<original_session_address+sizeof(gs::Session))item.context=reinterpret_cast<gs::Context*>(reinterpret_cast<std::uintptr_t>(&gs::session)+pointer-original_session_address);}
        std::memcpy(fixture,before.data(),before.size());state::random_streams[0]=before_random;auxiliary=before_auxiliary;events.clear();FloatingEnvironment::prepare();auto* actual_pointer=it::spawn(*fixture,type,position,color,angle,speed,delay,extra,sound,host);
        check("spawn_pointer",test,&expected_pointer,&actual_pointer,4);check("spawn_all_pool_lists_and_item_bytes",test,expected.data(),fixture,sizeof(*fixture));check("spawn_real_random_stream",test,&expected_random,&state::random_streams[0],sizeof(expected_random));check("spawn_bonus_notification",test,expected_auxiliary.data(),auxiliary.data(),auxiliary.size());const auto expected_count=expected_events.size(),actual_count=events.size();check("spawn_event_count",test,&expected_count,&actual_count,sizeof(actual_count));if(expected_count==actual_count)check("spawn_event_order_and_scripts",test,expected_events.data(),events.data(),events.size()*sizeof(events[0]));
    }
    std::array<std::uint8_t,5> activation_body;std::memcpy(activation_body.data(),reinterpret_cast<void*>(mapped_image_base+0xc4420),5);
    std::array<std::uint8_t,5> small_power_body;std::memcpy(small_power_body.data(),reinterpret_cast<void*>(mapped_image_base+0xc4b90),5);
    hook(0x4776a0,reinterpret_cast<void*>(&select_boundary));hook(0x44f3d0,reinterpret_cast<void*>(&layer_boundary));hook(0x4c4420,reinterpret_cast<void*>(&activate_boundary));hook(0x4c4b90,reinterpret_cast<void*>(&collect_boundary));hook(0x426eb0,reinterpret_cast<void*>(&sound_boundary));hook(0x42b5d0,reinterpret_cast<void*>(&update_animation_boundary));hook(0x44c570,reinterpret_cast<void*>(&draw_animation_boundary));hook(0x4502c0,reinterpret_cast<void*>(&move_boundary));hook(0x44fcd0,reinterpret_cast<void*>(&retire_boundary));
    std::array<std::uint8_t,0x2200> player_entity{};std::array<std::uint8_t,0x60> bomb{};std::array<std::uint8_t,0x1c0> boss{};
    gs::context(0).objects_04[0]=player_entity.data();gs::context(0).objects_04[5]=bomb.data();*reinterpret_cast<void**>(mapped_image_base+0x1c06a4)=boss.data();
    auto put=[&](unsigned offset,auto value){std::memcpy(player_entity.data()+offset,&value,sizeof(value));};
    for(unsigned test=0;test<2048;++test){
        fixture->initialize_pool();fixture->processed=random();fixture->special_count=random();fixture->attract=test%2;fixture->attraction_center={coordinate(),coordinate(),coordinate()};fixture->speed_scale=float(test%15)/10;fixture->view_index=test%2;
        const sp::Vec3 player_position{coordinate()/2,coordinate(),coordinate()};put(0x614,player_position);put(0x10,int(test%5));put(0x2080,float(test%15));put(0x2084,float(test%50));put(0x2088,float(test%150));put(0x2090,float(test%200));
        *reinterpret_cast<int*>(bomb.data()+0x18)=test%2;*reinterpret_cast<int*>(bomb.data()+0x20)=test%100;boss_collect=test%7==0;*reinterpret_cast<int*>(boss.data()+0x1bc)=boss_collect;
        state::clock_scale=float(test%8)/4;*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=state::clock_scale;
        for(unsigned index=0;index<12;++index){auto& item=fixture->pool[index];sc::unlink(item.link);sc::append(fixture->active,item.link);item.state=(test+index)%6;item.type=1;item.delay=int((test+index)%5)-2;item.position={coordinate(),coordinate()+200,coordinate()};item.velocity={coordinate()/20,coordinate()/50,coordinate()/90};item.attraction_speed=float((test+index)%20);item.attachment=random();item.animation.base.flags[0]=index%2?0x10000u:0;item.secondary_animation.base.flags[0]=index%3?0x10000u:0;item.animation.base.vector_2c.y=coordinate();item.secondary_animation.base.vector_2c.y=coordinate();item.secondary_animation.base.field_490=random();item.timer={int(random()),int(random()%100),coordinate(),random()};}
        std::memcpy(mapped_session,&gs::session,sizeof(gs::session));std::memcpy(before.data(),fixture,before.size());events.clear();FloatingEnvironment::prepare();const int expected_return=cpu<int>(0x4c25a0,fixture);std::memcpy(expected.data(),fixture,expected.size());const auto expected_events=events;
        std::memcpy(fixture,before.data(),before.size());events.clear();FloatingEnvironment::prepare();const int actual_return=it::update(*fixture,host);check("frame_return",test,&expected_return,&actual_return,4);check("frame_full_pool_lists_state",test,expected.data(),fixture,sizeof(*fixture));const auto ec=expected_events.size(),ac=events.size();check("frame_event_count",test,&ec,&ac,sizeof(ac));if(ec==ac)check("frame_event_order",test,expected_events.data(),events.data(),ac*sizeof(events[0]));
        const int layer=int(test%4)-1;std::memcpy(before.data(),fixture,before.size());events.clear();FloatingEnvironment::prepare();const int expected_draw=cpu<int>(0x4c38d0,fixture,layer);std::memcpy(expected.data(),fixture,expected.size());const auto expected_draw_events=events;
        std::memcpy(fixture,before.data(),before.size());events.clear();FloatingEnvironment::prepare();const int actual_draw=it::draw(*fixture,layer,host);check("draw_return",test,&expected_draw,&actual_draw,4);check("draw_full_pool_state",test,expected.data(),fixture,sizeof(*fixture));const auto edc=expected_draw_events.size(),adc=events.size();check("draw_event_count",test,&edc,&adc,sizeof(adc));if(edc==adc)check("draw_event_order",test,expected_draw_events.data(),events.data(),adc*sizeof(events[0]));
    }
    hook(0x4b8650,reinterpret_cast<void*>(&bomb_hud_boundary));hook(0x4b8bf0,reinterpret_cast<void*>(&life_hud_boundary));hook(0x4b90e0,reinterpret_cast<void*>(&notice_boundary));hook(0x426d70,reinterpret_cast<void*>(&simple_sound_boundary));hook(0x510710,reinterpret_cast<void*>(&score_boundary));hook(0x4faca0,reinterpret_cast<void*>(&refresh_boundary));hook(0x534d00,reinterpret_cast<void*>(&phase_boundary));
    auto compare_events=[&](const char* name,unsigned test,const auto& expected_events){const auto ec=expected_events.size(),ac=events.size();check(name,test,&ec,&ac,sizeof(ec));if(ec==ac)check(name,test,expected_events.data(),events.data(),ac*sizeof(events[0]));};
    namespace ps=th20::source::gameplay::player_state;
    for(unsigned test=0;test<4096;++test){
        gs::Player player_before;for(auto& value:reinterpret_cast<std::array<unsigned,60>&>(player_before))value=random();ps::write(player_before,0x38,int(test%301)+100);ps::write(player_before,0xc4,int(test%10));ps::write(player_before,0xc0,int(test%22)-3);ps::write(player_before,0xd0,int(test%15)-3);
        has_hud=test%2;*reinterpret_cast<void**>(mapped_image_base+0x1c06a4)=has_hud?boss.data():nullptr;const int amount=static_cast<int>(random());
        auto run_player=[&](unsigned va,const char* name,auto source,int argument){gs::Player p=player_before;events.clear();int expected_return=0;if(va==0x4e1410)expected_return=cpu<bool>(va,&p,argument);else if(va==0x4e1250)expected_return=cpu<int>(va,&p,argument);else if(va==0x4e1510)cpu<void>(va,&p);else cpu<void>(va,&p,argument);const auto expected_player=p;const auto expected_events=events;p=player_before;events.clear();const int actual_return=source(p,argument);check(name,test,&expected_player,&p,sizeof(p));check("reward_return",test,&expected_return,&actual_return,4);compare_events("reward_event_order",test,expected_events);};
        run_player(0x4e1410,"add_power",[&](auto& p,int value){return int(it::add_power(p,value,reward_host));},amount);
        run_player(0x4e10e0,"add_bombs",[&](auto& p,int value){it::add_bombs(p,value,reward_host);return 0;},amount);
        run_player(0x4e11a0,"add_bomb_fragments",[&](auto& p,int value){it::add_bomb_fragments(p,value,reward_host);return 0;},amount);
        run_player(0x4e1250,"add_lives",[&](auto& p,int value){return it::add_lives(p,value,reward_host);},amount);
        run_player(0x4e1510,"extend_life",[&](auto& p,int){it::extend_life(p,reward_host);return 0;},amount);
        mapped_session->player_table.field_1e0=test%5;
        run_player(0x4e1310,"add_life_fragments",[&](auto& p,int value){it::add_life_fragments(p,value,test%5,reward_host);return 0;},int(test%21)-3);
        run_player(0x4c46a0,"point_item_count",[&](auto& p,int value){it::add_point_items(p,value);return 0;},amount);
        run_player(0x4a9fb0,"special_item_count",[&](auto& p,int value){it::add_special_items(p,value);return 0;},amount);
        constexpr unsigned addresses[]{0x4a9ec0,0x4a9d80,0x4a9f20,0x4a9e60};
        for(unsigned index=0;index<4;++index)run_player(addresses[index],"special_color_counter",[&](auto& p,int value){it::add_special_counter(p,index,value);return 0;},amount);
    }
    // Restore the original small-power body after the frame dispatch-boundary
    // tests; reward comparisons execute the real original body and its getters.
    std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x4c4b90-0x400000),small_power_body.data(),5);
    gs::context(0).objects_04[6]=reinterpret_cast<void*>(0x23456780);gs::context(0).current_player=&gs::player(0);fixture->context=&gs::context(0);*reinterpret_cast<void**>(mapped_image_base+0x1c06a4)=boss.data();has_hud=true;
    auto normalize_session=[&](gs::Session& value){for(auto& context:value.contexts){auto pointer=reinterpret_cast<std::uintptr_t>(context.current_player),base=reinterpret_cast<std::uintptr_t>(mapped_session);if(pointer>=base&&pointer<base+sizeof(value))context.current_player=reinterpret_cast<gs::Player*>(reinterpret_cast<std::uintptr_t>(&gs::session)+pointer-base);}};
    for(unsigned test=0;test<4096;++test){
        auto& p=gs::player(0);for(auto& value:reinterpret_cast<std::array<unsigned,60>&>(p))value=random();ps::write(p,0x30,int(test%701)-200);ps::write(p,0x38,int(test%301)+100);gs::session.player_table.field_1e0=test%5;put(0x614,sp::Vec3{coordinate(),coordinate()+250,0});put(0x2090,coordinate());
        auto& item=fixture->pool[0];item.position={coordinate(),coordinate()+250,coordinate()};item.state=test%5;item.context=&gs::context(0);const auto before_session=gs::session;
        *mapped_session=gs::session;mapped_session->contexts[0].current_player=&mapped_session->player_table.players[0];item.context=&mapped_session->contexts[0];events.clear();FloatingEnvironment::prepare();constexpr unsigned addresses[]{0x4c4960,0x4c4b90,0x4c4da0,0x4c47c0};cpu<void>(addresses[test%4],&item);auto expected_session=*mapped_session;normalize_session(expected_session);const auto expected_events=events;
        gs::session=before_session;item.context=&gs::context(0);events.clear();FloatingEnvironment::prepare();switch(test%4){case 0:it::collect_point(item,reward_host);break;case 1:it::collect_small_power(item,reward_host);break;case 2:it::collect_large_power(item,reward_host);break;case 3:it::collect_full_power(item,reward_host);break;}check("pickup_whole_session",test,&expected_session,&gs::session,sizeof(gs::session));compare_events("pickup_event_order",test,expected_events);
        ps::write(p,0x4c,int(test%620)-60);ps::write(p,0x50,int(test%620)-60);p.bytes_a4[0]=test%2;p.bytes_2c[3]=test%3==0;*reinterpret_cast<int*>(overlay.data()+0x54)=test%5==0;auxiliary[0x35]=0;boss_collect=test%7==0;*reinterpret_cast<int*>(boss.data()+0x1bc)=boss_collect;
        const auto overlay_session=gs::session;const auto before_aux=auxiliary;*mapped_session=gs::session;mapped_session->contexts[0].current_player=&mapped_session->player_table.players[0];events.clear();cpu<void>(0x533780,overlay.data(),int(test%5)-1);expected_session=*mapped_session;normalize_session(expected_session);const auto expected_aux=auxiliary;const auto expected_overlay_events=events;
        gs::session=overlay_session;auxiliary=before_aux;events.clear();it::add_overlay_meter(*reinterpret_cast<th20::source::runtime::CallbackOwner*>(overlay.data()),int(test%5)-1,reward_host);check("overlay_meter_whole_session",test,&expected_session,&gs::session,sizeof(gs::session));check("overlay_bonus_flag",test,expected_aux.data(),auxiliary.data(),auxiliary.size());compare_events("overlay_event_order",test,expected_overlay_events);
    }
    auto normalize_pool=[&](std::vector<std::uint8_t>& value){auto* owner=reinterpret_cast<it::ItemInf*>(value.data());auto normalize=[&](gs::Context*& context){const auto pointer=reinterpret_cast<std::uintptr_t>(context),base=reinterpret_cast<std::uintptr_t>(mapped_session);if(pointer>=base&&pointer<base+sizeof(gs::Session))context=reinterpret_cast<gs::Context*>(reinterpret_cast<std::uintptr_t>(&gs::session)+pointer-base);};normalize(owner->context);for(auto& item:owner->pool)normalize(item.context);};
    for(unsigned test=0;test<256;++test){fixture->initialize_pool();fixture->select_context(0);gs::player(0).bytes_a4[1]=test%2;gs::player(0).bytes_2c[3]=test%3==0;gs::context(0).current_player=&gs::player(0);auxiliary[0x35]=0;state::seed(state::random_streams[0],random());*mapped_random=state::random_streams[0];const auto before_random=state::random_streams[0];*mapped_session=gs::session;const sp::Vec3 position{coordinate(),coordinate(),coordinate()};const int count=int(test%19)-2,type=test%2?13:1;std::memcpy(before.data(),fixture,before.size());const auto before_aux=auxiliary;
        events.clear();FloatingEnvironment::prepare();cpu<void>(0x4c45b0,fixture,&position,count,type);std::memcpy(expected.data(),fixture,expected.size());normalize_pool(expected);const auto expected_random=*mapped_random;const auto expected_aux=auxiliary;const auto expected_events=events;
        std::memcpy(fixture,before.data(),before.size());state::random_streams[0]=before_random;auxiliary=before_aux;events.clear();FloatingEnvironment::prepare();it::spawn_many(*fixture,position,count,type,host);check("scatter_whole_controller",test,expected.data(),fixture,sizeof(*fixture));check("scatter_random_stream",test,&expected_random,&state::random_streams[0],sizeof(expected_random));check("scatter_bonus_flag",test,expected_aux.data(),auxiliary.data(),auxiliary.size());compare_events("scatter_event_order",test,expected_events);
    }
    real_collection=true;std::array<std::uint8_t,0xb4> special{};*reinterpret_cast<void**>(mapped_image_base+0x1c6118)=special.data();
    for(unsigned test=0;test<544;++test){
        fixture->initialize_pool();fixture->select_context(0);put(0x10,0);put(0x614,sp::Vec3{});put(0x2084,1000.f);put(0x2088,0.f);put(0x2090,1000.f);state::clock_scale=0;*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=0;*reinterpret_cast<int*>(boss.data()+0x1bc)=0;boss_collect=false;special_active=test%2;special[0xb0]=special_active;*reinterpret_cast<int*>(overlay.data()+0x54)=0;
        auto& p=gs::player(0);gs::construct_player(p);ps::write(p,0x30,int(test%501));ps::write(p,0x34,400);ps::write(p,0x38,100);ps::write(p,0x40,10000+int(test)*100);ps::write(p,0x44,int(test));ps::write(p,0x48,5000);ps::write(p,0x50,100);ps::write(p,0xbc,7);ps::write(p,0xd8,7);ps::write(p,0xc4,int(test%9));ps::write(p,0xb8,int(test%5));ps::write(p,0xcc,int(test%6));ps::write(p,0xd0,int(test%3));ps::write(p,0xc0,int(test%3));p.bytes_a4[0]=test%3==0;gs::session.player_table.field_1e0=test%5;auxiliary[0x35]=0;
        auto& item=fixture->pool[0];sc::unlink(item.link);sc::append(fixture->active,item.link);item.state=2;item.type=test%17;item.position={float(test%7),float(test%11),0};item.context=&gs::context(0);item.velocity={0,-1,0};
        *mapped_session=gs::session;mapped_session->contexts[0].current_player=&mapped_session->player_table.players[0];const auto before_session=gs::session;const auto before_aux=auxiliary;std::memcpy(before.data(),fixture,before.size());fixture->context=&mapped_session->contexts[0];item.context=&mapped_session->contexts[0];events.clear();FloatingEnvironment::prepare();const auto expected_return=cpu<int>(0x4c25a0,fixture);std::memcpy(expected.data(),fixture,expected.size());normalize_pool(expected);auto expected_session=*mapped_session;normalize_session(expected_session);const auto expected_aux=auxiliary;const auto expected_events=events;
        gs::session=before_session;auxiliary=before_aux;std::memcpy(fixture,before.data(),before.size());events.clear();FloatingEnvironment::prepare();const auto actual_return=it::update(*fixture,host);check("all_type_pickup_frame_return",test,&expected_return,&actual_return,4);check("all_type_pickup_frame_controller",test,expected.data(),fixture,sizeof(*fixture));check("all_type_pickup_frame_session",test,&expected_session,&gs::session,sizeof(gs::session));check("all_type_pickup_frame_overlay_bonus",test,expected_aux.data(),auxiliary.data(),auxiliary.size());compare_events("all_type_pickup_frame_events",test,expected_events);
    }
    std::memcpy(reinterpret_cast<void*>(mapped_image_base+0xc4420),activation_body.data(),5);std::array<std::uint8_t,0x14> special_owner{};*reinterpret_cast<void**>(special_owner.data()+0x10)=reinterpret_cast<void*>(0x12345678);*reinterpret_cast<void**>(mapped_image_base+0x1c6120)=special_owner.data();
    for(unsigned test=0;test<4096;++test){auto& item=fixture->pool[0];for(auto& value:reinterpret_cast<std::array<unsigned,sizeof(it::Item)/4>&>(item))value=random();item.type=test%6+8;item.position={coordinate(),coordinate(),coordinate()};for(unsigned i=3;i<=6;++i)gs::player(0).fields_00[i]=random();*mapped_session=gs::session;state::seed(state::random_streams[0],random());*mapped_random=state::random_streams[0];const auto before_random=state::random_streams[0];const auto before_item=item;events.clear();cpu<void>(0x4c4420,&item);const auto expected_item=item;const auto expected_random=*mapped_random;const auto expected_events=events;
        item=before_item;state::random_streams[0]=before_random;events.clear();it::activate_special_item(item,host);check("activate_special_whole_item",test,&expected_item,&item,sizeof(item));check("activate_special_random_stream",test,&expected_random,&state::random_streams[0],sizeof(expected_random));compare_events("activate_special_scripts",test,expected_events);
    }
    for(unsigned test=0;test<64;++test){
        std::memset(fixture,0,sizeof(*fixture));::new(static_cast<void*>(fixture))it::ItemInf;const int index=test%2;std::memcpy(before.data(),fixture,before.size());
        const int expected_return=cpu<int>(0x4c3c00,fixture,index);std::memcpy(expected.data(),fixture,expected.size());sc::Node expected_nodes[]{*fixture->update_node,*fixture->draw_node,*fixture->second_draw_node};sc::Node* old_nodes[]{fixture->update_node,fixture->draw_node,fixture->second_draw_node};
        cpu<void>(0x4c24c0,fixture);std::vector<std::uint8_t> expected_shutdown(sizeof(*fixture));std::memcpy(expected_shutdown.data(),fixture,expected_shutdown.size());
        std::memcpy(fixture,before.data(),before.size());const int actual_return=fixture->initialize(index);sc::Node* new_nodes[]{fixture->update_node,fixture->draw_node,fixture->second_draw_node};
        auto normalize_node_link=[&](sc::Link*& link){for(unsigned i=0;i<3;++i)if(link==&old_nodes[i]->link){link=&new_nodes[i]->link;break;}};
        auto normalize_owner=[&](std::vector<std::uint8_t>& storage){auto* object=reinterpret_cast<it::ItemInf*>(storage.data());object->context=fixture->context;object->update_node=fixture->update_node;object->draw_node=fixture->draw_node;object->second_draw_node=fixture->second_draw_node;};normalize_owner(expected);normalize_owner(expected_shutdown);
        check("initialize_result",test,&expected_return,&actual_return,4);check("initialize_full_item_controller",test,expected.data(),fixture,sizeof(*fixture));
        for(unsigned i=0;i<3;++i){expected_nodes[i].callback=new_nodes[i]->callback;expected_nodes[i].link.value=new_nodes[i];normalize_node_link(expected_nodes[i].link.next);normalize_node_link(expected_nodes[i].link.previous);check("initialize_registered_node",test*3+i,&expected_nodes[i],new_nodes[i],sizeof(sc::Node));}
        cpu<void>(0x4c4650,fixture);unsigned expected_enabled[]{new_nodes[0]->flags,new_nodes[1]->flags,new_nodes[2]->flags};for(unsigned i=0;i<3;++i)new_nodes[i]->flags=expected_nodes[i].flags;fixture->enable_callbacks();unsigned actual_enabled[]{new_nodes[0]->flags,new_nodes[1]->flags,new_nodes[2]->flags};check("enable_all_three_nodes",test,expected_enabled,actual_enabled,sizeof(actual_enabled));
        cpu<void>(0x421760,fixture);unsigned expected_disabled[]{new_nodes[0]->flags,new_nodes[1]->flags,new_nodes[2]->flags};for(unsigned i=0;i<3;++i)new_nodes[i]->flags=actual_enabled[i];fixture->disable_callbacks();unsigned actual_disabled[]{new_nodes[0]->flags,new_nodes[1]->flags,new_nodes[2]->flags};check("inherited_disable_only_two_nodes",test,expected_disabled,actual_disabled,sizeof(actual_disabled));
        fixture->~ItemInf();std::memcpy(expected_shutdown.data(),fixture,4);check("destructor_full_item_controller",test,expected_shutdown.data(),fixture,sizeof(*fixture));const bool empty=scheduler.update.sentinel.next==nullptr&&scheduler.update.tail==&scheduler.update.sentinel&&scheduler.draw.sentinel.next==nullptr&&scheduler.draw.tail==&scheduler.draw.sentinel,yes=true;check("destructor_both_chains_empty",test,&yes,&empty,1);
    }
    std::ofstream report(argv[2],std::ios::binary);report<<"{\n  \"module\": \"item_system\",\n  \"original_sha256\": \""<<expected_sha<<"\",\n  \"passed\": "<<passed<<",\n  \"failed\": "<<failed<<",\n  \"failures\": [";for(std::size_t i=0;i<failures.size();++i){if(i)report<<',';report<<'\"'<<failures[i]<<'\"';}report<<"]\n}\n";std::cout<<"passed="<<passed<<" failed="<<failed<<'\n';for(auto& failure:failures)std::cerr<<failure<<'\n';return failed?1:0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 2;}}
