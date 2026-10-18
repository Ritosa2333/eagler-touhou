{
    namespace rt=th20::source::runtime;namespace state=th20::source::state;
    rt::MemoryResource resource;auto* previous_default=rt::set_default_resource(&resource);auto*& original_default=*reinterpret_cast<std::pmr::memory_resource**>(mapped_image_base+0x1e4d28);auto* previous_original_default=original_default;original_default=&resource;
    for(unsigned test=0;test<256;++test){diagnostic_case=test;diagnostic_phase="Laser Type1 complete constructor and empty destructor";
        alignas(la::Type1Laser) std::array<std::uint8_t,sizeof(la::Type1Laser)> storage;storage.fill(static_cast<std::uint8_t>(test));cpu<void>(0x4c8880,storage.data());auto expected=storage;cpu<void>(0x4c8c00,storage.data());auto expected_dtor=storage;
        storage.fill(static_cast<std::uint8_t>(test));auto* value=new(storage.data())la::Type1Laser;std::memcpy(expected.data(),storage.data(),4);check("laser_type1_ctor",expected.data(),storage.data(),storage.size());value->~Type1Laser();std::memcpy(expected_dtor.data(),storage.data(),4);check("laser_type1_dtor_empty",expected_dtor.data(),storage.data(),storage.size());
    }
    la::Type1Laser value;la::Segment segment{};auto* source_vtable=*reinterpret_cast<void**>(&value);auto* native_vtable=reinterpret_cast<void*>(mapped_image_base+0x1712e8);
    value.parameters.commands.resize(24);value.select_context(0);
    for(unsigned test=0;test<8192;++test){diagnostic_case=test;diagnostic_phase="Laser Type1 ETEX and advance complete object";
        value.flags=test%7==0?0x20:rng()&~0x20u;value.allocated_6d8=test%11?&segment:nullptr;value.state=int(test%8);value.angle=finite()*40;value.position={finite(),finite(),finite()};value.speed=finite()*32;value.field_74=finite()*64;value.field_7c=finite()*8;value.active_commands=(static_cast<std::uint64_t>(rng())<<32)|rng();value.field_6cc=rng();value.command_index=test%27;
        value.age={int(rng()),int(test%17)-4,finite()*16,rng()};value.commands[5].timer={int(rng()),int(test%9)-4,finite()*8,rng()};
        auto& p=value.parameters;p.position={finite(),finite(),finite()};p.velocity={finite(),finite(),finite()};p.angular_velocity=finite()*8;p.radial_offset=test%3?finite()*4:0;p.length_limit=finite()*80;p.width=finite()*64;p.flags=0;p.delay=int(test%11)-2;p.grow=int(test%7)-1;p.sustain=int(test%5)-1;p.shrink=int(test%13)-1;
        if(test%31==0){const std::uint32_t nan=0x7fc12345;std::memcpy(&p.width,&nan,4);}if(test%37==0){const std::uint32_t nan=0x7fc54321;std::memcpy(&p.length_limit,&nan,4);}
        for(unsigned i=0;i<24;++i){auto& command=p.commands[i];command={};command.words[8]=(test+i)%35;command.words[9]=test%3;command.words[4]=rng();}p.commands[23].words[8]=0;
        clock_value=float(test%9)*.25f;state::clock_scale=clock_value;state::timer_rate=&clock_value;*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=clock_value;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=&clock_value;
        const auto initial=*reinterpret_cast<std::array<std::uint8_t,sizeof(value)>*>(&value);const auto initial_segment=segment;
        *reinterpret_cast<void**>(&value)=native_vtable;FloatingEnvironment::prepare();const int expected_return=cpu<int>(0x4d7280,&value);*reinterpret_cast<void**>(&value)=source_vtable;const auto expected=*reinterpret_cast<std::array<std::uint8_t,sizeof(value)>*>(&value);const auto expected_segment=segment;
        std::memcpy(&value,initial.data(),initial.size());segment=initial_segment;FloatingEnvironment::prepare();const auto actual_return=value.advance();check("laser_type1_advance",expected.data(),&value,expected.size());check("laser_type1_advance_return",&expected_return,&actual_return,4);check("laser_type1_advance_segment",&expected_segment,&segment,sizeof(segment));
    }
    for(unsigned test=0;test<8192;++test){diagnostic_case=test;diagnostic_phase="Laser Type1 complete collision segment geometry";
        value.allocated_6d8=&segment;value.flags=rng();value.state=int(test%8);value.position={finite()*384,finite()*448,finite()};value.angle=finite()*40;value.field_74=finite()*128;value.speed=finite()*64;
        if(test%5==0)value.field_74=float(test%3)*16;if(test%7==0)value.speed=float(test%3)*32;if(test%31==0){const std::uint32_t nan=0x7fc12345;std::memcpy(&value.speed,&nan,4);}
        for(unsigned i=0;i<sizeof(segment)/4;++i){const auto word=rng();std::memcpy(reinterpret_cast<std::uint8_t*>(&segment)+i*4,&word,4);}la::Segment output=segment;const auto initial_output=output,initial_segment=segment;const auto initial=*reinterpret_cast<std::array<std::uint8_t,sizeof(value)>*>(&value);
        FloatingEnvironment::prepare();const auto expected_return=cpu<int>(0x4d5e50,&value,test%3?&output:nullptr);const auto expected=*reinterpret_cast<std::array<std::uint8_t,sizeof(value)>*>(&value);const auto expected_segment=segment,expected_output=output;
        std::memcpy(&value,initial.data(),initial.size());segment=initial_segment;output=initial_output;FloatingEnvironment::prepare();const auto actual_return=value.collision_segment(test%3?&output:nullptr);check("laser_type1_segment_object",expected.data(),&value,expected.size());check("laser_type1_segment_storage",&expected_segment,&segment,sizeof(segment));check("laser_type1_segment_output",&expected_output,&output,sizeof(output));check("laser_type1_segment_return",&expected_return,&actual_return,4);
    }
    value.allocated_6d8=nullptr;clock_value=1;state::clock_scale=1;state::timer_rate=&clock_value;*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=1;
    #include "type1_initialize_cpu_cases.inc"
    original_default=previous_original_default;rt::set_default_resource(previous_default);
}
