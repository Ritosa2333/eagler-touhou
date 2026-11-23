// Run with original world-input.json and an isolated native fixture core: append CoopStockFixture.inc to its scratch WorldExports.cpp, then link the two _th128_coop_test_stock* exports.
// The release core must not expose either fixture export. All actions below are real input rows.
import assert from 'node:assert/strict';
import {balanceFixture,row} from './coop-balance-fixture.mjs';
import {prepareA13Preview} from '../runtime/preview-world.mjs';
const {build,options}=await balanceFixture();let module;
const actual=await prepareA13Preview({...options,async factory(spec){return module=await options.factory(spec);}});
async function start(players,overrides={}){
 const g=await actual.openCoopStage({players,stageId:3,difficulty:1,mode:'same-stage-coop',seed:34772,rollbackWindow:2,challengeMode:false,...build,...overrides});
 assert.equal(typeof module._th128_coop_test_stock,'function','provide the private native fixture, not a release core');
 assert.equal(module._th128_coop_test_stock_rules(),1,'cap, spend, death gain, and original percentage glyphs support 500%');
 let f=0;const tick=(...keys)=>{g.step(row(f++,Array.from({length:players},(_,i)=>keys[i]??0)));return g.state();};
 return {g,tick,frame:()=>f,rewind:()=>f--,prime:(seat,raw)=>module._th128_coop_test_stock(seat,raw)};
}
for(const n of [2,3]){
 for(const caster of [0,n-1]){
   const {g,tick,prime}=await start(n);try{
     assert.equal(prime(caster,50001),0);assert.equal(prime(caster,50000),1);
     const st=tick(...Array.from({length:n},(_,i)=>i===caster?2:0));
     assert.equal(st.seats[caster].charge,40000);assert.equal(st.seats[caster].perfectActive,true);
     assert.ok(st.seats.every(s=>s.seat===caster||s.charge===0),'X spends only the caster’s stock');
   }finally{g.close();}
 }
 for(const receiverStock of [40000,41000]){
   const {g,tick,rewind,frame,prime}=await start(n);try{
     const from=n-1,to=n-2,keys=Array(n).fill(0),tap=k=>tick(...keys.map((_,i)=>i===from?k:0));
     assert.equal(prime(from,50000),1);assert.equal(prime(to,receiverStock),1);
     for(let i=0;i<4;i++)tick(...keys.map((_,j)=>j===from||n===3&&j===0?64:0));
     const prep=g.state().seats;assert.ok(Math.hypot(prep[to].rawX-prep[from].rawX,prep[to].rawY-prep[from].rawY)<=20*128);
     if(n===3)assert.ok(Math.abs(prep[0].rawX-prep[from].rawX)>20*128);
     // Firing C during the chain breaks it, followed by distinct short taps five times.
     tap(1);tap(0);tap(1);tap(0);tap(513);tap(0);assert.equal(g.state().seats[from].skillTaps,0);
     for(let j=1;j<=4;j++){
       const state=tap(1);tap(0);
       if(receiverStock===40000&&j===3){assert.equal(state.seats[from].skillTaps,3);
         assert.ok(g.draws().some(r=>r.id===0x39000000+from*16&&r.sprite===51),'original-font S progress visible');}
     }
     const cp=g.checkpoint(),control=tap(0);g.restore(cp);rewind();const after=tap(1);
     const moved=receiverStock===40000?10000:0;
     assert.equal(after.seats[from].charge,control.seats[from].charge-moved);
     assert.equal(after.seats[to].charge,control.seats[to].charge+moved);
     assert.equal(after.seats[from].skillTransfers,moved?1:0);
     assert.deepEqual(after.seats.map(s=>s.icePower),control.seats.map(s=>s.icePower),'Z energy is not transferred');
     if(moved)assert.ok(g.draws().some(r=>r.id===0x38000001&&r.sprite===51),'skill courier visible');
     g.restore(cp);rewind();assert.deepEqual(tap(1),after);g.release(cp);
     for(let i=0;i<24;i++)tap(1);assert.equal(g.state().seats[from].skillTransfers,moved?1:0,'held Z is not another gift');
     console.log({seats:n,frame:frame(),giftRaw:moved,receiver:to+1,stock:after.seats.map(s=>s.charge),rollback:true});
   }finally{g.close();}
 }
}
