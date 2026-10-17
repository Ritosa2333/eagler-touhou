import test from 'node:test';import assert from 'node:assert/strict';import{readFileSync,readdirSync}from 'node:fs';import{resolve}from 'node:path';import{core,memory,string,root,report}from './helpers.mjs';
test('TH15 actual scheduled scene entrance waits for the clear notice and background transition, resets input on activation and replays both stages',async()=>{
 const c=await core(),name=c.allocate(256),out=c.allocate(48),files=[];let frames=0,checks=0,active=0;const results=[];
 try{
  for(const n of readdirSync(resolve(root,'reference/assets')).filter(n=>/\.(anm|ecl|sht|std|msg)$/.test(n))){const bytes=readFileSync(resolve(root,'reference/assets',n)),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);files.push({name:n,p,size:bytes.length});}
  const make=()=>{const a=c.stage_assets_create();for(const file of files){memory(c,name,file.name.length+1).set(Buffer.from(file.name+'\0'));c.stage_assets_file(a,name,file.p,file.size);}return {a,r:c.run_session_create(a)};};
  // Native ReplayManager mode differs between recording and playback; it is not recorded player state.
  const snapshot=r=>{const player=Buffer.from(memory(c,c.run_gameplay_field(r,1),48));player.fill(0,44,48);return Buffer.concat([Buffer.from(memory(c,c.run_gameplay_field(r,3),12)),player,Buffer.from(memory(c,c.run_gameplay_field(r,2),60)),Buffer.from(memory(c,c.run_session_controls(r),280)),Buffer.from(memory(c,c.run_session_age(r),20))]);};
  for(let character=0;character<4;character++){
   const recorded=make(),played=make(),trajectory=[[],[]],expected=[];let replayBytes;
   try{
    assert.equal(c.run_session_load(recorded.r,1,character,0,0),1,string(c,c.run_session_error(recorded.r)));assert.equal(c.run_session_clock(recorded.r),-1);const ids=Array.from({length:7},(_,i)=>c.run_gameplay_identity(recorded.r,i));let firstActivation=-1,nextActivation=-1;
    for(let stage=1;stage<=2;stage++){
     if(stage===2){assert.equal(c.run_session_next(recorded.r,2,1),1,string(c,c.run_session_error(recorded.r)));assert.deepEqual(Array.from({length:7},(_,i)=>c.run_gameplay_identity(recorded.r,i)),ids);}
     for(let frame=0;frame<720;frame++){
      const held=stage===2&&frame<151?0x141:1|(frame%121<31?0x40:frame%121<61?0x20:0),before=c.run_session_input_active(recorded.r);assert.equal(c.run_session_step(recorded.r,held,0,60,stage===2&&frame>=151),1,'record '+character+'/'+stage+'/'+frame+' '+string(c,c.run_session_error(recorded.r)));const clock=c.run_session_clock(recorded.r);c.run_gameplay_counts(recorded.r,out);const counts=Array.from(new Uint32Array(c.memory.buffer,out,12));
      if(stage===2&&frame<120){assert.equal(clock,720,'disabled recording callback preserves preceding stage clock');assert.equal(c.run_session_input_active(recorded.r),0,'clear notice does not enable replay input');assert.equal(counts[0],0,'no premature ECL main');assert.equal(counts[9],0,'background waits for clear notice');assert.deepEqual(Buffer.from(memory(c,c.run_session_controls(recorded.r),280)),Buffer.alloc(280),'held clocks do not accumulate during wait');checks+=4;}
      if(stage===2&&frame>=120&&frame<150){assert.equal(clock,720,'disabled recording callback leaves the old clock unchanged');assert.equal(c.run_session_input_active(recorded.r),0,'background transition does not enable replay input');assert.equal(counts[0],0,'main waits for the 30-tick transition');assert.ok(counts[9]>0,'new background updates during the transition');checks+=3;}
      if(!before&&c.run_session_input_active(recorded.r)){assert.equal(clock,1,'first enabled replay tick');const dv=new DataView(c.memory.buffer),controls=c.run_session_controls(recorded.r);assert.equal(dv.getUint32(controls+12,true),held,'first real input keeps its press edge');assert.equal(dv.getUint32(controls+152,true),1,'shoot duration starts at one');checks+=3;if(stage===1)firstActivation=frame;else nextActivation=frame;}
      const enabled=c.run_session_input_active(recorded.r);trajectory[stage-1].push(enabled?snapshot(recorded.r):null);expected.push({stage,frame,held,clock,enabled});if(enabled)active++;frames++;
     }
    }
    assert.equal(firstActivation,0);assert.equal(nextActivation,150);assert.equal(c.run_session_counters(recorded.r,0),1);assert.equal(c.run_session_counters(recorded.r,1),1);assert.equal(c.run_session_counters(recorded.r,2),1);assert.equal(c.run_gameplay_identity(recorded.r,11),0,'previous background released after capture transition');
    const p=c.allocate(16);memory(c,p,9).set(Buffer.from('ENTRY'+character+'\0'));assert.equal(c.run_session_export(recorded.r,p),1,string(c,c.run_session_error(recorded.r)));c.release(p);replayBytes=Buffer.from(memory(c,c.run_session_data(recorded.r),c.run_session_size(recorded.r)));const input=c.allocate(replayBytes.length);memory(c,input,replayBytes.length).set(replayBytes);
    try{
     assert.equal(c.run_session_load(played.r,1,character,input,replayBytes.length),1,string(c,c.run_session_error(played.r)));let index=0;
     for(let stage=1;stage<=2;stage++){if(stage===2)assert.equal(c.run_session_next(played.r,2,1),1,string(c,c.run_session_error(played.r)));
      for(let frame=0;frame<720;frame++){assert.equal(c.run_session_step(played.r,0,0,60,stage===2&&frame>=151),1,'play '+character+'/'+stage+'/'+frame+' '+string(c,c.run_session_error(played.r)));const e=expected[index++];assert.equal(c.run_session_input_active(played.r),e.enabled,'same scheduled input activation');assert.equal(c.run_session_clock(played.r),e.enabled?e.clock:-1,'same active input clock / original playback stage wait');if(trajectory[stage-1][frame])assert.deepEqual(snapshot(played.r),trajectory[stage-1][frame],'actual stage world/controls '+character+'/'+stage+'/'+frame);frames++;checks+=2;}
     }
    }finally{c.release(input);}
    results.push({character,firstActivation,nextActivation,replayBytes:replayBytes.length});
   }finally{c.run_session_delete(recorded.r);c.run_session_delete(played.r);c.stage_assets_delete(recorded.a);c.stage_assets_delete(played.a);}
  }
  report('run-session',{passed:true,frames,checks,activeRecordingFrames:active,results,scope:'Actual RunGameplay/SessionRuntime/SessionActivation/SessionReplay on the same scheduler, initial scene entered by runtime, 120-tick clear notice followed by 30-tick overlapping background transition, no premature main or input, input reset at first actual tick, retained run identity, capture/release requests and both original-format recorded stage streams replayed through the same world. Original runtime/activation control flow is separately native-verified. Invulnerability, full power and forced stage completion; GPU/audio/file services are explicit fixture boundaries, not a complete game application.'});
 }finally{for(const file of files)c.release(file.p);c.release(name);c.release(out);}
});
