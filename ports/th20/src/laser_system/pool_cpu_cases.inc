{
    namespace la=th20::source::laser;namespace pe_state=th20::source::program_entry;namespace gs=th20::source::game_session;
    q::State function_state{};q::initialize_state(function_state);pe_state::function_controller=&function_state;*reinterpret_cast<void**>(mapped_image_base+0x1b66d8)=&function_state;
    for(unsigned test=0;test<256;++test){diagnostic_case=test;diagnostic_phase="Laser complete controller constructor";
        alignas(la::Controller) std::array<std::uint8_t,sizeof(la::Controller)> memory;memory.fill(static_cast<std::uint8_t>(test));cpu<void>(0x4c87c0,memory.data());auto expected=memory;memory.fill(static_cast<std::uint8_t>(test));auto* value=new(memory.data())la::Controller;std::memcpy(expected.data(),memory.data(),4);check("laser_controller_ctor",expected.data(),memory.data(),memory.size());value->~Controller();
    }
    struct Probe final:la::Laser {
        std::int32_t result=0;std::uint32_t calls=0;std::array<std::uint32_t,10> arguments{};
        int update() override {unexpected("laser entity update outside cancel boundary test");}
        int draw() override {unexpected("laser entity draw outside cancel boundary test");}
        int finish() override {unexpected("laser entity finish outside cancel boundary test");}
        int cancel_all(std::int32_t) override {unexpected("laser entity cancel_all outside cancel boundary test");}
        int erase(int first,int second) override {++calls;arguments[0]=first;arguments[1]=second;arguments[9]=3;return result;}
        int cancel_rectangle(const s::Vec3& p,const s::Vec3& size,float angle,std::int32_t mode,std::int32_t kind) override {++calls;std::memcpy(arguments.data(),&p,12);std::memcpy(arguments.data()+3,&size,12);std::memcpy(arguments.data()+6,&angle,4);arguments[7]=mode;arguments[8]=kind;arguments[9]=1;return result;}
        int cancel_circle(const s::Vec3& p,float radius,std::int32_t mode,std::int32_t kind) override {++calls;std::memcpy(arguments.data(),&p,12);std::memcpy(arguments.data()+3,&radius,4);arguments[4]=mode;arguments[5]=kind;arguments[9]=2;return result;}
        static int __fastcall native_rectangle(Probe* self,void*,const s::Vec3* p,const s::Vec3* size,float angle,std::int32_t mode,std::int32_t kind){return self->Probe::cancel_rectangle(*p,*size,angle,mode,kind);}
        static int __fastcall native_erase(Probe* self,void*,int first,int second){return self->Probe::erase(first,second);}
        static int __fastcall native_circle(Probe* self,void*,const s::Vec3* p,float radius,std::int32_t mode,std::int32_t kind){return self->Probe::cancel_circle(*p,radius,mode,kind);}
    };
    void* native_table[34]{};native_table[0x44/4]=reinterpret_cast<void*>(&Probe::native_erase);native_table[0x2c/4]=reinterpret_cast<void*>(&Probe::native_rectangle);native_table[0x30/4]=reinterpret_cast<void*>(&Probe::native_circle);
    la::Controller owner;std::array<Probe,16> probes;void* source_table=*reinterpret_cast<void**>(&probes[0]);
    auto finite=[&]{return float(int(rng()%1000)-500)/499.f;};
    for(unsigned test=0;test<1536;++test){diagnostic_case=test;diagnostic_phase="Laser cancellation virtual call boundary";q::initialize_list(owner.active);owner.count=0;owner.select_context(test%2);
        for(auto& p:probes){*reinterpret_cast<void**>(&p)=source_table;p.field_04=rng();p.flags=rng();p.state=int(rng()%4);p.field_24=rng();p.age={1,2,2.5f,rng()};p.result=int(rng()%15)-7;p.calls=0;p.arguments.fill(0);owner.attach(p);}
        const s::Vec3 center{finite(),finite(),finite()},size{finite(),finite(),finite()};const float scalar=finite();const int mode=int(rng()),kind=int(rng());
        const auto initial_owner=*reinterpret_cast<std::array<std::uint8_t,sizeof(owner)>*>(&owner);const auto initial_probes=*reinterpret_cast<std::array<std::uint8_t,sizeof(probes)>*>(&probes);
        for(auto& p:probes)*reinterpret_cast<void**>(&p)=native_table;
        const auto expected_return=test%3==2?(cpu<void>(0x4c9490,&owner,mode,kind),0):test%3==1?cpu<int>(0x4c9db0,&owner,&center,&size,scalar,mode,kind):cpu<int>(0x4caad0,&owner,&center,scalar,mode,kind);
        const auto expected_owner=*reinterpret_cast<std::array<std::uint8_t,sizeof(owner)>*>(&owner);for(auto& p:probes)*reinterpret_cast<void**>(&p)=source_table;const auto expected_probes=*reinterpret_cast<std::array<std::uint8_t,sizeof(probes)>*>(&probes);
        std::memcpy(&owner,initial_owner.data(),initial_owner.size());std::memcpy(&probes,initial_probes.data(),initial_probes.size());const auto actual_return=test%3==2?(owner.erase_all(mode,kind),0):test%3==1?owner.cancel_rectangle(center,size,scalar,mode,kind):owner.cancel_circle(center,scalar,mode,kind);
        const auto label=test%3==2?"laser_erase_boundary":test%3==1?"laser_rectangle_boundary":"laser_circle_boundary";check(std::string(label)+"_owner",expected_owner.data(),&owner,sizeof(owner));check(std::string(label)+"_entities",expected_probes.data(),&probes,sizeof(probes));check(std::string(label)+"_return",&expected_return,&actual_return,4);
        for(auto& p:probes)owner.detach(p);
    }
    // Entity methods above are explicit trace fixtures, not recovered lasers.
    #include "type3_cpu_cases.inc"
    #include "type1_cpu_cases.inc"
    #include "type0_cpu_cases.inc"
    #include "type2_path_cpu_cases.inc"
    #include "type2_cpu_cases.inc"
}
