import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {gunzipSync} from 'node:zlib';
import {installGameOverTransition as th02} from '../../ports/th02/web/managed-gameover.mjs';
import {installGameOverTransition as th04} from '../../ports/th04/web/managed-gameover.mjs';
import {Fat12 as Fat02} from '../../ports/th02/web/solo-fat12.js';
import {Fat12 as Fat04} from '../../ports/th04/web/solo-fat12.js';

for(const [game,install,Fat] of [['02',th02,Fat02],['04',th04,Fat04]])for(const language of ['jp','cn'])for(const mode of ['solo','coop'])test(`TH${game} ${language} ${mode} patches only the Game Over wait`,async()=>{
  const base=new URL(`../../../th${game}-coop-lab/web/`,import.meta.url);
  let exe;
  if(mode==='solo')exe=gunzipSync(await readFile(new URL(`practice-${language}.exe.gz`,base)));
  else{
    const name=game==='02'?`th02-local-${language}.hdi.gz`:`th04-coop${language==='jp'?'-jp':''}.hdi.gz`;
    const disk=new Fat(new Uint8Array(gunzipSync(await readFile(new URL(name,base)))));
    exe=disk.read(disk.find(game==='02'?'HUMAROKU/MAIN.EXE':'GENSO/MAIN.EXE'));
  }
  const before=Uint8Array.from(exe),header=before[8]*16+before[9]*4096;
  install(exe);
  const changes=[];for(let i=0;i<before.length;i++)if(before[i]!==exe[i])changes.push(i-header);
  assert.deepEqual(changes,game==='02'?[0xc405,0xc406]:[0xe608]);
  assert.throws(()=>install(exe));
});
