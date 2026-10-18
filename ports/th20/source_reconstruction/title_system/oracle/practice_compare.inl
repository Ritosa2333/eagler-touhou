{
    // Page bodies execute unchanged. Only their animation/audio/renderer and
    // transition endpoints are paired; card rendering has its own GDI oracle.
    struct PracticeTrace {
        th20::source::progress::Profile profile{};std::array<unsigned,16> effect{};std::array<int,5> refreshed{};bool available=false;
        static PracticeTrace*& current(){static PracticeTrace* p=nullptr;return p;}
        static void __fastcall delete_animation(ti::TitleInf*,void*,int index){event(60,index);}
        static int __fastcall group(ti::TitleInf*,void*,int stage,int boss){event(61,stage);event(62,boss);return current()->available?1:0;}
        static void __fastcall refresh(ti::TitleInf*,void*,int stage,int boss,unsigned* out,int selected){event(63,stage);event(64,boss);event(65,selected);std::memcpy(out,current()->refreshed.data(),20);}
        static void __fastcall select(ti::TitleInf*,void*,int selected){event(66,selected);}
        static void* __fastcall get_profile(void*,void*){return &current()->profile;}
        static void __fastcall card_interrupt(unsigned* handle,void*,int id){event(67,*handle);event(68,id);}
        static void __fastcall card_retire(unsigned* handle,void*){card_interrupt(handle,nullptr,1);}
        static void __fastcall fade(void*,void*,float duration){event(69,std::bit_cast<unsigned>(duration));}
        static void* __cdecl effect_owner(int player){event(70,player);return current()->effect.data();}
        static void __fastcall loading(void*,void*,float x,float y){event(71,std::bit_cast<unsigned>(x));event(72,std::bit_cast<unsigned>(y));}
        static unsigned* __fastcall effect_spawn(void*,void*,unsigned* out,int type,void* parameters,void* existing){event(73,type);event(74,parameters==nullptr);event(75,existing==nullptr);*out=9001;return out;}
        static void __fastcall stage(gs::Session* s,void*,int value){event(76,value);s->player_table.fields_1ec[2]=value;}
        static void __fastcall spell(gs::Session* s,void*,int value){event(77,value);th20::source::gameplay::player_state::write(s->player_table,0x204,value);}
    } p;PracticeTrace::current()=&p;
    struct PracticeStage final:ti::StageEnvironment {
        PracticeStage():StageEnvironment(at<gs::Session>(0x5ba568),at<unsigned>(0x5c6128),at<int>(0x5b0a50)){}
        bool pressed(unsigned mask) override{return(at<unsigned>(0x5b88c0)&mask)!=0;}
        bool repeated(unsigned mask) override{return((at<unsigned>(0x5b88c0)|at<unsigned>(0x5b88b8))&mask)!=0;}
        const th20::source::progress::Profile& profile() override{return PracticeTrace::current()->profile;}
        int keyboard_number() override{throw std::logic_error("Unexpected practice keyboard read");}
        void spawn_heading(ti::TitleInf&) override{throw std::logic_error("Unexpected practice stage heading");}
        void retire_heading(ti::TitleInf&) override{throw std::logic_error("Unexpected practice stage retirement");}
        void sound(int id) override{event(1,id);}
        void fade(float duration) override{PracticeTrace::fade(nullptr,nullptr,duration);}
        bool effects_ready() override{return static_cast<unsigned*>(PracticeTrace::effect_owner(0))[0x38/4]!=0;}
        void loading() override{PracticeTrace::loading(nullptr,nullptr,480.f,392.f);auto* effect=PracticeTrace::effect_owner(0);unsigned result;PracticeTrace::effect_spawn(effect,nullptr,&result,0,nullptr,nullptr);at<unsigned>(0x5c4f04)=result;PracticeTrace::card_interrupt(&at<unsigned>(0x5c4f04),nullptr,7);}
        void request_start(int) override{throw std::logic_error("Practice uses the spell transition");}
    } practice_stage;
    struct PracticeHost final:ti::PracticeEnvironment {
        PracticeHost(ti::SelectionEnvironment& s,ti::StageEnvironment& t):PracticeEnvironment(s,t,at<int>(0x5b0a54),at<int>(0x5b0a58),at<int>(0x5b0a5c)){}
        void delete_animation(ti::TitleInf& o,int index) override{PracticeTrace::delete_animation(&o,nullptr,index);}
        bool group_available(int s,int b) override{return PracticeTrace::group(nullptr,nullptr,s,b);}
        bool card_playable(int id) override{const auto& p=PracticeTrace::current()->profile;return th20::source::progress::read<unsigned>(p.bytes,0xb08+id*0xe0+0xc8)||th20::source::progress::read<unsigned>(p.bytes,0xb08+id*0xe0+0xcc);}
        void refresh_cards(ti::TitleInf& o,int s,int b,int selected) override{PracticeTrace::refresh(&o,nullptr,s,b,o.words58d8+3,selected);}
        void select_card(ti::TitleInf& o,int selected) override{PracticeTrace::select(&o,nullptr,selected);}
        bool card_exists(ti::TitleInf&,int) override{throw std::logic_error("Unexpected practice handle lookup");}
        void card_interrupt(ti::TitleInf& o,int slot,int id) override{PracticeTrace::card_interrupt(o.handles394+slot,nullptr,id);}
        void launch(int s,int card) override{PracticeTrace::stage(&stage.session,nullptr,s);PracticeTrace::spell(&stage.session,nullptr,card);stage.session.player_table.field_1e0=at<unsigned>(0x5af058+card*4);at<unsigned>(0x5c584c)=7;}
    } practice_host(selection_host,practice_stage);
    for(auto [va,target]:hooks)hook(va,target);for(auto [va,target]:selection_hooks)hook(va,target);
    const std::pair<unsigned,void*> practice_hooks[]={{0x52cdf0,&PracticeTrace::delete_animation},{0x52cb30,&PracticeTrace::group},{0x51feb0,&PracticeTrace::refresh},{0x51fd80,&PracticeTrace::select},{0x50fc50,&PracticeTrace::get_profile},{0x44ef90,&PracticeTrace::card_interrupt},{0x479040,&PracticeTrace::card_retire},{0x4d99d0,&PracticeTrace::fade},{0x437520,&PracticeTrace::effect_owner},{0x4a0aa0,&PracticeTrace::loading},{0x49dcf0,&PracticeTrace::effect_spawn},{0x4be360,&PracticeTrace::stage},{0x50a7f0,&PracticeTrace::spell}};
    for(auto [va,target]:practice_hooks)hook(va,target);
    const unsigned globals[]{0x5c6128,0x5b0a50,0x5b0a54,0x5b0a58,0x5b0a5c,0x5c584c,0x5c4f04};
    const unsigned addresses[]{0x528fc0,0x5284f0,0x527f00};
    const int counts[]{3,3,4,4,4,7,13};
    for(unsigned page=0;page<3;++page)for(unsigned test=0;test<16384;++test){
        for(unsigned i=0;i<sizeof(object);++i)if(i<0x24||i>=0x70)storage[i]=std::uint8_t(random());
        auto& c=object.cursor;object.state=18+page;object.phase=test%6;object.words58d8[0]=(test/6)%7;object.words58d8[1]=random()%counts[object.words58d8[0]];
        c.count=page==0?7:page==1?counts[object.words58d8[0]]:5;c.current=random()%c.count;c.previous=int(random());c.minimum=0;c.wrapping=1;c.excluded.clear();
        c.history.first=c.secondary_history.first=0;c.history.size=c.secondary_history.size=1;history[0][0]=random()%3;history[1][0]=7;
        object.handle390=test%3?4001:0;object.handles[37]=object.handles[85]=object.handles[86]=test%3?0:1;
        th20::recovered::timer_set(object.age,int((test/42)%45)-1);th20::recovered::timer_set(object.selection_age,0);th20::recovered::timer_set(object.flash_age,0);
        for(auto va:globals)at<unsigned>(va)=random();practice_stage.menu_selection=(test/7)%7;practice_host.resume_stage=(test/11)%7;practice_host.resume_boss=(test/13)%counts[object.words58d8[0]];practice_host.resume_difficulty=int((test/17)%6)-1;
        p.available=(test/3)%2;p.effect[0x38/4]=(test/5)%2;
        for(int id=0;id<113;++id){th20::source::progress::write(p.profile.bytes,0xb08+id*0xe0+0xc8,unsigned(random()%2));th20::source::progress::write(p.profile.bytes,0xb08+id*0xe0+0xcc,unsigned(random()%2));}
        for(int i=0;i<5;++i){object.words58d8[3+i]=random()%113;p.refreshed[i]=(i&&random()%3==0)?-1:int(random()%113);}
        at<unsigned>(0x5b88c0)=random()&0x801ff;at<unsigned>(0x5b88b8)=random()&0xf0;
        std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));const auto session_before=host.session;int hb[2][4];std::memcpy(hb,history,sizeof(history));int xb[16];std::memcpy(xb,c.excluded.data(),64);std::array<unsigned,7> gb;for(unsigned i=0;i<7;++i)gb[i]=at<unsigned>(globals[i]);
        trace.clear();const int er=cpu<int>(addresses[page],&object);const auto et=trace;const auto es=host.session;std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));int eh[2][4];std::memcpy(eh,history,sizeof(history));int ex[16];std::memcpy(ex,c.excluded.data(),64);std::array<unsigned,7> eg;for(unsigned i=0;i<7;++i)eg[i]=at<unsigned>(globals[i]);
        std::memcpy(&object,before.data(),sizeof(object));host.session=session_before;std::memcpy(history,hb,sizeof(history));std::memcpy(c.excluded.data(),xb,64);for(unsigned i=0;i<7;++i)at<unsigned>(globals[i])=gb[i];trace.clear();const int ar=page==0?ti::update_practice_stage(object,practice_host):page==1?ti::update_practice_boss(object,practice_host):ti::update_practice_difficulty(object,practice_host);std::array<unsigned,7> ag;for(unsigned i=0;i<7;++i)ag[i]=at<unsigned>(globals[i]);
        const unsigned label=page*16384+test;check("practice_object",label,expected.data(),&object,sizeof(object));check("practice_session",label,&es,&host.session,sizeof(es));check("practice_history",label,eh,history,sizeof(history));check("practice_exclusions",label,ex,c.excluded.data(),64);check("practice_globals",label,eg.data(),ag.data(),28);check("practice_return",label,&er,&ar,4);const bool same=et==trace,yes=true;check("practice_endpoint_trace",label,&yes,&same,1);
        if(et!=trace&&failures.size()<30){std::ostringstream msg;msg<<"practice trace page="<<page<<" test="<<test<<" expected:";for(auto v:et)msg<<' '<<v;msg<<" actual:";for(auto v:trace)msg<<' '<<v;failures.push_back(msg.str());}
    }
}
