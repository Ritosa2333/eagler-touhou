import fs from 'node:fs';import path from 'node:path';import crypto from 'node:crypto';
import {parseReplay} from '../src/replay.mjs';
const root=path.resolve(import.meta.dirname,'../..'),title=path.join(root,'th12-eagler');
const fixtures=[];fs.mkdirSync(path.join(title,'site/replays'),{recursive:true});
for(const [dir,kind] of [[path.join(root,'th12/replay'),'user-supplied'],[path.join(title,'local/retail'),'built-in-demo']]){
  for(const name of fs.readdirSync(dir).filter(n=>kind==='built-in-demo'?/^demo\d\.rpy$/.test(n):/\.rpy$/i.test(n)).sort()){
    const file=path.join(dir,name),bytes=fs.readFileSync(file),r=parseReplay(bytes);
    fs.copyFileSync(file,path.join(title,'site/replays',name));
    fixtures.push({id:name,provenance:kind,sha256:crypto.createHash('sha256').update(bytes).digest('hex'),bytes:bytes.length,name:r.name,character:r.character,shot:r.shot,difficulty:r.difficulty,stages:r.stages.map(({number,seed,frames,initial})=>({number,seed,frames,initial})),completion:'not-observed'});
  }
}
const quick=fixtures.filter(f=>f.provenance==='built-in-demo').map(f=>f.id);
const daily={standardLoadout:{character:1,shot:1},requiredDifficulties:[3,4],fixtures:[],status:'missing-standard-loadout-fixtures'};
const manifest={schema:'th12-replay-corpus/1',game:'th12',version:'1.00b',fixtures,quick,daily,goldenStatus:'gameplay-goldens-not-yet-captured'};
fs.writeFileSync(path.join(title,'tools/replay-verifier/corpus.json'),JSON.stringify(manifest,null,2)+'\n');
fs.writeFileSync(path.join(title,'site/replays/index.json'),JSON.stringify(manifest));
console.log(`${fixtures.length} original replays; ${quick.length} demos; daily standard loadout unavailable.`);
