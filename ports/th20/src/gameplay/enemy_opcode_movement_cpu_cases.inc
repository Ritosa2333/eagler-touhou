{
    namespace sp=th20::source::sprite;namespace gs=th20::source::game_session;namespace st=th20::source::state;
    struct MovementOpcodeHost final:gp::EnemyMovementOpcodeServices {
        float rate=1;gp::Enemy* target=nullptr;
        float clock_scale() override{return rate;}
        float random_angle() override{return original<float>(0x4297d0,reinterpret_cast<void*>(mapped_image_base+0x1ba4a8));}
        unsigned random_integer() override{return original<unsigned>(0x497470,reinterpret_cast<void*>(mapped_image_base+0x1ba4a8));}
        sp::Vec3 player_position(gs::Context& context) override{return th20::source::player_entity::position(context.objects_04[0]);}
        gp::Enemy* selected(gp::EnemyController&,unsigned) override{return target;}
        gp::Enemy* find(gp::EnemyController&,unsigned id) override{return id==target->state.identifier?target:nullptr;}
    } host;
    auto& global=*reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568);const auto old_context=global.contexts[0];
    auto& random=*reinterpret_cast<st::Random*>(mapped_image_base+0x1ba4a8);
    auto& rate_slot=*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0);const auto old_rate=rate_slot;rate_slot=&host.rate;
    auto& scale_slot=*reinterpret_cast<float*>(mapped_image_base+0x1aefe4);const auto old_scale=scale_slot;
    std::array<std::uint8_t,0x134> owner_bytes{};auto& owner=*reinterpret_cast<gp::EnemyController*>(owner_bytes.data());scheduler::initialize_list(owner.enemies);
    std::array<std::uint8_t,0x620> player_bytes{};global.contexts[0].objects_04[0]=player_bytes.data();global.contexts[0].objects_04[1]=&owner;
    gp::ScriptLoader scripts(environment);std::vector<std::uint8_t> code(16+16+32);scripts.records.push_back({"movement",code.data()});scripts.subroutine_count=1;
    gp::Enemy enemy,target;host.target=&target;target.context=enemy.context=&global.contexts[0];target.state.identifier=47;scheduler::append(owner.enemies,target.controller_link);owner.data.handles_44[0]=47;
    enemy.reset();enemy.loader=&scripts;enemy.main.subroutine=enemy.main.instruction_offset=0;enemy.state.entity=&enemy;enemy.main.stack.words.resize(128);enemy.state.movements.resize(4);
    const auto finite=[&]{return float(int(rng()%10000)-5000)/32.f;};
    for(unsigned op=400;op<=448;++op)for(unsigned n=0;n<640;++n){
        std::vector<unsigned> args(8);for(auto& value:args)value=std::bit_cast<unsigned>(finite());
        for(unsigned i=0;i<args.size();++i)if((n+i)%7==0)args[i]=std::bit_cast<unsigned>(-999999.f);else if((n+i)%11==0)args[i]=std::bit_cast<unsigned>(-1000000.f);
        switch(op){case 401:case 403:case 405:case 407:case 409:case 411:case 412:case 413:case 421:case 423:case 429:case 431:case 434:case 435:case 436:case 437:case 438:case 439:case 441:case 443:case 445:case 447:args[0]=unsigned(int(n%7)-2);args[1]=n%34;break;case 425:case 426:args[0]=unsigned(int(n%7)-2);break;case 432:case 433:args[0]=47;break;case 424:case 448:args[0]=n%4;break;}
        if(op==434||op==435||op==438||op==439)args[2]=(n/3)%34;
        auto instruction=enemy_vm_test::instruction(op,args);std::memcpy(code.data()+16,instruction.data(),instruction.size());
        enemy.state.movements.resize(4);
        for(auto& record:enemy.state.movements){for(auto& word:reinterpret_cast<unsigned(&)[sizeof(record)/4]>(record))word=std::bit_cast<unsigned>(finite());record.motion.words[0x44/4]=(rng()&~15u)|(n%5);}
        for(auto& word:enemy.state.motion_110.words)word=std::bit_cast<unsigned>(finite());enemy.state.motion_110.words[0x44/4]=(rng()&~15u)|(n%5);
        enemy.state.fields_1c[(0x44-0x1c)/4]=n%4;enemy.state.fields_2c8[1]=rng();enemy.state.fields_2c8[2]=rng();enemy.state.fields_2c8[3]=0;enemy.state.fields_2c8[9]=reinterpret_cast<unsigned>(&global.contexts[0]);
        for(auto& word:enemy.state.fields_178)word=std::bit_cast<unsigned>(finite());
        const sp::Vec3 target_position{finite(),finite(),finite()},player_position{finite(),finite(),finite()};std::memcpy(target.state.motion_110.words,&target_position,12);std::memcpy(player_bytes.data()+0x614,&player_position,12);
        host.rate=scale_slot=float(n%9)*0.25f;random.state=rng();random.modulus=0x7fffffffu;random.last=rng();
        const auto before=*reinterpret_cast<std::array<std::uint8_t,sizeof(enemy)>*>(&enemy);const auto records=enemy.state.movements;const auto random_before=random;
        FloatingEnvironment::prepare();const int expected_return=original<int>(0x48c010,&enemy.state);const auto expected=*reinterpret_cast<std::array<std::uint8_t,sizeof(enemy)>*>(&enemy);const auto expected_records=enemy.state.movements;const auto expected_random=random;
        std::memcpy(&enemy,before.data(),before.size());std::copy(records.begin(),records.end(),enemy.state.movements.begin());random=random_before;
        FloatingEnvironment::prepare();gp::EnemyOpcodeReader reader(enemy.state);const auto actual_return=gp::execute_enemy_movement_opcode(reader,host);
        bool object_same=std::memcmp(&enemy,expected.data(),sizeof(enemy))==0;bool records_same=enemy.state.movements.size()==expected_records.size()&&std::memcmp(enemy.state.movements.data(),expected_records.data(),sizeof(gp::EnemyMovementRecord)*expected_records.size())==0;
        if((!object_same||!records_same)&&failed<12){std::cerr<<"Movement opcode "<<op<<" case "<<n<<" object "<<object_same<<" records "<<records_same;if(!records_same){const auto* a=reinterpret_cast<const unsigned char*>(enemy.state.movements.data());const auto* b=reinterpret_cast<const unsigned char*>(expected_records.data());unsigned off=0;while(off<expected_records.size()*sizeof(gp::EnemyMovementRecord)&&a[off]==b[off])++off;std::cerr<<" record byte "<<std::hex<<off<<" got "<<unsigned(a[off])<<" expected "<<unsigned(b[off])<<std::dec;}std::cerr<<'\n';}
        check("Enemy_entity_movement_opcode_all1064bytes",object_same);check("Enemy_entity_movement_opcode_all_records_and_return",records_same&&actual_return&&*actual_return==expected_return);check("Enemy_entity_movement_opcode_shared_random_all28bytes",std::memcmp(&random,&expected_random,sizeof(random))==0);
    }
    for(auto* value:{&enemy,&target}){for(auto& link:value->state.animations)link.handle=0;value->state.fields_2c8[3]=0;value->state.fields_2c8[1]|=0x40000u;}
    scheduler::unlink(target.controller_link);global.contexts[0]=old_context;rate_slot=old_rate;scale_slot=old_scale;
    // Destruction uses the same valid controller view even after global restore.
    static gs::Context destruction_context;destruction_context.objects_04[1]=&owner;enemy.context=target.context=&destruction_context;
}
