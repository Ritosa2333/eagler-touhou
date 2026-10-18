// Included inside pool_cpu_compare's wmain. The enclosing harness provides
// its hash-verified CPU image, real source pool, source Iterator and check().
{
    struct InterruptFixture final:s::AnimationCallback {
        s::Animation* animation=nullptr;
        s::AnimationLink* unlink_target=nullptr;
        std::vector<std::array<std::uint32_t,3>>* events=nullptr;
        std::uint32_t id=0;
        std::int32_t update() override {throw std::logic_error("Unexpected interrupt-fixture update");}
        void draw() override {throw std::logic_error("Unexpected interrupt-fixture draw");}
        void retire() override {throw std::logic_error("Unexpected interrupt-fixture retire");}
        void interrupt(std::int32_t event) override {
            events->push_back({id,static_cast<std::uint32_t>(event),animation->base.field_438});
            animation->base.field_438=0xdeadbeefu; // original helper must overwrite AFTER callback
            if(unlink_target)q::unlink(reinterpret_cast<q::Link&>(*unlink_target));
        }
    };
    for(unsigned test=0;test<800;++test) {
        s::Animation animation{};s::construct_animation(animation);
        animation.base.field_438=rng();const auto event=static_cast<std::int32_t>(rng());
        std::vector<std::array<std::uint32_t,3>> log;
        InterruptFixture callback;callback.animation=&animation;callback.events=&log;
        if(test&1)animation.fields_550[6]=reinterpret_cast<std::uint32_t>(&callback);
        const auto before=animation;
        cpu<void>(0x4388f0,&animation,event);const auto expected=animation;const auto expected_log=log;
        animation=before;log.clear();s::set_animation_interrupt(animation,event);
        check("interrupt_helper_all_bytes",&expected,&animation,sizeof(animation));
        const auto first_size=expected_log.size(),second_size=log.size();check("interrupt_callback_count",&first_size,&second_size,sizeof(first_size));
        if(first_size==second_size&&first_size)check("interrupt_callback_before_store",expected_log.data(),log.data(),first_size*sizeof(log[0]));
    }
    for(unsigned test=0;test<400;++test) {
        init();auto& parent=c.pool[0].animation;auto& child1=c.pool[1].animation;
        auto& child2=c.pool[2].animation;auto& grandchild=c.pool[3].animation;
        c.pool[0].active=1;parent.handle=0x12340000u;
        std::vector<std::array<std::uint32_t,3>> log;
        InterruptFixture callbacks[4];
        for(unsigned i=0;i<4;++i) {
            auto& animation=c.pool[i].animation;animation.base.field_438=rng();
            callbacks[i].animation=&animation;callbacks[i].events=&log;callbacks[i].id=i;
            animation.fields_550[6]=reinterpret_cast<std::uint32_t>(&callbacks[i]);
        }
        q::insert_after(reinterpret_cast<q::Link&>(parent.links[3]),reinterpret_cast<q::Link&>(child1.links[2]));
        q::insert_after(reinterpret_cast<q::Link&>(child1.links[2]),reinterpret_cast<q::Link&>(child2.links[2]));
        q::insert_after(reinterpret_cast<q::Link&>(child1.links[3]),reinterpret_cast<q::Link&>(grandchild.links[2]));
        if(test%3==1)callbacks[0].unlink_target=&child1.links[2]; // callback removes Iterator.next
        if(test%3==2)callbacks[1].unlink_target=&child1.links[2]; // callback removes Iterator.current
        std::array<s::PooledAnimation,4> before;std::memcpy(before.data(),c.pool,sizeof(before));
        const auto event=static_cast<std::int32_t>(rng());const auto handle=test%7?parent.handle:0u;
        cpu<void>(0x44ee90,&c,handle,event);
        std::array<s::PooledAnimation,4> expected;std::memcpy(expected.data(),c.pool,sizeof(expected));const auto expected_log=log;
        std::memcpy(c.pool,before.data(),sizeof(before));log.clear();s::interrupt_animation_children(c,handle,event);
        check("interrupt_root_child_chain_all_bytes",expected.data(),c.pool,sizeof(expected));
        const auto first_size=expected_log.size(),second_size=log.size();check("interrupt_child_chain_call_count",&first_size,&second_size,sizeof(first_size));
        if(first_size==second_size&&first_size)check("interrupt_child_chain_calls",expected_log.data(),log.data(),first_size*sizeof(log[0]));
        const auto grandchild_before=before[3].animation.base.field_438;
        check("interrupt_does_not_visit_grandchild",&grandchild_before,&grandchild.base.field_438,4);
    }
}
