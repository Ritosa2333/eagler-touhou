import test from 'node:test';import assert from 'node:assert/strict';import {core,oracle,memory,report} from './helpers.mjs';
test('TH15 music preload wrapper appends original WAV extension before actual command enqueue',async()=>{
 const c=await core(),m=await oracle(),f=c.music_commands_create(),p=c.allocate(256),n=m.allocate(256);let cases=0;
 try{m.view(0x51bce0,0x4600).fill(0);m.view(0x503d80,8).fill(0);for(const stem of ['bgm/th15_01','bgm/th15_02','th15_15','th128_08','E:\\game\\bgm\\th15_16','a/b\\th15_17','th15_01.wav',''])for(const slot of[0,1,3,31]){const bytes=Buffer.from(stem+'\0');memory(c,p,bytes.length).set(bytes);m.write(n,bytes);m.view(0x51bce0+0x23c4,32*268).fill(0);memory(c,c.music_commands_queue(f),32*268).fill(0);assert.equal(m.call(0x44d360,{args:[slot,n]}),1);assert.equal(c.music_commands_preload_track(f,slot,p),1);assert.deepEqual(Buffer.from(memory(c,c.music_commands_queue(f),32*268)),Buffer.from(m.bytes(0x51bce0+0x23c4,32*268)),stem+' slot '+slot);cases++;}
 report('music-preload-name',{passed:true,cases,originalFunctions:['0x44d360','0x476f10'],scope:'Original higher-level stem-to-WAV wrapper into real original command array versus named C++ preload_track. This covers Music Room and stage names before decoder lookup.'});
 }finally{c.release(p);c.music_commands_delete(f);m.close();}
});
