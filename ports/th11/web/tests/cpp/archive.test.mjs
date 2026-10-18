import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';import {resolve,dirname} from 'node:path';
import {core,oracle,memory,root,report,sha} from './helpers.mjs';
test('Every TH11 resource matches original cipher and LZSS execution',async()=>{
 const c=await core(),m=await oracle(),raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),a=c.archive_create(),p=c.allocate(raw.length),out=c.allocate(64*1024*1024),input=m.allocate(16*1024*1024),dest=m.allocate(32*1024*1024),heap=m.heap;
 const ciphers=JSON.parse(readFileSync(resolve(root,'reference/archive-ciphers.json'),'utf8')).entries,resources=[];let total=0;
 try{memory(c,p,raw.length).set(raw);assert.equal(c.archive_open(a,p,raw.length),1,'THA1 index');
 for(let i=0;i<c.archive_count(a);++i){
  const n=c.archive_name(a,i),b=memory(c,n,1024),name=Buffer.from(b.subarray(0,b.indexOf(0))).toString();const offset=c.archive_field(a,i,0),size=c.archive_field(a,i,1),packed=c.archive_field(a,i,2);assert.ok(packed<16*1024*1024&&size<32*1024*1024,name);
  const cipher=ciphers[[...Buffer.from(name)].reduce((a,b)=>(a+b)&255,0)&7];m.heap=heap;m.write(input,raw.subarray(offset,offset+packed));m.reg('EAX',cipher.key);m.call(0x4581c0,{args:[input,packed,cipher.step,cipher.block,cipher.limit],limit:100000000});
  let original=input;if(size!==packed){m.reg('EAX',size);original=m.call(0x4426c0,{args:[input,packed,dest],limit:2000000000});}
  const length=c.archive_read(a,i,out,64*1024*1024);assert.equal(length,size,name);const bytes=Buffer.from(memory(c,out,length));assert.deepEqual(bytes,Buffer.from(m.bytes(original,size)),name);
  const path=resolve(root,'reference/assets',name),base=resolve(root,'reference/assets');assert.ok(path.startsWith(base+'\\')||path.startsWith(base+'/'));mkdirSync(dirname(path),{recursive:true});writeFileSync(path,bytes);resources.push({name,bytes:length,sha256:sha(bytes)});total+=length;if(i%20===0)console.log({resource:i,total:c.archive_count(a),name});
 }report('archive',{passed:true,entries:resources.length,totalBytes:total,sourceSha256:sha(raw),resources});writeFileSync(resolve(root,'reference/archive-manifest.json'),JSON.stringify(resources,null,2)+'\n');
 }finally{c.archive_delete(a);c.release(p);c.release(out);m.close();}
});
