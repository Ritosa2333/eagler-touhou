// Original mapping exists only in this isolated oracle, never in production.
#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../regions.hpp"
#include "../geometry.hpp"
#include "../damage.hpp"
#include "../hit_callbacks.hpp"
#include "../../program_entry/program_entry.hpp"
namespace d=th20::source::damage;namespace state=th20::source::state;namespace gs=th20::source::game_session;namespace sc=th20::source::scheduler;
namespace th20::source::program_entry {sc::State* function_controller=nullptr;sc::Environment scheduler_environment;}
namespace pe=th20::source::program_entry;
namespace th20::source::sprite {int execute_animation(Animation&){throw std::logic_error("ANM interpreter execution outside damage hit fixture");}}
template<class R,class... A>R cpu(std::uint32_t va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
template<class... A>bool geometry_cpu(std::uint32_t va,A... args){using F=bool(__cdecl*)(A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(args...);}
struct Event {std::uint32_t kind,handle;th20::source::sprite::Vec3 position;int count,type;};
std::vector<Event> events;
namespace th20::source::damage::unrecovered {void spawn_item_004c45b0(void*,const sprite::Vec3& p,int count,int type){events.push_back({1,0,p,count,type});}}
void __fastcall item_boundary(void* item,void*,const th20::source::sprite::Vec3* p,int count,int type){d::unrecovered::spawn_item_004c45b0(item,*p,count,type);}
int __cdecl hit_boundary(d::Region* region,const th20::source::sprite::Vec3* p,const th20::source::sprite::Vec2*,float,float){events.push_back({2,region->callback_target,*p,region->damage,region->field_90});return region->field_90;}
int __fastcall shot_custom_boundary(void* shot,void*,const th20::source::sprite::Vec3* p,const th20::source::sprite::Vec2* size,float angle,float radius){auto* values=static_cast<std::uint8_t*>(shot);*reinterpret_cast<float*>(values+0x70)=p->x+angle+radius;*reinterpret_cast<unsigned*>(values+0xa8)=reinterpret_cast<unsigned>(size);return *reinterpret_cast<int*>(values+0xac);}
struct DamageFixture final:d::Environment {
    int bomb_damage(gs::Context& context,const th20::source::sprite::Vec3&,const th20::source::sprite::Vec2*) override {if(*reinterpret_cast<void**>(static_cast<std::uint8_t*>(context.objects_04[5])+0x14))throw std::logic_error("Bomb active branch outside this fixture");return 0;} //actual477cf0 null-active branch
    int hit_callback(d::Region& region,const th20::source::sprite::Vec3& p,const th20::source::sprite::Vec2* size,float angle,float radius) override{return hit_boundary(&region,&p,size,angle,radius);}
    void damage_reward(const th20::source::sprite::Vec3& p,int damage) override{d::accumulate_damage_reward(gs::overlay_owner(0),p,damage,13);}
};
int wmain(int argc,wchar_t** argv){try{
    AddVectoredExceptionHandler(1,[](PEXCEPTION_POINTERS p)->LONG{if(p->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION)std::cerr<<"access violation VA="<<std::hex<<p->ContextRecord->Eip-mapped_image_base+0x400000<<" address="<<p->ExceptionRecord->ExceptionInformation[1]<<std::dec<<std::endl;return EXCEPTION_CONTINUE_SEARCH;});
    if(argc!=3)throw std::runtime_error("Usage: th20_damage_cpu_compare ORIGINAL.exe REPORT.json");const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");const auto pe_info=th20::parse_pe(bytes);Mapping image(bytes,pe_info);mapped_image_base=image.address();
    for(const auto& entry:pe_info.imports)if(auto address=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),entry.name.c_str()))*reinterpret_cast<FARPROC*>(mapped_image_base+entry.iat_rva)=address;
    *reinterpret_cast<HANDLE*>(mapped_image_base+0x1e5990)=GetProcessHeap();cpu<void>(0x452e00,reinterpret_cast<void*>(mapped_image_base+0x1c0240));
    sc::State scheduler;sc::initialize_state(scheduler);pe::function_controller=&scheduler;*reinterpret_cast<void**>(mapped_image_base+0x1b66d8)=&scheduler;
    std::mt19937 random(0x4c20d0);unsigned passed=0,failed=0;std::vector<std::string> failures;
    auto check=[&](const char* name,unsigned test,const void* a,const void* b,std::size_t size){if(!std::memcmp(a,b,size)){++passed;return;}++failed;if(failures.size()<30){std::ostringstream out;out<<name<<" test="<<test;for(std::size_t i=0;i<size;++i)if(static_cast<const unsigned char*>(a)[i]!=static_cast<const unsigned char*>(b)[i]){out<<" +0x"<<std::hex<<i<<" expected="<<unsigned(static_cast<const unsigned char*>(a)[i])<<" actual="<<unsigned(static_cast<const unsigned char*>(b)[i]);break;}failures.push_back(out.str());}};
    for(unsigned test=0;test<2048;++test){d::Region original,source;auto* words=reinterpret_cast<std::uint32_t*>(&original);for(unsigned i=0;i<sizeof(original)/4;++i)words[i]=random();source=original;cpu<void>(0x4bffb0,&original);d::construct_region(source);original.link.value=source.link.value;check("region_constructor",test,&original,&source,sizeof(source));
        for(unsigned i=0;i<sizeof(original)/4;++i)words[i]=random();source=original;const th20::source::sprite::Vec3 position{float(int(random()%2000)-1000)/13.f,float(int(random()%2000)-1000)/17.f,float(random()%10)};const float radius=float(random()%300)/7.f,growth=float(int(random()%300)-100)/11.f,angle=float(int(random()%20000)-10000)/100.f;const int frames=random(),damage=random();
        if(test%2){FloatingEnvironment::prepare();cpu<void>(0x4c1c50,&original,&position,radius,growth,angle,frames,damage);FloatingEnvironment::prepare();d::set_rectangle(source,position,radius,growth,angle,frames,damage);}else {cpu<void>(0x4c1d80,&original,&position,radius,growth,frames,damage);d::set_circle(source,position,radius,growth,frames,damage);}check("region_shape_initialization",test,&original,&source,sizeof(source));
    }
    auto* fixture=static_cast<d::HitCtrlInf*>(VirtualAlloc(nullptr,sizeof(d::HitCtrlInf),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));if(!fixture)throw std::bad_alloc();
    for(unsigned test=0;test<256;++test){std::memset(fixture,test%2?0xa5:0,sizeof(*fixture));cpu<void>(0x4bfee0,fixture);std::array<std::uint8_t,sizeof(d::HitCtrlInf)> expected;std::memcpy(expected.data(),fixture,expected.size());std::memset(fixture,test%2?0xa5:0,sizeof(*fixture));::new(static_cast<void*>(fixture))d::HitCtrlInf;std::memcpy(expected.data(),fixture,4);check("controller_constructor",test,expected.data(),fixture,sizeof(*fixture));}
    for(unsigned test=0;test<512;++test){fixture->next_handle=random();const auto before=fixture->next_handle;cpu<void>(0x4c0c60,fixture);const auto expected=fixture->next_handle;fixture->next_handle=before;fixture->advance_handle();check("handle_generation",test,&expected,&fixture->next_handle,4);}
    for(unsigned test=0;test<64;++test){const int index=test%2;fixture->select_context(index);gs::context(index).object_28=fixture;reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568)->contexts[index].object_28=fixture;fixture->initialize_pool(index);
        for(unsigned i=0;i<256;++i){std::array<std::uint8_t,sizeof(d::HitCtrlInf)> before,expected;std::memcpy(before.data(),fixture,before.size());auto* original=cpu<d::Region*>(0x4c0e60,fixture);std::memcpy(expected.data(),fixture,expected.size());std::memcpy(fixture,before.data(),before.size());auto* source=fixture->allocate();check("pool_allocate_pointer",test*256+i,&original,&source,4);check("pool_allocate_state",test*256+i,expected.data(),fixture,sizeof(*fixture));d::select_context(*source,index);}
        for(unsigned i=0;i<256;++i){auto& region=fixture->pool[(i*73)%256];std::array<std::uint8_t,sizeof(d::HitCtrlInf)> before,expected;std::memcpy(before.data(),fixture,before.size());cpu<void>(0x4c1f30,&region);std::memcpy(expected.data(),fixture,expected.size());std::memcpy(fixture,before.data(),before.size());d::retire(region);check("pool_retire_state",test*256+i,expected.data(),fixture,sizeof(*fixture));}
    }
    for(unsigned test=0;test<2048;++test){d::Region original{},source{};d::construct_region(original);original.link.value=nullptr;original.flags=random();original.radius=float(int(random()%10000)-5000)/31.f;original.radius_step=float(int(random()%1000)-500)/137.f;original.angle=float(int(random()%10000)-5000)/111.f;original.angle_step=.27f;original.lifetime={3,int(test%7),float(test%7),1};original.cooldown=random();original.last_target=random();
        auto* motion=reinterpret_cast<float*>(&original.motion);for(unsigned i=0;i<17;++i)motion[i]=float(int(random()%1000)-500)/113.f;original.motion.field_44=(test%16)|(test%3==0?32:0);source=original;
        state::clock_scale=(test%3+1)*.5f;*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=state::clock_scale;
        FloatingEnvironment::prepare();cpu<void>(0x4c03d0,&original);FloatingEnvironment::prepare();d::update(source);check("region_frame_motion_and_lifetime",test,&original,&source,sizeof(source));
    }
    for(unsigned test=0;test<96;++test){fixture->select_context(0);gs::context(0).object_28=fixture;reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568)->contexts[0].object_28=fixture;fixture->initialize_pool(0);
        for(unsigned i=0;i<32;++i){auto* region=fixture->allocate();d::select_context(*region,0);d::set_circle(*region,{float(i),0,0},10,.15f,1+int(i%8),7);region->motion.angle_1c=.2f;region->motion.field_18=1.23f;}
        state::clock_scale=(test%3+1)*.5f;*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=state::clock_scale;
        for(unsigned frame=0;frame<20;++frame){std::array<std::uint8_t,sizeof(d::HitCtrlInf)> before,expected;std::memcpy(before.data(),fixture,before.size());FloatingEnvironment::prepare();const auto original_result=cpu<int>(0x4c02d0,fixture);std::memcpy(expected.data(),fixture,expected.size());std::memcpy(fixture,before.data(),before.size());FloatingEnvironment::prepare();const auto source_result=fixture->update();check("controller_frame_return",test*20+frame,&original_result,&source_result,4);check("controller_frame_list_and_lifetime",test*20+frame,expected.data(),fixture,sizeof(*fixture));}
    }
    for(unsigned test=0;test<128;++test){fixture->select_context(0);gs::context(0).object_28=fixture;sc::initialize_list(fixture->active);sc::initialize_list(fixture->free);fixture->next_handle=1+test;
        auto* original=cpu<d::Region*>(0x4c0e60,fixture);d::Region expected=*original;cpu<void>(0x4c1f30,original);fixture->next_handle=1+test;auto* source=fixture->allocate();
        expected.link=source->link;expected.context=source->context;check("heap_fallback_region",test,&expected,source,sizeof(*source));d::retire(*source);
    }
    namespace g=th20::source::geometry;
    auto boolean=[&](const char* name,unsigned test,bool expected,bool actual){check(name,test,&expected,&actual,1);};
    auto coordinate=[&]{return float(int(random()%20000)-10000)/101.f;};auto dimension=[&]{return float(random()%10000)/137.f;};
    for(unsigned test=0;test<12000;++test){const auto x=coordinate(),y=coordinate(),w=dimension(),h=dimension(),angle=coordinate()/20.f,cx=coordinate(),cy=coordinate(),r=dimension(),ir=dimension(),other_angle=coordinate()/20.f;const int sides=3+random()%10;
        boolean("circle_point",test,geometry_cpu(0x456fe0,x,y,cx,cy,r),g::circle_point(x,y,cx,cy,r));
        boolean("rectangle_circle",test,geometry_cpu(0x457610,x,y,w,h,angle,cx,cy,r),g::rectangle_circle(x,y,w,h,angle,cx,cy,r));
        boolean("segment_segment",test,geometry_cpu(0x456920,x,y,cx,cy,w,h,r,ir),g::segment_segment(x,y,cx,cy,w,h,r,ir));
        boolean("rectangle_rectangle",test,geometry_cpu(0x4580c0,x,y,w,h,angle,cx,cy,r,ir,other_angle),g::rectangle_rectangle(x,y,w,h,angle,cx,cy,r,ir,other_angle));
        boolean("ellipse_point",test,geometry_cpu(0x457040,x,y,cx,cy,w,h,angle),g::ellipse_point(x,y,cx,cy,w,h,angle));
        boolean("ellipse_circle",test,geometry_cpu(0x4562e0,x,y,r,cx,cy,w,h,angle),g::ellipse_circle(x,y,r,cx,cy,w,h,angle));
        boolean("rectangle_ellipse",test,geometry_cpu(0x4578b0,x,y,w,h,angle,cx,cy,r,ir,other_angle),g::rectangle_ellipse(x,y,w,h,angle,cx,cy,r,ir,other_angle));
        boolean("polygon_point",test,geometry_cpu(0x4570f0,x,y,cx,cy,r,angle,sides),g::polygon_point(x,y,cx,cy,r,angle,sides));
        boolean("star_point",test,geometry_cpu(0x457380,x,y,cx,cy,r,ir,angle,sides),g::star_point(x,y,cx,cy,r,ir,angle,sides));
        boolean("polygon_circle",test,geometry_cpu(0x456690,x,y,w,cx,cy,r,angle,sides),g::polygon_circle(x,y,w,cx,cy,r,angle,sides));
        boolean("star_circle",test,geometry_cpu(0x4567d0,x,y,w,cx,cy,r,ir,angle,sides),g::star_circle(x,y,w,cx,cy,r,ir,angle,sides));
        boolean("rectangle_polygon",test,geometry_cpu(0x457c10,x,y,w,h,angle,cx,cy,r,other_angle,sides),g::rectangle_polygon(x,y,w,h,angle,cx,cy,r,other_angle,sides));
        boolean("rectangle_star",test,geometry_cpu(0x458670,x,y,w,h,angle,cx,cy,r,ir,other_angle,sides),g::rectangle_star(x,y,w,h,angle,cx,cy,r,ir,other_angle,sides));
        d::Region region{};region.flags=2*(test%8)|1;region.radius=r;region.inner_radius=ir;region.angle=angle;region.size={w,h};region.polygon_sides=sides;region.motion.position={cx,cy,0};th20::source::sprite::Vec3 target{x,y,0};th20::source::sprite::Vec2 size{ir,r};const auto* size_pointer=test%2?&size:nullptr;
        boolean("region_shape_dispatch",test,cpu<bool>(0x4c1030,&region,&target,size_pointer,other_angle,w),d::intersects(region,target,size_pointer,other_angle,w));
        size_pointer=test%2?nullptr:&size;boolean("region_shape_other_target",test,cpu<bool>(0x4c1030,&region,&target,size_pointer,other_angle,w),d::intersects(region,target,size_pointer,other_angle,w));
    }
    {
        // Only the separately owned Item creation boundary is substituted; the
        // original complete damage loop, lookup, reward counter and score run.
        auto* hook=reinterpret_cast<std::uint8_t*>(mapped_image_base+0xc45b0);std::array<std::uint8_t,5> old_hook;std::memcpy(old_hook.data(),hook,5);hook[0]=0xe9;*reinterpret_cast<int*>(hook+1)=reinterpret_cast<std::uintptr_t>(&item_boundary)-reinterpret_cast<std::uintptr_t>(hook+5);FlushInstructionCache(GetCurrentProcess(),hook,5);
        auto*& callback=*reinterpret_cast<void**>(mapped_image_base+0x170c7c);const auto old_callback=callback;callback=reinterpret_cast<void*>(&hit_boundary);
        std::array<std::uint8_t,0x220c> player_entity{};std::array<std::uint8_t,0x60> overlay{};std::array<std::uint8_t,0x3c> bomb{};std::array<std::uint8_t,0x134> enemy_owner{};std::array<std::uint8_t,0x100> parameters{};std::array<std::array<std::uint8_t,0x428>,2> enemies{};sc::Link links[2];
        auto& enemy_list=*reinterpret_cast<sc::List*>(enemy_owner.data()+0x108);sc::initialize_list(enemy_list);for(unsigned i=0;i<2;++i){*reinterpret_cast<unsigned*>(enemies[i].data()+0x88)=7+i*8;sc::initialize_link(links[i],reinterpret_cast<sc::Node*>(enemies[i].data()));sc::append(enemy_list,links[i]);}
        *reinterpret_cast<void**>(player_entity.data()+0x2208)=parameters.data();gs::context(0).objects_04[0]=player_entity.data();gs::context(0).objects_04[1]=enemy_owner.data();gs::context(0).objects_04[2]=reinterpret_cast<void*>(0x12345678);gs::context(0).objects_04[5]=bomb.data();gs::context(0).overlay_owner=reinterpret_cast<th20::source::runtime::CallbackOwner*>(overlay.data());gs::context(0).current_player=&gs::player(0);gs::context(0).object_28=fixture;
        auto* mapped_session=reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568);DamageFixture host;
        for(unsigned test=0;test<4096;++test){fixture->select_context(0);fixture->initialize_pool(0);const int count=test%32;
            for(int i=0;i<count;++i){auto* region=fixture->allocate();d::select_context(*region,0);d::set_circle(*region,{coordinate(),coordinate(),coordinate()},dimension()*2,0,random()%10,1+random()%40);region->flags=1|((test+i)%5)*2|((random()%8)<<4);region->size={dimension(),dimension()};region->inner_radius=dimension();region->angle=coordinate()/20;region->polygon_sides=3+random()%8;region->lifetime.previous=region->lifetime.current-(test%9!=0);region->damage_limit=test%3?9999999:10;region->total_damage=random()%20;region->period=1+random()%5;region->cooldown=int(random()%3)-1;region->damage_group=int(random()%7)-1;region->hit_callback=test%4?0:1;region->field_90=int(random()%61)-20;region->last_target=random()%3?0:7;}
            *reinterpret_cast<int*>(player_entity.data()+0x644)=4;*reinterpret_cast<int*>(player_entity.data()+0x648)=test%17?5:4;player_entity[0x204c]=test%3;*reinterpret_cast<int*>(overlay.data()+0x54)=test%4;*reinterpret_cast<int*>(overlay.data()+0x38)=test%101;*reinterpret_cast<int*>(overlay.data()+0x3c)=101;
            gs::player(0).fields_00[4]=test%3;gs::player(0).fields_00[5]=(test/3)%3;for(unsigned i=0;i<parameters.size()/4;++i)reinterpret_cast<int*>(parameters.data())[i]=1+random()%100;gs::player(0).fields_00[0]=999999900-test;gs::player(0).fields_00[1]=0;
            for(auto& enemy:enemies){*reinterpret_cast<unsigned*>(enemy.data()+0x22c)=random();*reinterpret_cast<unsigned*>(enemy.data()+0x354)=random();*reinterpret_cast<unsigned*>(enemy.data()+0x358)=random();}
            std::memcpy(mapped_session,&gs::session,sizeof(gs::session));const auto before_player=gs::player(0);const auto before_enemies=enemies;const auto before_overlay=overlay;std::array<std::uint8_t,sizeof(*fixture)> before,expected;std::memcpy(before.data(),fixture,before.size());
            th20::source::sprite::Vec3 target{coordinate(),coordinate(),coordinate()},expected_position{123,456,789},actual_position=expected_position;th20::source::sprite::Vec2 size{dimension(),dimension()};const auto* size_pointer=test%2?&size:nullptr;const float angle=coordinate()/20,radius=dimension();unsigned expected_flag=0xdeadbeef,actual_flag=expected_flag;const int preview=test%2;const unsigned identifier=test%4==0?0:test%4==1?7:test%4==2?15:999;
            events.clear();FloatingEnvironment::prepare();const int expected_result=cpu<int>(0x4c0480,fixture,&target,size_pointer,angle,radius,&expected_flag,&expected_position,preview,identifier);const auto expected_events=events;const auto expected_overlay=overlay;const auto expected_enemies=enemies;const auto expected_player=mapped_session->player_table.players[0];std::memcpy(expected.data(),fixture,expected.size());
            std::memcpy(fixture,before.data(),before.size());overlay=before_overlay;enemies=before_enemies;gs::player(0)=before_player;events.clear();FloatingEnvironment::prepare();const auto actual_result=d::calculate_damage(*fixture,target,size_pointer,angle,radius,&actual_flag,&actual_position,preview,identifier,host);
            check("damage_aggregate_result",test,&expected_result,&actual_result,4);check("damage_aggregate_flag",test,&expected_flag,&actual_flag,4);check("damage_aggregate_position",test,&expected_position,&actual_position,12);check("damage_aggregate_all_regions",test,expected.data(),fixture,sizeof(*fixture));check("damage_aggregate_enemy_marks",test,expected_enemies.data(),enemies.data(),sizeof(enemies));check("damage_aggregate_reward_counter",test,expected_overlay.data(),overlay.data(),overlay.size());check("damage_aggregate_score",test,&expected_player,&gs::player(0),sizeof(expected_player));
            const auto expected_count=expected_events.size(),actual_count=events.size();check("damage_aggregate_external_event_count",test,&expected_count,&actual_count,sizeof(expected_count));if(expected_count==actual_count)check("damage_aggregate_external_event_order",test,expected_events.data(),events.data(),events.size()*sizeof(Event));
        }
        callback=old_callback;std::memcpy(hook,old_hook.data(),5);FlushInstructionCache(GetCurrentProcess(),hook,5);
    }
    #include "hit_cases.inc"
    #include "lifecycle_cases.inc"
    std::ofstream report(argv[2],std::ios::binary);report<<"{\n  \"module\": \"damage_regions\",\n  \"original_sha256\": \""<<expected_sha<<"\",\n  \"passed\": "<<passed<<",\n  \"failed\": "<<failed<<",\n  \"failures\": [";for(std::size_t i=0;i<failures.size();++i){if(i)report<<',';report<<'\"'<<failures[i]<<'\"';}report<<"]\n}\n";
    fixture->~HitCtrlInf();VirtualFree(fixture,0,MEM_RELEASE);std::cout<<"passed="<<passed<<" failed="<<failed<<'\n';for(auto& failure:failures)std::cerr<<failure<<'\n';return failed?1:0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 2;}}
