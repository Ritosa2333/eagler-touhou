import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync,readdirSync}from 'node:fs';import{resolve}from 'node:path';import{core,memory,string,root,report}from './helpers.mjs';
test('TH15 actual main-loop stage flow advances from unmodified ECL/dialogue completion after frame return and preserves its run owners',async()=>{
 const c=await core(),name=c.allocate(256),out=c.allocate(48),files=[];let frames=0,checks=0;const results=[],characters=(process.env.TH15_RUN_FLOW_CHARACTERS||'0').split(',').map(Number),limit=Number(process.env.TH15_RUN_FLOW_STAGE_LIMIT||90000),difficulties=(process.env.TH15_RUN_FLOW_DIFFICULTIES||'1').split(',').map(Number),firstStage=Number(process.env.TH15_RUN_FLOW_START_STAGE||1);assert.ok(firstStage===1||firstStage===7);assert.ok(difficulties.every(n=>Number.isInteger(n)&&n>=0&&n<=4&&(firstStage===7?n===4:n<4)));
 try{
  for(const n of readdirSync(resolve(root,'reference/assets')).filter(n=>/\.(anm|ecl|sht|std|msg)$/.test(n))){const bytes=readFileSync(resolve(root,'reference/assets',n)),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);files.push({name:n,p,size:bytes.length});}
  for(const character of characters)for(const difficulty of difficulties){const a=c.stage_assets_create(),r=c.run_construction_create(a);try{
   for(const f of files){memory(c,name,f.name.length+1).set(Buffer.from(f.name+String.fromCharCode(0)));c.stage_assets_file(a,name,f.p,f.size);}
   assert.equal(c.run_construction_load(r,firstStage,character,0,difficulty,0,0),1,string(c,c.run_construction_error(r)));
   const dv=()=>new DataView(c.memory.buffer),int=p=>dv().getInt32(p,true),ids=Array.from({length:7},(_,i)=>c.run_gameplay_identity(r,i)),progress=c.run_gameplay_field(r,0),session=c.run_gameplay_field(r,1);
   // Controlled full power and invulnerability exercise real clear commands;
   // they are not evidence of an independently matching original whole run.
   dv().setInt32(session+28,400,true);
   let stage=firstStage,stageFrames=0,maxEnemies=0,maxBullets=0,maxLasers=0,finished=false,transitions=0;const scenes=[];
   for(let tick=0;tick<limit*6;tick++){
    const flags=dv().getUint32(progress+96,true),previous=c.run_gameplay_identity(r,11),backgroundDone=!!previous&&!(flags&0x800);
    assert.equal(c.run_construction_step(r,1|0x100,0,60,backgroundDone),1,'char '+character+' stage '+stage+' frame '+stageFrames+' '+string(c,c.run_construction_error(r)));frames++;stageFrames++;
    const current=int(progress),newFlags=dv().getUint32(progress+96,true);
    assert.deepEqual(Array.from({length:7},(_,i)=>c.run_gameplay_identity(r,i)),ids,'persistent run owners after complete frame');checks++;
    assert.equal(c.run_construction_queue(r,0),0,'main loop consumes clear request before next frame');checks++;
    if(current!==stage){assert.equal(current,stage+1);assert.ok(stageFrames<limit,'stage completion limit');assert.equal(memory(c,c.run_construction_record_field(r,character,1,difficulty,stage,1)+4,1)[0],1,'real stage clear record');assert.ok(c.run_gameplay_identity(r,11),'transition retains actual outgoing background');assert.equal(c.run_session_input_active(r),0,'new input waits until activation');scenes.push({stage,frames:stageFrames,maxEnemies,maxBullets,maxLasers});console.log('character',character,'difficulty',difficulty,'stage',stage,'natural clear',stageFrames);stage=current;stageFrames=0;maxEnemies=maxBullets=maxLasers=0;transitions++;checks+=5;}
    c.run_gameplay_counts(r,out);const counts=Array.from(new Uint32Array(c.memory.buffer,out,12));maxEnemies=Math.max(maxEnemies,counts[0]);maxBullets=Math.max(maxBullets,counts[1]);maxLasers=Math.max(maxLasers,counts[2]);
    if(newFlags&0x4000){assert.equal(stage,firstStage===7?7:6);assert.equal(int(c.run_construction_record_field(r,character,1,difficulty,stage,3)),1,'real full-run clear record');assert.equal(int(c.run_construction_record_field(r,character,1,difficulty,stage,2)),1,'real no-continue unlock record');scenes.push({stage,frames:stageFrames,maxEnemies,maxBullets,maxLasers});finished=true;checks+=3;break;}
    assert.ok(stageFrames<limit,JSON.stringify({stage,stageFrames,flags:newFlags,progress:Array.from(new Int32Array(c.memory.buffer,progress,25)),counts,age:int(c.run_session_age(r)+4),inputActive:c.run_session_input_active(r),maxEnemies,maxBullets,maxLasers}));
   }
   assert.ok(finished,'unmodified final dialogue must reach real completion');assert.equal(transitions,firstStage===7?0:5);assert.equal(int(c.run_construction_record_field(r,character,1,difficulty,firstStage,0)),1,'whole run counts as one play');results.push({character,difficulty,firstStage,transitions,scenes});
  }finally{c.run_construction_delete(r);c.stage_assets_delete(a);}}
  report('run-stage-flow'+(firstStage===7?'-extra':'')+(process.env.TH15_RUN_FLOW_DIFFICULTIES?'-matrix':''),{passed:true,frames,checks,results,scope:'Actual original resources and original session clocks drive natural stage completion, clear records, real retained-object teardown and next-stage initialization after callback return. Full power, invulnerability and accelerated dialogue remain controlled input. Ending rendering, files/audio/capture platform work, browser and independent native whole-run/replay parity remain separate.'});
 }finally{for(const f of files)c.release(f.p);c.release(name);c.release(out);}
});
