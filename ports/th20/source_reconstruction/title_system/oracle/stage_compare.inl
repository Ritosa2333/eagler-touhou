{
    struct StageTrace {
        th20::source::progress::Profile profile{};std::array<unsigned,16> effect{};std::array<unsigned char,256> keys{};bool direct=false;std::vector<unsigned> trace;
        static StageTrace*& current(){static StageTrace* p=nullptr;return p;}
        static void record(std::initializer_list<unsigned> data){auto& out=current()->trace;out.insert(out.end(),data.begin(),data.end());}
        static void __fastcall heading(ti::TitleInf* o,void*,int index){record({1,unsigned(index)});o->handles[index]=1001+index;}
        static void __fastcall retire(ti::TitleInf* o,void*,int index){record({2,unsigned(index)});o->handles[index]=0;}
        static void __fastcall sound(void*,void*,int id,int position){record({3,unsigned(id),unsigned(position)});}
        static void* __fastcall get_profile(void*,void*){return &current()->profile;}
        static int __cdecl keyboard(void* target){std::memcpy(target,current()->keys.data(),256);return current()->direct?1:0;}
        static void __fastcall fade(void*,void*,float time){record({4,std::bit_cast<unsigned>(time)});}
        static void* __cdecl effect_owner(int player){record({5,unsigned(player)});return current()->effect.data();}
        static void __fastcall loading(void*,void*,float x,float y){record({6,std::bit_cast<unsigned>(x),std::bit_cast<unsigned>(y)});}
        static unsigned* __fastcall effect_spawn(void* self,void*,unsigned* out,int type,void* parameters,void* existing){record({7,reinterpret_cast<unsigned>(self),unsigned(type),unsigned(parameters==nullptr),unsigned(existing==nullptr)});*out=2001;return out;}
        static void __fastcall interrupt(unsigned* handle,void*,int event){record({8,*handle,unsigned(event)});}
        static void __fastcall select_stage(gs::Session* s,void*,int stage){record({9,unsigned(stage)});s->player_table.fields_1ec[2]=stage;}
    } stage_trace;StageTrace::current()=&stage_trace;
    struct StageHost final:ti::StageEnvironment {
        StageHost():StageEnvironment(at<gs::Session>(0x5ba568),at<unsigned>(0x5c6128),at<int>(0x5b0a50)){}
        bool pressed(unsigned mask) override{return (at<unsigned>(0x5b88c0)&mask)!=0;}
        bool repeated(unsigned mask) override{return ((at<unsigned>(0x5b88c0)|at<unsigned>(0x5b88b8))&mask)!=0;}
        const th20::source::progress::Profile& profile() override{return StageTrace::current()->profile;}
        int keyboard_number() override{const bool direct=StageTrace::keyboard(reinterpret_cast<void*>(mapped_image_base+0x1c6148))!=0;for(int n=1;n<=9;++n)if(at<unsigned char>(0x5c6148+(direct?1:0x30)+n)&0x80)return n;return 0;}
        void spawn_heading(ti::TitleInf& o) override{StageTrace::heading(&o,nullptr,43);}
        void retire_heading(ti::TitleInf& o) override{StageTrace::retire(&o,nullptr,43);}
        void sound(int id) override{StageTrace::sound(nullptr,nullptr,id,0);}
        void fade(float time) override{StageTrace::fade(nullptr,nullptr,time);}
        bool effects_ready() override{auto* effect=static_cast<unsigned*>(StageTrace::effect_owner(0));return effect[0x38/4]!=0;}
        void loading() override{StageTrace::loading(nullptr,nullptr,480.f,392.f);auto* effect=StageTrace::effect_owner(0);unsigned result;StageTrace::effect_spawn(effect,nullptr,&result,0,nullptr,nullptr);at<unsigned>(0x5c4f04)=result;StageTrace::interrupt(&at<unsigned>(0x5c4f04),nullptr,7);}
        void request_start(int stage) override{StageTrace::select_stage(&session,nullptr,stage);at<int>(0x5afcfc)=-1;at<int>(0x5c584c)=7;}
    } stage_host;
    const std::pair<unsigned,void*> stage_hooks[]={{0x52cf80,&StageTrace::heading},{0x52cc30,&StageTrace::retire},{0x426d70,&StageTrace::sound},{0x50fc50,&StageTrace::get_profile},{0x51f310,&StageTrace::keyboard},{0x4d99d0,&StageTrace::fade},{0x437520,&StageTrace::effect_owner},{0x4a0aa0,&StageTrace::loading},{0x49dcf0,&StageTrace::effect_spawn},{0x44ef90,&StageTrace::interrupt},{0x4be360,&StageTrace::select_stage}};
    for(auto [va,target]:stage_hooks)hook(va,target);
    const unsigned globals[]{0x5b0a50,0x5c6128,0x5afcfc,0x5c584c,0x5c4f04};
    for(unsigned test=0;test<16384;++test){
        for(unsigned i=0;i<sizeof(object);++i)if(i<0x24||i>=0x70)storage[i]=std::uint8_t(random());
        object.state=8;object.phase=test%6;object.cursor.count=6;object.cursor.current=(test/6)%6;object.cursor.previous=int(random());object.cursor.minimum=0;object.cursor.wrapping=1;object.cursor.excluded.clear();
        object.cursor.history.first=object.cursor.secondary_history.first=0;object.cursor.history.size=object.cursor.secondary_history.size=1;history[0][0]=test%2;history[1][0]=8;
        th20::recovered::timer_set(object.age,int((test/36)%50)-2);stage_host.session.player_table.field_1e0=test%5;
        for(auto va:globals)at<unsigned>(va)=random();stage_host.menu_selection=(test/7)%7;stage_host.last_stage=int((test/11)%8)-1;
        stage_trace.effect[0x38/4]=(test/9)%2;stage_trace.direct=(test/13)%2;stage_trace.keys.fill(0);if(test%10)stage_trace.keys[(stage_trace.direct?1:0x30)+test%10]=0x80;if(test%17==0)stage_trace.keys[3]=0x80;
        for(int d=0;d<5;++d)for(int i=0;i<9;++i){stage_trace.profile.bytes[0x76f8+d*0x90+i*16+8]=random()%2;stage_trace.profile.bytes[0x76f8+d*0x90+i*16+9]=random()%2;}
        at<unsigned>(0x5b88c0)=random()&0x801ff;at<unsigned>(0x5b88b8)=random()&0xf0;
        std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));const auto session_before=stage_host.session;int history_before[2][4];std::memcpy(history_before,history,sizeof(history));std::array<unsigned,5> global_before;for(unsigned i=0;i<5;++i)global_before[i]=at<unsigned>(globals[i]);
        stage_trace.trace.clear();const int expected_return=cpu<int>(0x529430,&object);const auto expected_trace=stage_trace.trace;const auto expected_session=stage_host.session;std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));int history_expected[2][4];std::memcpy(history_expected,history,sizeof(history));std::array<unsigned,5> global_expected;for(unsigned i=0;i<5;++i)global_expected[i]=at<unsigned>(globals[i]);
        std::memcpy(&object,before.data(),sizeof(object));stage_host.session=session_before;std::memcpy(history,history_before,sizeof(history));for(unsigned i=0;i<5;++i)at<unsigned>(globals[i])=global_before[i];stage_trace.trace.clear();const int actual_return=ti::update_stage(object,stage_host);std::array<unsigned,5> global_actual;for(unsigned i=0;i<5;++i)global_actual[i]=at<unsigned>(globals[i]);
        check("stage_object",test,expected.data(),&object,sizeof(object));check("stage_session",test,&expected_session,&stage_host.session,sizeof(expected_session));check("stage_cursor_history",test,history_expected,history,sizeof(history));check("stage_global_scene_last_stage_and_effect",test,global_expected.data(),global_actual.data(),20);check("stage_return",test,&expected_return,&actual_return,4);const bool same=stage_trace.trace==expected_trace,yes=true;check("stage_endpoint_trace",test,&yes,&same,sizeof(bool));
    }
}
