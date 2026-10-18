// Standard SHT initialization callback bodies. ANM is absent; RNG, sound and
// immutable enemy queries are explicit native service boundaries.
{
    namespace gs=th20::source::game_session;namespace sp=th20::source::sprite;namespace dm=th20::source::damage;namespace sq=th20::source::scheduler;
    struct CallbackFiring final:pl::FiringServices {
        gs::Session& state;explicit CallbackFiring(gs::Session& s):state(s){}
        gs::Session& session() override{return state;}float clock_rate() override{return **reinterpret_cast<float**>(mapped_image_base+0x1aefe0);}
        float signed_random() override{return original<float>(0x4298e0,reinterpret_cast<void*>(mapped_image_base+0x1ba4a8));}
        pl::Shot* create_heap_shot() override{throw std::logic_error("Callback pool outside domain");}
        pl::ShotCallbacks callbacks(const pl::ShotRecord&) override{throw std::logic_error("Recursive callback lookup outside domain");}
        std::uint32_t spawn_animation(sp::AnimationFile&,int) override{throw std::logic_error("Callback ANM outside domain");}
        sp::Animation& animation(std::uint32_t& handle) override{return *original<sp::Animation*>(0x44ced0,&handle);}
        std::uint32_t create_damage(gs::Context&,const pl::Shot&) override{throw std::logic_error("Callback damage creation outside domain");}
        dm::Region* damage(std::uint32_t& handle) override{return original<dm::Region*>(0x4c0e10,&handle);}
        void retire(pl::Shot& shot) override{original<void>(0x506af0,&shot);}
        void sound_at(int id,float x) override{original<void>(0x426eb0,reinterpret_cast<void*>(mapped_image_base+0x1ba830),id,x);}
    };
    struct Callbacks final:pl::ShotCallbackEnvironment {
        CallbackFiring& host;explicit Callbacks(CallbackFiring& h):host(h){}
        pl::FiringServices& firing() override{return host;}
        sq::List& enemies(gs::Context& context) override{return *reinterpret_cast<sq::List*>(reinterpret_cast<std::uint8_t*>(context.objects_04[1])+0x108);}
        bool excluded_enemy(const void* enemy) override{return original<bool>(0x47a3e0,const_cast<void*>(enemy));}
        sp::Vec3 enemy_position(const void* enemy) override{return *original<sp::Vec3*>(0x47a2c0,const_cast<void*>(enemy));}
        std::uint32_t nearest_enemy(gs::Context& context,const sp::Vec2& point,float radius) override{std::uint32_t handle;original<void>(0x4aac00,context.objects_04[1],&handle,&point,radius);return handle;}
        void* find_enemy(std::uint32_t& handle) override{return original<void*>(0x4aaac0,&handle);}
        float random_angle() override{auto& random=*reinterpret_cast<th20::source::state::Random*>(mapped_image_base+0x1ba4a8);const auto value=original<std::uint32_t>(0x423ee0,&random);return pl::shot_random_angle(value,random.modulus);}
        sp::Animation* find_animation(std::uint32_t handle) override{return original<sp::Animation*>(0x44ced0,&handle);}
        void interrupt(std::uint32_t handle,int event) override{original<void>(0x44ef90,&handle,event);}
        void sound_pan(int id,float x) override{original<void>(0x429090,reinterpret_cast<void*>(mapped_image_base+0x1ba830),id,x);}
        void stop_sound(int id) override{original<void>(0x428890,reinterpret_cast<void*>(mapped_image_base+0x1ba830),id);}
        void retire_damage(dm::Region& region) override{original<void>(0x4c1f30,&region);}
        bool dialogue_active() override{auto* hud=*reinterpret_cast<void**>(mapped_image_base+0x1c06a4);return hud&&original<bool>(0x478160,hud);}
        int weapon_pattern() override{return original<int>(0x534210,host.state.contexts[0].overlay_owner);}
        int weapon_phase() override{return original<int>(0x4ff6d0,host.state.contexts[0].overlay_owner);}
        bool weapon_shooting() override{return original<bool>(0x506950,host.state.contexts[0].overlay_owner);}
        sp::Animation* animation_child(std::uint32_t& handle,int script,int occurrence) override{return original<sp::Animation*>(0x44c670,&handle,script,occurrence);}
        void cancel_rectangles(gs::Context& context,const sp::Vec3& center,const sp::Vec3& size,float angle) override{original<void>(0x47cc90,context.primary_owner,&center,&size,angle,0,0u);original<void>(0x4c9db0,context.objects_04[4],&center,&size,angle,0,1);}
        static int __fastcall fixture_pattern(void*,void*){return 0;}
        struct PanDevice {std::uintptr_t* methods;int value,count;};
        static long __stdcall fixture_pan(void* self,int pan){auto& device=*static_cast<PanDevice*>(self);device.value=pan;++device.count;return 0;}
    };
    auto& native_session=*reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568);native_session=gs::Session{};native_session.contexts[0].objects_04[0]=&owner;
    std::array<std::uint8_t,0x134> enemies_owner{};auto& enemies=*reinterpret_cast<sq::List*>(enemies_owner.data()+0x108);native_session.contexts[0].objects_04[1]=reinterpret_cast<th20::source::runtime::CallbackOwner*>(enemies_owner.data());
    std::array<std::array<std::uint8_t,0x428>,10> enemy_bytes{};std::array<sq::Link,10> links{};
    std::array<std::uint8_t,0xc460> damage_bytes{};auto& damage_owner=*reinterpret_cast<dm::HitCtrlInf*>(damage_bytes.data());native_session.contexts[0].object_28=reinterpret_cast<th20::source::runtime::CallbackOwner*>(damage_bytes.data());
    auto* sound=reinterpret_cast<std::uint8_t*>(mapped_image_base+0x1ba830);
    std::array<std::uint8_t,0x5d8> data{};std::array<pl::ShotRecord,2> rows{};auto& row=rows[0];rows[1].period=-1;put(data.data(),0x5d4,&row);
    std::array<std::uint8_t,0x80> overlay{};native_session.contexts[0].overlay_owner=reinterpret_cast<th20::source::runtime::CallbackOwner*>(overlay.data());
    std::array<std::uintptr_t,26> weapon_methods{};weapon_methods[25]=reinterpret_cast<std::uintptr_t>(&Callbacks::fixture_pattern);auto* weapon_table=weapon_methods.data();put(overlay.data(),0x2c,&weapon_table);put(overlay.data(),0x30,&weapon_table);
    std::array<std::uint8_t,0x1c0> laser_hud{};put(reinterpret_cast<void*>(mapped_image_base),0x1c06a4,laser_hud.data());
    std::array<std::uintptr_t,17> sound_methods{};sound_methods[16]=reinterpret_cast<std::uintptr_t>(&Callbacks::fixture_pan);Callbacks::PanDevice pan_device{sound_methods.data(),0,0};
    std::vector<std::uint8_t> bullet_bytes(0x286da8);std::array<std::uint8_t,0x58> laser_bytes{};sq::initialize_list(*reinterpret_cast<sq::List*>(bullet_bytes.data()+0x286d64));sq::initialize_list(*reinterpret_cast<sq::List*>(laser_bytes.data()+0x10));native_session.contexts[0].primary_owner=reinterpret_cast<th20::source::runtime::CallbackOwner*>(bullet_bytes.data());native_session.contexts[0].objects_04[4]=laser_bytes.data();
    sp::Animation shot_child{};shot_child.base.field_440=9;
    std::array<std::uint8_t,0xd10> sprite_bytes{};auto& animation=*reinterpret_cast<sp::PooledAnimation*>(sprite_bytes.data()+0x710);animation.active=1;animation.animation.handle=0x10000u;put(reinterpret_cast<void*>(mapped_image_base),0x1c0028,sprite_bytes.data());
    CallbackFiring callback_host(native_session);Callbacks callback_env(callback_host);
    const std::array<std::pair<unsigned,std::uint32_t>,31> cases{{{1,0x500270},{2,0x500c50},{3,0x500290},{4,0x500380},{5,0x5006f0},{6,0x500790},{7,0x500ec0},{8,0x5005d0},{9,0x5005d0},{10,0x5008c0},{11,0x500950},{12,0x5009f0},{13,0x414b50},{14,0x500dd0},{15,0x500ef0},{16,0x500fe0},{17,0x501110},{18,0x414b50},{19,0x5011d0},{20,0x501240},{21,0x501580},{22,0x5013b0},{23,0x501040},{24,0x5010a0},{25,0x5015a0},{26,0x501650},{27,0x500660},{28,0x501580},{29,0x501770},{30,0x501580},{31,0x5017c0}}};
    for(unsigned n=0;n<1400;++n){firing_case=n;firing_phase="standard Shot initializer callbacks";
        pl::construct_player_fields(owner);owner.shot_data=data.data();owner.context=&native_session.contexts[0];owner.position_614={finite(),finite(),0};auto& shot=owner.shots.pool[0];pl::select_context(shot,0,native_session);
        sq::initialize_list(enemies);for(unsigned i=0;i<10;++i){auto* enemy=enemy_bytes[i].data();put(enemy,0x88,i+1u);put(enemy,0x350,rng()%8?0u:rng());put(enemy,0x354,rng()%8?0u:rng());put(enemy,0x198,sp::Vec3{finite(),finite(),0});sq::initialize_link(links[i],reinterpret_cast<sq::Node*>(enemy));sq::append(enemies,links[i]);}
        owner.shots.timer_12580.current=std::bit_cast<int>(rng());row={};row.angle=n%4?finite():-1.5708f;row.fields_20[1]=std::bit_cast<std::uint32_t>(finite());row.fields_58[4]=std::bit_cast<std::uint32_t>(finite());
        shot.motion.position={finite(),finite(),0};shot.motion.angle_1c=finite()*40;shot.fields_3c[0]=rng();shot.fields_98[2]=rng();shot.vector_b0={finite(),finite()};shot.fields_b8[8]=0;shot.fields_b8[9]=1;shot.handle_18=0x10000u;shot.field_110=reinterpret_cast<std::uint32_t>(&owner.options[0]);owner.options[0].vector_78={std::bit_cast<int>(rng()),std::bit_cast<int>(rng())};owner.shots.handle_12560=std::bit_cast<std::uint32_t>(finite());owner.shots.field_12564=std::bit_cast<std::uint32_t>(finite());owner.vector_20c4.x=finite();
        original<void>(0x4bffb0,&damage_owner.pool[0]);sq::initialize_list(damage_owner.active);sq::initialize_link(damage_owner.pool[0].link,reinterpret_cast<sq::Node*>(&damage_owner.pool[0]));sq::append(damage_owner.active,damage_owner.pool[0].link);damage_owner.pool[0].handle=1;
        std::memset(sound,0,0x57e8);for(unsigned i=0;i<12;++i)put(sound,0x1c+i*0x208,-1);
        th20::source::state::Random random(0);random.state=rng()%0x7ffffffeu+1;random.upper=random.modulus=0x7fffffffu;
        for(const auto [index,address]:cases){
            const auto before=owner_storage;const auto before_animation=sprite_bytes;const auto before_damage=damage_bytes;const auto before_sound=std::vector<std::uint8_t>(sound,sound+0x57e8);std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),&random,sizeof(random));
            FloatingEnvironment::prepare();const int expected_return=original<int>(address,&shot,int(n));const auto expected=owner_storage;const auto expected_animation=sprite_bytes;const auto expected_damage=damage_bytes;const auto expected_sound=std::vector<std::uint8_t>(sound,sound+0x57e8);th20::source::state::Random expected_random(0);std::memcpy(&expected_random,reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),sizeof(random));
            owner_storage=before;damage_bytes=before_damage;sprite_bytes=before_animation;std::memcpy(sound,before_sound.data(),before_sound.size());std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),&random,sizeof(random));
            FloatingEnvironment::prepare();const int actual_return=pl::initialize_shot_callback(shot,index,int(n),callback_env);
            if(owner_storage!=expected&&n<20){std::size_t offset=0;while(owner_storage[offset]==expected[offset])++offset;std::cerr<<"Shot callback "<<index<<" case "<<n<<" byte "<<std::hex<<offset<<" expected angle "<<std::bit_cast<std::uint32_t>(reinterpret_cast<const pl::Player*>(expected.data())->shots.pool[0].motion.angle_1c)<<" source "<<std::bit_cast<std::uint32_t>(shot.motion.angle_1c)<<std::dec<<std::endl;}
            if(sprite_bytes!=expected_animation&&n<2){std::size_t offset=0;while(sprite_bytes[offset]==expected_animation[offset])++offset;std::cerr<<"Shot ANM callback "<<index<<" byte "<<std::hex<<offset<<" cpu "<<int(expected_animation[offset])<<" source "<<int(sprite_bytes[offset])<<std::dec<<std::endl;}
            check("Shot_standard_initializer_callback_all84060bytes",owner_storage==expected);check("Shot_standard_initializer_callback_return",expected_return==actual_return);
            check("Shot_standard_initializer_callback_RNG_state",std::memcmp(&expected_random,reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),sizeof(random))==0);
            check("Shot_standard_initializer_callback_damage_and_sound",damage_bytes==expected_damage&&std::memcmp(sound,expected_sound.data(),expected_sound.size())==0);
            check("Shot_initializer_callback_animation",sprite_bytes==expected_animation);
        }
        const std::array<std::pair<unsigned,std::uint32_t>,16> updates{{{1,0x501820},{2,0x501ba0},{3,0x501be0},{4,0x501f50},{5,0x502550},{6,0x501ba0},{7,0x501f30},{8,0x502a20},{9,0x502ad0},{10,0x502b10},{11,0x502b70},{12,0x502c50},{13,0x502db0},{14,0x502ad0},{15,0x502f70},{16,0x503240}}};
        for(const auto [index,address]:updates){
            shot.fields_98[1]=n%3;shot.fields_98[2]=n%4?0u:rng()%14;shot.fields_3c[0]=(rng()&~0x3cu)|((n%4)<<2);shot.motion.position={finite()*6,finite(),0};shot.motion.angle_1c=finite();shot.motion.field_18=finite();
            shot.timer_1c={};shot.timer_1c.current=int(n%240)-20;shot.timer_2c={};shot.timer_2c.current=n%9;shot.timer_2c.current_f=float(n%9);shot.vector_100={finite(),finite(),0};shot.fields_b8[0]=std::bit_cast<std::uint32_t>(finite());shot.fields_b8[1]=std::bit_cast<std::uint32_t>(finite());
            row.fields_3c[0]=std::uint32_t(int(n%4)-1)*20;row.fields_3c[1]=40;row.offset={finite(),finite()};row.damage=static_cast<std::int16_t>(rng());row.group=n%30;row.type=n%4?2:0;row.source=n%4;owner.options[0].fields_f4[1]=n%3;shot.field_110=n%2?0u:reinterpret_cast<std::uint32_t>(&owner.options[0]);
            owner.state=n%5;owner.shots.timer_12400.current=int(n%2)-1;owner.shots.field_1255c=n%2;shot.vector_b0.x=float(int(n%150)-5)*4;shot.fields_98[3]=n%2;shot.fields_98[4]=n%3;put(laser_hud.data(),0x1bc,n%11==0?1u:0u);put(overlay.data(),0x54,n%4);overlay[0x78]=n%7==0;
            original<void>(0x4bffb0,&damage_owner.pool[0]);sq::initialize_list(damage_owner.active);sq::initialize_list(damage_owner.free);sq::initialize_link(damage_owner.pool[0].link,reinterpret_cast<sq::Node*>(&damage_owner.pool[0]));sq::append(damage_owner.active,damage_owner.pool[0].link);damage_owner.pool[0].handle=1;damage_owner.pool[0].context=&native_session.contexts[0];shot.fields_b8[9]=n%11?1u:0u;
            put(sound,0x1994+20*0x18,&pan_device);pan_device.value=pan_device.count=0;
            shot.motion.vector_38={finite(),finite(),finite()};animation.animation.links[3].value=n%2?&shot_child:nullptr;shot_child.base.vector_50.x=finite();
            const auto before_child=shot_child;const auto before_laser=laser_bytes;
            const auto before=owner_storage;const auto before_animation=sprite_bytes;const auto before_damage=damage_bytes;const auto before_sound=std::vector<std::uint8_t>(sound,sound+0x57e8);std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),&random,sizeof(random));
            FloatingEnvironment::prepare();const int expected_return=original<int>(address,&shot);const auto expected=owner_storage;const auto expected_animation=sprite_bytes;const auto expected_damage=damage_bytes;const auto expected_sound=std::vector<std::uint8_t>(sound,sound+0x57e8);th20::source::state::Random expected_random(0);std::memcpy(&expected_random,reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),sizeof(random));
            const auto expected_pan=pan_device;pan_device.value=pan_device.count=0;
            const auto expected_child=shot_child;const auto expected_laser=laser_bytes;shot_child=before_child;laser_bytes=before_laser;
            owner_storage=before;sprite_bytes=before_animation;damage_bytes=before_damage;std::memcpy(sound,before_sound.data(),before_sound.size());std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),&random,sizeof(random));
            FloatingEnvironment::prepare();const int actual_return=pl::update_shot_callback(shot,index,callback_env);
            if(owner_storage!=expected&&failed<20){std::size_t offset=0;while(owner_storage[offset]==expected[offset])++offset;std::cerr<<"Shot update "<<index<<" case "<<n<<" byte "<<std::hex<<offset<<std::dec<<std::endl;}
            check("Shot_update_callback_all84060bytes",owner_storage==expected);check("Shot_update_callback_return",expected_return==actual_return);check("Shot_update_callback_RNG_state",std::memcmp(&expected_random,reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),sizeof(random))==0);check("Shot_update_callback_ANM",sprite_bytes==expected_animation);
            if(damage_bytes!=expected_damage&&failed<20){std::size_t offset=0;while(damage_bytes[offset]==expected_damage[offset])++offset;std::cerr<<"Shot damage update "<<index<<" case "<<n<<" byte "<<std::hex<<offset<<std::dec<<std::endl;}
            check("Shot_update_callback_damage_sound",damage_bytes==expected_damage&&std::memcmp(sound,expected_sound.data(),expected_sound.size())==0);
            check("Shot_update_callback_DirectSound_pan",pan_device.value==expected_pan.value&&pan_device.count==expected_pan.count);
            check("Shot_update_callback_child_and_cancel",std::memcmp(&shot_child,&expected_child,sizeof(shot_child))==0&&laser_bytes==expected_laser);
        }
    }
    #include "shot_hit_cpu_cases.inc"
    #include "stage_reset_cpu_cases.inc"
    #include "frame_helpers_cpu_cases.inc"
    #include "option_frame_cpu_cases.inc"
}
