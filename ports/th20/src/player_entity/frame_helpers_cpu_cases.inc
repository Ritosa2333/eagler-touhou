{
    struct FrameHost final:pl::ShotFrameServices {
        Callbacks& env;explicit FrameHost(Callbacks& e):env(e){}
        pl::ShotCallbackEnvironment& callbacks() override{return env;}
        void corners(sp::Animation& a,sp::Vec3(&values)[4]) override{original<void>(0x445770,&a,values);}
        int viewport_x() override{return *reinterpret_cast<int*>(mapped_image_base+0x1b6758+0x58);}
        int viewport_y() override{return *reinterpret_cast<int*>(mapped_image_base+0x1b6758+0x5c);}
    } frame_host(callback_env);
    const float saved_rate=**reinterpret_cast<float**>(mapped_image_base+0x1aefe0);
    const float saved_direct_rate=*reinterpret_cast<float*>(mapped_image_base+0x1aefe4);
    const auto saved_window_x=frame_host.viewport_x(),saved_window_y=frame_host.viewport_y();
    for(unsigned n=0;n<4800;++n){firing_case=n;firing_phase="Player fixed motion and feedback";
        pl::construct_player_fields(owner);owner.shot_data=data.data();owner.context=&native_session.contexts[0];native_session.contexts[0].objects_04[0]=&owner;
        const float rates[]{0,.25f,.5f,1,1.25f,2,-1};const float rate=rates[n%7];
        **reinterpret_cast<float**>(mapped_image_base+0x1aefe0)=rate;put(reinterpret_cast<void*>(mapped_image_base),0x1aefe4,rate);
        owner.entity_flags=rng();owner.fields_674[2]=n%2;owner.fixed_position={int(rng()),int(rng())};owner.position_614={finite(),finite(),sample(n)};owner.vector_20c4.z=sample(n);owner.vector_20d0={sample(n),sample(n+1),sample(n+2)};
        const int dx=n%2?int(rng()):int(finite()*128),dy=n%2?int(rng()):int(finite()*128);
        const auto before=owner_storage;FloatingEnvironment::prepare();original<void>(0x4ffa10,&owner,dx,dy);const auto expected=owner_storage;owner_storage=before;
        FloatingEnvironment::prepare();pl::advance_fixed_motion(owner,dx,dy,rate);check("Player_fixed_motion_all84060bytes",owner_storage==expected);
        owner.position_614={n%3?finite()*5:-140.0f+float(int(n%3)-1),n%4?finite()*3:80.0f+float(int(n%3)-1),0};auto& feedback=owner.feedback;
        feedback.enabled=n%2;feedback.handle_4c=n%4?0x10000u:0u;for(auto& t:feedback.timers){const int now=int(rng()%90)-20;t={now-1,now,float(now)+.25f,0};}feedback.fields_30[1]=rng();feedback.fields_30[2]=rng();
        animation.active=1;animation.animation.handle=0x10000u;animation.animation.base.vector_50={finite(),finite()};
        const auto before_feedback=owner_storage;const auto before_anm=sprite_bytes;FloatingEnvironment::prepare();original<void>(0x4f7210,&feedback);const auto expected_feedback=owner_storage;const auto expected_anm=sprite_bytes;owner_storage=before_feedback;sprite_bytes=before_anm;
        FloatingEnvironment::prepare();pl::update_feedback(feedback,callback_env);check("Player_feedback_full_object_and_ANM",owner_storage==expected_feedback&&sprite_bytes==expected_anm);
        const int vx=int(rng()%200)-100,vy=int(rng()%200)-100;put(reinterpret_cast<void*>(mapped_image_base),0x1b6758+0x58,vx);put(reinterpret_cast<void*>(mapped_image_base),0x1b6758+0x5c,vy);
        sp::Vec3 corners[4];for(auto& corner:corners)corner={finite()*20,finite()*20,sample(n)};if(n%3==0)corners[n%4].x=std::bit_cast<float>(0x7fc12345u);if(n%7==0)corners[n%4]={float(vx)-192,float(vy),0};
        const auto expected_outside=reinterpret_cast<int(__cdecl*)(const sp::Vec3*)>(mapped_image_base+0x106570)(corners);
        check("Shot_corner_viewport_test",int(pl::shot_corners_outside(corners,vx,vy))==expected_outside);
    }
    for(unsigned n=0;n<4000;++n){firing_case=n;firing_phase="Shot complete single frame";
        pl::construct_player_fields(owner);owner.shot_data=data.data();owner.context=&native_session.contexts[0];native_session.contexts[0].objects_04[0]=&owner;auto& shot=owner.shots.pool[0];pl::select_context(shot,0,native_session);
        const float rate=float(n%7)*.25f;**reinterpret_cast<float**>(mapped_image_base+0x1aefe0)=rate;put(reinterpret_cast<void*>(mapped_image_base),0x1aefe4,rate);
        row={};row.type=n%3==0?0:(n%2?2:8);row.field_18=finite();row.fields_20[0]=std::bit_cast<std::uint32_t>(finite());row.field_34=n%5?0u:20u;
        shot.timer_1c={int(n%40)-1,int(n%40),float(n%40)+.25f,0};shot.flags=1;shot.fields_3c[0]=rng();shot.fields_3c[2]=0;shot.motion.field_18=finite();shot.motion.angle_1c=finite()*20;shot.motion.position={finite(),finite(),finite()};shot.motion.vector_38={finite(),finite(),finite()};shot.fields_98[1]=n%3;shot.fields_98[5]=rng();shot.vector_b0={finite(),finite()};shot.fields_b8[8]=0;shot.fields_b8[9]=n%3?1u:0u;shot.handle_18=n%11?0x10000u:0u;shot.owner=&owner.shots;
        sq::initialize_list(owner.shots.active);sq::initialize_list(owner.shots.free);sq::initialize_link(shot.link,reinterpret_cast<sq::Node*>(&shot));sq::append(owner.shots.active,shot.link);
        original<void>(0x4bffb0,&damage_owner.pool[0]);sq::initialize_list(damage_owner.active);sq::initialize_list(damage_owner.free);sq::initialize_link(damage_owner.pool[0].link,reinterpret_cast<sq::Node*>(&damage_owner.pool[0]));sq::append(damage_owner.active,damage_owner.pool[0].link);damage_owner.pool[0].handle=1;damage_owner.pool[0].context=&native_session.contexts[0];
        sp::construct_animation(animation.animation);animation.active=1;animation.animation.handle=0x10000u;animation.animation.base.flags[0]=4;animation.animation.base.flags[2]=(n%8)<<21;animation.animation.base.vector_50={finite(),finite()};
        if(n%5==1)shot.fields_3c[2]=mapped_image_base+0x102b10; // actual update callback, also exercised independently above
        const auto before=owner_storage;const auto before_anm=sprite_bytes;const auto before_damage=damage_bytes;
        FloatingEnvironment::prepare();original<void>(0x504480,&shot);const auto expected=owner_storage;const auto expected_anm=sprite_bytes;const auto expected_damage=damage_bytes;owner_storage=before;sprite_bytes=before_anm;damage_bytes=before_damage;
        FloatingEnvironment::prepare();pl::update_shot(shot,frame_host);
        if(owner_storage!=expected&&failed<10){std::size_t off=0;while(owner_storage[off]==expected[off])++off;std::cerr<<"Shot frame case "<<n<<" byte "<<std::hex<<off<<std::dec<<'\n';}
        if(sprite_bytes!=expected_anm&&failed<10){std::size_t off=0;while(sprite_bytes[off]==expected_anm[off])++off;std::cerr<<"Shot frame ANM case "<<n<<" byte "<<std::hex<<off<<std::dec<<'\n';}
        check("Shot_single_frame_all84060bytes",owner_storage==expected);check("Shot_single_frame_ANM",sprite_bytes==expected_anm);check("Shot_single_frame_damage",damage_bytes==expected_damage);
    }
    #include "shot_controller_frame_cpu_cases.inc"
    **reinterpret_cast<float**>(mapped_image_base+0x1aefe0)=saved_rate;put(reinterpret_cast<void*>(mapped_image_base),0x1aefe4,saved_direct_rate);put(reinterpret_cast<void*>(mapped_image_base),0x1b6758+0x58,saved_window_x);put(reinterpret_cast<void*>(mapped_image_base),0x1b6758+0x5c,saved_window_y);
}
