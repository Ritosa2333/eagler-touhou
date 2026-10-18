{
    struct MoveReset final:pl::StageResetServices {
        Callbacks& env;explicit MoveReset(Callbacks& e):env(e){}
        gs::Session& session() override{return env.host.state;}
        void refresh_power(pl::Player&) override{throw std::logic_error("Movement does not refresh power");}
        void interrupt(std::uint32_t handle,int event) override{env.interrupt(handle,event);}
        void delete_animation(std::uint32_t&) override{throw std::logic_error("Movement does not delete via stage reset");}
        void hide_animation(sp::Animation&) override{throw std::logic_error("Movement does not disable secondary player");}
    } move_reset(callback_env);
    struct MoveHost final:pl::MovementServices {
        OptionHost& option;MoveReset& resets;MoveHost(OptionHost& o,MoveReset& r):option(o),resets(r){}
        pl::OptionFrameServices& options() override{return option;}pl::StageResetServices& reset() override{return resets;}
        int input_slot(int view) override{return original<int>(0x498fb0,*reinterpret_cast<void**>(mapped_image_base+0x1b8898),view);}
        std::uint32_t held(int slot,std::uint32_t mask) override{if(slot<0)return 0;auto* state=reinterpret_cast<void**>(mapped_image_base+0x1b889c)[slot];return state?original<std::uint32_t>(0x4973a0,state,mask):0;}
        bool enemy_ready(gs::Context& context) override{return context.objects_04[1]&&original<std::uint32_t>(0x478080,context.objects_04[1]);}
        float clock_scale() override{return *reinterpret_cast<float*>(mapped_image_base+0x1aefe4);}
        void bind_script(pl::Player& player,int script) override{original<void>(0x4382b0,player.animation_file,&player.animation,script,static_cast<sp::Animation*>(nullptr));}
        sp::Animation* animation(std::uint32_t& handle) override{return original<sp::Animation*>(0x44ced0,&handle);}
        std::uint32_t spawn_focus_effect(pl::Player& player) override{auto* effect=static_cast<std::uint8_t*>(player.context->objects_04[7]);sp::AnimationFile* file;std::memcpy(&file,effect+0x10,4);std::uint32_t handle;original<void>(0x450c70,file,&handle,"effect",19,-1,static_cast<const sp::Vec3*>(nullptr));return handle;}
        void sound(int id) override{original<void>(0x426d70,reinterpret_cast<void*>(mapped_image_base+0x1ba830),id,0);}
    } move_host(option_host,move_reset);
    const auto saved_input=*reinterpret_cast<void**>(mapped_image_base+0x1b8898);std::array<void*,4> saved_slots{};std::memcpy(saved_slots.data(),reinterpret_cast<void*>(mapped_image_base+0x1b889c),sizeof(saved_slots));
    std::array<std::uint8_t,0x2ef8> input_bytes{};std::array<std::uint8_t,0x2c0> input_buttons{};put(reinterpret_cast<void*>(mapped_image_base),0x1b8898,input_bytes.data());put(reinterpret_cast<void*>(mapped_image_base),0x1b889c,input_buttons.data());
    std::array<std::uint8_t,0x28> effect_owner{};put(effect_owner.data(),0x10,&file);native_session.contexts[0].objects_04[7]=effect_owner.data();
    const float old_rate=**reinterpret_cast<float**>(mapped_image_base+0x1aefe0),old_clock=*reinterpret_cast<float*>(mapped_image_base+0x1aefe4);
    for(unsigned n=0;n<4800;++n){firing_case=n;firing_phase="Player alive movement";
        for(auto& list:sprites->lists){list={};list.tail=&list.sentinel;}sprites->free_sentinel={};sprites->field_7d40e88=2;sprites->field_6c4=0;
        for(unsigned i=0;i<4;++i){auto& pool=sprites->pool[i];pool={};sp::construct_pooled_animation(pool);pool.index=i;pool.free_link.value=&pool.animation;pool.active=i<2;pool.animation.handle=0x10000u+i;if(i>=2)sq::insert_after(reinterpret_cast<sq::Link&>(sprites->free_sentinel),reinterpret_cast<sq::Link&>(pool.free_link));}
        pl::construct_player_fields(owner);owner.shot_data=data.data();owner.animation_file=&file;owner.context=&native_session.contexts[0];owner.focused_204c=n%2;owner.entity_flags=(rng()&~0xc0u)|((n%4)<<6);owner.fixed_position={int(rng()%40000)-20000,int(rng()%60000)};owner.fields_20e4[0]=n%100;owner.fields_20e4[1]=n%35;owner.fields_20e4[2]=std::bit_cast<std::uint32_t>(float(n%6)*.25f);owner.vector_20f0={finite(),finite(),finite()};owner.collision_expansion=finite();
        const float rate=float(n%3+1)*.5f;**reinterpret_cast<float**>(mapped_image_base+0x1aefe0)=rate;put(reinterpret_cast<void*>(mapped_image_base),0x1aefe4,rate);
        const int transition=n%18;owner.timers_2050[2]={transition-1,transition,float(transition),0};owner.timers_644[1].current=n%10;owner.fields_674[0]=int(n%3)-1;owner.fields_674[3]=4;for(auto& script:owner.animation_scripts)script=n%2;for(auto& speed:owner.speeds_20b4)speed=int(n%1000);for(auto& point:owner.vectors_20fc){put(&point,0,rng());put(&point,4,rng());}
        owner.handle_60c=n%11?0x10000u:0u;const int slot=n%7?-1:0;put(input_bytes.data(),0x2e30,slot);const auto bits=rng()&0xffu;put(input_buttons.data(),0x29c,bits);put(enemies_owner.data(),0x124,n%5?1u:0u);
        const bool transient=(n%4)!=0&&int(float(transition)+rate)<16;const bool expected_focus=transient?bool(owner.focused_204c):(n%5&&n%10>=4&&slot==0&&(bits&8u));
        auto& option=owner.options[n%10];option.state=2;option.fields_f4[4]=expected_focus;option.handle_dc=0x10001u;option.fields_f4[2]=n%3;option.offsets_80[0]={int(n),-int(n)};option.offsets_80[1]={-int(n),int(n)};option.vector_78={int(n*16),int(n*32)};
        std::memset(sound,0,0x57e8);for(unsigned i=0;i<12;++i)put(sound,0x1c+i*0x208,-1);
        const auto before=owner_storage;const auto before_session=native_session;const Snapshot before_sprites(*sprites);const auto generation=file.fields_5c[3];const auto before_sound=std::vector<std::uint8_t>(sound,sound+0x57e8);
        FloatingEnvironment::prepare();const int expected_return=original<int>(0x4f9c30,&owner);const auto expected=owner_storage;const auto expected_session=native_session;const Snapshot expected_sprites(*sprites);const auto expected_generation=file.fields_5c[3];const auto expected_sound=std::vector<std::uint8_t>(sound,sound+0x57e8);
        owner_storage=before;native_session=before_session;before_sprites.restore(*sprites);file.fields_5c[3]=generation;std::memcpy(sound,before_sound.data(),before_sound.size());FloatingEnvironment::prepare();const int actual_return=pl::update_movement(owner,move_host);
        if(owner_storage!=expected&&failed<10){std::size_t off=0;while(owner_storage[off]==expected[off])++off;std::cerr<<"Player movement case "<<n<<" byte "<<std::hex<<off<<std::dec<<'\n';}
        check("Player_alive_movement_all84060bytes",owner_storage==expected);check("Player_alive_movement_return_and_PlayerTable",actual_return==expected_return&&std::memcmp(&native_session,&expected_session,sizeof(native_session))==0);check("Player_alive_movement_ANM_slots",std::memcmp(sprites->pool,expected_sprites.slots,sizeof(expected_sprites.slots))==0);check("Player_alive_movement_sound_and_generation",std::memcmp(sound,expected_sound.data(),expected_sound.size())==0&&file.fields_5c[3]==expected_generation);
    }
    #include "death_state_cpu_cases.inc"
    **reinterpret_cast<float**>(mapped_image_base+0x1aefe0)=old_rate;put(reinterpret_cast<void*>(mapped_image_base),0x1aefe4,old_clock);put(reinterpret_cast<void*>(mapped_image_base),0x1b8898,saved_input);std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1b889c),saved_slots.data(),sizeof(saved_slots));
}
