import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const root=fileURLToPath(new URL('../../',import.meta.url)),repo=resolve(root,'..'),out=resolve(root,'artifacts/multiplayer-tests');
const read=p=>JSON.parse(readFileSync(p,'utf8')),sha=b=>createHash('sha256').update(b).digest('hex');
const build=read(resolve(root,'artifacts/sdl3/build.json')),release=read(resolve(root,'artifacts/sdl-release/build.json'));
for(const [path,hash] of Object.entries(build.sources))assert.equal(sha(readFileSync(resolve(repo,path))),hash,path+' changed since diagnostic build');
for(const [path,hash] of Object.entries(release.sources))assert.equal(sha(readFileSync(resolve(repo,path))),hash,path+' changed since release build');
assert(!release.exports.some(e=>e.name.startsWith('th09_probe_')));
const labels=['owning-final-old-a','owning-final-new-b','owning-final-new-b2','owning-final-old-a2'];
const runs=labels.map(label=>({label,...read(resolve(out,label+'-report.json'))}));
for(const r of runs){assert(r.passed&&r.hardwareRequested);assert.deepEqual(r.result.audits,runs[0].result.audits);}
for(const i of [1,2])assert.equal(runs[i].wasm,build.sha256);
const metrics=Object.fromEntries(Object.keys(runs[0].result.summary).map(k=>{
 const old=(runs[0].result.summary[k].total+runs[3].result.summary[k].total)/2;
 const current=(runs[1].result.summary[k].total+runs[2].result.summary[k].total)/2;
 return [k,{old,current,reductionPercent:100*(old-current)/old}];
}));
const gates=Object.fromEntries(['owning-world','owning-dense','owning-round-reset','peer-browser','owning-rtc','owning-relay','owning-rtc-release','dynamic-state','session','keyboard-adaptive'].map(n=>[n,read(resolve(out,n+'-report.json'))]));
for(const [name,r] of Object.entries(gates)){assert(r.passed,name);if(r.wasm&&typeof r.wasm==='string')assert.equal(r.wasm,name.endsWith('-release')?release.sha256:build.sha256,name);}
const report={passed:true,scope:'TH09 PC owning-pool optimization; CPU fixture and correctness evidence, not human competitive acceptance',diagnostic:build,release,metrics,runs,gates};
writeFileSync(resolve(repo,'docs/multiplayer/rollback-owning-evidence.json'),JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify({passed:true,diagnostic:build.sha256,release:release.sha256,metrics}));
