// Isolated original-image oracle and explicitly named source integration cases.
{
    namespace gs=th20::source::game_session;namespace sp=th20::source::sprite;namespace dm=th20::source::damage;
    struct Firing final:pl::FiringServices {
        gs::Session& state;std::vector<std::unique_ptr<pl::Shot>> heap;
        sp::Animation visual{};dm::Region region{};std::vector<int> calls;
        unsigned random_calls=0,spawns=0,damages=0;bool reject=false;float rate=1.0f;
        static int __fastcall callback(pl::Shot* shot,void*,int frame){shot->fields_b8[0]=std::uint32_t(frame);return frame==-77?-1:0;}
        explicit Firing(gs::Session& s):state(s){}
        gs::Session& session() override{return state;}
        float clock_rate() override{return rate;}
        float signed_random() override{calls.push_back(1);return (++random_calls&1)?0.25f:-0.5f;}
        pl::Shot* create_heap_shot() override{calls.push_back(2);auto p=std::make_unique<pl::Shot>();std::memset(p.get(),0,sizeof(pl::Shot));pl::construct_shot(*p);auto* result=p.get();heap.push_back(std::move(p));return result;}
        pl::ShotCallbacks callbacks(const pl::ShotRecord&) override{calls.push_back(3);return {reinterpret_cast<std::uintptr_t>(&callback),0,0,0};}
        std::uint32_t spawn_animation(sp::AnimationFile&,int) override{calls.push_back(4);return ++spawns;}
        sp::Animation& animation(std::uint32_t&) override{return visual;}
        std::uint32_t create_damage(gs::Context&,const pl::Shot& shot) override{calls.push_back(5);region.motion.position=shot.motion.position;region.size=shot.vector_b0;region.damage=std::bit_cast<int>(shot.fields_98[5]);return ++damages;}
        dm::Region* damage(std::uint32_t&) override{return &region;}
        void retire(pl::Shot& shot) override{calls.push_back(6);shot.fields_98[1]=0;th20::source::scheduler::unlink(shot.link);pl::recycle_shot(*shot.owner,shot);shot.flags=0;}
        void sound_at(int,float) override{calls.push_back(7);}
    };
    auto& native_session=*reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568);native_session=gs::Session{};
    native_session.contexts[0].objects_04[0]=native_session.contexts[1].objects_04[0]=&owner;
    native_session.contexts[0].current_player=&native_session.player_table.players[0];native_session.contexts[1].current_player=&native_session.player_table.players[1];
    std::array<std::uint8_t,0x5d4+32*4> table{};std::array<pl::ShotRecord,33> rows{};owner.shot_data=table.data();
    for(unsigned i=0;i<32;++i)put(table.data(),0x5d4+i*4,rows.data());
    Firing host(native_session);
    for(unsigned n=0;n<3000;++n){
        pl::construct_player_fields(owner);owner.shot_data=table.data();pl::initialize_shots(owner.shots,n%2,native_session);
        auto& selected=owner.shots.pool[n%256];owner.shots.field_12460=rng();owner.shots.view_index=std::bit_cast<int>(rng());
        const auto before=owner_storage;original<void>(0x505fc0,&owner.shots);const auto expected=owner_storage;owner_storage=before;pl::advance_shot_handle(owner.shots);
        check("Shot_next_handle_full_object_wrap_and_view_bits",owner_storage==expected);
        for(int index:{0,1,9,100,101,111}){
            auto* result=original<pl::Option*>(0x5060a0,&owner,index);check("Shot_option_primary_secondary_actual_record_pointer",result==&pl::shot_option(owner,index));
            auto* position=original<pl::Fixed2*>(0x5062f0,&owner,index);check("Shot_option_fixed_position_pointer",position==&pl::shot_option_position(owner,index));
            const float value=finite();pl::shot_option(owner,index).field_d4=std::bit_cast<std::uint32_t>(value);const auto angle=original<float>(0x506310,&owner,index);
            check("Shot_option_angle_original_field_bits",std::bit_cast<std::uint32_t>(angle)==std::bit_cast<std::uint32_t>(pl::shot_option_angle(owner,index)));
        }
        const auto packed=((n%32)<<8)|(n%33);check("Shot_SHT_record_relocated_table_and_stride",original<const pl::ShotRecord*>(0x506480,&owner,packed)==&pl::shot_record(owner,packed));
        owner.shots.view_index=n%2;
        const auto allocation_before=owner_storage;auto* cpu=original<pl::Shot*>(0x506210,&owner.shots);const auto allocation_expected=owner_storage;owner_storage=allocation_before;auto* source=pl::allocate_shot(owner.shots,host);
        check("Shot_pool_allocate_return",cpu==source);check("Shot_pool_allocate_all84060bytes_handle_and_links",owner_storage==allocation_expected);
        (void)selected;
    }
    for(unsigned n=0;n<1600;++n){
        pl::construct_player_fields(owner);owner.shot_data=table.data();pl::initialize_shots(owner.shots,n%2,native_session);
        const int frame=int(rng()%1024)-512,secondary=int(rng()%2048)-1024,pattern=int(n%32);const unsigned length=n%30+1;
        for(unsigned i=0;i<length;++i){rows[i]={};rows[i].period=std::int8_t(rng()%128);rows[i].phase=std::int8_t(rng());rows[i].secondary_period=n%2?std::int8_t(rng()%127+1):0;
            rows[i].secondary_phase=std::int8_t(rng());rows[i].source=std::int8_t(rng()%6);rows[i].type=2;rows[i].group=i;
            owner.shots.counters_12468[i]=rng()|1u;owner.shots.counters_124e0[i]=reinterpret_cast<std::uint32_t>(&owner.shots.pool[i]);owner.shots.pool[i].fields_b8[8]=rng();}
        rows[length]={};rows[length].period=-1;
        const auto before=owner_storage;const int cpu=original<int>(0x504d40,&owner.shots,frame,secondary,pattern);const auto expected=owner_storage;owner_storage=before;
        const int source=pl::fire_shots(owner.shots,frame,secondary,pattern,host);
        check("Shot_fire_signed_period_phase_and_type2_reuse_return",cpu==source);check("Shot_fire_all84060bytes_multiple_patterns_original_nested505d90",owner_storage==expected);
    }
    for(unsigned pattern=0;pattern<224;++pattern){
        pl::construct_player_fields(owner);owner.shot_data=table.data();owner.animation_file=&file;owner.field_24=reinterpret_cast<std::uint32_t>(&file);owner.position_614={10,20,30};
        pl::initialize_shots(owner.shots,pattern%2,native_session);const unsigned source=pattern%7,type=(pattern/7)%8;const int view=pattern%2;
        owner.focused_204c=(pattern&1)!=0;owner.shots.field_1255c=(pattern&2)?8:0;
        for(auto& option:owner.options){option.vector_78={128*12,128*-36};option.field_d4=std::bit_cast<std::uint32_t>(-0.25f);}
        auto& player_record=*native_session.contexts[0].current_player;player_record.fields_30[(0x9c-0x30)/4]=200;player_record.fields_30[(0xa0-0x30)/4]=30;
        auto& row=rows[0];row={};row.period=0;row.type=std::int8_t(type);row.source=std::int8_t(source);row.angle=(pattern&4)?1001.0f:-0.5f;row.speed=4;row.offset={1,2};row.size={12,14};row.damage=10;row.group=4;row.sound=3;rows[1].period=-1;
        Firing fixture(native_session);fixture.visual.base.flags[2]=((pattern/28)%8)<<21;fixture.visual.base.vector_50.x=-2;
        const int frame=(pattern&8)?-77:19;auto* shot=pl::allocate_shot(owner.shots,fixture);auto* attachment=source?&owner.options[source-1]:nullptr;
        const int result=pl::initialize_shot(*shot,0,frame,owner.position_614,attachment,view,fixture);
        check("source_Shot_init_callback_abort_and_pool_recycle",result==(frame==-77?-1:0)&&shot->fields_98[1]==(frame==-77?0u:1u));
        check("source_Shot_init_context_attachment_and_callback_frame",shot->context==&native_session.contexts[view]&&shot->owner==&owner.shots&&shot->field_110==reinterpret_cast<std::uint32_t>(attachment)&&shot->fields_b8[0]==std::uint32_t(frame));
        check("source_Shot_init_bonus_uses_global_player0_and_clamps",shot->fields_98[5]==(!source?10u:owner.focused_204c?20u:13u));
        check("source_Shot_init_damage_callback_flags_and_target",fixture.region.hit_callback==1&&(fixture.region.flags&0x40u)&&fixture.region.field_90==1);
        check("source_Shot_init_random_two_draws_for_wide_option_angle",fixture.random_calls==((pattern&4)&&source?2u:0u));
        check("source_Shot_init_success_positions_or_abort_before_sound",frame==-77?fixture.calls.back()==6:fixture.calls.back()==7&&std::memcmp(&fixture.visual.vector_5bc,&shot->motion.position,12)==0);
        check("source_Shot_init_type2_controller_registry",type!=2||(owner.shots.counters_12468[4]==1&&owner.shots.counters_124e0[4]==reinterpret_cast<std::uint32_t>(shot)));
    }
    // Exhaustion really allocates a Shot and initially selects context0, even
    // when the requesting controller belongs to context1 (506210).
    pl::construct_player_fields(owner);owner.shot_data=table.data();pl::initialize_shots(owner.shots,1,native_session);Firing exhaustion(native_session);
    for(unsigned i=0;i<256;++i)pl::allocate_shot(owner.shots,exhaustion);
    auto* overflow=pl::allocate_shot(owner.shots,exhaustion);
    check("source_Shot_pool_exhaustion_real_heap_record",exhaustion.heap.size()==1&&overflow==exhaustion.heap[0].get()&&(overflow->flags&0x1000000u));
    check("source_Shot_pool_exhaustion_original_context0_quirk",overflow->view_index==0&&overflow->context==&native_session.contexts[0]&&overflow->link.owner==&owner.shots.active);
}
