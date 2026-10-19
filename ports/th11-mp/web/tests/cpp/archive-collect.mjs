import assert from 'node:assert/strict';
import test from 'node:test';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {core,oracle,memory,root} from './helpers.mjs';

test('stage original archive resources into the maintenance output',async()=>{
const output=process.env.EAGLER_MAINTENANCE_OUTPUT;
assert.ok(output,'EAGLER_MAINTENANCE_OUTPUT is required for archive collection');
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
const ciphers=JSON.parse(readFileSync(resolve(root,'reference/archive-ciphers.json'),'utf8')).entries;
const c=await core(),m=await oracle(),archive=c.archive_create(),data=c.allocate(raw.length),
 input=c.allocate(16*1024*1024),decompressed=c.allocate(32*1024*1024),heap=m.heap;
const assets=resolve(output,'assets'),resources=[];mkdirSync(assets,{recursive:true});
try{
 memory(c,data,raw.length).set(raw);assert.equal(c.archive_open(archive,data,raw.length),1,'THA1 index');
 for(let i=0;i<c.archive_count(archive);++i){
  const p=c.archive_name(archive,i),nameBytes=memory(c,p,1024),end=nameBytes.indexOf(0);
  assert.notEqual(end,-1,'archive name must terminate');const name=Buffer.from(nameBytes.subarray(0,end)).toString();
  assert.ok(!name.startsWith('/')&&!name.split(/[\\/]/).includes('..'),'unsafe archive path: '+name);
  const offset=c.archive_field(archive,i,0),size=c.archive_field(archive,i,1),packed=c.archive_field(archive,i,2);
  assert.ok(packed<16*1024*1024&&size<32*1024*1024,name);
  const cipher=ciphers[[...Buffer.from(name)].reduce((sum,byte)=>(sum+byte)&255,0)&7];assert.ok(cipher,'archive cipher entry missing for '+name);
  m.heap=heap;m.write(input,raw.subarray(offset,offset+packed));m.reg('EAX',cipher.key);
  m.call(0x4581c0,{args:[input,packed,cipher.step,cipher.block,cipher.limit],limit:100000000});
  let original=input;if(size!==packed){m.reg('EAX',size);original=m.call(0x4426c0,{args:[input,packed,decompressed],limit:2000000000});}
  const bytes=Buffer.from(m.bytes(original,size)),path=resolve(assets,name);
  assert.ok(path.startsWith(assets+'\\')||path.startsWith(assets+'/'),'archive path escapes staging');
  mkdirSync(resolve(path,'..'),{recursive:true});writeFileSync(path,bytes);
  resources.push({name,bytes:bytes.length,sha256:digest(bytes)});
 }
 assert.ok(resources.length>0,'original archive contains resources');
 const manifest={schema:'eagler-touhou/original-archive-staging/1',game:'th11-mp',sourceSha256:digest(raw),entries:resources.length,
  totalBytes:resources.reduce((sum,entry)=>sum+entry.bytes,0),resources};
 writeFileSync(resolve(output,'archive-manifest.json'),JSON.stringify(manifest,null,2)+'\n');
 console.log(JSON.stringify({output,entries:resources.length,totalBytes:manifest.totalBytes,sourceSha256:manifest.sourceSha256}));
}finally{c.archive_delete(archive);for(const p of [data,input,decompressed])c.release(p);m.close();}
});
