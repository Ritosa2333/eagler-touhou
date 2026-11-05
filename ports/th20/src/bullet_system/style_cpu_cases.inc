// All original instructions execute unmodified. The finite synthetic ANM
// programs exercise VM restart and pool lifecycle, not the complete bullet.anm.
{
    const auto original_remap=mapped_image_base+0x7d580;
    const auto source_remap=reinterpret_cast<std::uint32_t>(&bu::remap_sprite);
    for(unsigned type=0;type<50;++type)for(unsigned color=0;color<16;++color)for(unsigned id=0;id<5;++id){
        diagnostic_case=type*80+color*5+id;diagnostic_phase="Bullet style full remap table";
        bu::Bullet bullet;bullet.field_4c=static_cast<short>(type);bullet.field_4e=static_cast<short>(color);s::Animation animation{};animation.field_5c8=reinterpret_cast<std::uint32_t>(&bullet);
        using Remap=int(__cdecl*)(s::Animation*,int);const auto expected=reinterpret_cast<Remap>(original_remap)(&animation,id);const auto actual=bu::remap_sprite(&animation,id);check("bullet_style_remap",&expected,&actual,4);
    }
    auto* stop=script_pointer; // Bounded label-1/stop/end permits interrupt scans.
    std::vector<s::AnmInstruction*> scripts(512,stop);
    std::vector<s::Animation> templates(512);
    for(unsigned i=0;i<templates.size();++i){auto& value=templates[i];s::construct_animation(value);s::reset_animation_state(value);value.base.fields_10_28[2]=7;value.base.fields_10_28[3]=7;value.base.fields_10_28[5]=i;value.base.fields_10_28[6]=0;value.base.flags[2]=1;value.base.flags[1]=0;}
    auto* old_scripts=file.scripts;auto* old_templates=file.templates;file.scripts=scripts.data();file.templates=templates.data();
    for(unsigned test=0;test<2048;++test){diagnostic_case=test;diagnostic_phase="Animation 4383a0 full restart and absent-resource state";reset(test,1);auto& a=c.pool[0].animation;
        // Valid stop programs do not inspect these payload fields, allowing a
        // broad storage pattern to check precisely which restart bytes survive.
        for(auto& word:a.base.fields_444)word=rng();a.base.flags[0]=rng();a.base.flags[1]=0;a.base.flags[2]=rng();a.base.flags[3]=rng();a.base.field_438=0;a.field_5dc=rng();
        a.timer_4c8={int(rng()),int(rng()),finite(),rng()};a.timer_4d8={int(rng()),int(rng()),finite(),rng()};a.field_5e0=rng();a.field_5c8=rng();a.field_5cc=0;
        const unsigned script=test%512;file.fields_5c[0]=test%4==0?rng()|1:0;scripts[script]=test%4==1?nullptr:stop;c.field_b8=rng();const auto initial_counter=c.field_b8;const auto initial=a;
        FloatingEnvironment::prepare();cpu<void>(0x4383a0,&file,&a,int(script));const auto expected=a;const auto expected_counter=c.field_b8;a=initial;c.field_b8=initial_counter;FloatingEnvironment::prepare();bu::restart_animation(file,a,script);
        check("bullet_restart_animation",&expected,&a,sizeof(a));check("bullet_restart_counter",&expected_counter,&c.field_b8,4);scripts[script]=stop;
    }
    file.fields_5c[0]=0;
    for(unsigned test=0;test<3200;++test){diagnostic_case=test;diagnostic_phase="ETEX style and real child ANM lifecycle";reset(0,1);auto& b=owner.pool[0];b.metadata=metadata;b.flags=rng()|0x200u;b.field_38=0;b.field_90=0;b.field_4e=test%16;b.cancel_script=int(rng());b.animation_handle=test%4?b.animation->handle:0;
        // View zero keeps this bounded fixture's free pool disjoint from the
        // original second group's unusual +0x48 overlap with the pool array.
        for(unsigned index=32;index<64;++index)q::insert_after(reinterpret_cast<q::Link&>(c.free_sentinel),reinterpret_cast<q::Link&>(c.pool[index].free_link));
        for(auto& cmd:metadata->commands)cmd={};auto& op=metadata->commands[0];op.words[8]=9;op.words[9]=1;op.words[4]=test%50;op.words[5]=(test/50)%16|(test&1?0x8000u:0);
        auto& a=*b.animation;a.base.flags[0]=rng();a.base.flags[1]=rng();a.base.flags[2]=rng();a.base.field_438=0;a.field_5e0=0;a.field_5cc=0;c.field_b8=rng();file.fields_5c[3]=rng();
        const auto initial=b;const Snapshot initial_pool(c);const auto initial_counter=c.field_b8,initial_file_counter=file.fields_5c[3];
        FloatingEnvironment::prepare();cpu<void>(0x47dcf0,&b);const auto expected=b;Snapshot expected_pool(c);const auto expected_counter=c.field_b8,expected_file_counter=file.fields_5c[3];
        // Function pointers refer to separately compiled code. Normalize exactly
        // the proven remap callback address and no other byte or pointer.
        auto& expected_animation=*reinterpret_cast<s::Animation*>(expected_pool.pool.data());if(expected_animation.field_5e0==original_remap)expected_animation.field_5e0=source_remap;
        b=initial;initial_pool.restore(c);c.field_b8=initial_counter;file.fields_5c[3]=initial_file_counter;FloatingEnvironment::prepare();bu::execute_extended_commands(b);
        check("bullet_etex_9",&expected,&b,sizeof(b));compare("bullet_etex_9_animation",expected_pool,Snapshot(c));check("bullet_etex_9_counter",&expected_counter,&c.field_b8,4);check("bullet_etex_9_file_counter",&expected_file_counter,&file.fields_5c[3],4);b.metadata.reset();
    }
    #include "shoot_cpu_cases.inc"
    #include "respawn_cpu_cases.inc"
    file.scripts=old_scripts;file.templates=old_templates;file.fields_5c[0]=0;
}
