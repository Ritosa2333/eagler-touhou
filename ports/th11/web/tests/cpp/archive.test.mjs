import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve,dirname,relative,sep} from 'node:path';
import {core,oracle,memory,root,report,sha} from './helpers.mjs';

function resourceNames(directory,prefix='') {
 return readdirSync(directory,{withFileTypes:true}).flatMap(entry=>{
  const name=prefix?prefix+'/'+entry.name:entry.name,path=resolve(directory,entry.name);
  if(entry.isDirectory())return resourceNames(path,name);
  assert.ok(entry.isFile(),'accepted archive assets must be regular files: '+name);
  return [name.replaceAll(sep,'/')];
 }).sort();
}

test('Every TH11 archive entry matches the original decoder and accepted resource bytes',async()=>{
 const c=await core(),m=await oracle(),raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),
  ciphers=JSON.parse(readFileSync(resolve(root,'reference/archive-ciphers.json'),'utf8')).entries,
  acceptedNames=resourceNames(resolve(root,'reference/assets')),archive=c.archive_create(),packedInput=c.allocate(16*1024*1024),
  originalOutput=c.allocate(32*1024*1024),cppOutput=c.allocate(64*1024*1024),data=c.allocate(raw.length),heap=m.heap;
 let total=0;const entries=[];
 try {
  memory(c,data,raw.length).set(raw);assert.equal(c.archive_open(archive,data,raw.length),1,'THA1 index');
  const names=Array.from({length:c.archive_count(archive)},(_,i)=>{
   const p=c.archive_name(archive,i),b=memory(c,p,1024),end=b.indexOf(0);
   assert.notEqual(end,-1,'archive name must terminate');return Buffer.from(b.subarray(0,end)).toString();
  });
  assert.deepEqual([...names].sort(),acceptedNames,'accepted resource inventory');
  for(let i=0;i<c.archive_count(archive);++i){
   const name=names[i],offset=c.archive_field(archive,i,0),size=c.archive_field(archive,i,1),packed=c.archive_field(archive,i,2);
   assert.ok(!name.startsWith('/')&&!name.split(/[\\/]/).includes('..'),'unsafe archive path: '+name);
   assert.ok(packed<16*1024*1024&&size<32*1024*1024,name);
   const cipher=ciphers[[...Buffer.from(name)].reduce((sum,byte)=>(sum+byte)&255,0)&7];
   assert.ok(cipher,'archive cipher entry missing for '+name);
   m.heap=heap;m.write(packedInput,raw.subarray(offset,offset+packed));m.reg('EAX',cipher.key);
   m.call(0x4581c0,{args:[packedInput,packed,cipher.step,cipher.block,cipher.limit],limit:100000000});
   let original=packedInput;
   if(size!==packed){m.reg('EAX',size);original=m.call(0x4426c0,{args:[packedInput,packed,originalOutput],limit:2000000000});}
   const expected=Buffer.from(m.bytes(original,size)),length=c.archive_read(archive,i,cppOutput,64*1024*1024);
   assert.equal(length,size,name);
   const actual=Buffer.from(memory(c,cppOutput,length));assert.deepEqual(actual,expected,name+' original decoder');
   const accepted=readFileSync(resolve(root,'reference/assets',name));assert.deepEqual(actual,accepted,name+' accepted asset');
   entries.push({name,bytes:length,sha256:sha(actual)});total+=length;
  }
  report('archive-verification',{passed:true,entries:entries.length,totalBytes:total,sourceSha256:sha(raw),resources:entries,
   scope:'Read-only comparison of each candidate extraction against the original decoder and the accepted resource tree.'});
 }finally{c.archive_delete(archive);for(const p of [packedInput,originalOutput,cppOutput,data])c.release(p);m.close();}
});
