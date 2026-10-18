    {
        namespace sp=th20::source::sprite;
        auto* sprites=static_cast<sp::Controller*>(VirtualAlloc(nullptr,sizeof(sp::Controller),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));if(!sprites)throw std::bad_alloc();
        *reinterpret_cast<void**>(mapped_image_base+0x1c0028)=sprites;
        std::vector<std::uint8_t> player(0x14700);std::array<std::array<std::uint8_t,0x118>,2> shots{};
        auto& list=*reinterpret_cast<sc::List*>(player.data()+0x146e4);sc::initialize_list(list);
        for(unsigned i=0;i<2;++i){auto& link=*reinterpret_cast<sc::Link*>(shots[i].data());sc::initialize_link(link,reinterpret_cast<sc::Node*>(shots[i].data()));sc::append(list,link);*reinterpret_cast<int*>(shots[i].data()+0x14)=10+i;}
        gs::context(0).objects_04[0]=player.data();gs::context(0).object_28=fixture;reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568)->contexts[0]=gs::context(0);
        for(unsigned test=0;test<2048;++test){fixture->select_context(0);fixture->initialize_pool(0);auto handle=d::create_circle(*fixture,{1,2,3},10,0,20,1);
            d::Region region{};d::select_context(region,0);region.field_90=test%2+10;region.motion.position={coordinate(),coordinate(),coordinate()};auto* shot=shots[test%2].data();
            *reinterpret_cast<unsigned*>(shot+0x18)=0x10000u;*reinterpret_cast<unsigned*>(shot+0x4c)=test%3==1?reinterpret_cast<unsigned>(&shot_custom_boundary):0;*reinterpret_cast<sp::Vec3*>(shot+0x50)={coordinate(),coordinate(),coordinate()};*reinterpret_cast<float*>(shot+0x68)=coordinate();*reinterpret_cast<unsigned*>(shot+0x9c)=random();*reinterpret_cast<unsigned*>(shot+0xac)=random();*reinterpret_cast<unsigned*>(shot+0xdc)=test%3?handle:0;shot[0x114]=test%5==0;
            auto& animation=sprites->pool[0];std::memset(&animation,0,sizeof(animation));animation.active=1;animation.animation.handle=0x10000u;
            const auto expected_lookup=cpu<void*>(0x4c0f70,&region);const auto actual_lookup=d::find_player_shot(region);check("player_shot_lookup",test,&expected_lookup,&actual_lookup,4);
            const auto before_shots=shots;const auto before_animation=animation;std::array<std::uint8_t,sizeof(*fixture)> before_owner,expected_owner;std::memcpy(before_owner.data(),fixture,before_owner.size());const sp::Vec3 position{coordinate(),coordinate(),coordinate()};
            const bool first_callback=(test/2)%2!=0;const auto callback_va=first_callback?0x4bfcd0:0x4bfd60;using Hit=int(__cdecl*)(d::Region*,const sp::Vec3*,const sp::Vec2*,float,float);
            FloatingEnvironment::prepare();const auto expected_return=reinterpret_cast<Hit>(mapped_image_base+callback_va-0x400000)(&region,&position,nullptr,1.f,2.f);const auto expected_shots=shots;const auto expected_animation=animation;std::memcpy(expected_owner.data(),fixture,expected_owner.size());
            shots=before_shots;animation=before_animation;std::memcpy(fixture,before_owner.data(),before_owner.size());FloatingEnvironment::prepare();const auto actual_return=first_callback?d::shot_hit_callback(region,position,nullptr,1,2,*sprites):d::diminish_shot_hit(region,position);
            check("player_shot_hit_return",test,&expected_return,&actual_return,4);check("player_shot_hit_bytes_and_iterators",test,expected_shots.data(),shots.data(),sizeof(shots));check("player_shot_hit_real_animation",test,&expected_animation,&animation,sizeof(animation));check("player_shot_hit_real_damage_retirement",test,expected_owner.data(),fixture,sizeof(*fixture));
        }
        VirtualFree(sprites,0,MEM_RELEASE);
    }
    {
        const float nan=std::numeric_limits<float>::quiet_NaN(),infinity=std::numeric_limits<float>::infinity();const float values[]{-0.f,0.f,-4.f,-1.f,1.f,4.f,std::nextafter(1.f,0.f),std::nextafter(1.f,2.f),nan,infinity,-infinity};
        unsigned test=0;for(const auto x:values)for(const auto y:values)for(const auto radius:values){
            boolean("rectangle_circle_IEEE_boundary",test,geometry_cpu(0x457610,0.f,0.f,2.f,2.f,0.f,x,y,radius),g::rectangle_circle(0,0,2,2,0,x,y,radius));
            boolean("circle_point_IEEE_boundary",test,geometry_cpu(0x456fe0,x,y,0.f,0.f,radius),g::circle_point(x,y,0,0,radius));
            boolean("ellipse_point_IEEE_boundary",test,geometry_cpu(0x457040,x,y,0.f,0.f,radius,radius,0.f),g::ellipse_point(x,y,0,0,radius,radius,0));
            ++test;
        }
        for(unsigned i=0;i<4096;++i){gs::Player expected{},actual{};expected.fields_00[0]=random();expected.fields_00[1]=random();actual=expected;const std::uint64_t amount=(std::uint64_t(random())<<32)|random();cpu<void>(0x4e14b0,&expected,static_cast<unsigned>(amount),static_cast<unsigned>(amount>>32));d::add_score(actual,amount);check("unsigned_score_add_divide_clamp",i,&expected,&actual,sizeof(actual));}
    }
