{
    // The 4f7430 body is unmodified. Only the listed subsystem endpoints are
    // replaced in this isolated oracle image; both paths record identical calls.
    // The real Movement, Death, ShotController and Feedback bodies remain active.
    struct Endpoints {
        std::vector<std::array<std::uint32_t,12>> calls;bool bomb_allowed=false;
        static Endpoints*& current(){static Endpoints* value=nullptr;return value;}
        static void record(unsigned kind,void* self,const sp::Vec3* p,std::initializer_list<std::uint32_t> args){std::array<std::uint32_t,12> row{};row[0]=kind;row[1]=reinterpret_cast<std::uintptr_t>(self);if(p)std::memcpy(row.data()+2,p,12);std::copy(args.begin(),args.end(),row.begin()+5);current()->calls.push_back(row);}
        static int __fastcall bullet(void* self,void*,const sp::Vec3* p,float radius,int drop,int limit,unsigned kind){record(1,self,p,{std::bit_cast<unsigned>(radius),unsigned(drop),unsigned(limit),kind});return 0;}
        static void __fastcall near_bullet(void* self,void*,const sp::Vec3* p,float radius,int drop){record(2,self,p,{std::bit_cast<unsigned>(radius),unsigned(drop)});}
        static int __fastcall laser(void* self,void*,const sp::Vec3* p,float radius,int drop,int kind){record(3,self,p,{std::bit_cast<unsigned>(radius),unsigned(drop),unsigned(kind)});return 0;}
        static void __fastcall erase(void* self,void*,int first,int second){record(4,self,nullptr,{unsigned(first),unsigned(second)});}
        static unsigned* __fastcall damage(void* self,void*,unsigned* output,const sp::Vec3* p,float radius,float growth,int duration,int amount){record(5,self,p,{std::bit_cast<unsigned>(radius),std::bit_cast<unsigned>(growth),unsigned(duration),unsigned(amount)});*output=0x10023;return output;}
        static unsigned* __fastcall activate(unsigned* handle,void*,unsigned* output){record(6,nullptr,nullptr,{*handle});*output=*handle;return output;}
        static void* __fastcall item(void* self,void*,int kind,const sp::Vec3* p,unsigned color,float angle,float speed,int delay,int extra,int sound){record(7,self,p,{unsigned(kind),color,std::bit_cast<unsigned>(angle),std::bit_cast<unsigned>(speed),unsigned(delay),unsigned(extra),unsigned(sound)});return nullptr;}
        static bool __fastcall can_bomb(void* self,void*){record(8,self,nullptr,{});return current()->bomb_allowed;}
        static int __fastcall bomb(void* self,void*){record(9,self,nullptr,{});return 0;}
        static void __fastcall finish(void* self,void*){record(10,self,nullptr,{});}
        static void __fastcall lives(void* self,void*,int first,int second,int third){record(11,self,nullptr,{unsigned(first),unsigned(second),unsigned(third)});}
        static void __fastcall bombs(void* self,void*,int first,int second,int third){record(12,self,nullptr,{unsigned(first),unsigned(second),unsigned(third)});}
    } endpoints;Endpoints::current()=&endpoints;
    struct BoundaryPatch {
        std::uint8_t* address;std::array<std::uint8_t,5> before;
        BoundaryPatch(unsigned va,void* callback):address(reinterpret_cast<std::uint8_t*>(mapped_image_base+va-0x400000)){std::memcpy(before.data(),address,5);DWORD old;VirtualProtect(address,5,PAGE_EXECUTE_READWRITE,&old);address[0]=0xe9;const auto offset=std::uint32_t(reinterpret_cast<std::uintptr_t>(callback)-reinterpret_cast<std::uintptr_t>(address)-5);std::memcpy(address+1,&offset,4);FlushInstructionCache(GetCurrentProcess(),address,5);}
        ~BoundaryPatch(){std::memcpy(address,before.data(),5);FlushInstructionCache(GetCurrentProcess(),address,5);}
    } p1(0x47cf60,reinterpret_cast<void*>(&Endpoints::bullet)),p2(0x47d100,reinterpret_cast<void*>(&Endpoints::near_bullet)),p3(0x4caad0,reinterpret_cast<void*>(&Endpoints::laser)),p4(0x4c9490,reinterpret_cast<void*>(&Endpoints::erase)),p5(0x4c1b80,reinterpret_cast<void*>(&Endpoints::damage)),p6(0x4c1c10,reinterpret_cast<void*>(&Endpoints::activate)),p7(0x4c3c90,reinterpret_cast<void*>(&Endpoints::item)),p8(0x4780a0,reinterpret_cast<void*>(&Endpoints::can_bomb)),p9(0x477e30,reinterpret_cast<void*>(&Endpoints::bomb)),p10(0x4e5bd0,reinterpret_cast<void*>(&Endpoints::finish)),p11(0x4b8bf0,reinterpret_cast<void*>(&Endpoints::lives)),p12(0x4b8650,reinterpret_cast<void*>(&Endpoints::bombs));
    struct ShotFrames final:pl::ShotFrameServices {
        Callbacks& env;explicit ShotFrames(Callbacks& e):env(e){}pl::ShotCallbackEnvironment& callbacks() override{return env;}
        void corners(sp::Animation& a,sp::Vec3(&out)[4]) override{original<void>(0x445770,&a,out);}int viewport_x() override{return 0;}int viewport_y() override{return 0;}
    } shot_frames(callback_env);
    struct Shooting final:pl::ShotControllerServices {
        ShotFrames& frames_;MoveHost& moves;Shooting(ShotFrames& f,MoveHost& m):frames_(f),moves(m){}
        pl::ShotFrameServices& frames() override{return frames_;}bool input_blocked() override{return original<unsigned>(0x4ab2e0,*reinterpret_cast<void**>(mapped_image_base+0x1ba828))!=0;}
        bool world_allows_shooting() override{if(original<bool>(0x478160,*reinterpret_cast<void**>(mapped_image_base+0x1c06a4)))return false;if(reinterpret_cast<int(__cdecl*)()>(mapped_image_base+0xbd850)()||*reinterpret_cast<void**>(mapped_image_base+0x1c6114))return false;return original<unsigned>(0x4ab270,*reinterpret_cast<void**>(mapped_image_base+0x1ba828))==0;}
        int input_slot(int view) override{return moves.input_slot(view);}unsigned held(int slot,unsigned mask) override{return moves.held(slot,mask);}
        void shoot_weapons(gs::Context& c,int first,int second,int power) override{original<void>(0x5331e0,c.overlay_owner,first,second,power);}
        void activate_weapons(gs::Context& c,bool inactive) override{original<void>(0x5330a0,c.overlay_owner,char(inactive));}
    } shooting(shot_frames,move_host);
    struct WholeFrame final:pl::FrameServices {
        MoveHost& moves;DeathHost& deaths;Shooting& shooting;WholeFrame(MoveHost& m,DeathHost& d,Shooting& s):moves(m),deaths(d),shooting(s){}
        pl::MovementServices& movement() override{return moves;}pl::DeathServices& death() override{return deaths;}pl::ShotControllerServices& shots() override{return shooting;}
        void select_view(int index) override{original<void>(0x4776a0,*reinterpret_cast<void**>(mapped_image_base+0x1c0028),index);}
        unsigned pressed(int slot,unsigned mask) override{if(slot<0)return 0;auto* buttons=reinterpret_cast<void**>(mapped_image_base+0x1b889c)[slot];return buttons?original<unsigned>(0x4b58e0,buttons,mask):0;}
        bool bomb_exists(gs::Context& c) override{return c.objects_04[5]!=nullptr;}bool can_bomb(gs::Context& c) override{return original<bool>(0x4780a0,c.objects_04[5]);}void trigger_bomb(gs::Context& c) override{original<void>(0x477e30,c.objects_04[5]);}
        void cancel_bullets(gs::Context& c,const sp::Vec3& p,float radius) override{original<void>(0x47cf60,c.primary_owner,&p,radius,0,99999,0u);}
        void cancel_near_bullets(gs::Context& c,const sp::Vec3& p,float radius) override{original<void>(0x47d100,c.primary_owner,&p,radius,0);}
        void cancel_lasers(gs::Context& c,const sp::Vec3& p,float radius,int kind) override{original<void>(0x4caad0,c.objects_04[4],&p,radius,0,kind);}
        void finish_lasers(gs::Context& c,int a,int b) override{original<void>(0x4c9490,c.objects_04[4],a,b);}
        void create_damage(gs::Context& c,const sp::Vec3& p,float r,float g,int duration,int amount,bool activate) override{unsigned handle;original<void>(0x4c1b80,c.object_28,&handle,&p,r,g,duration,amount);if(activate){unsigned out;original<void>(0x4c1c10,&handle,&out);}}
        void spawn_power_item(const sp::Vec3& p,float angle) override{original<void>(0x4c3c90,moves.options().callbacks().firing().session().contexts[0].objects_04[2],1,&p,0xffffffffu,angle,3.f,0,0,-1);}
        bool replay_playing() override{return original<bool>(0x488800,*reinterpret_cast<void**>(mapped_image_base+0x1c60fc));}void finish_game() override{original<void>(0x4e5bd0,*reinterpret_cast<void**>(mapped_image_base+0x1c60bc));}
        void set_bombs(gs::Player& p,int amount) override{original<void>(0x4e15e0,&p,amount);}void clock_scale(float value) override{original<void>(0x4292a0,reinterpret_cast<void*>(mapped_image_base+0x1aefe4),value);}
        void execute_animation(sp::Animation& a) override{original<void>(0x42b5d0,&a);}float sample_expansion(sp::Interpolation<float>& value) override{return original<float>(0x42a110,&value);}
    } whole_frame(move_host,death_host,shooting);
    struct Weapon {
        void** methods;unsigned calls=0;
        static void __fastcall activate(Weapon* self,void*){++self->calls;}
        static void __fastcall shoot(Weapon* self,void*,int,int,int){++self->calls;}
    };void* weapon_methods[30]{};for(unsigned i=2;i<5;++i)weapon_methods[i]=reinterpret_cast<void*>(&Weapon::shoot);for(unsigned i=7;i<10;++i)weapon_methods[i]=reinterpret_cast<void*>(&Weapon::activate);
    std::array<Weapon,3> weapons{};for(auto& w:weapons)w.methods=weapon_methods;put(overlay.data(),0x28,&weapons[0]);put(overlay.data(),0x2c,&weapons[1]);put(overlay.data(),0x30,&weapons[2]);
    const auto old_game=*reinterpret_cast<void**>(mapped_image_base+0x1ba828),old_replay=*reinterpret_cast<void**>(mapped_image_base+0x1c60fc),old_clear=*reinterpret_cast<void**>(mapped_image_base+0x1c6114);
    std::array<std::uint8_t,0x110> game{};std::array<std::uint8_t,0x360> replay{};put(reinterpret_cast<void*>(mapped_image_base),0x1ba828,game.data());put(reinterpret_cast<void*>(mapped_image_base),0x1c60fc,replay.data());put(reinterpret_cast<void*>(mapped_image_base),0x1c6114,static_cast<void*>(nullptr));put(reinterpret_cast<void*>(mapped_image_base),0x1c06a4,laser_hud.data());put(laser_hud.data(),0x1bc,0u);put(enemies_owner.data(),0x124,1u);
    for(unsigned n=0;n<5600;++n){firing_case=n;firing_phase="Player entire main frame";
        for(auto& list:sprites->lists){list={};list.tail=&list.sentinel;}sprites->free_sentinel={};sprites->field_7d40e88=2;sprites->field_6c4=0;
        for(unsigned i=0;i<4;++i){auto& pool=sprites->pool[i];pool={};sp::construct_pooled_animation(pool);pool.index=i;pool.free_link.value=&pool.animation;pool.active=i<2;pool.animation.handle=0x10000u+i;if(i>=2)sq::insert_after(reinterpret_cast<sq::Link&>(sprites->free_sentinel),reinterpret_cast<sq::Link&>(pool.free_link));}
        pl::construct_player_fields(owner);owner.shot_data=data.data();owner.animation_file=&file;owner.context=&native_session.contexts[0];owner.shots.context=owner.context;owner.feedback.context=owner.context;owner.state=n%8;owner.position_614={finite(),finite(),finite()};owner.fixed_position={int(rng()%20000)-10000,int(rng()%60000)};owner.vector_638={finite(),finite(),finite()};owner.entity_flags=rng()&0x43fu;owner.fields_20e4[2]=std::bit_cast<unsigned>(float(n%12)*.25f);owner.field_2204=n%10;owner.handle_18=std::bit_cast<unsigned>(finite());owner.collision_expansion=finite();owner.vector_20f0={finite(),finite(),finite()};
        const int times[]{0,1,3,4,8,14,15,16,29,30,31,39,40,59,60,61,-1};const int time=times[(n/8)%17];owner.timers_644[0]={time-int(n%2),time,float(time)+.25f,0};owner.timers_644[1]={2,3,3.5f,0};owner.timers_2050[0]={0,int(n%5)-2,float(int(n%5)-2),0};owner.timers_2050[1]={0,int(n%7)-3,float(int(n%7)-3),0};owner.shots.timer_12420.current=0;owner.shots.timer_12420.current_f=0;for(auto& t:owner.feedback.timers){t={0,int(n%80)-4,float(int(n%80)-4),0};}owner.feedback.handle_4c=0x10000;owner.feedback.enabled=n%2;
        for(auto& script:owner.animation_scripts)script=0;for(auto& speed:owner.speeds_20b4)speed=256;owner.handle_60c=0x10001;owner.interpolation_220c={finite(),finite(),finite(),finite(),finite(),{0,int(n%12),float(n%12)+.25f,0},int(n%11),n%18};owner.animation.base.flags[2]=1;move_host.bind_script(owner,0); //actual stopped ANM, no GPU access
        const float rate=float(n%5)*.5f;**reinterpret_cast<float**>(mapped_image_base+0x1aefe0)=rate;put(reinterpret_cast<void*>(mapped_image_base),0x1aefe4,rate);put(input_bytes.data(),0x2e30,n%3?-1:0);put(input_buttons.data(),0x29c,rng()&0xffu);put(input_buttons.data(),0x2a8,rng()&7u);put(game.data(),0xe8,n%5?0u:0x80000u);put(replay.data(),0x10,int(n%2));endpoints.bomb_allowed=n%3!=0;native_session.contexts[0].objects_04[5]=n%11?bomb.data():nullptr;
        auto& record=native_session.player_table.players[0];put(&record,0x30,int((n/8)%6)*100);put(&record,0x38,100);put(&record,0xb8,int(n%12)-3);put(&record,0xcc,int(n%15)-3);put(&record,0xd4,int(n%15)-3);put(&record,0x40,2);put(&record,0x44,3);put(&record,0xc8,10);native_session.mode=2;{const int power=std::min(int((n/8)%6)*100,400);const int level=power/100;const int loss=level>=4?80:level>=3?60:level>=2?50:40;owner.fields_674[3]=unsigned(power>100?std::max(power-loss,100)/100:level);}
        std::fill(std::begin(effects_owner.handles),std::end(effects_owner.handles),0u);put(&effects_owner,0x20,n%1024);std::memset(sound,0,0x57e8);for(unsigned i=0;i<12;++i)put(sound,0x1c+i*0x208,-1);for(auto& w:weapons)w.calls=0;endpoints.calls.clear();
        const auto before=owner_storage;const auto before_session=native_session;const auto before_enemy=enemies_owner;const auto before_effects=effects_storage;const auto before_card=card;const Snapshot before_sprites(*sprites);const auto generation=file.fields_5c[3];const auto before_sound=std::vector<std::uint8_t>(sound,sound+0x57e8);
        FloatingEnvironment::prepare();const int expected_return=original<int>(0x4f7430,&owner);const auto expected=owner_storage;const auto expected_session=native_session;const auto expected_enemy=enemies_owner;const auto expected_effects=effects_storage;const auto expected_card=card;const Snapshot expected_sprites(*sprites);const auto expected_generation=file.fields_5c[3];const auto expected_sound=std::vector<std::uint8_t>(sound,sound+0x57e8);const auto expected_weapons=weapons;const auto expected_calls=endpoints.calls;const float expected_rate=*reinterpret_cast<float*>(mapped_image_base+0x1aefe4);
        owner_storage=before;native_session=before_session;enemies_owner=before_enemy;effects_storage=before_effects;card=before_card;before_sprites.restore(*sprites);file.fields_5c[3]=generation;std::memcpy(sound,before_sound.data(),before_sound.size());**reinterpret_cast<float**>(mapped_image_base+0x1aefe0)=rate;put(reinterpret_cast<void*>(mapped_image_base),0x1aefe4,rate);for(auto& w:weapons)w.calls=0;endpoints.calls.clear();FloatingEnvironment::prepare();const int actual_return=pl::update_player(owner,whole_frame);
        if(owner_storage!=expected&&failed<10){std::size_t off=0;while(owner_storage[off]==expected[off])++off;std::cerr<<"Player full frame case "<<n<<" state "<<n%8<<" time "<<time<<" byte "<<std::hex<<off<<std::dec<<'\n';}
        check("Player_main_frame_return",actual_return==expected_return);check("Player_main_frame_all84060bytes",owner_storage==expected);check("Player_main_frame_session_enemy_effect_Card",std::memcmp(&native_session,&expected_session,sizeof(native_session))==0&&enemies_owner==expected_enemy&&effects_storage==expected_effects&&card==expected_card);check("Player_main_frame_ANM_generation",std::memcmp(sprites->pool,expected_sprites.slots,sizeof(expected_sprites.slots))==0&&file.fields_5c[3]==expected_generation);check("Player_main_frame_sound_weapons_clock",std::memcmp(sound,expected_sound.data(),expected_sound.size())==0&&std::memcmp(&weapons,&expected_weapons,sizeof(weapons))==0&&*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)==expected_rate);check("Player_main_frame_external_call_sequence",endpoints.calls==expected_calls);
    }
    put(reinterpret_cast<void*>(mapped_image_base),0x1ba828,old_game);put(reinterpret_cast<void*>(mapped_image_base),0x1c60fc,old_replay);put(reinterpret_cast<void*>(mapped_image_base),0x1c6114,old_clear);put(reinterpret_cast<void*>(mapped_image_base),0x1c06a4,static_cast<void*>(nullptr));
}

