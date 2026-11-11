import {Fat12,installNativePause,nativeAssets,sha256} from '../disk.js';
import {unlockDiskScores} from '../scores.js';
import {installNetworkAudio} from '../network-audio.js';
import {loadVerifiedBytes,loadNetworkAssets} from '../startup-assets.js';
export async function bootBattleResources(settings){
  const {language,difficulty,clock,rollback,focusEnabled,touchUnlimitedAllowed,hostSeat}=settings;
  if(!['cn','jp'].includes(language)||!Number.isInteger(Number(difficulty))||Number(difficulty)<0||Number(difficulty)>3||![0,1].includes(hostSeat))throw Error('对战录像开局配置无效');
  const response=await fetch('disks/manifest.json');if(!response.ok)throw Error('对战磁盘清单缺失');
  const meta=(await response.json()).games['3-'+language];if(!meta)throw Error('对战语言无效');
  const data=(await loadVerifiedBytes(meta.url,meta.sha256,{gzip:true})).slice();
  const decode=(at,n)=>new TextDecoder().decode(data.subarray(at,at+n));
  if(data.length!==meta.size||decode(meta.autoexecSizeOffset-28,11)!=='AUTOEXECBAT'||decode(meta.languageMarkerOffset,8)!==meta.languageMarker)throw Error('对战磁盘布局不一致');
  unlockDiskScores(new Fat12(data));await installNativePause(data,language);const music=await installNetworkAudio(data);
  const script=new TextEncoder().encode('@ECHO OFF\r\nPATH A:\\DOS;A:\\\r\nSET TEMP=A:\\DOS\r\nSET DOSDIR=A:\\DOS\r\nA:\r\nCD \\YUMEZIKU\r\nCALL GAME.BAT\r\n');
  if(script.length>meta.autoexecCapacity)throw Error('对战启动脚本越界');
  data.fill(0,meta.autoexecOffset,meta.autoexecOffset+meta.autoexecCapacity);data.set(script,meta.autoexecOffset);new DataView(data.buffer).setUint32(meta.autoexecSizeOffset,script.length,true);
  const cfg=meta.keyboardConfig;if(cfg.size!==8||cfg.offset<0||cfg.offset+8>data.length)throw Error('对战键盘配置越界');
  data[cfg.offset]=1;data[cfg.offset+1]=0;data[cfg.offset+2]=Number(difficulty);
  const r=await fetch('lockstep-runtime.json',{cache:'no-cache'});if(!r.ok)throw Error('对战同步清单缺失');const runtime=await r.json();
  const wasmBinary=await loadNetworkAssets(runtime,()=>{},{rollbackEnabled:rollback});
  return {data,wasmBinary,patch:(await nativeAssets()).meta,music,opening:{protocol:runtime.protocol,files:runtime.files,disk:await sha256(data),hostSeat,language,difficulty:String(difficulty),clock:String(clock),rollback,focusEnabled,touchUnlimitedAllowed}};
}
