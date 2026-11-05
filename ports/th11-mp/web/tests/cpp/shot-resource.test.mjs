import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,root,report} from './helpers.mjs';
test('TH11 six SHT resources and every firing group match the original loader and selection',async()=>{
 const c=await core(),m=await oracle(),player=m.allocate(0x8d40),sht=c.sht_create();m.view(player,0x8d40).fill(0);
 let source,base,emitted=[],checks=0,records=0;
 m.replace(0x458400,'SHT input',()=>{base=m.allocate(source.length);m.write(base,source);return base;},2);
 m.replace(0x433f90,'record selected shot',()=>{emitted.push(m.reg('EAX')-base);return 0;},2);
 const spawn=[0,0x434e20,0x435210,0x435250],update=[0,0x434e30,0x4352a0,0x435330];
 try{for(const [k,name]of ['pl00a','pl00b','pl00c','pl01a','pl01b','pl01c'].entries()){
  source=readFileSync(resolve(root,'reference/assets',name+'.sht'));const data=c.allocate(source.length);memory(c,data,source.length).set(source);
  assert.equal(c.sht_open(sht,data,source.length),1,name);m.reg('ESI',player);assert.equal(m.call(0x431c70),0);
  assert.deepEqual(Buffer.from(memory(c,c.sht_header(sht),0x268)),Buffer.from(m.bytes(base,0x268)));
  const groups=source.readUInt16LE(2),maximum=source.readInt32LE(0x20),step=source.readInt32LE(0x24);
  for(let g=0;g<groups;++g){let at=source.readUInt32LE(0x268+g*8),count=0;while(source.readInt8(at)>=0){
   const expected=Buffer.from(m.bytes(base+at,0x34));expected.writeUInt32LE(spawn.indexOf(expected.readUInt32LE(0x24)),0x24);expected.writeUInt32LE(update.indexOf(expected.readUInt32LE(0x28)),0x28);
   assert.deepEqual(Buffer.from(memory(c,c.sht_shots(sht,g)+count*0x34,0x34)),expected,`${name} ${g} ${count}`);++count;++records;at+=0x34;
  }assert.equal(c.sht_count(sht,g),count);}
  const character=Math.floor(k/3),subtype=k%3;
  m.i32(0x4a5710,character);m.i32(0x4a5714,subtype);m.i32(0x4a574c,step);
  for(let power=0;power<=maximum*step;++power)for(let focus=0;focus<2;++focus)for(let mode=0;mode<(k===4?5:1);++mode){
   m.i32(0x4a56e8,power);m.i32(player+0x8d20,focus);m.i32(player+0x8bac,mode);
   const group=c.sht_group(sht,power,step,character,subtype,focus,mode),start=source.readUInt32LE(0x268+group*8);
   for(const frame of [-1,0,1,2,3,6,13,14,29,30]){emitted=[];m.reg('EDI',player);m.reg('EBX',frame);m.call(0x4342f0);
    const actual=[];for(let n=0;n<c.sht_count(sht,group);++n)if(c.sht_due(c.sht_shots(sht,group)+n*0x34,frame))actual.push(start+n*0x34);
    assert.deepEqual(actual,emitted,`${name} power${power} focus${focus} mode${mode} frame${frame}`);++checks;
   }
  }
  const malformed=Buffer.from(source);malformed[malformed.readUInt32LE(0x268)]=0;memory(c,data,source.length).set(malformed);assert.equal(c.sht_open(sht,data,source.length),0,'zero interval rejected');assert.equal(c.sht_count(sht,0)>0,true,'failed load keeps prior resource');
  for(const length of [0,0x267,0x270,source.readUInt32LE(0x268)+20])assert.equal(c.sht_open(sht,data,length),0,'truncated resource rejected');
  c.release(data);
 }
 report('shot-resource',{passed:true,resources:6,records,checks,scope:'Original SHT loader relocation/callback resolution, all six resource records, every valid power/focus/weapon mode and ten firing phases. Shot construction is tested separately.'});
 }finally{c.sht_delete(sht);m.close();}
});
test('TH11 firing schedule preserves hold/release, warp, pause and fractional timer behavior',async()=>{
 const c=await core(),m=await oracle(),p=m.allocate(0x8d40),timer=c.allocate(20),rate=c.allocate(4);let expected=-1,checks=0;
 m.replace(0x4342f0,'record firing frame',()=>{expected=m.reg('EBX')|0;return 0;});
 try{for(const speed of [1,.5,1.5,.99,1.01,.25]){
  m.view(p,0x8d40).fill(0);memory(c,timer,20).fill(0);const v=()=>new DataView(c.memory.buffer);v().setFloat32(rate,speed,true);m.f32(0x4a7948,speed);c.timer_set(timer,-1,rate);m.reg('EAX',p+0x930);m.call(0x406100,{args:[-1]});
  for(let frame=0;frame<420;++frame){const state=frame%113<8?2:1,warp=frame%83<9?99:0,held=frame%41<21?1:0;
   m.i32(p+0x928,state);m.i32(p+0x920,warp);m.u32(0x4c93c0,held);m.reg('EAX',p);expected=-1;m.call(0x434380);
   assert.equal(c.shot_schedule(timer,state,warp,held,rate),expected,`frame${frame} speed${speed}`);
   const b=Buffer.from(memory(c,timer,20)),a=Buffer.from(m.bytes(p+0x930,20));a.writeUInt32LE(0,12);b.writeUInt32LE(0,12);assert.deepEqual(b,a);++checks;
  }
 }report('shot-schedule',{passed:true,checks,scope:'Original player firing timer and emitted frame for six rates, held/released fire, warp and non-active states.'});
 }finally{c.release(timer);c.release(rate);m.close();}
});
