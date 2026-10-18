// Isolated read-only original image oracle; never linked into production.
#define wmain unused_native_cpu_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "player.hpp"
#include "events.hpp"
#include "state_layout.hpp"
#include "owner.hpp"
#include "power.hpp"
#include "shots.hpp"
#include "firing.hpp"
#include "shot_callbacks.hpp"
#include "shot_geometry.hpp"
#include "shot_hit.hpp"
#include "stage_reset.hpp"
#include "frame_helpers.hpp"
#include "shot_controller_frame.hpp"
#include "option_frame.hpp"
#include "movement.hpp"
#include "death_state.hpp"
#include "frame.hpp"
#include "../effect_system/effect.hpp"
#include "../damage_regions/regions.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/sprite.hpp"
#include "initialize.hpp"
#include "shot_data.hpp"
#include "../archive/archive.hpp"
#include "source_hashes.hpp"
namespace pl=th20::source::player_entity;
namespace th20::source::player_entity {
int update_player(Player&){throw std::logic_error("CPU lifecycle fixture must use explicit frame services");}
int draw_player(Player&){throw std::logic_error("CPU test does not cross real renderer boundary");}
}
namespace {
unsigned firing_case=0;const char* firing_phase="prior tests";
LONG WINAPI firing_exception(EXCEPTION_POINTERS* p){std::cerr<<"Player oracle exception 0x"<<std::hex<<p->ExceptionRecord->ExceptionCode<<" EIP 0x"<<p->ContextRecord->Eip<<" address 0x"<<p->ExceptionRecord->ExceptionInformation[1]<<" base 0x"<<mapped_image_base<<std::dec<<" case "<<firing_case<<" phase "<<firing_phase<<std::endl;return EXCEPTION_EXECUTE_HANDLER;}
LONG WINAPI firing_vectored(EXCEPTION_POINTERS* p){if(p->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION)firing_exception(p);return EXCEPTION_CONTINUE_SEARCH;}
template<class R,class... A>R original(std::uint32_t va,void* self,A... arguments){return reinterpret_cast<R(__thiscall*)(void*,A...)>(mapped_image_base+va-0x400000)(self,arguments...);}
template<class T>void put(void* p,std::size_t offset,T value){std::memcpy(static_cast<std::uint8_t*>(p)+offset,&value,sizeof(value));}
struct Host final:pl::CollisionServices {
    std::array<std::uint8_t,0x1c0> hud{};bool hud_present=true;int hit_count=0;
    const void* boss_hud() override{return hud_present?hud.data():nullptr;}
    void hit(void*) override{++hit_count;}
};
struct Events final:pl::EventServices {
    th20::source::game_session::Session global;std::vector<int> calls;std::vector<std::pair<int,float>> sounds;
    bool active=false,selected=false;std::uint32_t random=0,color=0;int delay=0,reward=0,special_gain=0;void* reset_player=nullptr;
    th20::source::game_session::Context* affected=nullptr;th20::source::sprite::Vec3 origin{};
    th20::source::game_session::Session& session() override{return global;}
    void mark_enemies() override{calls.push_back(1);}
    void notify_secondary(void*) override{calls.push_back(2);}
    void sound(int id) override{calls.push_back(3);sounds.push_back({id,0.0f});}
    void sound_at(int id,float x) override{calls.push_back(9);sounds.push_back({id,x});}
    void spawn_hit_effect(th20::source::game_session::Context& context,const th20::source::sprite::Vec3& p) override{calls.push_back(4);affected=&context;origin=p;}
    void reset_player_animation(void* player) override{calls.push_back(5);reset_player=player;}
    std::uint32_t random_next() override{calls.push_back(6);return random;}
    void enqueue_graze(th20::source::game_session::Context& context,const th20::source::sprite::Vec3& p,std::uint32_t c,int d) override{calls.push_back(7);affected=&context;origin=p;color=c;delay=d;}
    bool special_active() override{calls.push_back(10);return active;}
    bool selected_enemy_present() override{calls.push_back(11);return selected;}
    void accumulate_reward(void* overlay,const th20::source::sprite::Vec3& p,int amount,int type) override{if(overlay!=global.contexts[0].overlay_owner||type!=13)throw std::logic_error("Wrong graze reward owner/type");calls.push_back(8);origin=p;reward=amount;}
    void add_special_items(th20::source::game_session::Player& player,int amount) override{if(&player!=global.contexts[0].current_player)throw std::logic_error("Graze reward must use global player0");calls.push_back(12);special_gain=amount;}
};
struct OwnerHost final:pl::PlayerServices {
    th20::source::scheduler::State state{};th20::source::scheduler::Environment environment;
    bool preserve=false;std::vector<int> calls,unloaded;std::vector<pl::Shot*> retired;
    OwnerHost(){th20::source::scheduler::initialize_state(state);}
    th20::source::scheduler::State& scheduler() override{return state;}
    th20::source::scheduler::Environment& scheduler_environment() override{return environment;}
    bool preserve_animation_files() override{calls.push_back(1);return preserve;}
    void unload_animation_file(int slot,bool p) override{if(p!=preserve)throw std::logic_error("wrong ANM preservation");calls.push_back(2);unloaded.push_back(slot);}
    void delete_animation(std::uint32_t& handle) override{calls.push_back(3);handle=0;}
    void destroy_animation(th20::source::sprite::Animation&) override{calls.push_back(4);}
    void retire_shot(pl::Shot& shot) override{retired.push_back(&shot);th20::source::scheduler::unlink(shot.link);}
};
struct Powers final:pl::PowerServices {
    pl::Player* player=nullptr;std::uint32_t selected_view=0,next=1;int variants=0,initialized=0;
    std::array<th20::source::sprite::Animation,40> animations{};
    std::vector<std::uint32_t> removed,interrupted;std::vector<std::array<int,3>> spawns;
    void select_view(int i) override{selected_view=i;}
    pl::Player& global_player() override{return *player;}
    void interrupt(std::uint32_t handle,int event) override{if(event!=1)throw std::logic_error("power interruption event");interrupted.push_back(handle);}
    void delete_animation(std::uint32_t& handle) override{removed.push_back(handle);handle=0;}
    th20::source::sprite::Animation& animation(std::uint32_t& handle) override{return animations.at(handle);}
    int script_variant() override{++variants;return player->focused_204c?1:0;}
    th20::source::sprite::Vec2 option_offset(th20::source::game_session::Context&,int level,int index,bool focus) override{return {float(level+index),focus?-3.5f:2.25f};}
    std::uint32_t spawn(th20::source::sprite::AnimationFile&,int script,int layer,std::uint32_t flags) override{spawns.push_back({script,layer,int(flags)});return next++;}
    void initialize_option(pl::Option&,int index) override{if(index!=initialized++)throw std::logic_error("wrong option initialization order");}
};
struct Shots final:pl::ShotServices {
    std::vector<int> calls;pl::Shot* released=nullptr;
    void delete_animation(std::uint32_t& handle) override{calls.push_back(1);handle=0;}
    void retire_damage(std::uint32_t& handle) override{calls.push_back(2);handle=0;}
    void release(pl::Shot* p) override{calls.push_back(3);if(p->flags||p->link.owner)throw std::logic_error("shot release before unlink/reset");released=p;}
};
}
int wmain(int argc,wchar_t** argv){SetUnhandledExceptionFilter(firing_exception);AddVectoredExceptionHandler(1,firing_vectored);
 try{
    if(argc!=3)throw std::runtime_error("Usage: player_entity_cpu_compare VERIFIED_TH20.exe REPORT.json");
    const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original hash mismatch");
    Mapping mapped(bytes,th20::parse_pe(bytes));mapped_image_base=mapped.address();
    unsigned failed=0;std::map<std::string,unsigned> counts;std::map<std::string,std::string> resource_hashes;std::mt19937 rng(0x4f8ce020);
    const auto check=[&](const char* name,bool passed){++counts[name];if(!passed){if(failed<20)std::cerr<<"FAIL "<<name<<'\n';++failed;}};
    for(unsigned n=0;n<5000;++n){
        pl::Option option,option_source;for(auto& byte:reinterpret_cast<std::uint8_t(&)[sizeof(option)]>(option))byte=static_cast<std::uint8_t>(rng());option_source=option;
        original<void>(0x4f49c0,&option);pl::construct_option(option_source);check("Option_ctor_all300bytes",std::memcmp(&option,&option_source,sizeof(option))==0);
        pl::Shot shot,shot_source;for(auto& byte:reinterpret_cast<std::uint8_t(&)[sizeof(shot)]>(shot))byte=static_cast<std::uint8_t>(rng());shot_source=shot;
        original<void>(0x4f4cb0,&shot);pl::construct_shot(shot_source);check("Shot_ctor_all292bytes_preserving_padding",std::memcmp(&shot,&shot_source,sizeof(shot))==0);
        pl::Feedback feedback,feedback_source;for(auto& byte:reinterpret_cast<std::uint8_t(&)[sizeof(feedback)]>(feedback))byte=static_cast<std::uint8_t>(rng());feedback_source=feedback;
        original<void>(0x4f4580,&feedback);pl::construct_feedback(feedback_source);check("Feedback_ctor_all92bytes_preserving_padding",std::memcmp(&feedback,&feedback_source,sizeof(feedback))==0);
    }
    std::vector<std::uint8_t> shot_storage(sizeof(pl::ShotController));
    for(unsigned n=0;n<500;++n){
        for(auto& byte:shot_storage)byte=static_cast<std::uint8_t>(rng());const auto before=shot_storage;
        original<void>(0x4f4b80,shot_storage.data());const auto expected=shot_storage;shot_storage=before;
        pl::construct_shot_controller(*reinterpret_cast<pl::ShotController*>(shot_storage.data()));
        check("ShotController_ctor_all75160bytes_pool_and_actual_list_anchors",shot_storage==expected);
    }
    std::vector<std::uint8_t> owner_storage(sizeof(pl::Player));OwnerHost owner_host;
    for(unsigned n=0;n<500;++n){
        for(auto& byte:owner_storage)byte=static_cast<std::uint8_t>(rng());const auto before=owner_storage;
        original<void>(0x4f46a0,owner_storage.data());const auto expected=owner_storage;owner_storage=before;
        //The original vptr is an evidence address; field comparison starts
        //after the independently recovered polymorphic base.
        std::memcpy(owner_storage.data(),expected.data(),16);pl::construct_player_fields(*reinterpret_cast<pl::Player*>(owner_storage.data()));
        if(std::memcmp(owner_storage.data(),expected.data(),0x1485c)!=0&&failed<20){std::size_t offset=16;while(owner_storage[offset]==expected[offset])++offset;std::cerr<<"Player ctor byte +"<<std::hex<<offset<<std::dec<<'\n';}
        check("Player_ctor_all84060bytes_after_base_vptr_normalization",std::memcmp(owner_storage.data(),expected.data(),0x1485c)==0);
        auto& shots=reinterpret_cast<pl::Player*>(owner_storage.data())->shots;
        for(auto& count:shots.counters_12468)count=rng();shots.timer_12400.current=17;shots.timer_12410.current=-23;shots.timer_12420.current=91;
        const auto clear_before=owner_storage;original<void>(0x504950,&shots);const auto clear_expected=owner_storage;owner_storage=clear_before;
        pl::clear_shots(shots,owner_host);check("clear_shots_empty_actual_list_all84060bytes",std::memcmp(owner_storage.data(),clear_expected.data(),0x1485c)==0);
    }
    for(unsigned pattern=0;pattern<16;++pattern){
        OwnerHost service;service.preserve=(pattern&1)!=0;
        //Factory4f32b0 zeroes the allocation before invoking the constructor.
        std::memset(owner_storage.data(),0,owner_storage.size());auto* owner=new(owner_storage.data())pl::Player(service);
        owner->view_index=pattern%2;owner->entity_flags=(pattern&2)?0x100u:0u;
        for(unsigned i=0;i<(pattern>>2);++i){auto& shot=owner->shots.pool[i];shot.link.value=reinterpret_cast<th20::source::scheduler::Node*>(&shot);th20::source::scheduler::append(owner->shots.active,shot.link);}
        owner->~Player();
        check("source_Player_destructor_child_order",service.calls==((pattern&2)?std::vector<int>{1,3,4}:std::vector<int>{1,2,2,3,4}));
        check("source_Player_destructor_retained_ANM_slots",service.unloaded==((pattern&2)?std::vector<int>{}:std::vector<int>{int(pattern%2)+9,int(pattern%2)+10}));
        check("source_Player_destructor_intrusive_retirement",service.retired.size()==(pattern>>2));
    }
    std::vector<std::uint8_t> player(0x1485c);th20::source::game_session::Context context{};th20::source::game_session::Player record{};context.current_player=&record;
    put(player.data(),0x14858,&context);Host host;
    const auto finite=[&](){return static_cast<float>(static_cast<int>(rng()%1001)-500)/8.0f;};
    constexpr std::array<std::uint32_t,16> special{0,0x80000000,0x3f800000,0xbf800000,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7fa12345,1,0x80000001,0x00800000,0x80800000,0x7f7fffff,0x3f000000,0x42480000};
    const auto sample=[&](unsigned test){if(test%4)return finite();const auto bits=special[rng()%special.size()];float value;std::memcpy(&value,&bits,4);return value;};
    for(unsigned n=0;n<6000;++n){
        const th20::source::sprite::Vec2 input{sample(n),sample(n)};pl::Fixed2 cpu{},actual=pl::fixed_coordinates(input);original<void>(0x4fee50,&cpu,&input);check("fixed_coordinates_CVTTSS2SI_special_IEEE",std::memcmp(&cpu,&actual,8)==0);
        const pl::Fixed2 a{std::int32_t(rng()),std::int32_t(rng())},b{std::int32_t(rng()),std::int32_t(rng())};actual=pl::add_fixed_coordinates(a,b);original<void>(0x4f58f0,const_cast<pl::Fixed2*>(&a),&cpu,&b);check("fixed_add_modulo32",std::memcmp(&cpu,&actual,8)==0);
        for(auto [va,source]:std::array<std::pair<std::uint32_t,int(*)(th20::source::game_session::Player&)>,4>{{{0x4993b0,pl::clamped_power},{0x4b81d0,pl::clamped_power_unit},{0x4b8210,pl::clamped_maximum_power},{0x4b81a0,pl::power_level}}}){
            auto baseline=record;for(auto& value:baseline.fields_30)value=rng();auto source_player=baseline;int got=original<int>(va,&baseline),expected=source(source_player);check("power_Player_clamps_division_and_all240bytes",got==expected&&std::memcmp(&baseline,&source_player,sizeof(baseline))==0);
        }
        const auto before=player;original<void>(0x4ffd80,player.data(),input.x,input.y);const auto expected=player;player=before;pl::set_position(*reinterpret_cast<pl::Player*>(player.data()),input.x,input.y);check("set_position_fixed_all84060bytes_and_all22_option_markers",player==expected);
    }
    std::array<std::uint8_t,0x700> sprite_prefix{};put(reinterpret_cast<void*>(mapped_image_base),0x1c0028,sprite_prefix.data());
    auto& owner=*reinterpret_cast<pl::Player*>(owner_storage.data());std::array<std::uint8_t,0x5d4> shot_data{};th20::source::sprite::AnimationFile file;
    for(unsigned n=0;n<500;++n){
        pl::construct_player_fields(owner);owner.context=&context;context.objects_04[0]=&owner;owner.view_index=n%2;owner.fields_674[3]=n%3;
        owner.shots.field_1255c=rng();for(auto& option:owner.secondary_options){option.state=rng();option.fields_f4[3]=rng();option.fields_f4[5]=rng();}
        record.fields_30[0]=n%100;record.fields_30[1]=rng();record.fields_30[2]=rng();
        const auto before=owner_storage;const auto before_record=record;original<void>(0x4faca0,&owner,n%2?-1:2);const auto expected=owner_storage;const auto expected_record=record;owner_storage=before;record=before_record;
        Powers service;service.player=&owner;pl::refresh_power(owner,n%2?-1:2,service);
        check("power_full_entry_zero_level_all84060bytes_and240recordbytes",owner_storage==expected&&std::memcmp(&record,&expected_record,sizeof(record))==0);
        check("power_full_entry_actual_selected_view",service.selected_view==*reinterpret_cast<std::uint32_t*>(sprite_prefix.data()+0x6c4));
    }
    for(unsigned pattern=0;pattern<40;++pattern){
        pl::construct_player_fields(owner);owner.context=&context;context.objects_04[0]=&owner;owner.shot_data=shot_data.data();owner.animation_file=&file;
        const unsigned level=pattern%5;record.fields_30[0]=level*100;record.fields_30[2]=100;owner.fields_674[3]=(pattern&8)?level:7;owner.focused_204c=pattern&1;owner.fixed_position={1000,-2000};
        put(shot_data.data(),0x88,90);put(shot_data.data(),0x8c,91);put(shot_data.data(),0xa8,120);put(shot_data.data(),0xac,121);
        Powers service;service.player=&owner;pl::refresh_power(owner,-1,service);
        const bool changed=(pattern&8)==0;const unsigned option_spawns=changed?level:0,extra_spawns=level==4?4:0;
        check("source_power_ANM_calls_match_options_and_full_power",service.spawns.size()==option_spawns+extra_spawns&&service.initialized==int(option_spawns));
        check("source_power_cache_and_ShotController_flag",owner.fields_674[3]==level&&(owner.shots.field_1255c&1));
        bool options=true;for(unsigned i=0;i<option_spawns;++i){const auto& option=owner.options[i];options&=option.vector_70.x==1000+int(level+i)*128&&option.vector_70.y==-2000+(owner.focused_204c?-448:288)&&option.vector_78.x==option.vector_70.x&&option.state==2;}
        check("source_power_global_focus_and_fixed_option_positions",options);
        bool scripts=true;for(unsigned i=0;i<extra_spawns;++i)scripts&=service.spawns[i]==std::array<int,3>{owner.focused_204c?121:120,-1,2};for(unsigned i=extra_spawns;i<service.spawns.size();++i)scripts&=service.spawns[i]==std::array<int,3>{owner.focused_204c?91:90,14,0};check("source_power_script_variants_layers_and_flags",scripts);
    }
    for(unsigned pattern=0;pattern<8;++pattern){
        pl::construct_shot_controller(owner.shots);auto& shot=owner.shots.pool[0];shot.context=&context;shot.owner=&owner.shots;shot.flags=(pattern&1)?0x1000000u:0;shot.link.value=reinterpret_cast<th20::source::scheduler::Node*>(&shot);th20::source::scheduler::append(owner.shots.active,shot.link);context.object_28=(pattern&2)?&record:nullptr;
        Shots service;pl::retire_shot(shot,service);check("source_shot_retire_child_order",service.calls==((pattern&1)?(pattern&2)?std::vector<int>{1,2,3}:std::vector<int>{1,3}:(pattern&2)?std::vector<int>{1,2}:std::vector<int>{1}));
        check("source_shot_free_list_or_heap_release",!owner.shots.active.sentinel.next&&shot.flags==0&&((pattern&1)?service.released==&shot&&owner.shots.free.sentinel.next==nullptr:owner.shots.free.sentinel.next==&shot.link&&owner.shots.free.tail==&shot.link&&shot.link.owner==&owner.shots.free));
    }
    #include "initialize_cpu_cases.inc"
    for(unsigned n=0;n<12000;++n){
        const th20::source::sprite::Vec3 position{finite(),finite(),finite()};auto origin=n%11==0?position:th20::source::sprite::Vec3{finite(),finite(),finite()};
        put(player.data(),0x614,position);put(player.data(),0x204c,static_cast<std::uint8_t>(rng()));put(player.data(),0x14,rng());
        const float expected=original<float>(0x4ff350,player.data(),&origin),actual=pl::angle_to_player(player.data(),origin);
        check("angle_to_player_exact_float_and_zero_vector",std::memcmp(&expected,&actual,4)==0);
        const auto got=pl::position(player.data());check("position_view_exact12bytes",std::memcmp(&got,&position,12)==0);
        check("focus_flag",original<bool>(0x4ff5e0,player.data())==pl::focused(player.data()));
        check("expanded_collision_flag",original<unsigned>(0x4ff800,player.data())==static_cast<unsigned>(pl::expanded_collision(player.data())));
        put(&record,0xe8,rng());auto source=record;
        check("collision_percent_signed_clamp_all_player_bytes",original<int>(0x4ff490,&record)==pl::collision_percent(source)&&std::memcmp(&source,&record,sizeof(record))==0);
        const int delta=static_cast<int>(rng());put(&record,0xe4,rng());source=record;original<void>(0x4fe8d0,&record,delta);pl::add_graze_count(source,delta);
        check("graze_counter_wrapped_add_clamp_all_Player_bytes",std::memcmp(&record,&source,sizeof(record))==0);
        //Mapped original CRT error handling is not initialized. Nonfinite
        //atan2/sin inputs are excluded; scalar collision inputs still include
        //the special IEEE values without invoking those CRT domain errors.
        put(player.data(),0x614,th20::source::sprite::Vec3{sample(n),sample(n),sample(n)});origin={sample(n),sample(n),sample(n)};
        put(player.data(),0x2094,sample(n));put(player.data(),0x2098,sample(n));put(player.data(),0x2238,sample(n));
        put(player.data(),0x209c,th20::source::sprite::Vec3{sample(n),sample(n),sample(n)});put(player.data(),0x20a8,th20::source::sprite::Vec3{sample(n),sample(n),sample(n)});
        //Positive invulnerability prevents the real hit routine reaching its
        //not-yet-reconstructed ownership graph; all collision bodies unpatched.
        put(player.data(),0x2054,1+int(n%4));put(player.data(),0x10,int(n%6));
        host.hud_present=n%3!=0;put(host.hud.data(),0x1bc,n%7==0?1u:0u);
        *reinterpret_cast<const void**>(mapped_image_base+0x1c06a4)=host.boss_hud();
        const th20::source::sprite::Vec2 size{sample(n),sample(n)};const float radius=sample(n),angle=finite();const int preview=n%5==0?1:0;
        for(unsigned shape=0;shape<3;++shape){
            put(&record,0xe8,static_cast<std::int32_t>(rng()%151)-25);const auto before=player;const auto record_before=record;
            int cpu=shape==0?original<int>(0x4f8ce0,player.data(),&origin,&size,preview):shape==1?original<int>(0x4f8ff0,player.data(),&origin,radius,preview):original<int>(0x4f91d0,player.data(),&origin,angle,size.x,size.y,preview);
            const auto expected_player=player;const auto expected_record=record;player=before;record=record_before;
            const int source_result=shape==0?pl::collide_axis_aligned(player.data(),origin,size,preview,host):shape==1?pl::collide_circle(player.data(),origin,radius,preview,host):pl::collide_rectangle(player.data(),origin,angle,size.x,size.y,preview,host);
            if(cpu!=source_result&&failed<20)std::cerr<<"collision n="<<n<<" shape="<<shape<<" cpu="<<cpu<<" source="<<source_result<<'\n';
            check(shape==0?"axis_collision_return":shape==1?"circle_collision_return":"rectangle_collision_return",cpu==source_result);
            check("collision_preserves_all_player_entity_bytes",player==expected_player);
            check("collision_mutates_real_Player_percent_only",std::memcmp(&record,&expected_record,sizeof(record))==0);
            check("CPU_collision_domain_no_hit_boundary",host.hit_count==0);
        }
    }
    // Actual hit dispatch is source-tested across all shapes, live/preview,
    // suppression and invulnerability without claiming original hit-body work.
    for(unsigned shape=0;shape<3;++shape)for(unsigned pattern=0;pattern<16;++pattern){
        put(player.data(),0x614,th20::source::sprite::Vec3{});put(player.data(),0x2094,4.0f);put(player.data(),0x2098,4.0f);put(player.data(),0x204c,std::uint8_t{0});put(player.data(),0x14,std::uint32_t{0});
        put(player.data(),0x209c,th20::source::sprite::Vec3{4,4,4});put(&record,0xe8,100);put(player.data(),0x10,pattern&1?2:1);put(player.data(),0x2054,pattern&2?5:0);
        host.hud_present=true;put(host.hud.data(),0x1bc,pattern&4?1u:0u);host.hit_count=0;const int preview=pattern&8?1:0;
        const int got=shape==0?pl::collide_axis_aligned(player.data(),{},th20::source::sprite::Vec2{4,4},preview,host):shape==1?pl::collide_circle(player.data(),{},4,preview,host):pl::collide_rectangle(player.data(),{},0,4,4,preview,host);
        const int expected=(pattern&4)?0:preview?2:(pattern&1)?0:(pattern&2)?(shape==2?0:1):1;
        check("source_collision_hit_dispatch_return",got==expected);check("source_collision_hit_dispatch_count",host.hit_count==(!(pattern&15)?1:0));
    }
    for(unsigned pattern=0;pattern<8;++pattern){
        Events events;std::array<std::uint8_t,0x60> overlay{};events.global.contexts[0].overlay_owner=reinterpret_cast<th20::source::runtime::CallbackOwner*>(overlay.data());
        put(player.data(),0x14858,&context);put(player.data(),0x14,pattern&2?8u:0u);put(player.data(),0x10,1);put(player.data(),0x2204,91);
        put(player.data(),0x614,th20::source::sprite::Vec3{3,5,7});th20::recovered::timer_set(*reinterpret_cast<th20::recovered::Timer*>(player.data()+0x644),15);th20::recovered::timer_set(*reinterpret_cast<th20::recovered::Timer*>(player.data()+0x2050),19);
        put(overlay.data(),0x54,pattern&1?1:0);pl::hit(player.data(),events);
        const std::vector<int> order=pattern&1?std::vector<int>{1,2,3}:pattern&2?std::vector<int>{1,2,4,2,5}:std::vector<int>{1,2,3,4,2,5};
        check("source_hit_shared_owner_and_effect_order",events.calls==order);
        check("source_hit_invulnerability_timer",*reinterpret_cast<int*>(player.data()+0x2054)==(pattern&1?4:6));
        check("source_hit_overlay_mode_or_death_state",*reinterpret_cast<int*>(overlay.data()+0x54)==(pattern&1?2:0)&&*reinterpret_cast<int*>(player.data()+0x10)==(pattern&1?1:4));
        check("source_hit_animation_reset_and_frame_timer",pattern&1?events.reset_player==nullptr&&*reinterpret_cast<int*>(player.data()+0x648)==15:events.reset_player==player.data()&&*reinterpret_cast<int*>(player.data()+0x648)==0&&*reinterpret_cast<int*>(player.data()+0x2204)==8);
        check("source_hit_sound_suppression_and_special_sound",pattern&1?events.sounds==std::vector<std::pair<int,float>>{{45,0.0f}}:pattern&2?events.sounds.empty():events.sounds==std::vector<std::pair<int,float>>{{2,0.0f}});
        check("source_hit_effect_origin_and_context",pattern&1?events.affected==nullptr:events.affected==&context&&events.origin.x==3&&events.origin.y==5&&events.origin.z==7);
    }
    for(unsigned pattern=0;pattern<64;++pattern){
        Events events;std::array<std::uint8_t,0x60> overlay{};events.global.contexts[0].overlay_owner=reinterpret_cast<th20::source::runtime::CallbackOwner*>(overlay.data());
        events.global.contexts[0].current_player=&events.global.player_table.players[0];events.global.contexts[1].current_player=&events.global.player_table.players[1];
        auto& selected_context=events.global.contexts[pattern&1];put(player.data(),0x14858,&selected_context);put(selected_context.current_player,0xe4,31);
        events.active=(pattern&2)!=0;events.selected=(pattern&4)!=0;events.random=rng();const auto color=rng();const th20::source::sprite::Vec3 p{3,7,11};pl::graze(player.data(),p,color,events);
        check("source_graze_rng_queue_reward_sound_order",events.calls==std::vector<int>({6,7,10,8,11,12,9}));
        check("source_graze_current_context_counter",*reinterpret_cast<int*>(reinterpret_cast<std::uint8_t*>(selected_context.current_player)+0xe4)==32);
        check("source_graze_delay_uses_one_shared_random_draw",events.delay==static_cast<int>(events.random%4));
        check("source_graze_color_forces_alpha_only",events.color==((color&0xffffffu)|0xff000000u));
        check("source_graze_actual_context_and_position",events.affected==&selected_context&&events.origin.x==3&&events.origin.y==7&&events.origin.z==11);
        check("source_graze_special_reward",events.reward==(events.active?1500:1000)&&events.special_gain==(events.selected?50:10));
        check("source_graze_sound_pan",events.sounds==std::vector<std::pair<int,float>>{{42,3.0f}});
    }
    #include "firing_cpu_cases.inc"
    #include "firing_native_services_cpu_cases.inc"
    #include "shot_callbacks_cpu_cases.inc"
    #include "shot_geometry_cpu_cases.inc"
    unsigned total=0,source_checks=0;for(auto [name,count]:counts){total+=count;if(name.rfind("source_",0)==0)source_checks+=count;}
    std::ofstream out(argv[2]);out<<"{\"status\":"<<th20::json_string(failed?"failed":"passed")<<",\"checks\":"<<total<<",\"original_cpu_checks\":"<<total-source_checks<<",\"source_assertions\":"<<source_checks<<",\"failed\":"<<failed<<",\"original_sha256\":"<<th20::json_string(expected_sha)<<",\"coverage\":{";bool first=true;
    for(auto [name,count]:counts){if(!first)out<<',';first=false;out<<th20::json_string(name)<<':'<<count;}out<<"},\"source_sha256\":{";first=true;
    for(auto source:source_hashes){if(!first)out<<',';first=false;out<<th20::json_string(source.path)<<':'<<th20::json_string(source.sha);}out<<"},\"resource_sha256\":{";first=true;for(auto [name,hash]:resource_hashes){if(!first)out<<',';first=false;out<<th20::json_string(name)<<':'<<th20::json_string(hash);}
    out<<"},\"scope\":\"Original unpatched Player/Option/Shot/Feedback constructors, full ShotController initialization and context selection, fixed-point coordinates, power clamps, full zero-power refresh entry, scalar accessors and three collision bodies. Collision scalar inputs include infinity, NaN and subnormal values; trig angles are finite. Nonfinite trig tests terminated the isolated mapped-image process and are excluded. Positive invulnerability excludes original hit-body effects. Source assertions separately cover full hit/graze/power/shot-retirement orchestration and Player factory/load/teardown with explicit recorded ANM, HUD, damage and allocation boundaries. Complete firing entry504d40, initializer504e90, all31 initialization,16 update and10 hit callback indices, plus four ray/segment geometry functions, actual Effect handle registry and complete stage reset4fb450 with zero-power refresh service boundary are covered. ANM allocation/VM, DamageRegion allocation/retirement, readonly enemy queries, RNG draws, sound/device endpoints and empty-list Bullet/Laser cancellation are explicitly shared native service boundaries in the applicable oracle groups. Fixed motion, Feedback update, Shot single-frame update and viewport predicates, ShotController complete firing/frame loops, complete Option update and Player alive movement are also compared. ANM creation/VM, gate getters, retirement and weapon virtual endpoints are explicit shared services in those groups; movement uses one live Option. Death, counters and effect reservation have full-object checks. Main frame4f7430 states0..7 and17 timer boundary values are compared with unchanged dispatch, Movement, Death, ShotController and Feedback bodies. This group alone redirects Bullet/Laser cancellation, Damage allocation/activation, Item creation, Bomb eligibility/trigger, Pause finish and HUD lives/bombs notifications to explicit call-recording endpoints in the isolated image, restored afterwards. ANM execution and scalar interpolation are shared native services. Main-frame shots/options start inactive, and power branches preserve the already selected Option level; active lower-body cases are independently covered. Both actual archive SHT files are relocated; GPU upload, higher-level power animation creation and whole-game equivalence are not asserted.\"}\n";
    std::cout<<total<<" PlayerEntity checks, "<<failed<<" failed\n";return failed?1:0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}
}
