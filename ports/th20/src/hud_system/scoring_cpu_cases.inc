{
    namespace hd=th20::source::hud;namespace gs=th20::source::game_session;namespace ps=th20::source::gameplay::player_state;
    auto& original=*reinterpret_cast<gs::Session*>(mapped_image_base+0x1ba568);const auto saved_source=gs::session,saved_original=original;
    alignas(hd::FrontInf) std::uint8_t storage[sizeof(hd::FrontInf)];auto& front=*reinterpret_cast<hd::FrontInf*>(storage);
    constexpr std::uint64_t edges[]{0,1,31,32,33,0x8d55d,0x8d55e,0x8d55f,0x7fffffff,0x80000000,0xffffffff,0x100000000,0x100000001,0x12345678ffffffff,0x7fffffffffffffff,0x8000000000000000,0xffffffffffffffff};
    for(unsigned test=0;test<32768;++test){diagnostic_case=test;diagnostic_phase="HUD score easing full unsigned64 and sign-extended-low-word edges";
        for(auto& word:*reinterpret_cast<std::array<std::uint32_t,sizeof(front)/4>*>(&front))word=rng();for(auto& word:*reinterpret_cast<std::array<std::uint32_t,sizeof(gs::Session)/4>*>(&gs::session))word=rng();
        const auto target=test<16384?edges[test%std::size(edges)]:(std::uint64_t(rng())<<32)|rng();front.score=test<16384?edges[(test/std::size(edges))%std::size(edges)]:(std::uint64_t(rng())<<32)|rng();ps::write(gs::session.player_table.players[0],0,target);
        if(test%3==0){const auto speed=edges[(test/(std::size(edges)*std::size(edges)))%std::size(edges)];front.field_168=std::uint32_t(speed);front.field_16c=std::uint32_t(speed>>32);}gs::session.player_table.continue_count=int(test%15)-3;original=gs::session;
        const auto before=std::vector<std::uint8_t>(storage,storage+sizeof(storage));cpu<void>(0x4b4490,&front);const auto expected=std::vector<std::uint8_t>(storage,storage+sizeof(storage));const auto expected_session=original;std::memcpy(storage,before.data(),sizeof(storage));hd::update_score(front);check("hud_score_front",expected.data(),storage,sizeof(storage));check("hud_score_session",&expected_session,&gs::session,sizeof(gs::Session));
    }
    gs::session=saved_source;original=saved_original;
}
