// Actual original functions and the SDK's exported D3DX math; no instruction patches.
{
    namespace bg=th20::source::background;namespace dx=th20::source::platform_window::directx;
    auto finite=[&]{return float(int(rng()%10001)-5000)/17.f;};
    for(unsigned test=0;test<4096;++test){diagnostic_case=test;diagnostic_phase="object cull";
        bg::Object object{};for(auto& value:object.parameters)value=finite();object.parameters[0]=test%11?0.f:1.f;
        camera={};camera.viewport={rng()%60,rng()%80,640,480,0,1};camera.vectors[0][0]=finite();camera.vectors[0][1]=finite();camera.vectors[0][2]=finite();camera.vectors[5][0]=finite();camera.vectors[5][1]=finite();camera.vectors[5][2]=finite();
        const dx::Vector3 eye{0,0,-80},target{0,0,1},up{0,1,0};dx::look_at(camera.view,eye,target,up);dx::perspective(camera.projection,1.1f,4.f/3.f,30.f,8000.f);
        const s::Vec3 instance{finite(),finite(),finite()};const float distance=test%5?float(rng()%3000000):0.f;
        FloatingEnvironment::prepare();const auto expected=cpu<int>(0x473b20,&object,&instance,distance,&camera);FloatingEnvironment::prepare();const auto actual=bg::cull_object(object,instance,distance,camera);check("background_cull",&expected,&actual,4);
    }
    for(unsigned test=0;test<512;++test){diagnostic_case=test;diagnostic_phase="background perspective";
        for(auto& word:*reinterpret_cast<std::array<std::uint32_t,sizeof(camera)/4>*>(&camera))word=rng();
        camera.viewport={rng()%32,rng()%64,100+rng()%2048,100+rng()%2048,0,1};camera.field_of_view=.2f+float(rng()%2000)/1000.f;
        for(auto& vector:camera.vectors)for(auto& v:vector)v=finite()/100.f;
        if(test%17==0)std::memset(camera.vectors[3],0,12);
        if(test%19==0)for(auto& v:camera.vectors[3])v/=100000.f;
        const auto initial=camera;c.quad_count=0;c.field_c0=0;c.draw_calls=0;device_calls.clear();matrix_calls.clear();
        FloatingEnvironment::prepare();cpu<void>(0x4d9f60,nullptr,&camera);const auto expected=camera;const auto calls=device_calls;const auto matrices=matrix_calls;
        camera=initial;c.quad_count=0;c.field_c0=0;c.draw_calls=0;device_calls.clear();matrix_calls.clear();FloatingEnvironment::prepare();bg::update_perspective_camera(camera);
        check("background_perspective_camera",&expected,&camera,sizeof(camera));const auto n=calls.size(),m=device_calls.size(),nmat=matrices.size(),mmat=matrix_calls.size();check("background_perspective_call_count",&n,&m,sizeof(n));if(n==m)check("background_perspective_calls",calls.data(),device_calls.data(),n*sizeof(calls[0]));check("background_perspective_matrix_count",&nmat,&mmat,sizeof(nmat));if(nmat==mmat)check("background_perspective_matrices",matrices.data(),matrix_calls.data(),nmat*sizeof(matrices[0]));
    }
}

