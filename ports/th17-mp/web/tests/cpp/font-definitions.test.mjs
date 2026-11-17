import test from'node:test';import assert from'node:assert/strict';import{readFileSync}from'node:fs';import{resolve}from'node:path';import{oracle,root,report}from'./helpers.mjs';
test('TH17 native font creation and text heights are measured before choosing baked glyphs',async()=>{
 const m=await oracle(),records=[];let fonts=[];const decoder=new TextDecoder('shift_jis');
 m.replace(0x45f350,'offscreen font DIB creation boundary',()=>1,3);
 for(const e of m.importMap.values()){
  if(e.name==='GetDC'){e.argc=1;e.handler=()=>1;}
  if(e.name==='ReleaseDC'){e.argc=2;e.handler=()=>1;}
  if(e.name==='EnumFontFamiliesExA'){e.argc=5;e.handler=()=>1;}
  if(e.name==='CreateFontA'){e.argc=14;e.handler=()=>{const sp=m.reg('ESP'),p=m.u32(sp+56),raw=m.bytes(p,100),n=raw.indexOf(0),parameters=m.readWords(sp+4,13);const handle=100+fonts.length;fonts.push({handle,parameters,faceBytes:Buffer.from(raw.subarray(0,n)).toString('hex'),face:decoder.decode(raw.subarray(0,n))});return handle;};}
 }
 try{for(const fallback of[0,1]){fonts=[];m.u32(0x52a354,fallback);m.call(0x45f990);const styles=[0x334,0x338,0x33c,0x340,0x344,0x348,0x34c,0x350,0x330].map(i=>m.u32(0x52a000+i)-100);records.push({fallback,fonts:fonts.slice(),styleMapping:styles});}
  const baseline=JSON.parse(readFileSync(resolve(root,'../th15_web/artifacts/cpp/verification/font-definitions-original.json'),'utf8')).records;
  assert.deepEqual(records.map(r=>({fallback:r.fallback,fonts:r.fonts.map(({handle,...f})=>f),styleMapping:r.styleMapping})),baseline.map(r=>({fallback:r.fallback,fonts:r.fonts.map(({handle,...f})=>f),styleMapping:r.styleMapping})),'Reuse measured glyph coverage only when all original font definitions and mappings match');
  const heights=m.readWords(0x4a11b8,9);assert.deepEqual(heights,[17,17,17,17,21,21,21,21,14]);
  report('font-definitions-original',{passed:true,records,heights,originalFunctions:['0x45f990'],scope:'Executed TH17 CreateFontA initialization in both native face modes, all 13 parameters, CP932 face bytes, handle order and style mapping. Identical to the separately measured TH15 definitions; this permits reuse of immutable offline GDI glyph coverage, not TH15 game logic.'});
 }finally{m.close();}
});
