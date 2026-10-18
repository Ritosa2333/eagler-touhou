// Included by native-gameplay.cpp after its real-world fixture helpers.
void score_cases(const std::vector<u8>& bytes){
    auto peer=std::make_unique<Peer>(bytes);MultiplayerOptions options;options.seat_count=3;options.selections={0,3,5};
    check(peer->session.begin_multiplayer(peer->resources,options),"shared score world");auto& w=*peer->session.battle;
    std::array<MultiplayerInput,3> input{};for(unsigned i=0;i<125;++i)step(*peer,input);
    for(unsigned seat=0;seat<3;++seat){const auto& e=w.pilots[seat]->economy;check(&e.score_units==&w.economy.score_units,"all pilot score references bind one session value");if(seat)check(&e.power!=&w.economy.power&&&e.communication!=&w.economy.communication&&&e.graze!=&w.economy.graze,"personal power communication and graze owners remain independent");}
    w.economy.score_units=0;
    const Vec3 target{0,100,0};const Vec2 size{16,16};
    for(unsigned seat=0;seat<3;++seat)check(w.pilots[seat]->player->shots.damage_areas.circle(target,32,0,31,10*(seat+1),&w.animations.rate)!=nullptr,"native personal damage contribution");
    i32 damage=0;check(w.mp_damage_position(target,size,damage)&&damage==60&&w.economy.score_units==3,"three native shot awards contribute once each to team score");
    check(w.add_score(137)&&w.economy.score_units==16,"shared enemy or ECL score awarded once with native division");
    auto& p2=w.pilots[1]->economy;p2.communication=10000;p2.graze=100;p2.point_value=5000000;
    const i32 p1_communication=w.economy.communication,p1_graze=w.economy.graze;
    ItemState point{};point.type=2;point.position={0,100,0};w.items.multiplayer_current_target=1;bool convert=false;
    check(w.mp_collect(point,convert)&&w.economy.score_units==5066,"P2 point reward uses P2 communication and graze then adds 50500 points to shared score");
    check(w.economy.communication==p1_communication&&w.economy.graze==p1_graze,"shared score pickup cannot merge personal communication or graze");
    w.spell_flags=3;w.spell_id=1;w.spell_bonus=12340;
    check(w.spells.end()&&w.economy.score_units==6300,"one captured shared spell adds its bonus once");
    w.pilots[2]->economy.add_score(109);
    check(w.economy.score_units==6310&&w.pilots[1]->economy.score_units==6310,"P3 score is immediately the same value before any world tick");
    w.economy.score_units=999999995;w.pilots[1]->economy.add_score(100);
    check(w.economy.score_units==999999999&&w.pilots[2]->economy.score_units==999999999,"single native score cap applies across all contributors");
    check(peer->session.draw(peer->renderer),"single native score HUD draw");unsigned high=0,current=0;
    for(const auto& request:w.ascii.requests){if(request.position.x==508&&request.position.y==48)++high;if(request.position.x==508&&request.position.y==72)++current;}
    check(high==1&&current==1,"exactly one HiScore and Score use their original native positions");
    struct Effects final:StageCompletionEffects{bool stage_result_animation()override{return true;}} effects;
    for(unsigned stage:{1u,6u,7u}){
        GameEconomy team;GameEconomy second(team.score_units),third(team.score_units);ClearRecords records;float rate=1;
        std::array<GameEconomy*,3> economies{&team,&second,&third};
        const i32 lives[]={2,1,-1},powers[]={40,48,20},points[]={5000000,6000000,7000000};
        for(unsigned seat=0;seat<3;++seat){auto& e=*economies[seat];e.lives=lives[seat];e.power=powers[seat];e.point_value=points[seat];e.difficulty=stage==7?4:1;}
        StageCompletion completion(team,records,effects,&rate);completion.mode.stage=stage;completion.multiplayer_economies=economies;completion.multiplayer_count=3;
        check(completion.complete(),"native shared stage completion");
        if(stage==1)check(team.score_units==400000&&completion.state.displayed_bonus==4000000&&completion.state.exit==StageExit::NextStage,"stage base once plus 2+1+0 remaining lives, ghost negative life cannot subtract score");
        if(stage==6)check(team.score_units==28020000&&completion.state.displayed_bonus==100200000,"Normal clear contributes 9000000 stage/life + 180000000 personal point + 91200000 life/power");
        if(stage==7)check(team.score_units==35320000&&completion.state.exit==StageExit::Results,"Extra clear contributes 10000000 stage/life + 180000000 personal point + 163200000 life/power");
    }
    for(unsigned stage:{1u,6u,7u}){
        GameEconomy economy;economy.lives=2;economy.power=40;economy.point_value=5000000;economy.difficulty=stage==7?4:1;
        ClearRecords records;float rate=1;StageCompletion completion(economy,records,effects,&rate);completion.mode.stage=stage;
        check(completion.complete(),"ordinary single-economy stage completion branch");
        check(economy.score_units==(stage==1?300000:stage==6?11400000:15500000),"ordinary award order and formulas remain 3m/114m/155m points");
    }
    options.seed+=1;check(peer->session.begin_multiplayer(peer->resources,options),"shared score generation reset");
    for(unsigned seat=0;seat<3;++seat)check(peer->session.multiplayer_seat(seat).score==0&&&peer->session.battle->pilots[seat]->economy.score_units==&peer->session.economy.score_units,"restart resets one score and rebinds every permanent pilot to the new world");
    std::puts("PASS: shared score owner, personal shot/item contributions, one enemy/spell award, native cap, stage/Extra resource bonuses, original one-score HUD");
}
