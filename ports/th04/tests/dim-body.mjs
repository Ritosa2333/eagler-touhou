import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {installProjectileArt} from '../web/projectile-art.js';

const patch=JSON.parse(await readFile(new URL('../web/patch.json',import.meta.url),'utf8'));
const original=await readFile(new URL('../web/assets/projectiles-rgb.bin',import.meta.url));
const body=await readFile(new URL('../web/assets/player-body-dim.bin',import.meta.url));
const dim=await readFile(new URL('../web/assets/projectiles-rgb-dim.bin',import.meta.url));
const palette=await readFile(new URL('../web/assets/projectiles-original-dim.bin',import.meta.url));
globalThis.fetch=async url=>new Response(url.pathname.endsWith('/player-body-dim.bin')?body:url.pathname.endsWith('/projectiles-rgb-dim.bin')?dim:url.pathname.endsWith('/projectiles-original-dim.bin')?palette:original);
const heap=new Uint8Array(9*1024*1024);let next=8;
const api={malloc(size){const at=next;next=(next+size+7)&~7;return at;},thArtBind(){},thLayerSetView(){}};
const art=await installProjectileArt({HEAPU8:heap,nativeExports:()=>api},patch);
const colors=new DataView(body.buffer,body.byteOffset,body.byteLength),table=new DataView(heap.buffer);
for(let characters=0;characters<8;characters++)for(let local=0;local<3;local++){
  const ranks=[0,0],loadouts=Array.from({length:3},(_,i)=>((characters>>i)&1)*2);
  art.configureBody(loadouts,local);
  for(let seat=0;seat<3;seat++){
    const character=loadouts[seat]>>1,rank=ranks[character]++;
    for(let pixel=0;pixel<3*1536;pixel++){
      const expected=colors.getUint32(((character*3+rank)*3*1536+pixel)*4,true);
      assert.equal(table.getUint32(art.table+(1+seat*3*1536+pixel)*4,true),expected&&seat===local?0x01000000:expected);
    }
  }
  const seen=[0,0];
  for(let seat=0;seat<3;seat++){
    const character=loadouts[seat]>>1,rank=seen[character]++;
    if(rank){
      const bank=(16000+(character*2+rank-1)*44*256)*4;
      for(let pixel=0;pixel<44*256;pixel++){
        const at=bank+pixel*4;
        if(at>=60016*4&&at<60064*4)continue;
        assert.equal(table.getUint32(art.table+at,true),(seat===local?original:dim).readUInt32LE(at));
      }
    }else{
      const at=(60016+seat*16)*4;
      for(let index=0;index<16;index++)assert.equal(table.getUint32(art.table+at+index*4,true),seat===local?0:palette.readUInt32LE((character*16+index)*4));
    }
  }
}
console.log('TH04 three-player body and projectile variants verified');
