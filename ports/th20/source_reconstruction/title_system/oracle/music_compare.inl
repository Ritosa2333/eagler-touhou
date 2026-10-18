{
    struct MusicTrace {
        std::vector<unsigned> values;std::vector<std::string> names;th20::source::progress::Metadata metadata{};bool finish=false;
        static MusicTrace*& current(){static MusicTrace* p=nullptr;return p;}
        static void record(std::initializer_list<unsigned> data){auto& out=current()->values;out.insert(out.end(),data.begin(),data.end());}
        static void __fastcall heading(ti::TitleInf* o,void*,int index){record({1,unsigned(index)});o->handles[index]=unsigned(index)+1001;}
        static void __fastcall retire(ti::TitleInf* o,void*,int index){record({2,unsigned(index)});o->handles[index]=0;}
        static unsigned* __fastcall background(void* file,void*,unsigned* out,const char* name,int script,int layer,void* output){record({3,reinterpret_cast<unsigned>(file),unsigned(name==nullptr),unsigned(script),unsigned(layer),unsigned(output==nullptr)});*out=2001;return out;}
        static void* __fastcall file(void*,void*){return reinterpret_cast<void*>(0x1000);}
        static void __fastcall interrupt(unsigned* handle,void*,int e){record({4,*handle,unsigned(e)});}
        static void __fastcall interrupt1(unsigned* handle,void*){interrupt(handle,nullptr,1);}
        static void __fastcall sound(void*,void*,int id,int position){record({5,unsigned(id),unsigned(position)});}
        static void __cdecl play(int index,const char* name){record({6,unsigned(index)});current()->names.emplace_back(name);}
        static void __cdecl start(int index,int unlock){record({7,unsigned(index),unsigned(unlock)});}
        static void __cdecl stop(){record({8});}
        static void __fastcall read(void* worker,void*,ti::TitleInf* o,void* callback,int* argument){record({9,reinterpret_cast<unsigned>(worker),reinterpret_cast<unsigned>(o),unsigned(reinterpret_cast<std::uintptr_t>(callback)==mapped_image_base+0x11f890),unsigned(*argument)});if(current()->finish){o->flag478.store(true);o->cursor.count=22;o->cursor.select(0);o->word56d0=0;o->words47c[0]=22;}}
        static void* __fastcall get_metadata(void*,void*){return &current()->metadata;}
    } music_trace;MusicTrace::current()=&music_trace;
    struct MusicHost final:ti::MusicEnvironment {
        bool pressed(unsigned mask) override{return (at<unsigned>(0x5b88c0)&mask)!=0;}
        bool repeated(unsigned mask) override{return ((at<unsigned>(0x5b88c0)|at<unsigned>(0x5b88b8))&mask)!=0;}
        bool unlocked(int index) override{return MusicTrace::current()->metadata.bytes[0x36+index]!=0;}
        void spawn_background(ti::TitleInf& o) override{MusicTrace::background(reinterpret_cast<void*>(0x1000),nullptr,&o.handle390,nullptr,19,-1,nullptr);}
        void spawn_heading(ti::TitleInf& o) override{MusicTrace::heading(&o,nullptr,40);}
        void retire_heading(ti::TitleInf& o) override{MusicTrace::retire(&o,nullptr,40);}
        void begin_read(ti::TitleInf& o) override{int zero=0;MusicTrace::read(&o.worker,nullptr,&o,reinterpret_cast<void*>(mapped_image_base+0x11f890),&zero);}
        void interrupt(unsigned handle,int e) override{MusicTrace::interrupt(&handle,nullptr,e);}
        void sound(int id) override{MusicTrace::sound(nullptr,nullptr,id,0);}
        void play(const char* name) override{MusicTrace::play(0,name);MusicTrace::start(0,0);}
        void stop() override{MusicTrace::stop();}
    } music_host;
    const std::pair<unsigned,void*> music_hooks[]={{0x52cf80,&MusicTrace::heading},{0x52cc30,&MusicTrace::retire},{0x450c70,&MusicTrace::background},{0x437950,&MusicTrace::file},{0x44ef90,&MusicTrace::interrupt},{0x479040,&MusicTrace::interrupt1},{0x426d70,&MusicTrace::sound},{0x4d9a70,&MusicTrace::play},{0x4d9b50,&MusicTrace::start},{0x4d9bc0,&MusicTrace::stop},{0x50ab10,&MusicTrace::read},{0x4640e0,&MusicTrace::get_metadata}};
    for(auto [va,target]:music_hooks)hook(va,target);
    for(unsigned test=0;test<16384;++test){
        for(unsigned i=0;i<sizeof(object);++i)if(i<0x24||i>=0x70)storage[i]=std::uint8_t(random());
        object.state=14;object.phase=test%5;object.flag478.store((test/5)%2!=0);object.handle390=test%3?0:2001;music_trace.finish=test%2!=0;
        const unsigned count=1+test%32;object.words47c[0]=count;object.words47c[1]=(test/7)%9;object.words47c[2]=(test/11)%count;object.words47c[3]=(test/13)%count;object.words47c[4]=(test/17)%2;object.word56d0=(test/19)%count;
        for(unsigned i=0;i<32;++i){char value[64];sprintf_s(value,"th20_music_%02u",i);std::memcpy(object.data490+i*64,value,std::strlen(value)+1);music_trace.metadata.bytes[0x36+i]=std::uint8_t(random()%2);}
        object.cursor.count=count;object.cursor.current=int((test/23)%count);object.cursor.previous=int(random());object.cursor.minimum=0;object.cursor.wrapping=1;object.cursor.excluded.clear();
        object.cursor.history.first=object.cursor.secondary_history.first=0;object.cursor.history.size=object.cursor.secondary_history.size=1;history[0][0]=6;history[1][0]=10;
        th20::recovered::timer_set(object.age,int((test/31)%32)-1);at<unsigned>(0x5b88c0)=random()&0x801ff;at<unsigned>(0x5b88b8)=random()&0xf0;
        std::array<unsigned char,sizeof(object)> before;std::memcpy(before.data(),&object,sizeof(object));int old_history[2][4];std::memcpy(old_history,history,sizeof(history));
        music_trace.values.clear();music_trace.names.clear();const int expected_return=cpu<int>(0x5205d0,&object);const auto expected_trace=music_trace.values;const auto expected_names=music_trace.names;std::array<unsigned char,sizeof(object)> expected;std::memcpy(expected.data(),&object,sizeof(object));int expected_history[2][4];std::memcpy(expected_history,history,sizeof(history));
        std::memcpy(&object,before.data(),sizeof(object));std::memcpy(history,old_history,sizeof(history));music_trace.values.clear();music_trace.names.clear();const int actual_return=ti::update_music(object,music_host);
        check("music_object",test,expected.data(),&object,sizeof(object));check("music_history",test,expected_history,history,sizeof(history));check("music_return",test,&expected_return,&actual_return,4);
        const bool trace_equal=music_trace.values==expected_trace,names_equal=music_trace.names==expected_names,yes=true;
        check("music_endpoint_trace",test,&yes,&trace_equal,sizeof(bool));check("music_track_names",test,&yes,&names_equal,sizeof(bool));
    }
}
