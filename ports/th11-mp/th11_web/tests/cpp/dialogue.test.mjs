import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,root,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';
import {normalizeAnimation,firstDifference} from './shot-oracle.mjs';
test('TH11 original dialogue scripts preserve timers, portraits, expressions, text requests and interrupts',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),stack=n=>m.u32(m.reg('ESP')+n*4);let events=[],checks=0,frames=0,entries=0,stageBoundary=false;
 const text=o.load('text.anm'),front=o.load('front.anm',true),players=[o.load('pl00.anm',true),o.load('pl01.anm',true)];
 const hud=m.allocate(0x4460),player=m.allocate(0x30),enemy=m.allocate(0x60),stage=m.allocate(0x40),laser=m.allocate(0x30),state=m.allocate(0xac);
 for(const [a,p]of [[0x4a8d84,hud],[0x4a8eb4,player],[0x4a8d7c,enemy],[0x4a8ec8,stage],[0x4a8e94,laser]])m.u32(a,p);
 m.u32(0x4c3808,text.original);m.u32(hud+0x444c,front.original);m.u32(hud+0x4438,state);m.u32(laser+0x18,0);
 m.replace(0x40b5c0,'isolate bullet cancellation',()=>0);m.replace(0x412880,'isolate enemy cancellation',()=>0);
 m.replace(0x454c60,'record original glyph requests',()=>{const b=Buffer.from(m.bytes(stack(5),256)),n=b.indexOf(0);events.push(0,m.u32(m.reg('ESI')),stack(1),stack(3),stack(4),m.reg('EDI'),n,...b.subarray(0,n));return 0;});
 m.replace(0x44a1e0,'record original dialogue sound',()=>{events.push(1,m.reg('ESI'));return 0;});
 m.replace(0x42a150,'record boss music',()=>{events.push(2);return 0;},2);
 m.replace(0x42a270,'record music fade',()=>{events.push(3,stack(1));return 0;},1);
 m.replace(0x41eb15,'stop at stage-completion integration boundary',()=>{stageBoundary=true;m.reg('ESP',m.stack-8);return 0xfffffffe;});
 const commonHeap=m.heap,view=()=>new DataView(c.memory.buffer),read=p=>view().getUint32(p,true);
 const put=(p,v)=>view().setUint32(p,v,true);
 function loadCpp(f,index,data){const p=c.allocate(data.length);memory(c,p,data.length).set(data);assert.equal(c.anm_open(c.dialog_resource(f,index),p,data.length),1);c.release(p);}
 try{for(let st=1;st<=7;++st){
  if(process.env.TH11_DIALOGUE_STAGE&&st!==+process.env.TH11_DIALOGUE_STAGE)continue;
  m.heap=commonHeap;const enemies=o.load(`stgenm${String(st).padStart(2,'0')}.anm`,true),logo=o.load(`st${String(st).padStart(2,'0')}logo.anm`,true);
  for(const [r,id]of [[text,0],[front,5],...players.map(r=>[r,7]),[enemies,9],[logo,27]])m.view(r.original,2).set([id,0]);
  m.u32(enemy+0x48,enemies.original);m.u32(hud+0x43ec,logo.original);const heap=m.heap;
  for(let shot=0;shot<6;++shot){
   if(process.env.TH11_DIALOGUE_SHOT&&shot!==+process.env.TH11_DIALOGUE_SHOT)continue;
   const char=Math.floor(shot/3),bytes=readFileSync(resolve(root,'reference/assets',`st${String(st).padStart(2,'0')}_0${char}${'abc'[shot%3]}.msg`));
   for(let id=0;id<bytes.readUInt32LE(0);++id)for(const skip of [false,true]){
    const begin=bytes.readUInt32LE(4+id*8);let p=begin,stageEnd=false;while(p+4<=bytes.length){if(bytes[p+2]===21)stageEnd=true;if(bytes[p+2]===0)break;p+=4+bytes[p+3];}
    // Stage-result opcode 21 is deliberately unsupported in the live game.
    // Verify all preceding operations, then stop before its external systems.
    const f=c.dialog_create(),manager=c.dialog_manager(f),s=c.dialog_data(f,0),d=c.allocate(bytes.length);memory(c,d,bytes.length).set(bytes);c.dialog_load(f,d,bytes.length);c.release(d);
    [text,players[char],front,enemies,logo].forEach((r,i)=>loadCpp(f,i,r.source));
    m.heap=heap;o.reset();m.view(state,0xac).fill(0);const nm=m.allocate(bytes.length);m.write(nm,bytes);m.u32(player+0x10,players[char].original);m.i32(0x4a5710,char);m.i32(0x4a5714,shot%3);m.i32(0x4a5728,st);m.i32(0x4a5730,id+1);m.i32(0x4a5738,0);
    const speed=skip?1:.5;view().setFloat32(c.anm_env_rate(manager),speed,true);m.f32(0x4a7948,speed);for(const v of [0,1]){memory(c,c.anm_env_rng(manager,v),8).fill(0);put(c.anm_env_rng(manager,v),12345);}
    function compare(label){
     const a=Buffer.from(memory(c,s,0xac)),b=Buffer.from(m.bytes(state,0xac));for(const out of [a,b])for(const off of [0x10,0x24,0x38])out.writeUInt32LE(0,off);
     a.writeUInt32LE(a.readUInt32LE(0x64)-c.dialog_data(f,1),0x64);b.writeUInt32LE(b.readUInt32LE(0x64)-nm,0x64);assert.equal(firstDifference(a,b),'',label+' controller');
     const native=o.states();assert.equal(c.anm_manager_count(manager),native.length,label+' ANM count');for(const v of native){const cp=c.anm_manager_find(manager,v.id);assert.ok(cp,label+' missing VM '+v.id);assert.equal(firstDifference(normalizeAnimation(memory(c,cp,0x434),read,read(cp+0x3a4)),normalizeAnimation(m.bytes(v.p,0x434),p=>m.u32(p),m.u32(v.p+0x3a4))),'',label+' ANM '+v.id);++checks;}
     for(const vis of [0,1])assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(manager,vis),8)),Buffer.from(m.bytes(vis?0x4c2ef8:0x4c2f00,8)),label+' RNG');
     assert.deepEqual(Array.from({length:c.dialog_events(f)},(_,i)=>read(c.dialog_data(f,2)+i*4)),events.map(v=>v>>>0),label+' outputs');
    }
    try{
     events=[];assert.equal(c.dialog_begin(f,id,char,st),1);m.reg('EDI',state);m.call(0x41cfe0,{args:[nm+begin]});m.i32(state,id);compare(`init ${st}/${shot}/${id}`);
     let terminal=false;
     for(let frame=0;frame<6000;++frame){
      const held=skip?0x200:0,pressed=!skip&&frame%7===0?1:0;
      // Keep a native stage-result boundary so executing an unrelated results
      // implementation cannot silently become a fake dialogue success.
      events=[];stageBoundary=false;m.u32(0x4c93c0,held);m.u32(0x4c93cc,pressed);let nr;try{nr=m.call(0x41d380,{args:[state]});}catch(error){if(!stageBoundary)throw error;}
      const cr=c.dialog_tick(f,held,pressed);
      if(stageBoundary){assert.ok(stageEnd);assert.equal(cr,-2);compare(`stage completion boundary ${st}/${shot}/${id}`);m.resetThreadFPU();terminal=true;break;}
      if(cr<0)assert.fail(`dialogue ${st}/${shot}/${id}/${frame}: `+new TextDecoder().decode(memory(c,c.dialog_data(f,3),128)).split('\0')[0]);
      assert.equal(cr,nr===0xffffffff?1:0,`result ${st}/${shot}/${id}/${frame}`);if(cr){m.reg('ESI',state);m.call(0x419f80);}
      compare(`${st}/${shot}/${id}/${skip}/${frame}`);++frames;
      assert.equal(c.anm_manager_update(manager,0),1);o.update(false);if(cr){terminal=true;break;}
     }
     assert.ok(terminal,`did not terminate ${st}/${shot}/${id}`);++entries;
    }finally{c.dialog_delete(f);}
   }
  }
 }
 report('dialogue',{passed:true,entries,frames,checks,scope:'Native MSG timers, skip/confirm, all shipped entry prefixes, ANM portrait families/expression sprites, decoded glyph requests and audio requests. Stage-completion opcode 21 and glyph/audio output are separate unimplemented boundaries.'});
 }finally{m.close();}
});
