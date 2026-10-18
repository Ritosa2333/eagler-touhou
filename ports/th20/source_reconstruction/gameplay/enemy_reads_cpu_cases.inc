{
    static std::uint32_t diagnostic_code=0;static unsigned diagnostic_kind=0,diagnostic_case=0;static const char* diagnostic_phase="setup";
    const auto observer=AddVectoredExceptionHandler(1,[](EXCEPTION_POINTERS* info)->LONG{std::cerr<<"Enemy read exception "<<std::hex<<info->ExceptionRecord->ExceptionCode<<" at "<<info->ContextRecord->Eip<<" mapped "<<mapped_image_base<<" code "<<diagnostic_code<<" access "<<info->ExceptionRecord->ExceptionInformation[1]<<std::dec<<" kind "<<diagnostic_kind<<" case "<<diagnostic_case<<" phase "<<diagnostic_phase<<'\n';return EXCEPTION_CONTINUE_SEARCH;});
    auto& global=*reinterpret_cast<th20::source::game_session::Session*>(mapped_image_base+0x1ba568);
    auto& cpu_random=*reinterpret_cast<th20::source::state::Random*>(mapped_image_base+0x1ba4a8);
    th20::source::state::Random source_random(0);
    std::array<std::array<std::uint8_t,0x134>,2> controller_bytes{};
    std::array<std::array<std::uint8_t,0x620>,2> players{};
    std::array<std::array<std::uint8_t,0x80>,2> bullets{},secondary{};
    gp::Enemy subjects[3];
    for(unsigned i=0;i<2;++i){auto& owner=*reinterpret_cast<gp::EnemyController*>(controller_bytes[i].data());scheduler::initialize_list(owner.enemies);owner.player_index=i;owner.field_124=20;
        global.contexts[i].objects_04[0]=players[i].data();global.contexts[i].objects_04[1]=&owner;global.contexts[i].objects_04[3]=secondary[i].data();global.contexts[i].primary_owner=reinterpret_cast<rt::CallbackOwner*>(bullets[i].data());global.contexts[i].current_player=&global.player_table.players[i];}
    auto& first=*reinterpret_cast<gp::EnemyController*>(controller_bytes[0].data());auto& second=*reinterpret_cast<gp::EnemyController*>(controller_bytes[1].data());
    for(unsigned i=0;i<3;++i){auto& entity=subjects[i];entity.select_context(i%2,global);entity.state.entity=&entity;entity.state.movements.resize(2);entity.state.animations.resize(1);entity.state.identifier=i+1;scheduler::append(first.enemies,entity.controller_link);}
    std::array<std::uint8_t,0x110> game{};*reinterpret_cast<void**>(mapped_image_base+0x1ba828)=game.data();
    std::int32_t restart=0,replay=-1;std::uint32_t new_game=0;
    gp::EnemyReadEnvironment read_host{global,source_random,restart,new_game,replay,read_vm_animation};
    spawn_frame=&frame;frame.pool=reinterpret_cast<th20::source::sprite::PooledAnimation*>(animation_pool.data()+0x710);frame.pool[1].active=1;frame.pool[1].animation.handle=0x10001;
    const auto real=[&](){return float(std::int32_t(rng()%100000)-50000)/32.f;};
    for(unsigned n=0;n<350;++n){
        diagnostic_case=n;
        for(auto& entity:subjects){
            for(auto& value:entity.state.fields_78)value=rng();
            for(auto* motion:{&entity.state.motion_c8,&entity.state.motion_110})for(auto& word:motion->words)word=th20::source::ecl::float_bits(real());
            for(auto& movement:entity.state.movements)for(auto& word:movement.motion.words)word=th20::source::ecl::float_bits(real());
            entity.state.field_04=rng();entity.state.fields_2c8[0]=rng();entity.state.fields_2c8[1]=rng();entity.state.fields_2c8[2]=rng();entity.state.timer_288.current=int(n%7)-3;entity.state.timer_a8.current=int(rng());entity.state.timer_a8.current_f=real();
            entity.state.animations[0].handle=0x10001;entity.state.auxiliary_18c.words[0]=rng();*reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(&entity)+0xd8)=rng();
        }
        for(auto& storage:controller_bytes)for(unsigned offset=0x10;offset<0x54;offset+=4)*reinterpret_cast<std::uint32_t*>(storage.data()+offset)=rng();
        for(auto* owner:{&first,&second}){std::fill(std::begin(owner->data.handles_44),std::end(owner->data.handles_44),0u);owner->data.handles_44[n%16]=3;owner->data.handles_44[0]=n%3?3u:0u;owner->field_e0=rng();owner->field_124=rng();}
        for(unsigned i=0;i<2;++i){*reinterpret_cast<float*>(players[i].data()+0x614)=real();*reinterpret_cast<float*>(players[i].data()+0x618)=real();*reinterpret_cast<std::uint32_t*>(bullets[i].data()+0x44)=rng();*reinterpret_cast<std::uint32_t*>(secondary[i].data()+0x78)=rng();}
        auto& entity=subjects[n%2];
        for(std::uint32_t code=0xffffd8f0u;code<=0xffffd96eu;++code)for(unsigned floating=0;floating<2;++floating){
            diagnostic_code=code;diagnostic_kind=floating;diagnostic_phase="CPU";
            //Original integer915/916 dereferences null selected handles; float has explicit fallbacks.
            if(!floating&&(code==0xffffd915u||code==0xffffd916u)&&!reinterpret_cast<gp::EnemyController*>(entity.context->objects_04[1])->data.handles_44[0])continue;
            for(auto& word:reinterpret_cast<std::uint32_t(&)[sizeof(global.player_table)/4]>(global.player_table))word=rng();
            const auto table_before=global.player_table;const std::uint32_t seed=rng();source_random.state=cpu_random.state=seed;source_random.modulus=cpu_random.modulus=0x7fffffffu;source_random.last=cpu_random.last=0;
            restart=int(n%4);std::memcpy(game.data()+0x108,&restart,4);
            new_game=n%3;*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c5858)=new_game;
            replay=int(n)-10;*reinterpret_cast<std::int32_t*>(mapped_image_base+0x1afcfc)=replay;
            gp::previous_enemy_generation=rng();*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c49ec)=gp::previous_enemy_generation;
            for(unsigned i=0;i<4;++i)*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c49d8+i*4)=gp::enemy_script_globals[i]=rng();
            FloatingEnvironment::prepare();const std::uint32_t expected=floating?th20::source::ecl::float_bits(original<float>(0x4995d0,&entity,int(code))):original<std::uint32_t>(0x49abc0,&entity,int(code));const auto table_after=global.player_table;
            global.player_table=table_before;
            diagnostic_phase="source";
            FloatingEnvironment::prepare();const std::uint32_t actual=floating?th20::source::ecl::float_bits(gp::read_enemy_float(entity,int(code),read_host)):std::uint32_t(gp::read_enemy_integer(entity,int(code),read_host));
            if(expected!=actual&&failed<20)std::cerr<<"ECL read "<<std::hex<<code<<" expected "<<expected<<" actual "<<actual<<std::dec<<" float "<<floating<<'\n';
            check(floating?"EnemyVM_native_float_variable_getter_all_cases":"EnemyVM_native_integer_variable_getter_all_cases",expected==actual&&std::memcmp(&table_after,&global.player_table,sizeof table_after)==0&&source_random.state==cpu_random.state&&source_random.last==cpu_random.last);
        }
    }
    for(auto& entity:subjects)entity.state.animations[0].handle=0;
    RemoveVectoredExceptionHandler(observer);
}
