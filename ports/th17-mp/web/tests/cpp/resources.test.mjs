import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {core,memory,string,sha,root,target,report} from './helpers.mjs';
test('C++ archive recovers every byte of all 256 original resources',async()=>{
 const c=await core(),manifest=JSON.parse(readFileSync(resolve(root,'reference/archive-manifest.json'))),bytes=readFileSync(resolve(dirname(resolve(root,target.executable)),'th17.dat'));
 assert.equal(sha(bytes),manifest.sourceSha256);const input=c.allocate(bytes.length),output=c.allocate(Math.max(...manifest.entries.map(x=>x.size))),archive=c.archive_create();
 try{
  memory(c,input,bytes.length).set(bytes);assert.equal(c.archive_open(archive,input,bytes.length),1);assert.equal(c.archive_count(archive),manifest.entries.length);
  for(let i=0;i<manifest.entries.length;i++){const e=manifest.entries[i];assert.equal(string(c,c.archive_name(archive,i)),e.name);for(const [j,k] of ['offset','size','compressed','reserved'].entries())assert.equal(c.archive_field(archive,i,j)>>>0,e[k]);assert.equal(c.archive_read(archive,i,output,e.size),e.size,e.name);assert.equal(sha(memory(c,output,e.size)),e.sha256,e.name);}
  report('archive',{passed:true,files:manifest.entries.length,totalBytes:manifest.totalBytes,archiveSha256:sha(bytes),baseline:'Independent JS decoder and pinned thtk format'});
 }finally{c.archive_delete(archive);c.deallocate(output);c.deallocate(input);}
});
test('TH17 animation, enemy-script and stage resources parse without skipped files',async()=>{
 const c=await core(),manifest=JSON.parse(readFileSync(resolve(root,'reference/archive-manifest.json'))),results=[];
 for(const e of manifest.entries.filter(e=>/\.(anm|ecl|std)$/.test(e.name))){const bytes=readFileSync(resolve(root,'reference/assets',e.name)),input=c.allocate(bytes.length),kind=e.name.endsWith('.anm')?'anm':e.name.endsWith('.ecl')?'ecl':'std',object=c[kind+'_create']();
  try{memory(c,input,bytes.length).set(bytes);assert.equal(c[kind+'_open'](object,input,bytes.length),1,e.name);const counts=[0,1,2].map(i=>c[kind+'_count'](object,i));results.push({file:e.name,kind,counts});
   if(kind==='anm'){let at=0,textures=0,sprites=0,scripts=0;do{assert.equal(bytes.readUInt32LE(at),8,e.name);textures++;sprites+=bytes.readUInt16LE(at+4);scripts+=bytes.readUInt16LE(at+6);const next=bytes.readUInt32LE(at+36);if(!next)break;at+=next;}while(at<bytes.length);assert.deepEqual(counts,[textures,sprites,scripts],e.name);}
   if(kind==='ecl'){assert.equal(counts[0],bytes.readUInt16LE(16),e.name);assert.equal(bytes.toString('ascii',0,4),'SCPT');}
   if(kind==='std')assert.equal(counts[0],bytes.readUInt16LE(0),e.name);
  }finally{c[kind+'_delete'](object);c.deallocate(input);}
 }
 report('resource-parsers',{passed:true,files:results.length,results,note:'File structure validated; script opcode behavior is verified separately.'});
});
