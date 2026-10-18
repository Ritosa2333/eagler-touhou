    for(unsigned test=0;test<128;++test){
        std::memset(fixture,0,sizeof(*fixture));::new(static_cast<void*>(fixture))d::HitCtrlInf;const int index=test%2;
        std::array<std::uint8_t,sizeof(*fixture)> before,expected,expected_shutdown;std::memcpy(before.data(),fixture,before.size());
        const auto expected_result=cpu<int>(0x4c0b30,fixture,index);std::memcpy(expected.data(),fixture,expected.size());const auto expected_node=*fixture->update_node;
        cpu<void>(0x4c0120,fixture);std::memcpy(expected_shutdown.data(),fixture,expected_shutdown.size());
        std::memcpy(fixture,before.data(),before.size());const auto actual_result=fixture->initialize(index);
        auto* initial=reinterpret_cast<d::HitCtrlInf*>(expected.data());initial->context=fixture->context;initial->update_node=fixture->update_node;auto node=expected_node;node.callback=fixture->update_node->callback;node.link.value=fixture->update_node;
        check("initialize_scheduler_result",test,&expected_result,&actual_result,4);check("initialize_scheduler_node",test,&node,fixture->update_node,sizeof(node));check("initialize_full_controller_pool",test,expected.data(),fixture,sizeof(*fixture));
        std::array<std::uint8_t,sizeof(*fixture)> frame_before,frame_expected;std::memcpy(frame_before.data(),fixture,frame_before.size());using Update=int(__cdecl*)(void*);const int old_result=reinterpret_cast<Update>(mapped_image_base+0xc0cb0)(fixture);std::memcpy(frame_expected.data(),fixture,frame_expected.size());std::memcpy(fixture,frame_before.data(),frame_before.size());const int new_result=fixture->update_node->callback(fixture);check("registered_update_return",test,&old_result,&new_result,4);check("registered_update_full_controller",test,frame_expected.data(),fixture,sizeof(*fixture));std::memcpy(fixture,frame_before.data(),frame_before.size());
        auto* shutdown=reinterpret_cast<d::HitCtrlInf*>(expected_shutdown.data());shutdown->context=fixture->context;shutdown->update_node=fixture->update_node;fixture->~HitCtrlInf();std::memcpy(expected_shutdown.data(),fixture,4);check("shutdown_unregister_full_controller",test,expected_shutdown.data(),fixture,sizeof(*fixture));
        const bool empty=scheduler.update.sentinel.next==nullptr&&scheduler.update.tail==&scheduler.update.sentinel;const bool yes=true;check("shutdown_scheduler_empty",test,&yes,&empty,1);
    }
    ::new(static_cast<void*>(fixture))d::HitCtrlInf;
