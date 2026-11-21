'use strict';
const assert=require('node:assert/strict');
exports.webAudio=function(events){
 const param=()=>({value:1,cancelScheduledValues(time){events.push(['cancel',time]);},setValueAtTime(value,time){this.value=value;events.push(['gain',value,time]);},exponentialRampToValueAtTime(value,time){events.push(['ramp',value,time]);}});
 const node=()=>({gain:param(),connect(){},disconnect(){}});
 return{state:'running',currentTime:10,sampleRate:44100,createGain:node,createChannelMerger:node,createChannelSplitter:node,
  createBufferSource(){return{connect(){},disconnect(){},start(){events.push('source-start');},stop(time){events.push(['source-stop',time??null]);}};},
  createBuffer(channels,frames,rate){return{numberOfChannels:channels,length:frames,sampleRate:rate,duration:frames/rate,getChannelData:()=>new Float32Array(frames)};}};
};
// Explicit synthetic IDB transaction/readback scheduling. Never actual browser persistence.
exports.ReceiptsDatabase=class ReceiptsDatabase{
 constructor(events){this.events=events;this.name='synthetic-owned-ending-receipts';this.rows=new Map();this.writes=0;this.holdReads=false;this.held=[];this.strict=true;this.mutate=null;this.closed=false;}
 close(){this.closed=true;}
 release(){this.holdReads=false;for(const resume of this.held.splice(0))setImmediate(resume);}
 transaction(name,mode,options){
  if(this.closed)throw new DOMException('The receipt database is closing','InvalidStateError');
  assert.equal(name,'receipts');const database=this,write=mode==='readwrite';if(write)assert.equal(options.durability,'strict');
  const snapshot=new Map([...this.rows].map(([k,v])=>[k,structuredClone(v)]));let pending=0,aborted=false,queued=false;
  const tx={durability:write?(database.strict?'strict':'relaxed'):'default',error:null,
   abort(){aborted=true;this.error=Error('synthetic transaction abort');setImmediate(()=>this.onabort?.());},
   objectStore(store){assert.equal(store,'receipts');return{
    add(value){assert(write&&!aborted);assert(!snapshot.has(value.key));snapshot.set(value.key,structuredClone(value));++database.writes;++pending;
     setImmediate(()=>{--pending;complete();});},
    get(key){const req={result:undefined};++pending;const run=()=>{if(aborted)return;req.result=structuredClone(snapshot.get(key));
     if(database.mutate&&req.result){database.mutate(req.result);database.mutate=null;}database.events.push('strict-readback');req.onsuccess?.();--pending;complete();};
     setImmediate(()=>{if(database.holdReads)database.held.push(run);else run();});return req;}
   };}
  };
  function complete(){if(pending||aborted||queued)return;queued=true;setImmediate(()=>{queued=false;if(pending||aborted)return;if(write)database.rows=snapshot;tx.oncomplete?.();});}
  return tx;
 }
};
exports.until=async function(predicate){for(let i=0;i<500;++i){if(predicate())return;await new Promise(resolve=>setImmediate(resolve));}throw Error('synthetic consumer did not reach its expected scheduling boundary');};
