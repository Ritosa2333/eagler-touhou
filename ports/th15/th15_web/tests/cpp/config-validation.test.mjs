import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
test('TH15 config range checks accept and reject the same bytes as original loader',async()=>{
 const c=await core(),m=await oracle(),config=c.config_create(),p=c.allocate(108),original=m.allocate(108),manager=m.allocate(0x800);let inputSize=108,invalid=false,checks=0;
 m.replace(0x490f2b,'test_chdir',()=>0);m.replace(0x402db0,'test_config_file_read',()=>{m.u32(m.reg('EDX'),inputSize);return original;},1);m.replace(0x402ff0,'test_config_file_write',()=>0,1);m.replace(0x403490,'test_config_log',()=>{if(m.u32(m.reg('ESP')+8)===0x4ccd90)invalid=true;return 0;});
 try{c.config_reset(config,0);const defaults=Buffer.from(memory(c,config,108));for(let offset=0x1c;offset<=0x21;offset++)for(let value=0;value<256;value++){
  const bytes=Buffer.from(defaults);bytes[offset]=value;memory(c,p,108).set(bytes);m.write(original,bytes);invalid=false;m.call(0x44d040,{ecx:manager});assert.equal(Boolean(c.config_open(config,p,108)),!invalid,JSON.stringify({offset,value}));checks++;
 }for(const size of [0,1,107,109]){inputSize=size;m.write(original,defaults);invalid=false;m.call(0x44d040,{ecx:manager});assert.equal(invalid,true,'original size '+size);assert.equal(c.config_open(config,p,size),0,'C++ size '+size);checks++;}report('config-validation',{passed:true,checks,originalFunction:'0x44d040',scope:'Header, length and six option ranges. Test host supplies config file bytes and suppresses filesystem writes.'});
 }finally{c.config_delete(config);c.release(p);m.close();}
});
