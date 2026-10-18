{
    namespace rt=th20::source::runtime;rt::MemoryResource resource;auto* old_default=rt::set_default_resource(&resource);auto*& original_default=*reinterpret_cast<std::pmr::memory_resource**>(mapped_image_base+0x1e4d28);auto* old_original_default=original_default;original_default=&resource;
    for(unsigned test=0;test<256;++test){diagnostic_case=test;diagnostic_phase="Laser Type3 full constructor and destructor";
        alignas(la::Type3Laser) std::array<std::uint8_t,sizeof(la::Type3Laser)> storage;storage.fill(static_cast<std::uint8_t>(test));cpu<void>(0x4c8390,storage.data());auto expected=storage;cpu<void>(0x4c8a40,storage.data());auto expected_destroyed=storage;
        storage.fill(static_cast<std::uint8_t>(test));auto* value=new(storage.data())la::Type3Laser;std::memcpy(expected.data(),storage.data(),4);check("laser_type3_ctor",expected.data(),storage.data(),storage.size());value->~Type3Laser();std::memcpy(expected_destroyed.data(),storage.data(),4);check("laser_type3_dtor_empty",expected_destroyed.data(),storage.data(),storage.size());
    }
    la::Type3Parameters input;input.commands.resize(24);
    for(unsigned test=0;test<2048;++test){diagnostic_case=test;diagnostic_phase="Laser Type3 complete initialization and PMR vector";
        input.position={finite()*128,finite()*448,finite()};input.vector_0c={finite(),finite(),finite()};input.angle=finite()*12;input.field_1c=finite();input.field_20=finite();input.field_24=finite();input.handle=rng();input.field_2c=rng();input.field_30=rng();input.field_34=rng();input.flags=rng();input.view_index=test%2;input.commands.resize(test%25);for(auto& command:input.commands)for(auto& word:command.words)word=rng();
        alignas(la::Type3Laser) std::array<std::uint8_t,sizeof(la::Type3Laser)> storage{};auto* value=new(storage.data())la::Type3Laser;
        FloatingEnvironment::prepare();const int expected_return=cpu<int>(0x4d3110,value,&input);auto expected=storage;const auto expected_commands=std::vector<th20::source::bullet::Command>(value->parameters.commands.begin(),value->parameters.commands.end());value->~Type3Laser();
        std::memset(storage.data(),0,storage.size());value=new(storage.data())la::Type3Laser;FloatingEnvironment::prepare();const auto actual_return=value->initialize(input);
        constexpr unsigned vector_base=offsetof(la::Type3Laser,parameters)+offsetof(la::Type3Parameters,commands);std::uint32_t expected_data;std::memcpy(&expected_data,expected.data()+vector_base+4,4);const auto delta=reinterpret_cast<std::uint32_t>(value->parameters.commands.data())-expected_data;
        for(unsigned offset:{4u,8u,12u}){std::uint32_t pointer;std::memcpy(&pointer,expected.data()+vector_base+offset,4);pointer+=delta;std::memcpy(expected.data()+vector_base+offset,&pointer,4);}
        check("laser_type3_initialize",expected.data(),storage.data(),storage.size());check("laser_type3_initialize_return",&expected_return,&actual_return,4);const auto expected_size=expected_commands.size(),actual_size=value->parameters.commands.size();check("laser_type3_command_count",&expected_size,&actual_size,sizeof(actual_size));if(expected_size==actual_size)check("laser_type3_command_values",expected_commands.data(),value->parameters.commands.data(),actual_size*sizeof(th20::source::bullet::Command));
        value->flags=rng();const auto flag=rng(),keep=rng()%3;auto initial_flags=value->flags;const auto expected_request=cpu<int>(0x4c9350,value,int(rng()),int(keep));const auto expected_flags=value->flags;value->flags=initial_flags;const auto actual_request=value->request_delete(0,keep);check("laser_type3_delete_flags",&expected_flags,&value->flags,4);check("laser_type3_delete_return",&expected_request,&actual_request,4);
        const auto initial_parameter_flags=value->parameters.flags;cpu<void>(0x4d7d20,value,flag);const auto expected_parameter_flags=value->parameters.flags;value->parameters.flags=initial_parameter_flags;value->set_parameter_flag(flag);check("laser_type3_parameter_flags",&expected_parameter_flags,&value->parameters.flags,4);value->~Type3Laser();
    }
    for(unsigned test=0;test<512;++test){diagnostic_case=test;diagnostic_phase="Laser Type3 real vector geometry and base-allocation destruction";
        alignas(la::Type3Laser) std::array<std::uint8_t,sizeof(la::Type3Laser)> storage{};auto* value=new(storage.data())la::Type3Laser;value->parameters.commands.resize(test%25);value->flags=test;value->animation.fields_550[7]=32+test;
        if(test&1)value->allocated_6d8=HeapAlloc(GetProcessHeap(),0,32+test);if(test&2)value->animation.fields_550[5]=reinterpret_cast<std::uint32_t>(HeapAlloc(GetProcessHeap(),0,64+test));
        cpu<void>(0x4c8a40,value);auto expected=storage;
        std::memset(storage.data(),0,storage.size());value=new(storage.data())la::Type3Laser;value->parameters.commands.resize(test%25);value->flags=test;value->animation.fields_550[7]=32+test;
        if(test&1)value->allocated_6d8=rt::allocate_bytes(32+test);if(test&2)value->animation.fields_550[5]=reinterpret_cast<std::uint32_t>(rt::allocate_bytes(64+test));
        value->~Type3Laser();std::memcpy(expected.data(),storage.data(),4);check("laser_type3_dtor_owned_resources",expected.data(),storage.data(),storage.size());
    }
    la::Type3Laser laser;
    for(unsigned test=0;test<8192;++test){diagnostic_case=test;diagnostic_phase="Laser base geometry and context fields";laser.position={finite()*512,finite()*512,finite()};laser.angle=finite()*50;laser.flags=rng();s::Vec3 output{finite(),finite(),finite()};const auto initial_output=output;const float distance=finite()*512;
        FloatingEnvironment::prepare();cpu<void>(0x4d67e0,&laser,distance,&output);const auto expected_output=output;output=initial_output;FloatingEnvironment::prepare();laser.point_at_distance(distance,output);check("laser_point_at_distance",&expected_output,&output,sizeof(output));
        const auto initial=*reinterpret_cast<std::array<std::uint8_t,sizeof(la::Laser)>*>(&laser);const s::Vec3 position{finite(),finite(),finite()};cpu<void>(test%2?0x4d7d50:0x4d7dd0,&laser,test%2?reinterpret_cast<std::uint32_t>(&position):test%2);const auto expected=*reinterpret_cast<std::array<std::uint8_t,sizeof(la::Laser)>*>(&laser);std::memcpy(&laser,initial.data(),initial.size());if(test%2)laser.set_position(position);else laser.select_context(test%2);check("laser_base_position_context",expected.data(),&laser,expected.size());
    }
    input.commands.clear();original_default=old_original_default;rt::set_default_resource(old_default);
}
