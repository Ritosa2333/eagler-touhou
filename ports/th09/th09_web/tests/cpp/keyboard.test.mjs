import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';
test('All keyboard layouts and combined menu input match the native key sampler',async()=>{
 const m=await oracle(),c=await core(),p=c.allocate(256);let keys=new Uint8Array(256),checks=0;const old=m.onImport;
 m.onImport=e=>{if(e.name==='GetKeyboardState'){m.write(m.u32(m.reg('ESP')+4),keys.map(n=>n?128:0));m.ret(1,1);return;}old(e);};m.u32(0x4b30b8,1);m.u32(0x4b3110,0);m.replace(0x42b410,'disconnected-gamepad',()=>m.u32(m.reg('ESP')+4),1);
 try{for(let device=0;device<5;++device)for(let sample=0;sample<768;++sample){keys.fill(0);if(sample<256)keys[sample]=1;else for(let n=0;n<256;++n)keys[n]=((n*73+sample*7)%(5+sample%29))===0?1:0;memory(c,p,256).set(keys);m.write(0x4b353f,Uint8Array.of(device));assert.equal(c.keyboard_input_mask(p,device),m.call(0x42b850,{ecx:0}),'layout '+device+' case '+sample);assert.equal(c.keyboard_input_mask(p,2),m.call(0x42b850,{ecx:2}),'menu '+sample);checks+=2;}report('keyboard',{checks,layouts:5,scope:'Every virtual key alone and simultaneous combinations, including original left/right keyboard layouts and menu union. Joystick contribution is a separate adapter.'});}finally{m.close();}
});
