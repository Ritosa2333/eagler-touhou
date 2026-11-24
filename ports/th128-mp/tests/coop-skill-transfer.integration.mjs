// Run with original world-input.json and an isolated native fixture core: append CoopStockFixture.inc to its scratch WorldExports.cpp, then link the _th128_coop_test_* exports.
// The release core must not expose the fixture exports.
import assert from 'node:assert/strict';
import {test} from 'node:test';
import {balanceFixture,row} from './coop-balance-fixture.mjs';
import {createCoopLockstep} from '../runtime/coop-lockstep.mjs';
import {makeCoopRules} from '../runtime/coop-rules.mjs';
import {prepareA13Preview} from '../runtime/preview-world.mjs';
const B=2,stock=10000,cap=20000;
const {build,options}=await balanceFixture();let module;
const actual=await prepareA13Preview({...options,async factory(spec){return module=await options.factory(spec);}});
async function start(players,overrides={}){
 const g=await actual.openCoopStage({players,stageId:3,difficulty:1,mode:'same-stage-coop',seed:34772,rollbackWindow:2,challengeMode:false,...build,...overrides});
 assert.equal(typeof module._th128_coop_test_stock,'function','provide the private native fixture, not a release core');
 const fixture=module;
 let f=0;const tick=(...keys)=>{g.step(row(f++,Array.from({length:players},(_,i)=>keys[i]??0)));return g.state();};
 return {g,tick,frame:()=>f,rewind:()=>f--,prime:(seat,raw)=>fixture._th128_coop_test_stock(seat,raw),
   overlap:seat=>fixture._th128_coop_test_overlap(seat),stockRules:()=>fixture._th128_coop_test_stock_rules()};
}
const castKeys=(players,caster)=>Array.from({length:players},(_,seat)=>seat===caster?B:0);
const charges=state=>state.seats.map(seat=>seat.charge);
const activeSeats=state=>state.seats.filter(seat=>seat.perfectActive).map(seat=>seat.seat);
function waitForVulnerableSeats(g,tick){
 let state=g.state();
 for(let i=0;i<330&&state.seats.some(seat=>seat.protection>0);i++)state=tick();
 assert.ok(state.seats.every(seat=>seat.state===1&&seat.protection===0),'opening protection has expired');
}

test('the native multiplayer cap, freeze award, death award and HUD stop at 200%',async()=>{
 const {g,prime,stockRules}=await start(2);
 try {
   for(const seat of [0,1]) {
     assert.equal(prime(seat,cap+1),0,'stock just above 200% is rejected');
     assert.equal(prime(seat,cap),1);
   }
   assert.equal(stockRules(),1);
 } finally {g.close();}
});

test('a Perfect Freeze with no ice shows its zero result after the shatter phase',async()=>{
 const {g,tick,frame,prime}=await start(2);
 try{
   assert.equal(prime(0,stock),1);assert.equal(prime(1,stock),1);
   let state=tick(B);
   while(frame()<150)state=tick();
   assert.equal(state.seats[0].freezeCount,1);
   assert.equal(state.seats[0].lastPixels,0);
   assert.equal(g.stageState().freezeRewardPanel.active,false,'the skill is still in progress');
   while(g.stageState().freezeRewardPanel.age<30&&frame()<200)tick();
   const panel=g.stageState().freezeRewardPanel;
   assert.equal(panel.age,30);
   assert.equal(panel.percent,0);
   assert.equal(panel.bonus,0);
   assert.deepEqual(activeSeats(tick(0,B)),[1]);
 }finally{g.close();}
});

