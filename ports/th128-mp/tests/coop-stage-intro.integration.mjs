// Real authored stage logo/prompt/caption animation with an untouched difficulty strip.
import assert from 'node:assert/strict';
import {balanceFixture,row} from './coop-balance-fixture.mjs';
import {prepareCoopStage} from '../runtime/preview-world.mjs';
import {makeCoopRules} from '../runtime/coop-rules.mjs';
const {options,build}=await balanceFixture();
const logo=r=>r.bank===30&&r.id>=50000000&&r.id<51000000;
for(const [players,route,difficulty] of [[2,'A',1],[3,'B',2],[2,'C',3],[3,'EX',4]]){
 const rules=makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:'original-intro',epoch:1,players,route,difficulty,challengeMode:players===3,seed:34772,inputDelay:0,rollbackWindow:2,...build});
 const prepared=await prepareCoopStage({...options,rules}),g=await prepared.openCoopStage(rules);
 let frame=0;
 try{
   const ids=route==='EX'?[rules.stageId]:Array.from({length:5},(_,i)=>rules.stageId+i);
   for(const id of ids)assert.match(prepared.banksFor(id).find(([bank])=>bank===30)?.[1]??'',/^logo_/,'each route stage requires its original logo texture bank');
   const rank=()=>g.draws().find(r=>r.bank===5&&r.script===66+difficulty);
   for(;frame<20;frame++)g.step(row(frame,Array(players).fill(0)));
   assert.ok(rank()?.y<42,'difficulty appears above the team HUD from the opening');
   const during=new Set(),checkpoint=g.checkpoint();
   const through=()=>{
     for(let end=frame+200;frame<end;frame++){
       g.step(row(frame,Array(players).fill(0)));
       if(frame%6===0)for(const r of g.draws().filter(logo))during.add(r.script);
     }
     return g.draws().filter(logo);
   };
   const first=through();assert.ok(during.has(1),'authored title/intro sprite is animated');
   if(route!=='EX')assert.ok(during.has(2),'the original stage-one hint sprite is animated');
   assert.ok(during.has(3),'stage track caption is bound on the same deterministic timeline');
   g.restore(checkpoint);frame=20;assert.deepEqual(through(),first,'original logo state follows rollback');g.release(checkpoint);
   for(;frame<550;frame++)g.step(row(frame,Array(players).fill(0)));
   assert.equal(g.draws().filter(logo).length,0,'logo and track naturally leave the stage after their animation');
   console.log({players,route,difficulty,logoScripts:[...during],rankVisible:!!rank()});
 }finally{g.close();}
}
