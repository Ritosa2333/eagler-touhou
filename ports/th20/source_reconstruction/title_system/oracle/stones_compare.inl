{
    struct StonesTrace {
        std::array<unsigned,16> effect{};std::array<unsigned char,256> keys{};std::array<bool,41> available{},named{};int keyboard_mode=1;
        static StonesTrace*& current(){static StonesTrace* p=nullptr;return p;}
        static const char* __cdecl title(int id){event(101,unsigned(id));return current()->named.at(id)?"Trophy message":nullptr;}
        static int __fastcall achieved(void*,void*,unsigned id){event(102,id);return current()->available.at(id);}
        static void __fastcall fade(void*,void*,float value){event(103,std::bit_cast<unsigned>(value));}
        static void* __cdecl ending(){event(104,at<unsigned>(0x5afc98));return nullptr;}
        static int __cdecl keyboard(void* p){event(105);std::memcpy(p,current()->keys.data(),256);return current()->keyboard_mode;}
        static void __fastcall unlock(void*,void*){event(106);}
        static void* __cdecl effect_owner(int index){event(107,unsigned(index));return current()->effect.data();}
        static void __fastcall loading(void*,void*,float x,float y){event(108,std::bit_cast<unsigned>(x));event(109,std::bit_cast<unsigned>(y));}
        static unsigned* __fastcall effect_spawn(void*,void*,unsigned* out,int type,void* p,void* a){event(110,unsigned(type));event(111,unsigned(p==nullptr));event(112,unsigned(a==nullptr));*out=9001;return out;}
        static void __fastcall interrupt(unsigned* handle,void*,int value){event(113,*handle);event(114,unsigned(value));}
        static void __fastcall profile(void*,void*,int slot,int character,int profile){event(115,unsigned(slot));event(116,unsigned(character));event(117,unsigned(profile));}
        static void __fastcall stage(gs::Session* s,void*,int value){event(118,unsigned(value));s->player_table.fields_1ec[2]=value;}
        static void* __fastcall file(void*,void*){return reinterpret_cast<void*>(0x1000);}
        static unsigned* __fastcall background(void* file,void*,unsigned* out,const char* name,int script,int layer,void* output){event(119,unsigned(file));event(120,name==nullptr);event(121,unsigned(script));event(122,unsigned(layer));event(123,output==nullptr);*out=4001;return out;}
    } data;StonesTrace::current()=&data;
    struct StonesHost final:ti::StonesEnvironment {
        StonesHost(ti::MainEnvironment& m):StonesEnvironment(m,at<int>(0x5c6130),at<ti::StonesUnlockState>(0x5c6550)){}
        const char* title(int id)override{return StonesTrace::title(id);}const char* description(int,bool,int)override{throw std::logic_error("Draw is separately compared");}bool achieved(unsigned id)override{return StonesTrace::achieved(nullptr,nullptr,id)!=0;}
        void background(ti::TitleInf& o)override{StonesTrace::background(reinterpret_cast<void*>(0x1000),nullptr,&o.handle390,nullptr,19,-1,nullptr);}void heading(ti::TitleInf& o,bool visible)override{if(visible)::spawn(&o,nullptr,47);else ::erase(&o,nullptr,47);}
        void fade(float value)override{StonesTrace::fade(nullptr,nullptr,value);}void ending(int id)override{at<int>(0x5afc98)=id;main.sound(7);fade(.5f);StonesTrace::ending();}
        bool effects_ready()override{return static_cast<unsigned*>(StonesTrace::effect_owner(0))[0x38/4]!=0;}
        void loading()override{StonesTrace::loading(nullptr,nullptr,480.f,392.f);auto* effect=StonesTrace::effect_owner(0);unsigned h;StonesTrace::effect_spawn(effect,nullptr,&h,0,nullptr,nullptr);at<unsigned>(0x5c4f04)=h;StonesTrace::interrupt(&at<unsigned>(0x5c4f04),nullptr,7);}
        void prepare_replay(ti::TitleInf& o)override{auto& s=main.session;auto& p=*s.contexts[0].current_player;at<unsigned>(0x5c6128)=6;at<unsigned>(0x5c6134)=p.fields_00[2];at<unsigned>(0x5c6138)=p.fields_00[3];const int selected=o.cursor.current-18;ti::set_character(s,selected/8);p.fields_00[3]=unsigned(selected%8);StonesTrace::profile(nullptr,nullptr,0,int(p.fields_00[2]),int(p.fields_00[3]));th20::source::gameplay::player_state::write(s.player_table,0x204,-1);at<int>(0x5afcfc)=1;StonesTrace::stage(&s,nullptr,7);ti::set_state(o,2);last_record=o.cursor.current;at<unsigned>(0x5c584c)=7;}
        int keyboard(std::span<std::uint8_t,256> k)override{return StonesTrace::keyboard(k.data());}void unlock_progress()override{StonesTrace::unlock(nullptr,nullptr);}
    } stones_host(host);
    for(auto [va,target]:hooks)hook(va,target);
    // Practice's paired spell setter is not a boundary of this page. Restore
    // its original prologue so this comparison executes the real tiny setter.
    {const auto pe=th20::parse_pe(bytes);std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x10a7f0),bytes.data()+pe.rva_to_file(0x10a7f0,5),5);}
    const std::pair<unsigned,void*> own_hooks[]{{0x52f540,&StonesTrace::title},{0x52ca70,&StonesTrace::achieved},{0x4d99d0,&StonesTrace::fade},{0x4a1010,&StonesTrace::ending},{0x51f310,&StonesTrace::keyboard},{0x51e1c0,&StonesTrace::unlock},{0x437520,&StonesTrace::effect_owner},{0x4a0aa0,&StonesTrace::loading},{0x49dcf0,&StonesTrace::effect_spawn},{0x44ef90,&StonesTrace::interrupt},{0x51c920,&StonesTrace::profile},{0x4be360,&StonesTrace::stage},{0x437950,&StonesTrace::file},{0x450c70,&StonesTrace::background}};for(auto [va,target]:own_hooks)hook(va,target);
    const unsigned globals[]{0x5c6130,0x5afc98,0x5afcfc,0x5c6128,0x5c6134,0x5c6138,0x5c584c,0x5c4f04};
    for(unsigned test=0;test<16384;++test){
        for(unsigned i=0;i<sizeof(object);++i)if(i<0x24||i>=0x70)storage[i]=std::uint8_t(random());
        auto& c=object.cursor;object.state=23;object.phase=test%5;c.count=41;c.current=int((test/5)%41);c.previous=int(random());c.minimum=0;c.wrapping=1;c.excluded.clear();object.handle390=test%3?0:2001;
        c.history.first=c.secondary_history.first=0;c.history.size=c.secondary_history.size=1;history[0][0]=6;history[1][0]=12;th20::recovered::timer_set(object.age,int((test/9)%44)-2);host.session.contexts[0].current_player=&host.session.player_table.players[0];
        for(auto va:globals)at<unsigned>(va)=random();stones_host.last_record=int(test%41);data.effect[0x38/4]=(test/7)%2;data.keyboard_mode=(test/13)%3;
        for(unsigned i=0;i<41;++i){data.available[i]=(random()%2)!=0;data.named[i]=(random()%4)!=0;}for(auto& key:data.keys)key=std::uint8_t(random());for(auto& byte:reinterpret_cast<unsigned char(&)[sizeof(stones_host.unlock)]>(stones_host.unlock))byte=std::uint8_t(random());stones_host.unlock.matched=test%9;stones_host.unlock.idle=(test/17)%303;
        at<unsigned>(0x5b88c0)=random()&0x801ff;at<unsigned>(0x5b88b8)=random()&0xf0;
        if(test%11==0){object.phase=1;c.current=13;at<unsigned>(0x5b88c0)=at<unsigned>(0x5b88b8)=0;data.keyboard_mode=1;data.keys.fill(0);stones_host.unlock.current.fill(0);if(stones_host.unlock.matched<8)data.keys[at<unsigned>(0x5755cc+stones_host.unlock.matched*4)]=0x80;}
        std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));const auto sb=host.session;const auto ub=stones_host.unlock;int hb[2][4];std::memcpy(hb,history,sizeof(history));std::array<unsigned,8> gb;for(unsigned i=0;i<8;++i)gb[i]=at<unsigned>(globals[i]);trace.clear();const int er=cpu<int>(0x52a8a0,&object);const auto et=trace;const auto se=host.session;const auto ue=stones_host.unlock;std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));int he[2][4];std::memcpy(he,history,sizeof(history));std::array<unsigned,8> ge;for(unsigned i=0;i<8;++i)ge[i]=at<unsigned>(globals[i]);
        std::memcpy(&object,before.data(),sizeof(object));host.session=sb;stones_host.unlock=ub;std::memcpy(history,hb,sizeof(history));for(unsigned i=0;i<8;++i)at<unsigned>(globals[i])=gb[i];trace.clear();const int ar=ti::update_stones(object,stones_host);std::array<unsigned,8> ga;for(unsigned i=0;i<8;++i)ga[i]=at<unsigned>(globals[i]);
        check("stones_object",test,expected.data(),&object,sizeof(object));check("stones_session",test,&se,&host.session,sizeof(se));check("stones_unlock_all776bytes",test,&ue,&stones_host.unlock,sizeof(ue));check("stones_history",test,he,history,sizeof(history));check("stones_globals",test,ge.data(),ga.data(),32);check("stones_return",test,&er,&ar,4);const bool same=et==trace,yes=true;check("stones_endpoint_trace",test,&yes,&same,1);
        if(!same&&failures.size()<30){std::ostringstream msg;msg<<"stones trace test="<<test<<" expected:";for(auto v:et)msg<<' '<<v;msg<<" actual:";for(auto v:trace)msg<<' '<<v;failures.push_back(msg.str());}
    }
}
