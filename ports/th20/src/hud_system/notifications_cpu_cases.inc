{
    namespace hd=th20::source::hud;namespace tx=th20::source::text;namespace pe=th20::source::program_entry;namespace gs=th20::source::game_session;
    const std::uint32_t program[]{0x000c0005,0,2,0x00080002,0,0x0008ffff,0};auto* instruction=reinterpret_cast<s::AnmInstruction*>(const_cast<std::uint32_t*>(program));
    std::vector<s::AnmInstruction*> scripts(384,instruction);std::vector<s::Animation> templates[3];s::AnimationFile files[3]{};s::AnimationFile* old_files[3]{};std::uint8_t file_bytes[3]{};std::array<s::SpriteData,256> sprites{};
    for(unsigned i=0;i<sprites.size();++i){auto& a=sprites[i];a.left=float(i%17);a.top=float(i%11);a.right=a.left+13;a.bottom=a.top+27;a.texture_extent_1c=128;a.texture_extent_20=256;a.u0=float(i%13)/16;a.v0=float(i%7)/8;a.u1=a.u0+.03125f;a.v1=a.v0+.0625f;a.extent_48=8+float(i%5);a.extent_4c=16+float(i%7);a.scale_50=.75f;a.scale_54=1.25f;}
    for(unsigned f=0;f<3;++f){templates[f].resize(384);files[f].id=10+f;files[f].stem=f==0?"front":f==1?"notice":"stage";files[f].bytes=&file_bytes[f];files[f].sprites=sprites.data();files[f].templates=templates[f].data();files[f].scripts=scripts.data();files[f].script_count=384;files[f].sprite_count=256;for(unsigned i=0;i<templates[f].size();++i){auto& a=templates[f][i];s::construct_animation(a);s::reset_animation_state(a);a.base.fields_10_28[2]=10+f;a.base.fields_10_28[3]=10+f;a.base.fields_10_28[5]=i;a.base.fields_10_28[6]=0;a.base.flags[2]=1;a.base.field_490=0xffaabbcc;a.vector_5bc={1,2,3};}old_files[f]=c.files[10+f];c.files[10+f]=&files[f];}
    auto front_storage=std::make_unique<std::uint8_t[]>(sizeof(hd::FrontInf));auto& front=*reinterpret_cast<hd::FrontInf*>(front_storage.get());auto text_storage=std::make_unique<std::uint8_t[]>(sizeof(tx::Renderer));auto& renderer=*reinterpret_cast<tx::Renderer*>(text_storage.get());std::memset(&renderer,0,sizeof(renderer));renderer.animation_file=&files[1];auto* old_renderer=tx::renderer;tx::renderer=&renderer;auto* old_native_renderer=*reinterpret_cast<void**>(mapped_image_base+0x1c0698);*reinterpret_cast<void**>(mapped_image_base+0x1c0698)=&renderer;const auto old_group=c.field_6c4;c.field_6c4=0;
    constexpr int values[]{0,1,9,10,99,100,999,1000,999999,1000000,9999999,10000000,99999999};
    for(unsigned test=0;test<3072;++test){diagnostic_case=test;diagnostic_phase="HUD notification full named spawn sprite digits and deletion";init();std::memset(&front,0,sizeof(front));front.front_file=&files[0];front.stage_file=&files[2];front.fields_11c[7]=rng();front.handles_f8[7]=rng();
        for(unsigned i=0;i<16;++i){auto& p=c.pool[i];p.active=1;s::register_animation(c,c.lists,p.animation,false,false);p.animation.base.flags[0]=rng();p.animation.base.flags[1]=rng();p.animation.base.flags[2]=rng();if(i<10)front.score_handles[i]=test%5?p.animation.handle:0;else if(i<13)front.notice_handles[i-10]=test%7?p.animation.handle:0;}
        for(unsigned i=24;i<56;++i)q::insert_after(reinterpret_cast<q::Link&>(c.free_sentinel),reinterpret_cast<q::Link&>(c.pool[i].free_link));
        const int type=int(test%10)-1,value=test%2?values[(test/10)%std::size(values)]:int(rng()%100000000);const Snapshot initial_pool(c);const auto initial_front=std::vector<std::uint8_t>(front_storage.get(),front_storage.get()+sizeof(front));std::uint32_t counters[3];for(unsigned i=0;i<3;++i)counters[i]=files[i].fields_5c[3];
        FloatingEnvironment::prepare();cpu<void>(0x4b90e0,&front,type,value);const Snapshot expected_pool(c);const auto expected_front=std::vector<std::uint8_t>(front_storage.get(),front_storage.get()+sizeof(front));std::uint32_t expected_counters[3];for(unsigned i=0;i<3;++i)expected_counters[i]=files[i].fields_5c[3];initial_pool.restore(c);std::memcpy(&front,initial_front.data(),sizeof(front));for(unsigned i=0;i<3;++i)files[i].fields_5c[3]=counters[i];
        FloatingEnvironment::prepare();hd::notify(front,type,value);check("hud_notifications_front",expected_front.data(),&front,sizeof(front));compare("hud_notifications",expected_pool,Snapshot(c));std::uint32_t actual_counters[3];for(unsigned i=0;i<3;++i)actual_counters[i]=files[i].fields_5c[3];check("hud_notifications_file_counters",expected_counters,actual_counters,sizeof(actual_counters));
    }
    #include "enable_cpu_cases.inc"
    #include "update_cpu_cases.inc"
    #include "../stage_clear/pool_cpu_cases.inc"
    #include "../card_system/pool_cpu_cases.inc"
    #include "../pause_system/draw_cpu_cases.inc"
    #include "../gameplay/frame_cpu_cases.inc"
    #include "../trophy_system/pool_cpu_cases.inc"
    #include "../ending_scene/pool_cpu_cases.inc"
    #include "../notice_system/pool_cpu_cases.inc"
    #include "../title_system/music_draw_cpu_cases.inc"
    #include "../title_system/stage_draw_cpu_cases.inc"
    #include "../title_system/practice_pool_cpu_cases.inc"
    #include "../title_system/replay_draw_cpu_cases.inc"
    #include "../title_system/stones_draw_cpu_cases.inc"
    for(unsigned i=0;i<3;++i)c.files[10+i]=old_files[i];c.field_6c4=old_group;tx::renderer=old_renderer;*reinterpret_cast<void**>(mapped_image_base+0x1c0698)=old_native_renderer;
}
