import {test} from 'node:test';
import assert from 'node:assert/strict';
import {emptyPc98Scores} from '../browser/pc98-empty-scores.mjs';

test('TH02 empty score sections have valid zero checksums',()=>{
  const b=emptyPc98Scores('th02');assert.equal(b.length,910);
  for(let at=0;at<b.length;at+=182){assert(b.subarray(at,at+178).every(v=>v===0x12));assert.equal(new DataView(b.buffer).getUint32(at+178,true),0);}
});
test('TH04 empty score sections decode with valid checksums',()=>{
  const b=emptyPc98Scores('th04');assert.equal(b.length,1960);
  for(let at=0;at<b.length;at+=196){const decoded=b.slice(at,at+196);for(let i=4;i<195;i++)decoded[i]=(decoded[i]+((decoded[i+1]>>>3)|(decoded[i+1]<<5)))&255;
    const sum=decoded.subarray(4).reduce((a,n)=>a+n,0)&65535;assert.equal(new DataView(decoded.buffer).getUint16(2,true),sum);assert(decoded.subarray(94,174).every(v=>v===0xa0));}
});
