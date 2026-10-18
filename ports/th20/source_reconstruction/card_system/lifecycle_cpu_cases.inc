{
    auto* chain_lock=reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c0240);std::array<std::uint8_t,0x30> saved_chain_lock;std::memcpy(saved_chain_lock.data(),chain_lock,0x30);std::memset(chain_lock,0,0x30);chain_lock[0]=0x101;chain_lock[10]=GetCurrentThreadId();chain_lock[11]=1;
    for(unsigned test=0;test<64;++test){
        diagnostic_case=test;diagnostic_phase="Card actual scheduler registration and owned node destruction";
        init();std::memset(storage,0,sizeof(storage));auto* value=new(storage)ca::CardInf;const auto initial=bytes_of(card);q::initialize_state(callbacks);const auto initial_state=callbacks;
        card_checkpoint<<"initialize original "<<test<<std::endl;const auto expected_return=cpu<int>(0x487b10,value,test%2);auto expected_object=bytes_of(card);const auto expected_state=callbacks;const auto expected_update=*value->update_node,expected_draw=*value->draw_node;auto* old_update=value->update_node;auto* old_draw=value->draw_node;
        card_checkpoint<<"dtor original "<<test<<std::endl;cpu<void>(0x486d20,value);const auto expected_destroyed=bytes_of(card);const auto expected_empty=callbacks;
        std::memcpy(storage,initial.data(),sizeof(storage));callbacks=initial_state;card_checkpoint<<"initialize source "<<test<<std::endl;const auto actual_return=ca::initialize(*value,test%2);auto* update=value->update_node;auto* draw=value->draw_node;
        const std::pair<std::uint32_t,std::uint32_t> addresses[]{
            {reinterpret_cast<std::uint32_t>(old_update),reinterpret_cast<std::uint32_t>(update)},
            {reinterpret_cast<std::uint32_t>(old_draw),reinterpret_cast<std::uint32_t>(draw)},
            {reinterpret_cast<std::uint32_t>(&old_update->link),reinterpret_cast<std::uint32_t>(&update->link)},
            {reinterpret_cast<std::uint32_t>(&old_draw->link),reinterpret_cast<std::uint32_t>(&draw->link)},
            {mapped_image_base+0x885d0,reinterpret_cast<std::uint32_t>(update->callback)},
            {mapped_image_base+0x88680,reinterpret_cast<std::uint32_t>(draw->callback)}};
        auto normalized=[&](const void* input,std::size_t length){std::vector<std::uint8_t> out(static_cast<const std::uint8_t*>(input),static_cast<const std::uint8_t*>(input)+length);for(std::size_t offset=0;offset+4<=length;offset+=4){std::uint32_t value;std::memcpy(&value,out.data()+offset,4);for(const auto& pair:addresses)if(value==pair.first){std::memcpy(out.data()+offset,&pair.second,4);break;}}return out;};
        auto object=normalized(expected_object.data(),expected_object.size());check("card_initialize_object",object.data(),storage,sizeof(storage));auto sched=normalized(&expected_state,sizeof(expected_state));check("card_initialize_scheduler",sched.data(),&callbacks,sizeof(callbacks));auto unode=normalized(&expected_update,sizeof(expected_update)),dnode=normalized(&expected_draw,sizeof(expected_draw));check("card_initialize_update_node",unode.data(),update,sizeof(*update));check("card_initialize_draw_node",dnode.data(),draw,sizeof(*draw));check("card_initialize_return",&expected_return,&actual_return,4);
        card_checkpoint<<"dtor source "<<test<<std::endl;value->~CardInf();auto destroyed=normalized(expected_destroyed.data(),expected_destroyed.size());std::memcpy(destroyed.data(),storage,4);check("card_owned_destructor_object",destroyed.data(),storage,sizeof(storage));check("card_owned_destructor_scheduler",&expected_empty,&callbacks,sizeof(callbacks));
    }
    std::memcpy(chain_lock,saved_chain_lock.data(),0x30);
}

