import {WASI} from 'node:wasi';
import {readFile,writeFile} from 'node:fs/promises';
import assert from 'node:assert/strict';
const instances={};
for(const key of ['baseline','optimized']){
 const wasi=new WASI({version:'preview1',args:[],env:{}});
 const {instance}=await WebAssembly.instantiate(await readFile(new URL(key+'.wasm',import.meta.url)),wasi.getImportObject());
 wasi.initialize(instance);instances[key]=instance.exports.run;
}
const cases=[];
for(let pattern=0;pattern<3;pattern++){
 const rows={baseline:[],optimized:[]};
 const queries=15000;
 assert.equal(instances.baseline(1000,pattern),instances.optimized(1000,pattern));
 for(let n=0;n<5;n++){
  const hashes=[];
  for(const key of n%2?['optimized','baseline']:['baseline','optimized']){
   const start=performance.now();
   hashes.push(instances[key](queries,pattern));
   rows[key].push(performance.now()-start);
  }
  assert.equal(hashes[0],hashes[1]);
 }
 const median=a=>a.sort((x,y)=>x-y)[2];
 cases.push({pattern,queries,activeSources:176,baselineMs:median(rows.baseline),optimizedMs:median(rows.optimized)});
}
console.log(JSON.stringify(cases));await writeFile(new URL('result.json',import.meta.url),JSON.stringify(cases,null,2)+'\n');