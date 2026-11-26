import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {existsSync} from 'node:fs';
import {gunzipSync} from 'node:zlib';
import {installNetPause} from '../browser/pc98-net-pause.mjs';
import {installChallengeHud} from '../browser/pc98-challenge-hud-install.mjs';
const root=new URL('../../',import.meta.url);
const game='04';
for(const language of ['cn','jp'])test(`TH${game} ${language} installs native challenge HUD without changing the original`,async(t)=>{
  const port=new URL(`ports/th${game}/web/`,root),{Fat12}=await import(new URL('solo-fat12.js',port));
  const disk=`th04-coop${language==='jp'?'-jp':''}.hdi.gz`;
  if(!existsSync(new URL(disk,port))){t.skip('private game disk is not available');return;}
  const fat=new Fat12(new Uint8Array(gunzipSync(await readFile(new URL(disk,port)))));
  const exe=fat.read(fat.find('GENSO/MAIN.EXE'));
  const patch=JSON.parse(await readFile(new URL(language==='jp'?'patch-jp.json':'patch.json',port),'utf8'));
  const overlay=JSON.parse(await readFile(new URL('netplay/native-pause-overlay.json',port),'utf8')).languages[language];
  const prepared=installNetPause(exe,overlay);
  const before=prepared.slice(),result=installChallengeHud(prepared,game,patch);
  assert.deepEqual(prepared,before);assert.equal(result.countsOffset,0xb920);
  assert.equal(result.addedParagraphs,10);
  assert.equal(result.bytes.length,prepared.length+16*result.addedParagraphs);
  const header=new DataView(result.bytes.buffer).getUint16(8,true)*16,cs=patch.code_segment,site=0x8fe3;
  const view=new DataView(result.bytes.buffer),relative=(view.getUint16(header+cs*16+site+1,true)<<16)>>16;
  assert.equal((site+3+relative)&65535,result.countsOffset+12);
  const base=header+cs*16;
  assert.deepEqual(result.bytes.slice(base+result.countsOffset,base+result.countsOffset+12),new Uint8Array(12));
});
