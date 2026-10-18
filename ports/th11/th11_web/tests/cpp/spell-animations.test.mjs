import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,root,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';
test('TH11 spell presentation executes the original ANM files in native order',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(raw.length),src=c.resources_create(),s=c.game_session_create(),cn=c.allocate(64);
 memory(c,data,raw.length).set(raw);assert.equal(c.resources_open(src,data,raw.length),1);memory(c,cn,11).set(Buffer.from('Spell test\0'));
 const ascii=o.load('ascii.anm').original,bullet=o.load('bullet.anm',true).original,text=o.load('text.anm',true).original;
 const front=o.load('front.anm',true).original,hud=m.allocate(0x4460);m.u32(front,5);m.u32(hud+0x444c,front);m.u32(0x4a8d84,hud);
 const files=Array.from({length:7},(_,i)=>o.load(`stgenm${String(i+1).padStart(2,'0')}.anm`,true).original);
 for(const [p,id]of [[ascii,2],[bullet,6],[text,0],...files.map(p=>[p,9])])m.view(p,2).set([id,0]);
 const spell=m.allocate(0x920),stage=m.allocate(0x3000),boss=m.allocate(0x2700),enemies=m.allocate(0x60),player=m.allocate(0x900),bomb=m.allocate(0x50),replay=m.allocate(0x20),scores=m.allocate(0x30000),am=m.allocate(0x18500),bm=m.allocate(0x46d680),name=m.allocate(64);
 for(const [address,p]of [[0x4a8d6c,spell],[0x4a8d60,stage],[0x4a8d7c,enemies],[0x4a8eb4,player],[0x4a8d64,bomb],[0x4a8eb8,replay],[0x4a8ebc,scores],[0x4a8d58,am],[0x4a8d68,bm]])m.u32(address,p);
 m.u32(am+0x184ac,ascii);m.u32(bm+0x46d674,bullet);m.u32(0x4c3808,text);m.u32(enemies+0x1c,boss);m.i32(replay+16,1);m.write(name,Buffer.from('Spell test\0'));
 m.replace(0x454d00,'isolate glyph pixels',()=>0);m.replace(0x44a1e0,'isolate audio',()=>0);
 const heap=m.heap,view=()=>new DataView(c.memory.buffer),read=p=>view().getUint32(p,true);let manager=0,checks=0,frames=0;
 const normalize=(input,get)=>{const b=Buffer.from(input);for(const off of [0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0])b.writeUInt32LE(0,off);
  for(const off of [4,16,0x1c]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE(get(p),off);}for(const off of [8,12,20,24]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE(get(get(p)),off);}
  const base=b.readUInt32LE(0x3a4);for(const off of [0x394,0x3a4,0x3a8]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE((p-base)>>>0,off);}for(const off of [0x3ac,0x400,0x410,0x414,0x428,0x42c])b.writeUInt32LE(b.readUInt32LE(off)?1:0,off);return b;};
 const cmp=(cp,np,label)=>{const a=normalize(memory(c,cp,0x434),read),b=normalize(m.bytes(np,0x434),p=>m.u32(p));if(!a.equals(b)){let off=0;while(a[off]===b[off])++off;off&=~3;assert.fail(`${label} +0x${off.toString(16)} cpp=${a.readUInt32LE(off).toString(16)} native=${b.readUInt32LE(off).toString(16)}`);}++checks;};
 const compare=label=>{const states=o.states();assert.equal(c.anm_manager_count(manager),states.length,label+' active');for(const v of states){const p=c.anm_manager_find(manager,v.id);assert.ok(p,`missing ${v.id}`);cmp(p,v.p,`${label} vm ${v.id}`);}for(let j=0;j<2;++j)cmp(c.battle_spell_data(s,j),spell+0x10+j*0x434,label+' bg '+j);for(let j=0;j<2;++j)assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(manager,j),8)),Buffer.from(m.bytes(j?0x4c2ef8:0x4c2f00,8)),label+' rng');};
 try{
  for(const [st,section,id]of [[1,0,0],[1,0,4],[2,0,20],[3,0,40],[4,0,60],[5,0,100],[5,0,122],[6,23,124],[6,24,130],[7,23,158],[7,24,164]]){
   assert.equal(c.game_session_begin(s,src,st,0,0,1,0),1);manager=c.battle_spell_setup(s,section);
   // Manager allocation ids remain monotonic after clear(), as native does.

   m.heap=heap;o.reset();m.view(hud+0x4350,0x70).fill(0);m.view(spell,0x920).fill(0);m.u32(enemies+0x48,files[st-1]);m.i32(0x4a5710,0);m.i32(0x4a5714,0);m.i32(0x4a5720,1);m.i32(0x4a5728,st);m.i32(0x4a5730,section);m.f32(boss+0x1070,-33.25);m.f32(boss+0x1074,47.125);m.f32(boss+0x1078,12.5);
   for(let j=0;j<2;++j){memory(c,c.anm_env_rng(manager,j),8).fill(0);view().setUint16(c.anm_env_rng(manager,j),12345,true);}
   assert.equal(c.battle_spell_begin(s,id,1800,cn),1);m.reg('EAX',name);m.call(0x40c650,{args:[id,1800]});const baseId=read(c.battle_spell_data(s,2))-m.u32(spell+0x878);if(baseId){o.reset();m.u32(o.manager+0x7bd888,baseId);m.view(spell,0x920).fill(0);m.reg('EAX',name);m.call(0x40c650,{args:[id,1800]});}compare(`start ${st}/${section}/${id}`);
   for(let frame=0;frame<145;++frame){const y=frame<125?80:160;m.f32(player+0x880,y);assert.equal(c.battle_spell_tick(s,y),1);m.reg('EDI',spell);m.call(0x40c0a0);o.update(false);compare(`${st}/${section}/${frame}`);++frames;}
   assert.equal(c.battle_spell_end(s),1);m.call(0x40cd60);compare('end');assert.equal(c.game_session_title(s),1);
  }
  report('spell-animations',{passed:true,checks,frames,scope:'All seven stages and phase-dependent spell backgrounds, title/circle ANM families, script RNG and interrupts. Glyph pixels and audio substituted; result HUD executes original code.'});
 }finally{c.game_session_delete(s);c.resources_delete(src);c.release(data);c.release(cn);m.close();}
});
