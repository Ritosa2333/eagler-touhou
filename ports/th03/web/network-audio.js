import {Fat12} from './disk.js';
import {loadVerifiedBytes} from './startup-assets.js';

let assets;
export function networkAudioAssets(){
  return assets ||= (async()=>{
    const response=await fetch(new URL('./network-audio.json',import.meta.url),{cache:'no-cache'});
    if(!response.ok)throw Error('联机音效桥清单加载失败');
    const meta=await response.json();
    const bytes=await loadVerifiedBytes(new URL(meta.url,import.meta.url).href,meta.sha256);
    if(bytes.length!==meta.bytes||meta.soundEffectCommand!==12)throw Error('联机音效桥版本不匹配');
    return {meta,bytes};
  })();
}
export async function installNetworkAudio(data){
  const {meta,bytes}=await networkAudioAssets();
  // Only the online private disk gets this resident wrapper. Original game
  // arbitration/PMD state and the local/solo driver bytes remain intact.
  new Fat12(data).put('YUMEZIKU/WEBMUSIC.COM',bytes);
  return meta;
}
