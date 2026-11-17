// Exercise the production room gate: bound speculative work without losing
// authoritative input, preventing replay, or changing confirmed-only play.
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
const {pathToFileURL}=require('node:url');
(async()=>{
 const root=path.resolve(__dirname,'..');
 const {RollbackQueue}=await import(pathToFileURL(path.join(root,'web/netplay/rollback-queue.js')));
 const source=fs.readFileSync(path.join(root,'web/netplay/room.js'),'utf8');
 const body=source.slice(source.indexOf('function nextInputs('),source.indexOf('function updateMetrics('));
 let waitClosures=0;
 const makeNext=q=>Function('queue','runtime','MAX_ROLLBACK','endInputWait','peers','state',`let waitDetail='';${body};return nextInputs;`)(q,{canPredict:()=>true,predictionBlockReason:()=>''},12,()=>{waitClosures++;},new Map(),{slots:{}});
 for(const players of [2,3])for(const horizon of [3,6]){
  const q=new RollbackQueue(0,players,{directionPrediction:horizon});q.arm(0);
  const next=makeNext(q);
  while(q.frame<2+horizon){q.capture(8);const pair=next();assert(pair);q.commit(pair);}
  q.capture(4);assert.equal(next(),null,'fast peer must wait at useful prediction horizon');
  // Real inputs invalidate an earlier prediction. Replay must reach the old
  // frontier, then pause again instead of creating a full speculative tail.
  for(let slot=1;slot<players;slot++)q.receive(2,8,slot);
  assert.equal(q.dirty,2);const frontier=q.frame;q.rewind(q.dirty);
  while(q.frame<frontier){const pair=next();assert(pair,'replay must progress');q.commit(pair);}
  for(let at=3;at<=frontier;at++)for(let slot=1;slot<players;slot++)q.receive(at,8,slot);
  if(q.dirty!==null){const target=q.frame;q.rewind(q.dirty);while(q.frame<target){const pair=next();assert(pair);q.commit(pair);}}
  assert(next(),'authoritative input must release pacing');
 }
 const q=new RollbackQueue(0,2,{rollbackEnabled:false});
 const next=makeNext(q);
 for(let i=0;i<30;i++){q.capture(32);q.receive(i+2,32,1);const pair=next();assert(pair);q.commit(pair);}
 assert(waitClosures>0,'successful input delivery must close the wait telemetry interval');
 console.log('PASS prediction pacing: 2/3 players, LAN/relay, correction replay, input recovery, rollback off');
})().catch(e=>{console.error(e);process.exitCode=1;});
