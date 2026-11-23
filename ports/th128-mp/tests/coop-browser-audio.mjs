// Browser QA instruments Web Audio calls but delegates to the actual device/context/decoder.
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {readZipDirectory,readEntryData} from '../../th128/site/runtime/zip-reader.mjs';

export async function pinnedAudio(packagePath){
 const zip=bytes=>new Map(readZipDirectory(bytes).entries.map(e=>[e.name,e]));
 const json=async e=>JSON.parse(new TextDecoder().decode(await readEntryData(e)));
 const outer=zip(await readFile(packagePath)),manifest=await json(outer.get('package.json'));
 const lang=zip(await readEntryData(outer.get(manifest.files[manifest.components.language.entries[0].file].source)));
 const asset=(await json(lang.get('runtime-assets.json'))).resources;
 const bindings=await json(lang.get(asset['/assets/runtime/sound-bindings.json'].entry));
 const menu=Object.fromEntries([7,9,10,14].map(id=>{
   const name=bindings.sounds[id].name,file=Object.entries(asset).find(([key])=>key.endsWith('/sounds/'+name))?.[1];
   assert.ok(file?.sha256,`native menu cue ${id} must exist in the installed package`);return [id,file.sha256];
 }));
 return {title:manifest.files['ogg:th128_07.ogg'].sha256,firstStage:manifest.files['ogg:th128_00.ogg'].sha256,extra:manifest.files['ogg:th128_05.ogg'].sha256,menu};
}
export function instrumentAudio(){
 if(!location.pathname.includes('/runtime/th128/multiplayer/')||!globalThis.AudioContext)return;
 const contexts=new Map(),buffers=new WeakMap(),sourceIds=new WeakMap(),started=[],stopped=[];let next=0;
 const lane=context=>{if(!contexts.has(context))contexts.set(context,contexts.size+1);return contexts.get(context);};
 const decode=BaseAudioContext.prototype.decodeAudioData;
 BaseAudioContext.prototype.decodeAudioData=function(bytes,...rest){
   if(rest.length)return decode.call(this,bytes,...rest);
   lane(this);const hash=crypto.subtle.digest('SHA-256',bytes);
   return Promise.all([decode.call(this,bytes),hash]).then(([buffer,digest])=>{
     buffers.set(buffer,Array.from(new Uint8Array(digest),x=>x.toString(16).padStart(2,'0')).join(''));return buffer;
   });
 };
 const start=AudioBufferSourceNode.prototype.start,stop=AudioBufferSourceNode.prototype.stop;
 AudioBufferSourceNode.prototype.start=function(...args){
   const result=start.apply(this,args),id=++next;sourceIds.set(this,id);
   started.push({id,context:lane(this.context),loop:this.loop,digest:buffers.get(this.buffer),at:this.context.currentTime});return result;
 };
 AudioBufferSourceNode.prototype.stop=function(...args){
   const result=stop.apply(this,args);stopped.push({id:sourceIds.get(this),when:args[0]??0});return result;
 };
 globalThis.__coopAudioReadback=()=>({started,stopped,lanes:Array.from(contexts,([ctx,id])=>({id,state:ctx.state,time:ctx.currentTime}))});
}
export const readAudio=frame=>frame.evaluate(()=>globalThis.__coopAudioReadback?.());
export const audioLane=(report,source)=>report.lanes.find(c=>c.id===source.context);
export const liveLoop=(report,digest)=>report?.started.findLast(s=>s.loop&&(!digest||s.digest===digest)&&!report.stopped.some(t=>t.id===s.id));
