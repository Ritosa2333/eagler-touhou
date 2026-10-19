{
    namespace rt=th20::source::runtime;
    rt::MemoryResource resource;auto* previous_default=rt::set_default_resource(&resource);
    auto*& original_resource=*reinterpret_cast<std::pmr::memory_resource**>(mapped_image_base+0x1e4d28);auto* saved_original_resource=original_resource;original_resource=&resource;
    // The original and source allocator contracts were independently checked in
    // runtime_core/cpu_compare.cpp; this binds the same real allocator to both.
    for(unsigned test=0;test<256;++test){diagnostic_case=test;diagnostic_phase="ETEX Command complete constructor";
        std::array<std::uint8_t,sizeof(bu::Command)> bytes;bytes.fill(static_cast<std::uint8_t>(test));cpu<void>(0x47bca0,bytes.data());const auto expected=bytes;bytes.fill(static_cast<std::uint8_t>(test));new(bytes.data())bu::Command;check("bullet_command_ctor",expected.data(),bytes.data(),bytes.size());
    }
    for(unsigned test=0;test<64;++test){diagnostic_case=test;diagnostic_phase="ShotMetadata real allocator constructor";
        alignas(bu::ShotMetadata) std::array<std::uint8_t,sizeof(bu::ShotMetadata)> bytes;bytes.fill(static_cast<std::uint8_t>(test*4));cpu<void>(0x47bb90,bytes.data());auto expected=bytes;auto* original=reinterpret_cast<bu::ShotMetadata*>(bytes.data());const auto original_address=reinterpret_cast<std::uint32_t>(original->commands.data());const auto expected_commands=std::vector<bu::Command>(original->commands.begin(),original->commands.end());cpu<void>(0x47c450,bytes.data());const auto expected_destroyed=bytes;
        bytes.fill(static_cast<std::uint8_t>(test*4));auto* value=new(bytes.data())bu::ShotMetadata;const auto delta=reinterpret_cast<std::uint32_t>(value->commands.data())-original_address;
        for(unsigned offset:{8u,12u,16u}){std::uint32_t pointer;std::memcpy(&pointer,expected.data()+offset,4);pointer+=delta;std::memcpy(expected.data()+offset,&pointer,4);}
        check("bullet_metadata_ctor",expected.data(),bytes.data(),bytes.size());const auto expected_size=expected_commands.size(),actual_size=value->commands.size();check("bullet_metadata_command_count",&expected_size,&actual_size,sizeof(actual_size));if(expected_size==actual_size)check("bullet_metadata_commands",expected_commands.data(),value->commands.data(),actual_size*sizeof(bu::Command));
        value->~ShotMetadata();check("bullet_metadata_dtor",expected_destroyed.data(),bytes.data(),bytes.size());
    }
    for(unsigned test=0;test<32;++test){diagnostic_case=test;diagnostic_phase="Original 47b290 shared metadata factory size and capacity";
        std::uint32_t original_pair[2]{};std::uint8_t allocator_tag=0;using Factory=void(__cdecl*)(void*,void*);reinterpret_cast<Factory>(mapped_image_base+0x7b290)(original_pair,&allocator_tag);const auto* expected=reinterpret_cast<const bu::ShotMetadata*>(original_pair[0]);bu::ShotMetadata actual;
        const auto expected_size=expected->commands.size(),actual_size=actual.commands.size(),expected_capacity=expected->commands.capacity(),actual_capacity=actual.commands.capacity();const std::size_t two=2;check("bullet_metadata_factory_expected_two",&two,&expected_size,sizeof(two));check("bullet_metadata_factory_size",&expected_size,&actual_size,sizeof(actual_size));check("bullet_metadata_factory_capacity",&expected_capacity,&actual_capacity,sizeof(actual_capacity));if(expected_size==actual_size)check("bullet_metadata_factory_commands",expected->commands.data(),actual.commands.data(),actual_size*sizeof(bu::Command));cpu<void>(0x47c300,original_pair);
    }
    using Initializer=void(__cdecl*)();reinterpret_cast<Initializer>(mapped_image_base+0x1280)();check("bullet_style_table",reinterpret_cast<void*>(mapped_image_base+0x1c06a8),bu::styles,sizeof(bu::styles));
    for(unsigned type=0;type<50;++type){using Radius=float(__cdecl*)(int);const auto expected=reinterpret_cast<Radius>(mapped_image_base+0x85700)(type);const auto actual=bu::bullet_radius(type);check("bullet_style_radius",&expected,&actual,4);}
    std::array<std::uint8_t,0x2200> player{};gs::context(0).objects_04[0]=player.data();gs::context(1).objects_04[0]=player.data();
    auto metadata=std::make_shared<bu::ShotMetadata>();metadata->commands.resize(24);
    auto float_word=[](float value){std::uint32_t word;std::memcpy(&word,&value,4);return word;};
    const unsigned unsupported[]{9,13,24,27};
    for(unsigned type=0;type<34;++type){if(std::find(std::begin(unsupported),std::end(unsupported),type)!=std::end(unsupported))continue;
        for(unsigned test=0;test<256;++test){diagnostic_case=test;diagnostic_phase="ETEX single command complete state";reset(test,1);auto& b=owner.pool[0];b.metadata=metadata;b.flags=0x200u;b.field_38=test%2;b.field_3c=test%4;b.field_90=test%7==0?(1ull<<45):0;b.field_40=0xffffffffu;b.field_4c=static_cast<short>(test%50);b.cancel_script=-1;b.field_20=finite()*8;b.angle=finite()*12;
            for(auto& cmd:b.commands){cmd.timer={7,8,8.25f,rng()};cmd.field_10=finite();cmd.field_14=finite();cmd.vector_18={finite(),finite(),finite()};cmd.vector_24={finite(),finite(),finite()};cmd.field_30=rng();cmd.field_34=rng();cmd.field_38=rng();cmd.field_3c=rng();}
            auto& interpolation=b.interpolation_420;interpolation.current={finite(),finite(),finite()};b.interpolation_474.current=finite();
            for(auto& cmd:metadata->commands)cmd={};auto& command=metadata->commands[b.field_38];for(unsigned i=0;i<4;++i)command.words[i]=float_word(finite()*4);
            command.words[4]=1+test%12;command.words[5]=test%10;command.words[6]=test%8;command.words[7]=rng();command.words[8]=type;command.words[9]=test%3?1:0;
            if(type==11)command.words[4]=test%90;if(type==16){command.words[4]=b.field_38+1;command.words[5]=test%4;}if(type==17)command.words[5]=(test%10)|(test%2?0x100u:0);if(type==29)command.words[0]=float_word(test%2?finite()*8:4.f);if(type==25)command.words[4]=test%6;
            const s::Vec3 player_position{finite()*192,finite()*448,finite()};std::memcpy(player.data()+0x614,&player_position,12);
            state::Random initial_rng(0);initial_rng.state=1+rng()%0x7ffffffe;initial_rng.upper=initial_rng.modulus=0x7fffffff;state::random_streams[0]=initial_rng;std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),&initial_rng,sizeof(initial_rng));sound_reset();
            const auto initial=b;const Snapshot initial_pool(c);const auto initial_commands=metadata->commands;const auto initial_sound=std::vector<std::uint8_t>(reinterpret_cast<std::uint8_t*>(&pe_state::thread_registry),reinterpret_cast<std::uint8_t*>(&pe_state::thread_registry)+0x57e8);
            FloatingEnvironment::prepare();cpu<void>(0x47dcf0,&b);const auto expected=b;const Snapshot expected_pool(c);const auto expected_commands=metadata->commands;const auto expected_rng=*reinterpret_cast<state::Random*>(mapped_image_base+0x1ba4a8);const auto expected_sound=std::vector<std::uint8_t>(reinterpret_cast<std::uint8_t*>(mapped_image_base+0x1ba830),reinterpret_cast<std::uint8_t*>(mapped_image_base+0x1ba830)+0x57e8);
            b=initial;initial_pool.restore(c);metadata->commands=initial_commands;state::random_streams[0]=initial_rng;std::memcpy(&pe_state::thread_registry,initial_sound.data(),initial_sound.size());FloatingEnvironment::prepare();bu::execute_extended_commands(b);
            const auto label="bullet_etex_"+std::to_string(type);check(label,&expected,&b,sizeof(b));compare(label+"_animation",expected_pool,Snapshot(c));check(label+"_commands",expected_commands.data(),metadata->commands.data(),metadata->commands.size()*sizeof(bu::Command));check(label+"_rng",&expected_rng,&state::random_streams[0],sizeof(expected_rng));check(label+"_sound",expected_sound.data(),&pe_state::thread_registry,expected_sound.size());
            b.metadata.reset();
        }
    }
    // Explicitly exercise the prelude without reaching any domain-dependent
    // opcode: parallel flag filtering, type 0, index 24 and unsigned -1.
    for(unsigned test=0;test<2048;++test){diagnostic_case=test;diagnostic_phase="ETEX entry gate and sequential metadata";reset(test,1);auto& b=owner.pool[0];b.metadata=metadata;b.flags=0x200u;b.field_40=0xffffffffu;for(auto& cmd:metadata->commands)cmd={};
        const unsigned indices[]{0,1,23,24,0xffffffffu};b.field_38=indices[test%5];b.field_90=(1ull<<63)|(static_cast<std::uint64_t>(rng())<<32)|rng();
        for(unsigned i=0;i<24;++i){auto& command=metadata->commands[i];command.words[8]=test%4?unsigned(1+test%33):0;command.words[9]=0;command.words[4]=4;}
        if(test%8==0){b.field_38=0;b.field_90=0;const unsigned sequence[]{7,15,23,25,33,0};for(unsigned i=0;i<std::size(sequence);++i){metadata->commands[i].words[8]=sequence[i];metadata->commands[i].words[9]=1;metadata->commands[i].words[4]=i;}}
        if(test%8==1){b.field_38=0;b.field_90=0;b.field_3c=0;metadata->commands[0].words[8]=16;metadata->commands[0].words[4]=2;metadata->commands[0].words[5]=3;metadata->commands[2].words[8]=15;metadata->commands[2].words[4]=77;metadata->commands[3].words[8]=16;metadata->commands[3].words[4]=2;metadata->commands[3].words[5]=3;metadata->commands[4].words[8]=0;}
        const auto initial=b;FloatingEnvironment::prepare();cpu<void>(0x47dcf0,&b);const auto expected=b;b=initial;FloatingEnvironment::prepare();bu::execute_extended_commands(b);check("bullet_etex_sequences_and_gate",&expected,&b,sizeof(b));b.metadata.reset();
    }
    #include "style_cpu_cases.inc"
    #include "angle_cpu_cases.inc"
    metadata.reset();gs::context(0).objects_04[0]=gs::context(1).objects_04[0]=nullptr;original_resource=saved_original_resource;rt::set_default_resource(previous_default);
}
