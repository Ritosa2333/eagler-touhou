{
    namespace gs=th20::source::game_session;namespace sp=th20::source::sprite;
    struct Endpoints {
        std::vector<std::array<unsigned,4>> calls;std::vector<std::string> names;static Endpoints*& current(){static Endpoints* p=nullptr;return p;}
        static void __fastcall clear(gp::Enemy* e,void*){current()->calls.push_back({1,reinterpret_cast<unsigned>(e),0,0});}
        static void __fastcall reset(gp::Enemy* e,void*){current()->calls.push_back({2,reinterpret_cast<unsigned>(e),0,0});}
        static void __fastcall select(gp::Enemy* e,void*,const char* name){current()->calls.push_back({3,reinterpret_cast<unsigned>(e),0,0});current()->names.emplace_back(name);}
        static void __fastcall attach(void* self,void*,unsigned enemy,int color){current()->calls.push_back({4,reinterpret_cast<unsigned>(self),enemy,unsigned(color)});}
    } endpoints;Endpoints::current()=&endpoints;
    struct Patch {std::uint8_t* address;std::array<std::uint8_t,5> before;Patch(unsigned va,void* callback):address(reinterpret_cast<std::uint8_t*>(mapped_image_base+va-0x400000)){std::memcpy(before.data(),address,5);DWORD old;VirtualProtect(address,5,PAGE_EXECUTE_READWRITE,&old);address[0]=0xe9;const auto displacement=unsigned(reinterpret_cast<std::uintptr_t>(callback)-reinterpret_cast<std::uintptr_t>(address)-5);std::memcpy(address+1,&displacement,4);FlushInstructionCache(GetCurrentProcess(),address,5);}~Patch(){std::memcpy(address,before.data(),5);FlushInstructionCache(GetCurrentProcess(),address,5);}}
      clear_patch(0x4973c0,reinterpret_cast<void*>(&Endpoints::clear)),reset_patch(0x4972c0,reinterpret_cast<void*>(&Endpoints::reset)),select_patch(0x540200,reinterpret_cast<void*>(&Endpoints::select)),attach_patch(0x512f60,reinterpret_cast<void*>(&Endpoints::attach));
    struct Host final:gp::EnemyMiscOpcodeServices {
        gp::Enemy* find(gp::EnemyController& c,unsigned id)override{return static_cast<gp::Enemy*>(gp::find_enemy_in_list(c.enemies,id));}gp::Enemy* selected(gp::EnemyController& c,unsigned id)override{return original<gp::Enemy*>(0x485660,&c,id);}
        void select_script(gp::Enemy& e,const char* name)override{Endpoints::clear(&e,nullptr);Endpoints::reset(&e,nullptr);Endpoints::select(&e,nullptr,name);}void attach_stone(unsigned enemy,int color)override{Endpoints::attach(*reinterpret_cast<void**>(mapped_image_base+0x1c6118),nullptr,enemy,color);}
    } host;
    auto& global=*reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568);const auto old_context=global.contexts[0];
    std::array<std::uint8_t,0x134> owner_bytes{};auto& owner=*reinterpret_cast<gp::EnemyController*>(owner_bytes.data());scheduler::initialize_list(owner.enemies);global.contexts[0].objects_04[1]=&owner;
    gp::ScriptLoader scripts(environment);std::vector<std::uint8_t> code(256);scripts.records.push_back({"misc",code.data()});scripts.subroutine_count=1;
    gp::Enemy enemy,targets[3];enemy.context=&global.contexts[0];enemy.reset();enemy.loader=&scripts;enemy.main.subroutine=enemy.main.instruction_offset=0;enemy.main.stack.words.resize(128);enemy.state.entity=&enemy;enemy.state.fields_2c8[9]=reinterpret_cast<unsigned>(&global.contexts[0]);
    for(unsigned i=0;i<3;++i){targets[i].context=&global.contexts[0];targets[i].state.identifier=47+i;targets[i].state.auxiliary.resize(4);scheduler::append(owner.enemies,targets[i].controller_link);}
    const unsigned operations[]{800,801,802,1001,1002,1003};const auto finite=[&](){return float(int(rng()%40000)-20000)/32.f;};
    for(auto op:operations)for(unsigned n=0;n<1024;++n){std::vector<unsigned> args(24);for(auto& value:args)value=rng();unsigned mask=0;
        if(op==800){args[0]=n%4?47+n%3:0;args[1]=4*(1+n%12);char* name=reinterpret_cast<char*>(args.data()+2);for(unsigned i=0;i+1<args[1];++i)name[i]=char('a'+(i+n)%26);name[args[1]-1]=0;}
        if(op==801){args[0]=std::bit_cast<unsigned>(0.f);args[1]=std::bit_cast<unsigned>(n%2?0.f:4.f);args[2]=47+n%3;mask=3;}
        if(op==802)args[0]=n%4;auto instruction=enemy_vm_test::instruction(op,args,mask);std::memcpy(code.data()+16,instruction.data(),instruction.size());
        enemy.state.identifier=rng();enemy.state.fields_250[8]=n%5;enemy.state.fields_250[10]=rng();enemy.state.fields_2c8[0]=rng();enemy.state.fields_2c8[1]=rng();enemy.state.fields_2c8[2]=rng();owner.field_e0=rng();
        for(unsigned i=0;i<3;++i){owner.data.handles_44[i]=n&(1u<<i)?47+i:0;for(unsigned j=0;j<4;++j){auto& entry=targets[i].state.auxiliary[j];entry.values[0]=(n+i+j)%3?j:0xffffffffu;std::snprintf(reinterpret_cast<char*>(entry.values.data()+2),64,"phase_%u_%u_%u",n,i,j);}sp::Vec3 p{finite(),finite(),finite()};std::memcpy(targets[i].state.motion_110.words,&p,12);}
        const auto before=*reinterpret_cast<std::array<std::uint8_t,sizeof(enemy)>*>(&enemy);const auto stack=enemy.main.stack.words;const auto owner_before=owner_bytes;endpoints.calls.clear();endpoints.names.clear();FloatingEnvironment::prepare();const auto expected_return=original<int>(0x48c010,&enemy.state);const auto expected=*reinterpret_cast<std::array<std::uint8_t,sizeof(enemy)>*>(&enemy);const auto expected_stack=enemy.main.stack.words;const auto expected_owner=owner_bytes;const auto expected_calls=endpoints.calls;const auto expected_names=endpoints.names;
        std::memcpy(&enemy,before.data(),sizeof(enemy));std::copy(stack.begin(),stack.end(),enemy.main.stack.words.begin());owner_bytes=owner_before;endpoints.calls.clear();endpoints.names.clear();FloatingEnvironment::prepare();gp::EnemyOpcodeReader reader(enemy.state);const auto result=gp::execute_enemy_misc_opcode(reader,host);
        const bool data_same=std::memcmp(&enemy,expected.data(),sizeof(enemy))==0&&enemy.main.stack.words==expected_stack&&owner_bytes==expected_owner;const bool calls_same=endpoints.calls==expected_calls&&endpoints.names==expected_names;
        if((!data_same||!calls_same)&&failed<20)std::cerr<<"Misc opcode "<<op<<" case "<<n<<" data "<<data_same<<" calls "<<calls_same<<'\n';check("Enemy_entity_misc_opcode_all_enemy_stack_controller",data_same);check("Enemy_entity_misc_opcode_script_and_stone_endpoint_order",calls_same);check("Enemy_entity_misc_opcode_return",result&&*result==expected_return);
    }
    static gs::Context destruction_context;destruction_context.objects_04[1]=&owner;enemy.context=&destruction_context;enemy.state.fields_2c8[1]|=0x40000u;for(auto& target:targets){scheduler::unlink(target.controller_link);target.state.fields_2c8[1]|=0x40000u;target.context=&destruction_context;}global.contexts[0]=old_context;
}