for(const n of [2,3]){
 for(let caster=0;caster<n;caster++){
   test(`${n}P seat ${caster+1} protects the team and only a fresh press can spend the next skill`,async()=>{
     const {g,tick,prime,overlap}=await start(n);try{
       for(let seat=0;seat<n;seat++)assert.equal(prime(seat,cap),1);
       waitForVulnerableSeats(g,tick);
       const other=caster===0?n-1:0,blocked=castKeys(n,other);
       const after=tick(...castKeys(n,caster)),expected=Array(n).fill(cap);expected[caster]-=stock;
       assert.deepEqual(activeSeats(after),[caster]);
       assert.deepEqual(charges(after),expected);
       assert.ok(after.seats.every(seat=>seat.protection>0),'protection starts on the casting frame');

       let state=tick(...blocked);
       assert.deepEqual(activeSeats(state),[caster],'only the current skill may remain active');
       assert.deepEqual(charges(state),expected,'blocked teammate does not spend stock');
       assert.ok(state.seats.every(seat=>seat.protection>0),'every living seat is invulnerable');
       for(let seat=0;seat<n;seat++){
         assert.equal(overlap(seat),1);
         assert.equal(g.state().seats[seat].state,1,'an actual overlap leaves the protected seat alive');
       }
       for(let i=0;i<200;i++)state=tick(...blocked);
       assert.deepEqual(activeSeats(state),[],'holding a blocked press never queues another cast');
       assert.deepEqual(charges(state),expected);
       assert.ok(state.seats.every(seat=>seat.protection===0),'the remaining team protection expires normally');
       tick();
       state=tick(...blocked);
       expected[other]-=stock;
       assert.deepEqual(activeSeats(state),[other]);
       assert.deepEqual(charges(state),expected);
     }finally{g.close();}
   });
 }
 for(const overlapping of [false,true])test(`${n}P unlocks X when the Perfect Freeze result finishes counting${overlapping?' even with an overlapping Z result':''}`,async()=>{
   const {g,tick,frame,prime}=await start(n),caster=n===2?0:2,other=n===2?1:0;
   const castFrame=215,ordinaryRelease=304,retry=castKeys(n,other);
   try{
     for(let seat=0;seat<n;seat++)assert.equal(prime(seat,cap),1);
     while(frame()<castFrame)tick();
     assert.deepEqual(activeSeats(tick(...castKeys(n,caster))),[caster]);
     let result,stage=g.stageState();
     while(frame()<castFrame+200){
       const holdingZ=overlapping&&frame()>=ordinaryRelease-46&&frame()<ordinaryRelease;
       const state=tick(...Array.from({length:n},(_,seat)=>seat===other&&holdingZ?1:0));
       const before=stage;stage=g.stageState();
       if(!state.seats[caster].freezeCount)continue;
       if(!result){
         result=stage.freezeRewardPanel;
         assert.ok(result.percent>5&&result.bonus>0,'use a real nonempty Perfect Freeze');
         assert.ok(stage.scoreRaw-before.scoreRaw>=Math.floor(result.bonus/10),'the Perfect bonus was credited');
       }
       assert.equal(stage.freezeRewardPanel.serial,result.serial,'ordinary Z cannot replace a counting Perfect result');
       if(stage.freezeRewardPanel.age===28)break;
     }
     assert.ok(result);
     assert.equal(stage.freezeRewardPanel.age,28);
     assert.equal(g.state().seats[other].freezeCount,Number(overlapping));
     const beforeCharge=g.state().seats[other].charge;
     const blocked=tick(...retry);
     assert.deepEqual(activeSeats(blocked),[caster],'X is blocked during the displayed tally');
     assert.equal(blocked.seats[other].charge,beforeCharge);
     tick();
     assert.equal(g.stageState().freezeRewardPanel.age,30);
     assert.ok(g.draws().some(draw=>draw.drawPass==='original-freeze-reward-panel'&&draw.sprite>=243&&draw.sprite<=252),
       'the original score bonus is drawn at the end of the tally');

     const next=tick(...retry);
     assert.deepEqual(activeSeats(next),[other],'a fresh X works on the next frame');
     assert.equal(next.seats[other].charge,beforeCharge-stock);
     if(overlapping){
       assert.equal(g.stageState().freezeRewardPanel.serial,result.serial,'the final Perfect numbers stay visible');
       while(g.stageState().freezeRewardPanel.serial===result.serial&&frame()<next.frame+100)tick();
       const deferred=g.stageState().freezeRewardPanel;
       assert.equal(deferred.serial,result.serial+1,'the deferred Z reward gets its own display');
       assert.ok(Math.abs(deferred.percent-next.seats[other].lastPixels/172032*100)<1e-5);
     }
   }finally{g.close();}
 });
 test(`${n}P rolls back a predicted guest cast to one confirmed cast and one sound`,async()=>{
   const {g,prime}=await start(n),localSeat=n-1,packets=[],confirmed=[];
   let peer;
   try{
     for(let seat=0;seat<n;seat++)assert.equal(prime(seat,cap),1);
     const rules=makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:'team-perfect',
       epoch:1,players:n,route:'A',difficulty:1,challengeMode:false,seed:34772,
       inputDelay:0,rollbackWindow:2,...build});
     peer=await createCoopLockstep({rules,localSeat,engine:g,send:packet=>packets.push(packet),
       onConfirmed:(row,effects)=>confirmed.push({row,effects})});
     peer.announce();
     for(let seat=0;seat<localSeat;seat++)peer.receive({...packets[0],seat},seat);
     peer.sample(B);
     assert.ok(peer.advance());
     assert.deepEqual(activeSeats(g.state()),[localSeat]);
     assert.equal(confirmed.length,0);
     for(let seat=0;seat<localSeat;seat++)peer.receive({...packets[1],seat,keys:seat===0?B:0},seat);
     peer.flush();
     assert.equal(peer.status().replayed,1);
     assert.deepEqual(activeSeats(g.state()),[0],'simultaneous confirmed presses resolve in seat order');
     assert.deepEqual(charges(g.state()),[stock,...Array(n-1).fill(cap)]);
     assert.equal(confirmed.length,1);
     assert.equal(confirmed[0].effects.sounds.filter(sound=>sound.id===46).length,1);
   }finally{if(peer)peer.close();else g.close();}
 });
 for(const receiverStock of [stock,stock+1000])test(`${n}P gifts a full stock only when the teammate can hold it (${receiverStock/100}%)`,async()=>{
   const {g,tick,rewind,frame,prime}=await start(n);try{
     const from=n-1,to=n-2,keys=Array(n).fill(0),tap=k=>tick(...keys.map((_,i)=>i===from?k:0));
     assert.equal(prime(from,cap),1);assert.equal(prime(to,receiverStock),1);
     for(let i=0;i<4;i++)tick(...keys.map((_,j)=>j===from||n===3&&j===0?64:0));
     const prep=g.state().seats;assert.ok(Math.hypot(prep[to].rawX-prep[from].rawX,prep[to].rawY-prep[from].rawY)<=20*128);
     if(n===3)assert.ok(Math.abs(prep[0].rawX-prep[from].rawX)>20*128);
     // Firing C during the chain breaks it, followed by distinct short taps five times.
     tap(1);tap(0);tap(1);tap(0);tap(513);tap(0);assert.equal(g.state().seats[from].skillTaps,0);
     for(let j=1;j<=4;j++){
       const state=tap(1);tap(0);
       if(receiverStock===stock&&j===3){assert.equal(state.seats[from].skillTaps,3);
         assert.ok(g.draws().some(r=>r.id===0x39000000+from*16&&r.sprite===51),'original-font S progress visible');}
     }
     const cp=g.checkpoint(),control=tap(0);g.restore(cp);rewind();const after=tap(1);
     const moved=receiverStock===stock?stock:0;
     assert.equal(after.seats[from].charge,control.seats[from].charge-moved);
     assert.equal(after.seats[to].charge,control.seats[to].charge+moved);
     assert.equal(after.seats[from].skillTransfers,moved?1:0);
     assert.deepEqual(after.seats.map(s=>s.icePower),control.seats.map(s=>s.icePower),'Z energy is not transferred');
     if(moved)assert.ok(g.draws().some(r=>r.id===0x38000001&&r.sprite===51),'skill courier visible');
     g.restore(cp);rewind();assert.deepEqual(tap(1),after);g.release(cp);
     for(let i=0;i<24;i++)tap(1);assert.equal(g.state().seats[from].skillTransfers,moved?1:0,'held Z is not another gift');
     console.log({seats:n,frame:frame(),giftRaw:moved,receiver:to+1,stock:after.seats.map(s=>s.charge),rollback:true});
   }finally{g.close();}
 });
}

test('the first eligible guest can use a deathbomb when P1 has no stock',async()=>{
 const {g,tick,prime,overlap}=await start(3);
 try{
   assert.equal(prime(1,stock),1);assert.equal(prime(2,stock),1);
   waitForVulnerableSeats(g,tick);
   assert.equal(overlap(1),1);assert.equal(g.state().seats[1].state,4);
   const state=tick(B,B,B);
   assert.deepEqual(activeSeats(state),[1]);
   assert.deepEqual(charges(state),[0,0,stock]);
   assert.ok(state.seats.every(seat=>seat.state===1&&seat.protection>0));
 }finally{g.close();}
});

test('a P1 deathbomb reserves the same team skill window',async()=>{
 const {g,tick,prime,overlap}=await start(2);
 try{
   assert.equal(prime(0,stock),1);assert.equal(prime(1,stock),1);
   waitForVulnerableSeats(g,tick);
   assert.equal(overlap(0),1);assert.equal(g.state().seats[0].state,4);
   const state=tick(B,B);
   assert.deepEqual(activeSeats(state),[0]);
   assert.deepEqual(charges(state),[0,stock]);
   assert.ok(state.seats.every(seat=>seat.state===1&&seat.protection>0));
 }finally{g.close();}
});
