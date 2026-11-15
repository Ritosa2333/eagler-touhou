import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync,readdirSync} from 'node:fs';import {resolve} from 'node:path';
import {core,memory,string,root,report} from './helpers.mjs';
test('TH17 actual stage scripts activate the production scene and all seven coupled worlds',async()=>{
 const c=await core(),source=c.asset_source_create();let manager=0,assets=0,game=0,frames=0;const summaries=[];
 const add=(name,bytes)=>{const n=Buffer.from(name+'\0'),np=c.allocate(n.length),p=c.allocate(bytes.length);memory(c,np,n.length).set(n);memory(c,p,bytes.length).set(bytes);c.asset_source_add(source,np,p,bytes.length);c.deallocate(p);c.deallocate(np);};
 try{
  for(const name of readdirSync(resolve(root,'reference/assets')).filter(n=>/\.(anm|ecl|msg|sht|std)$/.test(n)))add(name,readFileSync(resolve(root,'reference/assets',name)));
  const stages=process.env.TH17_STAGE?[Number(process.env.TH17_STAGE)]:[1,2,3,4,5,6,7],limit=Number(process.env.TH17_STAGE_LIMIT??1200);
  const cases=stages.flatMap(stage=>(process.env.TH17_ALL_LOADOUTS==='1'?Array.from({length:9},(_,i)=>[Math.floor(i/3),i%3]):[[(stage-1)%3,stage%3]]).flatMap(([character,animal])=>(stage===7?[4]:process.env.TH17_ALL_DIFFICULTIES==='1'?[0,1,2,3]:[1]).map(difficulty=>({stage,character,animal,difficulty}))));
  for(const {stage,character,animal,difficulty}of cases){
   manager=c.anm_manager_create();assets=c.stage_assets_create(source,manager,0);
   assert.equal(c.stage_assets_load(assets,stage,character,animal,0),1,string(c,c.stage_assets_error(assets)));
   game=c.stage_gameplay_create(assets,manager,stage,character,animal,difficulty);assert.equal(c.stage_gameplay_prepare(game),1,string(c,c.stage_gameplay_error(game)));
   let maximumEnemies=0,maximumBullets=0,stageFrames=0;const protectedPlayer=process.env.TH17_STAGE_PROTECTED==='1';
   for(let frame=0;frame<limit;frame++){
    if(protectedPlayer){const dv=new DataView(c.memory.buffer),inv=c.stage_gameplay_field(game,5);dv.setInt32(inv,59999,true);dv.setInt32(inv+4,60000,true);dv.setFloat32(inv+8,60000,true);dv.setUint32(inv+16,1,true);}
    const held=1|0x200|(frame%180<90?0x10:0x20)|(frame%120<60?8:0);
    assert.equal(c.stage_gameplay_step(game,held,1),1,`stage ${stage} frame ${frame}: ${string(c,c.stage_gameplay_error(game))}`);
    maximumEnemies=Math.max(maximumEnemies,c.stage_gameplay_enemy_count(game));maximumBullets=Math.max(maximumBullets,c.stage_gameplay_bullet_count(game));frames++;stageFrames++;
    c.stage_gameplay_clear_events(game);if(c.stage_gameplay_screen(game)||(new DataView(c.memory.buffer).getUint32(c.stage_gameplay_field(game,11),true)&0x10))break;
   }
   const summary={stage,character,animal,difficulty,frames:stageFrames,maximumEnemies,maximumBullets,protectedPlayer,scene:c.stage_gameplay_screen(game),flags:new DataView(c.memory.buffer).getUint32(c.stage_gameplay_field(game,11),true)};summaries.push(summary);assert.ok(maximumEnemies>0,'actual stage spawned enemies');
   if(protectedPlayer&&limit>=90000)assert.equal(summary.scene,stage<6?12:stage===6?15:16,'Actual ECL reaches its ending '+JSON.stringify(summary));
   if(cases.length>7)console.log(JSON.stringify(summary));
   c.stage_gameplay_delete(game);game=0;c.stage_assets_delete(assets);assets=0;c.anm_manager_delete(manager);manager=0;
  }
  report(process.env.TH17_ALL_LOADOUTS==='1'?'stage-gameplay-all-loadouts':process.env.TH17_STAGE_PROTECTED==='1'?'stage-gameplay-actual-script-protected':'stage-gameplay-actual-script-smoke',{passed:true,frames,summaries,scope:'Production StageGameplay with real stage ECL/STD/MSG/ANM/SHT, shared scheduler and combat owners. Platform audio, text rasterization, popups, achievements and scene destinations are captured. An optional explicitly marked protected-player fixture bypasses death only to reach later script paths. This checks integration failures in actual script streams, not original replay synchronization or GPU output.'});
 }finally{if(game)c.stage_gameplay_delete(game);if(assets)c.stage_assets_delete(assets);if(manager)c.anm_manager_delete(manager);c.asset_source_delete(source);}
});
