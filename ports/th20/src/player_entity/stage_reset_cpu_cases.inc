{
    struct ResetHost final:pl::StageResetServices {
        Callbacks& env;explicit ResetHost(Callbacks& e):env(e){}
        gs::Session& session() override{return env.host.state;}
        void refresh_power(pl::Player& player) override{original<void>(0x4faca0,&player,-1);}
        void interrupt(std::uint32_t handle,int event) override{env.interrupt(handle,event);}
        void delete_animation(std::uint32_t& handle) override{original<void>(0x44fcd0,&handle);}
        void hide_animation(sp::Animation& animation) override{original<void>(0x450160,&animation);}
    } reset_host(callback_env);
    native_session.contexts[0].current_player=&native_session.player_table.players[0];native_session.contexts[1].current_player=&native_session.player_table.players[1];
    native_session.contexts[1].objects_04[0]=&owner;
    for(unsigned n=0;n<2400;++n){firing_case=n;firing_phase="Player stage reset";
        pl::construct_player_fields(owner);owner.services=&owner_host;pl::select_context(owner,n%2,native_session);owner.shot_data=data.data();native_session.player_table.players[0]={};native_session.player_table.players[1]={};
        owner.fixed_position={int(rng()),int(rng())};owner.position_614={finite(),finite(),finite()};owner.entity_flags=rng();owner.fields_20e4[1]=rng();owner.handle_60c=n%3?0x10000u:0u;owner.handle_610=n%2?0x10000u:0u;
        owner.fields_674[3]=0;owner.interpolation_220c.duration=int(rng());owner.collision_expansion=finite();owner.clock_scale=finite();
        for(unsigned i=0;i<4;++i)put(data.data(),0x10+i*4,sample(n));
        for(auto& option:owner.options){option.handle_dc=n%2?0x10000u:0u;option.handle_e0=n%3?0u:0x10000u;}for(auto& option:owner.secondary_options){option.handle_dc=n%3?0u:0x10000u;option.handle_e0=n%2?0x10000u:0u;}
        native_session.flags=n%4;native_session.player_table.field_1e4=n%5==0?rng():std::uint32_t(int(n%4)-1);
        const auto before=owner_storage;const auto before_animation=sprite_bytes;const auto before_session=native_session;
        FloatingEnvironment::prepare();original<void>(0x4fb450,&owner);const auto expected=owner_storage;const auto expected_animation=sprite_bytes;const auto expected_session=native_session;
        owner_storage=before;sprite_bytes=before_animation;native_session=before_session;
        FloatingEnvironment::prepare();pl::reset_for_stage(owner,reset_host);
        if(owner_storage!=expected&&failed<10){std::size_t offset=0;while(owner_storage[offset]==expected[offset])++offset;std::cerr<<"Player reset case "<<n<<" byte "<<std::hex<<offset<<std::dec<<'\n';}
        check("Player_stage_reset_all84060bytes",owner_storage==expected);check("Player_stage_reset_animation_requests",sprite_bytes==expected_animation);check("Player_stage_reset_session_clamp",std::memcmp(&native_session,&expected_session,sizeof(native_session))==0);
        for(bool finish:{false,true}){const auto before_visibility=owner_storage;const auto before_visual=sprite_bytes;
            original<void>(finish?0x4bd0c0:0x4ff8f0,&owner);const auto expected_visibility=owner_storage;const auto expected_visual=sprite_bytes;owner_storage=before_visibility;sprite_bytes=before_visual;
            if(finish)pl::finish_stage_visibility(owner,reset_host);else pl::restore_stage_visibility(owner,reset_host);
            check("Player_option_visibility_full_object_and_ANM",owner_storage==expected_visibility&&sprite_bytes==expected_visual);
        }
    }
}
