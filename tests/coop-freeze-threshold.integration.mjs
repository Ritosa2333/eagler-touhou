// Original A1-3 data: real shot/ice damage, both sides of the raw 5% threshold,
// and freezes that hit no enemies. Area, X stock, stars and shot gain stay independent.
import assert from 'node:assert/strict';
import {balanceFixture,row} from './coop-balance-fixture.mjs';
import {prepareA13Preview} from '../runtime/preview-world.mjs';
const {options,build}=await balanceFixture();let native;
const preview=await prepareA13Preview({...options,async factory(args){native=await options.factory(args);return native;}});
const stats=()=>JSON.parse(native.UTF8ToString(native._th128_world_state())).damageStats;
const full=172032;
for(const players of [2,3]){
 const rate=players===2?0.8:0.75;
 const cases=[
  {label:'small, no damage',move:32,distance:players===2?2:4,small:true},
  {label:'over 5%, no damage',move:32,distance:0,small:false},
  ...[2.5,2,1,.75,.5].map(multiplier=>({label:'capped damage',move:16,distance:32,multiplier,small:false,damage:true,capped:true})),
  {label:'host ice damage',move:16,distance:32,multiplier:2.5,small:false,damage:true,capped:true,owner:0},
  {label:'uncapped damage',move:16,distance:18,start:145,hold:45,multiplier:.75,small:false,damage:true,capped:false},
  ...(players===3?[{label:'under 5%, with damage',move:16,distance:17,start:130,hold:45,multiplier:2.5,small:true,damage:true,capped:false}]:[]),
 ];
 for(const c of cases){
  const owner=c.owner??players-1,multiplier=c.multiplier??1,start=c.start??190,hold=c.hold??60;
  const g=await preview.openCoopStage({players,stageId:3,difficulty:1,mode:'same-stage-coop',seed:34772,rollbackWindow:2,challengeMode:false,...build,
   ...(c.multiplier===undefined?{}:{freezeMotivationMultiplier:multiplier})});
  try{
   let state=g.state(),previous=stats(),settled=false,baseReward=0;const samples=[];
   for(let f=0;f<490;f++){
    const held=f<c.distance?c.move:f>=start&&f<start+hold?1:0;
    const before=state;g.step(row(f,Array.from({length:players},(_,i)=>i===owner?held:0)));state=g.state();
    const damage=stats(),ice=(damage[0]-previous[0])|0,finish=state.seats[owner].freezeCount>before.seats[owner].freezeCount;
    if(ice>0||finish){
     const original=ice>0?Math.floor(Math.min(ice,1700)/2):0;
     const amount=Math.floor(original*multiplier),earlier=baseReward;baseReward+=amount;
     assert.equal(damage[1],previous[1],`${c.label}: no ordinary shot damage on sample frame ${f}`);
     for(const [seat,s] of state.seats.entries()){
      const p=before.seats[seat];assert.equal(s.deaths,p.deaths,`${c.label}: no death on sample frame ${f}`);
      const net=s.motivation-p.motivation-(s.starCredits-p.starCredits);
      const wanted=seat===owner?amount:c.small?0:finish?earlier+amount:settled?amount:0;
      assert.equal(net,wanted,`${players}P ${c.label} x${multiplier} frame ${f}: P${seat+1} motivation from ${ice} original ice damage`);
     }
     samples.push({frame:f,ice,amount,finish});
    }
    if(!ice&&!finish&&baseReward>0&&damage[1]===previous[1]&&state.seats.every((s,i)=>s.deaths===before.seats[i].deaths&&s.damage===before.seats[i].damage)){
     for(const [seat,s] of state.seats.entries()){
      const p=before.seats[seat];
      assert.equal(s.motivation-p.motivation-(s.starCredits-p.starCredits),0,'A prior ice hit must not pay again on later ticks');
     }
    }
    if(finish){
     assert.equal(settled,false);settled=true;
     const raw=100*state.teamPixels/full;
     assert.equal(raw<5,c.small,'fixture must straddle the actual pre-scale threshold');
     assert.ok(Math.abs(state.firepower-raw*rate)<1e-5,'shared area follows the existing multiplayer freeze rate');
     const original=raw<=10?Math.trunc(raw*50):raw<=30?500+Math.trunc((raw-10)*25):2000;
     for(const s of state.seats)assert.equal(s.charge-s.deaths*2500,s.seat===owner||raw>=5?Math.floor(original*rate):0,'existing X reward');
    }
    previous=damage;
   }
   assert.ok(settled);assert.equal(baseReward>0,!!c.damage);
   if(c.damage)assert.equal(samples.some(s=>s.ice>=1700),c.capped);
   assert.ok(state.seats[owner].starCredits>0);
   for(const s of state.seats)assert.equal(s.starCredits,s.seat===owner||!c.small?state.seats[owner].starCredits:0);
   console.log(JSON.stringify({players,owner:owner+1,case:c.label,multiplier,pixels:state.teamPixels,samples}));
  }finally{g.close();}
 }
 for(let shooter=0;shooter<players;shooter++){
  const g=await preview.openCoopStage({players,stageId:3,difficulty:1,mode:'same-stage-coop',seed:34772,rollbackWindow:2,
   challengeMode:false,freezeMotivationMultiplier:2.5,...build});
  try{
   for(let f=0;f<365;f++)g.step(row(f,Array.from({length:players},(_,i)=>i===shooter?512:0)));
   const {seats}=g.state();assert.ok(seats[shooter].damage>0,'shooter must actually hit enemies');assert.ok(seats.every(s=>s.motivation>=0));
   assert.equal(stats()[0],0,'ordinary fire never generates ice damage');
   const gains=seats.map(s=>s.motivation-20000+s.deaths*10000);
   assert.ok(gains[shooter]>0);
   assert.deepEqual(gains,Array(players).fill(gains[shooter]),'shots give the same unscaled motivation to all seats');
   assert.equal(gains[shooter]%2,0,'each original shot query grants 2 raw motivation');
   console.log(JSON.stringify({players,shooter:shooter+1,hitMotivation:gains}));
  }finally{g.close();}
 }
}
