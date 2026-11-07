import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile,writeFile} from 'node:fs/promises';
import baseline from './baseline-codec.mjs';
import updated from './updated-codec.mjs';

const archive=await readFile(process.argv[2]);
const modules=await Promise.all([baseline(),updated()]);
const pointers=modules.map(module=>{
 const data=module._malloc(archive.length);
 module.HEAPU8.set(archive,data);
 return {data,input:module._malloc(16384),output:module._malloc(65536),history:module._history_bytes()};
});
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const animationBanks=[];
function readAll(i,hash){
 const mod=modules[i],count=mod._open_archive(pointers[i].data,archive.length),hashes=[];
 assert.equal(count,256);
 let bytes=0;
 for(let n=0;n<count;++n){
  const size=mod._read_archive(n);
  assert.ok(size>=0);
  bytes+=size;
  if(hash){
   const p=mod._output_bytes();
   hashes.push(sha(mod.HEAPU8.subarray(p,p+size)));
   const name=mod._archive_name(n),end=mod.HEAPU8.indexOf(0,name);
   if(i===0&&Buffer.from(mod.HEAPU8.subarray(name,end)).toString().endsWith('.anm'))
    animationBanks.push(mod.HEAPU8.slice(p,p+size));
  }
 }
 return {bytes,hashes};
}
const exact=readAll(0,true);
assert.deepEqual(readAll(1,true),exact);

let rng=0xb1c315;
function random(){rng^=rng<<13;rng^=rng>>>17;rng^=rng<<5;return rng>>>0;}
const history=Uint8Array.from({length:8192},()=>random()&255);
modules.forEach((mod,i)=>mod.HEAPU8.set(history,pointers[i].history));
const trials=3072;
for(let trial=0;trial<trials;++trial){
 let written=0,byte=0,left=8;
 const input=[];
 function bits(value,count){
  for(let bit=count-1;bit>=0;--bit){
   byte=byte*2+((value>>>bit)&1);
   if(--left===0){input.push(byte);byte=0;left=8;}
  }
 }
 const tokens=trial%16===0?1400:10+random()%100;
 for(let n=0;n<tokens;++n){
  if(random()%3===0){bits(1,1);bits(random(),8);++written;continue;}
  const distance=1+random()%(n%4===0?8192:24);
  const offset=(written+1-distance)&8191;
  const length=3+random()%16;
  bits(0,1);bits(offset||8191,13);bits(length-3,4);written+=length;
 }
 bits(0,14);
 if(left!==8)input.push(byte<<left);
 const capacity=trial%3===0?random()%(written+1):written;
 const size=trial%5===0?random()%input.length:input.length;
 const results=modules.map((mod,i)=>{
  const p=pointers[i];
  mod.HEAPU8.set(input,p.input);
  mod.HEAPU8.fill(0xaa,p.output,p.output+capacity+1);
  const ok=mod._decode(p.input,size,p.output,capacity);
  return {ok,written:mod._written_bytes(),result:mod.HEAPU8.slice(p.output,p.output+capacity+1),
   history:mod.HEAPU8.slice(p.history,p.history+8192)};
 });
 assert.deepEqual(results[1],results[0],`retained dictionary, output and status, trial ${trial}`);
}
const timings=[[],[]];
for(let trial=0;trial<9;++trial){
 for(const i of trial%2?[1,0]:[0,1]){
  const start=performance.now();
  assert.equal(readAll(i,false).bytes,exact.bytes);
  const elapsed=performance.now()-start;
  if(trial>=2)timings[i].push(elapsed);
 }
}
const summarize=values=>({trialsMs:values,medianMs:[...values].sort((a,b)=>a-b)[Math.floor(values.length/2)]});
function atlas(format,pattern){
 const bpp=[0,4,2,2,3,2,2,1][format],names=64+5*4+5*20,tx=names+4;
 const bytes=Buffer.alloc(tx+16+64*64*bpp);
 bytes.writeUInt32LE(8,0);bytes.writeUInt16LE(5,4);bytes.writeUInt16LE(64,10);bytes.writeUInt16LE(64,12);
 bytes.writeUInt16LE(1,14);bytes.writeUInt32LE(names,16);bytes.writeUInt32LE(tx,28);bytes[32]=1;
 for(let n=0;n<5;++n){
  const at=64+5*4+n*20;
  bytes.writeUInt32LE(at,64+n*4);bytes.writeUInt32LE(n,at);
  bytes.writeFloatLE(n===4?0:(n&1)*32,at+4);bytes.writeFloatLE(n===4?0:(n>>1)*32,at+8);
  bytes.writeFloatLE(n===4?64:32,at+12);bytes.writeFloatLE(n===4?64:32,at+16);
 }
 bytes.write('t',names);bytes.write('THTX',tx);bytes.writeUInt16LE(format,tx+6);
 bytes.writeUInt16LE(64,tx+8);bytes.writeUInt16LE(64,tx+10);bytes.writeUInt32LE(64*64*bpp,tx+12);
 for(let y=0;y<64;++y)for(let x=0;x<64;++x){
  const solid=[false,true,x===63&&y===63,x>=11&&x<24&&y>=11&&y<24,x===0||x===63,random()%100<3][pattern];
  const alphaMask=format===2?0x80:format===5?0xf0:0xff;
  const pixel=tx+16+(y*64+x)*bpp;
  for(let i=0;i<bpp;++i)bytes[pixel+i]=random()&255;
  bytes[pixel+bpp-1]=(bytes[pixel+bpp-1]&~alphaMask)|(solid?alphaMask:0);
 }
 return bytes;
}
const synthetic=[];
for(let format=1;format<=7;++format)for(let pattern=0;pattern<6;++pattern)synthetic.push(atlas(format,pattern));
const banks=[...animationBanks,...synthetic];
const uploaded=modules.map(mod=>banks.map(bytes=>{
 const p=mod._malloc(bytes.length);
 mod.HEAPU8.set(bytes,p);
 return [p,bytes.length];
}));
function scan(i,hash,limit=banks.length){
 const mod=modules[i],hashes=[];
 for(const [p,size] of uploaded[i].slice(0,limit)){
  const sprites=mod._open_animation(p,size);
  assert.ok(sprites>=0);
  if(hash){const bounds=mod._animation_bounds();hashes.push(sha(mod.HEAPU8.subarray(bounds,bounds+16*sprites)));}
 }
 return hashes;
}
assert.deepEqual(scan(1,true),scan(0,true),'original and synthetic atlas bounds');
const anmTimes=[[],[]];
for(let trial=0;trial<9;++trial)for(const i of trial%2?[1,0]:[0,1]){
 const start=performance.now();
 scan(i,false,animationBanks.length);
 if(trial>=2)anmTimes[i].push(performance.now()-start);
}
const report={archiveSha256:sha(archive),resourceCount:exact.hashes.length,decodedBytes:exact.bytes,
 resourcesByteIdentical:true,differentialTrials:trials,baseline:summarize(timings[0]),updated:summarize(timings[1]),
 animation:{originalBanks:animationBanks.length,syntheticBanks:synthetic.length,boundsByteIdentical:true,
  baseline:summarize(anmTimes[0]),updated:summarize(anmTimes[1])}};
await writeFile(new URL('./archive-bench.json',import.meta.url),JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify(report,null,2));
