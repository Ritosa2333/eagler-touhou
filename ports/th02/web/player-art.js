// Exact RGB garment/ghost colors; pixel provenance and scanout live inside the
// emulator, so overlap, raster scroll, text and rollback retain native order.
import {sha256} from './solo-fat12.js';
const signature=new TextEncoder().encode('TH02COOPv0002!!\0');
export async function loadPlayerArt(patch){
  const entry=patch.playerArt;
  if(entry?.bodySource!=='ghost-preview/th02-player-colors'||!entry.originalGenjiiPreserved)throw Error('TH02 专用绿色玄爷素材清单不匹配');
  const response=await fetch(new URL(entry.file,import.meta.url),{cache:'no-store'});
  if(!response.ok)throw Error('TH02 多人配色素材读取失败');
  const bytes=new Uint8Array(await response.arrayBuffer());
  if(bytes.length!==entry.bytes||await sha256(bytes)!==entry.sha256)throw Error('TH02 多人配色素材校验失败，请刷新游戏');
  const dimResponse=await fetch(new URL(entry.dimFile,import.meta.url),{cache:'no-store'});
  if(!dimResponse.ok)throw Error('TH02 队友素材读取失败');
  const dim=new Uint8Array(await dimResponse.arrayBuffer());
  if(dim.length!==entry.bytes||await sha256(dim)!==entry.dimSha256)throw Error('TH02 队友素材校验失败');
  return {bytes,dim};
}
export function installPlayerArt(mod,patch,art){
  const {bytes,dim}=art;
  const api=mod.nativeExports();
  if(!api.thArtBind||!api.thArtState||!api.thLayerSetView)throw Error('TH02 多人配色核心版本不匹配');
  const control=api.malloc(32),table=api.malloc(bytes.length),tags=api.malloc(2*262144*2);
  const records=api.malloc(3*524288*4),coverage=api.malloc(524288);
  if(!control||!table||!tags||!records||!coverage)throw Error('TH02 多人配色内存分配失败');
  mod.HEAPU8.set(bytes,table);mod.HEAPU8.fill(0,tags,tags+1048576);
  mod.HEAPU8.fill(0,control,control+32);mod.HEAPU8.fill(0,coverage,coverage+524288);
  const view=()=>new DataView(mod.HEAPU8.buffer);
  view().setUint32(control,0,true);view().setUint32(control+4,table,true);
  view().setUint32(control+8,tags,true);view().setUint32(control+12,0,true);
  view().setUint32(control+16,records,true);view().setUint32(control+20,coverage,true);
  api.thArtBind(control);api.thLayerSetView(0);
  const valid=at=>at>=0&&signature.every((value,i)=>mod.HEAPU8[at+i]===value);
  let mailbox=-1,checks=0;
  function setMailbox(at){
    const next=Number.isInteger(at)&&at>=0&&valid(at)&&mod.HEAPU8[at+22]===1?at:-1;
    const context=next<0?0:next-patch.entries.mailbox+patch.entries.art_context;
    if(next===mailbox&&view().getUint32(control,true)===context&&view().getUint32(control+12,true)===(next<0?0:next+22))return;
    mailbox=next;mod.thDirtyMark?.(control,32);
    view().setUint32(control,mailbox<0?0:mailbox-patch.entries.mailbox+patch.entries.art_context,true);
    view().setUint32(control+12,mailbox<0?0:mailbox+22,true);
  }
  function bind(at){
    if(arguments.length){setMailbox(at);return;}
    if(valid(mailbox)&&mod.HEAPU8[mailbox+22]===1)return;
    if(checks++%8)return;
    const base=api.thCoreState?api.thCoreState(0):0;
    const ram=api.thCoreState?mod.HEAPU8.subarray(base,base+0xa0000):mod.HEAPU8;
    for(let offset=ram.indexOf(signature[0]);offset>=0;offset=ram.indexOf(signature[0],offset+1)){
      const candidate=base+offset;
      if(!valid(candidate)||mod.HEAPU8[candidate+22]!==1)continue;
      setMailbox(candidate);break;
    }
  }
  function clearLayers(){
    // Records are ignored wherever coverage is zero; they need not be cleared.
    mod.thDirtyMark?.(tags,1048576);mod.thDirtyMark?.(coverage,524288);
    mod.HEAPU8.fill(0,tags,tags+1048576);mod.HEAPU8.fill(0,coverage,coverage+524288);
  }
  function setView(slot){
    for(let seat=0;seat<3;seat++){
      const source=seat===slot?bytes:dim,body=(1+seat*18432)*4,shot=(60016+seat*16)*4;
      mod.thDirtyMark?.(table+body,18432*4);mod.thDirtyMark?.(table+shot,64);
      mod.HEAPU8.set(source.subarray(body,body+18432*4),table+body);
      mod.HEAPU8.set(source.subarray(shot,shot+64),table+shot);
    }
    api.thLayerSetView(Number.isInteger(slot)&&slot>=0&&slot<3?slot:-1);
  }
  return {bind,clearLayers,view:setView,info:()=>({enabled:true,bound:valid(mailbox),source:patch.playerArt.bodySource,
    variants:patch.playerArt.variants,originalGenjiiPreserved:true}),control,table,tags};
}
