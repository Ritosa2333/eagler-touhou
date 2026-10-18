// Isolated CPU comparison of scalar state/generation and53fe60 record parsing.
// Resource/device calls are forbidden in this fixture, not production fallbacks.
#define wmain unused_native_cpu_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "script_loader.hpp"
#include "enemy_frame.hpp"
#include "enemy_state.hpp"
#include "enemy_variables.hpp"
#include "enemy_update.hpp"
#include "enemy_interpolation.hpp"
#include "enemy_movement.hpp"
#include "enemy_damage_helpers.hpp"
#include "enemy_damage_test_fixture.hpp"
#include "enemy_entity.hpp"
#include "enemy_reads.hpp"
#include "enemy_opcode_animation.hpp"
#include "enemy_opcode_movement.hpp"
#include "enemy_shot.hpp"
#include "enemy_opcode_laser.hpp"
#include "enemy_opcode_misc.hpp"
#include "enemy_mesh.hpp"
#include "enemy_defeat.hpp"
#include "../player_entity/player.hpp"
#include "enemy_opcode_state.hpp"
#include "enemy_drop.hpp"
#include "../runtime_state/motion.hpp"
#include "enemy_source_hashes.hpp"
#include "../archive/resource_manager.hpp"
#include "test_services.hpp"
namespace scheduler=th20::source::scheduler;
namespace th20::source::resources {
std::optional<Bytes> read(const char*,bool){throw std::logic_error("Unexpected archive read in parser CPU fixture");}
}
namespace th20::source::sprite {
AnimationFile* load_animation_file(Controller&,std::int32_t,const char*,runtime::Log&,std::uint32_t&){throw std::logic_error("Unexpected ANM load in parser CPU fixture");}
void unload_animation_file(Controller&,int){throw std::logic_error("Unexpected ANM unload in CPU fixture");}
void mark_file_animations(Controller&,AnimationFile*,bool){throw std::logic_error("Unexpected ANM retirement in CPU fixture");}
}
struct EnemyFixture final:gp::EnemyServices {
    rt::Log log_data;scheduler::State state{};scheduler::Environment environment;std::uint32_t flags=0;
    rt::Log& log() override{return log_data;}
    scheduler::State& scheduler_state() override{return state;}
    scheduler::Environment& scheduler_environment() override{return environment;}
    th20::source::sprite::Controller& sprites() override{throw std::logic_error("Unexpected sprites in parser CPU fixture");}
    std::uint32_t& graphics_flags() override{return flags;}
    int update_enemy(gp::EnemyController&) override{throw std::logic_error("Unexpected enemy update");}
    void draw_enemy_overlay() override{throw std::logic_error("Unexpected enemy draw");}
    void select_layer(int,int) override{throw std::logic_error("Unexpected select layer");}
    void retire_entity(void*) override{throw std::logic_error("Unexpected Enemy destruction");}
    th20::source::sprite::AnimationFile* existing_animation(gp::EnemyController&,unsigned) override{throw std::logic_error("Unexpected ANM query");}
};
struct FrameFixture final:gp::EnemyFrameServices {
    th20::source::sprite::PooledAnimation* pool=nullptr;
    float scale=1.0f;
    const float* timer_rate() override{return &scale;}
    float& clock_scale() override{return scale;}
    void update_boss_time(std::int32_t,std::int32_t) override{throw std::logic_error("Unexpected HUD call in scalar CPU fixture");}
    int update_entity_state(void* state) override {
        // The compared original4a8760 reaches the real4a8260 early-out at
        //004a8287. This fixture records only that already-updated input domain.
        if(!(*reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(state)+0x2cc)&4u))throw std::logic_error("Unrecovered entity simulation domain");
        return 0;
    }
    void update_special_objects() override{throw std::logic_error("Unexpected special manager in scalar CPU fixture");}
    th20::source::sprite::Animation* animation(std::uint32_t handle) override{
        if(!pool)throw std::logic_error("Unexpected animation in scalar CPU fixture");
        if(!handle)return nullptr;const auto index=handle&0xffffu;
        if(index>=8)throw std::logic_error("Invalid test animation slot");
        auto& slot=pool[index];return slot.active&&slot.animation.handle==handle?&slot.animation:nullptr;
    }
};
struct UpdateFixture final:gp::EnemyUpdateServices {
    FrameFixture& frames;explicit UpdateFixture(FrameFixture& host):frames(host){}
    const float* timer_rate() override{return frames.timer_rate();}
    float script_delta(const th20::recovered::Timer&) override{throw std::logic_error("Movement already processed in CPU domain");}
    th20::source::sprite::Animation* animation(std::uint32_t handle) override{return frames.animation(handle);}
    int move(gp::EnemyState&) override{throw std::logic_error("Movement already processed in CPU domain");}
    int run_scripts(void*,float) override{throw std::logic_error("ECL already processed in CPU domain");}
    int damage(gp::EnemyState& state) override{
        if(animation(state.animations[0].handle))throw std::logic_error("Active damage resolution excluded from CPU domain");
        state.animations[0].handle=0;return 0; //actual4a5df0 returns44ced0 zero on this input domain
    }
    void mesh(gp::EnemyState& state) override{if(state.fields_2c8[3])throw std::logic_error("Active mesh update excluded from CPU domain");}
};
struct MovementFixture final:gp::EnemyMovementServices {
    FrameFixture& frames;th20::source::sprite::Vec3 offset{};explicit MovementFixture(FrameFixture& host):frames(host){}
    const float* timer_rate() override{return frames.timer_rate();}
    float clock_scale() override{return frames.scale;}
    th20::source::sprite::Animation* animation(std::uint32_t handle) override{return frames.animation(handle);}
    th20::source::sprite::Vec3 viewport_offset() override{return offset;}
    th20::source::sprite::AnimationFile& animation_file(gp::EnemyState&,unsigned) override{throw std::logic_error("Direction-switch allocation is excluded from this CPU domain");}
    void delete_animation(std::uint32_t&) override{throw std::logic_error("Direction-switch allocation is excluded from this CPU domain");}
    std::uint32_t spawn_animation(th20::source::sprite::AnimationFile&,int,const th20::source::sprite::Vec3&,int) override{throw std::logic_error("Direction-switch allocation is excluded from this CPU domain");}
    float animation_height(th20::source::sprite::Animation& animation) override{
        if(animation.fields_550[2])throw std::logic_error("Test ANM parent must be null");
        return th20::recovered::mul32(th20::recovered::mul32(animation.base.vector_50.y,1),animation.base.vector_70.y);
    }
    float animation_width(th20::source::sprite::Animation& animation) override{
        if(animation.fields_550[2])throw std::logic_error("Test ANM parent must be null");
        return th20::recovered::mul32(th20::recovered::mul32(animation.base.vector_50.x,1),animation.base.vector_70.x);
    }
};
namespace {
struct EntityLifecycleFixture final:gp::EnemyLifecycleServices {
    unsigned deletions=0;
    void delete_animation(std::uint32_t& handle) override{if(handle)throw std::logic_error("Entity lifecycle CPU domain permits only absent ANM handles");++deletions;}
    void destroy_mesh(th20::source::sprite::RenderMesh*) override{throw std::logic_error("Entity lifecycle CPU domain excludes allocated mesh");}
} entity_lifecycle;
gp::EnemyFrameServices* spawn_frame=nullptr;
th20::source::sprite::Animation* read_vm_animation(std::uint32_t& handle){auto* value=spawn_frame->animation(handle);if(!value)handle=0;return value;}
}
namespace th20::source::gameplay {
EnemyLifecycleServices& enemy_lifecycle_services(){return ::entity_lifecycle;}
EnemyFrameServices& enemy_frame_services(){if(!::spawn_frame)throw std::logic_error("Missing recorded Enemy frame boundary");return *::spawn_frame;}
namespace unrecovered {
int execute_enemy_opcode_0048c010(EnemyState&){throw std::logic_error("Entity opcode execution outside constructor/init oracle domain");}
std::int32_t read_enemy_integer_0049abc0(Enemy&,std::int32_t){throw std::logic_error("Entity variable read outside constructor/init oracle domain");}
float read_enemy_float_004995d0(Enemy&,std::int32_t){throw std::logic_error("Entity variable read outside constructor/init oracle domain");}
}
}
namespace {
int __fastcall damage_callback(gp::EnemyState* state,void*,int amount){state->field_04+=static_cast<unsigned>(amount);return 3;}
void __fastcall collision_callback(gp::EnemyState* state,void*){state->field_04+=7;}
template<class R,class... A>R original(std::uint32_t address,void* self,A... args){return reinterpret_cast<R(__thiscall*)(void*,A...)>(mapped_image_base+address-0x400000)(self,args...);}
template<class T>void put(std::vector<std::uint8_t>& bytes,std::size_t offset,T value){std::memcpy(bytes.data()+offset,&value,sizeof(value));}
std::vector<std::uint8_t> document(const std::vector<std::string>& names) {
    std::vector<std::uint8_t> bytes(0x24+names.size()*4,0);put(bytes,0,0x54504353u);put(bytes,4,std::uint16_t{1});put(bytes,0x10,static_cast<std::uint16_t>(names.size()));
    for(const auto& name:names){bytes.insert(bytes.end(),name.begin(),name.end());bytes.push_back(0);}
    while(bytes.size()%4)bytes.push_back(0);
    for(std::size_t i=0;i<names.size();++i){put(bytes,0x24+i*4,static_cast<std::uint32_t>(bytes.size()));const auto offset=bytes.size();bytes.resize(offset+16);put(bytes,offset,0x484c4345u);put(bytes,offset+4,16u);}
    return bytes;
}
}
#include "enemy_vm_test_fixture.hpp"
int wmain(int argc,wchar_t** argv) {
    AddVectoredExceptionHandler(1,[](EXCEPTION_POINTERS* p)->LONG{if(p->ExceptionRecord->ExceptionCode!=0xe06d7363u)std::cerr<<"Native exception "<<std::hex<<p->ExceptionRecord->ExceptionCode<<" at "<<p->ContextRecord->Eip<<" access "<<p->ExceptionRecord->ExceptionInformation[1]<<std::dec<<'\n';return EXCEPTION_CONTINUE_SEARCH;});
    try {
        if(argc!=3)throw std::runtime_error("Usage: enemy_cpu_compare VERIFIED_TH20.exe REPORT.json");
        const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original hash mismatch");
        Mapping mapped(bytes,th20::parse_pe(bytes));mapped_image_base=mapped.address();
        unsigned failed=0;std::map<std::string,unsigned> counts;std::mt19937 rng(0x4a2fc020);
        const auto check=[&](const char* label,bool match){++counts[label];if(!match){if(failed<20)std::cerr<<"FAIL "<<label<<'\n';++failed;}};
        for(int n=0;n<5000;++n) {
            gp::EnemyData a,b;for(auto& byte:reinterpret_cast<std::uint8_t(&)[sizeof(a)]>(a))byte=static_cast<std::uint8_t>(rng());b=a;
            original<void>(0x4a2fc0,&a);gp::construct_enemy_data(b);check("enemy_data_ctor_all164bytes",std::memcmp(&a,&b,sizeof(a))==0);
            for(auto& byte:reinterpret_cast<std::uint8_t(&)[sizeof(a)]>(a))byte=static_cast<std::uint8_t>(rng());b=a;
            original<void>(0x4ab1b0,&a);gp::reset_enemy_counters(b);check("enemy_reset_first48_preserves_other_bytes",std::memcmp(&a,&b,sizeof(a))==0);
            std::array<std::uint32_t,0x134/4> owner{};const auto player=static_cast<int>(rng());owner[0x12c/4]=static_cast<std::uint32_t>(player);
            const std::uint32_t prior=n<8?std::array<std::uint32_t,8>{0,1,0xffff,0xfffe,0xffffffff,0x10000,0x1ffff,0xffff0000}[n]:rng();
            gp::current_enemy_generation=prior;*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c49f0)=prior;
            const auto result=original<std::uint32_t>(0x4ab140,owner.data()),actual=gp::advance_enemy_generation(player);
            check("enemy_generation_wrap_and_player_bits",result==actual&&gp::current_enemy_generation==*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c49f0)&&gp::previous_enemy_generation==*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c49ec));
        }
        EnemyFixture environment;
        *reinterpret_cast<void**>(mapped_image_base+0x1e4d28)=std::pmr::get_default_resource();
        for(int n=0;n<3000;++n) {
            alignas(gp::EnemyState)std::array<std::uint8_t,sizeof(gp::EnemyState)> cpu_storage,source_storage;
            for(auto& byte:cpu_storage)byte=static_cast<std::uint8_t>(rng());source_storage=cpu_storage;
            original<void>(0x4a3060,cpu_storage.data());auto* state=new(source_storage.data())gp::EnemyState;
            check("EnemyState_ctor_all752bytes_actual_PMR_resource",cpu_storage==source_storage);
            gp::EnemyAuxiliary28 cpu_aux,source_aux;for(auto& value:cpu_aux.words)value=rng();source_aux=cpu_aux;
            original<void>(0x4a7310,&cpu_aux);gp::reset_enemy_auxiliary(source_aux);
            check("EnemyState_auxiliary_reset_preserves_fields",std::memcmp(&cpu_aux,&source_aux,sizeof(cpu_aux))==0);
            gp::EnemyPatternState cpu_pattern,source_pattern;for(auto& byte:reinterpret_cast<std::uint8_t(&)[sizeof(cpu_pattern)]>(cpu_pattern))byte=static_cast<std::uint8_t>(rng());source_pattern=cpu_pattern;
            original<void>(0x4a7360,&cpu_pattern);gp::reset_enemy_pattern(source_pattern);
            check("EnemyState_pattern_reset_all168bytes",std::memcmp(&cpu_pattern,&source_pattern,sizeof(cpu_pattern))==0);
            original<void>(0x4a7170,cpu_storage.data());state->initialize();
            // Object pointers naturally differ; compare every vector size,
            // capacity and allocated byte, then normalize addresses only.
            auto normalized=cpu_storage;
            for(auto [offset,stride]:std::array<std::pair<unsigned,unsigned>,3>{{{0xc,20},{0x158,0x184},{0x2b8,0x88}}}) {
                auto* a=reinterpret_cast<std::uintptr_t*>(cpu_storage.data()+offset);
                auto* b=reinterpret_cast<std::uintptr_t*>(source_storage.data()+offset);
                const auto used_a=a[2]-a[1],used_b=b[2]-b[1],capacity_a=a[3]-a[1],capacity_b=b[3]-b[1];
                check("EnemyState_initialized_vector_sizes_and_bytes",used_a==used_b&&capacity_a==capacity_b&&used_a%stride==0&&(!used_a||std::memcmp(reinterpret_cast<void*>(a[1]),reinterpret_cast<void*>(b[1]),used_a)==0));
                std::memcpy(normalized.data()+offset+4,b+1,12);
            }
            check("EnemyState_initialize_all752bytes_pointer_normalized",normalized==source_storage);
            // Original repeated init clears movement state but retains existing
            // animation handle/offset/parent. Test that distinction directly.
            auto* original_animation=reinterpret_cast<gp::EnemyAnimationLink*>(*reinterpret_cast<std::uintptr_t*>(cpu_storage.data()+0x10));
            original_animation->handle=state->animations[0].handle=0x12345678u;
            original_animation->parent=state->animations[0].parent=42;
            original<void>(0x4a7170,cpu_storage.data());state->initialize();
            check("EnemyState_reinitialize_preserves_first_animation",std::memcmp(original_animation,&state->animations[0],20)==0);
            original<void>(0x4a3ac0,cpu_storage.data());state->~EnemyState();
        }
        for(unsigned n=1;n<=32;++n) {
            gp::EnemyState source;unsigned released=0;
            source.animations.resize(n);source.movements.resize(n);source.auxiliary.resize(n);
            for(unsigned i=0;i<n;++i) {
                gp::EnemyQueuedRecord item{std::shared_ptr<void>(new int(i),[&](void* p){++released;delete static_cast<int*>(p);}),{}};
                source.queued.push_front(std::move(item));
            }
            // All allocations and control blocks belong to the source PMR/CRT.
            // The unpatched original destructor walks and releases these exact
            // layouts. Its writes leave the containers valid and empty.
            original<void>(0x4a3ac0,&source);
            check("EnemyState_original_destructor_real_shared_owners",released==n&&source.queued.empty()&&source.animations.empty()&&source.movements.empty()&&source.auxiliary.empty());
        }
        FrameFixture frame;
        struct LookupController {std::uint8_t prefix[0x108]{};scheduler::List entities{};};
        static_assert(offsetof(LookupController,entities)==0x108);
        {
            // Retain the retired storage so this diagnostic can observe the
            // original invalid post-retirement write without invoking host UB.
            // The real controller frees Enemy before advancing this iterator.
            std::ofstream probe(std::filesystem::path(argv[2]).parent_path()/"enemy_iterator_hazard.json");
            probe<<"{\"original_sha256\":"<<th20::json_string(expected_sha)<<",\"retired_storage_kept_alive\":true,\"scenarios\":[";
            bool first_probe=true;
            for(unsigned outer_index=0;outer_index<4;++outer_index)for(unsigned id:{0u,1u,2u,4u,99u}){
                auto run=[&](bool native){
                    LookupController owner;scheduler::initialize_list(owner.entities);
                    std::array<std::array<std::uint8_t,0x428>,4> objects{};
                    for(unsigned i=0;i<objects.size();++i){
                        *reinterpret_cast<unsigned*>(objects[i].data()+0x88)=i+1;
                        auto& link=*reinterpret_cast<scheduler::Link*>(objects[i].data()+0x74);
                        scheduler::initialize_link(link,reinterpret_cast<scheduler::Node*>(objects[i].data()));scheduler::append(owner.entities,link);
                    }
                    alignas(scheduler::Iterator) std::array<unsigned char,sizeof(scheduler::Iterator)> iterator_bytes{};
                    auto* iterator=reinterpret_cast<scheduler::Iterator*>(iterator_bytes.data());
                    if(native)original<void>(0x412280,&owner.entities,iterator);
                    else ::new(iterator)scheduler::Iterator(owner.entities.sentinel.next);
                    for(unsigned i=0;i<outer_index;++i){if(native)original<void>(0x411c30,iterator);else iterator->advance();}
                    auto* retired=iterator->current;
                    if(native)(void)original<void*>(0x498a80,&owner,id);else(void)gp::find_enemy_in_list(owner.entities,id);
                    const bool observer_lost=retired->iterator!=iterator;
                    if(native)original<void>(0x411ce0,retired);else scheduler::unlink(*retired);
                    const bool current_still_retired=iterator->current==retired;
                    retired->iterator=reinterpret_cast<scheduler::Iterator*>(0xa5a5a5a5u);
                    if(native)original<void>(0x411c30,iterator);else iterator->advance();
                    const bool wrote_retired_storage=retired->iterator==nullptr;
                    if(native)original<void>(0x411b00,iterator);else iterator->~Iterator();
                    return std::array<bool,3>{observer_lost,current_still_retired,wrote_retired_storage};
                };
                const auto native=run(true),source=run(false);
                check("Enemy_nested_lookup_retirement_observer_behavior_matches_native",native==source);
                if(!first_probe)probe<<',';first_probe=false;
                probe<<"{\"outer_index\":"<<outer_index<<",\"lookup_identifier\":"<<id
                     <<",\"native_observer_lost\":"<<(native[0]?"true":"false")<<",\"native_current_still_retired\":"<<(native[1]?"true":"false")
                     <<",\"native_advance_wrote_retired_storage\":"<<(native[2]?"true":"false")<<",\"source_advance_wrote_retired_storage\":"<<(source[2]?"true":"false")<<'}';
            }
            probe<<"],\"scope\":\"Unchanged original412280 outer iterator,498a80 nested lookup,411ce0 unlink,411c30 advance and411b00 destruction. Retired Enemy storage remains allocated; the poison observer detects whether advance writes Enemy+84 after retirement. This establishes a composition hazard, not proof that the live demo took this path.\"}\n";
        }
        for(unsigned n=0;n<128;++n) {
            LookupController owner; scheduler::initialize_list(owner.entities);
            std::array<std::array<std::uint8_t,0x428>,4> objects{};std::array<scheduler::Link,4> links{};
            gp::EnemyMovementRecord movements[2];
            th20::source::game_session::Context context{};context.objects_04[1]=&owner;
            for(unsigned i=0;i<objects.size();++i) {
                *reinterpret_cast<std::uint32_t*>(objects[i].data()+0x88)=i%2+1;
                *reinterpret_cast<void**>(objects[i].data()+0x424)=&context;
                *reinterpret_cast<void**>(objects[i].data()+0x1e4)=movements;
                scheduler::initialize_link(links[i],reinterpret_cast<scheduler::Node*>(objects[i].data()));scheduler::append(owner.entities,links[i]);
            }
            const auto selected_id=n%4;*reinterpret_cast<unsigned*>(owner.prefix+0x54)=selected_id;
            th20::source::game_session::context(0).objects_04[1]=n%5?&owner:nullptr;
            *reinterpret_cast<void**>(mapped_image_base+0x1ba570)=n%5?&owner:nullptr;
            for(std::uint32_t code=0xffffd8e0u;code<=0xffffd970u;++code) {
                auto* a=original<std::uint32_t*>(0x498600,objects[3].data(),static_cast<int>(code));
                auto* b=gp::enemy_integer_destination(objects[3].data(),static_cast<int>(code));
                if(reinterpret_cast<std::uintptr_t>(a)>=mapped_image_base+0x1c49d8&&reinterpret_cast<std::uintptr_t>(a)<mapped_image_base+0x1c49e8)
                    a=&gp::enemy_script_globals[(reinterpret_cast<std::uintptr_t>(a)-mapped_image_base-0x1c49d8)/4];
                check("Enemy_integer_destinations_all_cases_and_null_default",a==b);
                check("Enemy_float_destinations_all_cases_and_null_default",original<std::uint32_t*>(0x498210,objects[3].data(),static_cast<int>(code))==gp::enemy_float_destination(objects[3].data(),static_cast<int>(code)));
            }
            for(unsigned id=0;id<4;++id) {
                check("Enemy_identifier_lookup_first_duplicate_or_null",original<void*>(0x498a80,&owner,id)==gp::find_enemy_in_list(owner.entities,id));
                for(auto& link:links)check("Enemy_identifier_lookup_clears_observer_links",link.iterator==nullptr);
            }
        }
        th20::source::game_session::context(0).objects_04[1]=nullptr;
        const std::array<std::uint32_t,12> targeting_special{0,0x80000000,0x3f800000,0x40000000,0xbf800000,0x7fc12345,0x7f800000,0xff800000,1,0x80000001,0x7fa12345,0xffc12345};
        for(unsigned n=0;n<6000;++n) {
            LookupController owner;scheduler::initialize_list(owner.entities);
            std::array<std::array<std::uint8_t,0x428>,6> objects{};std::array<scheduler::Link,6> links{};
            for(unsigned i=0;i<objects.size();++i) {
                for(auto& byte:objects[i])byte=static_cast<std::uint8_t>(rng());
                auto* object=objects[i].data();
                *reinterpret_cast<std::uint32_t*>(object+0x88)=i+1;
                *reinterpret_cast<std::uint32_t*>(object+0x350)&=~0x21u;
                *reinterpret_cast<std::uint32_t*>(object+0x354)&=~0xc00u;
                if(n&1u)*reinterpret_cast<std::uint32_t*>(object+0x350)|=((n>>(i%3))&1u)|(((n>>(i%3+1))&1u)<<5);
                if(n&2u)*reinterpret_cast<std::uint32_t*>(object+0x354)|=((n>>(i%3))&3u)<<10;
                auto* position=reinterpret_cast<th20::source::sprite::Vec3*>(object+0x198);
                position->x=static_cast<float>(static_cast<int>(rng()%2001)-1000)/4.0f;
                position->y=static_cast<float>(static_cast<int>(rng()%2001)-1000)/4.0f;
                // Equal first distances retain the first list entry. Vary z
                // independently to prove that targeting uses only two axes.
                if(i<2&&n%4==0) {position->x=i?3.0f:-3.0f;position->y=4.0f;}
                if(n%16==3)*reinterpret_cast<std::uint32_t*>(&position->x)=targeting_special[(n+i)%targeting_special.size()];
                scheduler::initialize_link(links[i],reinterpret_cast<scheduler::Node*>(object));
                if(i<n%7)scheduler::append(owner.entities,links[i]);
                const auto p=gp::enemy_position(object);
                check("Enemy_position_all12bytes",std::memcmp(original<void*>(0x47a2c0,object),&p,12)==0);
                unsigned id=0;check("Enemy_identifier_getter_output_and_return",original<unsigned*>(0x498f90,object,&id)==&id&&id==gp::enemy_identifier(object));
                check("Enemy_excluded_flags_all_combinations",original<int>(0x47a3e0,object)==static_cast<int>(gp::enemy_excluded(object)));
                check("EnemyState_excluded_flags_all_combinations",original<int>(0x47a370,object+0x88)==static_cast<int>(gp::enemy_state_excluded(*reinterpret_cast<gp::EnemyState*>(object+0x88))));
                auto source=objects[i];const auto mark=rng();original<void>(0x478260,object,mark);gp::set_enemy_bomb_mark(source.data(),mark);
                check("Enemy_bomb_mark_bit5_preserves_all_bytes",objects[i]==source);
            }
            th20::source::sprite::Vec2 origin{};float radius=static_cast<float>(n%251);
            if(n%4==0)radius=n%8==0?5.0f:6.0f;
            if(n%16==1)*reinterpret_cast<std::uint32_t*>(&radius)=targeting_special[(n/16)%targeting_special.size()];
            if(n%16==2)*reinterpret_cast<std::uint32_t*>(&origin.y)=targeting_special[(n/16)%targeting_special.size()];
            const auto before_objects=objects;const auto before_links=links;const auto before_list=owner.entities;
            unsigned result=0xaabbccddu;
            check("Enemy_nearest_output_and_return_pointer",original<unsigned*>(0x4aac00,&owner,&result,&origin,radius)==&result);
            const auto actual=gp::nearest_enemy_identifier(owner.entities,origin,radius);
            check("Enemy_nearest_strict_radius_ties_flags_IEEE_empty",result==actual);
            check("Enemy_nearest_preserves_entities_and_clears_observers",objects==before_objects&&std::memcmp(links.data(),before_links.data(),sizeof(links))==0&&std::memcmp(&owner.entities,&before_list,sizeof(before_list))==0);
        }
        // A bounded real Controller prefix contains actual8 pool slots. The
        // original lookup and full4a8260 execute unchanged against those slots.
        std::vector<std::uint8_t> animation_pool(0x710+8*sizeof(th20::source::sprite::PooledAnimation));
        frame.pool=reinterpret_cast<th20::source::sprite::PooledAnimation*>(animation_pool.data()+0x710);
        *reinterpret_cast<void**>(mapped_image_base+0x1c0028)=animation_pool.data();
        *reinterpret_cast<float**>(mapped_image_base+0x1aefe0)=&frame.scale;
        UpdateFixture update(frame);std::mt19937 core_rng(0x4a8260);
        for(unsigned n=0;n<2400;++n) {
            gp::EnemyState state;state.animations.resize(6);
            for(auto& field:state.fields_1c)field=core_rng();
            for(auto& field:state.fields_2c8)field=core_rng();
            state.fields_2c8[1]=(core_rng()&~4u)|0x4000000u;
            if(n%10==0)state.fields_2c8[1]|=4u;
            state.fields_2c8[3]=0;                           //actual mesh-null original branch
            for(auto* timer:{&state.pattern_1a8.timer_90,&state.timer_288,&state.timer_298,&state.timer_b8,&state.timer_a8}) {
                th20::recovered::timer_set(*timer,static_cast<int>(core_rng()%201)-100);timer->flags=core_rng();
            }
            for(auto& field:state.motion_110.words){const float value=static_cast<float>(static_cast<int>(core_rng()%4001)-2000)/16.0f;std::memcpy(&field,&value,4);}
            if(n%13==0)state.motion_110.words[0x38/4]=targeting_special[(n/13)%targeting_special.size()];
            frame.scale=static_cast<float>(static_cast<int>(n%9)-2)*.25f;
            for(unsigned i=0;i<8;++i) {
                auto& slot=frame.pool[i];for(auto& byte:reinterpret_cast<std::uint8_t(&)[sizeof(slot)]>(slot))byte=static_cast<std::uint8_t>(core_rng());
                slot.active=i!=0;slot.animation.handle=0x10000u|i;slot.animation.index=i;
                slot.animation.base.flags[2]=(slot.animation.base.flags[2]&~0xe00000u)|((n%8)<<21);
                slot.animation.base.vector_50.x=n%7? -.25f:std::numeric_limits<float>::quiet_NaN();
                slot.animation.base.vector_2c={static_cast<float>(i),static_cast<float>(i*2),static_cast<float>(i*3)};
            }
            for(unsigned i=0;i<state.animations.size();++i) {
                auto& link=state.animations[i];link.handle=i==0?0x20000u:i==5?0x20007u:0x10000u|i;
                link.offset[0]=static_cast<float>(i);link.offset[1]=static_cast<float>(n%17);link.offset[2]=static_cast<float>(n%23);
                link.parent=i==2?static_cast<int>(n%6):-1;
            }
            std::array<std::uint8_t,sizeof(state)> state_before;std::memcpy(state_before.data(),&state,sizeof(state));
            const auto links_before=state.animations;const auto pool_before=animation_pool;
            const auto result=original<int>(0x4a8260,&state);
            std::array<std::uint8_t,sizeof(state)> state_expected;std::memcpy(state_expected.data(),&state,sizeof(state));
            const auto links_expected=state.animations;const auto pool_expected=animation_pool;
            std::memcpy(&state,state_before.data(),sizeof(state));std::copy(links_before.begin(),links_before.end(),state.animations.begin());
            std::copy(pool_before.begin(),pool_before.end(),animation_pool.begin());
            check("Enemy_core_full_update_return_no_damage_no_mesh_already_moved",result==gp::update_enemy_state(state,update));
            if(failed<20) {
                for(unsigned k=0;k<sizeof(state);++k)if(state_expected[k]!=reinterpret_cast<std::uint8_t*>(&state)[k]){std::cerr<<"core state n="<<n<<" mode="<<n%8<<" offset="<<std::hex<<k<<" expected="<<unsigned(state_expected[k])<<" actual="<<unsigned(reinterpret_cast<std::uint8_t*>(&state)[k])<<std::dec<<'\n';break;}
                for(unsigned k=0;k<animation_pool.size();++k)if(pool_expected[k]!=animation_pool[k]){std::cerr<<"core pool n="<<n<<" mode="<<n%8<<" offset="<<std::hex<<k<<" expected="<<unsigned(pool_expected[k])<<" actual="<<unsigned(animation_pool[k])<<std::dec<<'\n';break;}
            }
            check("Enemy_core_all752_state_bytes",std::memcmp(state_expected.data(),&state,sizeof(state))==0);
            check("Enemy_core_animation_handles_parent_and_stale_resolution",std::memcmp(links_expected.data(),state.animations.data(),links_expected.size()*sizeof(gp::EnemyAnimationLink))==0);
            check("Enemy_core_all_pool_bytes_orientation_position_flags",pool_expected==animation_pool);
            //4a8760 also uses44ced0, so stale nonzero handles must be cleared.
            std::array<std::uint8_t,0x428> entity{};std::array<gp::EnemyAnimationLink,3> slowing{};
            slowing[0].handle=0x10001u;slowing[1].handle=0x20007u;slowing[2].handle=0;
            *reinterpret_cast<void**>(entity.data()+0x98)=slowing.data();*reinterpret_cast<void**>(entity.data()+0x9c)=slowing.data()+slowing.size();
            *reinterpret_cast<std::uint32_t*>(entity.data()+0x354)=4u|(n%2?0x10000u:0u);
            *reinterpret_cast<float*>(entity.data()+0xc4)=n%3==0?.25f:n%3==1?0.0f:-1.0f;
            const auto slowing_before=slowing;const auto entity_before=entity;const auto slow_pool_before=animation_pool;
            *reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=frame.scale;
            const auto slow_result=original<int>(0x4a8760,entity.data());const auto slowing_expected=slowing;const auto entity_expected=entity;const auto slow_pool_expected=animation_pool;
            slowing=slowing_before;entity=entity_before;std::copy(slow_pool_before.begin(),slow_pool_before.end(),animation_pool.begin());
            check("Enemy_slowdown_real_pool_stale_handles_all_bytes",slow_result==gp::update_enemy_with_time_scale(entity.data(),frame)&&std::memcmp(slowing.data(),slowing_expected.data(),sizeof(slowing))==0&&entity==entity_expected&&animation_pool==slow_pool_expected);
        }
        frame.pool=nullptr;
        for(unsigned mode=0;mode<34;++mode)for(unsigned separate=0;separate<2;++separate)for(unsigned n=0;n<128;++n) {
            gp::EnemyMotionInterpolation cpu{},source;
            for(auto* vector:{&cpu.current,&cpu.start,&cpu.end,&cpu.tangent_start,&cpu.tangent_end})
                *vector={static_cast<float>(static_cast<int>(core_rng()%2001)-1000)/16.0f,static_cast<float>(static_cast<int>(core_rng()%2001)-1000)/16.0f,static_cast<float>(static_cast<int>(core_rng()%2001)-1000)/16.0f};
            cpu.mode=static_cast<int>(mode);cpu.axis_modes[0]=mode;cpu.axis_modes[1]=(mode+7)%34;cpu.axis_modes[2]=(mode+17)%34;
            cpu.flags=(core_rng()&~1u)|separate;cpu.duration=n%4==0?-1:n%4==1?0:n%4==2?1:60;
            th20::recovered::timer_set(cpu.timer,static_cast<int>(n%81)-10);cpu.timer.flags=core_rng();
            frame.scale=static_cast<float>(n%9)*.25f;source=cpu;
            th20::source::sprite::Vec3 expected{};auto* returned=original<th20::source::sprite::Vec3*>(0x4a8d70,&cpu,&expected);
            const auto actual=gp::sample_enemy_motion_interpolation(source,&frame.scale);
            check("Enemy_extended_vec3_interpolation_output_all_modes",returned==&expected&&std::memcmp(&expected,&actual,12)==0);
            check("Enemy_extended_vec3_interpolation_all100bytes",std::memcmp(&cpu,&source,sizeof(cpu))==0);
        }
        for(unsigned n=0;n<4000;++n) {
            gp::EnemyState enemy;enemy.movements.resize(1+n%6);
            for(auto* motion:{&enemy.motion_c8,&enemy.motion_110})for(auto& word:motion->words) {
                const float value=static_cast<float>(static_cast<int>(core_rng()%4001)-2000)/16.0f;std::memcpy(&word,&value,4);
            }
            enemy.motion_110.words[0x44/4]=(core_rng()&~15u)|(n%5);
            enemy.fields_2c8[1]=core_rng();
            for(auto& word:enemy.fields_178) {const float value=static_cast<float>(static_cast<int>(core_rng()%2001)-1000)/4.0f;std::memcpy(&word,&value,4);}
            for(auto& movement:enemy.movements)for(auto& word:movement.motion.words) {
                const float value=static_cast<float>(static_cast<int>(core_rng()%4001)-2000)/16.0f;std::memcpy(&word,&value,4);
            }
            if(n%13==0)enemy.fields_178[(n/13)%4]=targeting_special[(n/13)%targeting_special.size()];
            *reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=frame.scale=static_cast<float>(n%9)*.25f;
            std::array<std::uint8_t,sizeof(enemy)> before{};std::memcpy(before.data(),&enemy,sizeof(enemy));const auto movements_before=enemy.movements;
            original<void>(0x4a7da0,&enemy);
            std::array<std::uint8_t,sizeof(enemy)> expected{};std::memcpy(expected.data(),&enemy,sizeof(enemy));const auto movements_expected=enemy.movements;
            std::memcpy(&enemy,before.data(),sizeof(enemy));std::copy(movements_before.begin(),movements_before.end(),enemy.movements.begin());
            gp::combine_enemy_movements(enemy,frame.scale);
            if(failed<20)for(unsigned k=0;k<sizeof(enemy);++k)if(expected[k]!=reinterpret_cast<std::uint8_t*>(&enemy)[k]){std::cerr<<"combine n="<<n<<" offset="<<std::hex<<k<<" expected="<<unsigned(expected[k])<<" actual="<<unsigned(reinterpret_cast<std::uint8_t*>(&enemy)[k])<<" flags="<<enemy.fields_2c8[1]<<std::dec<<'\n';break;}
            if(std::memcmp(&enemy,expected.data(),sizeof(enemy))!=0) {
                th20::source::state::Motion cpu_motion;std::memcpy(&cpu_motion,before.data()+0x110,sizeof(cpu_motion));std::memcpy(&cpu_motion.vector_38,enemy.motion_110.words+14,12);
                const auto input=cpu_motion;auto source_motion=cpu_motion;
                original<void>(0x453ac0,&cpu_motion);th20::source::state::update_motion_position(source_motion,frame.scale);
                std::ofstream example("source_reconstruction/gameplay/enemy_motion_counterexample.json");example<<"{\"clock\":"<<frame.scale<<",\"input_words\":[";
                for(unsigned i=0;i<18;++i){if(i)example<<',';example<<reinterpret_cast<const std::uint32_t*>(&input)[i];}
                example<<"],\"cpu_words\":[";for(unsigned i=0;i<18;++i){if(i)example<<',';example<<reinterpret_cast<const std::uint32_t*>(&cpu_motion)[i];}
                example<<"],\"source_words\":[";for(unsigned i=0;i<18;++i){if(i)example<<',';example<<reinterpret_cast<const std::uint32_t*>(&source_motion)[i];}example<<"]}\n";
                std::cerr<<"direct453ac0 match="<<(std::memcmp(&cpu_motion,&source_motion,sizeof(cpu_motion))==0)<<'\n';
            }
            check("Enemy_aggregate_movement_all_state_bytes",std::memcmp(&enemy,expected.data(),sizeof(enemy))==0);
            check("Enemy_aggregate_movement_record_bytes_and_clamp",std::memcmp(enemy.movements.data(),movements_expected.data(),enemy.movements.size()*sizeof(gp::EnemyMovementRecord))==0);
        }
        frame.pool=reinterpret_cast<th20::source::sprite::PooledAnimation*>(animation_pool.data()+0x710);
        MovementFixture movement_host(frame);
        for(unsigned n=0;n<4000;++n) {
            gp::EnemyState enemy;enemy.movements.resize(1+n%4);enemy.animations.resize(1);
            std::array<std::uint8_t,0x428> entity{},parent_entity{};scheduler::List parent_list{};
            parent_list.sentinel.value=reinterpret_cast<scheduler::Node*>(parent_entity.data());
            *reinterpret_cast<void**>(entity.data()+0x3f0)=&parent_list;enemy.entity=entity.data();
            *reinterpret_cast<th20::source::sprite::Vec3*>(parent_entity.data()+0x198)={static_cast<float>(n%257),static_cast<float>(n%401),static_cast<float>(n%23)};
            enemy.fields_2c8[0]=core_rng();enemy.fields_2c8[1]=core_rng()&~0x10u;enemy.fields_2c8[2]=core_rng();
            for(auto& word:enemy.motion_110.words){const float v=static_cast<float>(static_cast<int>(core_rng()%2001)-1000)/8.0f;std::memcpy(&word,&v,4);}enemy.motion_110.words[17]=0;
            for(auto& word:enemy.fields_178){const float v=static_cast<float>(static_cast<int>(core_rng()%2001)-1000)/4.0f;std::memcpy(&word,&v,4);}
            for(auto& record:enemy.movements) {
                for(auto& word:record.motion.words){const float v=static_cast<float>(static_cast<int>(core_rng()%2001)-1000)/8.0f;std::memcpy(&word,&v,4);}record.motion.words[17]=core_rng()%5;
                for(auto* scalar:{&record.scalar_ac,&record.scalar_d8}) {
                    scalar->start=-3.0f;scalar->end=9.0f;scalar->tangent_start=.5f;scalar->tangent_end=1.25f;scalar->duration=n%3?50:0;scalar->mode=n%34;th20::recovered::timer_set(scalar->timer,n%55);
                }
                auto& pair=record.vector_104;pair.start={-2,7};pair.end={6,1};pair.tangent_start={3,1};pair.tangent_end={4,.5f};pair.duration=n%5?45:0;pair.mode=n%34;th20::recovered::timer_set(pair.timer,n%55);
                auto& interpolation=record.position;interpolation.start={1,7,3};interpolation.end={27,19,2};interpolation.tangent_start={2,3,4};interpolation.tangent_end={9,3,2};interpolation.duration=n%3==1?40:0;interpolation.mode=n%34;interpolation.axis_modes[0]=(n+7)%34;interpolation.axis_modes[1]=(n+17)%34;interpolation.axis_modes[2]=n%34;interpolation.flags=n%2;th20::recovered::timer_set(interpolation.timer,n%45);
            }
            std::memset(frame.pool,0,8*sizeof(*frame.pool));auto& animation=frame.pool[1];animation.active=1;animation.animation.handle=0x10001;animation.animation.base.vector_50={1.5f,-2};animation.animation.base.vector_70={16,32};
            enemy.animations[0].handle=n%3==0?0:n%3==1?0x10001u:0x20001u;enemy.vector_170={20,32};
            frame.scale=static_cast<float>(n%9)*.25f;*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=frame.scale;
            movement_host.offset={static_cast<float>(n%7),static_cast<float>(n%11),static_cast<float>(n%3)};std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1c50fc),&movement_host.offset,12);
            std::array<std::uint8_t,sizeof(enemy)> before{};std::memcpy(before.data(),&enemy,sizeof(enemy));const auto records_before=enemy.movements;const auto animations_before=enemy.animations;
            const int result=original<int>(0x4a7710,&enemy);
            std::array<std::uint8_t,sizeof(enemy)> expected{};std::memcpy(expected.data(),&enemy,sizeof(enemy));const auto records_expected=enemy.movements;const auto animations_expected=enemy.animations;
            std::memcpy(&enemy,before.data(),sizeof(enemy));std::copy(records_before.begin(),records_before.end(),enemy.movements.begin());std::copy(animations_before.begin(),animations_before.end(),enemy.animations.begin());
            check("Enemy_movement_return_bounds_departure",result==gp::update_enemy_movement(enemy,movement_host));
            if(failed<20)for(unsigned k=0;k<sizeof(enemy);++k)if(expected[k]!=reinterpret_cast<std::uint8_t*>(&enemy)[k]){std::cerr<<"movement n="<<n<<" offset="<<std::hex<<k<<" expected="<<unsigned(expected[k])<<" actual="<<unsigned(reinterpret_cast<std::uint8_t*>(&enemy)[k])<<std::dec<<'\n';break;}
            check("Enemy_movement_all752_state_bytes",std::memcmp(&enemy,expected.data(),sizeof(enemy))==0);
            check("Enemy_movement_all_interpolation_and_motion_records",std::memcmp(enemy.movements.data(),records_expected.data(),enemy.movements.size()*sizeof(gp::EnemyMovementRecord))==0);
            check("Enemy_movement_animation_handle_resolution",std::memcmp(enemy.animations.data(),animations_expected.data(),enemy.animations.size()*sizeof(gp::EnemyAnimationLink))==0);
        }
        frame.pool=nullptr;
        for(int n=0;n<5000;++n) {
            std::array<std::uint8_t,0x2240> actual,source;
            for(auto& byte:actual)byte=static_cast<std::uint8_t>(rng());source=actual;
            const int seconds=n<8?std::array<int,8>{-100,-1,0,1,99,100,101,0x7fffffff}[n]:static_cast<int>(rng());
            const int hundredths=static_cast<int>(rng());
            original<void>(0x4ab5b0,actual.data(),seconds,hundredths);gp::store_boss_time(source.data(),seconds,hundredths);
            check("HUD_time_saturates_using_seconds_preserves_all_bytes",actual==source);
            const auto bits=rng();original<void>(0x4ab850,actual.data(),bits);gp::set_primary_entity_flag(source.data(),bits);
            check("primary_entity_flag_bit5_all_bytes",actual==source);
            const float scale=static_cast<float>(static_cast<int>(rng()%10000)-5000)/100.0f;
            original<void>(0x4ab590,actual.data(),scale);gp::set_primary_entity_scale(source.data(),scale);
            check("primary_entity_scale_set_all_bytes",actual==source);
            const auto read=original<float>(0x4aaa40,actual.data());check("primary_entity_scale_get",std::memcmp(&read,&scale,4)==0&&gp::primary_entity_scale(source.data())==scale);
            std::array<std::uint8_t,0x358> native_entity{},cpp_entity{};
            for(auto& byte:native_entity)byte=static_cast<std::uint8_t>(rng());
            *reinterpret_cast<void**>(native_entity.data()+0x98)=nullptr;*reinterpret_cast<void**>(native_entity.data()+0x9c)=nullptr;
            *reinterpret_cast<std::uint32_t*>(native_entity.data()+0x354)|=4;
            const std::array<std::uint32_t,12> special{0,0x80000000,0x3f800000,0x40000000,0xbf800000,0x7fc12345,0x7f800000,0xff800000,1,0x80000001,0x7fa12345,0xffc12345};
            *reinterpret_cast<std::uint32_t*>(native_entity.data()+0xc4)=n<12?special[n]:rng();cpp_entity=native_entity;
            frame.scale=scale;*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=scale;
            const auto result=original<int>(0x4a8760,native_entity.data());const auto source_result=gp::update_enemy_with_time_scale(cpp_entity.data(),frame);
            if(result!=source_result||native_entity!=cpp_entity||std::memcmp(&frame.scale,&scale,4))
                std::cerr<<"slowdown n="<<n<<" bits="<<std::hex<<*reinterpret_cast<std::uint32_t*>(native_entity.data()+0xc4)<<" flags CPU="<<*reinterpret_cast<std::uint32_t*>(native_entity.data()+0x354)<<" source="<<*reinterpret_cast<std::uint32_t*>(cpp_entity.data()+0x354)<<std::dec<<" results "<<result<<'/'<<source_result<<'\n';
            check("entity_slowdown_wrapper_empty_ANM_already_updated",result==source_result&&native_entity==cpp_entity&&std::memcmp(&frame.scale,&scale,4)==0);
        }
        for(unsigned n=0;n<6000;++n) {
            gp::EnemyAuxiliary28 cpu_health,source_health;for(auto& word:cpu_health.words)word=core_rng();source_health=cpu_health;
            const auto amount=static_cast<int>(core_rng());
            const auto expected=original<int>(0x4a3f80,&cpu_health,amount),actual=gp::apply_enemy_damage(source_health,amount);
            check("Enemy_health_apply_wrap_seventh_division_all28bytes",expected==actual&&std::memcmp(&cpu_health,&source_health,sizeof(cpu_health))==0);
            original<void>(0x4aa050,&cpu_health,amount);gp::record_enemy_damage(source_health,amount);
            check("Enemy_health_record_damage_preserves_life",std::memcmp(&cpu_health,&source_health,sizeof(cpu_health))==0);
            check("Enemy_health_positive_signed",original<bool>(0x4ab240,&cpu_health)==gp::enemy_health_positive(source_health));
            check("Enemy_health_forced_end_flag",original<unsigned>(0x4ab290,&cpu_health)==static_cast<unsigned>(gp::enemy_health_forced_end(source_health)));
            std::array<std::uint8_t,0x1c0> hud{};*reinterpret_cast<std::uint32_t*>(hud.data()+0x1bc)=n%3?core_rng():0;
            check("Enemy_HUD_damage_suppression",original<bool>(0x478160,hud.data())==gp::boss_damage_suppressed(hud.data()));
            th20::source::sprite::Animation cpu_animation{},source_animation{};for(auto& byte:reinterpret_cast<std::uint8_t(&)[sizeof(cpu_animation)]>(cpu_animation))byte=static_cast<std::uint8_t>(core_rng());source_animation=cpu_animation;
            const auto color=core_rng();original<void>(0x4ab610,&cpu_animation,color);gp::set_enemy_hit_color(source_animation,color);
            check("Enemy_hit_color_preserves_all_animation_bytes",std::memcmp(&cpu_animation,&source_animation,sizeof(cpu_animation))==0);
            alignas(4)std::array<std::uint8_t,0x3c> cpu_feedback,source_feedback;
            for(auto& byte:cpu_feedback)byte=static_cast<std::uint8_t>(core_rng());source_feedback=cpu_feedback;
            frame.scale=static_cast<float>(n%9)*.25f;const auto delta=static_cast<int>(core_rng()),limit=static_cast<int>(core_rng());
            original<void>(0x4aa000,cpu_feedback.data(),delta,limit);gp::extend_enemy_hit_feedback(source_feedback.data(),delta,limit,&frame.scale);
            check("Enemy_hit_feedback_positive_gate_add_or_cap",cpu_feedback==source_feedback);
        }
        #include "enemy_damage_cpu_cases.inc"
        #include "enemy_spawn_cpu_cases.inc"
        for(int test=0;test<400;++test) {
            gp::ScriptLoader source(environment);
            std::array<std::uint8_t,0x234> raw{};std::vector<gp::ScriptRecord> record_space(4096);
            auto* vector=reinterpret_cast<std::uintptr_t*>(raw.data()+0x20c);vector[0]=reinterpret_cast<std::uintptr_t>(std::pmr::get_default_resource());
            vector[1]=vector[2]=reinterpret_cast<std::uintptr_t>(record_space.data());vector[3]=reinterpret_cast<std::uintptr_t>(record_space.data()+record_space.size());
            std::vector<std::vector<std::uint8_t>> documents;
            const int files=1+test%5;documents.reserve(files);
            for(int f=0;f<files;++f) {
                std::vector<std::string> names;for(unsigned i=0,n=1+rng()%40;i<n;++i)names.push_back("sub"+std::to_string(rng()%15));
                documents.push_back(document(names));auto& file=documents.back();
                original<void>(0x53fe60,raw.data(),file.data());source.append(file);
                check("SCPT_file_and_subroutine_counts",*reinterpret_cast<std::uint32_t*>(raw.data()+4)==source.file_count&&*reinterpret_cast<std::uint32_t*>(raw.data()+8)==source.subroutine_count);
                const auto count=(vector[2]-vector[1])/8;
                check("SCPT_sorted_records_equal_names_stable",count==source.records.size()&&std::memcmp(reinterpret_cast<void*>(vector[1]),source.records.data(),count*8)==0);
                check("SCPT_64_file_slots_and_unused_fields",std::memcmp(raw.data()+0xc,source.files,512)==0);
                for(int query=0;query<18;++query) {
                    const auto name="sub"+std::to_string(query);
                    check("SCPT_lookup_exact_midpoint_with_duplicate_names",original<int>(0x540340,raw.data(),name.c_str())==source.find(name.c_str()));
                }
                for(std::size_t index=0;index<source.records.size();++index) {
                    const auto offset=static_cast<int>(rng());
                    check("SCPT_instruction_pointer_32bit_offset",original<std::uint8_t*>(0x53e8e0,raw.data(),static_cast<int>(index),offset)==source.instruction(static_cast<int>(index),offset));
                }
            }
        }
