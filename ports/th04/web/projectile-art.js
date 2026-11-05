export async function installProjectileArt(mod,patch){
  const info=patch?.projectileArt;if(!info)return null;
  const response=await fetch(new URL(info.file,import.meta.url));
  if(!response.ok)throw Error('射弹美术资源加载失败');
  const bytes=new Uint8Array(await response.arrayBuffer());
  const {sha256}=await import('./netplay/sha256.js');
  if(bytes.length!==info.bytes||await sha256(bytes)!==info.sha256)throw Error('射弹美术资源版本不匹配');
  const api=mod.nativeExports();if(!api.thArtBind||!api.thLayerSetView)throw Error('射弹显示核心版本不匹配');
  const control=api.malloc(32),table=api.malloc(bytes.length),tags=api.malloc(1048576);
  const records=api.malloc(3*524288*4),coverage=api.malloc(524288);
  if(!control||!table||!tags||!records||!coverage)throw Error('射弹显示内存不足');
  mod.HEAPU8.set(bytes,table);mod.HEAPU8.fill(0,tags,tags+1048576);mod.HEAPU8.fill(0,control,control+32);mod.HEAPU8.fill(0,coverage,coverage+524288);
  let v=new DataView(mod.HEAPU8.buffer);v.setUint32(control+4,table,true);v.setUint32(control+8,tags,true);
  v.setUint32(control+16,records,true);v.setUint32(control+20,coverage,true);api.thArtBind(control);api.thLayerSetView(0);
  return {control,table,tags,view:slot=>api.thLayerSetView(Number.isInteger(slot)&&slot>=0&&slot<3?slot:-1),bind(mailbox){
    mod.sidebarPaletteAddress=mailbox<0?-1:mailbox-patch.mailbox_cs_offset+(patch.data_segment-patch.code_segment)*16+0x2a82;
    mod.thDirtyMark?.(control,32);v=new DataView(mod.HEAPU8.buffer);
    v.setUint32(control,mailbox<0?0:mailbox-patch.mailbox_cs_offset+info.context_cs_offset,true);
    v.setUint32(control+12,mailbox<0?0:mailbox+25,true);
    v.setUint32(control+24,mailbox<0||!Number.isInteger(patch.body_layer_cs_offset)?0:mailbox-patch.mailbox_cs_offset+patch.body_layer_cs_offset,true);
  }};
}
