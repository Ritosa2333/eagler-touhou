{
    namespace rt=th20::source::runtime;namespace sc=th20::source::scheduler;namespace sp=th20::source::sprite;
    struct LifecycleTrace {
        ti::TitleInf* object=nullptr;std::array<unsigned char,sizeof(ti::TitleInf)> teardown{};
        sc::Node nodes[2]{};unsigned fail_slot=0;
        static LifecycleTrace*& current(){static LifecycleTrace* p=nullptr;return p;}
        static unsigned text_id(const char* p){unsigned h=2166136261u;while(*p)h=(h^static_cast<unsigned char>(*p++))*16777619u;return h;}
        static sc::Node* record_registration(int priority,bool draw,void* object){event(301,unsigned(priority));event(302,draw);event(303,object==current()->object);return current()->nodes+(draw?1:0);}
        static sc::Node* __cdecl update(int priority,unsigned callback,void* o){if(callback!=mapped_image_base+0x12c1e0)throw std::logic_error("Unexpected Title update callback");return record_registration(priority,false,o);}
        static sc::Node* __cdecl draw(int priority,unsigned callback,void* o){if(callback!=mapped_image_base+0x12c210)throw std::logic_error("Unexpected Title draw callback");return record_registration(priority,true,o);}
        static sp::AnimationFile* __fastcall load(void*,void*,int slot,const char* name){event(304,unsigned(slot));event(305,text_id(name));return unsigned(slot)==current()->fail_slot?nullptr:reinterpret_cast<sp::AnimationFile*>(0x12340000u+unsigned(slot)*16);}
        static void __cdecl error(void*,const char* text){event(306,text_id(text));}
        static void __fastcall remove(void*,void*,sc::Node* n){event(307,unsigned(n));}
        static void __fastcall unload(void*,void*,int slot){event(308,unsigned(slot));}
        static void __cdecl retire(void* p){event(309,unsigned(p));}
        static void __fastcall interrupt(unsigned* h,void*){event(310,*h);}
        static void __fastcall mesh(void*,void*,void* p){event(311,unsigned(p));}
        static void __fastcall rebuild(void*,void*){event(312);}
        static void __fastcall worker(rt::Worker* p,void*){
            // Capture the complete destructor body before C++ member lifetimes
            // end. Native Cursor destructors still execute with real heap data.
            std::memcpy(current()->teardown.data(),current()->object,sizeof(ti::TitleInf));p->~Worker();
        }
    } data;LifecycleTrace::current()=&data;
    struct LifecycleHost final:ti::LifecycleEnvironment {
        void publish(ti::TitleInf* o)override{at<void*>(0x5c6124)=o;if(!o)std::memcpy(LifecycleTrace::current()->teardown.data(),LifecycleTrace::current()->object,sizeof(ti::TitleInf));}
        sc::Node* register_callback(ti::TitleInf& o,int p,bool d)override{return LifecycleTrace::record_registration(p,d,&o);}
        sp::AnimationFile* load_file(int slot,const char* name)override{return LifecycleTrace::load(nullptr,nullptr,slot,name);}
        void load_error()override{LifecycleTrace::error(nullptr,reinterpret_cast<const char*>(mapped_image_base+0x16f610));}
        void remove(sc::Node* p)override{LifecycleTrace::remove(nullptr,nullptr,p);}
        void unload_file(int slot)override{LifecycleTrace::unload(nullptr,nullptr,slot);}
        void retire(rt::CallbackOwner* p)override{LifecycleTrace::retire(p);}
        void interrupt(unsigned h)override{LifecycleTrace::interrupt(&h,nullptr);}
        void release_mesh(sp::RenderMesh* p)override{LifecycleTrace::mesh(nullptr,nullptr,p);}
        void rebuild_input()override{LifecycleTrace::rebuild(nullptr,nullptr);}
    } lifecycle_host;ti::oracle_lifecycle_environment=&lifecycle_host;
    const auto pe=th20::parse_pe(bytes);
    for(const auto& item:pe.imports)if(auto proc=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),item.name.c_str()))*reinterpret_cast<FARPROC*>(mapped_image_base+item.iat_rva)=proc;
    at<HANDLE>(0x5e5990)=GetProcessHeap();cpu<void>(0x452e00,&at<unsigned>(0x5c0240));
    const std::pair<unsigned,void*> own_hooks[]{{0x412310,&LifecycleTrace::update},{0x4123b0,&LifecycleTrace::draw},{0x44ee50,&LifecycleTrace::load},{0x454150,&LifecycleTrace::error},{0x4124b0,&LifecycleTrace::remove},{0x44c430,&LifecycleTrace::unload},{0x4217c0,&LifecycleTrace::retire},{0x479040,&LifecycleTrace::interrupt},{0x4a2800,&LifecycleTrace::mesh},{0x420d80,&LifecycleTrace::rebuild},{0x40b980,&LifecycleTrace::worker}};
    for(auto [va,target]:own_hooks)hook(va,target);
    // This is an actual Title allocation with independently owned Cursor
    // iterator proxies. Pointer identities differ across native/source heaps;
    // normalize only those identities and the polymorphic vptr, never fields.
    alignas(ti::TitleInf) std::array<unsigned char,sizeof(ti::TitleInf)> raw;
    auto normalize=[&](std::array<unsigned char,sizeof(ti::TitleInf)>& e,const ti::TitleInf& a){
        const auto* p=reinterpret_cast<const unsigned char*>(&a);std::memcpy(e.data(),p,4);
        for(unsigned offset:{0x24u,0x70u,0xbcu,0x108u,0x56e8u})for(unsigned member:{0x10u,0x20u,0x34u})std::memcpy(e.data()+offset+member,p+offset+member,4);
    };
    auto proxies_valid=[](const ti::TitleInf& o){for(const auto* c:{&o.cursor,&o.cursor70,&o.cursorbc,&o.cursor108,&o.cursor56e8})for(const auto* d:{&c->history,&c->secondary_history})if(!d->proxy||d->proxy->container!=d||d->proxy->first_iterator)return false;return true;};
    auto setup_destructor=[&](ti::TitleInf& o,unsigned test){o.update_node=&data.nodes[0];o.draw_node=&data.nodes[1];o.handle390=test%2?0:0x13579u;o.mesh=test%3?reinterpret_cast<sp::RenderMesh*>(0x11112222):nullptr;for(unsigned i=0;i<100;++i)o.metadata[i]=(i+test)%3?nullptr:reinterpret_cast<rt::CallbackOwner*>(0x20000000u+i*32);};
    for(unsigned test=0;test<1024;++test){
        const unsigned char fill=test%2?0xa5:0;raw.fill(fill);data.object=reinterpret_cast<ti::TitleInf*>(raw.data());
        trace.clear();cpu<void>(0x51db00,data.object);auto expected=raw;const bool ep=proxies_valid(*data.object),yes=true;check("title_constructor_native_proxies",test,&yes,&ep,1);const bool published_native=at<void*>(0x5c6124)==data.object;check("title_constructor_native_global",test,&yes,&published_native,1);
        setup_destructor(*data.object,test);trace.clear();cpu<void>(0x51dea0,data.object);const auto et=trace;auto expected_body=data.teardown;const auto eg=at<void*>(0x5c6124);
        raw.fill(fill);::new(raw.data())ti::TitleInf;normalize(expected,*data.object);check("title_constructor_fields",test,expected.data(),raw.data(),raw.size());const bool ap=proxies_valid(*data.object);check("title_constructor_source_proxies",test,&yes,&ap,1);const bool published_source=at<void*>(0x5c6124)==data.object;check("title_constructor_source_global",test,&yes,&published_source,1);
        setup_destructor(*data.object,test);trace.clear();data.object->~TitleInf();auto source_body=data.teardown;normalize(expected_body,*reinterpret_cast<const ti::TitleInf*>(source_body.data()));check("title_destructor_body",test,expected_body.data(),source_body.data(),source_body.size());const bool same=et==trace;check("title_destructor_endpoint_trace",test,&yes,&same,1);const auto ag=at<void*>(0x5c6124);check("title_destructor_global",test,&eg,&ag,4);
        if(!same&&failures.size()<30){std::ostringstream msg;msg<<"lifecycle trace test="<<test<<" expected:";for(auto v:et)msg<<' '<<v;msg<<" actual:";for(auto v:trace)msg<<' '<<v;failures.push_back(msg.str());}
    }
    for(unsigned test=0;test<1536;++test){for(auto& b:raw)b=static_cast<unsigned char>(random());data.object=reinterpret_cast<ti::TitleInf*>(raw.data());data.fail_slot=test%3==0?11:test%3==1?12:0;const auto before=raw;trace.clear();const int er=cpu<int>(0x51f3c0,data.object);const auto expected=raw;const auto et=trace;raw=before;trace.clear();const int ar=ti::initialize(*data.object,lifecycle_host);check("title_initialize_fields",test,expected.data(),raw.data(),raw.size());check("title_initialize_return",test,&er,&ar,4);const bool same=et==trace,yes=true;check("title_initialize_endpoint_trace",test,&yes,&same,1);}
    ti::oracle_lifecycle_environment=nullptr;
}
