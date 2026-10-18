// Offline analysis only. Symbols must match the exact frozen runtime module.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,existsSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {createHash} from 'node:crypto';
const [directory,buildDirectory]=process.argv.slice(2).map(p=>resolve(p));
assert.ok(directory&&buildDirectory,'usage: node summarize-cpu-profile.mjs RUN_DIRECTORY BUILD_DIRECTORY');
const read=p=>JSON.parse(readFileSync(p,'utf8'));
const sha=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
const receipt=read(resolve(directory,'receipt.json')),build=read(resolve(buildDirectory,'build.json'));
const symbolsPath=resolve(buildDirectory,build.symbolMap.file);
assert.equal(sha(symbolsPath),build.symbolMap.sha256,'Wrong symbol map');
assert.equal(sha(resolve(receipt.runtime,'th11-sdl.wasm')),build.sha256,'Profile runtime differs from symbol build');
const symbols=new Map(readFileSync(symbolsPath,'utf8').trim().split(/\r?\n/).map(line=>{const i=line.indexOf(':');return [+line.slice(0,i),line.slice(i+1)];}));
const result={schema:'th11-cpu-profile-summary/1',runtimeWasm:build.sha256,symbolMap:build.symbolMap,
  note:'Sampled wall time, including profiler and CPU throttling effects. Windows include profiler start/stop overhead; this is not a frame acceptance result.',seats:[]};
for(const entry of receipt.cpuProfiles.files){
  assert.equal(sha(entry.path),entry.sha256,'Profile changed');const p=read(entry.path);
  const nodes=new Map(p.nodes.map(n=>[n.id,n])),parents=new Map(),names=new Map();
  for(const n of p.nodes){for(const child of n.children||[])parents.set(child,n.id);
    const f=n.callFrame,m=/^wasm-function\[(\d+)\]$/.exec(f.functionName);
    names.set(n.id,m&&f.url.endsWith('/th11-sdl.wasm')?(symbols.get(+m[1])||f.functionName):f.functionName||'(anonymous)');}
  const sums=new Map();const value=name=>{let x=sums.get(name);if(!x)sums.set(name,x={name,selfUs:0,inclusiveUs:0,selfSamples:0});return x;};
  for(let i=0;i<p.samples.length;++i){const id=p.samples[i],us=p.timeDeltas[i]||0;const x=value(names.get(id));x.selfUs+=us;++x.selfSamples;
    const seen=new Set();for(let n=id;n;n=parents.get(n)){const name=names.get(n);if(!seen.has(name)){seen.add(name);value(name).inclusiveUs+=us;}}}
  const functions=[...sums.values()].map(x=>({...x,selfMs:x.selfUs/1000,inclusiveMs:x.inclusiveUs/1000})).sort((a,b)=>b.selfUs-a.selfUs);
  result.seats.push({seat:entry.seat,profile:entry.path,profileSha256:entry.sha256,windowMs:(p.endTime-p.startTime)/1000,samples:p.samples.length,functions});
}
const output=resolve(directory,'cpu-profile-summary.json');assert.ok(!existsSync(output),'Never overwrite a previous analysis');
writeFileSync(output,JSON.stringify(result,null,2)+'\n');
console.log(JSON.stringify({output,runtimeWasm:result.runtimeWasm,seats:result.seats.map(s=>({seat:s.seat,windowMs:s.windowMs,samples:s.samples,
  top:s.functions.slice(0,18).map(f=>({name:f.name.slice(0,190),selfMs:Math.round(f.selfMs),inclusiveMs:Math.round(f.inclusiveMs)}))}))},null,2));
