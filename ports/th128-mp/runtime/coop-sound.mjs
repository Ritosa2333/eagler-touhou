import {GameSoundQueue,StageMusic} from '../../th128/site/runtime/sound.mjs';
import {safeAssetUrl} from '../../th128/site/inspector.mjs';

const menuIds=new Set([7,9,10,14,18]);
const boundSounds=(bindings,ids)=>({...bindings,sounds:Object.fromEntries(Object.entries(bindings.sounds??{}).filter(([id])=>ids.has(Number(id))))});

// As in the ordinary game, suspend the World context while the menu uses a
// separate context. The frozen AudioBufferSource is resumed, not restarted.
// Neither output context participates in simulation, rollback or room readiness.
export async function openCoopSound({resources,signal,events=globalThis.window,eventTargets=events?[events]:[],createContext=()=>new AudioContext(),
  makeQueue=(ctx,gain)=>new GameSoundQueue(ctx,gain),musicEnabled=false,stageEnabled=true,makeMusic=(ctx,gain)=>new StageMusic(ctx,gain)}){
  signal.throwIfAborted();
  const owner=new AbortController(),ctx=createContext(),gain=ctx.createGain(),queue=makeQueue(ctx,gain);
  let closed=false,stages=null,music=null,ending=null,endingActive=false,phase='playing';
  let menuCtx,menuGain,menuQueue,menuResume=null,queuedCue=null;
  function close(){
    if(closed)return;closed=true;signal.removeEventListener('abort',close);owner.abort();queuedCue=null;
    try{ending?.dispose();music?.dispose();queue.dispose();menuQueue?.dispose();}
    finally{gain.disconnect();menuGain?.disconnect();for(const lane of [ctx,menuCtx])if(lane&&lane.state!=='closed')void lane.close().catch(()=>{});}
  }
  const gameWanted=()=>phase==='playing'||endingActive;
  const resumeGame=()=>{if(!closed&&ctx.state!=='closed')void ctx.resume().catch(()=>{});};
  const suspendGame=()=>{if(!closed&&ctx.state!=='closed')void ctx.suspend().catch(()=>{});};
  function emit(target,lane,batch){
    if(!batch?.length)return;
    const present=lane.state==='running';
    for(const {id,x,op} of batch)target.consumeNativeRequest(id,x,op,{present});
    target.flush({present});
  }
  function wakeMenu(){
    if(closed||menuCtx.state==='running'||menuCtx.state==='closed'||menuResume)return;
    menuResume=menuCtx.resume().then(()=>{
      const cue=queuedCue;queuedCue=null;
      if(!closed&&cue&&performance.now()-cue.at<350)emit(menuQueue,menuCtx,[{id:cue.id,x:0,op:0}]);
    }).catch(()=>{}).finally(()=>{menuResume=null;});
  }
  function unlock(){
    if(closed)return;
    if(gameWanted()&&ctx.state==='suspended')resumeGame();
    if(menuCtx.state==='suspended')wakeMenu();
  }
  try{
    signal.addEventListener('abort',close,{once:true});
    menuCtx=createContext();menuGain=menuCtx.createGain();menuQueue=makeQueue(menuCtx,menuGain);
    gain.connect(ctx.destination);menuGain.connect(menuCtx.destination);
    // Browser autoplay may awaken a previously pending resume after the room
    // paused. The room phase still owns the final disposition of the clock.
    ctx.addEventListener?.('statechange',()=>{if(ctx.state==='running'&&!gameWanted())suspendGame();},{signal:owner.signal});
    for(const target of eventTargets)for(const e of ['pointerdown','keydown'])target.addEventListener(e,unlock,{signal:owner.signal,capture:true});
    const bindings=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(await resources.read('soundBindings')));
    await Promise.all([queue.load(stageEnabled?bindings:boundSounds(bindings,new Set([0])),resources.base,safeAssetUrl,{signal}),
      menuQueue.load(boundSounds(bindings,menuIds),resources.base,safeAssetUrl,{signal})]);
    signal.throwIfAborted();
    if(musicEnabled&&stageEnabled){
      music=makeMusic(ctx,gain);const catalog=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(await resources.read('atlas')));
      stages=new Map(catalog.stages.map(s=>[s.id,s.music]));
      await music.load(resources.musicTracks,resources.base,safeAssetUrl,{signal,tracks:[...new Set([...stages.values()].flat())]});
      signal.throwIfAborted();
    }
  }catch(error){close();throw error;}
  function setBattlePhase(next){
    if(closed||phase===next)return;
    if(!['playing','paused','stopped'].includes(next))throw Error('Invalid co-op audio phase');
    phase=next;
    if(next==='stopped'){
      music?.stopSource();
      // Retire looping SE from the old round too, but keep the decoded samples
      // available for a jointly restarted battle or the terminal presentation.
      if(queue.channels?.size||queue.retiredChannels?.size){
        for(const id of queue.channels.keys())queue.stop(id,{present:true});
        queue.flush({present:true});
      }
    }
    // Issue resume on the transition even if suspend hasn't reached the audio
    // thread yet. The two commands will execute in ownership order.
    if(gameWanted())resumeGame();else suspendGame();
  }
  function menuCue(id){
    if(closed||!menuIds.has(id))return;
    if(menuCtx.state==='running'){queuedCue=null;emit(menuQueue,menuCtx,[{id,x:0,op:0}]);}
    else{queuedCue={id,at:performance.now()};wakeMenu();}
  }
  async function prepareEnding(){
    if(closed)return;
    endingActive=true;resumeGame();
    if(!musicEnabled||ending)return;
    ending=makeMusic(ctx,gain);
    await ending.load(resources.musicTracks,resources.base,safeAssetUrl,{signal,tracks:['th128_09.wav']});signal.throwIfAborted();
  }
  function endingMusic(command){if(closed||!endingActive||!ending)return;if(command.operation===8){music?.stopSource();ending.start();}else ending.fadeOutFrames(command.durationFrames);}
  function endingSounds(batch){if(!closed&&endingActive)emit(queue,ctx,batch);}
  function finishEnding(){
    if(closed||!endingActive)return;
    endingActive=false;ending?.dispose();ending=null;if(!gameWanted())suspendGame();
  }
  function commit(batch,effects){
    if(closed||phase==='stopped')return;
    if(effects?.musicTrack&&music){const [stage,index]=effects.musicTrack,name=stages.get(stage)?.[index];if(!name)throw Error('Confirmed track is not in the original catalog');music.switchTo(name);}
    if(effects?.musicFade!=null&&music)music.fadeOutFrames(effects.musicFade);
    if(phase==='playing')emit(queue,ctx,batch);
  }
  return Object.freeze({prepareEnding,endingMusic,endingSounds,finishEnding,commit,setBattlePhase,menuCue,close});
}
