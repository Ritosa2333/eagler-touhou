{
    struct DeathSpawn final:pl::ShotHitServices {
        Callbacks& env;explicit DeathSpawn(Callbacks& e):env(e){}
        pl::ShotCallbackEnvironment& callbacks() override{return env;}
        int default_hit(pl::Shot&) override{throw std::logic_error("Death has no shot hit");}
        float random_signed(unsigned) override{throw std::logic_error("Death has no RNG");}
        std::uint32_t random_bounded(unsigned,std::uint32_t) override{throw std::logic_error("Death has no RNG");}
        sp::AnimationFile& effect_file(gs::Context&) override{throw std::logic_error("Death selects explicit file");}
        std::uint32_t spawn(sp::AnimationFile& file,const char* name,int script,const sp::Vec3& position,float angle) override{std::uint32_t handle;original<void>(0x4790e0,&file,&handle,name,script,&position,angle,-1,static_cast<void*>(nullptr));return handle;}
        dm::Region* create_circle(gs::Context&,const sp::Vec3&,float,float,int,int) override{throw std::logic_error("Death has no region");}
        void remember_effect(gs::Context&,std::uint32_t) override{throw std::logic_error("Death reserves effect first");}
    } death_spawn(callback_env);
    struct DeathHost final:pl::DeathServices {
        MoveHost& moves;DeathSpawn& spawn;DeathHost(MoveHost& m,DeathSpawn& s):moves(m),spawn(s){}
        pl::MovementServices& movement() override{return moves;}
        void mark_enemies() override{original<void>(0x478190,*reinterpret_cast<void**>(mapped_image_base+0x1ba568+8));}
        void spawn_effect(pl::Player& player) override{pl::spawn_tracked_effect(*static_cast<th20::source::effects::Controller*>(player.context->objects_04[7]),0,21,player.position_614,0,spawn);}
        void add_lives(gs::Player& p,int amount) override{original<void>(0x4e1250,&p,amount);}
        void add_meter(gs::Player& p,int amount) override{original<void>(0x477f60,&p,amount);}
        void notify_card(gs::Context& context) override{original<void>(0x487bb0,context.objects_04[3]);}
    } death_host(move_host,death_spawn);
    const auto saved_hud=*reinterpret_cast<void**>(mapped_image_base+0x1c06a4);put(reinterpret_cast<void*>(mapped_image_base),0x1c06a4,static_cast<void*>(nullptr));native_session.mode=2;
    std::vector<std::uint8_t> effects_storage(0x13044);auto& effects_owner=*reinterpret_cast<th20::source::effects::Controller*>(effects_storage.data());effects_owner.files[0]=&file;native_session.contexts[0].objects_04[7]=&effects_owner;
    std::array<std::uint8_t,0xc4> card{};std::array<std::uint8_t,0x3c> bomb{};native_session.contexts[0].objects_04[3]=card.data();native_session.contexts[0].objects_04[5]=bomb.data();put(card.data(),0xc0,&native_session.contexts[0]);
    sq::initialize_list(enemies);
    for(unsigned n=0;n<2600;++n){firing_case=n;firing_phase="Player death state and counters";
        for(auto& list:sprites->lists){list={};list.tail=&list.sentinel;}sprites->free_sentinel={};sprites->field_7d40e88=2;sprites->field_6c4=0;
        for(unsigned i=0;i<4;++i){auto& pool=sprites->pool[i];pool={};sp::construct_pooled_animation(pool);pool.index=i;pool.free_link.value=&pool.animation;pool.active=i<2;pool.animation.handle=0x10000u+i;if(i>=2)sq::insert_after(reinterpret_cast<sq::Link&>(sprites->free_sentinel),reinterpret_cast<sq::Link&>(pool.free_link));}
        pl::construct_player_fields(owner);owner.shot_data=data.data();owner.animation_file=&file;owner.context=&native_session.contexts[0];owner.position_614={finite(),finite(),0};owner.entity_flags=rng();for(auto& option:owner.options){option.state=n%4;option.handle_dc=0x10000;option.handle_e0=0x10001;}for(auto& option:owner.secondary_options){option.state=n%4;option.handle_dc=0x10001;option.handle_e0=0x10000;}
        auto& p=native_session.player_table.players[0];for(unsigned off:{0x74u,0x78u,0x7cu,0x80u,0xc8u,0x5cu})put(&p,off,int(rng()));put(&p,0x40,int(n%8));put(&p,0x44,int(n%10));put(enemies_owner.data(),0x48,rng());put(enemies_owner.data(),0x50,rng());native_session.player_table.fields_1ec[7]=rng();put(card.data(),0x78,rng());put(card.data(),0x28,int(n%90));put(card.data(),0x7c,rng());put(bomb.data(),0x18,int(n%3));
        std::fill(std::begin(effects_owner.handles),std::end(effects_owner.handles),0u);put(&effects_owner,0x20,n%1024);
        const auto before=owner_storage;const auto before_session=native_session;const auto before_enemy=enemies_owner;const auto before_effects=effects_storage;const auto before_card=card;const Snapshot before_sprites(*sprites);const auto generation=file.fields_5c[3];
        FloatingEnvironment::prepare();original<void>(0x4f8420,&owner);const auto expected=owner_storage;const auto expected_session=native_session;const auto expected_enemy=enemies_owner;const auto expected_effects=effects_storage;const auto expected_card=card;const Snapshot expected_sprites(*sprites);const auto expected_generation=file.fields_5c[3];
        owner_storage=before;native_session=before_session;enemies_owner=before_enemy;effects_storage=before_effects;card=before_card;before_sprites.restore(*sprites);file.fields_5c[3]=generation;FloatingEnvironment::prepare();pl::begin_death(owner,death_host);
        if(owner_storage!=expected&&failed<10){std::size_t off=0;while(owner_storage[off]==expected[off])++off;std::cerr<<"Player death case "<<n<<" byte "<<std::hex<<off<<std::dec<<'\n';}
        check("Player_death_full84060bytes",owner_storage==expected);check("Player_death_PlayerTable_and_enemy",std::memcmp(&native_session,&expected_session,sizeof(native_session))==0&&enemies_owner==expected_enemy);check("Player_death_effect_owner_and_Card",effects_storage==expected_effects&&card==expected_card);check("Player_death_ANM_slots_and_generation",std::memcmp(sprites->pool,expected_sprites.slots,sizeof(expected_sprites.slots))==0&&file.fields_5c[3]==expected_generation);
        const int amount=int(rng());const auto saved_player=p;put(&p,0x30,int(rng()%10000)-100);put(&p,0x38,int(rng()%400)+1);const auto power_before=p;const bool expected_power=original<bool>(0x4e1680,&p,amount);const auto power_after=p;p=power_before;const bool actual_power=pl::subtract_power(p,amount);check("Player_subtract_power_all240bytes_and_result",std::memcmp(&p,&power_after,sizeof(p))==0&&actual_power==expected_power);p=saved_player;
        for(unsigned slot=0;slot<4;++slot){const auto before_record=p;const std::uint32_t addresses[]{0x4fea60,0x4fe9b0,0x4feab0,0x4fea00};original<void>(addresses[slot],&p,amount);const auto expected_record=p;p=before_record;pl::add_stone_level(p,slot,amount);check("Player_death_stone_level_clamp",std::memcmp(&p,&expected_record,sizeof(p))==0);}
        const auto before_record=p;original<void>(0x4fe950,&p,amount);const auto expected_record=p;p=before_record;pl::add_death_count(p,amount);check("Player_death_counter_clamp",std::memcmp(&p,&expected_record,sizeof(p))==0);
        const auto before_table=native_session.player_table;original<void>(0x4feb00,&native_session.player_table,amount);const auto expected_table=native_session.player_table;native_session.player_table=before_table;pl::add_total_deaths(native_session.player_table,amount);check("Player_total_death_counter_clamp",std::memcmp(&native_session.player_table,&expected_table,sizeof(expected_table))==0);
        const auto before_cancel=owner_storage;original<void>(0x4f86c0,&owner);const auto expected_cancel=owner_storage;owner_storage=before_cancel;pl::cancel_death(owner);check("Player_cancel_death_full84060bytes",owner_storage==expected_cancel);
    }
    #include "frame_cpu_cases.inc"
    put(reinterpret_cast<void*>(mapped_image_base),0x1c06a4,saved_hud);
}
