{
    // Native51e1c0 changes the complete progress object and consumes exactly
    // the checksum RNG calls; source compares both, including untouched bytes.
    std::vector<unsigned char> before(sizeof(p::Snapshot)),expected(sizeof(p::Snapshot));
    for(unsigned test=0;test<512;++test){
        fill(raw_manager->current.profiles,sizeof(raw_manager->current.profiles),test);fill(&raw_manager->current.metadata,sizeof(p::Metadata),test);
        const int edges[]{0,1,99998,99999,100000,-1,(-2147483647-1),2147483647};
        for(int card=0;card<113;++card)p::write(raw_manager->current.profiles[18].bytes,0xb08+card*0xe0+0xc8,edges[(test+card)%8]);
        std::memcpy(before.data(),&raw_manager->current,before.size());state::Random stream(1);state::seed(stream,random());auto* native_rng=reinterpret_cast<state::Random*>(mapped_image_base+0x1ba4c4);*native_rng=stream;
        cpu<void>(0x51e1c0,raw_manager);std::memcpy(expected.data(),&raw_manager->current,expected.size());const auto expected_rng=*native_rng;
        std::memcpy(&raw_manager->current,before.data(),before.size());th20::source::title::unlock_all_data(*raw_manager,stream);
        check("title_unlock_all_profiles_metadata",test,expected.data(),&raw_manager->current,expected.size());check("title_unlock_checksum_rng",test,&expected_rng,&stream,sizeof(stream));
    }
}
