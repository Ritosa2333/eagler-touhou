import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
const bits=f=>{const b=Buffer.alloc(4);b.writeFloatLE(f);return b.readUInt32LE();};
const instruction=(op,args=[],refs=0,time=0,difficulty=255)=>{const b=Buffer.alloc(16+args.length*4);b.writeInt32LE(time);b.writeUInt16LE(op,4);b.writeUInt16LE(b.length,6);b.writeUInt16LE(refs,8);b[10]=difficulty;b[11]=args.length;args.forEach((v,i)=>b.writeUInt32LE(v>>>0,16+i*4));return b;};
test('TH11 ECL instruction execution matches the original dispatcher',async()=>{
 const c=await core(),m=await oracle(),cp=c.allocate(0x1024),np=m.allocate(0x1024),ci=c.allocate(4096),ni=m.allocate(4096),g=c.ecl_globals_create(),owner=m.allocate(0x1040),vtable=m.allocate(32);let checks=0,commands=0,commandResult=0;const opcodes=new Set();
 m.u32(owner,vtable);m.u32(owner+4,np);m.u32(vtable,m.registerImport({dll:'ecl-test',name:'command',argc:0,handler:()=>{++commands;return commandResult;}}));
 const put=(p,o,v)=>new DataView(c.memory.buffer).setUint32(p+o,v>>>0,true),get=(p,o)=>new DataView(c.memory.buffer).getUint32(p+o,true);
 const run=(op,{args=[],refs=0,values=[],base=64,top=values.length*8,frame=0,elapsed=1,mask=255,difficulty=255,yieldCommand=0,initial={}}={})=>{
  const first=instruction(op,args,refs,0,difficulty),script=Buffer.concat([first,instruction(0,[],0,1000000000)]),state=Buffer.alloc(0x1024);
  for(let i=8;i<0x1008;++i)state[i]=(i*31+7)&255;
  state.writeFloatLE(frame);state.writeUInt32LE(top,0x1008);state.writeUInt32LE(base,0x100c);state.writeUInt32LE(mask,0x101c);
  for(const [offset,value] of Object.entries(initial))state.writeUInt32LE(value>>>0,8+base+Number(offset));
  values.forEach(([type,value],i)=>{state[8+i*8]=type;state.writeUInt32LE(value>>>0,12+i*8);});
  m.write(np,state);memory(c,cp,0x1024).set(state);m.write(ni,script);memory(c,ci,script.length).set(script);m.u32(np+4,ni);put(cp,4,ci);m.u32(np+0x1014,owner);
  m.resetThreadFPU();m.reg('EAX',np);commandResult=yieldCommand;const before=commands,cppBefore=c.ecl_fixture_control(g,yieldCommand),native=m.call(0x45b860,{args:[bits(elapsed)],limit:100000})|0,actual=c.ecl_update(cp,g,elapsed),label=`op ${op} ${JSON.stringify({args,values,frame,mask})}`;
  assert.equal(actual,native,label+' return');assert.equal(get(cp,0),m.u32(np),label+' time');assert.equal(get(cp,4)?get(cp,4)-ci:0,m.u32(np+4)?m.u32(np+4)-ni:0,label+' ip');assert.deepEqual(Buffer.from(memory(c,cp+8,0x1008)),Buffer.from(m.bytes(np+8,0x1008)),label+' stack');assert.equal(c.ecl_fixture_control(g,yieldCommand)-cppBefore,commands-before,label+' commands');opcodes.add(op);++checks;
 };
 try{
  const integerOps=[50,52,54,56,58,59,61,63,65,67,69,73,74,75,76,77],floatOps=[51,53,55,57,60,62,64,66,68,70];
  let seed=12345;const rand=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed;};
  for(const op of integerOps)for(let i=0;i<160;++i){const a=(rand()%20000001)-10000000,b=(rand()%20000001)-10000000||1;run(op,{values:[[i&1?102:105,i&1?bits(a*.125):a],[i&2?102:105,i&2?bits(b*.125):b]]});}
  for(const op of floatOps)for(let i=0;i<192;++i){const a=(rand()|0),b=(rand()|0)||1;run(op,{values:[[i&1?102:105,i&1?bits(a*.001):a],[i&2?102:105,i&2?bits(b*.1):b]]});}
  for(const op of [71,72,84,85,79,80,88])for(let i=0;i<128;++i){let value=(rand()%100000)-50000;if(op===88)value=Math.abs(value);const type=i&1?102:105;run(op,{values:[[type,type===102?bits(value*.0125):value]]});}
  for(const op of [42,44])for(const ref of [0,1])for(let i=0;i<128;++i){const raw=rand();run(op,{args:[ref?(op===44?bits(12):12):raw],refs:ref,initial:{12:raw}});}
  for(const op of [43,45])for(let i=0;i<128;++i){const value=(rand()%100000)-50000,type=i&1?102:105;run(op,{args:[op===45?bits(16):16],refs:1,values:[[type,type===102?bits(value*.125):value]]});}
  for(let i=0;i<128;++i){run(78,{args:[8],refs:1,initial:{8:rand()}});run(83,{args:[i],frame:Math.fround(i*.375),elapsed:.75});run(82,{args:[bits(4)],refs:1,initial:{4:bits((i-64)*3.25)}});run(81,{args:[bits(0),bits(4),bits((i-64)*.125),bits(i*.375)],refs:3});run(86,{args:[bits(0),bits((i-64)*.125),bits(i*.375)],refs:1});run(87,{args:[bits(0),bits(i*.13),bits(i*.31-8),bits(i*.05-7)],refs:1,initial:{0:bits(i*.33-17)}});}
  for(let i=0;i<32;++i){run(40,{args:[i*4]});run(41,{base:16,top:52,initial:{32:8}});}
  for(const op of [0,1,42,400])for(const mask of [0,1,2,255])for(const elapsed of [0,.25,1,1.5])run(op,{args:op===42?[123]:[],mask,difficulty:1,elapsed});
  run(400,{yieldCommand:-1});run(0,{frame:-1});run(1,{frame:-1});
  for(const op of [12,13,14])for(const value of [0,1,-1])run(op,{args:[24,0],values:op===12?[]:[[105,value]]});
  run(10,{base:0,top:4,initial:{0:0}});
  report('ecl-interpreter',{passed:true,checks,opcodes:[...opcodes].sort((a,b)=>a-b),scope:'scalar arithmetic, stack/variables, jump/return, timing/difficulty, geometry math and command yielding; subroutine and thread tests are separate'});
 }finally{c.release(cp);c.release(ci);c.ecl_globals_delete(g);m.close();}
});
