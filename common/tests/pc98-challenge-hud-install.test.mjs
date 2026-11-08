import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {gunzipSync} from 'node:zlib';
import {installNetPause} from '../browser/pc98-net-pause.mjs';
import {installChallengeHud} from '../browser/pc98-challenge-hud-install.mjs';
const root=new URL('../../',import.meta.url);
for(const game of ['02','04'])for(const language of ['cn','jp'])test(`TH${game} ${language} installs native challenge HUD without changing the original`,async()=>{
  const port=new URL(`ports/th${game}/web/`,root),{Fat12}=await import(new URL('solo-fat12.js',port));
  const disk=game==='02'?`th02-local-${language}.hdi.gz`:`th04-coop${language==='jp'?'-jp':''}.hdi.gz`;
  const fat=new Fat12(new Uint8Array(gunzipSync(await readFile(new URL(disk,port)))));
  const exe=fat.read(fat.find(game==='02'?'HUMAROKU/MAIN.EXE':'GENSO/MAIN.EXE'));
  const patch=JSON.parse(await readFile(new URL(game==='02'?'local-patch.json':language==='jp'?'patch-jp.json':'patch.json',port),'utf8'));
  const overlay=JSON.parse(await readFile(new URL('netplay/native-pause-overlay.json',port),'utf8')).languages[language];
  if(game==='02'){const {installRapidFire}=await import(new URL('managed-rapid-fire.mjs',port));const {installBombBackgroundRepair}=await import(new URL('managed-bomb-background.mjs',port));const {installGameOverTransition,installNativeMarker}=await import(new URL('managed-gameover.mjs',port));installRapidFire(exe,patch,true);installBombBackgroundRepair(exe,patch);installGameOverTransition(exe);installNativeMarker(exe,patch);}
  const paused=installNetPause(exe,overlay),before=paused.slice(),result=installChallengeHud(paused,game,patch);
  assert.deepEqual(paused,before);assert.equal(result.countsOffset,game==='02'?0x8aa8:0xb920);
  assert.equal(result.bytes.length,paused.length+(game==='04'?160:0));
  const header=new DataView(result.bytes.buffer).getUint16(8,true)*16,cs=game==='02'?patch.codeSegment:patch.code_segment,site=game==='02'?0x6b33:0x8fe3;
  const view=new DataView(result.bytes.buffer),relative=(view.getUint16(header+cs*16+site+1,true)<<16)>>16;
  assert.equal((site+3+relative)&65535,result.countsOffset+12);
});
