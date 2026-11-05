import test from "node:test";
import assert from "node:assert/strict";
import {readFileSync} from "node:fs";
import {resolve} from "node:path";
import {core,memory,string,root,report,sha} from "./helpers.mjs";
test("Every TH15 archive entry matches independent thtk 15 extraction",async()=>{
 const c=await core(),raw=readFileSync(resolve(root,"../[th15] 东方绀珠传 (汉化版+日文版)/th15.dat")),p=c.allocate(raw.length),out=c.allocate(64*1024*1024),a=c.archive_create(),expected=new Map(JSON.parse(readFileSync(resolve(root,"reference/archive-manifest.json"))).map(e=>[e.name,e]));let total=0;const entries=[],seen=new Set();
 try{memory(c,p,raw.length).set(raw);assert.equal(c.archive_open(a,p,raw.length),1);for(let i=0;i<c.archive_count(a);i++){const name=string(c,c.archive_name(a,i)),ref=expected.get(name);assert.ok(ref,name);const n=c.archive_read(a,i,out,64*1024*1024);assert.equal(n,ref.size,name);const hash=sha(memory(c,out,n));assert.equal(hash,ref.sha256,name);entries.push({index:i,name,bytes:n,sha256:hash});seen.add(name);total+=n;}assert.equal(seen.size,expected.size);report("archive",{passed:true,sourceSha256:sha(raw),entries:entries.length,uniqueNames:seen.size,totalBytes:total,resources:entries});}finally{c.archive_delete(a);c.release(p);c.release(out);}
});
