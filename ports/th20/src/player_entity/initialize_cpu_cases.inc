//Included only inside the isolated oracle's wmain. Production never links this.
{
    auto& cpu_session=*reinterpret_cast<th20::source::game_session::Session*>(mapped_image_base+0x1ba568);
    cpu_session=th20::source::game_session::Session{};
    cpu_session.contexts[0].objects_04[0]=cpu_session.contexts[1].objects_04[0]=&owner;
    for(unsigned n=0;n<500;++n){
        pl::construct_player_fields(owner);owner.shot_data=shot_data.data();const auto before=owner_storage;
        original<void>(0x504bd0,&owner.shots,int(n%2));const auto expected=owner_storage;owner_storage=before;
        pl::initialize_shots(owner.shots,n%2,cpu_session);
        check("ShotController_initialize_all84060bytes_256pool_free_links_contexts",owner_storage==expected);
        for(auto address:{0x4ffe40u,0x4ffe80u,0x4ffe10u,0x506d80u}){
            void* self=address==0x4ffe40?static_cast<void*>(&owner):address==0x4ffe80?static_cast<void*>(&owner.options[0]):address==0x4ffe10?static_cast<void*>(&owner.feedback):static_cast<void*>(&owner.shots.pool[0]);
            const auto select_before=owner_storage;original<void>(address,self,int(n%2));const auto select_expected=owner_storage;owner_storage=select_before;
            if(address==0x4ffe40)pl::select_context(owner,n%2,cpu_session);else if(address==0x4ffe80)pl::select_context(owner.options[0],n%2,cpu_session);else if(address==0x4ffe10)pl::select_context(owner.feedback,n%2,cpu_session);else pl::select_context(owner.shots.pool[0],n%2,cpu_session);
            check("Player_Option_Feedback_Shot_context_selection_all84060bytes",owner_storage==select_expected);
        }
    }
    th20::source::Archive source_archive(std::filesystem::path(argv[1]).parent_path()/L"th20.dat");
    for(auto name:{"pl00.sht","pl01.sht"}){
        auto data=source_archive.read(name);const auto before=data;resource_hashes[name]=sha256(before);
        std::uint16_t count;std::memcpy(&count,data.data()+2,2);pl::relocate_shot_data(data);
        check("source_SHT_header_preserved_actual_archive",std::memcmp(data.data(),before.data(),0x5d4)==0);
        for(unsigned i=0;i<count;++i){std::int32_t offset;std::uint32_t pointer;std::memcpy(&offset,before.data()+0x5d4+i*4,4);std::memcpy(&pointer,data.data()+0x5d4+i*4,4);
            const auto expected=offset<0?std::uint32_t(offset):reinterpret_cast<std::uint32_t>(data.data())+std::uint32_t(offset);
            check("source_SHT_actual_archive_signed_relocation_entries",pointer==expected);
        }
        check("source_SHT_payload_preserved_actual_archive",std::memcmp(data.data()+0x5d4+count*4,before.data()+0x5d4+count*4,data.size()-0x5d4-count*4)==0);
    }
    struct Initialization final:pl::InitializationServices {
        th20::source::game_session::Session session_data;th20::source::Archive& archive;std::vector<std::string> requests;
        std::vector<int> calls;th20::source::sprite::AnimationFile animation;
        int fail=0,bound=0,created=0;unsigned selected=0;void* loaded=nullptr;
        explicit Initialization(th20::source::Archive& source):archive(source){}
        th20::source::game_session::Session& session() override{return session_data;}
        void select_view(int index) override{selected=index;calls.push_back(1);}
        th20::source::sprite::AnimationFile* load_animation(int slot,const char* name) override{requests.emplace_back(name);calls.push_back(2);if(slot!=int(selected)+9)throw std::logic_error("wrong player ANM slot");if(fail==1)return nullptr;auto bytes=archive.read(name);if(bytes.size()<64)throw std::logic_error("empty ANM fixture");return &animation;}
        void* load_shots(const char* name) override{requests.emplace_back(name);calls.push_back(3);if(fail==2)return nullptr;auto bytes=archive.read(name);loaded=th20::source::runtime::allocate_bytes(bytes.size());if(!loaded)throw std::bad_alloc();std::memcpy(loaded,bytes.data(),bytes.size());pl::relocate_shot_data({static_cast<std::uint8_t*>(loaded),bytes.size()});return loaded;}
        void resource_error() override{calls.push_back(9);}
        void bind_animation(th20::source::sprite::AnimationFile& file,th20::source::sprite::Animation&,int script) override{if(&file!=&animation||script)throw std::logic_error("wrong initial player script");calls.push_back(4);++bound;}
        std::uint32_t create_feedback_animation() override{calls.push_back(5);return 11;}
        void hide_feedback_animation(std::uint32_t handle) override{if(handle!=11)throw std::logic_error("wrong feedback handle");calls.push_back(6);}
        void create_damage(int index) override{if(index!=int(selected))throw std::logic_error("wrong damage context");calls.push_back(7);++created;}
    };
    std::uint32_t original_registration_flags[2]{};
    {
        // Run the actual412310/4123b0 wrappers used by4f9520. Only allocation
        // and intrusive insertion are bounded endpoints; original userdata and
        // disable4127f0 operations execute. This catches an enabled=true typo.
        namespace sc=th20::source::scheduler;
        struct Registration {
            sc::Node node{};int priority=0;
            static Registration*& current(){static Registration* p=nullptr;return p;}
            static sc::Node* __cdecl create(sc::Callback callback){auto& n=current()->node;sc::initialize_node(n);n.flags=1;n.callback=callback;return &n;}
            static int __fastcall insert(void*,void*,sc::Node* node,int priority){current()->priority=priority;return 0;}
        } data;Registration::current()=&data;
        struct Patch {std::uint8_t* address;std::array<std::uint8_t,5> old;Patch(unsigned va,void* target):address(reinterpret_cast<std::uint8_t*>(mapped_image_base+va-0x400000)){std::memcpy(old.data(),address,5);address[0]=0xe9;*reinterpret_cast<unsigned*>(address+1)=reinterpret_cast<unsigned>(target)-reinterpret_cast<unsigned>(address+5);}~Patch(){std::memcpy(address,old.data(),5);}} create_patch(0x4127b0,&Registration::create),update_patch(0x411f80,&Registration::insert),draw_patch(0x412100,&Registration::insert);
        using Register=sc::Node*(__cdecl*)(int,sc::Callback,void*);
        for(unsigned i=0;i<2;++i){const auto callback=reinterpret_cast<sc::Callback>(mapped_image_base+(i?0xfefb0:0xfeda0));auto* node=reinterpret_cast<Register>(mapped_image_base+(i?0x123b0:0x12310))(29+int(i),callback,&owner);original_registration_flags[i]=node->flags;check("Player_initialize_original_registration_disabled",!(node->flags&2u)&&data.priority==29+int(i)&&node->userdata==&owner&&node->callback==callback);}
    }
    for(unsigned pattern=0;pattern<12;++pattern){
        OwnerHost lifetime;Initialization init(source_archive);init.fail=pattern%3;const int index=pattern%2,character=(pattern/2)%2;
        init.session_data.contexts[index].current_player=&init.session_data.player_table.players[index];init.session_data.contexts[index].current_player->fields_00[2]=character;
        auto* result=pl::create_player(index,lifetime,init);
        if(init.fail){check("source_Player_factory_failure_original_resource_order",!result&&init.calls==((init.fail==1)?std::vector<int>{1,2,9}:std::vector<int>{1,2,3,9}));check("source_Player_factory_failure_published_context_retained",init.session_data.contexts[index].objects_04[0]!=nullptr);}
        else{
            check("source_Player_factory_resource_order_callback_damage_sequence",result&&init.calls==std::vector<int>({1,2,3,4,5,6,7}));
            check("source_Player_factory_actual_character_ANM_SHT_names",init.requests==std::vector<std::string>({character?"pl01.anm":"pl00.anm",character?"pl01.sht":"pl00.sht"}));
            check("source_Player_factory_shared_published_context",result->context==&init.session_data.contexts[index]&&result->context->objects_04[0]==result&&result->view_index==index);
            check("Player_factory_registration_matches_original_disabled_wrappers",result->update_node&&result->draw_node&&result->update_node->priority==29&&result->draw_node->priority==30&&result->update_node->userdata==result&&result->draw_node->userdata==result&&result->update_node->flags==original_registration_flags[0]&&result->draw_node->flags==original_registration_flags[1]);
            check("source_Player_factory_start_position_and_collision_expansion",result->position_614.x==0&&result->position_614.y==400&&result->fixed_position.x==0&&result->fixed_position.y==51200&&result->collision_expansion==1);
            check("source_Player_factory_actual_shot_header_and_feedback",result->context->current_player->fields_30[1]==400&&result->context->current_player->fields_30[2]==100&&result->feedback.handle_4c==11&&result->feedback.enabled==0);
            bool option_contexts=true;for(auto& option:result->options)option_contexts&=option.fields_f4[12]==unsigned(index)&&option.fields_f4[13]==reinterpret_cast<std::uint32_t>(result->context)&&option.vector_78.y==-51200;
            check("source_Player_factory_option_contexts_and_initial_fixed_position",option_contexts);
            th20::source::runtime::retire_callback_owner(result);check("source_Player_factory_teardown_removes_actual_callbacks",!lifetime.state.update.sentinel.next&&!lifetime.state.draw.sentinel.next);
        }
    }
}
