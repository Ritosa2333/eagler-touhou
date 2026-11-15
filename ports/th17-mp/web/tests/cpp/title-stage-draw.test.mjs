import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,string,report} from './helpers.mjs';

test('Stage Practice draws the original six rows, records, locks and confirmation blink',async()=>{
 const c=await core(),m=await oracle(),a=c.anm_manager_create(),t=c.title_main_create(a),f=c.title_data_create(t);
 const owner=m.allocate(0x5e18),records=m.allocate(0x2d600),ascii=m.allocate(0x19270),block=c.allocate(10*0x4820+0x4b0),draws=[];
 const latin=p=>{const b=m.bytes(p,1024);return Buffer.from(b.subarray(0,b.indexOf(0))).toString('latin1');};
 m.u32(0x4b7678,ascii);m.u32(0x4b77dc,records);
 m.replace(0x4082b0,'capture original Stage Practice formatted text',()=>{
  const sp=m.reg('ESP'),font=m.u32(sp+4),position=m.u32(sp+8);let arg=sp+16;
  const text=latin(m.u32(sp+12)).replace(/%s|%\.8d/g,token=>{const value=token==='%s'?latin(m.u32(arg)):String(m.i32(arg)).padStart(8,'0');arg+=4;return value;});
  draws.push({text,color:m.u32(font+0x19214),position:[m.f32(position),m.f32(position+4),m.f32(position+8)],shadow:m.u32(font+0x19228)});return 0;
 });
 let cases=0,rows=0;
 try{
  const bytes=Buffer.alloc(10*0x4820+0x4b0);
  for(let loadout=0;loadout<9;loadout++)for(let difficulty=0;difficulty<5;difficulty++)for(let stage=0;stage<8;stage++){
   const at=loadout*0x4820+0x4698+(difficulty*8+stage)*8;bytes.writeInt32LE((loadout*5+difficulty)*1000000+stage*123456,at);bytes[at+5]=(loadout+difficulty+stage)%3?1:0;
  }
  memory(c,block,bytes.length).set(bytes);assert.equal(c.title_data_import(f,block,block+10*0x4820),1);m.write(records+8,bytes.subarray(0,10*0x4820));
  for(let loadout=0;loadout<9;loadout++)for(let difficulty=0;difficulty<4;difficulty++)for(let selected=0;selected<6;selected++)for(const [phase,age] of [[0,0],[1,20],[2,0],[2,9],[2,10],[2,40],[3,0],[3,1],[3,2],[3,3],[4,20]]){
   const d=new DataView(c.memory.buffer),field=k=>c.title_main_field(t,k);
   d.setInt32(field(7)+12,Math.floor(loadout/3),true);d.setInt32(field(7)+16,loadout%3,true);d.setInt32(field(7)+8,difficulty,true);d.setInt32(field(0),selected,true);d.setInt32(field(2),phase,true);d.setInt32(field(3)+4,age,true);
   m.i32(0x4b59f4,Math.floor(loadout/3));m.i32(0x4b59f8,loadout%3);m.i32(0x4b5a00,difficulty);m.i32(owner+0x24,selected);m.i32(owner+0x20,phase);m.i32(owner+0x2b0,age);
   draws.length=0;assert.equal(m.call(0x456cd0,{ecx:owner}),1);assert.equal(c.title_data_draw(f,3),1);
   assert.equal(c.title_data_caption_count(f),draws.length,`${loadout}/${difficulty}/${selected}/${phase}/${age} row count`);
   for(let i=0;i<draws.length;i++){
    const style=c.title_data_caption_style(f,i),dv=new DataView(c.memory.buffer);assert.equal(string(c,c.title_data_caption(f,i)),draws[i].text);assert.equal(dv.getUint32(style,true),draws[i].color);assert.equal(dv.getUint8(style+28),draws[i].shadow);assert.deepEqual(Array.from(new Float32Array(c.memory.buffer,c.title_data_caption_position(f,i),3)),draws[i].position);rows++;
   }cases++;
  }
  report('title-stage-draw',{passed:true,cases,rows,originalFunctions:['0x456cd0'],scope:'Original formatted queue versus production six-row Stage Practice text, positions, high scores, all loadouts/difficulties, locked rows, selected colors, entry delay and confirmation blink.'});
 }finally{c.deallocate(block);c.title_data_delete(f);c.title_main_delete(t);c.anm_manager_delete(a);m.close();}
});
