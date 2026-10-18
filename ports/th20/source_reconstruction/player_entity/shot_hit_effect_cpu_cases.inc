{
    auto* sprites=static_cast<sp::Controller*>(VirtualAlloc(nullptr,sizeof(sp::Controller),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));if(!sprites)throw std::bad_alloc();put(reinterpret_cast<void*>(mapped_image_base),0x1c0028,sprites);
    const std::uint32_t program[]{0x000c0005,0,2,0x00080002,0,0x0008ffff,0};auto* code=reinterpret_cast<sp::AnmInstruction*>(const_cast<std::uint32_t*>(program));
    std::array<sp::AnmInstruction*,149> scripts;for(auto& script:scripts)script=code;std::vector<sp::Animation> templates(149);
    sp::AnimationFile effect_file{};effect_file.id=7;effect_file.stem="effect";effect_file.templates=templates.data();effect_file.scripts=scripts.data();sprites->files[7]=&effect_file;
    for(unsigned i=0;i<templates.size();++i){auto& visual=templates[i];sp::construct_animation(visual);sp::reset_animation_state(visual);visual.base.fields_10_28[2]=7;visual.base.fields_10_28[3]=7;visual.base.fields_10_28[5]=i;visual.base.fields_10_28[6]=0;visual.base.flags[2]=1;}
    struct SpawnHost final:HitHost {
        using HitHost::HitHost;
        sp::AnimationFile& effect_file(gs::Context& context) override{sp::AnimationFile* file;std::memcpy(&file,static_cast<std::uint8_t*>(context.objects_04[7])+0x10,4);return *file;}
        std::uint32_t spawn(sp::AnimationFile& file,const char* name,int script,const sp::Vec3& position,float angle) override{std::uint32_t handle;original<void>(0x4790e0,&file,&handle,name,script,&position,angle,-1,static_cast<void*>(nullptr));return handle;}
        void remember_effect(gs::Context& context,std::uint32_t handle) override{pl::remember_shot_effect(*static_cast<th20::source::effects::Controller*>(context.objects_04[7]),handle,env);}
    } spawn_host(callback_env);
    struct SpriteHitSnapshot {
        sp::PooledAnimation slots[2];sp::Controller::AnimationList lists[3];sp::AnimationLink free;std::uint32_t generation;
        explicit SpriteHitSnapshot(const sp::Controller& c):free(c.free_sentinel),generation(c.field_7d40e88){std::memcpy(slots,c.pool,sizeof(slots));std::memcpy(lists,c.lists,sizeof(lists));}
        void restore(sp::Controller& c)const{std::memcpy(c.pool,slots,sizeof(slots));std::memcpy(c.lists,lists,sizeof(lists));c.free_sentinel=free;c.field_7d40e88=generation;}
    };
    const std::array<std::pair<unsigned,std::uint32_t>,4> cases{{{1,0x5032d0},{2,0x503450},{7,0x503ee0},{8,0x503fc0}}};
    for(unsigned n=0;n<1400;++n)for(const auto [index,address]:cases){firing_case=n;firing_phase="Shot hit effect callbacks";
        for(auto& list:sprites->lists){list={};list.tail=&list.sentinel;}sprites->free_sentinel={};sprites->field_7d40e88=2;sprites->field_6c4=0;
        for(unsigned i=0;i<2;++i){auto& pool=sprites->pool[i];pool={};sp::construct_pooled_animation(pool);pool.index=i;pool.free_link.value=&pool.animation;}
        sprites->pool[0].active=1;sprites->pool[0].animation.handle=0x10000u;sq::insert_after(reinterpret_cast<sq::Link&>(sprites->free_sentinel),reinterpret_cast<sq::Link&>(sprites->pool[1].free_link));
        std::memset(effect_bytes.data(),0,effect_bytes.size());put(effect_bytes.data(),0x10,&effect_file);native_session.contexts[0].objects_04[7]=effect_bytes.data();
        pl::construct_player_fields(owner);owner.shot_data=data.data();owner.context=&native_session.contexts[0];auto& shot=owner.shots.pool[0];pl::select_context(shot,0,native_session);row={};row.damage=73;
        shot.fields_98[3]=rng();shot.fields_98[4]=n%3;shot.fields_98[5]=rng();shot.handle_18=0x10000u;shot.fields_b8[9]=1;shot.byte_114=n%2;shot.motion.position={finite(),finite(),0};shot.motion.angle_1c=finite();shot.motion.field_18=finite();shot.field_10c=reinterpret_cast<std::uint32_t>(&effect_file);
        shot.timer_1c={};shot.timer_1c.current=n%120;shot.timer_1c.previous=n%3?shot.timer_1c.current-1:shot.timer_1c.current;
        std::memset(damage_bytes.data(),0,damage_bytes.size());sq::initialize_list(damage_owner.active);sq::initialize_list(damage_owner.free);original<void>(0x4bffb0,&damage_owner.pool[0]);sq::initialize_link(damage_owner.pool[0].link,reinterpret_cast<sq::Node*>(&damage_owner.pool[0]));sq::append(damage_owner.active,damage_owner.pool[0].link);damage_owner.pool[0].handle=1;damage_owner.pool[0].context=&native_session.contexts[0];damage_owner.pool[0].damage=int(rng());damage_owner.pool[0].size={finite(),std::fabs(finite())+1};
        const sp::Vec3 position{finite(),finite(),0};const sp::Vec2 size{finite(),finite()};const auto* size_pointer=n%2?&size:nullptr;const float angle=finite(),radius=std::fabs(finite())+1;
        std::array<std::uint8_t,56> random_bytes{};for(unsigned stream=0;stream<2;++stream){th20::source::state::Random random(stream);random.state=rng()%0x7ffffffeu+1;random.upper=random.modulus=0x7fffffffu;std::memcpy(random_bytes.data()+stream*28,&random,28);}std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),random_bytes.data(),56);
        const auto before=owner_storage;const auto before_damage=damage_bytes;const auto before_effect=effect_bytes;const SpriteHitSnapshot before_sprites(*sprites);const auto file_generation=effect_file.fields_5c[3];
        FloatingEnvironment::prepare();const int expected_return=original<int>(address,&shot,&position,size_pointer,angle,radius);const auto expected=owner_storage;const auto expected_damage=damage_bytes;const auto expected_effect=effect_bytes;const SpriteHitSnapshot expected_sprites(*sprites);std::array<std::uint8_t,56> expected_random;std::memcpy(expected_random.data(),reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),56);
        owner_storage=before;damage_bytes=before_damage;effect_bytes=before_effect;before_sprites.restore(*sprites);effect_file.fields_5c[3]=file_generation;std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),random_bytes.data(),56);
        FloatingEnvironment::prepare();const int actual_return=pl::hit_shot_callback(shot,index,position,size_pointer,angle,radius,spawn_host);
        if(std::memcmp(sprites->pool,expected_sprites.slots,sizeof(expected_sprites.slots))&&failed<12){std::size_t offset=0;const auto* expected_bytes=reinterpret_cast<const std::uint8_t*>(expected_sprites.slots);const auto* actual_bytes=reinterpret_cast<const std::uint8_t*>(sprites->pool);while(expected_bytes[offset]==actual_bytes[offset])++offset;std::cerr<<"Shot hit effect "<<index<<" case "<<n<<" ANM byte "<<std::hex<<offset<<std::dec<<'\n';}
        check("Shot_hit_effect_all_player_bytes",owner_storage==expected);check("Shot_hit_effect_return",actual_return==expected_return);check("Shot_hit_effect_damage_owner",damage_bytes==expected_damage);check("Shot_hit_effect_two_ANM_slots",std::memcmp(sprites->pool,expected_sprites.slots,sizeof(expected_sprites.slots))==0);check("Shot_hit_effect_registry",effect_bytes==expected_effect);check("Shot_hit_effect_two_RNG_streams",std::memcmp(expected_random.data(),reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),56)==0);
    }
    put(reinterpret_cast<void*>(mapped_image_base),0x1c0028,sprite_bytes.data());VirtualFree(sprites,0,MEM_RELEASE);
}
