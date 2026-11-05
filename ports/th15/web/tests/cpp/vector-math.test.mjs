import test from 'node:test';import assert from 'node:assert/strict';import {core,oracle,memory,report,enableOriginalVectorMath} from './helpers.mjs';
test('TH15 portable vector normalization matches original D3DX mathematical routine',async()=>{
 const c=await core(),m=await oracle(),library=enableOriginalVectorMath(m),input=m.allocate(8),out=m.allocate(8);let checks=0;
 try{for(let sample=0;sample<4096;sample++){const x=(sample-2179)*.03125,y=(sample-2011)*.0625;m.f32(input,x);m.f32(input+4,y);m.call(library.exports.get('D3DXVec2Normalize'),{args:[out,input]});const actual=c.normalize_vector(x,y);assert.deepEqual(Buffer.from(memory(c,actual,8)),Buffer.from(m.bytes(out,8)),JSON.stringify({sample,x,y}));checks++;}report('vector-normalization',{passed:true,checks,originalLibrary:'d3dx9_43.dll',librarySha256:'0b28546be22c71834501f7d7185ede5d79742457331c7ee09efc14490dd64f5f',scope:'Original mathematical vector normalization versus ordinary C++ sqrt/divide. This dependency is used only by the test oracle, not by production rendering.'});
 }finally{m.close();}
});
