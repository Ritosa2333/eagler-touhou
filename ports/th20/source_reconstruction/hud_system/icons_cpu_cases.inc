{
    namespace hd=th20::source::hud;
    alignas(hd::FrontInf) std::uint8_t storage[sizeof(hd::FrontInf)];auto& front=*reinterpret_cast<hd::FrontInf*>(storage);
    std::array<s::Animation,14> animations;
    for(unsigned test=0;test<8192;++test){diagnostic_case=test;diagnostic_phase="HUD seven life/bomb icons actual interrupt writes";
        std::memset(storage,test,sizeof(storage));for(auto& a:animations){s::construct_animation(a);s::reset_animation_state(a);a.base.field_438=rng();a.vector_5bc={float(rng()%100),float(rng()%100),float(rng()%100)};a.base.flags[0]=rng();a.base.flags[1]=rng();a.base.flags[2]=rng();}
        for(unsigned i=0;i<7;++i){front.life_animations[i]=&animations[i];front.bomb_animations[i]=&animations[i+7];}if(test%13==0)front.life_animations[0]=front.bomb_animations[0]=nullptr;
        const int full=int(test%9)-1,fragments=int((test/9)%3),maximum=int((test/27)%8);const auto initial=animations;const auto initial_front=std::vector<std::uint8_t>(storage,storage+sizeof(storage));
        FloatingEnvironment::prepare();cpu<void>(test%2?0x4b8bf0:0x4b8650,&front,full,fragments,maximum);const auto expected=animations;const auto expected_front=std::vector<std::uint8_t>(storage,storage+sizeof(storage));animations=initial;std::memcpy(storage,initial_front.data(),sizeof(storage));FloatingEnvironment::prepare();if(test%2)hd::set_lives(front,full,fragments,maximum);else hd::set_bombs(front,full,fragments,maximum);
        check("hud_icons_animation",expected.data(),animations.data(),sizeof(animations));check("hud_icons_front",expected_front.data(),storage,sizeof(storage));
    }
}
