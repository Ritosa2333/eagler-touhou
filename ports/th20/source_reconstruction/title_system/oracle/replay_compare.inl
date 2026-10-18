{
    struct ReplayTrace {
        alignas(th20::source::replay::ReplayInf) std::array<unsigned char,sizeof(th20::source::replay::ReplayInf)*100> storage{};
        alignas(th20::source::replay::UserHeader) std::array<unsigned char,sizeof(th20::source::replay::UserHeader)*100> users{};
        alignas(th20::source::replay::StageRecord) std::array<unsigned char,sizeof(th20::source::replay::StageRecord)*8> stages{};
        std::array<unsigned,16> effect{};bool finish=false;std::vector<std::string> names;
        static ReplayTrace*& current(){static ReplayTrace* p=nullptr;return p;}
        th20::source::replay::ReplayInf& item(unsigned i){return *reinterpret_cast<th20::source::replay::ReplayInf*>(storage.data()+i*sizeof(th20::source::replay::ReplayInf));}
        static void __fastcall read(void* worker,void*,void* callback,ti::TitleInf** target){auto& o=**target;event(81,reinterpret_cast<unsigned>(worker));event(82,unsigned(reinterpret_cast<std::uintptr_t>(callback)==mapped_image_base+0x11fd60));event(83,reinterpret_cast<unsigned>(&o));if(current()->finish){for(unsigned i=0;i<75;++i)o.metadata[i]=i%3?&current()->item(i):nullptr;o.ui_flags=(o.ui_flags|8u)&~4u;}}
        static void* __cdecl effect_owner(int index){event(84,index);return current()->effect.data();}
        static void __fastcall loading(void*,void*,float x,float y){event(85,std::bit_cast<unsigned>(x));event(86,std::bit_cast<unsigned>(y));}
        static unsigned* __fastcall spawn(void*,void*,unsigned* out,int type,void* a,void* b){event(87,type);event(88,a==nullptr);event(89,b==nullptr);*out=2001;return out;}
        static void __fastcall interrupt(unsigned* handle,void*,int id){event(90,*handle);event(91,id);}
        static void __cdecl fade(float time){event(92,std::bit_cast<unsigned>(time));}
        static void __fastcall stage(gs::Session* s,void*,int id){event(93,id);s->player_table.fields_1ec[2]=id;}
        static void __cdecl retire(void* value){event(94,reinterpret_cast<unsigned>(value));}
    } replay_trace;ReplayTrace::current()=&replay_trace;
    struct ReplayHost final:ti::ReplayMenuEnvironment {
        explicit ReplayHost(SelectionHost& s):ReplayMenuEnvironment(s,at<int>(0x5c612c)){}
        void begin_read(ti::TitleInf& o)override{auto* pointer=&o;ReplayTrace::read(&o.worker,nullptr,reinterpret_cast<void*>(mapped_image_base+0x11fd60),&pointer);}
        bool effects_ready()override{return static_cast<unsigned*>(ReplayTrace::effect_owner(0))[0x38/4]!=0;}
        void loading_transition()override{auto* owner=ReplayTrace::effect_owner(0);unsigned out;ReplayTrace::spawn(owner,nullptr,&out,0,nullptr,nullptr);at<unsigned>(0x5c4f04)=out;ReplayTrace::interrupt(&at<unsigned>(0x5c4f04),nullptr,7);ReplayTrace::loading(nullptr,nullptr,480.f,392.f);}
        void fade(float f)override{ReplayTrace::fade(f);}
        void request_start(int stage,const char* filename)override{ReplayTrace::stage(&selection.main.session,nullptr,stage);at<int>(0x5c584c)=13;strcpy_s(reinterpret_cast<char*>(mapped_image_base+0x1c4b20),256,filename);}
        void retire(th20::source::runtime::CallbackOwner* value)override{ReplayTrace::retire(value);}
    } replay_host(selection_host);
    for(auto [va,target]:hooks)hook(va,target);for(auto [va,target]:selection_hooks)hook(va,target);
    const std::pair<unsigned,void*> replay_hooks[]={{0x40b1d0,&ReplayTrace::read},{0x437520,&ReplayTrace::effect_owner},{0x4a0aa0,&ReplayTrace::loading},{0x49dcf0,&ReplayTrace::spawn},{0x44ef90,&ReplayTrace::interrupt},{0x4d99d0,&ReplayTrace::fade},{0x4be360,&ReplayTrace::stage},{0x4217c0,&ReplayTrace::retire}};
    for(auto [va,target]:replay_hooks)hook(va,target);
    new(&object.cursor108)mn::Cursor;object.cursor108.excluded.reserve(16);int page_history[2][4]{};int* page_blocks[2][8]{};page_blocks[0][0]=page_history[0];page_blocks[1][0]=page_history[1];for(unsigned i=0;i<2;++i){auto& d=i?object.cursor108.secondary_history:object.cursor108.history;d.blocks=page_blocks[i];d.block_count=8;d.first=0;d.size=0;}
    const unsigned globals[]{0x5c612c,0x5c6128,0x5c584c,0x5c4f04};
    for(unsigned test=0;test<32768;++test){
        for(unsigned i=0;i<sizeof(object);++i)if((i<0x24||i>=0x70)&&(i<0x108||i>=0x154))storage[i]=std::uint8_t(random());
        object.state=12;object.phase=test%7;auto& c=object.cursor;auto& p=object.cursor108;c.count=object.phase==4?7:25;c.current=(test/7)%c.count;c.previous=random();c.minimum=0;c.wrapping=1;c.excluded.clear();c.history.first=c.secondary_history.first=0;c.history.size=c.secondary_history.size=1;history[0][0]=0;history[1][0]=10;
        p.count=3;p.current=(test/175)%3;p.previous=random();p.minimum=0;p.wrapping=1;p.excluded.clear();p.history.size=p.secondary_history.size=0;p.history.first=p.secondary_history.first=0;
        for(unsigned i=0;i<100;++i){auto& value=replay_trace.item(i);value.user=reinterpret_cast<th20::source::replay::UserHeader*>(replay_trace.users.data()+i*256);auto& u=*value.user;u.flags=static_cast<unsigned char>((test/13)%4);u.fields_d0[2]=(test/17)%2;u.difficulty=(test/23)%5;u.spell=int((test/31)%10030)-10;for(auto& stone:u.stones)stone=random()%9;for(unsigned stage=0;stage<8;++stage)value.playback[stage].stage=stage==1||random()%2?reinterpret_cast<th20::source::replay::StageRecord*>(replay_trace.stages.data()+stage*sizeof(th20::source::replay::StageRecord)):nullptr;sprintf_s(value.filename,"th20_ud%04u.rpy",i);object.metadata[i]=object.phase==2&&i%3==0?nullptr:&value;}
        object.words5734[1]=(test/11)%75;object.words5734[2]=(test/5)%7;object.ui_flags=(test/77)%16;object.handle390=test%3?4001:0;replay_trace.effect[0x38/4]=(test/9)%2;replay_trace.finish=(test/33)%2;
        th20::recovered::timer_set(object.age,int((test/49)%45)-2);host.session.mode=(test/50)%3;for(auto va:globals)at<unsigned>(va)=random();at<int>(0x5c612c)=(test/29)%75;at<unsigned>(0x5b88c0)=random()&0x801ff;at<unsigned>(0x5b88b8)=random()&0xf0;
        const auto before_session=host.session;std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));int before_history[2][4];std::memcpy(before_history,history,sizeof(history));int before_excluded[16];std::memcpy(before_excluded,c.excluded.data(),64);std::array<unsigned,4> before_globals;for(unsigned i=0;i<4;++i)before_globals[i]=at<unsigned>(globals[i]);char before_filename[256];std::memcpy(before_filename,reinterpret_cast<void*>(mapped_image_base+0x1c4b20),256);
        FloatingEnvironment::prepare();trace.clear();const int expected_return=cpu<int>(0x523440,&object);const auto expected_trace=trace;const auto expected_session=host.session;std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));int expected_history[2][4];std::memcpy(expected_history,history,sizeof(history));int expected_excluded[16];std::memcpy(expected_excluded,c.excluded.data(),64);std::array<unsigned,4> expected_globals;for(unsigned i=0;i<4;++i)expected_globals[i]=at<unsigned>(globals[i]);char expected_filename[256];std::memcpy(expected_filename,reinterpret_cast<void*>(mapped_image_base+0x1c4b20),256);
        host.session=before_session;std::memcpy(&object,before.data(),sizeof(object));std::memcpy(history,before_history,sizeof(history));std::memcpy(c.excluded.data(),before_excluded,64);for(unsigned i=0;i<4;++i)at<unsigned>(globals[i])=before_globals[i];std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1c4b20),before_filename,256);FloatingEnvironment::prepare();trace.clear();const int actual_return=ti::update_replay_menu(object,replay_host);std::array<unsigned,4> actual_globals;for(unsigned i=0;i<4;++i)actual_globals[i]=at<unsigned>(globals[i]);
        check("replay_object",test,expected.data(),&object,sizeof(object));check("replay_session",test,&expected_session,&host.session,sizeof(host.session));check("replay_history",test,expected_history,history,sizeof(history));check("replay_excluded",test,expected_excluded,c.excluded.data(),64);check("replay_globals",test,expected_globals.data(),actual_globals.data(),sizeof(expected_globals));check("replay_filename",test,expected_filename,reinterpret_cast<void*>(mapped_image_base+0x1c4b20),256);check("replay_return",test,&expected_return,&actual_return,4);const bool yes=true,same=expected_trace==trace;check("replay_endpoint_trace",test,&yes,&same,1);
    }
}
