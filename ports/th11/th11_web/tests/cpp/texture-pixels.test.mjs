import test from 'node:test';import assert from 'node:assert/strict';import {core,oracle,memory,report} from './helpers.mjs';import{graphicsOracle}from'./graphics-oracle.mjs';import{textureOracle}from'./texture-oracle.mjs';
test('TH11 texture channels match every packed 16-bit original pixel value',async()=>{
 const c=await core(),m=await oracle(),lib=graphicsOracle(m),t=textureOracle(m,lib),input=c.allocate(131072),out=c.allocate(262144),bytes=Buffer.alloc(131072);for(let i=0;i<65536;++i)bytes.writeUInt16LE(i,i*2);memory(c,input,bytes.length).set(bytes);let checks=0;
 try{for(const [format,original] of [[2,25],[3,23],[5,26]]){
  const expected=t.convert(bytes,256,256,original,21,2,4);assert.equal(c.texture_decode(format,256,256,input,bytes.length,out),1);const actual=Buffer.from(memory(c,out,262144));
  for(let n=0;n<65536;++n)for(let k=0;k<4;++k){const channel=[2,1,0,3][k];assert.equal(actual[n*4+k],expected[n*4+channel],`format=${format} pixel=${n.toString(16)} channel=${k}`);++checks;}
 }report('texture-pixels',{passed:true,checks,pixels:196608,scope:'all packed ARGB1555 RGB565 ARGB4444 values expanded against original D3DX conversion'});
 }finally{m.close();c.release(input);c.release(out);}
});
test('TH11 texture storage conversion and padded rows match original surface writes',async()=>{
 const c=await core(),m=await oracle(),t=textureOracle(m,graphicsOracle(m)),p=c.allocate(262144),out=c.allocate(524288),formats=[0,21,25,23,20,26],bpp=[4,4,2,2,3,2];let checks=0;
 try{for(let input=1;input<=5;++input)for(let output=1;output<=5;++output){
  const w=128,h=32,n=w*h,bytes=Buffer.alloc(n*bpp[input]);for(let i=0;i<bytes.length;++i)bytes[i]=(i*47+(i>>>4)*113)&255;
  memory(c,p,bytes.length).set(bytes);const expected=t.convert(bytes,w,h,formats[input],formats[output],bpp[input],bpp[output]);const size=c.texture_image(input,output,w,h,w,h,p,bytes.length,out);assert.equal(size,expected.length);assert.deepEqual(Buffer.from(memory(c,out,size)),expected,`conversion ${input}->${output}`);++checks;
  const padded=c.texture_image(input,output,w,h,w+7,h+5,p,bytes.length,out),actual=Buffer.from(memory(c,out,padded));assert.equal(padded,(w+7)*(h+5)*bpp[output]);for(let y=0;y<h+5;++y){const row=actual.subarray(y*(w+7)*bpp[output],(y+1)*(w+7)*bpp[output]);if(y<h)assert.deepEqual(row.subarray(0,w*bpp[output]),expected.subarray(y*w*bpp[output],(y+1)*w*bpp[output]));assert.ok(row.subarray(y<h?w*bpp[output]:0).every(v=>v===0));}++checks;
 }report('texture-storage',{passed:true,checks,scope:'25 original surface format conversions; declared texture dimensions preserve row padding; padding initialized deterministically'});
 }finally{m.close();c.release(p);c.release(out);}
});
