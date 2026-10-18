// These cases execute the unmodified original functions in the existing isolated
// PE oracle. Production libraries never load or call that image.
{
    namespace bu=th20::source::bullet;namespace gs=th20::source::game_session;
    namespace state=th20::source::state;namespace pe_state=th20::source::program_entry;
    auto finite=[&]{return float(int(rng()%1000)-500)/499.f;};
    q::State function_state{};q::initialize_state(function_state);pe_state::function_controller=&function_state;
    *reinterpret_cast<void**>(mapped_image_base+0x1b66d8)=&function_state;
    init();c.files[7]=nullptr;gs::context(0)={};gs::context(1)={};
    for(unsigned test=0;test<256;++test){diagnostic_case=test;diagnostic_phase="Bullet full constructor";
        alignas(bu::Bullet) std::array<std::uint8_t,sizeof(bu::Bullet)> memory;memory.fill(static_cast<std::uint8_t>(test));
        cpu<void>(0x47be00,memory.data());const auto expected=memory;
        memory.fill(static_cast<std::uint8_t>(test));auto* value=new(memory.data())bu::Bullet;
        check("bullet_ctor",expected.data(),memory.data(),memory.size());value->~Bullet();
    }
    auto memory=std::make_unique<std::uint8_t[]>(sizeof(bu::Controller));
    for(unsigned test=0;test<16;++test){diagnostic_case=test;diagnostic_phase="Bullet controller full constructor";
        const auto pattern=static_cast<std::uint8_t>(test*17);std::memset(memory.get(),pattern,sizeof(bu::Controller));
        cpu<void>(0x47b8a0,memory.get());std::vector<std::uint8_t> expected(memory.get(),memory.get()+sizeof(bu::Controller));
        std::memset(memory.get(),pattern,sizeof(bu::Controller));auto* value=new(memory.get())bu::Controller;
        // A native C++ class has its newly compiled vtable. No other byte is normalized.
        std::memcpy(expected.data(),memory.get(),4);check("bullet_controller_ctor",expected.data(),memory.get(),expected.size());value->~Controller();
    }
    const std::uint32_t edges[]{0,0x80000000,0x3f800000,0xbf800000,0x3f7fffff,0x3f800001,0x4effffff,0x4f000000,0xcf000000,0xcf000001,0x7f800000,0xff800000,0x7fc12345,0xffc12345,1,0x80000001};
    for(unsigned test=0;test<8192;++test){diagnostic_case=test;diagnostic_phase="Bullet float timer";
        th20::recovered::Timer timer{static_cast<int>(rng()),static_cast<int>(rng()),finite(),rng()};const auto initial=timer;
        const auto bits=test<4096?edges[test%std::size(edges)]:rng();float value;std::memcpy(&value,&bits,4);
        FloatingEnvironment::prepare();cpu<void>(0x4841f0,&timer,value);const auto expected=timer;timer=initial;
        FloatingEnvironment::prepare();bu::assign_timer_float(timer,value);check("bullet_timer_float",&expected,&timer,sizeof(timer));
    }
    for(unsigned test=0;test<16384;++test){diagnostic_case=test;diagnostic_phase="Bullet geometry IEEE boundary";
        s::Vec3 p{finite()*512,finite()*512,finite()},center{finite()*512,finite()*512,finite()};float width=finite()*64,height=finite()*64,radius=finite()*300;
        if(test<1024){p.x=float(int(test%5)-2)*192;p.y=float(int(test%4)-1)*224;width=float(test%3)*32;height=float(test%7)*32;}
        if(test%13==0){const auto bits=edges[(test/13)%std::size(edges)];std::memcpy(reinterpret_cast<float*>(&p)+(test%3),&bits,4);}
        using Outside=int(__cdecl*)(const s::Vec3*,float,float);using Circle=int(__cdecl*)(const s::Vec3*,const s::Vec3*,float);
        FloatingEnvironment::prepare();const int expected_out=reinterpret_cast<Outside>(mapped_image_base+0x85390)(&p,width,height);
        FloatingEnvironment::prepare();const int actual_out=bu::outside_viewport(p,width,height);check("bullet_viewport",&expected_out,&actual_out,4);
        FloatingEnvironment::prepare();const int expected_circle=reinterpret_cast<Circle>(mapped_image_base+0x85a60)(&p,&center,radius);
        FloatingEnvironment::prepare();const int actual_circle=bu::in_circle(p,center,radius);check("bullet_circle_geometry",&expected_circle,&actual_circle,4);
    }
    std::memset(memory.get(),0,sizeof(bu::Controller));auto& owner=*new(memory.get())bu::Controller;
    const auto empty_owner=std::vector<std::uint8_t>(memory.get(),memory.get()+sizeof(owner));
    // Interrupt label 1 followed by stop, plus the required scan terminator.
    const std::uint32_t script_words[]{0x000c0005,0,1,0x00080002,0,0x0008ffff,0};
    s::AnmInstruction* script_pointer=reinterpret_cast<s::AnmInstruction*>(const_cast<std::uint32_t*>(script_words));
    s::AnimationFile file{};file.id=7;file.stem="bullet";file.scripts=&script_pointer;
    auto reset=[&](unsigned test,unsigned count){
        std::memcpy(memory.get(),empty_owner.data(),empty_owner.size());init();c.files[7]=&file;c.field_6c4=test%2;owner.select_context(test%2);
        gs::context(test%2).primary_owner=&owner;owner.file=&file;owner.cancel_counter=rng();
        clock_value=float(test%7)*.25f;anm_clock_pointer=&clock_value;state::clock_scale=clock_value;state::timer_rate=&clock_value;
        *reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=clock_value;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=&clock_value;
        for(unsigned i=0;i<count;++i){auto& b=owner.pool[i];auto& a=c.pool[i].animation;c.pool[i].active=1;s::register_animation(c,c.lists,a,false,false);
            a.base.fields_10_28[2]=7;a.base.fields_10_28[5]=0;a.base.fields_10_28[6]=0;
            b.flags=(rng()|0x200u);b.index=i;b.state=(i+test)%6;b.field_18=(i+test)%7==0?1:0;b.cancel_script=-1;
            b.animation=&a;b.animation_handle=test%5?a.handle:0;b.position={finite()*240,finite()*500,finite()};b.velocity={finite()*8,finite()*8,finite()*8};b.size={float(rng()%40),finite()*40};b.scale=float(rng()%8)*.25f;b.angle=finite()*3;
            b.timer_4d8={1,2,2.25f,rng()};b.timer_4e8={3,4,4.25f,rng()};b.timer_4f8={5,6,6.25f,rng()};b.view_index=test%2;b.context=&gs::context(test%2);
            q::append(owner.active,b.link);
        }
    };
    auto sound_reset=[&]{for(auto& request:pe_state::thread_registry.requests){std::memset(&request,0,sizeof(request));request.id=-1;}std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1ba830),&pe_state::thread_registry,0x57e8);};
    for(unsigned test=0;test<1024;++test){diagnostic_case=test;diagnostic_phase="Bullet cancel real ANM interrupt";reset(test,1);auto& bullet=owner.pool[0];if(test&1)bullet.flags&=~0x200u;sound_reset();
        const auto initial_owner=std::vector<std::uint8_t>(memory.get(),memory.get()+sizeof(owner));const Snapshot initial_pool(c);const auto initial_sound=std::vector<std::uint8_t>(reinterpret_cast<std::uint8_t*>(&pe_state::thread_registry),reinterpret_cast<std::uint8_t*>(&pe_state::thread_registry)+0x57e8);
        FloatingEnvironment::prepare();cpu<void>(0x47c8f0,&bullet,0);const auto expected_owner=std::vector<std::uint8_t>(memory.get(),memory.get()+sizeof(owner));const Snapshot expected_pool(c);const auto expected_view=c.field_6c4;const auto expected_sound=std::vector<std::uint8_t>(reinterpret_cast<std::uint8_t*>(mapped_image_base+0x1ba830),reinterpret_cast<std::uint8_t*>(mapped_image_base+0x1ba830)+0x57e8);
        std::memcpy(memory.get(),initial_owner.data(),initial_owner.size());initial_pool.restore(c);std::memcpy(&pe_state::thread_registry,initial_sound.data(),initial_sound.size());FloatingEnvironment::prepare();bu::cancel(bullet,0);
        check("bullet_cancel_owner",expected_owner.data(),memory.get(),expected_owner.size());compare("bullet_cancel_animation",expected_pool,Snapshot(c));check("bullet_cancel_view",&expected_view,&c.field_6c4,4);check("bullet_cancel_sound",expected_sound.data(),&pe_state::thread_registry,expected_sound.size());
    }
    for(unsigned test=0;test<1536;++test){diagnostic_case=test;diagnostic_phase=test%3==2?"Bullet near circle active list":test%3==1?"Bullet rectangle active list":"Bullet circle active list";reset(test,32);
        const s::Vec3 center{finite()*192,224+finite()*224,finite()},size{float(rng()%768),float(rng()%896),finite()};const float angle=finite()*3.14f,radius=float(rng()%400);const int limit=int(test%37)-3;const std::uint32_t kind=rng();
        const auto initial_owner=std::vector<std::uint8_t>(memory.get(),memory.get()+sizeof(owner));const Snapshot initial_pool(c);
        FloatingEnvironment::prepare();const int expected_result=test%3==2?(cpu<void>(0x47d100,&owner,&center,radius,0),0):test%3==1?cpu<int>(0x47cc90,&owner,&center,&size,angle,0,kind):cpu<int>(0x47cf60,&owner,&center,radius,0,limit,kind);
        const auto expected_owner=std::vector<std::uint8_t>(memory.get(),memory.get()+sizeof(owner));const Snapshot expected_pool(c);const auto expected_view=c.field_6c4;
        std::memcpy(memory.get(),initial_owner.data(),initial_owner.size());initial_pool.restore(c);FloatingEnvironment::prepare();const int actual_result=test%3==2?(bu::cancel_near_circle(owner,center,radius,0),0):test%3==1?bu::cancel_rectangle(owner,center,size,angle,0,kind):bu::cancel_circle(owner,center,radius,0,limit,kind);
        const auto label=test%3==2?"bullet_cancel_near_circle":test%3==1?"bullet_cancel_rectangle":"bullet_cancel_circle";check(std::string(label)+"_owner",expected_owner.data(),memory.get(),expected_owner.size());compare(std::string(label)+"_animation",expected_pool,Snapshot(c));check(std::string(label)+"_view",&expected_view,&c.field_6c4,4);check(std::string(label)+"_return",&expected_result,&actual_result,4);
    }
    #include "movement_cpu_cases.inc"
    #include "bounds_cpu_cases.inc"
    #include "../bomb_system/cancellation_cpu_cases.inc"
    #include "commands_cpu_cases.inc"
    owner.~Controller();gs::context(0).primary_owner=gs::context(1).primary_owner=nullptr;c.files[7]=nullptr;
    clock_value=1;anm_clock_pointer=&clock_value;state::clock_scale=1;state::timer_rate=&clock_value;
    *reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=1;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=&clock_value;
}
