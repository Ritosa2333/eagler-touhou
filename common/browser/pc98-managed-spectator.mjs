import {createLocalSpectator} from './pc98-local-spectator.mjs';
import {createReplica} from './replay/replica.js';
import {validateHeader,validateRow,stable} from './replay/tape.js';
import {createReplayAudio} from './replay/audio.js';

export function createManagedSpectator(game,emit){
  let viewer,canvas,audio;
  return {
    async start({options,music}){
      canvas=document.createElement('canvas');canvas.width=640;canvas.height=400;
      Object.assign(canvas.style,{position:'fixed',inset:'0',width:'100%',height:'100%',objectFit:'contain',imageRendering:'pixelated',background:'#000',zIndex:'2'});
      document.body.append(canvas);const context=canvas.getContext('2d',{alpha:false});
      audio=createReplayAudio(game,error=>emit({event:'error',message:error.message}));
      if(music!=='none'){const active=audio;void active.enable().then(()=>{if(audio===active)active.mode(false);});}
      viewer=createLocalSpectator({game,options,createReplica,validateHeader,validateRow,stable,
        onFrame:bitmap=>context.drawImage(bitmap,0,0),onEvent:event=>audio.event(event),onError:error=>emit({event:'error',message:error.message})});
    },
    health(){return {frames:viewer?.frames??0,gameFrame:viewer?.frame??0,running:!!viewer,audio:{state:'running'}};},
    playback(message){if(message.action==='pause'){viewer.pause(true);audio.mode(true);return {};}
      if(message.action==='play'||message.action==='live'){viewer.pause(false);audio.mode(false);return {};}
      throw Error('观战命令无效');},
    files:message=>globalThis.__eaglerPC98FileCommands(message),
    stop(){viewer?.stop();viewer=null;audio?.stop();audio=null;canvas?.remove();canvas=null;},
  };
}
