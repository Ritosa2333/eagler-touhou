void item_rules(RetailAssets& files){
 for(unsigned count:{2u,3u}){
  World w(files,count,0,{0,1,3});auto& b=*w.battle;
  auto place=[](MultiplayerPilot& p,float x,float y){p.player->motion.position={x,y,0};p.player->motion.position_fixed={i32(x*128),i32(y*128)};};
  place(*b.pilots[0],-5,400);place(*b.pilots[1],5,400);if(count==3)place(*b.pilots[2],150,400);
  b.pilots[1]->session.power=100;require(b.pilots[1]->options_changed(),"Power options failed");w.step({});
  require(b.enemy_world.state==100,"ECL full-Power condition must consider the weakest active pilot");
  auto id=b.items->spawn(1,{0,400,0},0,0);require(b.items->find(id)->state.kind==1,"One full pilot converted a teammate's P item");w.step({});
  require(b.session.power==400&&b.pilots[1]->session.power==101,"P pickup must prefer an eligible underpowered pilot");
  id=b.items->spawn(3,{5,400,0},0,0);w.step({});require(b.pilots[1]->session.power==201&&b.session.power==400,"Large P affected teammate Power");
  id=b.items->spawn(8,{5,400,0},0,0);require(b.items->find(id)->state.kind==8,"Full Power was converted into a point item");w.step({});
  require(b.pilots[1]->session.power==400&&b.mp_all_power_full(),"Full Power must fill only its recipient");
  const auto old_score=b.score.score,old_points=b.score.point_items;
  id=b.items->spawn(1,{5,400,0},0,0);require(b.items->find(id)->state.kind==2,"All-full P spawn must become a point item");b.items->find(id)->state.state=3;w.step({});
  require(b.score.score==old_score+(b.score.point_value/100-b.score.point_value/100%10)/10&&b.score.point_items==old_points+1,"Converted P must use the shared point value exactly once");
  const auto old_value=b.score.point_value;id=b.items->spawn(8,{5,400,0},0,0);w.step({});require(b.score.point_value==old_value+10000&&b.session.power==400,"Full Power bonus must raise shared point value once");
  b.pilots[1]->session.power=399;require(b.pilots[1]->options_changed(),"Power options failed");
  const auto remaining=b.items->spawn(3,{150,250,0},0,0);id=b.items->spawn(1,{5,400,0},0,0);w.step({});
  require(b.pilots[1]->session.power==400&&b.items->find(remaining)->state.state&&b.items->find(remaining)->state.kind==2&&b.items->find(remaining)->body->source_script==113,"Same-tick last Power must convert remaining field P and its appearance");
  b.pilots[1]->session.power=100;b.pilots[1]->player->life.state=2;require(!b.mp_all_power_full(),"Temporary respawn must not trigger premature all-full conversion");b.pilots[1]->player->life.state=1;
  ItemState green;green.state=3;green.kind=10;green.position=b.player->motion.position;
  require(b.mp_item_owner(green)==0,"Cancel point-value item was incorrectly treated as Power");
  const auto before_green=b.score.point_value;require(b.collection->award(green)&&b.score.point_value==before_green+200&&b.pilots[1]->session.power==100,"Cancel point-value item must change shared value, not personal Power");
  b.score.point_value=b.score.max_point_value-1;green.kind=8;require(b.collection->award(green)&&b.score.point_value==b.score.max_point_value,"Shared point-value cap failed");
  place(*b.pilots[0],-100,400);place(*b.pilots[1],100,100);if(count==3)b.pilots[2]->ghost=true;w.step({});
  ItemState remote;remote.kind=1;remote.state=1;remote.position={-60,300,0};
  require(b.mp_nearest(remote.position)==0&&b.mp_item_owner(remote)==1,"Nearby ineligible pilot blocked a farther auto-collector");
  b.pilots[1]->ghost=true;require(b.mp_all_power_full(),"Ghost Power must not block full-team conversion");
  remote.state=3;require(b.mp_item_owner(remote)==0,"Homing item did not retarget after its recipient became a ghost");
  b.pilots[0]->ghost=true;require(!b.mp_all_power_full()&&b.mp_item_owner(remote)==-1,"All-ghost world must have no item owner");
 }
}
