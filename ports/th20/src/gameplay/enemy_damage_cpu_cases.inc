// Original bodies remain unpatched. Fixtures replace only separately named
// source dependencies; CPU-domain input excludes unimplemented child calls.
{
    using Session=th20::source::game_session::Session;
    using Player=th20::source::game_session::Player;
    EnemyDamageFixture host;
    host.global=reinterpret_cast<Session*>(mapped_image_base+0x1ba568);
    *host.global=Session{};
    *reinterpret_cast<void**>(mapped_image_base+0x1c06a4)=host.hud.data();
    *reinterpret_cast<float**>(mapped_image_base+0x1aefe0)=&host.scale;
    frame.pool=reinterpret_cast<th20::source::sprite::PooledAnimation*>(animation_pool.data()+0x710);
    host.lookup=[&](std::uint32_t handle){return frame.animation(handle);};
    host.timeout=[](void* secondary){if(*reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(secondary)+0x78)&1u)throw std::logic_error("CPU phase fixture permits only original487bb0 inactive early return");};
    for(unsigned n=0;n<6000;++n){
        Player cpu_player{},source_player{};
        for(auto& byte:reinterpret_cast<std::uint8_t(&)[sizeof(cpu_player)]>(cpu_player))byte=static_cast<std::uint8_t>(core_rng());source_player=cpu_player;
        const auto bonus=original<int>(0x4aaf50,&cpu_player);
        check("Enemy_damage_bonus_clamp_all_player_bytes",bonus==gp::enemy_damage_bonus(source_player)&&std::memcmp(&cpu_player,&source_player,sizeof(cpu_player))==0);
        const int delta=static_cast<int>(core_rng());original<void>(0x497c60,&cpu_player,delta);gp::add_phase_reward(source_player,delta);
        check("Enemy_phase_reward_wrap_and_clamp_all_player_bytes",std::memcmp(&cpu_player,&source_player,sizeof(cpu_player))==0);
        host.global->player_table.fields_1ec[4]=core_rng();const Session before=*host.global;
        const auto stage=original<int>(0x499330,host.global);const Session expected=*host.global;*host.global=before;
        check("Enemy_phase_stage_mutating_clamp_all_session_bytes",stage==gp::enemy_phase_stage(*host.global)&&std::memcmp(host.global,&expected,sizeof(Session))==0);
    }
    // This oracle domain accesses only the known scalar controller prefix.
    // It does not construct or retire the unrelated device-owning controller.
    alignas(gp::EnemyController)std::array<std::uint8_t,sizeof(gp::EnemyController)> owner_storage{};
    auto& owner=*reinterpret_cast<gp::EnemyController*>(owner_storage.data());
    alignas(gp::EnemyState)std::array<std::uint8_t,0x428> entity{};
    auto& state=*new(entity.data()+0x88)gp::EnemyState;
    std::array<std::uint8_t,0x2300> player_entity{};
    std::array<std::uint8_t,0xc4> secondary{};
    std::array<std::uint8_t,0x3c> bomb{};
    Player player{};auto& context=host.global->contexts[0];
    context.current_player=&player;context.objects_04[0]=player_entity.data();context.objects_04[1]=&owner;context.objects_04[3]=secondary.data();context.objects_04[5]=bomb.data();
    state.entity=entity.data();state.fields_2c8[9]=reinterpret_cast<std::uint32_t>(&context);
    *reinterpret_cast<void**>(entity.data()+0x424)=&context;
    state.animations.resize(1);
    for(unsigned n=0;n<3000;++n){
        state.auxiliary.resize(n%6);
        for(auto& record:state.auxiliary){for(auto& word:record.values)word=core_rng();record.values[0]=n%3?core_rng()%1000:0xffffffffu;record.values[1]=core_rng()%1000;}
        state.auxiliary_18c.words[0]=core_rng()%1500;state.auxiliary_18c.words[2]=core_rng();state.auxiliary_18c.words[4]=core_rng();
        state.fields_250[12]=n%9;state.fields_250[13]=core_rng();state.fields_2c8[1]=core_rng();host.global->player_table.fields_1ec[4]=n%9;
        th20::recovered::timer_set(state.timer_a8,static_cast<int>(n%1200)-100);
        *reinterpret_cast<unsigned*>(secondary.data()+0x78)=n%3==0?0:n%3==1?8:9;
        for(auto& value:player.fields_b4)value=core_rng();
        for(unsigned kind=0;kind<2;++kind){
            const auto before=entity;const auto records_before=state.auxiliary;const auto player_before=player;const auto secondary_before=secondary;const auto global_before=*host.global;const auto hud_before=host.hud;const auto owner_before=owner.data;
            const auto* result=original<const char*>(kind?0x4aa400:0x4aa250,entity.data());
            const auto expected=entity;const auto records_expected=state.auxiliary;const auto player_expected=player;const auto secondary_expected=secondary;const auto global_expected=*host.global;const auto hud_expected=host.hud;const auto owner_expected=owner.data;
            std::memcpy(entity.data(),before.data(),entity.size());std::copy(records_before.begin(),records_before.end(),state.auxiliary.begin());player=player_before;secondary=secondary_before;*host.global=global_before;host.hud=hud_before;owner.data=owner_before;
            const auto* actual=kind?gp::enemy_time_phase(state,host):gp::enemy_life_phase(state,host);
            check("Enemy_phase_selected_script_pointer",result==actual);
            check("Enemy_phase_all_entity_state_bytes",entity==expected);
            check("Enemy_phase_threshold_and_136byte_records",state.auxiliary.size()==records_expected.size()&&std::memcmp(state.auxiliary.data(),records_expected.data(),state.auxiliary.size()*sizeof(gp::EnemyAuxiliaryRecord))==0);
            check("Enemy_phase_shared_session_and_player_reward",std::memcmp(&player,&player_expected,sizeof(player))==0&&std::memcmp(host.global,&global_expected,sizeof(Session))==0);
            check("Enemy_phase_HUD_secondary_and_controller_side_effects",host.hud==hud_expected&&secondary==secondary_expected&&std::memcmp(&owner.data,&owner_expected,sizeof(owner.data))==0);
        }
    }
    state.auxiliary.clear();
    for(unsigned n=0;n<6000;++n){
        host.scale=static_cast<float>(n%7)*.25f;
        std::memset(frame.pool,0,8*sizeof(*frame.pool));auto& animation=frame.pool[1];animation.active=1;animation.animation.handle=0x10001;
        for(auto& byte:reinterpret_cast<std::uint8_t(&)[sizeof(animation.animation)]>(animation.animation))byte=static_cast<std::uint8_t>(core_rng());animation.animation.handle=0x10001;
        state.animations[0].handle=n%11==0?0x20001u:0x10001u;
        state.field_04=0;state.fields_2c8[0]=core_rng()&~(0x800u|0x200u);state.fields_2c8[1]=core_rng();state.fields_2c8[2]=core_rng();
        state.fields_2c8[5]=n%3?reinterpret_cast<std::uint32_t>(&damage_callback):0;
        state.fields_2c8[7]=reinterpret_cast<std::uint32_t>(&collision_callback);
        state.fields_250[3]=n%9+1;state.fields_250[6]=n%4;state.fields_250[13]=0;
        if(n%9==0){state.fields_250[3]=0;state.fields_2c8[0]|=0x20u;} //real periodic flash branch without sound generation
        state.fields_1c[(0x54-0x1c)/4]=0xffffffff;state.fields_1c[(0x48-0x1c)/4]=n%100;const float scale=.5f;std::memcpy(&state.fields_1c[(0x4c-0x1c)/4],&scale,4);
        state.bounds_5c={0,0};state.auxiliary_18c={{100000,0,90000,700000,0,core_rng(),n%2}};
        th20::recovered::timer_set(state.timer_a8,static_cast<int>(n)-12);th20::recovered::timer_set(state.timer_288,static_cast<int>(n%7)-3);th20::recovered::timer_set(state.timer_298,static_cast<int>(n%5)-2);th20::recovered::timer_set(state.timer_2a8,static_cast<int>(n%5)-2);
        *reinterpret_cast<unsigned*>(bomb.data()+0x18)=n%3;*reinterpret_cast<unsigned*>(secondary.data()+0x78)=core_rng();
        *reinterpret_cast<int*>(player_entity.data()+0x10)=n%4;*reinterpret_cast<unsigned*>(host.hud.data()+0x1bc)=n%7==0?1:0;
        th20::recovered::timer_set(*reinterpret_cast<th20::recovered::Timer*>(player_entity.data()+0x2244),static_cast<int>(n%9));*reinterpret_cast<int*>(player_entity.data()+0x227c)=static_cast<int>(n%5)-1;
        owner.field_c8=n%2;owner.data.field_9c=core_rng();owner.data.field_a0=core_rng();host.events.clear();host.sounds.clear();
        const auto before=entity;const auto links_before=state.animations;const auto pool_before=animation_pool;const auto player_before=player_entity;const auto data_before=owner.data;
        const auto result=original<int>(0x4a5df0,&state);
        const auto expected=entity;const auto links_expected=state.animations;const auto pool_expected=animation_pool;const auto player_expected=player_entity;const auto data_expected=owner.data;
        std::memcpy(entity.data(),before.data(),entity.size());std::copy(links_before.begin(),links_before.end(),state.animations.begin());std::copy(pool_before.begin(),pool_before.end(),animation_pool.begin());player_entity=player_before;owner.data=data_before;
        const auto actual=gp::update_enemy_damage(state,host);
        check("Enemy_damage_full_body_return",result==actual);
        if(failed<20&&entity!=expected)for(unsigned k=0;k<entity.size();++k)if(entity[k]!=expected[k]){std::cerr<<"damage n="<<n<<" byte="<<std::hex<<k<<" cpu="<<unsigned(expected[k])<<" source="<<unsigned(entity[k])<<std::dec<<'\n';break;}
        check("Enemy_damage_full_body_all_entity_state_bytes",entity==expected);
        check("Enemy_damage_health_feedback_shared_player_all_bytes",player_entity==player_expected);
        check("Enemy_damage_accounting_controller_all_bytes",std::memcmp(&owner.data,&data_expected,sizeof(owner.data))==0);
        check("Enemy_damage_flash_all_pool_bytes_and_handles",animation_pool==pool_expected&&std::memcmp(state.animations.data(),links_expected.data(),sizeof(gp::EnemyAnimationLink))==0);
        check("Enemy_damage_CPU_domain_has_no_unrecovered_child_calls",host.events.empty()&&host.sounds.empty());
    }
    state.~EnemyState();frame.pool=nullptr;
    *reinterpret_cast<float**>(mapped_image_base+0x1aefe0)=&frame.scale;
}
