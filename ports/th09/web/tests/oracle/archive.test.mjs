import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {createHash} from 'node:crypto';
import {oracle,core,root,memory,report} from '../cpp/helpers.mjs';

test('Every original TH09 archive resource matches raw original decode and unwrap',async()=>{
    const m=await oracle(),c=await core(),archive=c.archive_create(),dat=readFileSync(resolve(root,'../[th09] 东方花映塚 (日文版)/th09.dat'));
    const p=c.allocate(dat.length),q=c.allocate(64*1024*1024),name=c.allocate(1024);
    const source=m.allocate(16*1024*1024),out=m.allocate(32*1024*1024),heap=m.heap;
    let total=0;const hashes=[];
    try{
        memory(c,p,dat.length).set(dat);assert.equal(c.archive_open(archive,p,dat.length),1);
        const count=c.archive_count(archive);assert.ok(count>100);
        for(let i=0;i<count;++i){
            const a=c.archive_name(archive,i),all=memory(c,0,c.memory.buffer.byteLength),filename=new TextDecoder().decode(all.subarray(a,all.indexOf(0,a)));
            const offset=c.archive_field(archive,i,0),size=c.archive_field(archive,i,1),compressed=c.archive_field(archive,i,2);
            assert.ok(compressed<=16*1024*1024&&size<=32*1024*1024,filename);
            memory(c,name,1024).fill(0);memory(c,name,Buffer.byteLength(filename)).set(Buffer.from(filename));
            const length=c.archive_read(archive,name,q,64*1024*1024,1);assert.ok(length>=0,filename);
            m.write(source,dat.subarray(offset,offset+compressed));m.call(0x433aa0,{ecx:source,edx:compressed,args:[out,size],limit:2000000000});
            const unwrapped=m.call(0x42c290,{ecx:out,args:[size],limit:20000000});
            assert.equal(length,unwrapped===out?size:size-4,filename);assert.deepEqual(memory(c,q,length),m.bytes(unwrapped,length),filename);
            m.heap=heap;
            const bytes=Buffer.from(memory(c,q,length));
            const target=resolve(root,'reference/assets',filename);assert.ok(target.startsWith(resolve(root,'reference/assets')+'/')||target.startsWith(resolve(root,'reference/assets')+'\\'));
            mkdirSync(dirname(target),{recursive:true});writeFileSync(target,bytes);
            hashes.push({name:filename,bytes:length,sha256:createHash('sha256').update(bytes).digest('hex')});total+=length;
        }
        report('archive',{passed:true,entries:count,totalBytes:total,sourceSha256:createHash('sha256').update(dat).digest('hex'),resources:hashes});
    }finally{c.archive_delete(archive);c.release(p);c.release(q);c.release(name);m.close();}
});
