import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {graphicsOracle}from './graphics-oracle.mjs';import {textureOracle}from './texture-oracle.mjs';
test('TH11 text texture triangle resampling matches original D3DX9 pixels',async()=>{
 const c=await core(),m=await oracle(),t=textureOracle(m,graphicsOracle(m)),source=c.allocate(262144),dest=c.allocate(262144);let checks=0;
 try{
  for(const [format,native,bpp]of [[5,26,2],[1,21,4]])for(const [w,h,tw,th]of [[790,36,384,16],[104,36,41,17],[50,36,14,16],[128,32,64,16],[32,16,32,16],[32,16,48,24]]){
   const bytes=Buffer.alloc(w*h*bpp);for(let i=0;i<bytes.length;++i)bytes[i]=(i*41+(i>>>3)*29+7)&255;memory(c,source,bytes.length).set(bytes);
   const expected=t.convert(bytes,w,h,native,native,bpp,bpp,tw,th,4),size=c.image_resample(format,w,h,tw,th,source,dest);assert.equal(size,expected.length);
   const actual=Buffer.from(memory(c,dest,size));if(!actual.equals(expected)){let at=0;while(actual[at]===expected[at])++at;assert.fail(`format ${format}, ${w}x${h}->${tw}x${th} at ${at}: cpp=${actual[at]} original=${expected[at]}`);}++checks;
  }
  report('image-resample',{passed:true,checks,scope:'CPU triangle filtering against shipped D3DX9_37; original text source padding, fractional scaling, ARGB4444 and BGRA8.'});
 }finally{m.close();c.release(source);c.release(dest);}
});
