{
    struct LoadoutTrace {
        std::array<int,0x21104/4> stones{};std::array<unsigned,16> effect{};int cleared[5][2][9]{};
        static LoadoutTrace*& current(){static LoadoutTrace* value=nullptr;return value;}
        static int __fastcall clear_count(void*,void*,int difficulty,int character,int profile){return current()->cleared[difficulty][character][profile];}
        static void __fastcall open(void*,void*,int mode){event(60,mode);current()->stones[0x14/4]=1;}
        static void* __cdecl effect_owner(int index){event(61,index);return current()->effect.data();}
        static void __fastcall loading(void*,void*,float x,float y){event(62,std::bit_cast<unsigned>(x));event(63,std::bit_cast<unsigned>(y));}
        static unsigned* __fastcall spawn(void*,void*,unsigned* out,int type,void* parameters,void* existing){event(64,type);event(65,unsigned(parameters==nullptr));event(66,unsigned(existing==nullptr));*out=2001;return out;}
        static void __fastcall interrupt(unsigned* handle,void*,int id){if(handle==&at<unsigned>(0x5c4f04)){event(67,*handle);event(68,id);}else child_interrupt(handle,nullptr,id);}
        static void __cdecl fade(float time){event(69,std::bit_cast<unsigned>(time));}
        static void __fastcall stage(gs::Session* session,void*,int id){event(70,id);session->player_table.fields_1ec[2]=id;}
    } loadout_trace;LoadoutTrace::current()=&loadout_trace;
    struct LoadoutHost final:ti::LoadoutEnvironment {
        explicit LoadoutHost(SelectionHost& h):LoadoutEnvironment(h){}
        int clear_count(int d,int c,int p)override{return LoadoutTrace::clear_count(nullptr,nullptr,d,c,p);}
        void open_stones()override{LoadoutTrace::open(nullptr,nullptr,0);}
        int stone_display_state()override{const auto& s=LoadoutTrace::current()->stones;if(s[0x28/4]==1){if(s[0x210f4/4]==4)return 2;if(s[0x210f4/4]==3)return 1;}return 0;}
        int stone_visibility()override{return LoadoutTrace::current()->stones[0x14/4];}
        bool effects_ready()override{return static_cast<unsigned*>(LoadoutTrace::effect_owner(0))[0x38/4]!=0;}
        void loading_transition()override{LoadoutTrace::loading(nullptr,nullptr,480.f,392.f);auto* effect=LoadoutTrace::effect_owner(0);unsigned out;LoadoutTrace::spawn(effect,nullptr,&out,0,nullptr,nullptr);at<unsigned>(0x5c4f04)=out;LoadoutTrace::interrupt(&at<unsigned>(0x5c4f04),nullptr,7);LoadoutTrace::fade(0.05f);}
        void request_start(int id)override{LoadoutTrace::stage(&selection.main.session,nullptr,id);at<int>(0x5afcfc)=-1;at<int>(0x5c584c)=7;}
    } loadout_host(selection_host);
    const std::pair<unsigned,void*> loadout_hooks[]={{0x52c440,&LoadoutTrace::clear_count},{0x51a0b0,&LoadoutTrace::open},{0x437520,&LoadoutTrace::effect_owner},{0x4a0aa0,&LoadoutTrace::loading},{0x49dcf0,&LoadoutTrace::spawn},{0x44ef90,&LoadoutTrace::interrupt},{0x4d99d0,&LoadoutTrace::fade},{0x4be360,&LoadoutTrace::stage}};
    for(auto [va,target]:loadout_hooks)hook(va,target);
    at<void*>(0x5c6120)=loadout_trace.stones.data();const auto old_clock=at<float*>(0x5aefe0);const auto old_source_clock=th20::source::state::timer_rate;
    const unsigned globals[]{0x5c6144,0x5c6128,0x5afcfc,0x5c584c,0x5c4f04};
    for(unsigned test=0;test<32768;++test){
        for(unsigned i=0;i<sizeof(object);++i)if(i<0x24||i>=0x70)storage[i]=std::uint8_t(random());
        object.state=7;object.phase=test%6;auto& c=object.cursor;c.count=3;c.current=(test/6)%3;c.previous=random();c.minimum=0;c.wrapping=1;c.excluded.clear();c.history.first=c.secondary_history.first=0;c.history.size=c.secondary_history.size=1;history[0][0]=test%2;history[1][0]=2;
        object.previous5960=(test/36)%11-1;object.previous5964=(test/396)%4-1;host.session.mode=(test/1584)%3;host.session.player_table.field_1e0=(test/4752)%5;host.session.contexts[0].current_player->fields_00[2]=(test/5)%2;host.session.contexts[0].current_player->fields_00[3]=(test/11)%9;
        th20::recovered::timer_set(object.age,int((test/3)%50)-2);float rate=(test/198)%4==0?0.f:(test/198)%4==1?0.5f:(test/198)%4==2?1.f:2.f;at<float*>(0x5aefe0)=&rate;th20::source::state::timer_rate=&rate;
        loadout_trace.stones[0x14/4]=(test/13)%4;loadout_trace.stones[0x28/4]=(test/17)%3;loadout_trace.stones[0x210f4/4]=(test/19)%6;loadout_trace.effect[0x38/4]=(test/23)%2;for(auto& row:loadout_trace.cleared)for(auto& profiles:row)for(auto& value:profiles)value=random()%3;
        for(auto va:globals)at<unsigned>(va)=random();at<unsigned>(0x5c6128)=(test/100)%7;
        const auto session_before=host.session,unused_session_copy=session_before;const auto stones_before=loadout_trace.stones;std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));int history_before[2][4];std::memcpy(history_before,history,sizeof(history));std::array<unsigned,5> globals_before;for(unsigned i=0;i<5;++i)globals_before[i]=at<unsigned>(globals[i]);
        FloatingEnvironment::prepare();trace.clear();cpu<void>(0x52b8e0,&object);const auto expected_trace=trace;const auto expected_session=host.session;const auto expected_stones=loadout_trace.stones;std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));int expected_history[2][4];std::memcpy(expected_history,history,sizeof(history));std::array<unsigned,5> expected_globals;for(unsigned i=0;i<5;++i)expected_globals[i]=at<unsigned>(globals[i]);
        host.session=session_before;loadout_trace.stones=stones_before;std::memcpy(&object,before.data(),sizeof(object));std::memcpy(history,history_before,sizeof(history));for(unsigned i=0;i<5;++i)at<unsigned>(globals[i])=globals_before[i];FloatingEnvironment::prepare();trace.clear();ti::update_loadout(object,loadout_host);std::array<unsigned,5> actual_globals;for(unsigned i=0;i<5;++i)actual_globals[i]=at<unsigned>(globals[i]);
        check("loadout_object",test,expected.data(),&object,sizeof(object));check("loadout_session",test,&expected_session,&host.session,sizeof(host.session));check("loadout_stones",test,expected_stones.data(),loadout_trace.stones.data(),sizeof(expected_stones));check("loadout_cursor_history",test,expected_history,history,sizeof(history));check("loadout_globals",test,expected_globals.data(),actual_globals.data(),sizeof(expected_globals));const bool same=trace==expected_trace,yes=true;check("loadout_endpoint_trace",test,&yes,&same,1);
    }
    at<float*>(0x5aefe0)=old_clock;th20::source::state::timer_rate=old_source_clock;
}
