    std::cerr<<"option and key-config source draw tests\n";
    namespace op=th20::source::options;namespace kc=th20::source::key_config;namespace in=th20::source::input;
    alignas(op::OptionInf) unsigned char option_storage[sizeof(op::OptionInf)]{};auto& option=*reinterpret_cast<op::OptionInf*>(option_storage);::new(&option.cursor)th20::source::menu::Cursor;
    alignas(kc::KeyConfigInf) unsigned char key_storage[sizeof(kc::KeyConfigInf)]{};auto& key=*reinterpret_cast<kc::KeyConfigInf*>(key_storage);::new(&key.cursor)th20::source::menu::Cursor;
    auto* input_fixture=static_cast<in::Controller*>(VirtualAlloc(nullptr,sizeof(in::Controller),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));*reinterpret_cast<void**>(mapped_image_base+0x1b8898)=input_fixture;
    auto* ui_storage=VirtualAlloc(nullptr,sizeof(t::Job)*2,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);auto& ui_jobs=*reinterpret_cast<t::Job(*)[2]>(ui_storage);::new(&ui_jobs[0])t::Job;::new(&ui_jobs[1])t::Job;
    const unsigned ui_start_passed=passed,ui_start_failed=failed;
    for(unsigned test=0;test<4096;++test){
        option.cursor.current=rng()%6;option.cursor.count=6;option.cursor.excluded.clear();if(test%4==0)option.cursor.excluded.push_back(option.cursor.current);
        s::Vec3 position{float(int(rng()%20000)-10000)/13.f,float(int(rng()%20000)-10000)/17.f,5};const auto before_position=position;const auto flash=static_cast<int>(rng()),jitter=int(rng()%9);renderer.color=rng();renderer.shadow_color=rng();
        auto er=renderer.color,es=renderer.shadow_color;using Style=void(__cdecl*)(s::Vec3*,int,th20::source::menu::Cursor*,int,int);reinterpret_cast<Style>(mapped_image_base+0x6b590)(&position,int(test%6),&option.cursor,flash,jitter);const auto ep=position;er=renderer.color;es=renderer.shadow_color;position=before_position;
        t::style_menu_line(renderer,position,int(test%6),option.cursor,flash,jitter);check("menu_style_position",test,&ep,&position,sizeof(position));check("menu_style_color",test,&er,&renderer.color,4);check("menu_style_shadow",test,&es,&renderer.shadow_color,4);
    }
    for(unsigned test=0;test<3072;++test){
        const auto operation=test%3;const auto kind=(test/3)%3;const auto display=(test/9)%10;input_fixture->selected[0]=input_fixture->selected[1]=kind;input_fixture->devices[kind].kind=kind;
        option.position=key.position={float(test%97)-13.17f,float(test%121)+41.25f,4.2f};option.cursor.current=(test/27)%6;option.cursor.excluded.clear();option.selection_age.current=key.selection_age.current=(test/162)%9;option.key_config_age.current=key.transition_age.current=(test/1458)%31;key.selected_slot=0;key.cursor.current=(test/27)%(kind==0?9:6);key.cursor.excluded.clear();
        for(unsigned i=0;i<8;++i){key.bindings[0][i]=static_cast<std::int16_t>(i);key.bindings[1][i]=static_cast<std::int16_t>(i);key.bindings[2][i]=static_cast<std::int16_t>(0x41+i);}if(operation==1)key.cursor.current=(test/27)%3;
        auto& c=pe::graphics_state.configuration;c.value_7e=std::uint8_t(rng());c.value_7f=std::uint8_t(rng());*reinterpret_cast<th20::source::platform::Configuration*>(mapped_image_base+0x1c4f08)=c;*reinterpret_cast<int*>(mapped_image_base+0x1b87e4)=display;*reinterpret_cast<void**>(mapped_image_base+0x1c4d28)=test%29==0?&key:nullptr;
        renderer.line_count=test%327;renderer.fields_1a1d4[4]=3;renderer.scale_x=.71f;renderer.scale_y=1.91f;pe::window_state.scale=1.5f;*reinterpret_cast<float*>(mapped_image_base+0x1b8818)=1.5f;
        const char* strings[2]={"unused","unused"};
        if(operation==0){const char* labels[]{op::data::s_0057228c,op::data::s_005722a4,op::data::s_005722bc,op::data::s_005722d8,op::data::s_00570f94,op::data::s_005722fc};strings[0]=labels[option.cursor.current];if(option.cursor.current==0)strings[1]=op::data::screens[display];}
        else if(operation==1){const char* labels[]{kc::data::s_00570ebc,kc::data::s_00570edc,kc::data::s_00570efc};strings[0]=labels[key.cursor.current];}
        else if(kind==0){strings[0]=kc::data::s_00571030;strings[1]=kc::data::key_description[key.cursor.current];}else strings[0]=kc::data::pad_description[key.cursor.current];
        th20::source::scheduler::initialize_list(renderer.jobs);
        for(unsigned i=0;i<2;++i){auto& j=ui_jobs[i];j.text=strings[i];j.frames=0;j.fields_628[1]=3;th20::source::scheduler::initialize_link(j.link,reinterpret_cast<th20::source::scheduler::Node*>(&j));th20::source::scheduler::append(renderer.jobs,j.link);}
        std::vector<std::uint8_t> before_jobs(sizeof(t::Job)*2),expected_jobs(sizeof(t::Job)*2);std::memcpy(before_jobs.data(),ui_jobs,sizeof(ui_jobs));std::vector<std::uint8_t> before(sizeof(renderer)),expected(sizeof(renderer));std::memcpy(before.data(),&renderer,sizeof(renderer));
        FloatingEnvironment::prepare();if(operation==0)cpu<void>(0x4e07b0,&option);else if(operation==1)cpu<void>(0x4c7770,&key);else cpu<void>(0x4c6b30,&key);
        std::memcpy(expected.data(),&renderer,sizeof(renderer));std::memcpy(expected_jobs.data(),ui_jobs,sizeof(ui_jobs));std::memcpy(&renderer,before.data(),sizeof(renderer));std::memcpy(ui_jobs,before_jobs.data(),sizeof(ui_jobs));
        FloatingEnvironment::prepare();if(operation==0)op::draw(option,renderer,c,display,test%29==0);else if(operation==1)kc::draw_devices(key,renderer,*input_fixture);else kc::draw_bindings(key,renderer,*input_fixture);
        check("menus_full_renderer",test,expected.data(),&renderer,sizeof(renderer));check("menus_cached_jobs",test,expected_jobs.data(),ui_jobs,sizeof(ui_jobs));
    }
    {std::ofstream ui_report("source_reconstruction/options_system/draw_validation.json");ui_report<<"{\"passed\":"<<passed-ui_start_passed<<",\"failed\":"<<failed-ui_start_failed<<",\"scope\":\"46b590 style and4e07b0/4c7770/4c6b30 draw, full Renderer + two actual cached Jobs; real CRT formatting, no new glyph upload\"}\n";}
    VirtualFree(input_fixture,0,MEM_RELEASE);

    th20::source::scheduler::initialize_list(renderer.jobs);ui_jobs[1].~Job();ui_jobs[0].~Job();

    VirtualFree(ui_storage,0,MEM_RELEASE);
