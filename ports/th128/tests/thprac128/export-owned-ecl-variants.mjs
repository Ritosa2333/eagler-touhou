import fs from'node:fs';import path from'node:path';import{createHash}from'node:crypto';import{inflateRawSync}from'node:zlib';import{pathToFileURL}from'node:url';import assert from'node:assert/strict';
const W='D:/Codex-th128-owned-20261108/menu-lifecycle-fixes-20261113',R='D:/Codex-th128-owned-20261108/github-upload-20261115/th128',out=process.argv[2],sha=b=>createHash('sha256').update(b).digest('hex');
const datBytes=fs.readFileSync(W+'/Web-delivery-20261116/resource-package-dat-ogg-v2/extracted/games/th128/th128.dat');assert.equal(sha(datBytes),'38be25a5f1b0bf5e88dd7f66e34e295338d6bb968794a1ff587337f46b3db045');
const zipBytes=fs.readFileSync(W+'/Web-delivery-20261116/resource-package-dat-ogg-v2/extracted/games/th128/language/lang_zh-hans.zip');assert.equal(sha(zipBytes),'841a7665c681d60c4a41eaef562f6a6aa9fcdd32d16629cded616f69cde11ad4');
const{openTh128Dat}=await import(pathToFileURL(R+'/site/runtime/th128-dat.mjs'));const{readZipDirectory,readEntryData}=await import(pathToFileURL(R+'/site/runtime/zip-reader.mjs'));
const entries=new Map(readZipDirectory(zipBytes).entries.map(e=>[e.name,e])),get=async name=>readEntryData(entries.get(name),{inflate:async bytes=>new Uint8Array(inflateRawSync(bytes))});
const indexBytes=await get('runtime-assets.json');assert.equal(sha(indexBytes),'98cb2f6b4221f4d4e3b2e20fde256a0593147a2f3edbc9f8133ac37fd53b980e');
const index=JSON.parse(new TextDecoder().decode(indexBytes)),archive=openTh128Dat(datBytes),names=archive.names.filter(n=>n.endsWith('.ecl')).sort(),original=new Map(),localized=new Map(),pins=[];
fs.mkdirSync(out);for(const variant of['ja','zh'])fs.mkdirSync(path.join(out,variant));
for(const name of names){const a=archive.read(name),row=index.raw[name],b=row.patch?await get(row.patch):a;assert.equal(b.length,row.bytes);assert.equal(sha(b),row.sha256);original.set(name,a);localized.set(name,b);
 for(const[variant,bytes]of[['ja',a],['zh',b]]){fs.writeFileSync(path.join(out,variant,name),bytes,{flag:'wx'});pins.push({variant,name,bytes:bytes.length,sha256:sha(bytes)});}
}
for(const variant of['ja','zh'])fs.writeFileSync(path.join(out,variant,'files.txt'),names.join('\n')+'\n',{flag:'wx'});
function offsets(input){const v=new DataView(input.buffer,input.byteOffset,input.byteLength),count=v.getUint32(16,true),table=36+v.getUint16(6,true);let cursor=table+count*4;const subs=[];
 for(let i=0;i<count;i++){let end=cursor;while(input[end])end++;const name=new TextDecoder().decode(input.subarray(cursor,end));cursor=end+1;subs.push({name,start:v.getUint32(table+i*4,true)});}
 const ends=subs.map(s=>s.start).sort((a,b)=>a-b),map=new Map();
 for(const sub of subs){const end=ends.find(x=>x>sub.start)??input.length;let n=0;for(let off=sub.start+16;off<end;){const len=v.getUint16(off+6,true);assert(len>=16&&off+len<=end);map.set(off,{name:sub.name,index:n++,opcode:v.getUint16(off+4,true),time:v.getInt32(off,true)});off+=len;}}
 return map;
}
const report=JSON.parse(fs.readFileSync(W+'/case864-thprac128-original-ecl-layout/original-thprac-chapter-layout.json','utf8')),shifts=[];let operations=0;
for(const stage of report.stages){const a=offsets(original.get(stage.name)),b=offsets(localized.get(stage.name)),byKey=new Map([...b].map(([off,s])=>[s.name+':'+s.index,{off,...s}])),deltas=new Set();
 for(const chapter of stage.chapters)for(const op of chapter.ops)for(const off of[op.start,op.dest]){const s=a.get(off),t=byKey.get(s.name+':'+s.index);assert(t&&t.opcode===s.opcode&&t.time===s.time);deltas.add(t.off-off);operations++;}
 assert.equal(deltas.size,1);const shift=[...deltas][0];assert([0,4,8].includes(shift));shifts.push({stage:stage.stage,name:stage.name,shift});
}
assert.equal(operations,286);
const profile=fs.readFileSync(W+'/Web-delivery-20261116/resource-package-v1/extracted/games/th128/assets/runtime/bullet-profile-8.bin');assert.equal(sha(profile),'7427ad4aa6459742ffaa628c7a8e303029a9692b7b3ffbdd5b7f9a3207f0d80e');fs.writeFileSync(path.join(out,'bullet-profile-8.bin'),profile,{flag:'wx'});
fs.writeFileSync(path.join(out,'source-pins.json'),JSON.stringify(pins,null,2)+'\n',{flag:'wx'});fs.writeFileSync(path.join(out,'chapter-offset-shifts.json'),JSON.stringify(shifts,null,2)+'\n',{flag:'wx'});
console.log(JSON.stringify({originalECLs:names.length,localizedECLs:names.length,independentBranchEndpoints:operations,shifts,Worlds:0}));
