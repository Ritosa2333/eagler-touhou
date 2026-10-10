// Real TH11 PlayerFrame/Bomb/Item/HUD owners, driven by confirmed input ticks.
// Forced collision entry isolates lifecycle coverage from stage patterns.
void challenge_cases(const std::vector<u8>& bytes){
    auto a=std::make_unique<Peer>(bytes),b=std::make_unique<Peer>(bytes);
    MultiplayerOptions options;options.seat_count=3;options.challenge=true;options.seed=417;
    std::array<MultiplayerInput,3> input{};
    for(int group:{0,3}){
        options.selections={group,group+1,group+2};options.local_seat=0;
        check(a->session.begin_multiplayer(a->resources,options),"challenge begins with selected native loadouts");
        options.local_seat=2;check(b->session.begin_multiplayer(b->resources,options),"challenge peer begins from same seed");
        auto both=[&]{step(*a,input);step(*b,input);check(a->session.multiplayer_hash()==b->session.multiplayer_hash(),"challenge peers agree through native lifecycle and local rendering");};
        input={};for(unsigned i=0;i<125;++i)both();
        for(auto* peer:{a.get(),b.get()})for(unsigned seat=0;seat<3;++seat){auto& p=*peer->session.battle->pilots[seat];p.economy.power=p.economy.max_power;p.economy.lives=0;check(p.power_changed(),"challenge prepare native max Power");check(!p.start_bomb()&&p.bomb->start()==-1&&!p.bomb->state.active,"challenge blocks callback and direct native Bomb start");}
        for(auto& in:input)in.held=2;
        both();
        for(unsigned seat=0;seat<3;++seat){const auto& p=*a->session.battle->pilots[seat];check(p.economy.power==p.economy.max_power&&!p.bomb->state.active&&!(p.player->motion.state.flags&4),"all six Bomb inputs cost no Power and cannot create Nitori shield");check(!(p.keys.held&2)&&!p.player->input.special_available,"challenge input and native availability both block Bomb");}
        const unsigned repetitions=group==3?10:1;
        for(unsigned death=1;death<=repetitions;++death){
            for(auto* peer:{a.get(),b.get()}){auto& w=*peer->session.battle;for(unsigned seat=0;seat<3;++seat){auto& p=*w.pilots[seat];p.economy.power=p.economy.max_power;check(p.power_changed(),"challenge death Power setup");check(w.mp_hit(seat,{CollisionKind::Hit,true}),"challenge actual owner enters deathbomb window");}}
            for(unsigned frame=0;frame<100;++frame){both();
                if(frame==8)for(unsigned seat=0;seat<3;++seat){const auto& p=*a->session.battle->pilots[seat];check(p.player->state.life_state==2&&p.challenge_misses==death&&p.economy.lives==0,"held Bomb cannot deathbomb; each resolved Miss increments once without consuming zero lives");}
                if(frame==12){auto& w=*a->session.battle;unsigned drops=0;for(unsigned i=0;i<ItemManager::capacity;++i){const auto& item=w.items.at(i);if(!item.state)continue;check(item.type!=6,"challenge death keeps normal P drops, never final-life Full Power");if(item.lifetime.current<=5&&(item.type==1||item.type==4))++drops;}check(drops>=21,"all three normal deaths emit seven native P items each");}
            }
            auto& w=*a->session.battle;for(unsigned seat=0;seat<3;++seat){const auto& p=*w.pilots[seat];check(p.player->state.life_state==1&&!p.ghost&&p.challenge_misses==death&&p.economy.lives==0,"challenge respawns after every death including more than nine Misses");}
            check(w.mp_wipe_frames==0&&!w.game_over_requested&&a->session.state.phase==GameSessionPhase::stage,"simultaneous challenge deaths never start ghost wipe");
            check(w.enemy_environment.shared_integers[0]==i32(death*3),"challenge Miss still reaches native shared stage death accounting");
        }
        compare_owners(*a->session.battle,*b->session.battle);
    }
    auto& w=*a->session.battle;auto& giver=*w.pilots[0];auto& receiver=*w.pilots[1];
    input={};giver.economy.lives=3;receiver.economy.lives=1;
    put(*giver.player,0,400);put(*receiver.player,10,400);put(*w.pilots[2]->player,100,400);
    for(auto& p:w.pilots)p->player->state.invincibility.set(100000,&w.animations.rate);
    for(unsigned i=0;i<106;++i){input[0].held=8;step(*a,input);}
    check(giver.economy.lives==2&&receiver.economy.lives==2,"challenge retains hidden-life native transfer and delivery");
    ItemState fragment{};fragment.type=5;bool convert=false;w.items.multiplayer_current_target=0;
    for(unsigned i=0;i<5;++i)check(w.mp_collect(fragment,convert),"challenge team fragment pickup");
    check(giver.economy.lives==3&&receiver.economy.lives==3&&w.pilots[2]->economy.lives==1&&w.mp_fragments==0,"challenge five fragments award hidden team stock");
    for(auto& p:w.pilots)check(p->challenge_misses==10,"transfers and Extends cannot change cumulative Miss");
    giver.economy.lives=9;check(giver.rewards.add_life()&&giver.economy.lives==9&&giver.challenge_misses==10,"challenge hidden stock still caps at nine");
    w.items.reset();input={};step(*a,input);giver.economy.power=giver.economy.power_step*2;receiver.economy.power=0;w.pilots[2]->economy.power=w.pilots[2]->economy.max_power;
    for(unsigned i=0;i<5;++i){input[0].held=1;step(*a,input);input[0].held=0;step(*a,input);}for(unsigned i=0;i<16;++i)step(*a,input);
    check(giver.economy.power==giver.economy.power_step&&receiver.economy.power==receiver.economy.power_step,"challenge retains loadout-specific Power transfer");
    check(w.next_stage(a->resources,2),"challenge native stage transition");for(unsigned i=0;i<170;++i)step(*a,input);
    for(unsigned seat=0;seat<3;++seat)check(a->session.multiplayer_seat(seat).misses==10&&!w.pilots[seat]->ghost,"challenge stage transition preserves cumulative Miss");
    check(a->session.draw(a->renderer),"challenge HUD draw");unsigned miss_rows=0;for(const auto& text:w.ascii.requests)if(text.position.x==520&&(text.position.y==104||text.position.y==176||text.position.y==248)){++miss_rows;check(text.text=="10"&&text.style.font==3,"native life row renders cumulative Miss using native digits");}check(miss_rows==3,"every challenge life group renders its own Miss count");
    const auto before=a->session.multiplayer_hash();++giver.challenge_misses;check(a->session.multiplayer_hash()!=before,"Miss participates in deterministic state identity");--giver.challenge_misses;
    giver.challenge_misses=0xffffffffu;check(giver.record_death()&&giver.challenge_misses==0xffffffffu,"challenge Miss saturates like TH10");
    input[0].pause=true;step(*a,input);check(a->session.state.phase==GameSessionPhase::paused,"challenge restart starts from real paused owner");
    for(unsigned seed:{417u,418u}){options.seed=seed;options.local_seat=0;check(a->session.begin_multiplayer(a->resources,options)&&b->session.begin_multiplayer(b->resources,options),"challenge negotiated new run / replay reconstruction");input={};for(unsigned seat=0;seat<3;++seat)check(a->session.multiplayer_seat(seat).misses==0,"challenge new run clears all cumulative Miss counters");for(unsigned frame=0;frame<120;++frame){input[0].held=2|(frame<60?128u:64u);step(*a,input);step(*b,input);check(a->session.multiplayer_hash()==b->session.multiplayer_hash(),"challenge new-run seed reconstructs identical confirmed history");}}
    options.challenge=false;check(a->session.begin_multiplayer(a->resources,options),"ordinary MP rules still available");auto& ordinary=*a->session.battle;input={};for(unsigned i=0;i<125;++i)step(*a,input);auto& p=*ordinary.pilots[0];p.economy.power=p.economy.max_power;check(p.power_changed(),"ordinary Bomb Power setup");input[0].held=2;step(*a,input);check(p.bomb->state.active&&p.economy.power==p.economy.max_power-p.economy.power_step,"ordinary MP Bomb and native cost unchanged");
    auto& victim=*ordinary.pilots[1];victim.economy.lives=0;check(ordinary.mp_hit(1,{CollisionKind::Hit,true}),"ordinary last-life collision");input={};for(unsigned i=0;i<41;++i)step(*a,input);check(victim.ghost&&victim.economy.lives==-1&&victim.challenge_misses==0,"ordinary MP final death still becomes ghost");
    std::puts("PASS: challenge six loadouts/Bomb/deathbomb/Nitori, >9 zero-stock native deaths/drops/respawns, no wipe, independent cumulative Miss, hidden-life transfers/Extends, Power transfer, stage/restart/seed/HUD and ordinary MP control");
}
