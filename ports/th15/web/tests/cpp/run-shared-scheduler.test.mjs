import test from 'node:test';import assert from 'node:assert/strict';import{readFileSync,readdirSync}from 'node:fs';import{resolve}from 'node:path';import{core,memory,string,root,report}from './helpers.mjs';
test('TH15 stage factories preserve persistent game owners and the application scheduler across all stages',async()=>{
 const c=await core(),name=c.allocate(256),out=c.allocate(48);let scenes=0,frames=0,checks=0;const results=[];
 try{for(let character=0;character<4;character++)for(const firstStage of [1,7]){
  const a=c.stage_assets_create(),shared=c.shared_run_create(a),r=c.shared_run_gameplay(shared),owned=[];
  try{
   for(const file of readdirSync(resolve(root,'reference/assets')).filter(n=>/\.(anm|ecl|sht|std|msg)$/.test(n))){const bytes=readFileSync(resolve(root,'reference/assets',file)),p=c.allocate(bytes.length);owned.push(p);memory(c,p,bytes.length).set(bytes);memory(c,name,file.length+1).set(Buffer.from(file+'\0'));c.stage_assets_file(a,name,p,bytes.length);}
   assert.equal(c.run_gameplay_load(r,firstStage,character),1,string(c,c.run_gameplay_error(r)));const identities=Array.from({length:7},(_,i)=>c.run_gameplay_identity(r,i)),dv=()=>new DataView(c.memory.buffer);
   let progress=c.run_gameplay_field(r,0),player=c.run_gameplay_field(r,1),score=c.run_gameplay_field(r,2),maxBanks=0,maxBullets=0,maxEnemies=0;
   dv().setInt32(player+28,400,true);dv().setInt32(score+4,1000000,true);dv().setInt32(score+8,50000000,true);
   for(const stage of firstStage===7?[7]:[1,2,3,4,5,6]){
    if(stage!==firstStage){
     assert.equal(c.run_gameplay_close_spell(r),1,string(c,c.run_gameplay_error(r)));
     const before=Array.from({length:11},(_,i)=>dv().getUint32(player+i*4,true)),oldBackground=c.run_gameplay_identity(r,7);dv().setInt32(progress,stage,true);dv().setInt32(progress+4,0,true);dv().setInt32(progress+88,0,true);
     assert.equal(c.run_gameplay_next(r,stage),1,'next '+character+':'+stage+' '+string(c,c.run_gameplay_error(r)));assert.ok(c.run_gameplay_identity(r,11),'previous background survives factory');assert.notEqual(c.run_gameplay_identity(r,7),oldBackground,'background is a new stage object');
     assert.deepEqual(Array.from({length:7},(_,i)=>c.run_gameplay_identity(r,i)),identities,'original persistent manager identity');progress=c.run_gameplay_field(r,0);player=c.run_gameplay_field(r,1);score=c.run_gameplay_field(r,2);
     const after=Array.from({length:11},(_,i)=>dv().getUint32(player+i*4,true));for(const i of [0,1,2,3,4,6,7,8,9,10])assert.equal(after[i],before[i],'stock/progress preserved across factory '+i);checks+=15;
     c.run_gameplay_release_previous(r);assert.equal(c.run_gameplay_identity(r,11),0,'capture completion releases previous background');
    }
    assert.equal(c.run_gameplay_begin(r),1,'begin '+character+':'+stage+' '+string(c,c.run_gameplay_error(r)));let captured=null,stageEnemies=0,stageBullets=0;
    for(let frame=0;frame<2400;frame++){
     dv().setInt32(progress+8,frame,true);dv().setInt32(progress+12,frame,true);assert.equal(c.run_gameplay_step(r,1|0x100,frame%12===0?1:0,frame+1,frame+1),1,'stage '+stage+' char '+character+' frame '+frame+' '+string(c,c.run_gameplay_error(r)));c.run_gameplay_counts(r,out);const counts=Array.from(new Uint32Array(c.memory.buffer,out,12));assert.equal(counts[9],frame+1,'new background clock');stageEnemies=Math.max(stageEnemies,counts[0]);stageBullets=Math.max(stageBullets,counts[1]);maxBanks=Math.max(maxBanks,c.run_gameplay_resource_value(r,0));maxBullets=Math.max(maxBullets,counts[1]);maxEnemies=Math.max(maxEnemies,counts[0]);
     if(frame===1500){assert.equal(c.run_gameplay_capture(r,0),1,string(c,c.run_gameplay_error(r)));captured=Buffer.from(memory(c,c.run_gameplay_field(r,3),12));}
     if(frame===1540){assert.equal(c.run_gameplay_restore(r),1,string(c,c.run_gameplay_error(r)));assert.deepEqual(Buffer.from(memory(c,c.run_gameplay_field(r,3),12)),captured,'chapter restore uses retained player');}
     frames++;checks+=2;assert.equal(c.shared_run_updates(shared),c.shared_run_draws(shared)+1,'one global caption/platform update per game step');assert.ok(c.shared_run_draw(shared)>0);assert.equal(c.shared_run_draws(shared),c.shared_run_updates(shared));checks+=3;
    }
    assert.ok(stageEnemies>1,"Each stage keeps actual enemy updates after its previous background is released: "+character+":"+stage);assert.ok(stageBullets>0,"Each stage keeps actual projectile updates: "+character+":"+stage);checks+=2;scenes++;
   }
   assert.ok(maxEnemies>1,'actual ECL stage waves');assert.ok(maxBullets>0,'actual projectiles');results.push({character,firstStage,maxBanks,maxBullets,maxEnemies});
  }finally{c.shared_run_delete(shared);c.stage_assets_delete(a);for(const p of owned)c.release(p);}
 }
 report('run-shared-scheduler',{passed:true,scenes,frames,checks,results,scope:'Actual ANM/STD/ECL/SHT/MSG for four characters, six normal stages via the same persistent run owners plus independent Extra entry, previous-background retention/release, all seven persistent manager identities, global application update/draw callbacks surviving every scene factory, score/stock preservation, scheduled gameplay and real typed chapter snapshots after every stage factory. Controlled clocks, invulnerability, full power and accelerated dialogue; original factory control flow is verified separately. This does not yet establish complete native replay, compositor, browser or transition timing parity.'});
 }finally{c.release(name);c.release(out);}
});
