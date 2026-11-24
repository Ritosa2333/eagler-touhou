import {StageMusic} from '../../th128/site/runtime/sound.mjs';
import {safeAssetUrl} from '../../th128/site/inspector.mjs';

// retail play.mjs uses 07 for Main; 00 is the first stage, not the title.
const titleTrack='th128_07.wav';
// Decode only the actual title BGM. This owner never touches a match audio clock.
export function createCoopTitleMusic({resources,signal,eventTargets,onState=()=>{},onError=()=>{},createContext=()=>new AudioContext(),makeMusic=(ctx,gain)=>new StageMusic(ctx,gain)}){
 const owner=new AbortController();let ctx,gain,music,loading=null,loaded=false,active=false,playing=false,closed=false;
 function tryStart(){
   if(!active||closed||!loaded||playing)return;
   if(ctx.state!=='running'){onState('waiting-gesture');return;}
   music.start();playing=true;onState('playing');
 }
 function gesture(){if(active&&!closed&&ctx?.state==='suspended')void ctx.resume().then(tryStart).catch(()=>{});}
 for(const target of eventTargets)for(const type of ['pointerdown','keydown'])target.addEventListener(type,gesture,{signal:owner.signal,capture:true});
 function hide(){active=false;if(playing){music.stopSource();playing=false;}onState('off');}
 function show(enabled){
   if(closed)return;
   if(!enabled){hide();return;}
   active=true;
   try{
     if(!ctx){ctx=createContext();gain=ctx.createGain();gain.connect(ctx.destination);music=makeMusic(ctx,gain);}
     if(!loaded&&!loading){
       onState('loading');loading=music.load(resources.musicTracks,resources.base,safeAssetUrl,{signal:owner.signal,tracks:[titleTrack]}).then(()=>{if(!closed){loaded=true;tryStart();}})
         .catch(error=>{if(!closed&&active){onState('unavailable');onError(error);}}).finally(()=>{loading=null;});
     }
     tryStart();gesture();
   }catch(error){if(!closed&&active){onState('unavailable');onError(error);}}
 }
 function close(){if(closed)return;hide();closed=true;owner.abort();music?.dispose();gain?.disconnect();if(ctx?.state!=='closed')void ctx?.close().catch(()=>{});}
 signal.addEventListener('abort',close,{once:true});if(signal.aborted)close();
 return Object.freeze({show,hide,close});
}
