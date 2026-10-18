{
    namespace sp=th20::source::sprite;namespace gs=th20::source::game_session;namespace st=th20::source::state;
    struct DropCalls {
        std::vector<std::array<unsigned,11>> rows;
        static DropCalls*& current(){static DropCalls* p=nullptr;return p;}
        static void* __fastcall spawn(void* self,void*,int type,const sp::Vec3* p,unsigned color,float angle,float speed,int delay,int extra,int sound){
            current()->rows.push_back({reinterpret_cast<unsigned>(self),unsigned(type),std::bit_cast<unsigned>(p->x),std::bit_cast<unsigned>(p->y),std::bit_cast<unsigned>(p->z),color,std::bit_cast<unsigned>(angle),std::bit_cast<unsigned>(speed),unsigned(delay),unsigned(extra),unsigned(sound)});return nullptr;
        }
    } calls;DropCalls::current()=&calls;
    struct Patch {std::uint8_t* at;std::array<std::uint8_t,5> before;Patch(unsigned va,void* fn):at(reinterpret_cast<std::uint8_t*>(mapped_image_base+va-0x400000)){std::memcpy(before.data(),at,5);DWORD old;VirtualProtect(at,5,PAGE_EXECUTE_READWRITE,&old);at[0]=0xe9;const auto delta=unsigned(reinterpret_cast<std::uintptr_t>(fn)-reinterpret_cast<std::uintptr_t>(at)-5);std::memcpy(at+1,&delta,4);FlushInstructionCache(GetCurrentProcess(),at,5);}~Patch(){std::memcpy(at,before.data(),5);FlushInstructionCache(GetCurrentProcess(),at,5);}} patch(0x4c3c90,reinterpret_cast<void*>(&DropCalls::spawn));
    auto& global=*reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568);const auto saved_context0=global.contexts[0],saved_context1=global.contexts[1];
    std::array<unsigned,2> item_owners{};for(unsigned i=0;i<2;++i)global.contexts[i].objects_04[2]=&item_owners[i];
    auto& random=*reinterpret_cast<st::Random*>(mapped_image_base+0x1ba4a8);const auto saved_random=random;
    struct DropHost final:gp::EnemyDropServices {
        st::Random* random;gs::Session* session;
        float random_angle() override{return th20::recovered::mul32(st::signed_unit(*random),std::bit_cast<float>(0x40490fdbu));}
        float random_unit() override{return st::unit(*random);}
        void spawn(int player,int type,const sp::Vec3& position) override{DropCalls::spawn(session->contexts[player].objects_04[2],nullptr,type,&position,0xffffffffu,std::bit_cast<float>(0xbfc90fdbu),2.2f,0,0,-1);}
    } host;host.random=&random;host.session=&global;
    const auto finite=[&]{return float(int(rng()%10000)-5000)/32.f;};
    for(unsigned n=0;n<8192;++n){
        gp::EnemyPatternState value;for(auto& word:reinterpret_cast<unsigned(&)[sizeof(value)/4]>(value))word=rng();
        value.fields_00[0]=n%2;value.fields_00[1]=n%5?1+n%15:0;value.fields_00[2]=n%3?1+(n/3)%15:0;
        for(int i=0;i<16;++i){value.fields_00[3+i]=unsigned(int(rng()%5)-2);value.fields_00[19+i]=unsigned(int(rng()%5)-2);}
        value.field_8c=n%7?1+n%120:0;value.timer_90.current=int(n%180)-30;value.field_a0=finite();value.field_a4=finite();
        // Retain finite negative widths, timer boundary cases, zero/negative
        // counts and all16 clearing slots although only15 kinds are emitted.
        const sp::Vec3 origin{finite(),finite(),finite()};random=st::Random(0);random.state=rng();random.last=rng();random.modulus=0x7fffffffu;
        const auto before=value;const auto random_before=random;calls.rows.clear();FloatingEnvironment::prepare();
        if(n&1u)original<void>(0x48bf10,&value,&origin,bool(n&2u));else original<void>(0x4a5350,&value,&origin);
        const auto expected=value;const auto random_expected=random;const auto expected_calls=calls.rows;
        value=before;random=random_before;calls.rows.clear();FloatingEnvironment::prepare();
        if(n&1u)gp::emit_enemy_drop(value,origin,bool(n&2u),host);else gp::emit_enemy_pattern_items(value,origin,host);
        if(calls.rows!=expected_calls&&failed<12){std::cerr<<"Enemy drop case "<<n<<" source count "<<calls.rows.size()<<" native count "<<expected_calls.size()<<'\n';if(calls.rows.size()==expected_calls.size())for(unsigned i=0;i<calls.rows.size();++i)if(calls.rows[i]!=expected_calls[i]){std::cerr<<" first different item "<<i;for(unsigned j=0;j<11;++j)if(calls.rows[i][j]!=expected_calls[i][j])std::cerr<<" word "<<j<<" source="<<std::hex<<calls.rows[i][j]<<" native="<<expected_calls[i][j]<<std::dec;std::cerr<<'\n';break;}}
        check("Enemy_drop_all168bytes",std::memcmp(&value,&expected,sizeof(value))==0);
        check("Enemy_drop_shared_rng_all28bytes",std::memcmp(&random,&random_expected,sizeof(random))==0);
        check("Enemy_drop_every_item_position_type_and_parameters",calls.rows==expected_calls);
    }
    global.contexts[0]=saved_context0;global.contexts[1]=saved_context1;random=saved_random;
}
{
    struct Resource final:std::pmr::memory_resource {
        std::vector<std::array<std::size_t,3>> events;
        void* do_allocate(std::size_t size,std::size_t alignment) override{events.push_back({1,size,alignment});auto* p=std::pmr::new_delete_resource()->allocate(size,alignment);std::memset(p,0xa5,size);return p;}
        void do_deallocate(void* p,std::size_t size,std::size_t alignment) override{events.push_back({2,size,alignment});std::pmr::new_delete_resource()->deallocate(p,size,alignment);}
        bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override{return this==&other;}
    } native_resource,source_resource;
    struct OwnerView {std::array<std::uint8_t,0x88> prefix{};gp::EnemyState state;};
    static_assert(offsetof(OwnerView,state)==0x88);
    for(unsigned series=0;series<256;++series){
        native_resource.events.clear();source_resource.events.clear();
        {
            OwnerView native,source;using Vector=std::pmr::vector<gp::EnemyAuxiliaryRecord>;
            native.state.auxiliary.~Vector();new(&native.state.auxiliary)Vector(&native_resource);
            source.state.auxiliary.~Vector();new(&source.state.auxiliary)Vector(&source_resource);
            for(unsigned step=0;step<48;++step){
                const unsigned index=step<32?(step+series)%32:rng()%32;
                std::string storage;const unsigned size=1+rng()%64;for(unsigned i=1;i<size;++i)storage.push_back(char('A'+rng()%26));const char* name=(step+series)%5?storage.c_str():nullptr;
                const int life=int(rng()%10000)-3000,time=int(rng());
                if((step+series)&1u){original<void>(0x4ab650,&native,index,life,time,name);gp::configure_enemy_phase(source.state,index,life,time,name);}
                else{original<void>(0x4ab780,&native,index,name);gp::configure_enemy_timeout(source.state,index,name);}
                const auto& a=native.state.auxiliary;const auto& b=source.state.auxiliary;
                check("Enemy_phase_all136byte_records",a.size()==b.size()&&std::memcmp(a.data(),b.data(),a.size()*sizeof(gp::EnemyAuxiliaryRecord))==0);
                check("Enemy_phase_vector_capacity_and_PMR_events",a.capacity()==b.capacity()&&native_resource.events==source_resource.events);
            }
        }
        check("Enemy_phase_destructor_PMR_events",native_resource.events==source_resource.events);
    }
}
