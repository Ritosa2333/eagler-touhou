import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const site=resolve(process.argv[2]||'th10_web/artifacts/sdl-release/site');
const base=process.argv[3]||'https://api.steinsgateon.com/';
const inventory=JSON.parse(readFileSync(resolve(site,'files.json'),'utf8'));
const manifest=JSON.parse(readFileSync(resolve(site,'manifest.json'),'utf8'));
const runtime='runtime/'+manifest.game+'/'+manifest.game;
const paths=['manifest.json','host-manifest.json','release-catalog.json','packages/'+manifest.game+'/package.json','assets/launcher/app.mjs','app-shell-sw.js',runtime+'.html',runtime+'-sdl.mjs',runtime+'-sdl.wasm'];
const results=await Promise.allSettled(paths.map(async path=>{
  const response=await fetch(new URL(path,base),{cache:'no-store',signal:AbortSignal.timeout(25000)});
  assert.equal(response.status,200,path+': HTTP '+response.status);
  const bytes=Buffer.from(await response.arrayBuffer()),hash=createHash('sha256').update(bytes).digest('hex');
  assert.equal(hash,inventory[path],path+': public bytes differ from release');
  return {path,status:response.status,bytes:bytes.length,sha256:hash};
}));
for(let i=0;i<results.length;i++){const result=results[i];console.log(JSON.stringify(result.status==='fulfilled'?result.value:{path:paths[i],error:String(result.reason),cause:String(result.reason?.cause??'')}));}
if(results.some(result=>result.status==='rejected'))process.exitCode=1;
