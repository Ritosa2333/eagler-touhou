import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
test('Whole-run replay writer preserves native stage headers, chunk streams, metadata and RNG',async()=>{
 const m=await oracle(),c=await core(),fixture=c.replay_archive_create(),input=c.allocate(264),rng=c.allocate(8),name=c.allocate(32),date=c.allocate(32),out=c.allocate(2000000),decoded=c.replay_create();
 const manager=m.allocate(0x164),config=m.allocate(204),scores=[m.allocate(160),m.allocate(160)],path=m.allocate(64),nativeName=m.allocate(32);
 const arg=i=>m.u32(m.reg('ESP')+4+i*4),u16=(p,n)=>m.write(p,new Uint8Array(new Uint16Array([n]).buffer));
 const error=()=>{const b=memory(c,c.replay_archive_error(fixture),256);return new TextDecoder().decode(b.subarray(0,b.indexOf(0)));};
 m.replace(0x47b24e,'archive-new',()=>{const n=arg(0),p=m.allocate(n);m.view(p,n).fill(0);return p;});
 m.replace(0x47b249,'archive-delete',()=>0);
 const thread=m.allocate(256);m.view(thread,256).fill(0);m.u32(thread+0x64,m.u32(0x4a6eec));m.replace(0x47faf0,'archive-CRT-thread',()=>thread);
 // Formatting is a platform CRT boundary; preserve CP932 bytes and the
 // original format/argument sequence while keeping file construction native.
 const nativeString=p=>{const bytes=m.bytes(p,1024);return Buffer.from(bytes.subarray(0,bytes.indexOf(0))).toString('latin1');};
 m.replace(0x47c335,'archive-vsprintf',()=>{let args=arg(2);const formatted=nativeString(arg(1)).replace(/%(?:1\.1)?[sf]/g,token=>{if(token.endsWith('s')){const text=nativeString(m.u32(args));args+=4;return text;}const b=m.bytes(args,8),value=new DataView(b.buffer,b.byteOffset,8).getFloat64(0,true);args+=8;return value.toFixed(1);});m.write(arg(0),Buffer.from(formatted+'\0','latin1'));return formatted.length;});
 let writes=[];for(const e of m.importMap.values()){ 
  if(e.name==='CreateFileA'){e.handler=()=>123;e.argc=7;}
  if(e.name==='WriteFile'){e.handler=()=>{writes.push(Buffer.from(m.bytes(arg(1),arg(2))));m.u32(arg(3),arg(2));return 1;};e.argc=5;}
  if(e.name==='CloseHandle'){e.handler=()=>1;e.argc=1;}
 }
 m.write(path,Buffer.from('replay/th9_01.rpy\0'));m.write(nativeName,Buffer.from('Tester  \0'));memory(c,name,32).set(Buffer.from('Tester  \0'));memory(c,date,32).set(Buffer.from('09/20\0'));m.write(0x4ac879,Buffer.from('09/20\0'));
 let frames=0,files=0;const stageLengths=[3640,109,763];
 try{
  for(const mode of [0,1,2]){
   m.view(manager,0x164).fill(0);m.u32(0x4a7e78,config);m.u32(0x4a7dac,scores[0]);m.u32(0x4a7de4,scores[1]);
   const settings=Uint8Array.from({length:204},(_,i)=>(i*13+mode)&255);m.write(config,settings);
   const meta=c.replay_archive_metadata(fixture);memory(c,meta,224).fill(0);memory(c,meta,204).set(settings);let v=new DataView(c.memory.buffer);v.setUint32(meta+204,0x45328712,true);v.setUint32(meta+208,0x98263417,true);memory(c,meta+212,8).set([mode,1,mode===1?4:2,10,7,0,1,0]);c.replay_archive_begin(fixture);
   m.u32(0x4b38a4,0x45328712);m.u32(0x4b38a0,0x98263417);m.u32(0x4a7ea8,mode);m.u32(0x4a7ea4,1);m.u32(0x4a7eac,mode===1?4:2);m.u32(0x4a8108,10);m.u32(0x4a810c,7);m.u32(0x4a7dc0,0);m.u32(0x4a7df8,1);
   const indices=mode===2?[9]:[0,1,2];
   for(let si=0;si<indices.length;++si){const stage=indices[si],round=c.replay_archive_round(fixture);memory(c,round,256).fill(0);v=new DataView(c.memory.buffer);m.u32(0x4a7e8c,stage);u16(0x4a80d6,0x7351+si);m.u32(0x4a7e84,si+2);m.u32(0x4a7ec8,0xffffffff);
    for(let side=0;side<2;++side){const ch=side?si+3:mode+1,points=si*76543+side*1357,life=si+2+side*.5;v.setUint32(round+side*20,points,true);v.setUint16(round+side*20+4,0x7351+si,true);memory(c,round+side*20+6,7).set([ch,side,Math.trunc(life),255,si+4,0,si+2]);v.setUint32(round+side*20+16,si,true);
     const global=0x4a7db0+side*56;m.u32(global,ch);m.u32(global+8,side);m.u32(global+12,si+4);m.view(scores[side],160).fill(0);m.f32(scores[side],life);m.u32(scores[side]+8,points);m.u32(scores[side]+16,si);
    }
    assert.equal(c.replay_archive_stage(fixture,stage),1);assert.equal(m.call(0x421140,{ecx:manager,limit:10000000}),0);
    const header=m.u32(manager+8);for(let group=0;group<3;++group){const h=m.u32(header+32+(group*10+stage)*4);assert.ok(h);}
    const length=stageLengths[si];for(let frame=0;frame<length;++frame){const flags=frame>=length-5?0x204:4,paused=frame%199===0,rate=frame%47===0?0xb8:60;m.u32(0x4a7ec4,flags);m.u32(0x4b36d4,paused?8:0);u16(0x4b36c8,rate);
     for(let side=0;side<3;++side){const held=(frame*53+side*17)&0x10ff;new DataView(c.memory.buffer).setUint16(input+side*88+44,held,true);u16(0x4ace18+side*0x8e+44,held);}
     c.replay_archive_record(fixture,flags,paused,input,2,rate);m.call(0x4202a0,{ecx:manager});++frames;
    }
   }
   for(const seed of [0x1234,0xffff]){memory(c,rng,8).fill(0);new DataView(c.memory.buffer).setUint16(rng,seed,true);u16(0x4ace0c,seed);m.u32(0x4ace10,0);writes=[];
    const size=c.replay_archive_finish(fixture,rng,name,date,mode+1,mode===2?3:5,out);assert.equal(error(),'');assert.ok(size);m.call(0x420c60,{ecx:manager,edx:path,args:[nativeName],limit:200000000});const native=Buffer.concat(writes),actual=Buffer.from(memory(c,out,size));
    // USER trailer bytes 9..11 are unspecified stack padding in the original.
    const nativeEnd=native.readUInt32LE(12),actualEnd=actual.readUInt32LE(12);native.fill(0,nativeEnd+9,nativeEnd+12);actual.fill(0,actualEnd+9,actualEnd+12);
    const unpack=file=>{memory(c,out,file.length).set(file);assert.equal(c.replay_decode(decoded,out,file.length),1);return Buffer.from(memory(c,c.replay_data(decoded),c.replay_size(decoded)));};
    const actualPlain=unpack(actual),nativePlain=unpack(native),differences=[];for(let j=24;j<Math.max(actualPlain.length,nativePlain.length);++j)if(actualPlain[j]!==nativePlain[j]){differences.push(j.toString(16)+':'+actualPlain[j]+'/'+nativePlain[j]);if(differences.length>30)break;}
    assert.equal(actual.length,native.length,'file length '+differences.join(','));assert.deepEqual(actual,native,'native replay bytes mode '+mode+' seed '+seed);assert.deepEqual(memory(c,rng,8),m.bytes(0x4ace0c,8),'random calls');
    const p=m.allocate(actual.length);m.write(p,actual);assert.ok(m.call(0x4205e0,{ecx:p,edx:actual.length,limit:200000000}),'original reader accepts C++ replay');++files;
   }
  }
  report('replay-archive',{files,frames,modes:3,stages:7,scope:'Original whole-file writer byte comparison, including 3598-frame chunk boundary, three ending frames, all input groups, native CP932 USER trailer, metadata and encryption/compression. Only uninitialized USER padding is normalized; original decoder accepts authored files.'});
 }finally{c.replay_archive_delete(fixture);c.replay_delete(decoded);for(const p of [input,rng,name,date,out])c.release(p);m.close();}
});
