{
    struct ControllerFrameHost final:pl::ShotControllerServices {
        FrameHost& frame;explicit ControllerFrameHost(FrameHost& f):frame(f){}
        pl::ShotFrameServices& frames() override{return frame;}
        bool input_blocked() override{return original<std::uint32_t>(0x4ab2e0,*reinterpret_cast<void**>(mapped_image_base+0x1ba828))!=0;}
        bool world_allows_shooting() override {
            if(original<bool>(0x478160,*reinterpret_cast<void**>(mapped_image_base+0x1c06a4)))return false;
            if(reinterpret_cast<int(__cdecl*)()>(mapped_image_base+0xbd850)()||*reinterpret_cast<void**>(mapped_image_base+0x1c6114))return false;
            return original<std::uint32_t>(0x4ab270,*reinterpret_cast<void**>(mapped_image_base+0x1ba828))==0;
        }
        int input_slot(int view) override{return original<int>(0x498fb0,*reinterpret_cast<void**>(mapped_image_base+0x1b8898),view);}
        std::uint32_t held(int slot,std::uint32_t mask) override{if(slot<0)return 0;auto* buttons=reinterpret_cast<void**>(mapped_image_base+0x1b889c)[slot];return buttons?original<std::uint32_t>(0x4973a0,buttons,mask):0;}
        void shoot_weapons(gs::Context& context,int primary,int secondary,int power) override{original<void>(0x5331e0,context.overlay_owner,primary,secondary,power);}
        void activate_weapons(gs::Context& context,bool inactive) override{original<void>(0x5330a0,context.overlay_owner,char(inactive));}
    } controller_host(frame_host);
    struct WeaponFixture {
        std::uintptr_t* methods;std::uint32_t shots,activations;int primary,secondary,power;
        static void __fastcall shoot(void* self,void*,int primary,int secondary,int power){auto& w=*static_cast<WeaponFixture*>(self);++w.shots;w.primary=primary;w.secondary=secondary;w.power=power;}
        static void __fastcall activate(void* self,void*){++static_cast<WeaponFixture*>(self)->activations;}
    };
    std::array<std::uintptr_t,30> methods{};for(unsigned i=2;i<=4;++i)methods[i]=reinterpret_cast<std::uintptr_t>(&WeaponFixture::shoot);for(unsigned i=7;i<=9;++i)methods[i]=reinterpret_cast<std::uintptr_t>(&WeaponFixture::activate);
    std::array<WeaponFixture,3> weapons{};for(auto& w:weapons)w.methods=methods.data();put(overlay.data(),0x28,&weapons[0]);put(overlay.data(),0x2c,&weapons[1]);put(overlay.data(),0x30,&weapons[2]);
    const auto saved_game=*reinterpret_cast<void**>(mapped_image_base+0x1ba828),saved_input=*reinterpret_cast<void**>(mapped_image_base+0x1b8898),saved_clear=*reinterpret_cast<void**>(mapped_image_base+0x1c6114);
    std::array<void*,4> saved_buttons{};std::memcpy(saved_buttons.data(),reinterpret_cast<void*>(mapped_image_base+0x1b889c),sizeof(saved_buttons));
    std::array<std::uint8_t,0x110> game_bytes{};std::array<std::uint8_t,0x2ef8> input_bytes{};std::array<std::array<std::uint8_t,0x2c0>,4> buttons{};
    put(reinterpret_cast<void*>(mapped_image_base),0x1ba828,game_bytes.data());put(reinterpret_cast<void*>(mapped_image_base),0x1b8898,input_bytes.data());
    native_session.contexts[0].current_player=&native_session.player_table.players[0];
    for(unsigned n=0;n<4800;++n){firing_case=n;firing_phase="ShotController firing and frame";
        pl::construct_player_fields(owner);owner.shot_data=data.data();pl::select_context(owner,0,native_session);owner.state=n%5==0?2:1;owner.focused_204c=n%2;owner.entity_flags=n%4?0u:rng()&0xffu;
        const float rate=float(n%7)*.25f;**reinterpret_cast<float**>(mapped_image_base+0x1aefe0)=rate;put(reinterpret_cast<void*>(mapped_image_base),0x1aefe4,rate);
        put(game_bytes.data(),0xe8,n%7==0?0x80000u:(n%11==0?0x80u:0u));put(input_bytes.data(),0x2e30,int(n%5)-1);
        for(unsigned i=0;i<4;++i){put(buttons[i].data(),0x29c,rng());put(reinterpret_cast<void*>(mapped_image_base),0x1b889c+i*4,n%7?buttons[i].data():nullptr);}
        put(laser_hud.data(),0x1bc,n%17?0u:1u);put(enemies_owner.data(),0x124,n%13?1u:0u);put(reinterpret_cast<void*>(mapped_image_base),0x1c6114,n%19?nullptr:game_bytes.data());
        auto& shots=owner.shots;shots.context=&native_session.contexts[0];shots.field_1255c=rng();shots.field_12578=rng();shots.byte_1257c=1;
        const int time0=int(n%24)-4,time1=int(n%150)-6,time2=int(n%35);shots.timer_12400={time0-int(n%2),time0,float(time0)+.25f,0};shots.timer_12410={time1-1,time1,float(time1)+.5f,0};shots.timer_12420={time2-1,time2,float(time2),0};
        auto& record=native_session.player_table.players[0];put(&record,0x58,int(rng()%30000)-5000);put(&record,0x64,int(rng()%2000)+1);put(&record,0x10,n%2);put(&record,0x14,n%3);
        sq::initialize_list(shots.active);sq::initialize_list(shots.free);row={};row.type=2;
        sp::construct_animation(animation.animation);animation.active=1;animation.animation.handle=0x10000u;animation.animation.base.flags[2]=1u<<21;
        for(unsigned i=0;i<n%5;++i){auto& shot=shots.pool[i];shot.owner=&shots;shot.context=&native_session.contexts[0];shot.flags=1;shot.handle_18=i==0&&n%3?0x10000u:0u;shot.motion.field_18=finite();shot.motion.angle_1c=finite();shot.fields_b8[8]=0;sq::initialize_link(shot.link,reinterpret_cast<sq::Node*>(&shot));sq::append(shots.active,shot.link);}
        for(auto& w:weapons){w.shots=0;w.activations=0;w.primary=w.secondary=w.power=0;}
        for(bool whole:{false,true}){
            const auto before=owner_storage;const auto before_anm=sprite_bytes;const auto before_session=native_session;const auto before_weapons=weapons;
            FloatingEnvironment::prepare();const int expected_return=whole?(original<void>(0x504210,&shots),0):original<int>(0x505a30,&shots);const auto expected=owner_storage;const auto expected_anm=sprite_bytes;const auto expected_session=native_session;const auto expected_weapons=weapons;
            owner_storage=before;sprite_bytes=before_anm;native_session=before_session;weapons=before_weapons;
            FloatingEnvironment::prepare();const int actual_return=whole?(pl::update_shot_controller(shots,controller_host),0):pl::update_shooting(shots,controller_host);
            if(owner_storage!=expected&&failed<10){std::size_t off=0;while(owner_storage[off]==expected[off])++off;std::cerr<<"ShotController frame case "<<n<<" whole "<<whole<<" byte "<<std::hex<<off<<std::dec<<'\n';}
            check("ShotController_frame_all84060bytes",owner_storage==expected);check("ShotController_frame_ANM_and_return",sprite_bytes==expected_anm&&actual_return==expected_return);check("ShotController_frame_PlayerTable",std::memcmp(&native_session,&expected_session,sizeof(native_session))==0);check("ShotController_frame_weapon_calls",std::memcmp(&weapons,&expected_weapons,sizeof(weapons))==0);
            owner_storage=before;sprite_bytes=before_anm;native_session=before_session;weapons=before_weapons;
        }
    }
    put(reinterpret_cast<void*>(mapped_image_base),0x1ba828,saved_game);put(reinterpret_cast<void*>(mapped_image_base),0x1b8898,saved_input);put(reinterpret_cast<void*>(mapped_image_base),0x1c6114,saved_clear);std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1b889c),saved_buttons.data(),sizeof(saved_buttons));
}
