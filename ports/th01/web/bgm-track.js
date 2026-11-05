import {bgmVersion} from './bgm-version.js';
import {assetBytes,freshJSON} from './preload-cache.js';
let alternate;
async function json(path){return freshJSON(path.replace(/^\.\//,''));}
export async function preloadMusicTrack(key){
  const version=bgmVersion();
  let track;
  if(version==='new'){
    if(!alternate){alternate=json('./bgm/akyu/manifest.json');alternate.catch(()=>{alternate=null;});}
    track=(await alternate).tracks[key];
  }
  track??=await json(`./bgm/${key}.json`);
  const bytes=await assetBytes(`bgm/${track.url}`,{hash:track.audioSha256,size:track.bytes});
  return {bytes,track};
}
export async function loadMusicTrack(context,key){
  const {bytes,track}=await preloadMusicTrack(key);
  const buffer=await context.decodeAudioData(bytes.buffer);
  return [buffer,{loopStart:track.loopStart,loopEnd:track.fullLoop?buffer.duration:track.loopEnd}];
}
