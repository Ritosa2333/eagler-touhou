// Development aid only: rank previous-engine functions by instruction shape.
// A candidate is not behavioral evidence and is never linked into the game.
import {readFileSync,readdirSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const root=fileURLToPath(new URL('../../',import.meta.url));
const source=resolve(root,'../th15_web/artifacts/cpp/analysis/jp/functions'),target=resolve(root,'artifacts/cpp/analysis/jp/functions');
const read=directory=>readdirSync(directory).filter(n=>n.endsWith('.asm')).map(name=>{
 const lines=readFileSync(resolve(directory,name),'utf8').trim().split(/\r?\n/).map(s=>s.replace(/^[0-9a-f]+ /i,'').replace(/0x[0-9a-f]+/ig,'#'));
 const grams=new Set();for(let i=0;i<lines.length-2;i++)grams.add(lines.slice(i,i+3).join('|'));
 return {name:name.slice(0,-4),lines,grams};
});
const originals=read(source),candidates=read(target),selected=process.argv.slice(2).map(s=>Number(s).toString(16).padStart(8,'0'));
const results=[];
for(const original of originals.filter(f=>!selected.length||selected.includes(f.name))){
 const ranking=candidates.map(candidate=>{let common=0;for(const gram of original.grams)if(candidate.grams.has(gram))common++;
  const score=2*common/(original.grams.size+candidate.grams.size||1),size=Math.min(original.lines.length,candidate.lines.length)/Math.max(original.lines.length,candidate.lines.length);
  return {address:candidate.name,score:Math.round(score*size*10000)/10000,instructions:candidate.lines.length};
 }).sort((a,b)=>b.score-a.score).slice(0,5);
 results.push({reference:original.name,instructions:original.lines.length,candidates:ranking});
}
const dir=resolve(root,'artifacts/cpp/analysis');mkdirSync(dir,{recursive:true});
writeFileSync(resolve(dir,'reference-function-candidates.json'),JSON.stringify({scope:'Unverified development candidates, not a native implementation or proof of equivalent behavior',results},null,2)+'\n');
console.log(JSON.stringify(results,null,2));
