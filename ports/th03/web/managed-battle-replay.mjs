import {createReplica} from './replay/replica.js';
import {createReplayAudio} from './replay/audio.js';

// Rebuild the confirmed prefix without presenting selection, then show the match.
export async function startBattleReplay({tape,canvas,music,onError,onStatus=()=>{}}) {
  const context=canvas.getContext('2d',{alpha:false});
  if(!context)throw Error('Replay 画面不可用');
  let stopped=false,frames=0,timer=0,replica,paused=false,rate=1,target=tape.matchStart,working=false,revision=0;
  const rows=tape.rows,start=tape.matchStart,audio=createReplayAudio('03',onError,{language:tape.header.settings.language,musicEnabled:music!=='none'});
  void audio.enable();
  async function boot(){return createReplica(tape.header,{onFrame:bitmap=>{context.drawImage(bitmap,0,0);frames++;},onEvent:event=>audio.event(event)});}
  try{replica=await boot();}catch(error){audio.stop();throw error;}
  const status=()=>({frame:replica?.frame??0,total:rows.length,startFrame:start,paused,seeking:target!==null,target,rate,ended:replica?.frame===rows.length});
  function schedule(delay=0){if(!stopped&&!timer)timer=setTimeout(()=>{timer=0;void pump();},delay);}
  async function pump(){
    if(stopped||working)return;working=true;
    try{
      const destination=target,token=revision;
      if(destination!==null&&destination<replica.frame){replica.stop();const next=await boot();if(stopped){next.stop();return;}replica=next;}
      if(destination!==null&&replica.frame<destination){
        audio.mode(true);await replica.advanceBatch(rows.slice(replica.frame,Math.min(destination,replica.frame+32)));
      }else{
        if(destination!==null&&token===revision){target=null;await replica.present();}
        if(!paused&&replica.frame<rows.length){audio.mode(false);await replica.advance(rows[replica.frame]);await replica.present();}
        else audio.mode(true);
      }
      if(!stopped){onStatus(status());if(target!==null||!paused&&replica.frame<rows.length)schedule(target!==null||rate===0?0:Math.max(0,tape.header.tickMs/rate));}
    }catch(error){if(!stopped)onError(error);}finally{working=false;if(!stopped&&target!==null&&!timer)schedule();}
  }
  await replica.present();schedule();
  return {
    health:()=>({frames,gameFrame:replica.frame,audio:audio.info()}),status,
    control(action,value){
      if(action==='pause'){paused=true;audio.mode(true);}
      else if(action==='play'){if(replica.frame===rows.length)target=start;paused=false;if(target===null)audio.mode(false);}
      else if(action==='seek'){if(!Number.isSafeInteger(value)||value<start||value>rows.length)throw Error('录像位置无效');target=value;revision++;audio.mode(true);}
      else if(action==='rate'){if(![0,.5,1,2,4,8].includes(value))throw Error('录像速度无效');rate=value;}
      else throw Error('录像操作无效');
      onStatus(status());schedule();
    },
    stop(){stopped=true;revision++;clearTimeout(timer);replica.stop();audio.stop();}
  };
}
