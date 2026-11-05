import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {core,root,memory,report,target} from './helpers.mjs';
const hash=b=>createHash('sha256').update(b).digest('hex');
test('All TH09 resources retain hashes established by the complete original archive comparison',async()=>{
    const manifest=JSON.parse(readFileSync(resolve(root,'reference/archive-manifest.json'),'utf8'));
    assert.equal(manifest.target.sha256,target.sha256);
    const dat=readFileSync(resolve(root,'../[th09] 东方花映塚 (日文版)/th09.dat'));
    assert.equal(hash(dat),manifest.sourceSha256);
    const c=await core(),a=c.archive_create(),p=c.allocate(dat.length),q=c.allocate(64*1024*1024),name=c.allocate(1024);
    try{
        memory(c,p,dat.length).set(dat);assert.equal(c.archive_open(a,p,dat.length),1);assert.equal(c.archive_count(a),manifest.entries);
        for(const item of manifest.resources){
            const encoded=Buffer.from(item.name+'\0');memory(c,name,encoded.length).set(encoded);
            const length=c.archive_read(a,name,q,64*1024*1024,1);assert.equal(length,item.bytes,item.name);
            assert.equal(hash(memory(c,q,length)),item.sha256,item.name);
        }
        report('archive-regression',{passed:true,entries:manifest.entries,totalBytes:manifest.totalBytes,sourceSha256:manifest.sourceSha256,evidence:'reference/archive-manifest.json'});
    }finally{c.archive_delete(a);for(const x of [p,q,name])c.release(x);}
});
