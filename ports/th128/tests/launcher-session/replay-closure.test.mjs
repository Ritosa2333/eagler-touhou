import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {spawnSync}from'node:child_process';
import{fileURLToPath}from'node:url';
import{SITE_FILES}from'../../tools/package-online-site.mjs';
test('current Replay bridge and all static relative dependencies are in the source delivery graph',()=>{
 const site=new URL('../../site/',import.meta.url),allowed=new Set(SITE_FILES);
 function visit(file,seen=new Set()){
  if(seen.has(file))return;seen.add(file);assert(allowed.has(file),'Source builder misses '+file);
  const source=fs.readFileSync(new URL(file,site),'utf8');
  for(const row of source.matchAll(/(?:import[^;\n]*from|export[^;\n]*from)\s*['"]([^'"]+)['"]/g)){
   if(!row[1].startsWith('.'))continue;
   const target=fileURLToPath(new URL(row[1],new URL(file,site))).replace(fileURLToPath(site),'').replaceAll('\\','/');
   visit(target,seen);
  }
 }
 visit('runtime/replay-file-bridge.mjs');
});
test('complete changed file/store/session/entry modules parse without graphics or runtime evaluation',()=>{
 for(const name of ['site/import-entry.mjs','site/runtime/replay-file-bridge.mjs','site/runtime/native-score-file.mjs','site/runtime/music-output.mjs','site/runtime/native-ending-browser-host-v4.mjs','site/runtime/sound.mjs','site/runtime/launcher-session.mjs']){
  const result=spawnSync(process.execPath,['--check',fileURLToPath(new URL('../../'+name,import.meta.url))],{encoding:'utf8',windowsHide:true});
  assert.equal(result.status,0,result.stderr);assert.equal(result.stderr,'');
 }
});
