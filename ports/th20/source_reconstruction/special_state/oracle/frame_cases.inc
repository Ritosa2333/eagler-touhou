std::cerr<<"complete frame, entry, collection and overlay draw\n";
hook(0x478060,reinterpret_cast<void*>(&enemy_owner_boundary));hook(0x49be20,reinterpret_cast<void*>(&charge_boundary));hook(0x488850,reinterpret_cast<void*>(&mode_boundary));hook(0x478160,reinterpret_cast<void*>(&boss_boundary));hook(0x4aaac0,reinterpret_cast<void*>(&find_enemy_boundary));hook(0x4aadb0,reinterpret_cast<void*>(&enemy_position_boundary));hook(0x4a8920,reinterpret_cast<void*>(&spawn_boundary));hook(0x49db70,reinterpret_cast<void*>(&effect_boundary));hook(0x44fcd0,reinterpret_cast<void*>(&delete_boundary));hook(0x413b70,reinterpret_cast<void*>(&free_boundary));hook(0x47a2c0,reinterpret_cast<void*>(&position_boundary));hook(0x47cf60,reinterpret_cast<void*>(&bullets_boundary));hook(0x4caad0,reinterpret_cast<void*>(&lasers_boundary));
hook(0x4b8620,reinterpret_cast<void*>(&text_style_boundary));hook(0x488b00,reinterpret_cast<void*>(&text_font_boundary));hook(0x470ad0,reinterpret_cast<void*>(&text_color_boundary));hook(0x488940,reinterpret_cast<void*>(&text_alpha_boundary));hook(0x4b8d90,reinterpret_cast<void*>(&text_scale_boundary));hook(0x4e6920,reinterpret_cast<void*>(&text_save_boundary));hook(0x4e67e0,reinterpret_cast<void*>(&text_restore_boundary));hook(0x46ce90,reinterpret_cast<void*>(&line_boundary));hook(0x46cfa0,reinterpret_cast<void*>(&level_boundary));hook(0x498a80,reinterpret_cast<void*>(&draw_find_boundary));
auto compare_events=[&](const char* name,unsigned test,const std::vector<Event>& expected){const auto a=expected.size(),b=events.size();check(name,test,&a,&b,sizeof(a));if(a==b)check(name,test,expected.data(),events.data(),a*sizeof(Event));};
std::array<ss::Entry,8> entries;
ss::Controller owner;
for(unsigned test=0;test<4096;++test){
    fill(owner);ss::construct(owner);for(auto& entry:entries){fill(entry);ss::construct(entry);entry.color=int(random()%4);entry.enemy_handle=1+random()%15;entry.animation_handle=random();entry.level=int(random()%5);r::timer_set(entry.age,int(random()%160)-10);}
    const unsigned count=random()%9;for(unsigned i=0;i<count;++i)sc::append(owner.entries,entries[i].link);owner.active=std::uint8_t(test%2);r::timer_set(owner.age,int(random()%1900));r::timer_set(owner.charge_age,int(random()%50)-10);
    ss::start(owner.text_expansion,60,4,1.f,0.f);ss::start(owner.text_alpha,120,4,255,0);ss::start(owner.meter_remaining,1320,5,1.f,0.f);r::timer_set(owner.text_expansion.timer,int(random()%70));r::timer_set(owner.text_alpha.timer,int(random()%130));r::timer_set(owner.meter_remaining.timer,int(random()%1400));
    fill(player);player.fields_30[(0x98-0x30)/4]=unsigned(int(random()%5)-1); //actual valid forced colors[-1,3]
    player.fields_30[(0x5c-0x30)/4]=random()%12000;player.fields_30[(0x60-0x30)/4]=random()%5500;
    for(unsigned i=0;i<4;++i){player.fields_30[(0x64-0x30)/4+i]=unsigned(int(random()%1600)-300);player.fields_30[(0x74-0x30)/4+i]=unsigned(int(random()%8)-2);}
    charge=test%4!=0;boss=test%7==0;blocked=test%11==0;controller_exists=test%13!=0;session_mode=test%9==0?2:0;for(auto& value:exists)value=random()%5!=0;origin={float(int(random()%300)-150),float(random()%400),0};
    *reinterpret_cast<unsigned*>(mapped_image_base+0x1c6114)=blocked?1u:0u;float rate=float(1+random()%8)*.25f;st::timer_rate=&rate;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=&rate;
    st::Random rng(0);rng.state=random();rng.last=random();rng.minimum=0;rng.upper=0x7fffffff;rng.modulus=0x7fffffff;st::random_streams[0]=rng;std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),&rng,sizeof(rng));
    const auto before_owner=owner;const auto before_entries=entries;const auto before_player=player;
    events.clear();FloatingEnvironment::prepare();cpu<void>(0x5120d0,&owner);const auto expected_owner=owner;const auto expected_entries=entries;const auto expected_player=player;const auto expected_events=events;st::Random expected_rng(0);std::memcpy(&expected_rng,reinterpret_cast<void*>(mapped_image_base+0x1ba4a8),sizeof(rng));
    owner=before_owner;entries=before_entries;player=before_player;events.clear();FloatingEnvironment::prepare();ss::update(owner,host);
    check("frame_controller",test,&expected_owner,&owner,sizeof(owner));check("frame_entries",test,expected_entries.data(),entries.data(),sizeof(entries));check("frame_player",test,&expected_player,&player,sizeof(player));check("frame_rng",test,&expected_rng,&st::random_streams[0],sizeof(rng));compare_events("frame_events",test,expected_events);
    //Draw from a live, valid first entry; its text interpolation deliberately
    //retains arbitrary frame values so evaluate-without-tick is tested.
    owner=before_owner;entries=before_entries;owner.active=1;r::timer_set(owner.age,int(random()%220));const auto initial=owner;events.clear();text_color=0xffffffff;text_font=2;text_scale_x=text_scale_y=1;text_x=text_y=1;text_blend=0;
    FloatingEnvironment::prepare();cpu<void>(0x512aa0,&owner);const auto drawn=owner;const auto draw_events=events;
    owner=initial;events.clear();text_color=0xffffffff;text_font=2;text_scale_x=text_scale_y=1;text_x=text_y=1;text_blend=0;FloatingEnvironment::prepare();ss::draw(owner,host);check("draw_controller",test,&drawn,&owner,sizeof(owner));compare_events("draw_events",test,draw_events);
}
for(unsigned test=0;test<4096;++test){
    ss::Entry entry;ss::construct(entry);entry.color=int(random()%8)-2;ss::Entry* capture=&entry;
    fill(player);player.fields_30[(0x5c-0x30)/4]=unsigned(int(random()%16000)-3000);player.fields_30[(0x60-0x30)/4]=unsigned(int(random()%9000)-2000);
    for(unsigned i=0;i<4;++i)player.fields_30[(0x74-0x30)/4+i]=random();const auto before=player;
    events.clear();const auto er=cpu<int>(0x511f00,&capture,reinterpret_cast<void*>(1));const auto ep=player;const auto ee=events;
    player=before;events.clear();const auto ar=ss::collect(entry,reinterpret_cast<void*>(1),host);check("collect_player",test,&ep,&player,sizeof(player));check("collect_return",test,&er,&ar,4);compare_events("collect_events",test,ee);
}
