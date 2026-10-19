{
    namespace sp=th20::source::sprite;namespace gs=th20::source::game_session;
    // 48c010 and 496fb0 remain unmodified. Only allocation/device-like endpoint
    // bodies below are recorded; their parameters and all surrounding state
    // are compared, not their already separately tested internals.
    struct Endpoints {
        std::vector<std::array<unsigned,27>> calls;std::vector<std::string> names;
        static Endpoints*& current(){static Endpoints* p=nullptr;return p;}
        static void record(unsigned kind,void* self,std::initializer_list<unsigned> values){std::array<unsigned,27> row{};row[0]=kind;row[1]=reinterpret_cast<unsigned>(self);std::copy(values.begin(),values.end(),row.begin()+2);current()->calls.push_back(row);}
        static void __fastcall spawn(sp::AnimationFile* file,void*,unsigned* output,const char*,int script,const sp::Vec3* position,float angle,int layer,unsigned flags,sp::Animation**){record(1,file,{unsigned(script),unsigned(position!=nullptr),position?std::bit_cast<unsigned>(position->x):0,position?std::bit_cast<unsigned>(position->y):0,position?std::bit_cast<unsigned>(position->z):0,std::bit_cast<unsigned>(angle),unsigned(layer),flags});*output=0x10007;}
        static unsigned __fastcall remember(void* self,void*,unsigned handle){record(2,self,{handle});return 0;}
        static unsigned __fastcall effect(void* self,void*,int type,const sp::Vec3* p,int extra){record(3,self,{unsigned(type),std::bit_cast<unsigned>(p->x),std::bit_cast<unsigned>(p->y),std::bit_cast<unsigned>(p->z),unsigned(extra)});return 0;}
        static gp::Enemy* __fastcall enemy(gp::EnemyController* self,void*,const char* name,const gp::SpawnParameters* p,gp::Enemy* parent){record(4,self,{reinterpret_cast<unsigned>(parent)});std::memcpy(current()->calls.back().data()+3,p,84);current()->names.emplace_back(name);return parent;}
        static void __fastcall interrupt(void* controller,void*,unsigned handle,int event){record(5,controller,{handle,unsigned(event)});}
        static int __fastcall execute(sp::Animation* a,void*){record(6,a,{});++a->base.fields_10_28[0];return 0;}
    } endpoints;Endpoints::current()=&endpoints;
    struct Patch {std::uint8_t* address;std::array<std::uint8_t,5> before;Patch(unsigned va,void* callback):address(reinterpret_cast<std::uint8_t*>(mapped_image_base+va-0x400000)){std::memcpy(before.data(),address,5);DWORD old;VirtualProtect(address,5,PAGE_EXECUTE_READWRITE,&old);address[0]=0xe9;const auto displacement=unsigned(reinterpret_cast<std::uintptr_t>(callback)-reinterpret_cast<std::uintptr_t>(address)-5);std::memcpy(address+1,&displacement,4);FlushInstructionCache(GetCurrentProcess(),address,5);}~Patch(){std::memcpy(address,before.data(),5);FlushInstructionCache(GetCurrentProcess(),address,5);}}
      patch_spawn(0x450cb0,reinterpret_cast<void*>(&Endpoints::spawn)),patch_remember(0x497d20,reinterpret_cast<void*>(&Endpoints::remember)),patch_effect(0x4974d0,reinterpret_cast<void*>(&Endpoints::effect)),patch_enemy(0x4a8920,reinterpret_cast<void*>(&Endpoints::enemy)),patch_interrupt(0x44ee90,reinterpret_cast<void*>(&Endpoints::interrupt)),patch_execute(0x42b5d0,reinterpret_cast<void*>(&Endpoints::execute));
    struct CreationHost final:gp::EnemyAnimationOpcodeServices {
        sp::AnimationFile& file(gp::EnemyController& c,unsigned index) override{return *c.animation_files[index];}
        unsigned spawn(sp::AnimationFile& f,int script,const sp::Vec3* p,float angle,int layer,unsigned flags) override{unsigned handle;Endpoints::spawn(&f,nullptr,&handle,nullptr,script,p,angle,layer,flags,nullptr);return handle;}
        void delete_animation(unsigned& h) override{original<void>(0x44fcd0,&h);}sp::Animation* animation(unsigned& h) override{return original<sp::Animation*>(0x44ced0,&h);}
        float height(sp::Animation& a) override{return original<float>(0x4459a0,&a);}float width(sp::Animation& a) override{return original<float>(0x4459d0,&a);}
        void hide(unsigned h) override{original<void>(0x450120,&h);}void interrupt(unsigned h,int event) override{Endpoints::interrupt(*reinterpret_cast<void**>(mapped_image_base+0x1c0028),nullptr,h,event);}
        void layer(sp::Animation&,int) override{throw std::logic_error("Layer in scalar group");}void execute(sp::Animation& a) override{Endpoints::execute(&a,nullptr);}
        void remember(gs::Context& c,unsigned h) override{Endpoints::remember(c.objects_04[7],nullptr,h);}void effect(gs::Context& c,int type,const sp::Vec3& p) override{Endpoints::effect(c.objects_04[7],nullptr,type,&p,0);}
        void create_enemy(gp::EnemyController& c,const char* name,const gp::SpawnParameters& p,gp::Enemy* parent) override{Endpoints::enemy(&c,nullptr,name,&p,parent);}
        gp::Enemy* selected(gp::EnemyController& c,unsigned slot) override{return original<gp::Enemy*>(0x485660,&c,slot);}gp::Enemy* find(gp::EnemyController& c,unsigned id) override{return static_cast<gp::Enemy*>(gp::find_enemy_in_list(c.enemies,id));}
    } host;
    auto& global=*reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568);const auto old_context=global.contexts[0];
    std::array<std::uint8_t,0x134> owner_bytes{};auto& owner=*reinterpret_cast<gp::EnemyController*>(owner_bytes.data());scheduler::initialize_list(owner.enemies);owner.context=&global.contexts[0];global.contexts[0].objects_04[1]=&owner;std::array<unsigned,1> effect{};global.contexts[0].objects_04[7]=effect.data();
    sp::AnimationFile files[4];for(unsigned i=0;i<4;++i)owner.animation_files[2+i]=&files[i];
    gp::ScriptLoader scripts(environment);std::vector<std::uint8_t> code(256);scripts.records.push_back({"creation",code.data()});scripts.subroutine_count=1;
    gp::Enemy enemy,target;target.context=enemy.context=&global.contexts[0];target.state.identifier=47;scheduler::append(owner.enemies,target.controller_link);
    enemy.reset();enemy.loader=&scripts;enemy.main.subroutine=enemy.main.instruction_offset=0;enemy.state.entity=&enemy;enemy.main.stack.words.resize(128);enemy.state.animations.resize(4);
    *reinterpret_cast<void**>(mapped_image_base+0x1c0028)=animation_pool.data();auto* slots=reinterpret_cast<sp::PooledAnimation*>(animation_pool.data()+0x710);
    const unsigned operations[]{300,301,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,321,324,334,338,339,340,344};
    const auto finite=[&]{return float(int(rng()%10000)-5000)/32.f;};
    for(unsigned op:operations)for(unsigned n=0;n<512;++n){
        const bool spawn=op==300||op==301||op==304||op==305||(op>=309&&op<=312)||op==321;
        std::vector<unsigned> args(8);unsigned mask=0;for(auto& value:args)value=std::bit_cast<unsigned>(finite());args[0]=n%4;args[1]=n%9?unsigned(n%32):0xffffffffu;
        if(op==307||op==308||op==314||op==315||op==338||op==339)args[0]=2+n%4;
        if(op==339)args[2]=n%8;if(op==340)args[0]=n%2?47:48;
        if(op==324){mask=3;args[0]=std::bit_cast<unsigned>(0.f);args[1]=std::bit_cast<unsigned>(4.f);args[2]=n%2?47:48;}
        if(op==344){mask=1;args[0]=0;args[1]=n%2?47:48;}
        std::vector<std::uint8_t> instruction;
        if(spawn){const unsigned name_size=4u*(1+n%8);args.assign(1+name_size/4+5,0);args[0]=name_size;char* name=reinterpret_cast<char*>(args.data()+1);for(unsigned i=0;i+1<name_size;++i)name[i]=char('a'+(n+i)%26);name[name_size-1]=0;
            for(unsigned i=0;i<5;++i){unsigned value=i<2?std::bit_cast<unsigned>(finite()):rng();if((n>>i)&1u){mask|=1u<<(i+1);args[1+name_size/4+i]=i<2?std::bit_cast<unsigned>(float(i*4)):i*4;enemy.main.stack.local(i*4)=value;}else args[1+name_size/4+i]=value;}
        }
        instruction=enemy_vm_test::instruction(op,args,mask);std::memcpy(code.data()+16,instruction.data(),instruction.size());
        for(auto& word:enemy.state.fields_1c)word=rng();enemy.state.fields_1c[0]=2+n%4;for(auto& word:enemy.state.fields_78)word=rng();enemy.state.fields_2c8[0]=rng();enemy.state.fields_2c8[1]=rng();enemy.state.fields_2c8[2]=rng();enemy.state.fields_2c8[3]=0;enemy.state.fields_2c8[9]=reinterpret_cast<unsigned>(&global.contexts[0]);enemy.state.identifier=rng();
        const sp::Vec3 p{finite(),finite(),finite()},other{finite(),finite(),finite()};std::memcpy(enemy.state.motion_110.words,&p,12);std::memcpy(target.state.motion_110.words,&other,12);target.state.fields_2c8[1]=rng();owner.field_124=n%3;owner.data.field_88=n%5;owner.data.handles_44[0]=n%2?47:0;
        for(unsigned i=0;i<8;++i){slots[i]={};slots[i].active=1;slots[i].animation.handle=0x10000+i;slots[i].animation.base.vector_50={finite(),finite()};slots[i].animation.base.vector_70={finite(),finite()};slots[i].animation.base.flags[0]=rng();slots[i].animation.base.flags[1]=rng();slots[i].animation.base.flags[2]=rng();}
        for(unsigned i=0;i<4;++i){enemy.state.animations[i].handle=n%7?0x10000+i:0;enemy.state.animations[i].parent=int(rng());for(auto& v:enemy.state.animations[i].offset)v=finite();}
        const auto before=*reinterpret_cast<std::array<std::uint8_t,sizeof(enemy)>*>(&enemy);const auto links=enemy.state.animations;const auto pool=animation_pool;const auto stack=enemy.main.stack.words;const auto target_before=target.state.fields_2c8[1];
        endpoints.calls.clear();endpoints.names.clear();FloatingEnvironment::prepare();const auto expected_return=original<int>(0x48c010,&enemy.state);const auto expected=*reinterpret_cast<std::array<std::uint8_t,sizeof(enemy)>*>(&enemy);const auto expected_links=enemy.state.animations;const auto expected_pool=animation_pool;const auto expected_stack=enemy.main.stack.words;const auto expected_target=target.state.fields_2c8[1];const auto expected_calls=endpoints.calls;const auto expected_names=endpoints.names;
        std::memcpy(&enemy,before.data(),before.size());std::copy(links.begin(),links.end(),enemy.state.animations.begin());std::copy(stack.begin(),stack.end(),enemy.main.stack.words.begin());animation_pool=pool;target.state.fields_2c8[1]=target_before;endpoints.calls.clear();endpoints.names.clear();FloatingEnvironment::prepare();gp::EnemyOpcodeReader reader(enemy.state);const auto actual_return=gp::execute_enemy_animation_opcode(reader,host);
        const bool object_same=std::memcmp(&enemy,expected.data(),sizeof(enemy))==0&&std::memcmp(enemy.state.animations.data(),expected_links.data(),sizeof(gp::EnemyAnimationLink)*4)==0&&enemy.main.stack.words==expected_stack&&target.state.fields_2c8[1]==expected_target;const bool calls_same=endpoints.calls==expected_calls&&endpoints.names==expected_names;
        if((!object_same||!calls_same||animation_pool!=expected_pool)&&failed<12)std::cerr<<"Creation opcode "<<op<<" case "<<n<<" object "<<object_same<<" calls "<<calls_same<<" pool "<<(animation_pool==expected_pool)<<'\n';
        check("Enemy_entity_creation_opcode_object_links_stack_and_target",object_same);check("Enemy_entity_creation_opcode_exact_endpoint_calls_and_parameters",calls_same);check("Enemy_entity_creation_opcode_ANM_all_bytes_and_return",animation_pool==expected_pool&&actual_return&&*actual_return==expected_return);
    }
    for(auto* value:{&enemy,&target}){for(auto& link:value->state.animations)link.handle=0;value->state.fields_2c8[3]=0;value->state.fields_2c8[1]|=0x40000u;}scheduler::unlink(target.controller_link);global.contexts[0]=old_context;static gs::Context destruction_context;destruction_context.objects_04[1]=&owner;enemy.context=target.context=&destruction_context;
}
