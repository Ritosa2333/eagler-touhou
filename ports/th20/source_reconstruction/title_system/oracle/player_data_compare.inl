{
    struct DataTrace {
        std::array<std::uint8_t,256> keyboard{};int keyboard_mode=0;
        static DataTrace*& current(){static DataTrace* p=nullptr;return p;}
        static void __fastcall open(void*,void*,int mode){event(80,mode);}
        static void __fastcall select(void*,void*,int index){event(81,index);}
        static void __fastcall hide(void*,void*){event(82);}
        static int __cdecl read_keyboard(void* out){std::memcpy(out,current()->keyboard.data(),256);event(83,current()->keyboard_mode);return current()->keyboard_mode;}
        static void __fastcall unlock(void*,void*){event(84);}
    } dtrace;DataTrace::current()=&dtrace;
    struct UnlockHost final:ti::UnlockEnvironment {
        int read_keyboard(std::span<std::uint8_t,256> out) override{return DataTrace::read_keyboard(out.data());}
        void unlock_progress() override{DataTrace::unlock(nullptr,nullptr);}
        void sound(int id) override{event(1,id);}
    } unlock_host;
    struct DataHost final:ti::DataEnvironment {
        ti::DataUnlockState& state;UnlockHost& unlock;
        DataHost(ti::SelectionEnvironment& s,UnlockHost& u):DataEnvironment(s),state(at<ti::DataUnlockState>(0x5c6248)),unlock(u){}
        void open_stones() override{DataTrace::open(nullptr,nullptr,2);}
        void select_stone(int index) override{DataTrace::select(nullptr,nullptr,index);}
        void hide_stones() override{DataTrace::hide(nullptr,nullptr);}
        void update_unlock_sequence() override{if(selection.main.pressed(0x8010f))state.matched=state.idle=0;ti::advance_data_unlock(state,unlock);}
    } data_host(selection_host,unlock_host);
    for(auto [va,target]:hooks)hook(va,target);for(auto [va,target]:selection_hooks)hook(va,target);
    const std::pair<unsigned,void*> data_hooks[]={{0x51a0b0,&DataTrace::open},{0x52ce80,&DataTrace::select},{0x519840,&DataTrace::hide},{0x51f310,&DataTrace::read_keyboard},{0x51e1c0,&DataTrace::unlock}};
    for(auto [va,target]:data_hooks)hook(va,target);
    new(&object.cursorbc)mn::Cursor;new(&object.cursor108)mn::Cursor;
    object.cursorbc.excluded.reserve(16);object.cursor108.excluded.reserve(16);
    for(unsigned page=0;page<2;++page)for(unsigned test=0;test<16384;++test){
        for(unsigned i=0;i<sizeof(object);++i)if(i<0x24||i>=0x154)storage[i]=std::uint8_t(random());
        auto& c=object.cursor;auto& difficulty=object.cursorbc;auto& index=object.cursor108;object.state=page?11:10;object.phase=test%5;object.words58d8[8]=(test/5)%2;
        c.count=page?16:3;c.current=random()%c.count;c.previous=int(random());c.minimum=0;c.wrapping=1;c.excluded.clear();
        difficulty.count=5;difficulty.current=(test/10)%5;difficulty.previous=int(random());difficulty.minimum=0;difficulty.wrapping=1;difficulty.excluded.clear();
        index.count=3;index.current=random()%3;index.previous=int(random());index.minimum=0;index.wrapping=1;index.excluded.clear();
        c.history.first=c.secondary_history.first=0;c.history.size=c.secondary_history.size=1;history[0][0]=random()%3;history[1][0]=3;
        object.handle390=test%3?4001:0;th20::recovered::timer_set(object.age,int((test/50)%18)-1);th20::recovered::timer_set(object.selection_age,0);th20::recovered::timer_set(object.flash_age,0);
        host.session.player_table.field_1e0=test%5;at<unsigned>(0x5b88c0)=random()&0x801ff;at<unsigned>(0x5b88b8)=random()&0xf0;
        auto& keys=data_host.state;for(auto* array:{&keys.pressed,&keys.previous,&keys.current})for(auto& byte:*array)byte=std::uint8_t(random());keys.matched=(test/3)%14;keys.idle=(test/13)%305;
        dtrace.keyboard_mode=test%3;for(auto& byte:dtrace.keyboard)byte=std::uint8_t(random());
        // Exercise the keyboard sequence independently of navigation/cancel.
        if(page&&test%4==0){object.phase=2;c.current=7;difficulty.current=3;at<unsigned>(0x5b88c0)=test%8?0:1;at<unsigned>(0x5b88b8)=0;}
        const auto unlock_before=keys;std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));const auto session_before=host.session;int hb[2][4];std::memcpy(hb,history,sizeof(history));int xb[16];std::memcpy(xb,c.excluded.data(),64);
        trace.clear();const int er=cpu<int>(page?0x524c10:0x5265a0,&object);const auto et=trace;const auto es=host.session;const auto ek=keys;std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));int eh[2][4];std::memcpy(eh,history,sizeof(history));int ex[16];std::memcpy(ex,c.excluded.data(),64);
        std::memcpy(&object,before.data(),sizeof(object));host.session=session_before;keys=unlock_before;std::memcpy(history,hb,sizeof(history));std::memcpy(c.excluded.data(),xb,64);trace.clear();const int ar=page?ti::update_player_data_detail(object,data_host):ti::update_player_data_menu(object,selection_host);
        const unsigned label=page*16384+test;check("player_data_object",label,expected.data(),&object,sizeof(object));check("player_data_session",label,&es,&host.session,sizeof(es));check("player_data_history",label,eh,history,sizeof(history));check("player_data_exclusions",label,ex,c.excluded.data(),64);check("player_data_keyboard_and_unlock",label,&ek,&keys,sizeof(keys));check("player_data_return",label,&er,&ar,4);const bool same=et==trace,yes=true;check("player_data_endpoint_trace",label,&yes,&same,1);
        if(et!=trace&&failures.size()<30){std::ostringstream out;out<<"player data trace page="<<page<<" test="<<test<<" expected:";for(auto v:et)out<<' '<<v;out<<" actual:";for(auto v:trace)out<<' '<<v;failures.push_back(out.str());}
    }
    object.cursorbc.~Cursor();object.cursor108.~Cursor();
}
