// Freeze a source-identified Runtime before an A/B rebuild. Never overwrite evidence.
import {existsSync, readFileSync, writeFileSync, mkdirSync, copyFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';

const root=resolve(import.meta.dirname,'../..');
const label=process.argv[2];
if(!label||!/^[-a-zA-Z0-9_]+$/.test(label))throw Error('Usage: node freeze-performance.mjs LABEL [--production]');
const profile=process.argv.includes('--production')?'multiplayer':'multiplayer-fixtures';
const source=resolve(root,'th10_web/artifacts',profile);
const target=resolve(root,'artifacts/rollback-performance',label,'runtime');
if(existsSync(target))throw Error('Frozen Runtime already exists: '+target);
const sha=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const build=JSON.parse(readFileSync(resolve(source,'build.json')));
if(build.profile!==profile||sha(resolve(source,'th10-sdl.wasm'))!==build.sha256||
   sha(resolve(source,'th10-sdl.mjs'))!==build.loaderSha256)throw Error('Stale/mixed Runtime');
for(const [path,hash] of Object.entries(build.sourceFiles))
  if(sha(resolve(root,path))!==hash)throw Error('Build/source mismatch: '+path);
mkdirSync(target,{recursive:true});
const hashes={};
for(const name of ['th10-sdl.mjs','th10-sdl.wasm','th10-sdl.mjs.symbols','build.json']){
  const from=resolve(source,name),to=resolve(target,name);
  copyFileSync(from,to);hashes[name]=sha(to);
  if(hashes[name]!==sha(from))throw Error('Runtime changed while freezing: '+name);
}
writeFileSync(resolve(target,'frozen.json'),JSON.stringify({label,profile,sha256:build.sha256,
  sourceDigest:build.sourceDigest,files:hashes},null,2)+'\n');
console.log(JSON.stringify({label,runtime:target,sha256:build.sha256}));
