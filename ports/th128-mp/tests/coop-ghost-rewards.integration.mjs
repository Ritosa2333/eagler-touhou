// Real A1-3: P1 earns motivation by shooting after P2's last life is lost.
import assert from 'node:assert/strict';
import {balanceFixture,row} from './coop-balance-fixture.mjs';
const {open}=await balanceFixture();
const g=await open(2);let frame=0;
const steer=(x,y,tx,ty)=>(tx>x+2?128:x>tx+2?64:0)|(ty>y+2?32:y>ty+2?16:0);
function tick(a=0,b=0){g.step(row(frame++,[a,b]));return g.state();}
function safe(s){const x=s.rawX/128,y=s.rawY/128;
 const threats=g.stageState().bullets.map(b=>[b.x,b.y]).filter(([u,v])=>Math.abs(u-x)<140&&Math.abs(v-y)<180);
 let best=-Infinity,keys=0;for(const [dx,dy,input] of [[0,0,0],[4.5,0,128],[-4.5,0,64],[0,-4.5,16],[0,4.5,32],[-3.18,-3.18,80],[3.18,-3.18,144],[-3.18,3.18,96],[3.18,3.18,160]]){
  const tx=Math.max(-174,Math.min(174,x+dx*5)),ty=Math.max(310,Math.min(421,y+dy*5));let danger=0;
  for(const [u,v] of threats)danger+=1800/(16+(u-tx)**2+(v+7-ty)**2);
  const value=-danger-.0001*((tx+130)**2+(ty-401)**2);if(value>best){best=value;keys=input;}
 }return keys;
}
try{
 let st=g.state(),entered;
 for(;frame<4500;){st=tick(safe(st.seats[0])|512);
   if(st.seats[1].retired||st.seats[1].motivation<0){entered=st;break;}}
 assert.ok(entered,'P2 must genuinely exhaust its lives');
 assert.ok(entered.seats[1].motivation>=0&&entered.seats[1].motivation<100,'retirement starts at 0% rather than a negative sentinel');
 assert.equal(entered.seats[1].retired,true);
 const {respawns,deaths}=entered.seats[1];let progressed=false;
 for(let i=0;i<1500;i++){
   const before=st;st=tick(safe(st.seats[0])|512);
   const ghost=st.seats[1];assert.equal(ghost.retired,true);assert.equal(ghost.respawns,respawns);assert.equal(ghost.deaths,deaths);
   if(ghost.ghost&&ghost.motivation>entered.seats[1].motivation){progressed=true;break;}
   assert.equal(st.seats[0].retired,false,'the shooter stays alive for the test');
 }
 assert.ok(progressed,'the ghost must still earn motivation from shared enemy hits');
 const shared=st.seats[1].motivation,charge=st.seats[1].charge;
 for(let i=0;i<260;i++){
   const [a,b]=st.seats;const risk=(b.rawX-a.rawX)**2+(b.rawY-a.rawY)**2<=(18*128)**2;
   st=tick(steer(a.rawX/128,a.rawY/128,b.rawX/128,b.rawY/128)|(risk&&i%2===0?8:0));
   if(!st.seats[1].retired){assert.equal(st.seats[1].rescued,1);assert.ok(st.seats[1].motivation>=shared);assert.ok(st.seats[1].charge>=charge);assert.equal(st.seats[1].respawns,respawns);console.log({frame,shared,charge,rescued:st.seats[1].rescued});break;}
 }
 assert.equal(st.seats[1].retired,false,'actual life donation resurrects the ghost');
}finally{g.close();}
