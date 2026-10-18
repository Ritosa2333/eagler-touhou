{
    // This raw owner/entity layout supplies real list and position storage to
    // both recovered and original selectors; no selector call is replaced.
    std::array<std::uint8_t,0x134> enemy_owner{};std::array<std::uint8_t,0x428> enemy{};
    auto& enemies=*reinterpret_cast<q::List*>(enemy_owner.data()+0x108);q::initialize_list(enemies);q::Link enemy_link{};enemy_link.value=reinterpret_cast<q::Node*>(enemy.data());q::append(enemies,enemy_link);
    const std::uint32_t identifier=123;std::memcpy(enemy_owner.data()+0x54,&identifier,4);std::memcpy(enemy.data()+0x88,&identifier,4);
    auto* previous_enemy0=gs::context(0).objects_04[1];auto* previous_enemy1=gs::context(1).objects_04[1];gs::context(0).objects_04[1]=gs::context(1).objects_04[1]=enemy_owner.data();
    const float requests[]{-999991.f,-999990.f,-999989.9375f,999989.9375f,999990.f,1999989.875f,1999990.f,2999989.75f,2999990.f,3999989.75f,3999990.f,4999989.5f,4999990.f,7000000.f,0.f};
    const std::uint32_t nonfinite[]{0x7fc12345,0xffc12345,0x7f800000,0xff800000,0x80000000,0,1,0x80000001};
    for(unsigned test=0;test<8192;++test){diagnostic_case=test;diagnostic_phase="ETEX resolve-angle threshold IEEE and real target selector";
        const s::Vec3 from{finite()*192,finite()*448,finite()},player_position{finite()*192,finite()*448,finite()},target{finite()*192,finite()*448,finite()};std::memcpy(player.data()+0x614,&player_position,12);std::memcpy(enemy.data()+0x198,&target,12);
        float current=finite()*12,requested=requests[test%std::size(requests)],spread=test%5==0?-999990.f:finite()*4;
        if(test%11==0)std::memcpy(&requested,&nonfinite[(test/11)%std::size(nonfinite)],4);if(test%13==0)std::memcpy(&spread,&nonfinite[(test/13)%std::size(nonfinite)],4);if(test%17==0)std::memcpy(&current,&nonfinite[(test/17)%std::size(nonfinite)],4);
        state::Random initial_rng(0);initial_rng.state=1+rng()%0x7ffffffe;initial_rng.upper=initial_rng.modulus=0x7fffffff;state::random_streams[0]=initial_rng;std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),&initial_rng,sizeof(initial_rng));
        using Resolve=float(__cdecl*)(int,float,const s::Vec3*,float,float);FloatingEnvironment::prepare();const auto expected=reinterpret_cast<Resolve>(mapped_image_base+0x85740)(test%2,current,&from,requested,spread);const auto expected_rng=*reinterpret_cast<state::Random*>(mapped_image_base+0x1ba4a8);
        FloatingEnvironment::prepare();const auto actual=bu::resolve_angle(test%2,current,from,requested,spread);check("bullet_resolve_angle_ieee",&expected,&actual,4);check("bullet_resolve_angle_rng",&expected_rng,&state::random_streams[0],sizeof(expected_rng));
    }
    gs::context(0).objects_04[1]=previous_enemy0;gs::context(1).objects_04[1]=previous_enemy1;
}
