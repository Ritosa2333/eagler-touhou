import assert from 'node:assert/strict';import test from 'node:test';
import {createCoopMatch} from '../runtime/coop-match.mjs';import {makeCoopRules} from '../runtime/coop-rules.mjs';import {readMultiplayerReplay} from '../runtime/confirmed-history.mjs';
for(const n of [2,3])test(`${n} participant common restart seals the same confirmed tail before next independent World`,async()=>{
 const build={coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64)},rules=makeCoopRules({...build,schema:'th128-coop/1',mode:'same-stage-coop',matchId:'restart-local',epoch:1,players:n,route:'EX',difficulty:4,challengeMode:true,seed:92,inputDelay:0,rollbackWindow:2});
 const peers=[],q=[],closed=[],openings=[];function factory(i,r){const logs=[],t=new Map();openings.push([i,r.epoch]);return {checkpoint(){const token={};t.set(token,logs.slice());return token;},restore(x){assert.ok(t.has(x));logs.splice(0,logs.length,...t.get(x));},release(x){assert.equal(t.delete(x),true);},step(row){assert.equal(row.frame,logs.length);logs.push(row);},close(){assert.equal(t.size,0);closed.push([i,r.epoch]);}};}
 for(let i=0;i<n;i++)peers.push(await createCoopMatch({rules,localPlayer:i,allowedMask:763,openEngine:r=>factory(i,r),send:m=>q.push([i,m])}));
 async function drain(){for(let j=0;j<32;j++){while(q.length){const [i,m]=q.shift();for(let s=0;s<n;s++)if(s!==i)await peers[s].receive(m,i);}await Promise.resolve();peers.forEach(p=>p.flush());if(!q.length)await new Promise(r=>setImmediate(r));if(!q.length)break;}}
 try{peers.forEach(p=>p.announceStart());await drain();assert.ok(peers.every(p=>p.status().startup.canStart));
 for(let f=0;f<3;f++){for(let i=0;i<n;i++)peers[i].tick(()=>64<<Math.min(i,1));await drain();}
 peers[0].tick(()=>1);peers[n-1].requestRestart();await drain();for(const deadline=performance.now()+3000;performance.now()<deadline&&peers.some(p=>p.status().epoch===1);){await new Promise(resolve=>setTimeout(resolve,1));for(const p of peers)if(p.status().epoch===1)p.tick(()=>0);await drain();}
 assert.deepEqual(openings.length,n*2);const bytes=peers[0].previousReplay(),archive=await readMultiplayerReplay(bytes,build);assert.deepEqual(archive.status(),{nextFrame:4,end:'restart'});
 for(const p of peers){assert.equal(p.status().epoch,2);assert.equal(p.status().history.nextFrame,0);assert.deepEqual(p.previousReplay(),bytes);assert.equal(p.spectatorSource().header.rules.players,n);p.tick(()=>2);}await drain();
 assert.equal(closed.length,n);assert.ok(peers.every(p=>p.status().history.nextFrame===1));
 }finally{for(const p of peers)p.close();}assert.equal(closed.length,2*n);
});

for(const n of [2,3])test(`P${n} pauses the shared World at one confirmed boundary; other seat resumes, no idle replay rows`,async()=>{
 const rules=makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:'pause',epoch:1,players:n,route:'A',difficulty:1,seed:55,challengeMode:false,inputDelay:0,rollbackWindow:2,coreSHA256:'a'.repeat(64),resourceSHA256:'d'.repeat(64)});
 const peers=[],q=[],frames=Array(n).fill(0);function world(i,r){frames[i]=0;return {checkpoint:()=>({frame:frames[i]}),restore:t=>{frames[i]=t.frame;},release(){},step(row){assert.equal(row.frame,frames[i]++);},close(){}};}
 const fan=async()=>{for(let cycle=0;cycle<30;cycle++){while(q.length){const [i,m]=q.shift();for(let j=n-1;j>=0;j--)if(i!==j)await peers[j].receive(m,i);}peers.forEach(p=>p.flush());if(!q.length){await new Promise(r=>setImmediate(r));if(!q.length)break;}}};
 try{for(let i=0;i<n;i++)peers.push(await createCoopMatch({rules,localPlayer:i,allowedMask:763,openEngine:r=>world(i,r),send:m=>q.push([i,m])}));peers.forEach(p=>p.announceStart());await fan();
 peers.forEach(p=>p.tick(()=>0));await fan();peers[0].tick(()=>64);peers[n-1].togglePause();await fan();
 for(let j=0;j<6;j++){peers.forEach(p=>p.tick(()=>0));await fan();}
 assert.ok(peers.every(p=>p.status().phase==='paused'));assert.deepEqual(frames,Array(n).fill(2));assert.ok(peers.every(p=>p.spectatorSource().status().nextFrame===2));
 for(let i=0;i<20;i++)for(const p of peers)assert.equal(p.tick(()=>{throw Error('paused input sampled');}).advanced,false);
 peers[0].togglePause();await fan();assert.ok(peers.every(p=>p.status().phase==='running'));peers.forEach(p=>p.tick(()=>16));await fan();assert.deepEqual(frames,Array(n).fill(3));
 peers[0].togglePause();peers[n-1].togglePause();await fan();assert.ok(peers.every(p=>p.status().phase==='paused'));peers[n-1].requestRestart();await fan();
 for(const deadline=performance.now()+3000;performance.now()<deadline&&peers.some(p=>p.status().epoch===1);){await new Promise(resolve=>setTimeout(resolve,1));await fan();}assert.ok(peers.every(p=>p.status().epoch===2&&p.status().phase==='running'));assert.deepEqual(frames,Array(n).fill(0));
 }finally{peers.forEach(p=>p.close());}
});
test('P3 can end a paused match without creating a new World; all results and saved tails agree before independent return',async()=>{
 const n=3,build={coreSHA256:'2'.repeat(64),resourceSHA256:'3'.repeat(64)},rules=makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:'exit',epoch:1,players:n,route:'A',difficulty:2,seed:2,challengeMode:false,inputDelay:0,rollbackWindow:2,...build}),peers=[],q=[];let opens=0;
 const fan=async()=>{for(let repeat=0;repeat<24;repeat++){while(q.length){const [from,msg]=q.shift();for(let i=0;i<n;i++)if(i!==from)await peers[i].receive(msg,from);}for(const peer of peers)peer.flush();if(!q.length)return;}throw Error('unsettled');};
 try{for(let i=0;i<n;i++)peers.push(await createCoopMatch({rules,allowedMask:763,localPlayer:i,send:msg=>q.push([i,msg]),openEngine:()=>{opens++;return {step(){},checkpoint(){return{}},restore(){},release(){},close(){}};}}));peers.forEach(p=>p.announceStart());await fan();peers.forEach(p=>p.tick(()=>64));await fan();assert.equal(peers[2].requestFinish(),false,'no accidental live quit');
 peers[2].togglePause();await fan();assert.equal(peers[2].requestFinish(),true);await fan();const saved=peers[0].previousReplay();for(const p of peers){assert.equal(p.status().result,'left');assert.deepEqual(p.status().history,{nextFrame:1,end:'left'});assert.deepEqual(p.previousReplay(),saved);assert.equal(p.tick(()=>{throw Error('ended match sampled');}).advanced,false);}
 assert.equal(opens,n);const tape=await readMultiplayerReplay(saved,build);assert.equal(tape.status().end,'left');peers[0].close();assert.equal(peers[2].status().result,'left');}finally{peers.forEach(p=>p.close());}
});