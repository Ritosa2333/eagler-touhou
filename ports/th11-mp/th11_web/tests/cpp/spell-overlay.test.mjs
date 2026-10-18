import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,root,report} from './helpers.mjs';

test('TH11 battle spell overlay matches original 40c4a0 ASCII requests',async()=>{
 const c=await core(),m=await oracle(),raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
 const data=c.allocate(raw.length),src=c.resources_create(),session=c.game_session_create(),ascii=c.ascii_create(),name=c.allocate(16);
 memory(c,data,raw.length).set(raw);memory(c,name,6).set(Buffer.from('Spell\0'));
 const spell=m.allocate(0x920),supervisor=m.allocate(0x184c0),title=m.allocate(0x434),scores=m.allocate(0x30000);
 m.u32(0x4a8d58,supervisor);m.u32(0x4a8ebc,scores);
 m.u32(supervisor+0x18480,0xffffffff);m.f32(supervisor+0x18484,1);m.f32(supervisor+0x18488,1);
 let present=true,checks=0;
 m.replace(0x451ef0,'background rendering tested separately',()=>0);
 m.replace(0x4561e0,'title lookup boundary',()=>present?title:0,1);
 m.replace(0x45fc2c,'security cookie check',()=>0);
 m.replace(0x45fdd2,'platform integer formatter; preserve native va_list',()=>{
  const sp=m.reg('ESP'),out=m.u32(sp+4);let args=m.u32(sp+12);
  const value=m.string(m.u32(sp+8)).replace(/%\.(\d+)d/g,(_,precision)=>{const v=m.i32(args);args+=4;return (v<0?'-':'')+String(Math.abs(v)).padStart(Number(precision),'0');});
  m.write(out,Buffer.from(value+'\0'));return value.length;
 });
 const view=()=>new DataView(c.memory.buffer),put=(p,v)=>view().setUint32(p,v>>>0,true);
 const cstr=p=>Buffer.from(memory(c,p,256)).toString().split('\0')[0];
 const nstr=p=>Buffer.from(m.bytes(p,256)).toString().split('\0')[0];
 try{
  assert.equal(c.resources_open(src,data,raw.length),1);
  assert.equal(c.game_session_begin(session,src,1,0,0,1,0),1);
  const manager=c.battle_spell_setup(session,0);
  assert.equal(c.battle_spell_begin(session,0,2400,name),1);
  const titles=c.battle_spell_data(session,2),titleId=view().getUint32(titles+8,true),vm=c.anm_manager_find(manager,titleId);
  assert.ok(vm);
  for(let selection=0;selection<6;++selection)for(const id of [0,4,100,174])for(const flags of [0,1,3,7,11])for(const alpha of [0,64,255]){
   const captures=id?123:0,attempts=id?12345:1,bonus=id?98765430:1000000;
   c.battle_spell_record(session,selection,id,captures,attempts);
   put(c.battle_spell_data(session,4),flags);put(c.battle_spell_data(session,6),bonus);put(vm+0x374,(alpha<<24)|0xabcdef);
   m.u32(spell+0x8e0,flags);m.i32(spell+0x8dc,id);m.i32(spell+0x8e4,bonus);m.u32(spell+0x880,titleId);m.u32(title+0x374,(alpha<<24)>>>0);
   m.i32(0x4a5710,Math.floor(selection/3));m.i32(0x4a5714,selection%3);
   const record=scores+selection*0x68d4+id*0x90;m.i32(record+0x6ec,captures);m.i32(record+0x6f0,attempts);
   m.u32(supervisor+0x1847c,0);m.reg('EAX',spell);m.call(0x40c4a0);
   const count=c.battle_spell_ascii(session,ascii);assert.equal(count,m.u32(supervisor+0x1847c));
   for(let i=0;i<count;++i){
    const n=supervisor+0x87c+i*0x130,p=c.ascii_request(ascii,i,1),s=c.ascii_request(ascii,i,2);
    assert.equal(cstr(c.ascii_request(ascii,i,0)),nstr(n));
    for(let j=0;j<3;++j)assert.equal(view().getFloat32(p+4*j,true),m.f32(n+0x100+4*j));
    assert.equal(view().getUint32(s,true),m.u32(n+0x10c));
    assert.equal(view().getFloat32(s+4,true),m.f32(n+0x110));assert.equal(view().getFloat32(s+8,true),m.f32(n+0x114));
    assert.equal(view().getInt32(s+12,true),m.i32(n+0x120));assert.equal(view().getInt32(s+20,true),m.i32(n+0x128));
   }
   ++checks;
  }
  present=false;put(titles+8,0x7fffffff);put(c.battle_spell_data(session,4),3);
  m.u32(spell+0x8e0,3);m.u32(spell+0x880,0x7fffffff);m.u32(supervisor+0x1847c,0);m.reg('EAX',spell);m.call(0x40c4a0);
  assert.equal(c.battle_spell_ascii(session,ascii),0);assert.equal(view().getUint32(titles+8,true),m.u32(spell+0x880));
  report('spell-overlay',{passed:true,checks:checks+1,scope:'Actual GameBattle Spell render callback vs original 40c4a0/4015a0/4014e0: six shots, spell IDs 0/4/100/174, bonus/failed/inactive/survival/obscured states, alpha, positions, font/pass, capture history, expired title. Platform CRT formatter substituted; background rasterization separate.'});
 }finally{c.ascii_delete(ascii);c.game_session_delete(session);c.resources_delete(src);c.release(data);c.release(name);m.close();}
});
