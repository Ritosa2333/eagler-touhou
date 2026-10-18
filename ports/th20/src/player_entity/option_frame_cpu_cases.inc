{
    struct OptionPower final:pl::PowerServices {
        pl::Player& player;explicit OptionPower(pl::Player& p):player(p){}
        void select_view(int index) override{original<void>(0x4776a0,*reinterpret_cast<void**>(mapped_image_base+0x1c0028),index);}
        pl::Player& global_player() override{return player;}
        void interrupt(std::uint32_t handle,int event) override{original<void>(0x44ef90,&handle,event);}
        void delete_animation(std::uint32_t& handle) override{original<void>(0x44fcd0,&handle);}
        sp::Animation& animation(std::uint32_t& handle) override{return *original<sp::Animation*>(0x44ced0,&handle);}
        int script_variant() override{return original<int>(0x534130,player.context->overlay_owner);}
        sp::Vec2 option_offset(gs::Context&,int,int,bool) override{throw std::logic_error("Option uses already initialized offsets");}
        std::uint32_t spawn(sp::AnimationFile& file,int script,int layer,std::uint32_t flags) override{if(flags)throw std::logic_error("Option animation has no flags");std::uint32_t handle;original<void>(0x450c70,&file,&handle,static_cast<const char*>(nullptr),script,layer,static_cast<const sp::Vec3*>(nullptr));return handle;}
        void initialize_option(pl::Option&,int) override{throw std::logic_error("Option update does not reinitialize weapon strategy");}
        static int __fastcall variant(void*,void*){return 0;}
        static void __fastcall focused(pl::Option* option,void*){option->vector_70.x=std::bit_cast<int>(std::uint32_t(option->vector_70.x)+3);++option->field_d8;}
        static void __fastcall unfocused(pl::Option* option,void*){option->vector_70.y=std::bit_cast<int>(std::uint32_t(option->vector_70.y)-7);option->field_d8+=2;}
    } option_power(owner);
    struct OptionHost final:pl::OptionFrameServices {
        OptionPower& p;Callbacks& env;OptionHost(OptionPower& power,Callbacks& e):p(power),env(e){}
        pl::PowerServices& power() override{return p;}pl::ShotCallbackEnvironment& callbacks() override{return env;}
        void execute_interrupt(std::uint32_t handle,int event) override{original<void>(0x44f0d0,&handle,event);}
    } option_host(option_power,callback_env);
    auto* sprites=static_cast<sp::Controller*>(VirtualAlloc(nullptr,sizeof(sp::Controller),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));if(!sprites)throw std::bad_alloc();put(reinterpret_cast<void*>(mapped_image_base),0x1c0028,sprites);
    const std::uint32_t program[]{0x000c0005,0,2,0x00080002,0,0x0008ffff,0};auto* code=reinterpret_cast<sp::AnmInstruction*>(const_cast<std::uint32_t*>(program));
    std::array<sp::AnmInstruction*,22> scripts;for(auto& script:scripts)script=code;std::array<sp::Animation,22> templates;
    sp::AnimationFile file{};file.id=7;file.stem="effect";file.templates=templates.data();file.scripts=scripts.data();sprites->files[7]=&file;
    for(unsigned i=0;i<22;++i){auto& visual=templates[i];sp::construct_animation(visual);sp::reset_animation_state(visual);visual.base.fields_10_28[2]=7;visual.base.fields_10_28[3]=7;visual.base.fields_10_28[5]=i;visual.base.fields_10_28[6]=0;visual.base.flags[2]=1;}
    std::array<std::uintptr_t,20> methods{};methods[19]=reinterpret_cast<std::uintptr_t>(&OptionPower::variant);auto* table=methods.data();put(overlay.data(),0x2c,&table);put(overlay.data(),0x30,&table);
    struct Snapshot {
        sp::PooledAnimation slots[4];sp::Controller::AnimationList lists[3];sp::AnimationLink free;std::uint32_t generation;
        explicit Snapshot(const sp::Controller& c):free(c.free_sentinel),generation(c.field_7d40e88){std::memcpy(slots,c.pool,sizeof(slots));std::memcpy(lists,c.lists,sizeof(lists));}
        void restore(sp::Controller& c)const{std::memcpy(c.pool,slots,sizeof(slots));std::memcpy(c.lists,lists,sizeof(lists));c.free_sentinel=free;c.field_7d40e88=generation;}
    };
    native_session.contexts[0].objects_04[0]=&owner;native_session.contexts[0].current_player=&native_session.player_table.players[0];
    for(unsigned n=0;n<5200;++n){firing_case=n;firing_phase="Option complete update";
        for(auto& list:sprites->lists){list={};list.tail=&list.sentinel;}sprites->free_sentinel={};sprites->field_7d40e88=2;sprites->field_6c4=0;
        for(unsigned i=0;i<4;++i){auto& pool=sprites->pool[i];pool={};sp::construct_pooled_animation(pool);pool.index=i;pool.free_link.value=&pool.animation;pool.active=i<2;pool.animation.handle=0x10000u+i;if(i>=2)sq::insert_after(reinterpret_cast<sq::Link&>(sprites->free_sentinel),reinterpret_cast<sq::Link&>(pool.free_link));}
        pl::construct_player_fields(owner);owner.shot_data=data.data();owner.animation_file=&file;owner.context=&native_session.contexts[0];owner.focused_204c=n%2;owner.entity_flags=n%7?0u:2u;owner.fixed_position={int(rng()),int(rng())};owner.fields_20e4[0]=n%5?std::uint32_t(25+n%100):rng();owner.fields_20e4[1]=n%35;
        put(data.data(),0x88,0);put(data.data(),0xa8,1);auto& player_record=native_session.player_table.players[0];put(&player_record,0x30,int(n%500)-30);
        auto& option=owner.options[n%10];option.state=n%13?2u:0u;option.handle_dc=0x10000u;option.handle_e0=0x10001u;option.vector_70={int(rng()),int(rng())};option.vector_78={int(rng()),int(rng())};for(auto& offset:option.offsets_80)offset={int(rng()),int(rng())};option.fields_f4[2]=n%3;option.fields_f4[3]=n%11?0u:2u;option.fields_f4[4]=(rng()&0xffffff00u)|std::uint32_t(n%4?bool(owner.focused_204c):!owner.focused_204c);
        option.fields_f4[10]=n%3?reinterpret_cast<std::uint32_t>(&OptionPower::focused):0;option.fields_f4[11]=n%3?reinterpret_cast<std::uint32_t>(&OptionPower::unfocused):0;
        const auto before=owner_storage;const auto before_session=native_session;const Snapshot before_sprites(*sprites);const auto generation=file.fields_5c[3];
        FloatingEnvironment::prepare();const int expected_return=original<int>(0x4fa860,&owner,&option);const auto expected=owner_storage;const auto expected_session=native_session;const Snapshot expected_sprites(*sprites);const auto expected_file=file.fields_5c[3];owner_storage=before;native_session=before_session;before_sprites.restore(*sprites);file.fields_5c[3]=generation;
        FloatingEnvironment::prepare();const int actual_return=pl::update_option(owner,option,option_host);
        if(owner_storage!=expected&&failed<10){std::size_t off=0;while(owner_storage[off]==expected[off])++off;std::cerr<<"Option update case "<<n<<" byte "<<std::hex<<off<<std::dec<<'\n';}
        check("Option_update_all84060bytes",owner_storage==expected);check("Option_update_return_and_PlayerTable",actual_return==expected_return&&std::memcmp(&native_session,&expected_session,sizeof(native_session))==0);check("Option_update_four_ANM_slots",std::memcmp(sprites->pool,expected_sprites.slots,sizeof(expected_sprites.slots))==0);check("Option_update_ANM_lists_and_generation",std::memcmp(sprites->lists,expected_sprites.lists,sizeof(expected_sprites.lists))==0&&std::memcmp(&sprites->free_sentinel,&expected_sprites.free,sizeof(expected_sprites.free))==0&&sprites->field_7d40e88==expected_sprites.generation&&file.fields_5c[3]==expected_file);
    }
    #include "movement_cpu_cases.inc"
    put(reinterpret_cast<void*>(mapped_image_base),0x1c0028,sprite_bytes.data());VirtualFree(sprites,0,MEM_RELEASE);
}
