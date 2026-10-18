{
    namespace fx=th20::source::effects;namespace state=th20::source::state;namespace gs=th20::source::game_session;
    using T=fx::ConvergingParticles;
    auto finite=[&]{return float(int(rng()%2001)-1000)/113.f;};
    auto bytes_of=[](const T& object){const auto* p=reinterpret_cast<const std::uint8_t*>(&object);return std::vector<std::uint8_t>(p,p+sizeof(object));};
    for(unsigned test=0;test<256;++test){diagnostic_case=test;diagnostic_phase="ConvergingParticles complete constructor";
        s::Animation a{};s::construct_animation(a);s::reset_animation_state(a);alignas(T) std::uint8_t storage[sizeof(T)];std::memset(storage,test,sizeof(storage));
        cpu<void>(0x45c1f0,storage,&a);auto expected=std::vector<std::uint8_t>(storage,storage+sizeof(storage));const auto expected_a=a;
        std::memset(storage,test,sizeof(storage));auto* value=new(storage)T(a);std::memcpy(expected.data(),storage,4);check("effect_converging_ctor",expected.data(),storage,sizeof(storage));check("effect_converging_attachment",&expected_a,&a,sizeof(a));value->~T();
    }
    const std::uint32_t program[]{0x000c0005,0,2,0x00080002,0,0x0008ffff,0};auto* instruction=reinterpret_cast<s::AnmInstruction*>(const_cast<std::uint32_t*>(program));
    std::vector<s::AnmInstruction*> scripts(151,instruction);std::vector<s::Animation> templates(151);
    for(unsigned i=0;i<templates.size();++i){auto& a=templates[i];s::construct_animation(a);s::reset_animation_state(a);a.base.fields_10_28[2]=8;a.base.fields_10_28[3]=8;a.base.fields_10_28[5]=i;a.base.fields_10_28[6]=0;a.base.flags[2]=1;}
    s::AnimationFile file{};file.id=8;file.stem="effect";file.scripts=scripts.data();file.templates=templates.data();auto* old_file=c.files[8];c.files[8]=&file;
    // This group reads only the real EffectInf file member, without invoking its IO/worker lifecycle.
    auto owner_storage=std::make_unique<std::uint8_t[]>(sizeof(fx::Controller));auto& owner=*reinterpret_cast<fx::Controller*>(owner_storage.get());owner.files[0]=&file;
    auto* old_owner=gs::context(0).objects_04[7];gs::context(0).objects_04[7]=&owner;const auto old_group=c.field_6c4;c.field_6c4=0;
    auto& source_rng=state::random_streams[1];auto& original_rng=*reinterpret_cast<state::Random*>(mapped_image_base+0x1ba4c4);
    for(unsigned test=0;test<4096;++test){diagnostic_case=test;diagnostic_phase="ConvergingParticles true child spawn and two Hermite phases";init();
        s::Animation a{};s::construct_animation(a);s::reset_animation_state(a);a.vector_5bc={finite()*16,finite()*16,finite()};a.base.vector_38.z=finite();a.base.fields_444[10]=float_bits(finite());a.base.fields_444[0]=test%31;a.base.field_490=rng();a.fields_550[4]=float_bits(float(test%7)*.25f);
        T value(a);fx::Parameters parameters{};for(auto& word:*reinterpret_cast<std::array<std::uint32_t,sizeof(parameters)/4>*>(&parameters))word=rng();
        value.age={int(rng()),int(rng()),finite(),rng()%4};const auto init_before=bytes_of(value);const auto init_a=a;
        const auto init_result=cpu<int>(0x45cfe0,&value,&parameters,test%2);const auto init_expected=bytes_of(value);const auto init_expected_a=a;
        std::memcpy(&value,init_before.data(),sizeof(value));a=init_a;const auto actual_init=value.initialize(parameters,test%2);check("effect_converging_initialize",init_expected.data(),&value,sizeof(value));check("effect_converging_initialize_vm",&init_expected_a,&a,sizeof(a));check("effect_converging_initialize_return",&init_result,&actual_init,4);
        const int age=int(test%63);value.age={test%3?age-1:age,age,float(age)+.25f,rng()%4};
        const unsigned active=test%17;
        for(unsigned i=0;i<active;++i){auto& p=c.pool[i];p.active=1;s::register_animation(c,c.lists,p.animation,false,false);value.handles[i*7]=p.animation.handle;value.stages[i*7]=int((test+i)%3);value.targets[i*7]={finite(),finite(),finite()};value.tangents[i*7]={finite(),finite(),finite()};p.animation.timer_4d8.current=int((test+i)%40);p.animation.base.flags[0]=rng();}
        // Four additional allocations execute actual named-spawn, template binding, ANM and pool registration.
        for(unsigned i=32;i<40;++i)q::insert_after(reinterpret_cast<q::Link&>(c.free_sentinel),reinterpret_cast<q::Link&>(c.pool[i].free_link));
        float rate=float(test%7)*.25f;const float* pointer=test%17?&rate:nullptr;state::timer_rate=pointer;anm_clock_pointer=pointer;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=pointer;
        state::Random seed(1);seed.state=rng();seed.last=rng();seed.minimum=rng();seed.modulus=test%5?0x7fffffff:0x00ffff00;source_rng=original_rng=seed;
        const auto before=bytes_of(value);const auto before_a=a;const Snapshot before_pool(c);const auto file_counter=file.fields_5c[3];
        FloatingEnvironment::prepare();const auto expected_return=cpu<int>(0x45c360,&value);const auto expected=bytes_of(value);const auto expected_a=a;const Snapshot expected_pool(c);const auto expected_rng=original_rng;const auto expected_counter=file.fields_5c[3];
        std::memcpy(&value,before.data(),sizeof(value));a=before_a;before_pool.restore(c);file.fields_5c[3]=file_counter;
        FloatingEnvironment::prepare();const auto actual_return=value.update();check("effect_converging_update",expected.data(),&value,sizeof(value));check("effect_converging_update_vm",&expected_a,&a,sizeof(a));compare("effect_converging_update",expected_pool,Snapshot(c));check("effect_converging_update_rng",&expected_rng,&source_rng,sizeof(source_rng));check("effect_converging_update_file_counter",&expected_counter,&file.fields_5c[3],4);check("effect_converging_update_return",&expected_return,&actual_return,4);
        const auto retire_before=bytes_of(value);const Snapshot retire_pool(c);cpu<void>(0x45ceb0,&value);const auto retired=bytes_of(value);const Snapshot retired_pool(c);std::memcpy(&value,retire_before.data(),sizeof(value));retire_pool.restore(c);value.retire();check("effect_converging_retire",retired.data(),&value,sizeof(value));compare("effect_converging_retire",retired_pool,Snapshot(c));
        const auto interrupt_before=bytes_of(value);const int event=int(test%5)-1;cpu<void>(0x45d060,&value,event);const auto interrupted=bytes_of(value);std::memcpy(&value,interrupt_before.data(),sizeof(value));value.interrupt(event);check("effect_converging_interrupt",interrupted.data(),&value,sizeof(value));
    }
    c.files[8]=old_file;c.field_6c4=old_group;gs::context(0).objects_04[7]=old_owner;clock_value=1;anm_clock_pointer=&clock_value;state::timer_rate=&clock_value;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=&clock_value;
}
