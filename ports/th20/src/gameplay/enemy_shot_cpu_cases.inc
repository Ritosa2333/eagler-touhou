{
    namespace sp=th20::source::sprite;namespace gs=th20::source::game_session;namespace bt=th20::source::bullet;
    struct ShotHost final:gp::EnemyShotOpcodeServices {
        std::shared_ptr<void> metadata(const bt::ShotMetadata* copy) override{if(copy)return std::allocate_shared<bt::ShotMetadata>(std::pmr::polymorphic_allocator<bt::ShotMetadata>{},*copy);return std::allocate_shared<bt::ShotMetadata>(std::pmr::polymorphic_allocator<bt::ShotMetadata>{});}
        void fire(gs::Context&,const bt::ShotParameters&,const std::shared_ptr<void>&,unsigned) override{throw std::logic_error("Fire excluded from metadata oracle");}
        void cancel_all(gs::Context&) override{throw std::logic_error("Cancel excluded from metadata oracle");}void cancel_circle(gs::Context&,const sp::Vec3&,float,bool) override{throw std::logic_error("Cancel excluded from metadata oracle");}
        sp::Vec3 player_position(gs::Context& c) override{return th20::source::player_entity::position(c.objects_04[0]);}
        void mesh(gp::EnemyState&,float,unsigned) override{throw std::logic_error("Mesh excluded from metadata oracle");}void background_event(int) override{throw std::logic_error("Background excluded from metadata oracle");}
        unsigned callback(unsigned table,int index) override{const unsigned base[]{0x56fe8c,0x56fe98,0x56fe9c};return *reinterpret_cast<unsigned*>(mapped_image_base+base[table]-0x400000+index*4);}
        void invoke_callback(gp::EnemyState&,int) override{throw std::logic_error("Callback body excluded from metadata oracle");}void add_score(gs::Context&,const sp::Vec3&,int) override{throw std::logic_error("Score excluded from metadata oracle");}
    } host;
    std::array<std::uint8_t,0x134> owner_bytes{};auto& owner=*reinterpret_cast<gp::EnemyController*>(owner_bytes.data());gs::Context context{};context.objects_04[1]=&owner;
    std::array<std::uint8_t,0x620> player{};context.objects_04[0]=player.data();
    gp::ScriptLoader scripts(environment);std::vector<std::uint8_t> code(256);scripts.records.push_back({"shot",code.data()});scripts.subroutine_count=1;
    const unsigned operations[]{600,602,603,604,605,606,607,608,609,610,611,612,614,617,618,619,620,623,624,625,626,631,632,633};
    const auto finite=[&](){return float(int(rng()%20000)-10000)/32.f;};
    const auto nodes=[](gp::EnemyState& state){std::vector<gp::EnemyQueuedRecord*> out;for(auto& q:state.queued)out.push_back(&q);return out;};
    const auto same_queues=[&](gp::EnemyState& a,gp::EnemyState& b){auto aa=nodes(a),bb=nodes(b);if(aa.size()!=bb.size())return false;for(unsigned i=0;i<aa.size();++i){if(aa[i]->values!=bb[i]->values||bool(aa[i]->owner)!=bool(bb[i]->owner)||aa[i]->owner.use_count()!=bb[i]->owner.use_count())return false;if(aa[i]->owner){const auto& x=*static_cast<bt::ShotMetadata*>(aa[i]->owner.get());const auto& y=*static_cast<bt::ShotMetadata*>(bb[i]->owner.get());if(std::memcmp(&x,&y,4)||std::memcmp(reinterpret_cast<const std::uint8_t*>(&x)+0x14,reinterpret_cast<const std::uint8_t*>(&y)+0x14,0x36)||x.commands.size()!=y.commands.size()||x.commands.capacity()!=y.commands.capacity()||std::memcmp(x.commands.data(),y.commands.data(),x.commands.size()*sizeof(bt::Command)))return false;}for(unsigned j=0;j<aa.size();++j)if((aa[i]->owner.get()==aa[j]->owner.get())!=(bb[i]->owner.get()==bb[j]->owner.get()))return false;}return true;};
    for(unsigned op:operations)for(unsigned n=0;n<512;++n){
        gp::Enemy cpu,source;for(auto* value:{&cpu,&source}){value->context=&context;value->reset();value->loader=&scripts;value->main.subroutine=value->main.instruction_offset=0;value->main.stack.words.resize(128);value->state.entity=value;value->state.fields_2c8[9]=reinterpret_cast<unsigned>(&context);value->state.queued.resize(n%5);}
        std::vector<unsigned> args(16);for(auto& value:args)value=std::bit_cast<unsigned>(finite());args[0]=n%4;args[1]=n%3;args[2]=(n/3)%4;
        if(op>=609&&op<=612){args[1]=n%3;for(unsigned i=2;i<args.size();++i)args[i]=rng();}
        if(op==624||op==631)args[0]=n%3;if(op==625||op==626)args[0]=0;if(op==614)args[1]=(n+1)%4;if(op==632)args[1]=n%5;
        unsigned mask=0;if(op==617){mask=1;args[0]=std::bit_cast<unsigned>(0.f);}
        auto instruction=enemy_vm_test::instruction(op,args,mask);std::memcpy(code.data()+16,instruction.data(),instruction.size());
        auto cq=nodes(cpu.state),sq=nodes(source.state);
        for(unsigned i=0;i<cq.size();++i){for(auto& byte:cq[i]->values)byte=std::uint8_t(rng());sq[i]->values=cq[i]->values;auto v=gp::shot_values(*cq[i]);v.command_cursor=int((n+i)%4);gp::store_shot_values(*cq[i],v);gp::store_shot_values(*sq[i],v);
            if(n%7){cq[i]->owner=host.metadata(nullptr);sq[i]->owner=host.metadata(nullptr);auto& a=*static_cast<bt::ShotMetadata*>(cq[i]->owner.get());auto& b=*static_cast<bt::ShotMetadata*>(sq[i]->owner.get());a.field_00=finite();for(auto& f:a.fields_14)f=finite();for(auto& v:a.fields_24)v=rng();a.type=std::int16_t(rng());a.field_3a=std::int16_t(rng());a.field_3c=int(rng());a.field_40=int(rng());a.field_44=rng();a.field_48=std::uint8_t(rng());a.field_49=std::uint8_t(rng());a.commands.resize(1+n%6);for(auto& command:a.commands)for(auto& word:command.words)word=rng();b=a;}
        }
        // Native498b80 must detach shared metadata;498d10 must retain aliases.
        if(cq.size()>1&&n%2){cq[1]->owner=cq[0]->owner;sq[1]->owner=sq[0]->owner;}
        for(auto& word:cpu.state.fields_2c8)word=rng();cpu.state.fields_2c8[3]=0;cpu.state.fields_2c8[9]=reinterpret_cast<unsigned>(&context);std::copy(std::begin(cpu.state.fields_2c8),std::end(cpu.state.fields_2c8),std::begin(source.state.fields_2c8));
        const sp::Vec3 p{finite(),finite(),finite()},player_p{finite(),finite(),finite()};std::memcpy(cpu.state.motion_110.words,&p,12);source.state.motion_110=cpu.state.motion_110;std::memcpy(player.data()+0x614,&player_p,12);
        const auto owner_before=owner_bytes;
        FloatingEnvironment::prepare();const int expected_return=original<int>(0x48c010,&cpu.state);const auto expected_owner=owner_bytes;owner_bytes=owner_before;
        FloatingEnvironment::prepare();gp::EnemyOpcodeReader reader(source.state);const auto actual_return=gp::execute_enemy_bullet_opcode(reader,host);
        auto a=*reinterpret_cast<std::array<std::uint8_t,sizeof(gp::EnemyState)>*>(&cpu.state),b=*reinterpret_cast<std::array<std::uint8_t,sizeof(gp::EnemyState)>*>(&source.state);for(auto offset:{8u,0x16cu}){std::memset(a.data()+offset,0,4);std::memset(b.data()+offset,0,4);}const bool state_same=a==b&&expected_owner==owner_bytes&&cpu.main.stack.words==source.main.stack.words;
        const bool queues_same=same_queues(cpu.state,source.state);
        if((!queues_same||!state_same)&&failed<16)std::cerr<<"Shot opcode "<<op<<" case "<<n<<" state "<<state_same<<" queues "<<queues_same<<'\n';
        check("Enemy_entity_shot_opcode_values_metadata_commands_and_aliasing",queues_same);check("Enemy_entity_shot_opcode_all_state_stack_and_owner",state_same);check("Enemy_entity_shot_opcode_return",actual_return&&*actual_return==expected_return);
        cpu.state.fields_2c8[1]|=0x40000u;source.state.fields_2c8[1]|=0x40000u;
    }
}
