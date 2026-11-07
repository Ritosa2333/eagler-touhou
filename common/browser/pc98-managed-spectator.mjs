import {createLocalSpectator} from './pc98-local-spectator.mjs';
import {createReplica} from './replay/replica.js';
import {validateHeader,validateRow,stable} from './replay/tape.js';
import {createPc98SpectatorAudio} from './pc98-spectator-audio.mjs';
import {validatePointIndex} from './replay/network.js';
import {readLocalPoint} from './replay/local-points.js';
import {createPc98SpectatorChannel} from './pc98-spectator-channel.mjs';

export function createManagedSpectator(game,emit,{createAudio=createPc98SpectatorAudio}={}){
  let viewer,canvas,audio,timer,run,transport,revision=0;
  function report(){if(!viewer)return;const position=viewer.frame,received=viewer.received;
    audio?.mode(viewer.paused||!viewer.following||viewer.catching||viewer.seeking);
    emit({event:'runtime-info',spectatorPlayback:{version:1,run,revision:++revision,min:viewer.min,position,received,edge:received,target:viewer.target,
      paused:viewer.paused,seeking:viewer.seeking,following:viewer.following,ended:viewer.ended,nodes:viewer.nodes}});}
  return {
    async start({options,music}){
      const url=new URL(options.netplayUrl);run='pc98_'+game+'_'+url.searchParams.get('run');revision=0;
      canvas=document.createElement('canvas');canvas.width=640;canvas.height=400;
      Object.assign(canvas.style,{position:'fixed',inset:'0',width:'100%',height:'100%',objectFit:'contain',imageRendering:'pixelated',background:'#000',zIndex:'2'});
      document.body.append(canvas);const context=canvas.getContext('2d',{alpha:false});
      audio=createAudio(game,music,error=>emit({event:'error',message:error.message}));
      viewer=createLocalSpectator({game,options,createReplica,validateHeader,validateRow,validatePointIndex,readLocalPoint,stable,smoothPresentation:game==='03',
        channelFactory:()=>{const channel=createPc98SpectatorChannel(options,error=>{viewer?.stop();emit({event:'error',message:error.message});});transport={peers:new Map(),get relay(){return channel.relay;}};globalThis['__th'+game+'PeerTransport']=transport;return channel;},
        onHeader:header=>{void audio.start(header.settings);},
        onFrame:bitmap=>context.drawImage(bitmap,0,0),onEvent:event=>audio.event(event),onError:error=>emit({event:'error',message:error.message})});
      timer=setInterval(report,200);
    },
    health(){return {frames:viewer?.frames??0,gameFrame:viewer?.frame??0,running:!!viewer,audio:audio?.info?.()??{state:'loading'}};},
    playback(message){if(message.run!==run)throw Error('观战对局已改变');
      if(message.action==='pause'){viewer.pause(true);audio.mode(true);report();return {};}
      if(message.action==='play'){viewer.pause(false);audio.mode(false);report();return {};}
      if(message.action==='live'){viewer.live();audio.mode(false);report();return {};}
      if(message.action==='seek'){viewer.seek(message.frame);audio.mode(true);report();return {};}
      throw Error('观战命令无效');},
    files:message=>globalThis.__eaglerPC98FileCommands(message),
    stop(){clearInterval(timer);viewer?.stop();viewer=null;audio?.stop();audio=null;canvas?.remove();canvas=null;if(globalThis['__th'+game+'PeerTransport']===transport)delete globalThis['__th'+game+'PeerTransport'];transport=null;emit({event:'runtime-info',spectatorPlayback:null});},
  };
}
