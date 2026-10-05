import assert from 'node:assert/strict';
import {createAdonisCalibration} from '../browser/adonis-calibration.mjs';
function fixture(count=2){
 const memory={buffer:new ArrayBuffer(256)},s=new Uint32Array(memory.buffer,4,32);
 s.set([1,1,0,0,count,0,2,1,0,0,0,77]);s[31]=16000;
 const target={document:{hidden:false},__eaglerPeerTransport:{route:'rtc'}},events=[],errors=[];
 let polls=0,pauses=0,app=1;
 const core={memory,multiplayer_calibration_status:()=>4,multiplayer_network_poll:()=>{++polls;return 1;}};
 const calibration=createAdonisCalibration({core,getApp:()=>app,getOptions:()=>({netplayAdonisMode:2}),
  emit:(event,fields)=>events.push({event,...fields}),game:'th08',build:'build',target,
  onError:e=>errors.push(e.message),pause:()=>++pauses});
 return {s,target,events,errors,calibration,core,polls:()=>polls,pauses:()=>pauses,close:()=>{app=0;calibration.stop();}};
}
for(const count of [2,3]){
 const f=fixture(count);try{
  f.calibration.start();assert.equal(f.polls(),1);
  f.s[1]=3;f.s[2]=129;f.s[3]=120;f.calibration.pump();
  f.s[1]=5;f.s[8]=1;f.s[9]=3;f.s[10]=2;
  for(let p=0;p<count;++p)f.s.set([60000+p*1000,120,0,10000,100000,45000],12+p*6);
  f.calibration.pump();f.calibration.pump();
  const ready=f.events.filter(e=>e.netplayTiming.phase==='ready');assert.equal(ready.length,1);
  assert.equal(ready[0].netplayTiming.calibration.players.length,count);
  assert.equal(f.target.__eaglerNetplayInputDelayFrames,1);
  assert.equal(ready[0].netplayTiming.rttP95Us,60000+(count-1)*1000);
  assert.equal(ready[0].netplayTiming.route,'rtc');
  assert.equal(f.target.__eaglerPeerTransport.onReceive,null,'Committed measurement releases its receive hook');
  const completedPolls=f.polls();
  f.core.multiplayer_network_poll=()=>{throw Error('Gameplay wire must not be polled by the measurement timer');};
  f.target.__eaglerPeerTransport.error='RTC peer P2 input channel closed';
  f.calibration.pump();
  assert.equal(f.polls(),completedPolls);assert.equal(f.errors.length,0);assert.equal(f.pauses(),0);
  f.core.multiplayer_network_poll=()=>{f.core.restarted=true;return 1;};
  f.calibration.stop();assert.equal(f.target.__eaglerPeerTransport.onReceive,null);
  f.s[11]++;f.s[1]=3;f.calibration.start();assert.ok(f.core.restarted,'Restart restores polling');
 }finally{f.close();}
}
{
 const f=fixture();try{
  f.target.__eaglerPeerTransport.error='RTC peer P3 control channel closed';
  f.core.multiplayer_network_poll=()=>0;
  f.calibration.start();
  assert.deepEqual(f.errors,['RTC peer P3 control channel closed']);
  assert.equal(f.pauses(),1);
 }finally{f.close();}
}
for(const kind of ['hidden','route','native']){
 const f=fixture();try{
  f.s[1]=3;f.calibration.start();
  if(kind==='hidden')f.target.document.hidden=true;
  if(kind==='route')f.target.__eaglerPeerTransport.route='relay';
  if(kind==='native')f.s[1]=6;
  f.calibration.pump();const polls=f.polls();f.calibration.pump();
  assert.equal(f.errors.length,1);assert.equal(f.pauses(),1);assert.equal(f.polls(),polls);
 }finally{f.close();}
}
console.log('Adonis shell progress, immutable result, three seats, interruption and cleanup: PASS');
