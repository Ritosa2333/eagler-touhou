// Touch motion is an owned USER extension. Original RPY input words remain unchanged.
const KIND=0x31484354,SCHEMA='th128-touch-replay/1';
export function validTouchMotion(mode,x,y){
 if(![mode,x,y].every(Number.isSafeInteger))return false;
 if(mode===0)return x===0&&y===0;
 if(mode===1||mode===2)return x>=-23552&&x<=23552&&y>=4096&&y<=55296;
 return mode===3&&x>=-32767&&x<=32767&&y>=-32767&&y<=32767;
}
function plans(value,stages){
 if(value?.schema!==SCHEMA||!Array.isArray(value.stages)||value.stages.length!==stages.length)throw Error('Touch Replay stage metadata differs');
 return Object.freeze(value.stages.map((row,index)=>{
  const stage=stages[index],frames=stage.numFrames??new DataView(stage.rawHeader.buffer,stage.rawHeader.byteOffset,stage.rawHeader.byteLength).getUint32(4,true);
  if(!row||row.stage!==(stage.stage??new DataView(stage.rawHeader.buffer,stage.rawHeader.byteOffset,stage.rawHeader.byteLength).getUint16(0,true))
   ||!Number.isSafeInteger(row.worldFrames)||row.worldFrames<frames||row.worldFrames>1000000||!Array.isArray(row.events)||row.events.length>100000)throw Error('Touch Replay stage/frame extent differs');
  let prior=-1;const events=row.events.map(event=>{
   if(!Array.isArray(event)||event.length!==4||!Number.isSafeInteger(event[0])||event[0]<=prior||event[0]>=row.worldFrames
    ||!validTouchMotion(...event.slice(1)))throw Error('Touch Replay event order/raw command differs');
   prior=event[0];return Object.freeze(event.slice());
  });
  let auxiliary;
  if(Object.hasOwn(row,'auxiliary')){
   if(!Array.isArray(row.auxiliary)||row.auxiliary.length>12000||row.worldFrames!==frames+row.auxiliary.length)throw Error('Auxiliary Replay World/main frame extent differs');
   let prior=-1;auxiliary=Object.freeze(row.auxiliary.map(event=>{
    if(!Array.isArray(event)||event.length!==4||!event.every(Number.isSafeInteger)||event[0]<=prior||event[0]>=row.worldFrames||event.slice(1).some(n=>n<0||n>65535))throw Error('Auxiliary Replay input frame/order/domain differs');
    prior=event[0];return Object.freeze(event.slice());
   }));
  }
  return Object.freeze({stage:row.stage,worldFrames:row.worldFrames,events:Object.freeze(events),...(auxiliary?{auxiliary}:{})});
 }));
}
export function nativeTouchReplayBlock(bytes,stages){
 if(!bytes.length)return null;
 const v=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);let at=0;
 const word=()=>{if(at+4>bytes.length)throw Error('Native touch Replay trailer truncated');const n=v.getUint32(at,true);at+=4;return n;};
 if(word()!==KIND)throw Error('Native touch Replay trailer identity differs');
 const version=word();if(![1,2].includes(version)||word()!==stages.length)throw Error('Native touch Replay version/stage extent differs');
 const rows=stages.map(()=>{const stage=word(),worldFrames=word(),count=word(),auxCount=version===2?word():0;
  if(count>100000||auxCount>12000||at+(count+auxCount)*16>bytes.length)throw Error('Native touch Replay event extent');
  const events=Array.from({length:count},()=>[word(),word(),word()<<0,word()<<0]);
  const auxiliary=version===2?Array.from({length:auxCount},()=>[word(),word(),word(),word()]):null;
  return{stage,worldFrames,events,...(auxiliary?{auxiliary}:{})};});
 if(at!==bytes.length)throw Error('Native touch Replay unexpected tail');
 const owned=plans({schema:SCHEMA,stages:rows},stages),data=new TextEncoder().encode(JSON.stringify({schema:SCHEMA,stages:owned}));
 if(data.length>8*1024*1024)throw Error('Touch Replay USER budget');const size=(12+data.length+1+3)&~3,raw=new Uint8Array(size),out=new DataView(raw.buffer);
 out.setUint32(0,0x52455355,true);out.setUint32(4,size,true);out.setUint32(8,KIND,true);raw.set(data,12);
 return{kind:KIND,headerRaw:raw.slice(0,12),data:raw.slice(12),raw};
}
export function readTouchReplay(decoded){
 const blocks=decoded.userBlocks.filter(row=>row.kind===KIND);if(!blocks.length)return null;
 if(blocks.length!==1)throw Error('Duplicate touch Replay USER');const b=blocks[0],end=b.data.indexOf(0);
 if(b.raw.length%4||end<0||b.data.subarray(end).some(n=>n!==0)||b.data.length>8*1024*1024+4)throw Error('Touch Replay USER padding/budget');
 return plans(JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(b.data.subarray(0,end))),decoded.stages);
}
export function installTouchReplay(module,plan){
 if(!plan)return;
 if(typeof module?._th128_world_touch_playback!=='function')throw Error('Native touch Replay runtime unavailable');
 const state=JSON.parse(module.UTF8ToString(module._th128_world_state())),generation=state.flow?.worldGeneration;
 if(!Number.isSafeInteger(generation)||generation<1)throw Error('Touch Replay actual native generation unavailable');
 const bytes=new Uint8Array(16+plan.events.length*16),v=new DataView(bytes.buffer);
 [KIND,plan.worldFrames,plan.events.length,plan.stage].forEach((n,i)=>v.setUint32(i*4,n,true));
 plan.events.forEach((row,i)=>row.forEach((n,k)=>v.setInt32(16+i*16+k*4,n,true)));
 const p=module._malloc(bytes.length);try{module.HEAPU8.set(bytes,p);if(module._th128_world_touch_playback(p,bytes.length,generation)!==1)throw Error(module.UTF8ToString(module._th128_world_error())||'Touch Replay native history rejected');}finally{module._free(p);}
}
export function touchReplaySnapshot(snapshot,plan){
 if(!plan)return snapshot;
 if(snapshot.touchEvents!==plan.events.length||!Number.isSafeInteger(snapshot.touchConsumed)||snapshot.touchConsumed<0||snapshot.touchConsumed>plan.events.length
  ||snapshot.touchWorldFrames!==plan.worldFrames||!Number.isSafeInteger(snapshot.touchWorldConsumed)||snapshot.touchWorldConsumed<0||snapshot.touchWorldConsumed>plan.worldFrames)throw Error('Touch Replay actual native cursor differs');
 return{...snapshot,complete:snapshot.complete&&snapshot.touchConsumed===plan.events.length&&snapshot.touchWorldConsumed===plan.worldFrames};
}
