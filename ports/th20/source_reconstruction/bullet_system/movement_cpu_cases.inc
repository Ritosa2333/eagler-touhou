{
    s::SpriteData descriptor{};descriptor.extent_48=16;descriptor.extent_4c=12;file.sprites=&descriptor;
    std::array<std::uint8_t,0x2200> player{};gs::context(0).objects_04[0]=player.data();gs::context(1).objects_04[0]=player.data();
    const std::uint32_t addresses[]{0x4840e0,0x483f00,0x483d20,0x483920,0x483580,0x480db0,0x4827e0,0x47c800,0x483270};
    const char* labels[]{"bullet_launch","bullet_linear","bullet_secondary_linear","bullet_angular","bullet_repeated_turn","bullet_homing","bullet_position_interpolation","bullet_position_offset","bullet_screen_wrap"};
    auto invoke=[&](unsigned operation,bu::Bullet& b){switch(operation){case 0:return bu::update_launch_acceleration(b);case 1:return bu::update_linear_acceleration(b);case 2:return bu::update_linear_acceleration(b,true);case 3:return bu::update_angular_acceleration(b);case 4:return bu::update_repeated_turn(b);case 5:return bu::update_homing(b);case 6:return bu::update_position_interpolation(b);case 7:return bu::update_position_offset(b);default:return bu::update_screen_wrap(b);}};
    for(unsigned operation=0;operation<std::size(addresses);++operation){
        reset(operation,1);auto& b=owner.pool[0];b.animation->base.fields_10_28[3]=7;b.animation->base.fields_10_28[4]=0;
        for(unsigned test=0;test<2048;++test){diagnostic_case=test;diagnostic_phase=labels[operation];
            const auto initial_empty=b; // The owning shared_ptr remains empty throughout.
            b.flags=rng();b.field_90=(static_cast<std::uint64_t>(rng())<<32)|rng();b.field_20=finite()*10;b.position={finite()*256,224+finite()*320,finite()};b.velocity={finite()*8,finite()*8,finite()*8};b.angle=finite()*12;b.field_40=0xffffffffu;
            const int times[]{-1,0,1,15,16,17,29,30,31};const int current=times[test%std::size(times)];
            for(auto& cmd:b.commands){cmd.timer={current-1,current,float(current)+.25f,rng()};cmd.field_10=finite()*4;cmd.field_14=finite()*12;cmd.vector_24={finite()*8,finite()*8,finite()*8};cmd.field_30=30;cmd.field_34=1+test%5;cmd.field_38=test%17;cmd.field_3c=test%9;}
            b.interpolation_420.start={finite(),finite(),finite()};b.interpolation_420.end={finite()*32,finite()*32,finite()*32};b.interpolation_420.tangent_start={finite(),finite(),finite()};b.interpolation_420.tangent_end={finite(),finite(),finite()};b.interpolation_420.current={finite(),finite(),finite()};b.interpolation_420.duration=30;b.interpolation_420.mode=test%10;b.interpolation_420.timer={current-1,current,float(current)+.25f,rng()};
            if(test%11==0)b.velocity.x=b.velocity.y=0;if(test%17==0)b.velocity.x=.0001f;
            const s::Vec3 player_position=test%19?s::Vec3{finite()*192,finite()*448,finite()}:b.position;std::memcpy(player.data()+0x614,&player_position,12);
            clock_value=float(test%7)*.25f;anm_clock_pointer=&clock_value;state::clock_scale=clock_value;state::timer_rate=test%23?&clock_value:nullptr;
            *reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=clock_value;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=state::timer_rate;
            const auto initial=b;FloatingEnvironment::prepare();const int expected_return=cpu<int>(addresses[operation],&b);const auto expected=b;
            b=initial;FloatingEnvironment::prepare();const int actual_return=invoke(operation,b);check(labels[operation],&expected,&b,sizeof(b));check(std::string(labels[operation])+"_return",&expected_return,&actual_return,4);
            if(operation==5){FloatingEnvironment::prepare();const auto expected_angle=cpu<float>(0x4ff350,player.data(),&b.position);FloatingEnvironment::prepare();const auto actual_angle=bu::angle_to_player(b);check("bullet_angle_to_player",&expected_angle,&actual_angle,4);}
            b=initial_empty;
        }
    }
    for(unsigned test=0;test<1024;++test){diagnostic_case=test;diagnostic_phase="Bullet retained advance state and retirement";reset(test,1);auto& b=owner.pool[0];b.animation->base.fields_10_28[3]=7;b.animation->base.fields_10_28[4]=0;b.animation->base.fields_444[0]=0;b.state=test%4==0?0:1+test%4;
        b.field_90=0;b.field_30=2;b.field_18=test%5;b.flags=0x200u|(test%11==0?8u:0u);
        const auto initial_owner=std::vector<std::uint8_t>(memory.get(),memory.get()+sizeof(owner));const Snapshot initial_pool(c);const float initial_rate=state::clock_scale;
        state::timer_rate=&state::clock_scale;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=reinterpret_cast<float*>(mapped_image_base+0x1aefe4);
        FloatingEnvironment::prepare();const int expected_return=cpu<int>(0x485b60,&b);const auto expected_owner=std::vector<std::uint8_t>(memory.get(),memory.get()+sizeof(owner));const Snapshot expected_pool(c);const auto expected_rate=*reinterpret_cast<float*>(mapped_image_base+0x1aefe4);
        std::memcpy(memory.get(),initial_owner.data(),initial_owner.size());initial_pool.restore(c);state::clock_scale=initial_rate;FloatingEnvironment::prepare();const int actual_return=bu::unrecovered::advance_bullet_00485b60(b);
        check("bullet_advance_states",expected_owner.data(),memory.get(),expected_owner.size());compare("bullet_advance_animation",expected_pool,Snapshot(c));check("bullet_advance_return",&expected_return,&actual_return,4);check("bullet_advance_rate",&expected_rate,&state::clock_scale,4);
    }
    for(unsigned test=0;test<512;++test){diagnostic_case=test;diagnostic_phase="Bullet controller actual active iteration";reset(test,32);
        for(unsigned i=0;i<32;++i){auto& b=owner.pool[i];b.flags=0x600u;b.field_90=0;b.state=1;b.field_44=i%6;}
        for(auto& field:owner.fields_14)field=rng();th20::source::gameplay::controller=nullptr;*reinterpret_cast<void**>(mapped_image_base+0x1ba828)=nullptr;
        const auto initial_owner=std::vector<std::uint8_t>(memory.get(),memory.get()+sizeof(owner));const Snapshot initial_pool(c);
        FloatingEnvironment::prepare();const int expected_return=cpu<int>(0x47d600,&owner);const auto expected_owner=std::vector<std::uint8_t>(memory.get(),memory.get()+sizeof(owner));const Snapshot expected_pool(c);
        std::memcpy(memory.get(),initial_owner.data(),initial_owner.size());initial_pool.restore(c);FloatingEnvironment::prepare();const int actual_return=owner.update();
        check("bullet_controller_update",expected_owner.data(),memory.get(),expected_owner.size());compare("bullet_controller_update_animation",expected_pool,Snapshot(c));check("bullet_controller_update_return",&expected_return,&actual_return,4);
    }
    gs::context(0).objects_04[0]=gs::context(1).objects_04[0]=nullptr;file.sprites=nullptr;
}
