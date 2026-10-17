import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,root,report} from './helpers.mjs';
test('TH15 shot groups, specs and callback identifiers match original SHT loader',async()=>{
 const c=await core(),m=await oracle(),player=m.allocate(0x2c100),path=m.allocate(256),heap=m.heap;let source,shots=0;const resources=[];
 m.replace(0x402db0,'read SHT fixture',()=>{const p=m.allocate(source.length);m.write(p,source);return p;},1);
 try{for(const name of ['pl00.sht','pl01.sht','pl02.sht','pl03.sht']){m.heap=heap;source=readFileSync(resolve(root,'reference/assets',name));m.write(path,Buffer.from(name+'\0'));assert.equal(m.call(0x455a20,{ecx:player,args:[path]}),0);const base=m.u32(player+0x2c008),p=c.allocate(source.length),s=c.sht_create();memory(c,p,source.length).set(source);assert.equal(c.sht_open(s,p,source.length),1,name);assert.deepEqual(Buffer.from(memory(c,c.sht_header(s),0xe0)),Buffer.from(m.bytes(base,0xe0)));const counts=[];
 for(let group=0;group<10;group++){let native=m.u32(base+0xe0+group*4),count=0;while(m.bytes(native,1)[0]<128){const got=Buffer.from(memory(c,c.sht_shot(s,group,count),0x58)),original=Buffer.from(m.bytes(native,0x58));assert.deepEqual(got.subarray(0,0x28),original.subarray(0,0x28));assert.deepEqual(got.subarray(0x38),original.subarray(0x38));for(let k=0;k<4;k++){const table=[0x4cb360,0x4cb340,0x4e9bc0,0x4cb320][k];assert.equal(m.u32(table+got.readUInt32LE(0x28+k*4)*4),original.readUInt32LE(0x28+k*4));}native+=0x58;count++;shots++;}assert.equal(c.sht_count(s,group),count);counts.push(count);}resources.push({name,groups:counts});c.sht_delete(s);c.release(p);}
 report('sht-resource',{passed:true,files:4,shots,originalFunction:'0x455a20',scope:'Resource decoding only; shot callbacks and player simulation are not yet implemented.',resources});
 }finally{m.close();}
});
