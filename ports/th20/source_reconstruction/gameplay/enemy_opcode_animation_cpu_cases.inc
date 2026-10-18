{
    namespace sp=th20::source::sprite;namespace gs=th20::source::game_session;
    struct AnimationHost final:gp::EnemyAnimationOpcodeServices {
        sp::AnimationFile& file(gp::EnemyController&,unsigned) override{throw std::logic_error("No ANM resource load in scalar opcode group");}
        unsigned spawn(sp::AnimationFile&,int,const sp::Vec3*,float,int,unsigned) override{throw std::logic_error("No ANM allocation in scalar opcode group");}
        void delete_animation(unsigned&) override{throw std::logic_error("No ANM deletion in scalar opcode group");}
        sp::Animation* animation(unsigned& handle) override{return original<sp::Animation*>(0x44ced0,&handle);}
        float height(sp::Animation&) override{throw std::logic_error("No dimensions in scalar opcode group");}float width(sp::Animation&) override{throw std::logic_error("No dimensions in scalar opcode group");}
        void hide(unsigned) override{throw std::logic_error("No hide in scalar opcode group");}void interrupt(unsigned,int) override{throw std::logic_error("No interrupt in scalar opcode group");}
        void layer(sp::Animation& a,int layer) override{original<void>(0x450350,&a,layer);}
        void execute(sp::Animation&) override{throw std::logic_error("No VM execute in scalar opcode group");}void remember(gs::Context&,unsigned) override{throw std::logic_error("No Effect reserve in scalar opcode group");}void effect(gs::Context&,int,const sp::Vec3&) override{throw std::logic_error("No Effect spawn in scalar opcode group");}
        void create_enemy(gp::EnemyController&,const char*,const gp::SpawnParameters&,gp::Enemy*) override{throw std::logic_error("No Enemy spawn in scalar opcode group");}
        gp::Enemy* selected(gp::EnemyController&,unsigned) override{throw std::logic_error("No Enemy selection in scalar opcode group");}gp::Enemy* find(gp::EnemyController&,unsigned) override{throw std::logic_error("No Enemy lookup in scalar opcode group");}
    } animation_host;
    gp::ScriptLoader scripts(environment);std::vector<std::uint8_t> code(16+16+32);scripts.records.push_back({"op",code.data()});scripts.subroutine_count=1;
    std::array<std::uint8_t,0x134> owner_bytes{};gs::Context context{};context.objects_04[1]=owner_bytes.data();gp::Enemy enemy;enemy.context=&context;enemy.reset();enemy.loader=&scripts;enemy.main.subroutine=enemy.main.instruction_offset=0;enemy.state.entity=&enemy;enemy.state.animations.resize(4);enemy.main.stack.words.resize(128);
    *reinterpret_cast<void**>(mapped_image_base+0x1c0028)=animation_pool.data();auto* slots=reinterpret_cast<sp::PooledAnimation*>(animation_pool.data()+0x710);
    const unsigned operations[]{302,319,320,322,323,325,326,327,328,329,330,331,332,333,335,336,337,341,342,343};
    auto finite=[&]{return float(int(rng()%10000)-5000)/32.f;};
    for(unsigned op:operations)for(unsigned n=0;n<800;++n){
        std::vector<unsigned> args(8);for(auto& arg:args)arg=std::bit_cast<unsigned>(finite());args[0]=n%4;for(unsigned i=1;i<args.size();++i)if(op==325||op==326||op==328||op==330||op==332||op==333)args[i]=rng();
        auto instruction=enemy_vm_test::instruction(op,args);std::memcpy(code.data()+16,instruction.data(),instruction.size());
        for(auto& word:enemy.state.fields_1c)word=rng();for(auto& word:enemy.state.fields_2c8)word=rng();enemy.state.fields_2c8[3]=0;
        for(unsigned i=0;i<4;++i){auto& link=enemy.state.animations[i];link.handle=n%9?0x10000u+i:0;link.parent=int(rng());for(auto& v:link.offset)v=finite();auto& slot=slots[i];for(auto& byte:reinterpret_cast<std::uint8_t(&)[sizeof(slot.animation)]>(slot.animation))byte=std::uint8_t(rng());slot.active=1;slot.animation.handle=0x10000u+i;}
        const auto before=*reinterpret_cast<std::array<std::uint8_t,sizeof(enemy)>*>(&enemy);const auto links=enemy.state.animations;const auto pool=animation_pool;
        FloatingEnvironment::prepare();const int expected_return=original<int>(0x48c010,&enemy.state);const auto expected=*reinterpret_cast<std::array<std::uint8_t,sizeof(enemy)>*>(&enemy);const auto expected_links=enemy.state.animations;const auto expected_pool=animation_pool;
        std::memcpy(&enemy,before.data(),before.size());std::copy(links.begin(),links.end(),enemy.state.animations.begin());animation_pool=pool;FloatingEnvironment::prepare();gp::EnemyOpcodeReader reader(enemy.state);const auto actual_return=gp::execute_enemy_animation_opcode(reader,animation_host);
        if(std::memcmp(&enemy,expected.data(),sizeof(enemy))&&failed<10)std::cerr<<"Enemy scalar opcode "<<op<<" case "<<n<<" object mismatch\n";
        check("Enemy_entity_animation_opcode_all1064bytes",std::memcmp(&enemy,expected.data(),sizeof(enemy))==0);check("Enemy_entity_animation_opcode_links_and_return",actual_return&&*actual_return==expected_return&&std::memcmp(enemy.state.animations.data(),expected_links.data(),sizeof(gp::EnemyAnimationLink)*4)==0);if(animation_pool!=expected_pool&&failed<10){std::size_t off=0;while(animation_pool[off]==expected_pool[off])++off;std::cerr<<"ANM opcode "<<op<<" case "<<n<<" byte "<<std::hex<<off<<" got "<<unsigned(animation_pool[off])<<" wanted "<<unsigned(expected_pool[off])<<std::dec<<'\n';}check("Enemy_entity_animation_opcode_ANM_all_bytes",animation_pool==expected_pool);
    }
    for(auto& link:enemy.state.animations)link.handle=0;enemy.state.fields_2c8[3]=0;enemy.state.fields_2c8[1]|=0x40000u;
}
