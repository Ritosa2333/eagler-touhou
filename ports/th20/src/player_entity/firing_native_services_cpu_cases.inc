// 504e90 differential test. The ANM allocation/script, DamageRegion allocation,
// and RNG service boundaries are the SAME unmodified native oracle callees on
// both runs. This isolates the newly recovered initializer, not those services.
{
    namespace gs=th20::source::game_session;namespace sp=th20::source::sprite;namespace dm=th20::source::damage;namespace sq=th20::source::scheduler;
    const auto pe=th20::parse_pe(bytes);for(const auto& import:pe.imports)if(import.name=="GetCurrentThreadId")put(reinterpret_cast<void*>(mapped_image_base),import.iat_rva,reinterpret_cast<std::uintptr_t>(&GetCurrentThreadId));
    for(unsigned slot:{1u,9u,10u}){auto* lock=reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c0240+slot*0x30);std::memset(lock,0,0x30);lock[0]=0x101;lock[10]=GetCurrentThreadId();lock[11]=1;}
    auto* sprites=static_cast<sp::Controller*>(VirtualAlloc(nullptr,sizeof(sp::Controller),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));if(!sprites)throw std::bad_alloc();
    put(reinterpret_cast<void*>(mapped_image_base),0x1c0028,sprites);put(reinterpret_cast<void*>(mapped_image_base),0x1ba828,static_cast<void*>(nullptr));
    auto* rate=reinterpret_cast<float*>(mapped_image_base+0x1aefe4);put(reinterpret_cast<void*>(mapped_image_base),0x1aefe0,rate);*rate=1;
    auto& native_session=*reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568);native_session=gs::Session{};
    auto damage_storage=std::make_unique<std::uint8_t[]>(sizeof(dm::HitCtrlInf));auto& damage_owner=*reinterpret_cast<dm::HitCtrlInf*>(damage_storage.get());
    native_session.contexts[0].objects_04[0]=native_session.contexts[1].objects_04[0]=&owner;
    native_session.contexts[0].object_28=native_session.contexts[1].object_28=reinterpret_cast<th20::source::runtime::CallbackOwner*>(&damage_owner);
    native_session.contexts[0].current_player=&native_session.player_table.players[0];native_session.contexts[1].current_player=&native_session.player_table.players[1];
    const std::uint32_t program[]{0x000c0005,0,2,0x00080002,0,0x0008ffff,0};auto* code=reinterpret_cast<sp::AnmInstruction*>(const_cast<std::uint32_t*>(program));
    std::array<sp::AnmInstruction*,8> scripts;for(auto& script:scripts)script=code;std::array<sp::Animation,8> templates{};
    sp::AnimationFile animation_file{};animation_file.id=7;animation_file.stem="pl00";animation_file.templates=templates.data();animation_file.scripts=scripts.data();sprites->files[7]=&animation_file;
    std::array<std::uint8_t,0x5d8> data{};pl::ShotRecord row{};put(data.data(),0x5d4,&row);
    struct NativeBoundaries final:pl::FiringServices {
        gs::Session& state;float* rate;
        NativeBoundaries(gs::Session& s,float* r):state(s),rate(r){}
        gs::Session& session() override{return state;}
        float clock_rate() override{return *rate;}
        float signed_random() override{return original<float>(0x4298e0,reinterpret_cast<void*>(mapped_image_base+0x1ba4a8));}
        pl::Shot* create_heap_shot() override{throw std::logic_error("Initializer oracle excludes heap allocation");}
        pl::ShotCallbacks callbacks(const pl::ShotRecord& row) override{for(auto index:row.callbacks)if(index)throw std::logic_error("Initializer oracle excludes nonzero callback entries");return {};}
        std::uint32_t spawn_animation(sp::AnimationFile& file,int script) override{std::uint32_t handle;original<void>(0x450c70,&file,&handle,static_cast<void*>(nullptr),script,-1,static_cast<void*>(nullptr));return handle;}
        sp::Animation& animation(std::uint32_t& handle) override{return *original<sp::Animation*>(0x44ced0,&handle);}
        std::uint32_t create_damage(gs::Context& context,const pl::Shot& shot) override{std::uint32_t handle;original<void>(0x4c1ae0,context.object_28,&handle,&shot.motion.position,shot.vector_b0.x,shot.vector_b0.y,shot.motion.angle_1c,9999999,std::bit_cast<int>(shot.fields_98[5]));return handle;}
        dm::Region* damage(std::uint32_t& handle) override{return original<dm::Region*>(0x4c0e10,&handle);}
        void retire(pl::Shot&) override{throw std::logic_error("Initializer oracle excludes callback retirement");}
        void sound_at(int,float) override{throw std::logic_error("Initializer oracle sound disabled by SHT sentinel");}
    } boundaries(native_session,rate);
    struct SpriteSnapshot {
        sp::PooledAnimation slot;sp::Controller::AnimationList lists[3];sp::AnimationLink free;std::uint32_t generation;
        explicit SpriteSnapshot(const sp::Controller& c):slot(c.pool[0]),free(c.free_sentinel),generation(c.field_7d40e88){std::memcpy(lists,c.lists,sizeof(lists));}
        void restore(sp::Controller& c)const{c.pool[0]=slot;std::memcpy(c.lists,lists,sizeof(lists));c.free_sentinel=free;c.field_7d40e88=generation;}
    };
    for(unsigned n=0;n<1600;++n){
        firing_case=n;firing_phase="fixture setup";
        for(auto& list:sprites->lists){list={};list.tail=&list.sentinel;}sprites->free_sentinel={};sprites->field_7d40e88=(rng()&0xffff0000u)|1u;sprites->field_6c4=0;
        auto& pool=sprites->pool[0];pool={};sp::construct_pooled_animation(pool);pool.index=0;pool.free_link.value=&pool.animation;sq::insert_after(reinterpret_cast<sq::Link&>(sprites->free_sentinel),reinterpret_cast<sq::Link&>(pool.free_link));
        for(unsigned i=0;i<8;++i){auto& visual=templates[i];visual={};sp::construct_animation(visual);sp::reset_animation_state(visual);visual.base.fields_10_28[2]=7;visual.base.fields_10_28[3]=7;visual.base.fields_10_28[5]=i;visual.base.fields_10_28[6]=0;visual.base.flags[2]=1u|((n%8)<<21);visual.base.vector_50.x=n%2?-3.0f:2.0f;}
        std::memset(damage_storage.get(),0,sizeof(dm::HitCtrlInf));original<void>(0x4bffb0,&damage_owner.pool[0]);sq::initialize_list(damage_owner.active);sq::initialize_list(damage_owner.free);sq::initialize_link(damage_owner.pool[0].link,reinterpret_cast<sq::Node*>(&damage_owner.pool[0]));sq::append(damage_owner.free,damage_owner.pool[0].link);damage_owner.next_handle=1;damage_owner.view_index=n%2;damage_owner.context=&native_session.contexts[n%2];
        pl::construct_player_fields(owner);owner.shot_data=data.data();owner.animation_file=&animation_file;owner.field_24=reinterpret_cast<std::uint32_t>(&animation_file);owner.position_614={finite(),finite(),finite()};pl::initialize_shots(owner.shots,n%2,native_session);owner.focused_204c=n%2;owner.shots.field_1255c=n%3?0:8u;
        for(auto& option:owner.options){option.vector_78={std::bit_cast<int>(rng()),std::bit_cast<int>(rng())};option.field_d4=std::bit_cast<std::uint32_t>(finite());}
        native_session.player_table.players[0].fields_30[(0x9c-0x30)/4]=rng();native_session.player_table.players[0].fields_30[(0xa0-0x30)/4]=rng();
        row={};row.type=n%8;row.source=n%7;row.damage=std::int16_t(rng());row.size={finite(),finite()};row.offset={finite(),finite()};row.angle=n%5==0?1001.0f:n%5==1?997.0f:n%5==2?-1001.0f:finite();row.speed=finite();row.animation=n%8;row.sound=-1;row.group=n%30;
        auto& shot=owner.shots.pool[0];for(auto& word:shot.fields_3c)word=rng();shot.fields_98[3]=rng();shot.flags=rng();*rate=(n%5+1)*0.25f;
        th20::source::state::Random random(0);random.state=rng()%0x7ffffffeu+1;random.upper=random.modulus=0x7fffffffu;std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),&random,sizeof(random));
        const auto before=owner_storage;const auto record_before=native_session.player_table;const SpriteSnapshot sprite_before(*sprites);const auto generation_before=animation_file.fields_5c[3];
        const auto damage_before=std::vector<std::uint8_t>(damage_storage.get(),damage_storage.get()+sizeof(dm::HitCtrlInf));
        auto* attachment=row.source?&owner.options[(row.source&15)-1]:nullptr;
        firing_phase="original 504e90";FloatingEnvironment::prepare();const int expected_return=original<int>(0x504e90,&shot,0u,int(n),&owner.position_614,attachment,int(n%2));const auto expected=owner_storage;const auto records_expected=native_session.player_table;
        const SpriteSnapshot sprites_expected(*sprites);const auto damage_expected=std::vector<std::uint8_t>(damage_storage.get(),damage_storage.get()+sizeof(dm::HitCtrlInf));th20::source::state::Random random_expected(0);std::memcpy(&random_expected,reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),sizeof(random_expected));
        owner_storage=before;native_session.player_table=record_before;sprite_before.restore(*sprites);animation_file.fields_5c[3]=generation_before;std::memcpy(damage_storage.get(),damage_before.data(),damage_before.size());std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),&random,sizeof(random));
        firing_phase="source initializer with native service boundaries";FloatingEnvironment::prepare();const int actual_return=pl::initialize_shot(shot,0u,int(n),owner.position_614,attachment,n%2,boundaries);
        if(owner_storage!=expected&&failed<20){std::size_t first=0;while(owner_storage[first]==expected[first])++first;std::cerr<<"Shot initializer case "<<n<<" byte +"<<std::hex<<first<<std::dec<<'\n';}
        check("Shot_initializer_504e90_all84060bytes_native_service_boundaries",owner_storage==expected);check("Shot_initializer_504e90_return",actual_return==expected_return);
        check("Shot_initializer_504e90_PlayerTable_bonus_sideeffects",std::memcmp(&native_session.player_table,&records_expected,sizeof(records_expected))==0);
        check("Shot_initializer_504e90_ANM_full1536byte_slot",std::memcmp(&sprites->pool[0],&sprites_expected.slot,sizeof(pool))==0);
        check("Shot_initializer_504e90_damage_all50272bytes",std::memcmp(damage_storage.get(),damage_expected.data(),damage_expected.size())==0);
        check("Shot_initializer_504e90_RNG_state_and_call_sequence",std::memcmp(reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),&random_expected,sizeof(random_expected))==0);
    }
    VirtualFree(sprites,0,MEM_RELEASE);
}
