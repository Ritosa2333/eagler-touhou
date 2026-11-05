import test from 'node:test';import assert from 'node:assert/strict';import{readFileSync,readdirSync}from'node:fs';import{resolve}from'node:path';
import{core,memory,string,root,report}from'./helpers.mjs';
test('TH17 retained run owns its player and effects across all six normal stages',async()=>{
 const c=await core(),source=c.asset_source_create();let manager=0,run=0,transitions=0,frames=0;
 const add=(name,bytes)=>{const b=Buffer.from(name+'\0'),n=c.allocate(b.length),p=c.allocate(bytes.length);memory(c,n,b.length).set(b);memory(c,p,bytes.length).set(bytes);c.asset_source_add(source,n,p,bytes.length);c.deallocate(n);c.deallocate(p);};
 const dv=()=>new DataView(c.memory.buffer),field=i=>c.run_gameplay_field(run,i),get=p=>dv().getInt32(p,true),set=(p,v)=>dv().setInt32(p,v,true),bytes=(p,n)=>Buffer.from(memory(c,p,n));
 try{
  for(const name of readdirSync(resolve(root,'reference/assets')).filter(n=>/\.(anm|ecl|msg|sht|std)$/.test(n)))add(name,readFileSync(resolve(root,'reference/assets',name)));
  for(let loadout=0;loadout<9;loadout++){
   manager=c.anm_manager_create();run=c.run_gameplay_create(source,manager,Math.floor(loadout/3),loadout%3);assert.equal(c.run_gameplay_load(run,1),1,string(c,c.run_gameplay_error(run)));
   const player=field(0),hud=field(5);assert.equal(c.run_gameplay_step(run,0),1,string(c,c.run_gameplay_error(run)));frames++;
   for(let stage=2;stage<=6;stage++){
    const session=field(1),score=field(2);set(session+24,200);set(session+32,4);set(session+36,1);set(score,1234567+stage);set(score+4,1900000+stage);
    // Exercise the real stage-clear notification as well as the ownership
    // transfer. A disabled retained HUD would otherwise freeze the incoming
    // controller at age zero while waiting for notice_age to reach 120.
    assert.equal(c.run_gameplay_complete_stage(run),1,string(c,c.run_gameplay_error(run)));
    set(session+12,29);set(session+16,71);set(session+80,37);
    assert.ok(c.run_gameplay_spawn_spirit(run));assert.ok(c.run_gameplay_spirit_count(run)>0);
    const retainedSession=bytes(session,44),retainedScore=bytes(score,28),position=bytes(field(3),12),starts=bytes(field(11),36);retainedSession.writeInt32LE(0,12);retainedSession.writeInt32LE(0,16);
    assert.equal(c.run_gameplay_next(run,stage),1,string(c,c.run_gameplay_error(run)));assert.equal(field(0),player);assert.equal(field(5),hud);
    assert.equal(c.run_gameplay_spirit_count(run),0,'Field spirits must not survive the stage replacement');assert.equal(get(field(1)+80),0,'Enemy Roaring counter belongs to the incoming stage');assert.ok(get(field(10)+100)&0x200,'Animal mode pauses until stage entry');
    assert.deepEqual(bytes(field(1),44),retainedSession,'Player stock must survive stage '+stage);assert.deepEqual(bytes(field(2),28),retainedScore,'Score must survive stage '+stage);assert.deepEqual(bytes(field(3),12),position);
    assert.deepEqual(bytes(field(11),36),starts,'Starts must count once per run');assert.ok(get(field(6))&0x800);assert.ok(get(field(9))&2);assert.ok(get(field(8))&4);
    const movement=stage%2?0x20:0x10;
    for(let waiting=0;get(field(7)+4)===0;waiting++){assert.ok(waiting<160,'Retained HUD must release the incoming scene');assert.equal(c.run_gameplay_step(run,movement),1,string(c,c.run_gameplay_error(run)));assert.equal(c.run_gameplay_enemy_count(run),0,'Combat waits for the stage-clear notice');frames++;}
    for(let frame=1;frame<30;frame++){assert.equal(c.run_gameplay_step(run,movement),1,string(c,c.run_gameplay_error(run)));assert.equal(c.run_gameplay_enemy_count(run),0,'Combat waits through transition');frames++;}
    assert.notDeepEqual(bytes(field(3),12),position,'Native player movement continues during the HUD wait and fade');
    assert.equal(c.run_gameplay_step(run,0),1,string(c,c.run_gameplay_error(run)));frames++;assert.equal(get(field(6))&0x800,0);assert.ok(c.run_gameplay_enemy_count(run)>0,'Stage main activates at age 30');
    // This headless lifecycle test has no draw pass. StageDrawFrame has its own
    // original comparisons; release here models the compositor finishing it.
    assert.equal(c.run_gameplay_release_previous(run),1);
    for(let frame=0;frame<90;frame++){assert.equal(c.run_gameplay_step(run,1|8),1,string(c,c.run_gameplay_error(run)));frames++;}transitions++;
   }
   c.run_gameplay_delete(run);run=0;c.anm_manager_delete(manager);manager=0;
  }
  report('run-gameplay-lifecycle',{passed:true,loadouts:9,transitions,frames,completeGame:false,scope:'Real stage resources and retained C++ run across stages 1–6, fixed player/HUD identity, exact stock/score/inventory retention, no repeated run-start record, deferred main activation at native age 30, destruction of outgoing scenes without replacing active effect hooks. Platform services observed; compositor completion is explicit, full native factory/cross-stage replay and visual equality remain separate acceptance.'});
 }finally{if(run)c.run_gameplay_delete(run);if(manager)c.anm_manager_delete(manager);c.asset_source_delete(source);}
});
