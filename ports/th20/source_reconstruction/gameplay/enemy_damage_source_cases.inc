// Source control-flow tests keep external game objects explicit. The complete
// damage-region geometry/aggregation and scalar health routines have their own
// original CPU suites; these tests exercise their ordering at4a5df0.
for(unsigned scenario=0;scenario<64;++scenario){
    EnemyDamageFixture host;gp::EnemyState state;state.animations.resize(1);state.animations[0].handle=1;
    s::Animation animation{};host.lookup=[&](std::uint32_t handle){return handle==1?&animation:nullptr;};
    std::array<std::uint8_t,0x428> entity{};state.entity=entity.data();field(entity.data(),0x88,std::uint32_t{7});
    std::array<std::uint8_t,0x2300> primary{};std::array<std::uint8_t,0xc4> secondary{};std::array<std::uint8_t,0x3c> bomb{};
    std::array<std::uint8_t,0x40> overlay{},bonus_feedback{};
    alignas(gp::EnemyController)std::array<std::uint8_t,sizeof(gp::EnemyController)> owner_bytes{};auto& owner=*reinterpret_cast<gp::EnemyController*>(owner_bytes.data());
    auto& context=host.storage.contexts[0];context.objects_04[0]=primary.data();context.objects_04[1]=&owner;context.objects_04[3]=secondary.data();context.objects_04[5]=bomb.data();context.current_player=&host.storage.player_table.players[0];context.overlay_owner=reinterpret_cast<rt::CallbackOwner*>(overlay.data());
    field(overlay.data(),0x34,bonus_feedback.data());state.fields_2c8[9]=reinterpret_cast<std::uint32_t>(&context);
    field(primary.data(),0x10,1);field(primary.data(),0x223c,1.0f);field(primary.data(),0x227c,1);field(primary.data(),0x614,s::Vec3{3,4,5});
    field(&state,0x4c,1.0f);field(&state,0x54,-1);field(&state,0x264,-1);state.auxiliary_18c={{10000,0,10000,0,0,0,0}};
    field(&state.motion_110,0,s::Vec3{30,40,50});state.bounds_5c={8,6};state.bounds_64={10,12};state.fields_2c8[0]=2;host.amount=100;host.hit_flag=scenario&1;
    state.fields_2c8[1]=scenario&2?0x40000000u:0;field(context.current_player,0xac,20);
    if(scenario&4)field(primary.data(),0x10,2);
    if(scenario&8)state.fields_2c8[0]|=0x10u;
    if(scenario&16)field(secondary.data(),0x78,std::uint32_t{0x21});
    if(scenario&32)state.fields_2c8[0]|=0x1000u;
    const int before_reduction=(scenario&2?120:100)/(scenario&4?5:1);
    const int amount=before_reduction/(scenario&16?30:1);
    check(gp::update_enemy_damage(state,host)==0,"Damage source entry result differs");
    check(host.events==std::vector<int>({10,50}),"Damage/query and sound order differs");
    check(host.preview==0&&host.rectangle==bool(scenario&32)&&host.origin.x==30&&host.origin.y==40,"Damage geometry and origin differ");
    check((scenario&32)?host.size.x==8&&host.size.y==6&&host.radius==0:host.radius==4,"Enemy rectangle/circle query parameters differ");
    check(state.auxiliary_18c.words[0]==10000u-(scenario&8?0:amount)&&state.auxiliary_18c.words[5]==static_cast<unsigned>(amount),"Protected versus applied health accounting differs");
    check(owner.data.field_9c==static_cast<unsigned>(scenario&1?before_reduction:0)&&owner.data.field_a0==static_cast<unsigned>(scenario&1?0:before_reduction),"Enemy damage credit must precede secondary-state divide");
    check(field<std::uint8_t>(bonus_feedback.data(),0x35)==(scenario&2?1:0),"Bonus feedback observer differs");
    check(state.timer_2a8.current==29&&state.fields_250[3]==4&&(state.fields_2c8[1]&0x20u),"Hit feedback timer and cooldown differ");
    check(animation.base.field_494==0xff0000ffu&&(field<std::uint8_t>(&animation,0x4a1)&0x1cu)==4,"Hit color or ANM blend bits differ");
    check(host.sounds.size()==1&&host.sounds[0].first==0x22&&host.sounds[0].second==30,"Hit sound ID/pan origin differ");
    // Preview hits retire before phase/script/actual damage processing.
    state.fields_2c8[0]=0x800u;host.hit_flag=1;host.defeat_result=-7;host.events.clear();
    check(gp::update_enemy_damage(state,host)==-7&&host.events==std::vector<int>({11,20}),"Preview defeat must short-circuit remaining update");
    // Reset ordinary query inputs and trigger the first life phase.
    state.fields_2c8[0]=2;state.fields_2c8[1]=0;field(secondary.data(),0x78,std::uint32_t{0});field(primary.data(),0x10,1);state.auxiliary_18c.words[0]=120;
    state.auxiliary.resize(1);state.auxiliary[0].values.fill(0);state.auxiliary[0].values[0]=50;std::memcpy(state.auxiliary[0].values.data()+2,"phase",6);
    host.script_result=-9;host.events.clear();host.hit_flag=0;state.fields_250[3]=0;
    check(gp::update_enemy_damage(state,host)==-9&&host.events==std::vector<int>({10,30,31,32}),"Life phase must reset/select/run ECL before end-of-frame effects");
    check(state.auxiliary_18c.words[0]==50&&state.auxiliary[0].values[0]==0xffffffffu&&state.timer_a8.current==0,"Life phase transition state differs");
    // Timeout transition executes before ordinary damage, including its shared
    // controller/secondary-state mutations and required timeout notification.
    state.auxiliary[0].values[0]=40;state.auxiliary[0].values[1]=30;th20::recovered::timer_set(state.timer_a8,30);state.fields_250[13]=19;
    field(secondary.data(),0x78,std::uint32_t{0});field(&owner,0x50,std::uint32_t{17});host.timeout=[](void*){};host.events.clear();
    check(gp::update_enemy_damage(state,host)==-9&&host.events==std::vector<int>({40,30,31,32}),"Timeout must precede damage and dispatch its script");
    check((state.fields_2c8[1]&0x100u)&&field<unsigned>(secondary.data(),0x78)==0x80&&field<unsigned>(&owner,0x50)==0&&state.fields_250[13]==0,"Timeout shared-owner writes differ");
    // Collision calls consume the genuine enemy/player coordinates and keep
    // the original width/height order for the rectangle routine.
    state.auxiliary.clear();state.fields_2c8[0]=0x200u|(scenario&32?0x1000u:0);state.fields_2c8[1]=0;state.bounds_5c.x=0;state.auxiliary_18c.words[6]=0;
    th20::recovered::timer_set(state.timer_a8,12);state.fields_250[3]=2;host.events.clear();host.collision_result=2;host.defeat_result=0;
    check(gp::update_enemy_damage(state,host)==0&&host.events==std::vector<int>({scenario&32?61:60,62}),"Collision must precede periodic graze call");
    check(host.origin.x==3&&host.origin.y==4&&host.origin.z==5,"Graze position must come from PlayerEntity+614");
    check((scenario&32)?host.size.x==12&&host.size.y==10:host.radius==5,"Collision rectangle/circle dimensions differ");
    // Bomb-state ANM selection: both enter and leave restore exact source IDs.
    state.fields_2c8[0]=0x20u;state.fields_2c8[1]=0x1000u;field(&state,0x54,17);field(&state,0x58,23);field(bomb.data(),0x18,1);host.events.clear();
    check(gp::update_enemy_damage(state,host)==0&&host.events==std::vector<int>({1017})&&(state.fields_2c8[0]&1u)&&(state.fields_2c8[1]&0x2000u)&&field<int>(&state,0x28)==17,"Bomb enter animation/flags differ");
    field(bomb.data(),0x18,0);host.events.clear();
    check(gp::update_enemy_damage(state,host)==0&&host.events==std::vector<int>({1023})&&!(state.fields_2c8[0]&1u)&&!(state.fields_2c8[1]&0x2000u)&&field<int>(&state,0x28)==23,"Bomb leave animation/flags differ");
}
