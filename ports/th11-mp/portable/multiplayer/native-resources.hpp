// Included after the native Peer/step helpers. These cases exercise the actual
// stage-drop and pickup owners, then the real renderer's queued HUD text.
void resource_cases(const std::vector<u8>& bytes){
    for(unsigned count:{2u,3u}){
        auto peer=std::make_unique<Peer>(bytes);MultiplayerOptions options;options.seat_count=count;options.selections={0,3,5};
        check(peer->session.begin_multiplayer(peer->resources,options),"resource rule world");auto& w=*peer->session.battle;
        std::array<MultiplayerInput,3> input{};for(unsigned i=0;i<125;++i)step(*peer,input);
        auto item_count=[&](i32 type){unsigned total=0;for(unsigned i=0;i<ItemManager::capacity;++i)if(w.items.at(i).state&&w.items.at(i).type==type)++total;return total;};
        for(i32 type:{1,4,5,6,10,11}){
            EnemyState enemy{};enemy.current.position={0,100,0};enemy.drops.primary=type;enemy.drops.counts[type-1]=2;
            const auto before=item_count(type);check(w.drop_items(enemy),"native stage primary and counted item drop");
            const unsigned expected=3u*((type==1||type==4||type==10||type==11)?count:1u);
            check(item_count(type)==before+expected,"fragments and Full Power stay x1; ordinary Power is 2P x2 and 3P x3");
            check(enemy.drops.primary==0&&enemy.drops.counts[type-1]==0,"stage drop request is consumed once");
        }
        for(int type:{1,4,7})for(float x:{-192.f,0.f,192.f}){
            w.items.reset();const unsigned copies=type==7?count-1:count;
            check(w.items.spawn(type,{x,100,0})==0,"roster item batch at field edge");check(item_count(type)==copies,"batch quantity follows roster independently of operable slots");
            for(unsigned n=0;n<copies;++n){const auto& item=w.items.at(n);check(item.position.x>=-192&&item.position.x<=192&&item.velocity.y<0,"each batch item starts in field and moves upward");if(n)check(item.position.x-w.items.at(n-1).position.x>=17.99f,"copies start at least eighteen pixels apart");}
            for(unsigned n=0;n<13;++n)check(w.items.update(),"native batch spreading ticks");
            for(unsigned n=0;n<copies;++n){const auto& item=w.items.at(n);check(item.position.x>=-192&&item.position.x<=192,"twelve-tick fan stays in bounds");if(copies>1)check(item.velocity.x==0,"fan stops horizontal spreading after twelve native ticks");}
        }
        w.items.reset();check(w.items.spawn(1,{0,100,0},0xffffffff,-.9f,3)==0&&item_count(1)==count,"death-style ordinary P also follows roster multiplier");
        for(unsigned n=1;n<count;++n)check(float_bits(w.items.at(n).velocity.x)==float_bits(w.items.at(0).velocity.x)&&float_bits(w.items.at(n).velocity.y)==float_bits(w.items.at(0).velocity.y),"death copies retain the authored original arc");
        w.items.reset();check(w.items.spawn_transfer(4,{0,100,0},1)&&item_count(4)==1,"Power gifts remain one exact native item, never multiplied");
        auto& ghost=*w.pilots[count-1];ghost.economy.lives=-1;check(ghost.game_over(false),"fragment reward native ghost entry");
        w.pilots[0]->economy.lives=8;if(count==3)w.pilots[1]->economy.lives=9;
        ItemState fragment{};fragment.type=5;bool convert=false;
        for(unsigned piece=1;piece<=10;++piece){
            w.items.multiplayer_current_target=i32((piece-1)%(count-1));check(w.mp_collect(fragment,convert),"shared fragment pickup by a living teammate");
            check(w.mp_fragments==piece%5,"shared pool distributes exactly on every fifth fragment");
            check(w.pilots[0]->economy.lives==(piece<5?8:9),"first living seat receives one life and caps at nine");
            if(count==3)check(w.pilots[1]->economy.lives==9,"a capped collector still contributes to the team pool without exceeding nine");
            check(ghost.economy.lives==-1+i32(piece/5)&&ghost.ghost,"each fifth fragment grants the ghost one reserve life without reviving it");
            for(unsigned seat=0;seat<count;++seat)check(w.pilots[seat]->economy.life_fragments==i32(piece%5),"all native life icons read the same remaining fragment count");
        }
    }
    std::puts("PASS: 2P/3P original fragment drop quantities, separate Power multiplier, every-five team lives, cap nine and ghost preservation");
}

void local_hud_cases(Peer& peer){
    auto& w=*peer.session.battle;
    const i32 graze[]={111,222,333},point_value[]={5000000,6000000,7000000},communication[]={2500,5000,10000};
    const char* point_text[]={"050000*0.26","060000*0.52","070000*1.03"};
    for(unsigned seat=0;seat<3;++seat){auto& e=w.pilots[seat]->economy;e.graze=graze[seat];e.point_value=point_value[seat];e.communication=communication[seat];}
    check(w.mp_update_presentation(),"prepare distinct native local HUD values");
    const auto identity=peer.session.multiplayer_hash(),rng=w.animations.script_rng.calls,frame=w.frame;
    const unsigned previous_local=w.mp_options.local_seat;
    const auto original_comm=w.mp_communication_icons;
    for(unsigned local=0;local<3;++local){
        w.mp_options.local_seat=local;check(peer.session.draw(peer.renderer),"draw a different local view of the same authoritative world");
        unsigned high=0,score=0,graze_rows=0,point_rows=0,tags=0;
        for(const auto& request:w.ascii.requests){
            if(request.position.x==508&&request.position.y==48)++high;
            if(request.position.x==508&&request.position.y==72)++score;
            if(request.position.x==520&&request.position.y==320){++graze_rows;check(request.text==std::to_string(graze[local])&&request.style.font==3&&request.style.pass==0,"local Graze follows all three resource groups");}
            check(request.position.x!=520||request.position.y!=152,"Graze cannot appear within P1's resource group");
            if(request.text.find('*')!=std::string::npos){++point_rows;check(request.position.x==48&&request.position.y==455&&request.text==point_text[local]&&request.style.font==2&&request.style.pass==1,"only local point value and communication use the original inner HUD row");}
            for(unsigned seat=0;seat<3;++seat)if(request.text==std::to_string(seat+1)+"P"){
                ++tags;const float y[]={88,160,232};check(request.position.x==436&&request.position.y==y[seat]&&request.style.font==1,"seat number starts its native Life/Power group");
                check(request.style.color==(seat==local?0xffffff00u:0xffffffffu),"only the local seat number receives original yellow emphasis");
            }
        }
        check(high==1&&score==1&&graze_rows==1&&point_rows==1&&tags==3,"one shared score, one local statistics pair and three resource headings");
        check(peer.session.multiplayer_hash()==identity&&w.animations.script_rng.calls==rng&&w.frame==frame,"local HUD selection cannot advance or mutate authoritative simulation");
        check(std::memcmp(w.mp_communication_icons.data(),original_comm.data(),sizeof(original_comm))==0,"draw copies never alter native communication VMs");
    }
    w.mp_options.local_seat=previous_local;
    std::puts("PASS: local-only Graze/point/communication at native positions, leading yellow local seat and invariant authority across three views");
}
