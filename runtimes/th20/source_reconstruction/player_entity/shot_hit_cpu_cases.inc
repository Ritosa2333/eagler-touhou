// Included in the callback fixture: services call unmodified original ANM and
// DamageRegion endpoints. Effect-spawning branches get a separate fixture.
{
    struct HitHost:pl::ShotHitServices {
        Callbacks& env;explicit HitHost(Callbacks& e):env(e){}
        pl::ShotCallbackEnvironment& callbacks() override{return env;}
        int default_hit(pl::Shot& shot) override{return original<int>(0x504af0,&shot);}
        float random_signed(unsigned stream) override{return original<float>(0x4298e0,reinterpret_cast<void*>(mapped_image_base+0x1ba4a8+stream*28));}
        std::uint32_t random_bounded(unsigned stream,std::uint32_t limit) override{return original<std::uint32_t>(0x423ea0,reinterpret_cast<void*>(mapped_image_base+0x1ba4a8+stream*28),limit);}
        sp::AnimationFile& effect_file(gs::Context&) override{throw std::logic_error("Effect creation uses dedicated fixture");}
        std::uint32_t spawn(sp::AnimationFile&,const char*,int,const sp::Vec3&,float) override{throw std::logic_error("Effect creation uses dedicated fixture");}
        dm::Region* create_circle(gs::Context& context,const sp::Vec3& position,float radius,float growth,int frames,int damage) override{std::uint32_t handle;original<std::uint32_t*>(0x4c1b80,context.object_28,&handle,&position,radius,growth,frames,damage);return original<dm::Region*>(0x4c0d00,context.object_28,handle);}
        void remember_effect(gs::Context& context,std::uint32_t handle) override{original<std::uint32_t>(0x497d20,context.objects_04[7],handle);}
    } hit_host(callback_env);
    const std::array<std::pair<unsigned,std::uint32_t>,7> hits{{{2,0x503450},{3,0x503ad0},{4,0x503b70},{5,0x504180},{6,0x503d40},{9,0x504160},{10,0x503b70}}};
    for(unsigned n=0;n<1600;++n){
        for(const auto [index,address]:hits){firing_case=n;firing_phase="Shot hit damage callbacks";
            pl::construct_player_fields(owner);owner.shot_data=data.data();owner.context=&native_session.contexts[0];auto& shot=owner.shots.pool[0];pl::select_context(shot,0,native_session);
            row={};row.damage=static_cast<std::int16_t>(rng());row.fields_58[4]=std::bit_cast<std::uint32_t>(finite());shot.fields_98[3]=rng();shot.fields_98[4]=n%3;shot.fields_98[5]=rng();shot.handle_18=0x10000u;shot.fields_b8[9]=n%9?1u:0u;shot.byte_114=n%2;shot.motion.position={finite(),finite(),0};shot.motion.angle_1c=finite();shot.motion.field_18=finite();
            shot.timer_1c={};shot.timer_1c.current=n%120;shot.timer_1c.previous=index==2?shot.timer_1c.current:n%3?shot.timer_1c.current-1:shot.timer_1c.current;
            std::memset(damage_bytes.data(),0,damage_bytes.size());sq::initialize_list(damage_owner.active);sq::initialize_list(damage_owner.free);damage_owner.next_handle=2;damage_owner.context=&native_session.contexts[0];
            for(unsigned i=0;i<2;++i){original<void>(0x4bffb0,&damage_owner.pool[i]);sq::initialize_link(damage_owner.pool[i].link,reinterpret_cast<sq::Node*>(&damage_owner.pool[i]));damage_owner.pool[i].context=&native_session.contexts[0];}
            sq::append(damage_owner.active,damage_owner.pool[0].link);sq::append(damage_owner.free,damage_owner.pool[1].link);damage_owner.pool[0].handle=1;damage_owner.pool[0].damage=int(rng());damage_owner.pool[0].size={finite(),std::fabs(finite())+1};
            const sp::Vec3 position{finite(),finite(),0};const sp::Vec2 size{finite(),finite()};const auto* size_pointer=n%2?&size:nullptr;const float angle=finite(),radius=std::fabs(finite())+1;
            const auto before=owner_storage;const auto before_animation=sprite_bytes;const auto before_child=shot_child;const auto before_damage=damage_bytes;const auto before_sound=std::vector<std::uint8_t>(sound,sound+0x57e8);
            FloatingEnvironment::prepare();const int expected_return=original<int>(address,&shot,&position,size_pointer,angle,radius);const auto expected=owner_storage;const auto expected_animation=sprite_bytes;const auto expected_child=shot_child;const auto expected_damage=damage_bytes;const auto expected_sound=std::vector<std::uint8_t>(sound,sound+0x57e8);
            owner_storage=before;sprite_bytes=before_animation;shot_child=before_child;damage_bytes=before_damage;std::memcpy(sound,before_sound.data(),before_sound.size());
            FloatingEnvironment::prepare();const int actual_return=pl::hit_shot_callback(shot,index,position,size_pointer,angle,radius,hit_host);
            if((owner_storage!=expected||damage_bytes!=expected_damage||actual_return!=expected_return)&&failed<12){std::size_t offset=0;while(offset<damage_bytes.size()&&damage_bytes[offset]==expected_damage[offset])++offset;std::cerr<<"Hit callback "<<index<<" case "<<n<<" return "<<expected_return<<"/"<<actual_return<<" damage byte "<<std::hex<<offset<<std::dec<<'\n';}
            check("Shot_hit_damage_body_all_player_bytes",owner_storage==expected);check("Shot_hit_damage_body_return",expected_return==actual_return);check("Shot_hit_damage_body_damage_owner",damage_bytes==expected_damage);check("Shot_hit_damage_body_ANM_and_sound",sprite_bytes==expected_animation&&std::memcmp(&shot_child,&expected_child,sizeof(shot_child))==0&&std::memcmp(sound,expected_sound.data(),expected_sound.size())==0);
        }
    }
    std::vector<std::uint8_t> effect_bytes(0x13044);auto& effect_owner=*reinterpret_cast<th20::source::effects::Controller*>(effect_bytes.data());
    for(unsigned n=0;n<1400;++n){put(effect_bytes.data(),0x20,n%1024u);for(unsigned i=0;i<1024;++i)put(effect_bytes.data(),0x3c+i*4,i%5==0?0u:i%7==0?0x10000u:((rng()|0x80000000u)&0xffff0000u));const auto before=effect_bytes;const auto handle=rng();
        const auto expected_return=original<std::uint32_t>(0x497d20,effect_bytes.data(),handle);const auto expected=effect_bytes;effect_bytes=before;const auto actual_return=pl::remember_shot_effect(effect_owner,handle,callback_env);
        check("Shot_effect_registry_old_cursor_new_candidate_all_bytes",effect_bytes==expected);check("Shot_effect_registry_return",expected_return==actual_return);
    }
    #include "shot_hit_effect_cpu_cases.inc"
}
