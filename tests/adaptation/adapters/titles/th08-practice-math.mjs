import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';

const storage=new ArrayBuffer(4),view=new DataView(storage);
export const bits=value=>{view.setFloat32(0,value,true);return view.getUint32(0,true);};
export const float=word=>{view.setUint32(0,word,true);return view.getFloat32(0,true);};

/** Independent decoder of the source's compressed CPU data, never a native lookup call. */
export async function loadTh08MathReference(root){
  const path=resolve(root,'ports/th08/web/cpp/game/PracticeRsqrtTables.hpp'),source=await readFile(path),text=source.toString('utf8');
  const block=text.match(/th08_rsqrt_data\[\d+\]\s*=\s*\{([\s\S]*?)\};/);assert.ok(block);
  const bytes=Buffer.from([...block[1].matchAll(/0x([\da-f]{2})/gi)].map(m=>parseInt(m[1],16)));
  assert.equal(bytes.length,26896);assert.equal(bytes.subarray(0,4).toString(),'RSQ2');
  assert.deepEqual([4,8,12].map(n=>bytes.readUInt32LE(n)),[1024,4096,21]);
  const tables=[[],[]];let cursor=128;
  for(const [cpu,size] of [1024,4096].entries())for(let i=0;i<size*2;i++){
    let word=0;for(let b=0;b<21;b++,cursor++)word+=((bytes[cursor>>>3]>>>(cursor&7))&1)*2**b;
    tables[cpu].push((word*2048)>>>0);
  }
  assert.equal(cursor,bytes.length*8);
  return{source:{path,bytes:source.length,sha256:createHash('sha256').update(source).digest('hex')},seed(square,cpu,custom){
    const hi=square>>>11,exponent=(hi>>>12)&255,mantissa=hi&4095;
    if(cpu===2){assert.equal(custom.length,8388608);return Buffer.from(custom.buffer,custom.byteOffset,custom.byteLength).readUInt32LE(hi*4);}
    assert.ok(cpu===0||cpu===1);assert.ok(!(hi>>>20)&&exponent>0&&exponent<255);
    const size=cpu===0?1024:4096,parity=exponent&1,ref=tables[cpu][parity*size+(cpu===0?mantissa>>>2:mantissa)];
    const outExponent=((ref>>>23)&255)-(exponent-(parity?127:126))/2;
    return ((ref&0x807fffff)|(outExponent<<23))>>>0;
  }};
}

export const TH08_PRACTICE_MATH=Object.freeze({
  menuKey:'F12',toggle:12,cpu:13,choices:[27,28,29],export:14,import:15,tableBytes:8388608,
  fields:{sequence:64,enabled:65,cpu:66,square:67,seed:68,input:69,output:72,scale:75,selected:45,option:44},
  modes:[{id:'intel',index:0,code:105},{id:'amd',index:1,code:97},{id:'custom-default',index:2,code:99},{id:'custom-imported',index:2,code:99}],
  loadReference:loadTh08MathReference,bits,float,
});
