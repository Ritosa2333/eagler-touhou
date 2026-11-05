{
    spawn_frame=&frame;
    for(unsigned n=0;n<5000;++n){
        gp::SpawnParameters cpu,source;for(auto& byte:reinterpret_cast<std::uint8_t(&)[sizeof(cpu)]>(cpu))byte=std::uint8_t(rng());source=cpu;
        original<void>(0x47bb30,&cpu);gp::construct_spawn_parameters(source);check("EnemySpawn_ctor_all84bytes",std::memcmp(&cpu,&source,84)==0);
        alignas(gp::EnemyRuntime)std::array<std::uint8_t,sizeof(gp::EnemyRuntime)> cpu_runtime,source_runtime;
        for(auto& byte:cpu_runtime)byte=std::uint8_t(rng());source_runtime=cpu_runtime;
        original<void>(0x4a3580,cpu_runtime.data());auto* source_task=new(source_runtime.data())gp::EnemyRuntime;
        check("EnemyRuntime_ctor_all72bytes_actual_PMR_and_padding",cpu_runtime==source_runtime);
        original<void>(0x48b8d0,cpu_runtime.data());source_task->~EnemyRuntime();
    }
    auto& cpu_session=*reinterpret_cast<th20::source::game_session::Session*>(mapped_image_base+0x1ba568);
    th20::source::game_session::Session session;
    gp::ScriptLoader scripts(environment);auto script_document=document({"init","phase","phase"});scripts.append(script_document);
    std::array<std::uint8_t,0x134> controller_storage{};auto& owner=*reinterpret_cast<gp::EnemyController*>(controller_storage.data());owner.loader=&scripts;scheduler::initialize_list(owner.enemies);
    cpu_session.contexts[0].objects_04[1]=cpu_session.contexts[1].objects_04[1]=&owner;
    for(unsigned n=0;n<800;++n){
        alignas(gp::Enemy)std::array<std::uint8_t,sizeof(gp::Enemy)> storage;for(auto& byte:storage)byte=std::uint8_t(rng());const auto before=storage;
        original<void>(0x4a33c0,storage.data());const auto expected=storage;
        //Default construction allocates no vector/function payloads. Dispose
        //the members directly so teardown is not invoked before context binding.
        original<void>(0x4a3ac0,storage.data()+0x88);original<void>(0x48b8d0,storage.data()+0x10);
        storage=before;auto* source=new(storage.data())gp::Enemy;auto normalized=storage;std::memcpy(normalized.data(),expected.data(),4);
        if(normalized!=expected&&failed<20){unsigned offset=0;while(normalized[offset]==expected[offset])++offset;std::cerr<<"Enemy ctor byte +"<<std::hex<<offset<<std::dec<<'\n';}
        check("Enemy_ctor_all1064bytes_vptr_normalized_only",normalized==expected);
        source->state.~EnemyState();source->main.~EnemyRuntime();source->callback.~function();
    }
    for(unsigned n=0;n<500;++n){
        alignas(gp::Enemy)std::array<std::uint8_t,sizeof(gp::Enemy)> storage{};owner.player_index=n%2;owner.field_124=10;
        original<void>(0x4a33c0,storage.data());const auto prior=rng();*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c49f0)=prior;
        const char* name=n%3==0?"missing":n%3==1?"init":"phase";original<void>(0x4a73f0,storage.data(),int(n%2),name);
        auto expected=storage;std::array<std::vector<std::uint8_t>,3> payloads;
        constexpr std::array<unsigned,3> vectors{0x1c,0x94,0x1e0};
        for(unsigned i=0;i<vectors.size();++i){auto* vector=reinterpret_cast<std::uintptr_t*>(storage.data()+vectors[i]);payloads[i].assign(reinterpret_cast<std::uint8_t*>(vector[1]),reinterpret_cast<std::uint8_t*>(vector[2]));}
        const auto expected_generation=*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c49f0);original<void>(0x4a3b10,storage.data());const auto expected_owner_count=owner.field_124;
        storage.fill(0);auto* source=new(storage.data())gp::Enemy;gp::current_enemy_generation=prior;source->initialize(n%2,name,cpu_session);
        auto normalized=expected;for(unsigned i=0;i<vectors.size();++i){auto* cpu_vector=reinterpret_cast<std::uintptr_t*>(expected.data()+vectors[i]);auto* source_vector=reinterpret_cast<std::uintptr_t*>(storage.data()+vectors[i]);
            check("Enemy_initialize_all_stack_animation_motion_payloads",payloads[i].size()==source_vector[2]-source_vector[1]&&cpu_vector[3]-cpu_vector[1]==source_vector[3]-source_vector[1]&&std::memcmp(payloads[i].data(),reinterpret_cast<void*>(source_vector[1]),payloads[i].size())==0);
            std::memcpy(normalized.data()+vectors[i]+4,source_vector+1,12);
        }
        std::memcpy(normalized.data(),storage.data(),4);
        if(normalized!=storage&&failed<20){unsigned offset=0;while(normalized[offset]==storage[offset])++offset;std::cerr<<"Enemy init byte +"<<std::hex<<offset<<std::dec<<'\n';}
        check("Enemy_initialize_all1064bytes_owning_pointers_normalized",normalized==storage);
        check("Enemy_initialize_identifier_and_generation_order",source->state.identifier==prior&&gp::current_enemy_generation==expected_generation);
        owner.field_124=10;source->~Enemy();check("Enemy_destructor_original_controller_count_decrement",owner.field_124==expected_owner_count);
    }
    for(unsigned n=0;n<1600;++n){
        alignas(gp::EnemyState)std::array<std::uint8_t,0x428> cpu{},source{};
        original<void>(0x4a3060,cpu.data()+0x88);auto* state=new(source.data()+0x88)gp::EnemyState;
        const auto word=[&](unsigned offset,std::uint32_t value){std::memcpy(cpu.data()+offset,&value,4);std::memcpy(source.data()+offset,&value,4);};
        gp::SpawnParameters input;for(auto& byte:reinterpret_cast<std::uint8_t(&)[sizeof(input)]>(input))byte=std::uint8_t(rng());
        word(0x88,rng());word(0x354,rng()|4u);word(0x2dc,n%3?0:rng());word(0xa8,n%2?2:3);word(0xac,n%112);word(0x2e0,rng());
        const auto difficulty=std::int32_t(n%11)-4;session.player_table.field_1e0=cpu_session.player_table.field_1e0=difficulty;
        const std::int32_t phase=std::int32_t(n)-100;std::memcpy(reinterpret_cast<std::uint8_t*>(&session.player_table)+0x1fc,&phase,4);std::memcpy(reinterpret_cast<std::uint8_t*>(&cpu_session.player_table)+0x1fc,&phase,4);
        const int cpu_result=original<int>(0x4a89c0,cpu.data(),&input);const int source_result=gp::apply_enemy_spawn(source.data(),input,session,frame);
        auto normalized=cpu;auto* cpu_vector=reinterpret_cast<std::uintptr_t*>(cpu.data()+0x1e0);const auto source_vector=reinterpret_cast<std::uintptr_t*>(source.data()+0x1e0);
        const auto cpu_length=cpu_vector[2]-cpu_vector[1],source_length=source_vector[2]-source_vector[1];
        check("EnemySpawn_first_motion_vector_size_capacity_payload",cpu_length==source_length&&cpu_vector[3]-cpu_vector[1]==source_vector[3]-source_vector[1]&&std::memcmp(reinterpret_cast<void*>(cpu_vector[1]),reinterpret_cast<void*>(source_vector[1]),cpu_length)==0);
        std::memcpy(normalized.data()+0x1e4,source_vector+1,12);
        if(normalized!=source&&failed<20){unsigned offset=0;while(normalized[offset]==source[offset])++offset;std::cerr<<"Enemy spawn byte +"<<std::hex<<offset<<std::dec<<'\n';}
        check("EnemySpawn_apply_all1064bytes_after_vector_pointer_normalization",normalized==source&&cpu_result==source_result);
        check("EnemySpawn_global_stage_clamp_side_effect",std::memcmp(reinterpret_cast<std::uint8_t*>(&cpu_session.player_table)+0x1fc,reinterpret_cast<std::uint8_t*>(&session.player_table)+0x1fc,4)==0);
        original<void>(0x4a3ac0,cpu.data()+0x88);state->~EnemyState();
    }
}
