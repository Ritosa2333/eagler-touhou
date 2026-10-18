// Required archive/device test boundaries are shared with the resource fixture.
// This executable records active entity/special-manager calls; it never claims
// that those unrecovered bodies are implemented by the fixture.
#define wmain resource_fixture_main
#include "enemy_resource_tests.cpp"
#undef wmain
#include "enemy_frame.hpp"
#include "enemy_update.hpp"
#include "enemy_movement.hpp"
#include "enemy_damage_test_fixture.hpp"
#include <array>
#include <limits>
namespace {
template<class T>void field(void* object,std::size_t offset,T value){std::memcpy(static_cast<std::uint8_t*>(object)+offset,&value,sizeof(value));}
template<class T>T field(const void* object,std::size_t offset){T value;std::memcpy(&value,static_cast<const std::uint8_t*>(object)+offset,sizeof(value));return value;}
std::vector<void*> guarded_entity_pages;
struct Entity final:rt::CallbackOwner {
    std::uint8_t unknown[0x348]{}; //only test storage, known prefix through flags354
    q::Link link{};
    unsigned id;int outcome;
    std::vector<int>* events;
    q::List* nested_list=nullptr;
    std::array<gp::EnemyAnimationLink,2> animations{};
    Entity(unsigned number,int result,std::vector<int>& trace):id(number),outcome(result),events(&trace){q::initialize_link(link,reinterpret_cast<q::Node*>(this));}
    static void* operator new(std::size_t size) {
        void* page=VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
        if(!page)throw std::bad_alloc();guarded_entity_pages.push_back(page);return page;
    }
    static void operator delete(void* page) noexcept {
        // Keep the retired address reserved, but fault on ANY later access.
        // This detects the old iterator write without relying on heap reuse.
        DWORD old=0;if(!VirtualProtect(page,4096,PAGE_NOACCESS,&old))std::abort();
    }
    ~Entity() override {
        events->push_back(100+id);
        if(nested_list)for(q::Iterator inner(nested_list->sentinel.next);inner.current;inner.advance()){}
        q::unlink(link);
    }
};
static_assert(offsetof(Entity,link)==0x358);
struct GuardEnvironment final:Environment {
    void retire_entity(void* entity) override {delete static_cast<Entity*>(entity);}
};
struct FrameEnvironment final:gp::EnemyFrameServices {
    q::List* nested_list=nullptr;
    float scale=1.0f;
    std::vector<int> events;
    std::vector<float> sampled_scales;
    std::array<std::uint8_t,0x1cc> hud{};
    std::array<s::Animation,8> animations{};
    const float* timer_rate() override{return &scale;}
    float& clock_scale() override{return scale;}
    void update_boss_time(std::int32_t seconds,std::int32_t hundredths) override {events.push_back(200);gp::store_boss_time(hud.data(),seconds,hundredths);}
    int update_entity_state(void* state) override {
        auto& entity=*reinterpret_cast<Entity*>(static_cast<std::uint8_t*>(state)-0x88);
        if(nested_list)for(q::Iterator inner(nested_list->sentinel.next);inner.current;inner.advance()){}
        events.push_back(entity.id);sampled_scales.push_back(scale);
        field(&entity,0x354,field<std::uint32_t>(&entity,0x354)|4u);
        return entity.outcome;
    }
    void update_special_objects() override{events.push_back(201);}
    s::Animation* animation(std::uint32_t handle) override {return handle<animations.size()?&animations[handle]:nullptr;}
};
std::vector<int>* update_events=nullptr;int callback_result=0;
int __fastcall update_callback(gp::EnemyState* state,void*) {
    update_events->push_back(4);++state->field_04;return callback_result;
}
struct UpdateEnvironment final:gp::EnemyUpdateServices {
    std::vector<int> events;float scale=.5f;int move_result=0,script_result=0,damage_result=0;
    const float* timer_rate() override{return &scale;}
    float script_delta(const th20::recovered::Timer&) override{events.push_back(2);return scale;}
    s::Animation* animation(std::uint32_t) override{throw std::logic_error("No source-order fixture animation lookup expected");}
    int move(gp::EnemyState&) override{events.push_back(1);return move_result;}
    int run_scripts(void*,float delta) override{if(delta!=scale)throw std::logic_error("Wrong script clock delta");events.push_back(3);return script_result;}
    int damage(gp::EnemyState&) override{events.push_back(5);return damage_result;}
    void mesh(gp::EnemyState&) override{events.push_back(6);}
};
struct MovementEnvironment final:gp::EnemyMovementServices {
    float scale=1;bool old_present=true;std::vector<int> events;s::Animation old{},created{};s::AnimationFile file{};
    int script=-1,layer=-1;unsigned slot=0;s::Vec3 origin{};
    const float* timer_rate() override{return &scale;}float clock_scale() override{return scale;}
    s::Animation* animation(std::uint32_t handle) override{events.push_back(1);return handle==2?&created:handle==1&&old_present?&old:nullptr;}
    s::Vec3 viewport_offset() override{return {};}
    s::AnimationFile& animation_file(gp::EnemyState&,unsigned index) override{events.push_back(2);slot=index;return file;}
    void delete_animation(std::uint32_t& handle) override{events.push_back(3);handle=0;}
    std::uint32_t spawn_animation(s::AnimationFile& selected,int sub,const s::Vec3& position,int draw_layer) override{if(&selected!=&file)throw std::logic_error("Wrong movement animation file");events.push_back(4);script=sub;origin=position;layer=draw_layer;return 2;}
    float animation_height(s::Animation&) override{events.push_back(5);return -16;}
    float animation_width(s::Animation&) override{events.push_back(6);return -32;}
};
}
int wmain(int argc,wchar_t** argv) {
    unsigned assertions=0;
    try {
        if(argc!=2)throw std::runtime_error("Usage: enemy_frame_tests REPORT.json");
        const auto check=[&](bool condition,const char* reason){++assertions;if(!condition)throw std::runtime_error(reason);};
        #include "enemy_damage_source_cases.inc"
        for(bool nested:{false,true})for(unsigned pattern=0;pattern<256;++pattern) {
            GuardEnvironment services;FrameEnvironment frame;gp::EnemyController enemy(services);
            if(nested)frame.nested_list=&enemy.enemies;
            enemy.context=&th20::source::game_session::context(0);enemy.player_index=1;
            std::array<std::uint8_t,0x2240> primary{};enemy.context->objects_04[0]=primary.data();
            gp::set_primary_entity_scale(primary.data(),pattern%3==0?1.02f:pattern%3==1?1.01f:std::numeric_limits<float>::quiet_NaN());
            field(primary.data(),0x14,0xa5a5a5a5u);
            enemy.data.field_9c=enemy.data.field_a0=0xffffffff;
            enemy.field_cc=1;th20::recovered::timer_set(enemy.timer_d0,pattern%2?61:1);
            th20::recovered::timer_set(enemy.data.timer_8c,9);
            std::array<Entity*,4> entities{};std::vector<int> expected{200};
            for(unsigned i=0;i<4;++i) {
                const bool pre_deleted=(pattern&(1u<<i))!=0,returned_delete=(pattern&(1u<<(i+4)))!=0;
                auto* entity=entities[i]=new Entity(i,returned_delete?-1:0,frame.events);
                if(nested)entity->nested_list=&enemy.enemies;
                field(entity,0x354,pre_deleted?512u:0u);q::append(enemy.enemies,entity->link);
                if(!pre_deleted)expected.push_back(i);
                if(pre_deleted||returned_delete)expected.push_back(100+i);
            }
            expected.push_back(201);
            check(gp::update_enemy_controller(enemy,frame)==1,"Frame callback must return original EAX=1");
            check(frame.events==expected,"Enemy visit/delete/special-manager order differs");
            check(services.controller->field_6c4==1&&enemy.data.field_9c==0&&enemy.data.field_a0==0,"Frame initial field writes differ");
            check(enemy.timer_d0.current==(pattern%2?60:0)&&enemy.field_cc==(pattern%2?1u:0u),"Countdown decrement/end differs");
            check(field<int>(frame.hud.data(),0x1c4)==(pattern%2?1:0)&&field<int>(frame.hud.data(),0x1c8)==0,"HUD seconds/hundredths differ");
            check(enemy.data.timer_8c.previous==9&&enemy.data.timer_8c.current==10,"End-of-frame timer differs");
            check(field<std::uint32_t>(primary.data(),0x14)==((0xa5a5a5a5u&~32u)|(pattern%3==0?32u:0u))&&gp::primary_entity_scale(primary.data())==1.0f,"Primary scale/flag reset differs");
            for(unsigned i=0;i<4;++i)if(!(pattern&(1u<<i))&&!(pattern&(1u<<(i+4))))
                check(!(field<std::uint32_t>(entities[i],0x354)&4u),"Surviving entity retains updated bit");
            enemy.clear_entities();enemy.context->objects_04[0]=nullptr;
            check(enemy.enemies.sentinel.next==nullptr,"Remaining source entity destructors did not unlink");
        }
        // Test active slowdown propagation separately with real Animation bytes
        // and a recorded body invocation, including mutable slowdown rereads.
        for(float slowdown:std::array<float,7>{-1.0f,0.0f,0.25f,1.0f,2.0f,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}) {
            for(unsigned old_flag:{0u,0x10000u}) {
                FrameEnvironment frame;frame.scale=0.5f;Entity entity(3,17,frame.events);
                field(&entity,0xc4,slowdown);field(&entity,0x354,old_flag);
                entity.animations[0].handle=2;entity.animations[1].handle=99;
                field(&entity,0x98,entity.animations.data());field(&entity,0x9c,entity.animations.data()+2);
                field(&frame.animations[2],0x560,13.0f);
                check(gp::update_enemy_with_time_scale(&entity,frame)==17,"Slowed entity return value differs");
                check(frame.scale==0.5f&&frame.events==std::vector<int>{3},"Clock restoration or body invocation differs");
                const auto observed=frame.sampled_scales.at(0);float expected=0.5f;
                if(slowdown==0.25f)expected=0.375f;else if(slowdown>=1.0f)expected=0.0f;
                check(std::isnan(slowdown)?std::isnan(observed):observed==expected,"Clock slowdown operation/order differs");
                const float animation_scale=field<float>(&frame.animations[2],0x560);
                if(!(slowdown<=0))check(std::isnan(slowdown)?std::isnan(animation_scale):animation_scale==slowdown,"ANM slowdown propagation differs");
                else check(animation_scale==(old_flag?0.0f:13.0f),"Previous slowdown reset differs");
                check(entity.animations[1].handle==(old_flag||!(slowdown<=0)?0u:99u),"Stale slowdown ANM handle was not invalidated");
                check((field<std::uint32_t>(&entity,0x354)&0x10000u)==(old_flag||!(slowdown<=0)?0x10000u:0u),"Slowdown bit16 lifetime differs");
            }
        }
        for(unsigned pattern=0;pattern<256;++pattern) {
            gp::EnemyState state;UpdateEnvironment update;update_events=&update.events;
            state.fields_2c8[1]=(pattern&1?4u:0u)|(pattern&2?0x4000000u:0u)|(pattern&128?0x400u:0u);
            update.move_result=pattern&4?23:0;update.script_result=pattern&8?-7:0;callback_result=pattern&32?99:0;update.damage_result=pattern&64?6:0;
            state.fields_2c8[6]=pattern&16?reinterpret_cast<std::uint32_t>(&update_callback):0;
            th20::recovered::timer_set(state.timer_a8,7);th20::recovered::timer_set(state.timer_b8,9);
            th20::recovered::timer_set(state.timer_288,1);th20::recovered::timer_set(state.timer_298,-1);
            th20::recovered::timer_set(state.pattern_1a8.timer_90,11);
            std::vector<int> expected;int outcome=0;bool callback_called=false,completed=false;
            if(!(pattern&1)) {
                if(!(pattern&2)) {
                    expected.push_back(1);
                    if(update.move_result)outcome=-1;
                    else {
                        expected.push_back(2);expected.push_back(3);
                        if(update.script_result)outcome=-1;
                        else if(pattern&16){expected.push_back(4);callback_called=true;if(callback_result)outcome=-1;}
                    }
                }
                if(!outcome){expected.push_back(5);if(update.damage_result)outcome=-1;else{expected.push_back(6);completed=true;}}
            }
            check(gp::update_enemy_state(state,update)==outcome,"Enemy script/damage failure return differs");
            check(update.events==expected,"Enemy movement/script/callback/damage/mesh ordering differs");
            check(state.field_04==(callback_called?1u:0u),"Enemy callback ECX points to wrong State");
            const bool script_bit=completed?false:(pattern&1)?(pattern&2)!=0:true;
            check((state.fields_2c8[1]&4u)!=0&&((state.fields_2c8[1]&0x4000000u)!=0)==script_bit,"Enemy frame/script gate lifetimes differ");
            check(state.timer_a8.current_f==(completed?7.5f:7.0f)&&state.timer_b8.current_f==(completed?9.5f:9.0f)&&state.pattern_1a8.timer_90.current_f==(completed?11.5f:11.0f),"Enemy successful-frame timers differ");
            check(state.timer_288.current_f==(completed?.5f:1.0f)&&state.timer_298.current_f==-1,"Enemy positive countdown-only decrement differs");
        }
        update_events=nullptr;
        for(int old:{-2,-1,0,1,2})for(int direction:{-1,0,1})for(bool present:{false,true}) {
            gp::EnemyState state;state.animations.resize(1);state.movements.resize(1);MovementEnvironment host;host.old_present=present;host.old.base.vector_2c={7,11,13};
            state.fields_2c8[1]=0x10;state.fields_1c[1]=2;state.fields_1c[3]=100;state.fields_1c[4]=static_cast<std::uint32_t>(old);state.fields_1c[6]=4;
            state.animations[0].handle=1;
            const s::Vec3 position{static_cast<float>(direction),200,0};std::memcpy(state.movements[0].motion.words,&position,12);state.movements[0].motion.words[17]=32;
            int transition=0;if(old==-1)transition=direction==0?3:2;else if(old==0)transition=direction==-1?1:2;else if(old==1)transition=direction==0?4:1;
            std::vector<int> expected;
            if(old!=direction){expected={1,2};if(present)expected.push_back(3);expected.insert(expected.end(),{4,1,5,6});}
            else{expected={1};if(present)expected.insert(expected.end(),{5,6});}
            check(gp::update_enemy_movement(state,host)==0,"In-bounds movement must succeed");
            check(host.events==expected,"Direction animation lookup/delete/spawn/dimension order differs");
            check(th20::recovered::signed_bits(state.fields_1c[4])==direction,"Direction classification field differs");
            if(old!=direction) {
                check(host.slot==2&&host.script==100+transition&&host.layer==11,"Directional script or layer differs");
                const s::Vec3 expected_origin=present?s::Vec3{7,11,13}:s::Vec3{};
                check(std::memcmp(&host.origin,&expected_origin,12)==0&&state.animations[0].handle==2,"New directional animation position or handle differs");
            } else check(state.animations[0].handle==(present?1u:0u),"Unchanged-direction handle resolution differs");
            check((old!=direction||present)?state.vector_170.x==16&&state.vector_170.y==32:state.vector_170.x==0&&state.vector_170.y==0,"Animation extent absolute values differ");
        }
        for(void* page:guarded_entity_pages)VirtualFree(page,0,MEM_RELEASE);
        guarded_entity_pages.clear();
        std::ofstream report(argv[1]);report<<"{\"status\":\"passed\",\"assertions\":"<<assertions<<",\"retirement_guard\":\"PAGE_NOACCESS after actual destruction; 512 deletion masks with and without nested queries in update/destruction, plus clear_entities\",\"source_sha256\":{";bool first=true;
        for(auto& source:enemy_source_hashes){if(!first)report<<',';first=false;report<<th20::json_string(source.path)<<':'<<th20::json_string(source.sha);}
        report<<"},\"scope\":\"Recovered4a5040 order, source intrusive-list destruction, countdown/HUD/player writes and4a8760 slowdown with recorded required entity and special-manager calls; 4ab4c0/4a8260 movement/script/callback/damage/mesh failure order, gate lifetimes and successful timers with recorded required bodies; 4a7710 direction classification/transition scripts/ANM lookup/file selection/delete/spawn/dimension order using recorded allocation boundaries. 4a5df0 query geometry/order, preview retirement, health and bonus/reduction ordering, phase-script dispatch, timeout writes, circle/rectangle/graze parameters and bomb-animation switching with recorded external bodies. Active ECL/entity bodies and full original-frame equivalence are excluded.\"}\n";
        std::cout<<assertions<<" EnemyController frame-order assertions passed\n";return 0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
