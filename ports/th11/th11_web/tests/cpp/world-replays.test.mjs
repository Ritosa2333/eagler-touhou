import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';import {resolve} from 'node:path';import {gzipSync,gunzipSync} from 'node:zlib';
import {core,oracle,memory,root,report,sha,target} from './helpers.mjs';import {worldOracle} from './world-oracle.mjs';
const captureTools=process.env.TH11_VERIFIER_CAPTURE?await import('../../../tools/replay-verifier/observation.mjs'):null;

// Opt-in: original x86 world emulation takes much longer than component tests.
// No game callbacks, damage, enemies or collision are skipped on either side.
test('TH11 whole-world original demos retain native economy, movement and RNG', {skip:!process.env.TH11_WORLD_REPLAYS},async()=>{
 const campaign=process.env.TH11_WORLD_CAMPAIGN==='1',countInstructions=process.env.TH11_ORACLE_COUNT_INSTRUCTIONS!=='0',drawClocks=process.env.TH11_WORLD_DRAW_CLOCKS!=='0';
 const demos=process.env.TH11_WORLD_REPLAYS==='all'?[0,1,2,3]:process.env.TH11_WORLD_REPLAYS.split(',').map(Number);
 const globals=[0x4a56e4,0x4a56e8,0x4a56f0,0x4a56f4,0x4a5718,0x4a571c,0x4a5720,0x4a5744,0x4a5748,0x4a574c,0x4a5754,0x4a576c];
 const motionFields=[[0,0x87c,12],[12,0x888,8],[20,0x890,16],[36,0x8a0,12],[48,0x8ac,12],[60,0x8b8,8],[68,0x914,8],[76,0x91c,12],[88,0x8d20,4],[92,0x95c,4],[96,0x8ba0,16],[112,0x8bc8,4],[116,0x8bc4,4],[120,0x7c90,4],[124,0x8c14,4]];
 for(const demo of demos){
  const c=await core(),m=await oracle(),external=process.env.TH11_WORLD_FILE,raw=readFileSync(external?resolve(external):resolve(root,`reference/assets/demo${demo}.rpy`)),p=c.allocate(raw.length),r=c.replay_create();memory(c,p,raw.length).set(raw);assert.equal(c.replay_open(r,p,raw.length),1);
  const b=Buffer.from(memory(c,c.replay_data(r),c.replay_size(r))),selected=Number(process.env.TH11_WORLD_STAGE||b.readUInt16LE(112)),record=c.replay_stage(r,selected),offset=new DataView(c.memory.buffer).getUint32(record+4,true),options={stage:selected,character:b.readUInt32LE(92),subtype:b.readUInt32LE(96),difficulty:b.readUInt32LE(100),demo};
  // Stop before the terminal marker; replay shutdown is a separate menu gate.
  const frames=Math.min(campaign?110000:b.readUInt32LE(offset+4)-(captureTools?0:1),Number(process.env.TH11_WORLD_LIMIT||Infinity));let checks=0,activeTicks=0,firstInactiveFrame=null,finalState;
  const capture=captureTools?.captures(process.env.TH11_VERIFIER_CAPTURE,process.env.TH11_VERIFIER_CASE||`demo${demo}`,captureTools.identity(root,raw),{replaySha256:sha(raw),coreSha256:sha(readFileSync(resolve(root,'artifacts/cpp/game-core-test.wasm'))),stageDrawClocks:drawClocks});
  try{
   // Optional acceleration for the fixed original replay corpus: omit only
   // Unicorn's per-instruction budget hook, retaining every gameplay call and
   // every state comparison. NativeMachine still checks each return address.
   if(!countInstructions){const run=m.cpu.emu_start.bind(m.cpu);m.cpu.emu_start=(a,b,t)=>run(a,b,t,0);}
   const identity={coreSha256:sha(readFileSync(process.env.TH11_CORE_TEST_FILE?resolve(process.env.TH11_CORE_TEST_FILE):resolve(root,'artifacts/cpp/game-core-test.wasm'))),replaySha256:sha(raw),targetSha256:target.sha256};
   if(drawClocks)identity.stageDrawClocks=1;
   const w=worldOracle(m,{...options,replayData:external?raw:null,campaign});assert.equal(w.construct(),0);let currentFrame=0;
   const diagnose=process.env.TH11_WORLD_DIAGNOSE==='1';
   if(diagnose)for(const address of [0x458bc0,0x458c30])m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE,()=>console.log('native RNG',currentFrame,address.toString(16),'return',m.u32(m.reg('ESP')).toString(16),'ecx',m.reg('ECX').toString(16),'esi',m.reg('ESI').toString(16),'state',Array.from(m.bytes(0x4c2f00,8))),null,address,address));
   if(diagnose){
    m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE,()=>console.log('native damage result',m.reg('ECX')),null,0x434e10,0x434e10));
    m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE,()=>console.log('native take damage',m.reg('EAX'),'before',m.i32(m.reg('ECX')+12)),null,0x4100e0,0x4100e0));
    for(const address of [0x411bd4,0x411bfc,0x411c55,0x411c91,0x411cb7])m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE,()=>console.log('native damage phase',address.toString(16),m.reg('EAX'),m.reg('ESI'),'life',m.i32(m.u32(0x4a8eb4)+0x928),'spell',m.u32(m.u32(0x4a8d6c)+0x8e0)),null,address,address));
    m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE,()=>{const p=m.u32(m.reg('ESP')+4),z=m.u32(m.reg('ESP')+8);console.log('native damage target',currentFrame,p.toString(16),[m.f32(p),m.f32(p+4)],[m.f32(z),m.f32(z+4)]);},null,0x4347f0,0x4347f0));
    m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE,()=>{const p=m.reg('EBX')-0x4c;console.log('native shot hit',(p-m.u32(0x4a8eb4)-0x96c)/0x6c,'damage',m.i32(p+0x60),'position',[m.f32(p+0x14),m.f32(p+0x18)]);},null,0x434982,0x434982));
   }
   if(process.env.TH11_WORLD_TRACE)m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE,()=>{if(currentFrame>=3098)console.log('native score',currentFrame,m.i32(m.reg('ESP')+4),m.u32(m.reg('ESP')).toString(16));},null,0x40ceb0,0x40ceb0));
   const archive=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(archive.length),source=c.resources_create(),session=c.game_session_create();memory(c,data,archive.length).set(archive);assert.equal(c.resources_open(source,data,archive.length),1);assert.equal(c.game_session_replay(session,source,p,raw.length,selected,1),1);
   const render=campaign||drawClocks?c.comp_create():0;
   const registers=['EAX','EBX','ECX','EDX','ESI','EDI','EBP','ESP','EFLAGS','FPCW','FPSW','FPTAG','MXCSR'];let startFrame=0;
   if(process.env.TH11_WORLD_RESUME){const dir=resolve(process.env.TH11_WORLD_RESUME),saved=JSON.parse(readFileSync(resolve(dir,'state.json')));assert.deepEqual(saved.identity,identity,'checkpoint executable/replay/core identity');assert.deepEqual(saved.pointers,{data,source,session,render,p,r});const bytes=gunzipSync(readFileSync(resolve(dir,'cpp.gz')));if(c.memory.buffer.byteLength<bytes.length)c.memory.grow((bytes.length-c.memory.buffer.byteLength)/65536);new Uint8Array(c.memory.buffer).set(bytes);m.write(0x10000,gunzipSync(readFileSync(resolve(dir,'native.gz'))));w.heap_state(saved.heap);for(const[name,value]of Object.entries(saved.registers))m.reg(name,value);startFrame=saved.nextFrame;checks=saved.checks;activeTicks=saved.activeTicks;firstInactiveFrame=saved.firstInactiveFrame;console.log('resumed native comparison',startFrame);}
   // Diagnostic tail comparison across C++ rebuilds: rebuild C++ state by
   // replaying from zero, restore only the original executable checkpoint.
   // This is deliberately reported as a partial proof, never a full campaign.
   const nativeResumeOnly=!!process.env.TH11_WORLD_NATIVE_RESUME;
   let nativeCheckpoint=null;
   if(nativeResumeOnly){
    assert.ok(!process.env.TH11_WORLD_RESUME,'choose one checkpoint mode');
    const dir=resolve(process.env.TH11_WORLD_NATIVE_RESUME),saved=JSON.parse(readFileSync(resolve(dir,'state.json')));
    nativeCheckpoint={metadataSha256:sha(readFileSync(resolve(dir,'state.json'))),nativeSha256:sha(readFileSync(resolve(dir,'native.gz')))};
    for(const k of ['replaySha256','targetSha256','stageDrawClocks'])assert.equal(saved.identity[k],identity[k],'native checkpoint '+k);
    for(let frame=0;frame<saved.nextFrame;++frame){assert.equal(c.game_session_update(session,0,0,0,0,0,0),1,'C++ checkpoint warmup '+frame);if(render)assert.equal(c.game_session_render(session,render),1);}
    m.write(0x10000,gunzipSync(readFileSync(resolve(dir,'native.gz'))));w.heap_state(saved.heap);for(const[name,value]of Object.entries(saved.registers))m.reg(name,value);startFrame=saved.nextFrame;console.log('native-only diagnostic tail',startFrame);
   }
   for(let frame=startFrame;frame<frames;++frame){
    const dumpShots=label=>{if(!diagnose)return;const cp=c.game_session_transition_data(session,4),np=m.u32(0x4a8eb4);writeFileSync(resolve(root,`artifacts/cpp/shot-${label}-cpp.bin`),memory(c,cp+7052,256*108));writeFileSync(resolve(root,`artifacts/cpp/shot-${label}-native.bin`),m.bytes(np+0x96c,256*108));};
    dumpShots('before');
    if(diagnose){console.log('spell flags before',new DataView(c.memory.buffer).getUint32(c.battle_spell_data(session,4),true),m.u32(m.u32(0x4a8d6c)+0x8e0));const addr=m.u32(0x4a8d6c)+0x8e0;m.hooks.push(m.cpu.hook_add(m.uc.HOOK_MEM_WRITE,(_a,_b,address,size,value)=>console.log('native spell write',m.reg('EIP').toString(16),address,size,value),null,addr,addr+3));}
    if(diagnose){let cs=c.game_session_enemy_state(session,0),node=m.u32(m.u32(0x4a8d7c)+0x68);const dv=new DataView(c.memory.buffer);while(cs&&node){console.log('before enemy health',dv.getInt32(cs+0x14fc,true),m.i32(m.u32(node)+0x2538));const next=dv.getUint32(cs+0x16c,true);cs=next?dv.getUint32(next,true)+0x103c:0;node=m.u32(node+4);}}
    if(diagnose){const cp=c.game_session_transition_data(session,4),np=m.u32(0x4a8eb4);for(const [name,bytes]of[['cpp',memory(c,cp+3308,32*116)],['native',m.bytes(np+0x7c9c,32*116)]])writeFileSync(resolve(root,`artifacts/cpp/areas-before-${name}.bin`),bytes);}
    if(diagnose){const cs=c.game_session_stage_data(session,0),ns=m.u32(0x4a8d60),d=new DataView(c.memory.buffer);console.log('stage before',frame,[0x2ff0,0x3010,0x3028,0x38,0x40].map(off=>[off.toString(16),d.getUint32(cs+off,true),m.u32(ns+off)]),'screen',m.u32(0x4c342c));}
    currentFrame=frame;w.tick();assert.equal(c.game_session_update(session,0,0,0,0,0,0),1,`demo${demo} frame${frame} C++ update`);
    // An all-FFFF termination record is lifecycle evidence, not gameplay input.
    // Retail reaches its shutdown callback within the chain; C++ stops before
    // the remaining callbacks. Neither half-transaction is a completed tick.
    if(capture&&m.u32(0x4c93c0)===0xffff){assert.equal(c.game_session_value(session,0),4,'Replay marker must terminate candidate');finalState={phase:4,stage:c.game_session_value(session,2),score:m.i32(0x4a56e4),lives:m.i32(0x4a5718),terminalMarker:true};firstInactiveFrame=frame;break;}
    if(campaign&&m.u32(0x4c37d8)===12)assert.equal(w.next_stage(),0,'native stage construction');
    if(render){w.draw_transition();assert.equal(c.game_session_render(session,render),1,'C++ frame draw');}
    if(capture){capture.original.tick(captureTools.nativeRow(m,frame));capture.candidate.tick(captureTools.candidateRow(c,session,frame));}
    dumpShots('after');
    if(diagnose)console.log('spell flags after',new DataView(c.memory.buffer).getUint32(c.battle_spell_data(session,4),true),m.u32(m.u32(0x4a8d6c)+0x8e0));
    if(diagnose){const cs=c.game_session_stage_data(session,0),ns=m.u32(0x4a8d60),d=new DataView(c.memory.buffer);console.log('stage after',frame,[0x2ff0,0x3010,0x3028,0x38,0x40].map(off=>[off.toString(16),d.getUint32(cs+off,true),m.u32(ns+off)]));}
    const dv=new DataView(c.memory.buffer),e=c.battle_data(session,0),label=`demo${demo} frame${frame}`;
    if(drawClocks&&c.game_session_transition_value(session,2)){const cs=c.game_session_stage_data(session,0),ns=m.u32(0x4a8d60);for(const off of [0x38,0x3c,0x40,0x2ff0,0x2ff8,0x3028])assert.equal(dv.getUint32(cs+off,true),m.u32(ns+off),`${label} stage clock ${off.toString(16)}`);}
    if(process.env.TH11_WORLD_TRACE&&frame>=3098)console.log('economy',frame,globals.map((a,i)=>[dv.getInt32(e+i*4,true),m.i32(a)]));
    const player=m.u32(0x4a8eb4),motion=c.battle_data(session,2);
    const lifecycle=c.battle_data(session,1);
    for(const[off,np,n]of[[0,0x928,4],[64,0x908,12],[76,0x8e4,36],[112,0x8cc,24],[136,0x8bcc,72]])assert.deepEqual(Buffer.from(memory(c,lifecycle+off,n)),Buffer.from(m.bytes(player+np,n)),`${label} player lifecycle ${np.toString(16)}`);
    for(const [co,no,n]of motionFields)assert.deepEqual(Buffer.from(memory(c,motion+co,n)),Buffer.from(m.bytes(player+no,n)),`${label} motion ${no.toString(16)}`);
    assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(c.game_session_animations(session),0),8)),Buffer.from(m.bytes(0x4c2f00,8)),label+' script RNG');
    assert.equal(c.game_session_value(session,8),m.u32(m.u32(0x4a8d68)+0x5c),label+' bullet count');
    let cs=c.game_session_enemy_state(session,0),node=m.u32(m.u32(0x4a8d7c)+0x68),enemies=0;
    while(cs||node){assert.ok(cs&&node,label+' enemy list');const ns=m.u32(node)+0x103c;
     for(const[off,n]of[[0,0xe0],[0x104,0x50],[0x14ec,0x1c],[0x1580,0x14]])assert.deepEqual(Buffer.from(memory(c,cs+off,n)),Buffer.from(m.bytes(ns+off,n)),`${label} enemy${enemies} state${off.toString(16)}`);
     const next=dv.getUint32(cs+0x16c,true);cs=next?dv.getUint32(next,true)+0x103c:0;node=m.u32(node+4);++enemies;
    }
    if(c.battle_items){const ci=c.battle_items(session),ni=m.u32(0x4a8e90)+0x14;
     for(let i=0;i<2198;++i){const a=ci+i*0x478,b=ni+i*0x478;if(!dv.getUint32(a+0x464,true)&&!m.u32(b+0x464))continue;
      for(const[off,n]of[[0x434,0x28],[0x460,0x18]])assert.deepEqual(Buffer.from(memory(c,a+off,n)),Buffer.from(m.bytes(b+off,n)),`${label} item${i} state${off.toString(16)}`);
     }
    }
    globals.forEach((a,i)=>assert.equal(dv.getInt32(e+i*4,true),m.i32(a),`${label} economy ${a.toString(16)}`));
    ++checks;const phase=c.game_session_value(session,0);if(phase===1)++activeTicks;else if(firstInactiveFrame===null)firstInactiveFrame=frame;
    finalState={phase,stage:c.game_session_value(session,2),score:m.i32(0x4a56e4),lives:m.i32(0x4a5718)};
    if(frame%600===0)console.log(label,w.heap_stats(),finalState);
    if(campaign&&phase!==1)break;
    if(campaign&&process.env.TH11_WORLD_CHECKPOINT&&(frame===8799||(frame+1)%12000===0||frame+1===Number(process.env.TH11_WORLD_CHECKPOINT_FRAME))){
     const dir=resolve(process.env.TH11_WORLD_CHECKPOINT);mkdirSync(dir,{recursive:true});assert.equal(m.reg('FPTAG'),65535,'checkpoint requires empty x87 stack');writeFileSync(resolve(dir,'native.gz'),gzipSync(m.view(0x10000,0x07ff0000),{level:1}));writeFileSync(resolve(dir,'cpp.gz'),gzipSync(new Uint8Array(c.memory.buffer),{level:1}));writeFileSync(resolve(dir,'state.json'),JSON.stringify({identity,nextFrame:frame+1,checks,activeTicks,firstInactiveFrame,pointers:{data,source,session,render,p,r},heap:w.heap_state(),registers:Object.fromEntries(registers.map(name=>[name,m.reg(name)]))}));console.log('saved native comparison',frame+1);
    }
   }
   if(capture){const complete=finalState.phase===4&&(!campaign||finalState.stage===(selected===7?7:6));for(const stream of Object.values(capture))stream.finish({complete,reason:complete?'replay-complete':'capture-incomplete',evidence:{checks,selection:options,finalState,replaySha256:sha(raw)}});assert.ok(complete,'verifier requires terminal lifecycle and full route');}
   const name=campaign?(selected===7?'world-replay-external-extra':'world-replay-external-campaign'):external?`world-replay-external-stage${selected}`:`world-replay-demo${demo}`;
   report(name+(nativeResumeOnly?'-tail':campaign&&finalState.phase===1?'-partial':''),{passed:true,countInstructions,stageDrawClocks:drawClocks,completed:!nativeResumeOnly&&finalState.phase!==1,startFrame,nativeResumeOnly,nativeCheckpoint,replaySha256:sha(raw),checks,frames,activeTicks,firstInactiveFrame,finalState,selection:options,scope:'Native scheduler and C++ battle run original inputs. Each tick checks 12 economy fields, complete player movement, script RNG, active projectile count and selected enemy/item fields. Active ticks are distinguished from frozen post-game states. Host resource, GPU and audio boundaries are replaced. Stage draw timing executes original background/foreground clock instructions; campaign mode additionally runs native cross-stage teardown/construction. Full rendered pixels and shutdown are separate checks. Native-only checkpoint warmup checks only the reported tail, not the earlier C++ warmup.'});
   if(render)c.comp_delete(render);
   c.game_session_delete(session);c.resources_delete(source);c.release(data);
  }finally{c.replay_delete(r);c.release(p);m.close();}
 }
});
