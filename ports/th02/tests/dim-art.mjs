import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {loadPlayerArt,installPlayerArt} from '../web/player-art.js';

const patch=JSON.parse(await readFile(new URL('../web/local-patch.json',import.meta.url),'utf8'));
const original=await readFile(new URL('../web/assets/th02-player-art.bin',import.meta.url));
const dim=await readFile(new URL('../web/assets/th02-player-art-dim.bin',import.meta.url));
globalThis.fetch=async url=>new Response(url.pathname.endsWith('-dim.bin')?dim:original);
const art=await loadPlayerArt(patch),heap=new Uint8Array(9*1024*1024);let next=8;
const api={malloc(size){const at=next;next=(next+size+7)&~7;return at;},thArtBind(){},thArtState(){},thLayerSetView(){}};
const layer=installPlayerArt({HEAPU8:heap,nativeExports:()=>api},patch,art);
for(let local=0;local<3;local++){
  layer.view(local);
  for(let seat=0;seat<3;seat++){
    const source=new Uint8Array(seat===local?original:dim);
    for(const [offset,length] of [[(1+seat*18432)*4,18432*4],[(60016+seat*16)*4,64]])
      assert.deepEqual(heap.slice(layer.table+offset,layer.table+offset+length),source.slice(offset,offset+length));
  }
}
console.log('TH02 three-player body and projectile views verified');