#include "enemy_vm_cpu_cases.inc"
#include "enemy_opcode_animation_cpu_cases.inc"
#include "enemy_opcode_creation_cpu_cases.inc"
#include "enemy_opcode_movement_cpu_cases.inc"
#include "enemy_opcode_state_cpu_cases.inc"
#include "enemy_drop_phase_cpu_cases.inc"
#include "enemy_shot_cpu_cases.inc"
#include "enemy_opcode_laser_cpu_cases.inc"
#include "enemy_opcode_misc_cpu_cases.inc"
#include "enemy_mesh_cpu_cases.inc"
#include "enemy_defeat_cpu_cases.inc"
#include "enemy_reads_cpu_cases.inc"
        unsigned total=0;for(auto [name,count]:counts)total+=count;
        std::ofstream out(argv[2]);out<<"{\"status\":"<<th20::json_string(failed?"failed":"passed")<<",\"cases\":"<<total<<",\"failed\":"<<failed<<",\"original_sha256\":"<<th20::json_string(expected_sha)<<",\"coverage\":{";
        bool first=true;for(auto [name,count]:counts){if(!first)out<<',';first=false;out<<th20::json_string(name)<<':'<<count;}out<<"},\"source_sha256\":{";first=true;
        for(auto& source:enemy_source_hashes){if(!first)out<<',';first=false;out<<th20::json_string(source.path)<<':'<<th20::json_string(source.sha);}
        out<<"},\"scope\":\"Exact controller scalar data/generation/HUD/primary helpers; full752-byte EnemyState construction, initialization, vector payloads and real shared-owner destruction; all variable destinations and nearest-enemy search; SCPT files without include blocks, stable records, name lookup and instruction addresses; slowdown wrapper including real pooled ANM and stale handles; full4a8260 with movement bit already set, invalid first ANM for no-damage branch, mesh absent and remaining real pool animations exercising orientation/position/parent/timer behavior; global/per-axis extended interpolation, motion aggregation and full4a7710 movement with parent following/interpolation/viewport/bounds/ANM dimensions. Health/feedback/clamp helpers; original life/time phase transitions, records, reward, HUD and inactive timeout notification; full4a5df0 pending damage, protection, bomb state without replacement allocation, callback ABI, health/feedback/accounting, flash/cooldown and countdown. Enemy ctor/spawn parameter application and full initialization with native vector payloads and generation effects. Generic ECL directly on actual 72-byte runtime, 75 core opcodes, mutable instructions, all stack/interpolator words and vector capacities, RNG, synchronous calls, direct async setup including missing lookup, and full 400 six-frame original allocating/deleting manager chains and identifier lookup. Full integer/float Enemy variable getter bodies, all d8f0..d96e cases on valid populated objects plus explicit float null defaults, RNG and PlayerTable clamp side effects. Entity groups 300/400, 53 scalar State opcodes, 24 Bullet parameter opcodes, all 15 Laser and 6 misc opcodes; exact domains and isolated recorded endpoints are described in entity_opcode_validation.md. Native drop/phase helpers, all vertex/owner bytes for Enemy mesh deformation, and recursive defeat objects/links/feedback/reward/script/callback order are included. Heavy 600 adapters, active damage-region queries, ANM allocation/replacement internals, player collision/graze, active timeout notification, devices/archive and whole-game equivalence are excluded.\"}\n";
        std::cout<<total<<" EnemyController/parser CPU comparisons, "<<failed<<" failed\n";return failed?1:0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 2;}
}
