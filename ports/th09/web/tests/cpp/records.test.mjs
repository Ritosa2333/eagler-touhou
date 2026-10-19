import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,root,target,memory,report} from './helpers.mjs';
test('Player records match original defaults, imports, rankings, unlocks and clocks',async()=>{
 const m=await oracle(),c=await core(),p=c.records_create(),input=c.allocate(1048576),output=c.allocate(1048576),rng=c.allocate(8),native=m.allocate(1048576),entry=m.allocate(44),ending=m.allocate(0x2a60);let checks=0,now=0,current;
 const nativeBlock=(address,length)=>new Uint8Array(m.bytes(address,length));
 const compare=label=>{for(const [kind,address,length] of [[1,0x4a8180,508],[2,0x4a8168,24],[3,0x4a8380,17600]]){assert.equal(c.records_write(p,output,kind,0),length);assert.deepEqual(memory(c,output,length),nativeBlock(address,length),label+' chapter '+kind);++checks;}};
 function load(plain){memory(c,input,plain.length).set(plain);assert.equal(c.records_load(p,input,plain.length,1),1);m.write(native,plain);m.view(0x4a8168,0x4ac884-0x4a8168).fill(0);for(const address of [0x4218e0,0x421aa0,0x421b80])m.call(address,{ecx:native});compare('load');}
 function serialize(){const length=c.records_write(p,output,0,0);return Buffer.from(memory(c,output,length));}
 const empty=Buffer.alloc(36);empty.writeUInt16LE(4,4);empty.writeUInt32LE(24,8);empty.writeUInt32LE(36,12);empty.writeUInt32LE(12,16);empty.write('TH9K',24);empty.writeUInt16LE(12,28);empty.writeUInt16LE(12,30);empty[32]=1;
 function find(bytes,name){for(let at=24;at<bytes.length;at+=bytes.readUInt16LE(at+4))if(bytes.toString('ascii',at,at+4)===name)return at;throw Error(name);}
 for(const imported of m.importMap.values())if(imported.name==='timeGetTime'){imported.handler=()=>now;imported.argc=0;}
 for(const address of [0x4015f0,0x40e390,0x40e370,0x40e380])m.replace(address,'ending-render-lifetime',()=>0);
 m.replace(0x422200,'ending-bgm',()=>0,1);m.replace(0x401560,'ending-subtitle-animation',()=>0,2);m.replace(0x40e4f0,'ending-script',()=>1,1);
 m.replace(0x42c5c0,'log',()=>0);m.replace(0x42c970,'read-score',()=>{const ptr=m.allocate(current.length);m.write(ptr,current);m.u32(m.reg('EDX'),current.length);return ptr;},1);
 try{
  load(empty);const initial=serialize();load(initial);
  const file=readFileSync(resolve(root,target.executable,'..','score.dat'));memory(c,input,file.length).set(file);assert.equal(c.records_load(p,input,file.length,0),1);current=file;const decoded=m.call(0x421c80,{ecx:0x2020000,limit:100000000});for(const address of [0x4218e0,0x421aa0,0x421b80])m.call(address,{ecx:decoded});compare('original score');load(serialize());
  let seed=0x79268f13;const next=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed;};
  for(let i=0;i<800;++i){const bytes=Buffer.alloc(44);bytes.write('HSCR');bytes.writeUInt16LE(44,4);bytes.writeUInt16LE(44,6);bytes[8]=2;for(let at=9;at<44;++at)bytes[at]=next()>>>24;bytes[20]=i%16;bytes[21]=Math.floor(i/16)%5;bytes[22]=next()%5;bytes.writeUInt32LE([0,20000,100000,0x7fffffff,0xffffffff,next()][i%6],12);m.write(entry,bytes);memory(c,input,44).set(bytes);assert.equal(c.records_insert(p,input),m.call(0x421800,{ecx:entry}),'insertion '+i);compare('insertion '+i);}
  load(initial);
  for(let character=0;character<16;++character){for(let difficulty=0;difficulty<5;++difficulty){for(let j=0;j<4;++j){m.call(0x40e3a0,{ecx:0x4a8180,args:[character,difficulty]});c.records_action(p,0,character,difficulty);}assert.equal(c.records_clear_count(p,character,difficulty),m.call(0x4234a7,{ecx:0x4a8180,args:[character,difficulty]}));}m.call(0x4158e0,{ecx:0x4a8180,args:[character]});c.records_action(p,1,character,0);assert.equal(c.records_clear_count(p,character,-1),m.call(0x4234a7,{ecx:0x4a8180,args:[character,0xffffffff]}));compare('counts '+character);}
  // Compare the original ending initializer's actual unlock decisions. Only its
  // graphics, music and script-loading platform boundaries are replaced.
  for(let scenario=0;scenario<80;++scenario){const data=Buffer.from(initial),at=find(data,'PLST');for(let ch=0;ch<16;++ch){for(let d=0;d<5;++d)data.writeUInt32LE(scenario<16?(ch<scenario?1:0):(next()%5===0?1:0),at+124+(ch*6+d)*4);data.writeUInt32LE(scenario&1?1:next()%2,at+124+(ch*6+5)*4);}load(data);const character=scenario%16,difficulty=scenario%5;m.u32(0x4a7ea8,0);m.u32(0x4a7db0,character);m.u32(0x4a7eac,difficulty);m.u32(0x4a7c80,0);m.view(ending,0x2a60).fill(0);m.call(0x40e580,{ecx:ending,limit:10000000});c.records_action(p,2,character,difficulty);compare('ending '+scenario);}
  const limit=Buffer.from(initial),at=find(limit,'PLST');for(let i=0;i<96;++i)limit.writeUInt32LE([999998,999999,1000000,0xffffffff][i%4],at+124+i*4);load(limit);for(let ch=0;ch<16;++ch){for(let d=0;d<5;++d){m.call(0x40e3a0,{ecx:0x4a8180,args:[ch,d]});c.records_action(p,0,ch,d);}m.call(0x4158e0,{ecx:0x4a8180,args:[ch]});c.records_action(p,1,ch,0);}compare('saturated counters');
  load(initial);m.u32(0x4b3898,0);m.u32(0x4b389c,0);for(now of [1,999,1001,60001,3600001,0xfffffff0,0xffffffff,0,1234,7202345]){m.call(0x42ff70,{ecx:0x4b3100});c.records_action(p,3,now,0);m.call(0x430060,{ecx:0x4b3100});c.records_action(p,4,now,0);compare('clocks '+now);}
  // Saved files are loaded by the original reader and its real chapter managers.
  for(const seed of [0,1,0x1234,0xffff]){memory(c,rng,8).fill(0);new DataView(c.memory.buffer).setUint16(rng,seed,true);const size=c.records_write(p,output,4,rng);current=Buffer.from(memory(c,output,size));const ptr=m.call(0x421c80,{ecx:0x2020000,limit:100000000});assert.ok(ptr);for(const address of [0x4218e0,0x421aa0,0x421b80])m.call(address,{ecx:ptr});compare('saved '+seed);}
  const before=serialize();for(const [offset,value] of [[20,16],[21,5],[22,5]]){const invalid=Buffer.from(before),hscr=find(invalid,'HSCR');invalid[hscr+offset]=value;memory(c,input,invalid.length).set(invalid);assert.equal(c.records_load(p,input,invalid.length,1),0);assert.deepEqual(serialize(),before,'invalid import preserves records');}
  report('player-records',{checks,rankingInsertions:800,endingUnlockScenarios:80,clockCases:10,nativeSaveAcceptance:4,scope:'400 default/imported ranking entries, native CP932 names and dates, native PLST/LSNM bytes, tie ordering, saturated counters, real ending unlock logic, both time clocks including wraparound, save.dat accepted by original reader. Browser persistence and campaign integration remain separate.'});
 }finally{c.records_delete(p);c.release(input);c.release(output);c.release(rng);m.close();}
});
