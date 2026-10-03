// Read-only summary of retained V8 CPU profiles. Samples are diagnostic
// evidence, not a substitute for paired timing with the sampler disabled.
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

const directory=fileURLToPath(new URL('../../artifacts/multiplayer-tests/',import.meta.url));
const labels=process.argv.slice(2);
assert.ok(labels.length,'Pass one or more report labels');
for(const label of labels){
 assert.match(label,/^[\w-]+$/);
 const report=JSON.parse(readFileSync(resolve(directory,label+'-report.json')));
 console.log(JSON.stringify({label,identity:report.identity,parameters:report.parameters,sameFrame:report.sameFrame,summary:report.summary,
  endpoints:report.results.map(r=>({stats:r.stats,hash:r.hash,renderer:r.renderer,peakBullets:r.measure.loads?.reduce((n,s)=>Math.max(n,s[1]+s[2]),0)}))}));
 if(!report.parameters.profile)continue;
 for(let side=1;side<=2;++side){
  const profile=JSON.parse(readFileSync(resolve(directory,`${label}-P${side}.cpuprofile`)));
  const nodes=new Map(profile.nodes.map(n=>[n.id,{...n,self:0,total:0}]));
  assert.equal(profile.samples.length,profile.timeDeltas.length);
  profile.samples.forEach((id,i)=>{nodes.get(id).self+=profile.timeDeltas[i]/1000;});
  const visit=id=>{const n=nodes.get(id);n.total=n.self+(n.children||[]).reduce((s,c)=>s+visit(c),0);return n.total;};
  const total=visit(profile.nodes[0].id),functions=new Map();
  for(const n of nodes.values()){
   const name=n.callFrame.functionName||'(anonymous)',key=name+' @ '+n.callFrame.url;
   const f=functions.get(key)||{name,url:n.callFrame.url,selfMs:0,inclusiveMs:0};
   f.selfMs+=n.self;f.inclusiveMs+=n.total;functions.set(key,f);
  }
  console.log(JSON.stringify({label,side,totalMs:total,self:[...functions.values()].sort((a,b)=>b.selfMs-a.selfMs).slice(0,35),
   inclusive:[...functions.values()].filter(f=>f.url.startsWith('wasm:')||f.url.includes('.wasm')).sort((a,b)=>b.inclusiveMs-a.inclusiveMs).slice(0,30)}));
 }
}
