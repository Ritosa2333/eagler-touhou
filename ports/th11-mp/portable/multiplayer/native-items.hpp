// Included inside native-gameplay.cpp's fixture namespace. These cases use
// actual TH11 resources, PlayerFrame bounds, ItemManager motion and rewards.
void item_risk_cases(const std::vector<u8>& bytes){
    auto peer=std::make_unique<Peer>(bytes);MultiplayerOptions options;options.seat_count=3;options.selections={0,3,5};
    check(peer->session.begin_multiplayer(peer->resources,options),"item authority world begin");
    auto& w=*peer->session.battle;std::array<MultiplayerInput,3> inputs{};
    for(unsigned frame=0;frame<125;++frame)step(*peer,inputs);
    check(w.stage_active&&(!w.dialogue||!w.dialogue->active),"item fixture has active native battle");
    auto place=[&](unsigned seat,float x,float y,int communication=0,bool focused=false){
        auto& p=*w.pilots[seat];put(*p.player,x,y);p.input.movement.held=focused?8:0;p.input.movement.pressed=0;
        p.player->state.invincibility.set(100000,&w.animations.rate);
        check(p.tick(),"refresh actual native item bounds");p.economy.communication=communication;
    };
    auto fresh=[&](int type,Vec3 position,int age=40)->ItemState&{
        w.items.reset();const unsigned index=type==8?ItemManager::ordinary_capacity+w.items.cancel_cursor:0;
        check(w.items.spawn(type,position,0xffffffff,-1.57079637f,0)==0,"spawn native item fixture");
        // Isolate one native item for pickup/owner boundary tests. Batch
        // counts and trajectories are covered by resource_cases separately.
        if(type!=8)for(unsigned n=1;n<ItemManager::ordinary_capacity;++n)w.items.at(n).state=0;
        auto& item=w.items.at(index);item.position=position;if(type==8)check(w.items.activate(item),"activate actual cancel item");
        item.lifetime.set(age,&w.animations.rate);item.velocity={};return item;
    };
    auto owner=[&](ItemState& item){ItemPlayer selected{};const bool found=w.multiplayer_item_player(item,selected);
        check(found==(w.items.multiplayer_current_target>=0),"item selection and owner agree");return w.items.multiplayer_current_target;
    };
    auto& p=*w.pilots[0];auto& q=*w.pilots[1];auto& r=*w.pilots[2];
    for(auto& pilot:w.pilots)if(pilot)pilot->economy.power=0;

    place(0,0,250);place(1,150,400,10000);place(2,-150,400);
    auto& remote=fresh(2,{0,190,0});
    check(owner(remote)==1,"near ineligible player cannot block remote communication attraction");
    check(w.items.update()&&remote.state==3&&remote.velocity.x>0&&remote.velocity.y>0,"selected remote player drives actual native homing");
    auto& young=fresh(2,{0,190,0},39);check(owner(young)<0,"communication attraction retains native age-40 threshold");
    check(w.items.update()&&young.state==1,"unqualified item keeps ordinary falling phase");
    place(1,150,127);auto& above=fresh(2,{0,190,0},0);check(owner(above)==1,"native top-of-screen attraction before age 40");
    place(1,150,128);auto& boundary=fresh(2,{0,190,0},0);check(owner(boundary)<0,"native y-128 boundary is not full-screen attraction");
    place(1,150,400);r.force_attract=true;auto& forced=fresh(2,{0,190,0},0);check(owner(forced)==2,"per-seat forced attraction qualifies its owner");
    place(2,-150,400);place(1,150,400,10000);
    auto& unfocused=fresh(2,{0,210,0});check(owner(unfocused)==1,"outside unfocused range does not block remote attraction");
    place(0,0,250,0,true);auto& focused=fresh(2,{0,210,0});check(owner(focused)==0,"focused native attraction radius qualifies nearby player");
    check(w.items.update()&&focused.state==4,"focused nearby attraction retains original slow-homing phase");

    place(0,180,400);place(1,-100,300,10000);place(2,100,300,10000);
    auto& tie=fresh(2,{0,300,0});check(owner(tie)==1,"equal eligible distance selects lower seat");
    place(1,-120,300,10000);place(2,80,300,10000);auto& nearest=fresh(2,{0,300,0});check(owner(nearest)==2,"nearest is chosen only among eligible players");
    r.player->state.life_state=4;auto& dying=fresh(2,{0,300,0});check(owner(dying)==1,"deathbomb/death state cannot own ordinary items");r.player->state.life_state=1;

    place(0,0,300,10000);place(1,100,300,10000);place(2,-120,300,10000);
    p.economy.power=p.economy.max_power;q.economy.power=0;r.economy.power=r.economy.max_power;
    auto& power=fresh(4,{0,300,0});check(owner(power)==1,"eligible unfinished Power overrides a closer full-Power seat");
    q.economy.power=q.economy.max_power;check(owner(power)==0,"existing Power falls back to nearest eligible seat when all are full");

    place(0,0,400);place(1,150,400);place(2,-150,400);
    auto& apex=fresh(8,{0,200,0});apex.velocity.y=-.01f;
    check(owner(apex)==0,"cancel-item attraction uses native post-gravity apex crossing");
    check(w.items.update()&&apex.state==3,"apex-crossing cancel item starts homing in the same native tick");
    auto& rising=fresh(8,{0,200,0});rising.velocity.y=-.04f;
    check(owner(rising)<0&&w.items.update()&&rising.state==2,"still-rising cancel item keeps its native flight phase");
    place(0,0,300);auto& entering=fresh(2,{31,300,0},0);entering.velocity.x=-17;
    const auto score=p.economy.score_units;check(owner(entering)==0,"this tick's motion can enter native pickup bounds");
    check(w.items.update()&&entering.state==0&&p.economy.score_units>score,"newly entered pickup is awarded in the same tick");
    place(1,150,400,10000);auto& leaving=fresh(2,{14,300,0});leaving.velocity.x=45;
    check(owner(leaving)==1,"pre-motion overlap cannot block another eligible collector after motion leaves range");

    // Emit through the actual five-tap transfer rule, then let the still-locked
    // receiver move away before the native item flight reaches them.
    w.items.reset();place(0,0,400);place(1,10,400);place(2,150,400);
    p.economy.power=40;q.economy.power=0;r.economy.power=0;p.keys=q.keys=r.keys={};p.power_taps=p.power_gap=0;
    for(unsigned tap=0;tap<5;++tap){p.keys.held=p.keys.pressed=1;check(w.mp_update_rules(),"emit transfer tap");p.keys.held=p.keys.pressed=0;check(w.mp_update_rules(),"release transfer tap");}
    auto& gift=w.items.at(0);const int paid=p.economy.power;
    check(paid==20&&gift.type==4&&gift.state==3&&w.items.multiplayer_targets[0]==1,"transfer consumes donor once and locks recipient");
    place(1,120,400);check(w.items.update()&&gift.state==3&&w.items.multiplayer_targets[0]==1,"Power gift is in actual directed flight");
    q.economy.lives=-1;check(q.game_over(false),"directed recipient becomes native ghost");
    put(*q.player,gift.position.x,gift.position.y);place(0,-150,350);place(2,150,350);
    const int ghost_power=q.economy.power;check(w.items.update(),"detach Power from ghost recipient");
    check(w.items.multiplayer_targets[0]==-1&&gift.state==1&&gift.type==4,"ghost-directed Power becomes ordinary field Power");
    check(q.economy.power==ghost_power&&p.economy.power==paid,"detachment neither feeds ghost nor refunds donor");
    for(unsigned frame=0;frame<8;++frame)check(w.items.update(),"detached Power remains in native falling pool");
    check(gift.state==1&&q.ghost,"ordinary detached item does not revive or feed its ghost");
    place(2,gift.position.x,gift.position.y);const int receiver_power=r.economy.power;
    check(w.items.update()&&gift.state==0&&r.economy.power==receiver_power+r.economy.power_step,"legal living player collects detached Power using own native step");
    check(q.economy.power==ghost_power&&p.economy.power==paid,"later collection preserves ghost and donor resource balances");

    // A fragment after the last Power pickup in the same ItemManager update
    // must not clear the pending all-Power-full conversion of remaining items.
    check(q.revive(true),"restore recipient for all-full conversion case");
    place(0,0,400);place(1,150,400);place(2,-150,400);w.items.reset();
    p.economy.power=p.economy.max_power-1;q.economy.power=q.economy.max_power;r.economy.power=r.economy.max_power;
    check(w.items.spawn(1,{0,400,0},0xffffffff,-1.57079637f,0)==0,"spawn final Power unit");
    check(w.items.spawn(5,{0,400,0},0xffffffff,-1.57079637f,0)==0,"spawn later same-tick fragment");
    check(w.items.spawn(4,{0,100,0},0xffffffff,-1.57079637f,0)==0,"spawn uncollected field Power before team becomes full");
    const unsigned fragments=w.mp_fragments;check(w.items.update(),"ordered final Power and fragment collection");
    check(w.mp_all_power_full()&&w.mp_fragments==(fragments+1)%5,"same-tick Power and fragment rewards both applied");
    unsigned converted=0,remaining_power=0;for(unsigned i=0;i<ItemManager::ordinary_capacity;++i){const auto& item=w.items.at(i);if(!item.state)continue;
        converted+=item.type==9;remaining_power+=item.type==1||item.type==4||item.type==10||item.type==11;
    }
    check(converted>0&&remaining_power==0,"later fragment cannot clear all-full field-Power conversion");
    std::puts("PASS: native eligibility-before-nearest, focus/age/apex/pickup phases, Power ghost detachment, same-tick conversion");
}
