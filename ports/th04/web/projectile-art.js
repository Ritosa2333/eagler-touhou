export async function installProjectileArt(mod,patch){
  const info=patch?.projectileArt;if(!info)return null;
  const response=await fetch(new URL(info.file,import.meta.url));
  if(!response.ok)throw Error('射弹美术资源加载失败');
  const bytes=new Uint8Array(await response.arrayBuffer());
  const {sha256}=await import('./netplay/sha256.js');
  if(bytes.length!==info.bytes||await sha256(bytes)!==info.sha256)throw Error('射弹美术资源版本不匹配');
  const bodyResponse=await fetch(new URL(info.bodyFile,import.meta.url));
  if(!bodyResponse.ok)throw Error('人物美术资源加载失败');
  const body=new Uint8Array(await bodyResponse.arrayBuffer());
  if(await sha256(body)!==info.bodySha256)throw Error('人物美术资源版本不匹配');
  const [dimResponse,originalDimResponse]=await Promise.all([fetch(new URL(info.dimFile,import.meta.url)),fetch(new URL(info.originalDimFile,import.meta.url))]);
  if(!dimResponse.ok||!originalDimResponse.ok)throw Error('队友射弹素材加载失败');
  const dim=new Uint8Array(await dimResponse.arrayBuffer()),originalDim=new Uint8Array(await originalDimResponse.arrayBuffer());
  if(await sha256(dim)!==info.dimSha256||await sha256(originalDim)!==info.originalDimSha256)throw Error('队友射弹素材版本不匹配');
  const api=mod.nativeExports();if(!api.thArtBind||!api.thLayerSetView)throw Error('射弹显示核心版本不匹配');
  const control=api.malloc(32),table=api.malloc(bytes.length),tags=api.malloc(1048576);
  const records=api.malloc(3*524288*4),coverage=api.malloc(524288);
  if(!control||!table||!tags||!records||!coverage)throw Error('射弹显示内存不足');
  mod.HEAPU8.set(bytes,table);mod.HEAPU8.fill(0,tags,tags+1048576);mod.HEAPU8.fill(0,control,control+32);mod.HEAPU8.fill(0,coverage,coverage+524288);
  let v=new DataView(mod.HEAPU8.buffer);v.setUint32(control+4,table,true);v.setUint32(control+8,tags,true);
  v.setUint32(control+16,records,true);v.setUint32(control+20,coverage,true);api.thArtBind(control);api.thLayerSetView(0);
  return {control,table,tags,configureBody(loadouts,localSlot){
    const source=new DataView(body.buffer,body.byteOffset,body.byteLength),target=new DataView(mod.HEAPU8.buffer),ranks=[0,0];
    mod.thDirtyMark?.(table,13825*4);
    for(let seat=0;seat<loadouts.length;seat++){
      const character=loadouts[seat]>>1,rank=ranks[character]++;
      for(let pixel=0;pixel<3*1536;pixel++){
        const color=source.getUint32(((character*3+rank)*3*1536+pixel)*4,true);
        target.setUint32(table+(1+seat*3*1536+pixel)*4,color&&seat===localSlot?0x01000000:color,true);
      }
      if(rank){
        const bank=(16000+((character*2+rank-1)*44)*256)*4,length=44*256*4,source=seat===localSlot?bytes:dim;
        mod.thDirtyMark?.(table+bank,length);mod.HEAPU8.set(source.subarray(bank,bank+length),table+bank);
        if(character===1){const laser=(65000+(rank-1)*2)*4;mod.thDirtyMark?.(table+laser,8);mod.HEAPU8.set(source.subarray(laser,laser+8),table+laser);}
      }
    }
    ranks.fill(0);
    for(let seat=0;seat<loadouts.length;seat++){
      const character=loadouts[seat]>>1,rank=ranks[character]++;
      if(!rank){
        const dest=(60016+seat*16)*4,source=character*16*4;
        mod.thDirtyMark?.(table+dest,64);
        if(seat===localSlot)mod.HEAPU8.fill(0,table+dest,table+dest+64);
        else mod.HEAPU8.set(originalDim.subarray(source,source+64),table+dest);
      }
    }
  },clearLayers(){
    // Coverage gates every per-seat record, so stale records are inaccessible.
    mod.thDirtyMark?.(tags,1048576);mod.thDirtyMark?.(coverage,524288);
    mod.HEAPU8.fill(0,tags,tags+1048576);mod.HEAPU8.fill(0,coverage,coverage+524288);
  },view:slot=>api.thLayerSetView(Number.isInteger(slot)&&slot>=0&&slot<3?slot:-1),bind(mailbox){
    mod.sidebarPaletteAddress=mailbox<0?-1:mailbox-patch.mailbox_cs_offset+(patch.data_segment-patch.code_segment)*16+0x2a82;
    mod.thDirtyMark?.(control,32);v=new DataView(mod.HEAPU8.buffer);
    v.setUint32(control,mailbox<0?0:mailbox-patch.mailbox_cs_offset+info.context_cs_offset,true);
    v.setUint32(control+12,mailbox<0?0:mailbox+25,true);
    v.setUint32(control+24,mailbox<0||!Number.isInteger(patch.body_layer_cs_offset)?0:mailbox-patch.mailbox_cs_offset+patch.body_layer_cs_offset,true);
  }};
}
