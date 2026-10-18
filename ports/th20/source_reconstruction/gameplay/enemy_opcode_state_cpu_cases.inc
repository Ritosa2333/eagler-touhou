{
    namespace sp=th20::source::sprite;namespace gs=th20::source::game_session;namespace hd=th20::source::hud;
    struct StateHost final:gp::EnemyStateOpcodeServices {
        gs::Session* globals=nullptr;gp::GameController* game_owner=nullptr;hd::FrontInf* front=nullptr;
        gs::Session& session() override{return *globals;}gp::GameController& game() override{return *game_owner;}hd::FrontInf& hud() override{return *front;}
        void visibility(unsigned handle,bool visible) override{if(auto* a=original<sp::Animation*>(0x44ced0,&handle))original<void>(visible?0x44fa70:0x450160,a);}
        void drop(gp::EnemyPatternState&,const sp::Vec3&,bool) override{throw std::logic_error("Drop endpoint outside state scalar oracle");}
        void sound(int,float) override{throw std::logic_error("Sound endpoint outside state scalar oracle");}
        void shake(int,int,int) override{throw std::logic_error("Shake endpoint outside state scalar oracle");}
        void dialogue(int) override{throw std::logic_error("Dialogue endpoint outside state scalar oracle");}
        void cancel_bullets(gs::Context&) override{throw std::logic_error("Bullet endpoint outside state scalar oracle");}
        void erase_lasers(gs::Context&,bool) override{throw std::logic_error("Laser endpoint outside state scalar oracle");}
        void clear(gp::EnemyController&,gp::EnemyClearKind,int) override{throw std::logic_error("Clear endpoint outside state scalar oracle");}
        void card_start(gs::Context&,int,const char*,int,int) override{throw std::logic_error("Card start outside state scalar oracle");}
        void card_finish(gs::Context&) override{throw std::logic_error("Card finish outside state scalar oracle");}
        void delete_animation(unsigned& handle) override{original<void>(0x44fcd0,&handle);}
        void clock_scale(float value) override{*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=value;}
        void stage_title() override{throw std::logic_error("Stage title outside state scalar oracle");}
        void fog(int,int,unsigned,float,float) override{throw std::logic_error("Fog endpoint outside state scalar oracle");}
        void death_effect(gp::EnemyController&,unsigned,int,const sp::Vec3&,float) override{throw std::logic_error("Death ANM outside state scalar oracle");}
        int defeat(gp::Enemy&) override{throw std::logic_error("Defeat outside state scalar oracle");}
        gp::Enemy* selected(gp::EnemyController& c,unsigned index) override{return original<gp::Enemy*>(0x485660,&c,index);}
    } state_host;
    auto& native_session=*reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568);
    const auto saved_session=native_session;
    const auto saved_game=*reinterpret_cast<void**>(mapped_image_base+0x1ba828),saved_hud=*reinterpret_cast<void**>(mapped_image_base+0x1c06a4);
    alignas(8)std::array<std::uint8_t,0x114> game_bytes{};alignas(8)std::array<std::uint8_t,0x2d8> hud_bytes{};alignas(8)std::array<std::uint8_t,0xc8> card_bytes{};
    alignas(8)std::array<std::uint8_t,0x60> bullet_bytes{},player_bytes{};
    state_host.globals=&native_session;state_host.game_owner=reinterpret_cast<gp::GameController*>(game_bytes.data());state_host.front=reinterpret_cast<hd::FrontInf*>(hud_bytes.data());
    *reinterpret_cast<void**>(mapped_image_base+0x1ba828)=game_bytes.data();*reinterpret_cast<void**>(mapped_image_base+0x1c06a4)=hud_bytes.data();
    alignas(4)std::array<std::uint8_t,0x134> controller_bytes{};auto& owner=*reinterpret_cast<gp::EnemyController*>(controller_bytes.data());scheduler::initialize_list(owner.enemies);
    gp::ScriptLoader scripts(environment);std::vector<std::uint8_t> code(16+16+64);scripts.records.push_back({"op",code.data()});scripts.subroutine_count=1;
    gp::Enemy enemy;enemy.reset();enemy.loader=&scripts;enemy.main.subroutine=enemy.main.instruction_offset=0;enemy.main.stack.words.resize(128);enemy.state.entity=&enemy;enemy.state.animations.resize(4);
    auto& context=native_session.contexts[0];context.primary_owner=reinterpret_cast<rt::CallbackOwner*>(bullet_bytes.data());context.objects_04[0]=player_bytes.data();context.objects_04[1]=&owner;context.objects_04[3]=card_bytes.data();context.current_player=&native_session.player_table.players[1];enemy.context=&context;
    *reinterpret_cast<void**>(mapped_image_base+0x1c0028)=animation_pool.data();auto* slots=reinterpret_cast<sp::PooledAnimation*>(animation_pool.data()+0x710);
    const unsigned operations[]{500,501,502,503,504,505,506,507,508,510,511,512,513,515,519,520,524,526,527,529,530,534,535,536,537,538,540,541,542,543,544,546,547,549,550,552,553,554,557,558,559,561,562,563,564,566,567,568,570,571,572,574,575};
    auto finite=[&]{return float(int(rng()%10000)-5000)/32.f;};
    auto bytes=[]<class T>(const T& value){std::array<std::uint8_t,sizeof(T)> out;std::memcpy(out.data(),&value,sizeof(value));return out;};
    for(unsigned op:operations)for(unsigned n=0;n<1024;++n){
        std::vector<unsigned> args(12);for(auto& a:args)a=rng();
        if(op==507||op==567)args[0]=1+n%16;
        if(op==512)args[0]=n%5? n%16:0xffffffffu;
        if(op==527){args[0]=n%4;args[1]=std::bit_cast<unsigned>(finite());}
        const bool destination=op==529||op==530||op==549||op==570||op==571;
        if(destination)args[0]=0; //Real local stack destination, with reference mask set.
        if(op==500||op==501||op==504||op==508||op==526||op==541||op==554||op==558||op==559)for(auto& a:args)a=std::bit_cast<unsigned>(finite());
        if(op==550){args[0]=4;args[1]=n%2?0x0000706fu:0x00007878u;}
        auto instruction=enemy_vm_test::instruction(op,args,destination?1:0);std::memcpy(code.data()+16,instruction.data(),instruction.size());
        for(auto& word:enemy.state.fields_1c)word=rng();for(auto& word:enemy.state.fields_2c8)word=rng();enemy.state.fields_2c8[9]=reinterpret_cast<unsigned>(&context);enemy.state.fields_2c8[3]=0;
        for(auto& word:enemy.state.fields_250)word=rng();enemy.state.fields_250[8]=n%3;enemy.state.fields_250[12]=n%3?unsigned(n%1000):0xffffffffu;enemy.state.fields_250[13]=n%4;
        for(auto& word:enemy.state.auxiliary_18c.words)word=rng();for(auto& word:enemy.state.pattern_1a8.fields_00)word=rng();enemy.state.pattern_1a8.field_8c=rng();
        enemy.state.fields_1c[(0x50-0x1c)/4]=n%11;enemy.main.time=finite();
        enemy.state.timer_a8={};enemy.state.timer_288={};enemy.state.timer_298={};enemy.state.pattern_1a8.timer_90={};
        for(auto& byte:game_bytes)byte=std::uint8_t(rng());for(auto& byte:hud_bytes)byte=std::uint8_t(rng());for(auto& byte:card_bytes)byte=std::uint8_t(rng());
        state_host.front->collecting=nullptr;*reinterpret_cast<unsigned*>(card_bytes.data()+0x20)=n%2?0x10000u:0;
        for(auto& word:reinterpret_cast<std::uint32_t(&)[sizeof(gs::PlayerTable)/4]>(native_session.player_table))word=rng();
        native_session.player_table.field_1e0=n%7;native_session.player_table.fields_1ec[(0x1fc-0x1ec)/4]=n%3?unsigned(n%1000):0xffffffffu;
        for(auto& word:reinterpret_cast<std::uint32_t(&)[sizeof(gp::EnemyData)/4]>(owner.data))word=rng();std::fill_n(owner.data.handles_44,16,0u);
        for(unsigned i=0;i<4;++i){enemy.state.animations[i].handle=n%9?0x10000u+i:0;auto& slot=slots[i];std::memset(&slot.animation,0,sizeof(slot.animation));slot.active=1;slot.animation.handle=0x10000u+i;slot.animation.base.flags[1]=rng();}
        const auto before=bytes(enemy);const auto table_before=bytes(native_session.player_table);const auto owner_before=bytes(owner.data);const auto game_before=game_bytes;const auto hud_before=hud_bytes;const auto card_before=card_bytes;const auto bullet_before=bullet_bytes;const auto player_before=player_bytes;const auto pool_before=animation_pool;const auto stack_before=enemy.main.stack.words;const auto links_before=enemy.state.animations;
        const float scale_before=*reinterpret_cast<float*>(mapped_image_base+0x1aefe4);
        FloatingEnvironment::prepare();const int expected_return=original<int>(0x48c010,&enemy.state);
        const auto expected=bytes(enemy);const auto table_expected=bytes(native_session.player_table);const auto owner_expected=bytes(owner.data);const auto game_expected=game_bytes;const auto hud_expected=hud_bytes;const auto card_expected=card_bytes;const auto bullet_expected=bullet_bytes;const auto player_expected=player_bytes;const auto pool_expected=animation_pool;const auto stack_expected=enemy.main.stack.words;
        const float scale_expected=*reinterpret_cast<float*>(mapped_image_base+0x1aefe4);
        std::memcpy(&enemy,before.data(),before.size());std::memcpy(&native_session.player_table,table_before.data(),table_before.size());std::memcpy(&owner.data,owner_before.data(),owner_before.size());game_bytes=game_before;hud_bytes=hud_before;card_bytes=card_before;bullet_bytes=bullet_before;player_bytes=player_before;animation_pool=pool_before;std::copy(stack_before.begin(),stack_before.end(),enemy.main.stack.words.begin());std::copy(links_before.begin(),links_before.end(),enemy.state.animations.begin());*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=scale_before;
        FloatingEnvironment::prepare();gp::EnemyOpcodeReader reader(enemy.state);const auto actual_return=gp::execute_enemy_state_opcode(reader,state_host);
        if(std::memcmp(&enemy,expected.data(),sizeof(enemy))&&failed<20){std::size_t i=0;while(reinterpret_cast<const std::uint8_t*>(&enemy)[i]==expected[i])++i;std::cerr<<"State opcode "<<op<<" n="<<n<<" byte="<<std::hex<<i<<std::dec<<'\n';}
        check("Enemy_state_opcode_all_1064bytes",std::memcmp(&enemy,expected.data(),sizeof(enemy))==0);
        check("Enemy_state_opcode_return_and_local_stack",actual_return&&*actual_return==expected_return&&enemy.main.stack.words==stack_expected);
        check("Enemy_state_opcode_PlayerTable_and_controller",bytes(native_session.player_table)==table_expected&&bytes(owner.data)==owner_expected);
        check("Enemy_state_opcode_HUD_Card_Bullet_Player_Game_ANM",game_bytes==game_expected&&hud_bytes==hud_expected&&card_bytes==card_expected&&bullet_bytes==bullet_expected&&player_bytes==player_expected&&animation_pool==pool_expected&&std::memcmp(reinterpret_cast<void*>(mapped_image_base+0x1aefe4),&scale_expected,4)==0);
    }
    native_session=saved_session;*reinterpret_cast<void**>(mapped_image_base+0x1ba828)=saved_game;*reinterpret_cast<void**>(mapped_image_base+0x1c06a4)=saved_hud;
    for(auto& link:enemy.state.animations)link.handle=0;enemy.state.fields_2c8[3]=0;enemy.state.fields_2c8[1]|=0x40000u;
}
