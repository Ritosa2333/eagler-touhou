{
    struct SaveTrace {
        th20::source::progress::Metadata metadata{};std::vector<std::string> names;
        static SaveTrace*& current(){static SaveTrace* p=nullptr;return p;}
        static int __cdecl format(char* out,std::size_t capacity,const char* pattern,...){va_list args;va_start(args,pattern);const auto value=vsprintf_s(out,capacity,pattern,args);va_end(args);return value;}
        static void __fastcall stage(gs::Session* s,void*,int stage){event(120,stage);s->player_table.fields_1ec[2]=stage;}
        static th20::source::runtime::CallbackOwner* __cdecl read(const char* path){event(121);current()->names.emplace_back(path);int index=0;sscanf_s(path,"th20_%d.rpy",&index);return reinterpret_cast<th20::source::runtime::CallbackOwner*>(0x20000000u+unsigned(index)*16u);}
        static void __cdecl retire(void* value){event(122,reinterpret_cast<unsigned>(value));}
        static void* __fastcall get_metadata(void*,void*){return &current()->metadata;}
        static int __fastcall prepare(void* value,void*,int mode){event(123,reinterpret_cast<unsigned>(value));event(124,mode);return 0;}
        static void __fastcall save(void* value,void*,const char* path,const char* name,int stage,int mode){event(125,reinterpret_cast<unsigned>(value));event(126,stage);event(127,mode);current()->names.emplace_back(path);current()->names.emplace_back(name);}
        static void __cdecl play(int index,const char* name){event(128,index);current()->names.emplace_back(name);}
        static void __cdecl start(int index,int track){event(129,index);event(130,track);}
    } save_trace;SaveTrace::current()=&save_trace;
    struct SaveHost final:ti::ReplaySaveEnvironment {
        explicit SaveHost(SelectionHost& s):ReplaySaveEnvironment(s){}
        void select_stage_eight()override{SaveTrace::stage(&selection.main.session,nullptr,8);}
        th20::source::runtime::CallbackOwner* read_metadata(const char* filename)override{return SaveTrace::read(filename);}
        void retire(th20::source::runtime::CallbackOwner* value)override{SaveTrace::retire(value);}
        void retire_replay()override{SaveTrace::retire(at<void*>(0x5c60fc));}
        void prepare_save()override{SaveTrace::prepare(at<void*>(0x5c60fc),nullptr,1);}
        char* default_name()override{return reinterpret_cast<char*>(SaveTrace::current()->metadata.bytes+12);}
        void save(const char* path,const char* name)override{SaveTrace::save(at<void*>(0x5c60fc),nullptr,path,name,1,0);}
        void play_title_music()override{SaveTrace::play(0,"th20_01");SaveTrace::start(0,0);}
    } save_host(selection_host);
    for(auto [va,target]:hooks)hook(va,target);for(auto [va,target]:selection_hooks)hook(va,target);
    const std::pair<unsigned,void*> save_hooks[]={{0x41f220,&SaveTrace::format},{0x4be360,&SaveTrace::stage},{0x50a8e0,&SaveTrace::read},{0x4217c0,&SaveTrace::retire},{0x4640e0,&SaveTrace::get_metadata},{0x508310,&SaveTrace::prepare},{0x509280,&SaveTrace::save},{0x4d9a70,&SaveTrace::play},{0x4d9b50,&SaveTrace::start}};for(auto [va,target]:save_hooks)hook(va,target);
    for(unsigned test=0;test<32768;++test){
        for(unsigned i=0;i<sizeof(object);++i)if((i<0x24||i>=0x70)&&(i<0x56e8||i>=0x5734))storage[i]=std::uint8_t(random());
        object.state=16;object.phase=test%6;auto& c=object.cursor;auto& n=object.cursor56e8;c.count=25;c.current=(test/6)%25;c.previous=random();c.minimum=0;c.wrapping=1;c.excluded.clear();c.history.first=c.secondary_history.first=0;c.history.size=c.secondary_history.size=1;history[0][0]=0;history[1][0]=10;
        n.count=int(std::strlen(th20::source::pause::name_characters()));n.current=(test/7)%n.count;n.previous=random();n.minimum=0;n.wrapping=1;n.excluded.clear();n.history.size=n.secondary_history.size=0;n.history.first=n.secondary_history.first=0;
        object.word56e0=(test/11)%9;object.handle390=test%3?4001:0;std::memcpy(ti::entered_name(object),"ABCDEFGH",9);std::memcpy(save_trace.metadata.bytes+12,test%2?"PLAYER  ":"        ",9);for(unsigned i=0;i<100;++i)object.metadata[i]=i%3?reinterpret_cast<th20::source::runtime::CallbackOwner*>(0x20000000u+i*16u):nullptr;
        th20::recovered::timer_set(object.age,int((test/25)%12)-2);at<void*>(0x5c60fc)=reinterpret_cast<void*>(0x1234abcd);at<unsigned>(0x5b88c0)=random()&0x801ff;at<unsigned>(0x5b88b8)=random()&0xf0;
        const auto session_before=host.session;const auto metadata_before=save_trace.metadata;std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));int history_before[2][4];std::memcpy(history_before,history,sizeof(history));
        FloatingEnvironment::prepare();trace.clear();save_trace.names.clear();cpu<void>(0x526a90,&object);const auto expected_trace=trace;const auto expected_names=save_trace.names;const auto expected_session=host.session;const auto expected_metadata=save_trace.metadata;std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));int expected_history[2][4];std::memcpy(expected_history,history,sizeof(history));
        host.session=session_before;save_trace.metadata=metadata_before;std::memcpy(&object,before.data(),sizeof(object));std::memcpy(history,history_before,sizeof(history));FloatingEnvironment::prepare();trace.clear();save_trace.names.clear();ti::update_replay_save(object,save_host);
        check("save_object",test,expected.data(),&object,sizeof(object));check("save_session",test,&expected_session,&host.session,sizeof(host.session));check("save_metadata",test,&expected_metadata,&save_trace.metadata,sizeof(expected_metadata));check("save_cursor_history",test,expected_history,history,sizeof(history));const bool yes=true,same=expected_trace==trace,same_names=expected_names==save_trace.names;check("save_endpoint_trace",test,&yes,&same,1);check("save_names",test,&yes,&same_names,1);
    }
}
