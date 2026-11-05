// Pure CurveNode algorithms: no Type2 virtual methods are linked or replaced.
{
    struct Outputs {s::Vec3 position;float speed,angle;};
    std::array<la::CurveNode,4> path{};
    auto prepare_path=[&](unsigned test){for(unsigned i=0;i<path.size();++i){auto& node=path[i];node.next=i+1<path.size()?&path[i+1]:nullptr;node.field_04=rng();node.begin=float(i)*8;node.end=float(i+1)*8;node.kind=(test+i)%4;node.direction={finite(),finite(),finite()};node.position={finite()*384,finite()*448,finite()};node.angle=finite()*12;node.speed=finite()*8;node.acceleration=finite();node.angular_acceleration=test%5?finite():-1000;const std::uint32_t edges[]{0xc4778001u,0xc4778000u,0xc4777fffu};if(test%7==0)node.angular_acceleration=std::bit_cast<float>(edges[test%3]);}};
    for(unsigned operation=0;operation<3;++operation)for(unsigned test=0;test<8192;++test){diagnostic_case=test;diagnostic_phase="Laser Type2 pure linked path sampler";prepare_path(test);const auto initial_path=path;const s::Vec3 previous{finite()*384,finite()*448,finite()};const float previous_speed=finite()*8,previous_angle=finite()*12;
        float time=float(int(test%145)-16)*.25f;if(test%11==0)time=std::nextafter(float(test%5)*8,test%2?INFINITY:-INFINITY);if(operation==2&&test%13==0)time=std::bit_cast<float>(0x7fc10000u+test);
        Outputs actual{{finite(),finite(),finite()},finite(),finite()},expected=actual;auto* first=test%7?path.data():nullptr;const bool backwards=(test&1)!=0;
        FloatingEnvironment::prepare();if(operation==0)cpu<void>(0x4d6390,&path[0],&expected.position,&expected.speed,&expected.angle,time);else if(operation==1)cpu<void>(0x4d6830,&path[0],&expected.position,&expected.speed,&expected.angle,&previous,previous_speed,previous_angle,time);else cpu<void>(0x4d62e0,first,&expected.position,&expected.speed,&expected.angle,&previous,previous_speed,previous_angle,time,int(backwards));
        const auto expected_path=path;path=initial_path;FloatingEnvironment::prepare();if(operation==0)la::sample_curve_absolute(path[0],actual.position,actual.speed,actual.angle,time);else if(operation==1)la::sample_curve_backwards(path[0],actual.position,actual.speed,actual.angle,previous,previous_speed,previous_angle,time);else la::sample_curve_path(first,actual.position,actual.speed,actual.angle,previous,previous_speed,previous_angle,time,backwards);
        const auto label="laser_type2_sampler_"+std::to_string(operation);check(label+"_outputs",&expected,&actual,sizeof(actual));check(label+"_nodes",&expected_path,&path,sizeof(path));
    }
}
