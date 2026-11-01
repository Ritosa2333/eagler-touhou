import {ORIGINAL_SFX} from './audio-layout.mjs';
// Adapted from PR #143 / th05-coop-lab web/netplay/local-sfx.js, pinned at
// 6eb46ed1d4f581f109ffa494ea0d21d54c8b38df. The original FM effect part has
// one voice; later native triggers replace the current sound with a short fade.
export function createSfx(context,bytes){
 const manifest=ORIGINAL_SFX;
 if(!(bytes instanceof Uint8Array)||bytes.length!==manifest.bytes)throw Error('PR #143 FM effect bank is missing');
 const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),buffers=new Map();let active=null,pending=null,closed=false,played=0,superseded=0;
 for(const effect of manifest.effects){if(!Number.isInteger(effect.id)||effect.id<0||effect.id>15||!Number.isInteger(effect.offset)||!Number.isInteger(effect.frames)||effect.offset<0||effect.frames<0||(effect.offset+effect.frames)*2>bytes.length)throw Error('Invalid FM effect bank');if(!effect.frames)continue;const buffer=context.createBuffer(1,effect.frames,manifest.sampleRate),pcm=buffer.getChannelData(0);for(let i=0;i<effect.frames;i++)pcm[i]=view.getInt16((effect.offset+i)*2,true)/32768;buffers.set(effect.id,buffer);}
 function flush(){if(!active)return;const item=active;active=null;const now=context.currentTime;item.gain.gain.cancelScheduledValues(now);item.gain.gain.setValueAtTime(1,now);item.gain.gain.linearRampToValueAtTime(0,now+.003);try{item.source.stop(now+.003);}catch{}}
 return {
  trigger(id){if(!closed&&Number.isInteger(id)&&id>=0&&id<=15){if(pending!==null)superseded++;pending=id;}},
  pump(){const id=pending;pending=null;if(id===null||closed||context.state!=='running')return;flush();const buffer=buffers.get(id);if(!buffer)return;const source=context.createBufferSource(),gain=context.createGain();source.buffer=buffer;source.connect(gain);gain.connect(context.destination);const item={source,gain,id};active=item;source.onended=()=>{source.disconnect();gain.disconnect();if(active===item)active=null;};source.start();played++;},
  clear(){pending=null;flush();},
  info:()=>({backend:'pr143-fm-pcm',played,superseded,active:active?.id??null}),
  close(){closed=true;pending=null;flush();buffers.clear();},
 };
}
