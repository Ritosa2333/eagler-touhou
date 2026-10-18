{
    // Unmodified complete51e3c0/51f220 bodies. Pages, resource/OS operations,
    // entropy and music are paired endpoints; timers/session/demo data are real.
    namespace st=th20::source::state;namespace rp=th20::source::replay;
    namespace en=th20::source::ending;namespace sc=th20::source::scheduler;
    struct FrameTrace {
        alignas(rp::ReplayInf) std::array<unsigned char,sizeof(rp::ReplayInf)> replay_bytes{};
        alignas(rp::UserHeader) std::array<unsigned char,sizeof(rp::UserHeader)> user_bytes{};
        alignas(en::EndingInf) std::array<unsigned char,sizeof(en::EndingInf)> ending_bytes{};
        th20::source::input::ButtonState buttons{};sc::Node node{};unsigned entropy_index=0,seeds[2]{};bool mutate_page=false;
        struct Stone final:th20::source::runtime::CallbackOwner {void enable_callbacks()override{event(211);}} stone;
        rp::ReplayInf& replay(){return *reinterpret_cast<rp::ReplayInf*>(replay_bytes.data());}
        rp::UserHeader& user(){return *reinterpret_cast<rp::UserHeader*>(user_bytes.data());}
        static FrameTrace*& current(){static FrameTrace* p=nullptr;return p;}
        static unsigned text_id(const char* p){unsigned h=2166136261u;while(*p)h=(h^static_cast<unsigned char>(*p++))*16777619u;return h;}
        static rp::ReplayInf* __cdecl read(const char* name){event(202,text_id(name));return &current()->replay();}
        static void __cdecl retire(void* p){event(203,p==current()->replay_bytes.data()?1u:2u);if(p==at<void*>(0x5c49e8))at<void*>(0x5c49e8)=nullptr;}
        static void __fastcall stage(gs::Session* s,void*,int value){event(204,unsigned(value));s->player_table.fields_1ec[2]=value;}
        static void __fastcall profile(void*,void*,int slot,int character,int value){event(205,unsigned(slot));event(206,unsigned(character));event(207,unsigned(value));}
        static void __fastcall stop(void*,void*){event(208);}
        static void __fastcall music(void*,void*,int slot,const char* name){event(209,unsigned(slot));event(210,text_id(name));}
        static void __fastcall start(void*,void*,int a,int b){event(212,unsigned(a));event(213,unsigned(b));}
        static unsigned __cdecl entropy(){auto* p=current();const unsigned value=p->seeds[p->entropy_index++];event(214,value);return value;}
        static void __fastcall seed(st::Random* p,void*,unsigned value){event(215,unsigned(reinterpret_cast<unsigned char*>(p)-reinterpret_cast<unsigned char*>(&at<st::Random>(0x5ba4a8)))/28);event(216,value);st::seed(*p,value);}
        static void* __cdecl stones(){return &current()->stone;}
        static void __fastcall mesh(void*,void*,void* p){event(217,unsigned(p));}
        static void __fastcall hide(void*,void*,int index,bool visible){event(218,unsigned(index));event(219,visible);}
        static void __fastcall loading(void*,void*){event(220);}
        static void __fastcall background(ti::TitleInf* o,void*){event(221);o->angle=1.25f;o->wave=-4.f;}
        static void* __cdecl effect(int a,int b,int c,int d,int e){event(222,unsigned(a));event(223,unsigned(b));event(224,unsigned(c));event(225,unsigned(d));event(226,unsigned(e));return nullptr;}
        static void __fastcall scene(void*,void*,int index){event(227,unsigned(index));at<unsigned>(0x5c584c)=unsigned(index);}
        static void page(ti::TitleInf& o,int index){event(201,unsigned(index));if(current()->mutate_page){o.words58d8[8]+=unsigned(index);th20::recovered::timer_set(o.age,17+index);th20::recovered::timer_set(o.selection_age,5);th20::recovered::timer_set(o.flash_age,1);}}
        static void __fastcall update(ti::TitleInf* o,void*){page(*o,o->state==0?1:o->state);}
        static void __fastcall draw(ti::TitleInf* o,void*){event(228,unsigned(o->state));o->words58d8[7]+=unsigned(o->state);}
        static int __cdecl copy_filename(char* out,unsigned size,const char* text){return strcpy_s(out,size,text);}
    } data;FrameTrace::current()=&data;
    struct FrameHost final:ti::FrameEnvironment {
        FrameHost(ti::MainEnvironment& m):FrameEnvironment(m,at<unsigned>(0x5c6128),at<unsigned>(0x5ba518),at<int>(0x5afcfc),at<char[256]>(0x5c4b20),at<int>(0x5c584c),at<char>(0x5bcc74),at<int>(0x5c6134),at<int>(0x5c6138)){}
        en::EndingInf* current_ending()override{return at<en::EndingInf*>(0x5c49e8);}
        const th20::source::input::ButtonState* buttons()override{return at<th20::source::input::ButtonState*>(0x5b889c);}
        rp::ReplayInf* read_replay(const char* name)override{return FrameTrace::read(name);}
        void retire(th20::source::runtime::CallbackOwner* p)override{FrameTrace::retire(p);}
        void select_stage(int value)override{FrameTrace::stage(&main.session,nullptr,value);}
        void select_profile(int c,int p)override{FrameTrace::profile(nullptr,nullptr,0,c,p);}
        void stop_music()override{FrameTrace::stop(nullptr,nullptr);}
        void play_music(const char* name)override{FrameTrace::music(nullptr,nullptr,0,name);FrameTrace::start(nullptr,nullptr,0,0);}
        unsigned entropy()override{return FrameTrace::entropy();}
        void seed(int i,unsigned value)override{FrameTrace::seed(&at<st::Random>(0x5ba4a8+i*28),nullptr,value);}
        void enable_stones()override{FrameTrace::current()->stone.enable_callbacks();}
        void release_mesh(th20::source::sprite::RenderMesh* p)override{FrameTrace::mesh(nullptr,nullptr,p);}
        void hide_file(int index)override{FrameTrace::hide(nullptr,nullptr,index,false);}
        void clear_loading()override{FrameTrace::loading(nullptr,nullptr);}
        void background(ti::TitleInf& o)override{FrameTrace::background(&o,nullptr);}
        void transition_effect()override{FrameTrace::effect(9,30,0,0,0);}
        void request_scene(int index)override{FrameTrace::scene(nullptr,nullptr,index);}
        void update_page(ti::TitleInf& o,int index)override{FrameTrace::page(o,index);}
        void draw_page(ti::TitleInf& o,int)override{FrameTrace::draw(&o,nullptr);}
    } frame_host(host);
    for(auto [va,target]:hooks)hook(va,target);
    const auto pe=th20::parse_pe(bytes);
    // Restore small shared helpers changed by previous page fixtures.
    for(unsigned va:{0x50a7f0u,0x479040u,0x52ced0u,0x52ce50u})std::memcpy(reinterpret_cast<void*>(mapped_image_base+va-0x400000),bytes.data()+pe.rva_to_file(va-0x400000,5),5);
    const std::pair<unsigned,void*> own_hooks[]{{0x50a8e0,&FrameTrace::read},{0x4217c0,&FrameTrace::retire},{0x4be360,&FrameTrace::stage},{0x51c920,&FrameTrace::profile},{0x4d9bc0,&FrameTrace::stop},{0x4d9a70,&FrameTrace::music},{0x4d9b50,&FrameTrace::start},{0x51e170,&FrameTrace::entropy},{0x4d9940,&FrameTrace::seed},{0x51b960,&FrameTrace::stones},{0x4a2800,&FrameTrace::mesh},{0x4863f0,&FrameTrace::hide},{0x4a05d0,&FrameTrace::loading},{0x52c030,&FrameTrace::background},{0x425090,&FrameTrace::effect},{0x4a0fb0,&FrameTrace::scene},{0x548610,&FrameTrace::copy_filename}};
    for(auto [va,target]:own_hooks)hook(va,target);
    for(unsigned va:{0x529e80u,0x520fb0u,0x521bc0u,0x521060u,0x52b8e0u,0x529430u,0x5265a0u,0x524c10u,0x523440u,0x5205d0u,0x5223e0u,0x526a90u,0x5204c0u,0x528fc0u,0x5284f0u,0x527f00u,0x52a8a0u})hook(va,&FrameTrace::update);
    for(unsigned va:{0x52a700u,0x529b40u,0x5257f0u,0x5240d0u,0x520c80u,0x522e20u,0x5277f0u,0x528a50u,0x52b480u})hook(va,&FrameTrace::draw);
    const unsigned globals[]{0x5c6128,0x5ba518,0x5afcfc,0x5c584c,0x5bcc74,0x5c6134,0x5c6138,0x5c49e8};
    const auto old_rate=st::timer_rate;float rate=1.f;
    st::timer_rate=&rate;at<const float*>(0x5aefe0)=&rate;
    for(unsigned test=0;test<16384;++test){
        FloatingEnvironment::prepare();
        for(unsigned i=0;i<sizeof(object);++i)if(i<0x24||i>=0x70)storage[i]=std::uint8_t(random());
        object.state=int(test%27);object.draw_node=&data.node;data.node.flags=random();object.phase=random()%4;object.mesh=reinterpret_cast<th20::source::sprite::RenderMesh*>(0x12340000u+(test%8)*16);
        object.cursor.count=10;object.cursor.current=int(test%10);object.cursor.excluded.clear();object.cursor.history.first=object.cursor.secondary_history.first=0;object.cursor.history.size=object.cursor.secondary_history.size=1;history[0][0]=3;history[1][0]=10;
        object.ui_flags=random();object.music_delay=int((test/9)%33)-2;
        for(auto va:globals)at<unsigned>(va)=random();frame_host.menu_selection=(test/27)%9;
        host.session.contexts[0].current_player=&host.session.player_table.players[0];host.session.flags=random();host.session.fields_74[2]=(test/3)%1803;host.session.fields_74[3]=test%4;host.session.mode=test%3;host.session.player_table.field_1e0=(test/7)%5;host.session.field_2bc=random();
        frame_host.data_character=test%2;frame_host.data_profile=test%9;
        data.buttons.current=test%3==0?random()&0xffff:0;at<void*>(0x5b889c)=test%5?&data.buttons:nullptr;
        at<void*>(0x5c49e8)=test%13?nullptr:data.ending_bytes.data();reinterpret_cast<en::EndingInf*>(data.ending_bytes.data())->ending_flags=(test/13)%2?8:0;
        data.replay().user=&data.user();data.user().fields_d0[2]=test%2;data.user().difficulty=(test/3)%5;for(unsigned i=0;i<4;++i)data.user().stones[i]=random()%9;
        for(unsigned i=0;i<8;++i){data.replay().stages[i]=reinterpret_cast<rp::StageRecord*>(0xabcdef00u);data.replay().playback[i].stage=i==test%9?reinterpret_cast<rp::StageRecord*>(0x12340000u):nullptr;}
        if(test%31==0){object.state=1;host.session.fields_74[2]=1799;data.buttons.current=0;at<void*>(0x5c49e8)=nullptr;}
        data.entropy_index=0;data.seeds[0]=random();data.seeds[1]=random();data.mutate_page=test%2!=0;
        rate=test%7==0?.5f:test%7==1?1.5f:1.f;for(auto* t:{&object.age,&object.selection_age,&object.flash_age}){th20::recovered::timer_set(*t,int(random()%40)-4);t->flags=random()%4;}
        std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));const auto sb=host.session;int hb[2][4];std::memcpy(hb,history,sizeof(history));std::array<unsigned,8> gb;for(unsigned i=0;i<8;++i)gb[i]=at<unsigned>(globals[i]);const auto nb=data.node;std::array<unsigned char,56> rb;std::memcpy(rb.data(),&at<st::Random>(0x5ba4a8),56);std::array<char,256> fb;std::memcpy(fb.data(),frame_host.replay_filename,256);
        trace.clear();const int er=cpu<int>(0x51e3c0,&object);const auto et=trace;const auto se=host.session;std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));int he[2][4];std::memcpy(he,history,sizeof(history));std::array<unsigned,8> ge;for(unsigned i=0;i<8;++i)ge[i]=at<unsigned>(globals[i]);const auto ne=data.node;std::array<unsigned char,56> re;std::memcpy(re.data(),&at<st::Random>(0x5ba4a8),56);std::array<char,256> fe;std::memcpy(fe.data(),frame_host.replay_filename,256);
        std::memcpy(&object,before.data(),sizeof(object));host.session=sb;std::memcpy(history,hb,sizeof(history));for(unsigned i=0;i<8;++i)at<unsigned>(globals[i])=gb[i];data.node=nb;std::memcpy(&at<st::Random>(0x5ba4a8),rb.data(),56);std::memcpy(frame_host.replay_filename,fb.data(),256);data.entropy_index=0;trace.clear();FloatingEnvironment::prepare();const int ar=ti::update(object,frame_host);std::array<unsigned,8> ga;for(unsigned i=0;i<8;++i)ga[i]=at<unsigned>(globals[i]);
        check("frame_return",test,&er,&ar,4);check("frame_object",test,expected.data(),&object,sizeof(object));check("frame_session",test,&se,&host.session,sizeof(se));check("frame_history",test,he,history,sizeof(history));check("frame_globals",test,ge.data(),ga.data(),32);check("frame_draw_node",test,&ne,&data.node,sizeof(ne));check("frame_seeded_streams",test,re.data(),&at<st::Random>(0x5ba4a8),56);check("frame_demo_filename",test,fe.data(),frame_host.replay_filename,256);const bool same=et==trace,yes=true;check("frame_endpoint_trace",test,&yes,&same,1);
        if(!same&&failures.size()<30){std::ostringstream msg;msg<<"frame trace test="<<test<<" expected:";for(auto v:et)msg<<' '<<v;msg<<" actual:";for(auto v:trace)msg<<' '<<v;failures.push_back(msg.str());}
    }
    for(unsigned test=0;test<4096;++test){object.state=int(test%29)-2;at<void*>(0x5c49e8)=test%3?nullptr:data.ending_bytes.data();std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));trace.clear();const int er=cpu<int>(0x51f220,&object);const auto et=trace;std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));std::memcpy(&object,before.data(),sizeof(object));trace.clear();const int ar=ti::draw(object,frame_host);check("frame_draw_object",test,expected.data(),&object,sizeof(object));check("frame_draw_return",test,&er,&ar,4);const bool same=et==trace,yes=true;check("frame_draw_trace",test,&yes,&same,1);}
    st::timer_rate=old_rate;at<const float*>(0x5aefe0)=old_rate;at<void*>(0x5b889c)=reinterpret_cast<void*>(mapped_image_base+0x1b88b0);at<void*>(0x5c49e8)=nullptr;
}

