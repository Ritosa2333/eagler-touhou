import {PmdPlayer} from './pmd-player.mjs';
const room=['op','select','00mm','01mm','02mm','03mm','04mm','05mm','06mm','dec','07mm','08mm','demo1','demo2','demo3','demo4','demo5','ed'];
const stages=['00mm','01mm','02mm','03mm','04mm','05mm','06mm','dec','07mm','08mm'];
export function musicTrack(cue,parameter){return (cue===1?'op':cue===2?'select':cue===3?stages[parameter]:cue===4?room[parameter]:cue===5?'win':cue===6?'over':cue===7?'ed':cue===8?'score':cue>=9&&cue<=13?'demo'+(cue-8):null)??null;}
export function createNativeMusic({source='original',muted=false,onError=()=>{},readTrack,preparePmd=PmdPlayer.prepare}={}){
 let element,manifest,loaded,track=null,closed=false,desired=false,generation=0,context,pmd,pmdLoading,pmdSource,paused=false;
 async function catalog(){return loaded??=(async()=>{const response=await fetch('./bgm/akyu/manifest.json');if(!response.ok)throw Error('幺乐团音乐清单加载失败');return (await response.json()).tracks;})();}
 function stop(){desired=false;generation++;if(element){element.pause();element.removeAttribute('src');element.load();}if(pmdSource){pmdSource.stop(context.currentTime);pmdSource=undefined;}track=null;}
 async function play(){if(muted||!track||closed)return;const id=track,own=++generation;try{
  if(source==='original'){
   if(!context||!readTrack)return;
   pmdLoading??=preparePmd(context,new URL('.',import.meta.url));pmd??=await pmdLoading;
   if(closed||own!==generation||!desired)return;
   const bytes=readTrack(id),next=await pmd.createSource(bytes);
   if(closed||own!==generation||!desired){next.stop(context.currentTime);return;}
   if(pmdSource)pmdSource.stop(context.currentTime);pmdSource=next;next.connect(context.destination);if(!paused)next.start();return;
  }
  if(source!=='akyu')return;
  manifest??=await catalog();const item=manifest[id+'-m26'];if(!item)throw Error('缺少音乐：'+id);if(closed||own!==generation||!desired)return;
  element??=new Audio();element.loop=!!item.fullLoop;element.src='./bgm/'+item.url;element.volume=.7;await element.play();
 }catch(error){if(!closed&&own===generation&&error?.name!=='NotAllowedError')onError(error);}}
 return {commands(bytes){if(muted||!bytes)return;const v=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);for(let i=0;i+3<bytes.length;i+=4){const type=bytes[i],cue=bytes[i+1],parameter=v.getUint16(i+2,true);if(type===0){const next=musicTrack(cue,parameter);if(next)track=next;}else if(type===1){track=musicTrack(cue,parameter)??track;desired=true;paused=false;void play();}else if(type===2)stop();else if(type===3){paused=true;element?.pause();pmdSource?.pause();}else if(type===4&&desired){paused=false;if(element)void element.play().catch(()=>{});pmdSource?.start();}else if(type===5&&element)element.volume=0;}}
 ,unlock(audio){if(audio&&!context)context=audio;if(source==='original'&&desired&&!pmdSource)void play();if(desired&&element?.paused&&!paused)void element.play().catch(()=>{});},stop(){closed=true;stop();pmd?.dispose();},get track(){return track;},get playing(){return source==='original'?!!pmdSource&&!paused:!!element&&!element.paused;}};
}
