import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {existsSync} from 'node:fs';
import {gunzipSync} from 'node:zlib';
import {installNetPause} from '../browser/pc98-net-pause.mjs';
import {installChallengeHud} from '../browser/pc98-challenge-hud-install.mjs';
const root=new URL('../../',import.meta.url);
for(const game of ['02','04'])for(const language of ['cn','jp'])test(`TH${game} ${language} installs native challenge HUD without changing the original`,async(t)=>{
  const port=new URL(`ports/th${game}/web/`,root),{Fat12}=await import(new URL('solo-fat12.js',port));
  const disk=game==='02'?`th02-local-${language}.hdi.gz`:`th04-coop${language==='jp'?'-jp':''}.hdi.gz`;
  if(!existsSync(new URL(disk,port))){t.skip('private game disk is not available');return;}
  const fat=new Fat12(new Uint8Array(gunzipSync(await readFile(new URL(disk,port)))));
  const exe=fat.read(fat.find(game==='02'?'HUMAROKU/MAIN.EXE':'GENSO/MAIN.EXE'));
  const patch=JSON.parse(await readFile(new URL(game==='02'?'local-patch.json':language==='jp'?'patch-jp.json':'patch.json',port),'utf8'));
  const overlay=JSON.parse(await readFile(new URL('netplay/native-pause-overlay.json',port),'utf8')).languages[language];
  let prepared,insert;
  if(game==='02'){
    const {installRapidFire}=await import(new URL('managed-rapid-fire.mjs',port));
    const {installBombBackgroundRepair,installBombResourceReuse,installCoopAssetHeap}=await import(new URL('managed-bomb-background.mjs',port));
    const {installGameOverTransition,installNativeMarker}=await import(new URL('managed-gameover.mjs',port));
    const {installBonusConfirm}=await import(new URL('netplay/bonus-confirm.js',port));
    const {installRescueSound,rescueSoundBytes}=await import(new URL('managed-rescue-sound.mjs',port));
    installBonusConfirm(exe,patch);installRapidFire(exe,patch,true);installBombBackgroundRepair(exe,patch);
    installBombResourceReuse(exe,patch);installCoopAssetHeap(exe,patch);installGameOverTransition(exe);installNativeMarker(exe,patch);
    prepared=installRescueSound(installNetPause(exe,overlay),patch,overlay);
    insert=overlay.insert+overlay.bytes+rescueSoundBytes;
  }else prepared=installNetPause(exe,overlay);
  const before=prepared.slice(),result=installChallengeHud(prepared,game,patch,insert);
  assert.deepEqual(prepared,before);assert.equal(result.countsOffset,game==='02'?0x8f80:0xb920);
  assert.equal(result.addedParagraphs,game==='02'?9:10);
  assert.equal(result.bytes.length,prepared.length+16*result.addedParagraphs);
  const header=new DataView(result.bytes.buffer).getUint16(8,true)*16,cs=game==='02'?patch.codeSegment:patch.code_segment,site=game==='02'?0x6b33:0x8fe3;
  const view=new DataView(result.bytes.buffer),relative=(view.getUint16(header+cs*16+site+1,true)<<16)>>16;
  assert.equal((site+3+relative)&65535,result.countsOffset+12);
  const base=header+cs*16;
  assert.deepEqual(result.bytes.slice(base+result.countsOffset,base+result.countsOffset+12),new Uint8Array(12));
  if(game==='02')for(const [start,end] of [[0x8a80,0x8ae0],[0x8b60,0x8f80]]){
    assert.deepEqual(result.bytes.slice(base+start,base+end),before.slice(base+start,base+end));
  }
});
