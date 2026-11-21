import fs from 'node:fs';import path from 'node:path';import{createHash}from'node:crypto';import{pathToFileURL}from'node:url';import assert from'node:assert/strict';
const W='D:/Codex-th128-owned-20261108/menu-lifecycle-fixes-20261113',R='D:/Codex-th128-owned-20261108/github-upload-20261115/th128',out=process.argv[2],sha=b=>createHash('sha256').update(b).digest('hex');
const reportFile=W+'/case864-thprac128-original-ecl-layout/original-thprac-chapter-layout.json',reportBytes=fs.readFileSync(reportFile);
assert.equal(sha(reportBytes),'e8192ab0ec8aaac8873552d004b0d653f8d4076457a1d676cc0250651e38ca8b');
const dat=fs.readFileSync(W+'/Web-delivery-20261116/resource-package-dat-ogg-v2/extracted/games/th128/th128.dat');assert.equal(sha(dat),'38be25a5f1b0bf5e88dd7f66e34e295338d6bb968794a1ff587337f46b3db045');
const{openTh128Dat}=await import(pathToFileURL(R+'/site/runtime/th128-dat.mjs'));const archive=openTh128Dat(dat),report=JSON.parse(reportBytes),rows=[],pins=[];
fs.mkdirSync(out,{recursive:false});for(const stage of report.stages){
 const bytes=archive.read(stage.name);assert.equal(bytes.length,stage.bytes);assert.equal(sha(bytes),stage.sha256);fs.writeFileSync(path.join(out,stage.name),bytes,{flag:'wx'});pins.push({name:stage.name,bytes:bytes.length,sha256:sha(bytes)});
 for(const chapter of stage.chapters){assert(chapter.ops.length<=2);const words=chapter.ops.flatMap(o=>[o.start,o.dest,o.eclTime,o.atFrame]);while(words.length<8)words.push(0);rows.push([stage.stage,chapter.chapter,stage.name,chapter.ops.length,...words].join('\t'));}
}assert.equal(rows.length,126);fs.writeFileSync(path.join(out,'plans.tsv'),rows.join('\n')+'\n',{flag:'wx'});fs.writeFileSync(path.join(out,'source-pins.json'),JSON.stringify(pins,null,2)+'\n',{flag:'wx'});
console.log(JSON.stringify({actualOriginalStages:pins.length,chapterPlans:rows.length,originalDATUnchanged:true,worlds:0}));
