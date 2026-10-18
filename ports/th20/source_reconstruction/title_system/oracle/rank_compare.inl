{
    struct RankTrace {
        th20::source::progress::Profile profile{};th20::source::progress::Metadata metadata{};int rank=-1;
        std::vector<std::string> names;
        static RankTrace*& current(){static RankTrace* value=nullptr;return value;}
        static void* __fastcall get_profile(void*,void*,int character,int index){event(100,character);event(101,index);return &current()->profile;}
        static void* __fastcall current_profile(void*,void*){return &current()->profile;}
        static int __fastcall insert_score(void*,void*){event(102);return current()->rank;}
        static void* __fastcall get_metadata(void*,void*){return &current()->metadata;}
        static void __cdecl play(int index,const char* name){event(103,index);current()->names.emplace_back(name);}
        static void __cdecl start(int index,int track){event(104,index);event(105,track);}
        static void __fastcall open(void*,void*,int mode){event(106,mode);}
        static void __fastcall select(void*,void*,int index){event(107,index);}
        static void __fastcall hide(void*,void*){event(108);}
        static void __fastcall stage(gs::Session* s,void*,int stage){event(109,stage);s->player_table.fields_1ec[2]=stage;}
        static void __cdecl retire(void* value){event(110,reinterpret_cast<unsigned>(value));}
    } rank_trace;RankTrace::current()=&rank_trace;
    struct RankHost final:ti::RankEnvironment {
        explicit RankHost(SelectionHost& s):RankEnvironment(s){}
        void play_score_music()override{RankTrace::play(0,"th128_08");RankTrace::start(0,17);}
        void play_title_music()override{RankTrace::play(0,"th20_01");RankTrace::start(0,0);}
        void open_stones()override{RankTrace::open(nullptr,nullptr,2);}
        void select_stone(int index)override{RankTrace::select(nullptr,nullptr,index);}
        void hide_stones()override{RankTrace::hide(nullptr,nullptr);}
        int insert_score()override{auto& s=selection.main.session;auto& p=*s.contexts[0].current_player;RankTrace::get_profile(nullptr,nullptr,p.fields_00[2],p.fields_00[3]);return RankTrace::insert_score(nullptr,nullptr);}
        th20::source::progress::Profile& profile()override{return RankTrace::current()->profile;}
        char* default_name()override{return reinterpret_cast<char*>(RankTrace::current()->metadata.bytes+12);}
        void select_stage_zero()override{RankTrace::stage(&selection.main.session,nullptr,0);}
        void retire_replay()override{RankTrace::retire(at<void*>(0x5c60fc));}
    } rank_host(selection_host);
    for(auto [va,target]:hooks)hook(va,target);for(auto [va,target]:selection_hooks)hook(va,target);
    const std::pair<unsigned,void*> rank_hooks[]={{0x4bd460,&RankTrace::get_profile},{0x50fc50,&RankTrace::current_profile},{0x50f170,&RankTrace::insert_score},{0x4640e0,&RankTrace::get_metadata},{0x4d9a70,&RankTrace::play},{0x4d9b50,&RankTrace::start},{0x51a0b0,&RankTrace::open},{0x52ce80,&RankTrace::select},{0x519840,&RankTrace::hide},{0x4be360,&RankTrace::stage},{0x4217c0,&RankTrace::retire}};
    for(auto [va,target]:rank_hooks)hook(va,target);
    new(&object.cursor56e8)mn::Cursor;object.cursor56e8.excluded.reserve(16);int name_history[2][4]{};int* name_blocks[2][8]{};name_blocks[0][0]=name_history[0];name_blocks[1][0]=name_history[1];for(unsigned i=0;i<2;++i){auto& d=i?object.cursor56e8.secondary_history:object.cursor56e8.history;d.blocks=name_blocks[i];d.block_count=8;d.first=0;d.size=0;}
    for(unsigned test=0;test<32768;++test){
        for(unsigned i=0;i<sizeof(object);++i)if((i<0x24||i>=0x70)&&(i<0x56e8||i>=0x5734))storage[i]=std::uint8_t(random());
        object.state=15;object.phase=test%5;auto& c=object.cursor;auto& n=object.cursor56e8;c.count=30;c.current=(test/5)%10;c.previous=random();c.minimum=0;c.wrapping=1;c.excluded.clear();c.history.first=c.secondary_history.first=0;c.history.size=c.secondary_history.size=1;history[0][0]=0;history[1][0]=10;
        n.count=int(std::strlen(th20::source::pause::name_characters()));n.current=(test/7)%n.count;n.previous=random();n.minimum=0;n.wrapping=1;n.excluded.clear();n.history.size=n.secondary_history.size=0;n.history.first=n.secondary_history.first=0;
        object.word56e0=(test/11)%9;object.word56e4=(test/99)%2;object.handle390=test%3?4001:0;std::memcpy(ti::entered_name(object),"ABCDEFGH",9);rank_trace.rank=int((test/13)%11)-1;std::memcpy(rank_trace.metadata.bytes+12,test%2?"PLAYER  ":"        ",9);
        th20::recovered::timer_set(object.age,int((test/25)%12)-2);host.session.player_table.field_1e0=(test/10)%5;host.session.contexts[0].current_player->fields_00[2]=test%2;host.session.contexts[0].current_player->fields_00[3]=(test/19)%9;host.session.player_table.continue_count=int((test/51)%14)-2;at<void*>(0x5c60fc)=reinterpret_cast<void*>(0x1234abcd);at<unsigned>(0x5b88c0)=random()&0x801ff;at<unsigned>(0x5b88b8)=random()&0xf0;
        const auto session_before=host.session;const auto profile_before=rank_trace.profile;const auto metadata_before=rank_trace.metadata;std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));int history_before[2][4];std::memcpy(history_before,history,sizeof(history));
        FloatingEnvironment::prepare();trace.clear();rank_trace.names.clear();const int expected_return=cpu<int>(0x5223e0,&object);const auto expected_trace=trace;const auto expected_names=rank_trace.names;const auto expected_session=host.session;const auto expected_profile=rank_trace.profile;const auto expected_metadata=rank_trace.metadata;std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));int expected_history[2][4];std::memcpy(expected_history,history,sizeof(history));
        host.session=session_before;rank_trace.profile=profile_before;rank_trace.metadata=metadata_before;std::memcpy(&object,before.data(),sizeof(object));std::memcpy(history,history_before,sizeof(history));FloatingEnvironment::prepare();trace.clear();rank_trace.names.clear();const int actual_return=ti::update_rank_entry(object,rank_host);
        check("rank_object",test,expected.data(),&object,sizeof(object));check("rank_session",test,&expected_session,&host.session,sizeof(host.session));check("rank_profile",test,&expected_profile,&rank_trace.profile,sizeof(expected_profile));check("rank_metadata",test,&expected_metadata,&rank_trace.metadata,sizeof(expected_metadata));check("rank_cursor_history",test,expected_history,history,sizeof(history));check("rank_return",test,&expected_return,&actual_return,4);const bool yes=true,same=expected_trace==trace,same_names=expected_names==rank_trace.names;check("rank_endpoint_trace",test,&yes,&same,1);check("rank_music_names",test,&yes,&same_names,1);
    }
}
