import {fetchResource} from '../resource-fetch.mjs';
import {musicOutput} from './music-output.mjs';
export class StageMusic {
  constructor(context,gain){this.context=context;this.gain=gain;this.output=context.createGain();this.output.gain.value=1;this.output.connect(musicOutput(context,gain));this.track=null;this.buffer=null;this.source=null;this.fade=null;this.disposed=false;this.preloaded=new Map();}
  setPracticeOwner(owner,{enabled,section,replay}){
    if(this.disposed||!owner||typeof enabled!=='boolean'||!Number.isSafeInteger(section)||section<0||typeof replay!=='boolean')throw new Error('Invalid practice BGM owner');
    this.practice={owner,enabled,section,replay};
  }
  retainPracticeOwner(owner,next,name){
    const p=this.practice;
    if(this.disposed||!p||p.owner!==owner||!next||!p.enabled||p.section===0||p.replay||!this.source||this.fade||this.track?.name!==name)return false;
    this.practice={...p,owner:next};return true;
  }
  async load(tracks,base,resolveUrl,{signal,tracks:names=['th128_00.wav']}={}){
    if(this.disposed||signal?.aborted)throw new DOMException('Aborted music load','AbortError');
    this.cancelFade();
    if(!Array.isArray(names)||names.length<1||names.length>10||new Set(names).size!==names.length)throw new Error('Invalid original track preload set');
    const loaded=new Map();
    for(const name of names){
    const selected=tracks.filter(track=>track.name===name);
    if(selected.length!==1||!/^th128_\d\d\.wav$/.test(name))throw new Error('Original stage music metadata is unavailable');
    const track=selected[0];
    const oggFile=`music/ogg/${name.slice(0,-4)}.ogg`;
    const isOgg=track.file===oggFile;
    if(!(track.file===`music/${name}`||isOgg)||!Number.isFinite(track.loopStart)||!Number.isFinite(track.loopEnd)
      ||track.loopStart<0||track.loopEnd<=track.loopStart||track.loopEnd>600||track.sampleRate!==44100||track.channels!==2)
      throw new Error('Invalid original music loop metadata');
    if(isOgg&&(track.codec!=='vorbis'||!Number.isSafeInteger(track.bytes)||track.bytes<1||!/^[0-9a-f]{64}$/.test(track.sha256)))
      throw new Error('Invalid original OGG music file receipt');
    const response=await fetchResource(resolveUrl(base,track.file),{signal});
    if(!response.ok)throw new Error('Original stage music asset unavailable');
    const buffer=await this.context.decodeAudioData(await response.arrayBuffer());
    if(this.disposed||signal?.aborted)throw new DOMException('Aborted music decoding','AbortError');
    if(!Number.isFinite(buffer.duration)||buffer.duration+1/track.sampleRate<track.loopEnd)throw new Error('Original music loop exceeds decoded PCM');
    loaded.set(name,{track:{...track},buffer});
    }
    this.preloaded=loaded;const first=loaded.get(names[0]);this.track=first.track;this.buffer=first.buffer;
  }
  cancelFade(){
    if(!this.output)return;
    const now=this.context.currentTime;
    this.output.gain.cancelScheduledValues(now);
    this.output.gain.setValueAtTime(1,now);
    this.fade=null;
  }
  stopSource(){
    const source=this.source;
    if(!source)return;
    this.source=null;
    try{source.stop();}catch{}
    source.disconnect();
  }
  start(){
    if(this.disposed)throw new Error('Stage music is disposed');
    if(this.source)throw new Error('Stage music already started');
    if(!this.buffer||!this.track)throw new Error('Stage music is not loaded');
    this.cancelFade();
    const source=this.context.createBufferSource();
    source.buffer=this.buffer;source.loop=true;source.loopStart=this.track.loopStart;source.loopEnd=this.track.loopEnd;
    source.connect(this.output);source.start();this.source=source;
  }
  fadeOut(seconds){
    if(this.disposed)throw new Error('Stage music is disposed');
    if(!this.source)throw new Error('Stage music is not playing');
    if(!Number.isFinite(seconds)||seconds<0)throw new Error('Invalid original BGM fade duration');
    if(this.practice?.enabled&&this.practice.section>0&&!this.practice.replay)return {seconds,retained:true,source:this.source,owner:this.practice.owner};
    if(seconds===0){this.stopSource();this.cancelFade();return {seconds:0,immediate:true};}
    const now=this.context.currentTime;
    if(!Number.isFinite(now))throw new Error('Invalid audio clock');
    const end=now+seconds,target=Math.pow(10,-5000/2000);
    this.cancelFade();
    this.output.gain.cancelScheduledValues(now);
    this.output.gain.setValueAtTime(1,now);
    this.output.gain.exponentialRampToValueAtTime(target,end);
    const source=this.source;
    this.fade={seconds,start:now,end,source,target};
    source.onended=()=>{if(this.source===source){this.source=null;this.fade=null;}};
    try{source.stop(end);}catch{}
    return {seconds,start:now,end,target,immediate:false};
  }
  fadeOutFrames(frames){
    if(!Number.isInteger(frames)||frames<0)throw new Error('Invalid original BGM fade frame count');
    return this.fadeOut(frames/60);
  }
  snapshot(){return {track:this.track?.name??null,playing:Boolean(this.source)&&!this.disposed,loopStart:this.track?.loopStart??0,loopEnd:this.track?.loopEnd??0};}
  switchTo(name){
    if(this.disposed)throw new Error('Stage music is disposed');
    const next=this.preloaded.get(name);if(!next)throw new Error(`Original track was not preloaded: ${name}`);
    if(this.track?.name===name&&this.source)return;
    this.cancelFade();this.stopSource();
    this.track=next.track;this.buffer=next.buffer;this.start();
  }
  dispose(){
    if(this.disposed)return;this.disposed=true;
    this.cancelFade();this.stopSource();
    this.output.disconnect();this.output=null;this.fade=null;
    this.buffer=null;this.preloaded.clear();this.practice=null;
  }
}

export function directSoundPanGains(lPan){
  if(!Number.isInteger(lPan)||lPan< -10000||lPan>10000)throw new Error(`Invalid DirectSound pan ${lPan}`);
  if(lPan<0)return [1,Math.pow(10,lPan/2000)];
  if(lPan>0)return [Math.pow(10,-lPan/2000),1];
  return [1,1];
}

class SoundIdentityHost {
  constructor(){this.slots=Array.from({length:12},()=>({identifier:-1,count:0,pans:new Int32Array(128)}));}
  enqueue(identifier,pan){
    if(!Number.isInteger(identifier)||identifier< -1||identifier>=76||!Number.isInteger(pan)||pan< -2147483648||pan>2147483647)
      throw new Error('Unimplemented original sound identity/pan domain');
    for(const slot of this.slots){
      if(slot.identifier<0){
        if(slot.count<0||slot.count>=128)throw new Error('Unowned original inactive sound queue extent');
        slot.identifier=identifier;slot.pans[0]=pan;slot.count+=1;return true;
      }
      if(slot.identifier===identifier){
        if(slot.count<0||slot.count>=60)return false;
        slot.pans[slot.count]=pan;slot.count+=1;return true;
      }
    }
    return false;
  }
  stop(identifier){
    if(!Number.isInteger(identifier)||identifier<0||identifier>=76)throw new Error('Unimplemented original sound stop identity');
    for(const slot of this.slots){
      if(slot.identifier<0||slot.identifier===identifier){slot.identifier=identifier;slot.count=-1;return true;}
    }
    return false;
  }
  *drain(){
    for(const slot of this.slots){
      if(slot.identifier<0)break;
      const identifier=slot.identifier;
      if(slot.count>128)throw new Error('Unowned original sound queue read extent');
      let sum=0;
      for(let index=0;index<slot.count;index++)sum=(sum+slot.pans[index])|0;
      const event={identifier,kind:slot.count<0?'stop':'play',pan:slot.count>0?Math.trunc(sum/slot.count):0};
      slot.identifier=-1;slot.count=0;
      yield event;
    }
  }
  clear(){for(const slot of this.slots){slot.identifier=-1;slot.count=0;slot.pans.fill(0);}}
}

export class GameSoundQueue {
  #ownedIdentityHost;
  constructor(context,gain){
    this.context=context;this.gain=gain;this.buffers=new Map();this.channels=new Map();
    this.pendingIds=new Set();this.disposed=false;this.identityHost=new SoundIdentityHost();this.retiredChannels=new Set();
    this.#ownedIdentityHost=this.identityHost;
    this.maxChannels=12;this.maxQueuedPerChannel=60;
  }
  async load(bindings,base,resolveUrl,{signal}={}){
    for(const [identifier,sound] of Object.entries(bindings.sounds??{})){
      if(!/^[0-9]+$/.test(identifier)||!/^se_[A-Za-z0-9_-]+\.wav$/.test(sound.name)||!Number.isInteger(sound.volumeRaw))throw new Error('Invalid prepared native sound binding');
      const response=await fetchResource(resolveUrl(base,`sounds/${sound.name}`),{signal});
      if(!response.ok)throw new Error(`Original sound asset unavailable: ${sound.name}`);
      const buffer=await this.context.decodeAudioData(await response.arrayBuffer());
      if(this.disposed||signal?.aborted)return;
      if(buffer.numberOfChannels!==1&&buffer.numberOfChannels!==2)throw new Error('Unsupported original WAV channel count');
      this.buffers.set(Number(identifier),{buffer,volumeRaw:sound.volumeRaw,loop:Boolean(sound.loop)});
    }
  }
  enqueue(identifier,positionX){
    if(this.disposed)throw new Error('Game sound service is disposed');
    this.assertIdentityHost();
    const item=this.buffers.get(identifier);
    if((identifier!==-1&&!item)||!Number.isFinite(positionX))throw new Error(`Unimplemented original sound ID ${identifier}`);
    const nativePan=Math.trunc(positionX*1000/192);
    if(!this.identityHost.enqueue(identifier,nativePan))return false;
    if(identifier===-1)return true;
    let channel=this.channels.get(identifier);
    if(!channel){
      channel={item,pending:[],source:null,gain:null,pan:null};
      this.channels.set(identifier,channel);
    }
    channel.pending.push(nativePan);
    this.pendingIds.add(identifier);
    return true;
  }
  play(identifier,positionX){return this.enqueue(identifier,positionX);}
  acceptsSilentIdentity(){return !this.disposed&&this.identityHost===this.#ownedIdentityHost&&this.identityHost instanceof SoundIdentityHost;}
  assertIdentityHost(){
    if(this.disposed||this.identityHost!==this.#ownedIdentityHost||!(this.identityHost instanceof SoundIdentityHost))throw new Error('Original sound identity host is unavailable or changed');
    return this.identityHost;
  }
  consumeNativeRequest(identifier,positionX,operation,{present=true}={}){
    this.assertIdentityHost();
    if(typeof present!=='boolean'||!Number.isInteger(identifier)||!Number.isFinite(positionX)
      ||!Number.isInteger(operation)||![0,1,2].includes(operation)||(identifier<0&&(identifier!==-1||operation!==0)))
      throw new Error('Invalid original sound identity request');
    if(operation===0)return this.play(identifier,positionX);
    if(operation===1)return this.stop(identifier,{present});
    return this.position(identifier,positionX,{present});
  }
  position(identifier,positionX,{present=true}={}){
    this.assertIdentityHost();
    if(typeof present!=='boolean')throw new Error('Invalid sound presentation flag');
    if(this.disposed||!this.buffers.has(identifier)||!Number.isFinite(positionX))throw new Error(`Invalid original sound position ${identifier}`);
    const channel=this.channels.get(identifier);
    if(!channel||!channel.source||!channel.pan)return false;
    channel.logicalPan=Math.trunc(positionX*1000/192);
    if(present){
      const [leftGain,rightGain]=directSoundPanGains(channel.logicalPan);
      channel.pan.left.gain.value=leftGain;
      channel.pan.right.gain.value=rightGain;
    }
    return true;
  }
  flush({present=true}={}){
    if(this.disposed)throw new Error('Game sound service is disposed');
    this.assertIdentityHost();
    if(typeof present!=='boolean')throw new Error('Invalid sound presentation flag');
    if(present){for(const channel of this.retiredChannels)this.stopChannel(channel);this.retiredChannels.clear();}
    let started=0;
    for(const event of this.identityHost.drain()){
      const identifier=event.identifier;
      const channel=this.channels.get(identifier);
      if(event.kind==='stop'){
        if(channel){
          channel.pending.length=0;
          if(present)this.stopChannel(channel);else if(channel.source)this.retiredChannels.add(channel);
          this.channels.delete(identifier);
        }
        this.pendingIds.delete(identifier);continue;
      }
      if(!channel||channel.pending.length===0)continue;
      const pan=event.pan;
      channel.pending.length=0;
      this.pendingIds.delete(identifier);
      if(!present){
        if(channel.source){
          this.retiredChannels.add(channel);
          this.channels.set(identifier,{item:channel.item,pending:[],source:null,gain:null,pan:null});
        }
        continue;
      }
      this.stopChannel(channel);
      const channelCount=channel.item.buffer.numberOfChannels;
      if(channelCount!==1&&channelCount!==2)throw new Error('Unsupported original WAV channel count');
      const source=this.context.createBufferSource();
      const volume=this.context.createGain();
      const left=this.context.createGain();
      const right=this.context.createGain();
      const merger=this.context.createChannelMerger(2);
      const splitter=channelCount===2?this.context.createChannelSplitter(2):null;
      const [leftGain,rightGain]=directSoundPanGains(pan);
      source.buffer=channel.item.buffer;
      source.loop=Boolean(channel.item.loop);
      volume.gain.value=Math.pow(10,channel.item.volumeRaw/2000);
      left.gain.value=leftGain;
      right.gain.value=rightGain;
      if(splitter){
        source.connect(splitter);
        splitter.connect(left,0);
        splitter.connect(right,1);
      }else{
        source.connect(left);
        source.connect(right);
      }
      left.connect(merger,0,0);
      right.connect(merger,0,1);
      merger.connect(volume);
      volume.connect(this.gain);
      channel.source=source;channel.gain=volume;channel.pan={left,right,splitter,merger};channel.logicalPan=pan;
      source.onended=()=>{
        if(channel.source!==source)return;
        channel.source=null;
        this.releaseGraph(channel);
        this.retiredChannels.delete(channel);
      };
      source.start();
      ++started;
    }
    this.pendingIds.clear();
    return started;
  }
  stop(identifier,{present=true}={}){
    this.assertIdentityHost();
    if(typeof present!=='boolean')throw new Error('Invalid sound presentation flag');
    if(this.disposed||!this.buffers.has(identifier))throw new Error(`Unavailable original sound ID ${identifier}`);
    this.identityHost.stop(identifier);
    const channel=this.channels.get(identifier);
    if(!channel)return false;
    channel.pending.length=0;this.pendingIds.delete(identifier);
    if(present)this.stopChannel(channel);else if(channel.source)this.retiredChannels.add(channel);
    this.channels.delete(identifier);return true;
  }
  releaseGraph(channel){
    if(channel.gain){channel.gain.disconnect();channel.gain=null;}
    if(channel.pan){
      channel.pan.left.disconnect();channel.pan.right.disconnect();
      if(channel.pan.splitter)channel.pan.splitter.disconnect();
      channel.pan.merger.disconnect();channel.pan=null;
    }
  }
  stopChannel(channel){
    const source=channel.source;
    if(source){try{source.stop();}catch{}source.disconnect();channel.source=null;}
    this.releaseGraph(channel);
  }
  dispose(){
    if(this.disposed)return;
    this.disposed=true;
    for(const channel of this.channels.values())this.stopChannel(channel);
    for(const channel of this.retiredChannels)this.stopChannel(channel);
    this.retiredChannels.clear();
    this.channels.clear();this.pendingIds.clear();this.buffers.clear();this.identityHost.clear();
  }
}

const keyBits=new Map([['ArrowUp',0x10],['ArrowDown',0x20],['ArrowLeft',0x40],['ArrowRight',0x80],['KeyZ',1],['Enter',0x200],['KeyC',0x200],['ControlLeft',0x200],['ControlRight',0x200],['KeyX',2],['Escape',0x100]]);
// Rapid-fire aliases share the native confirm bit with Enter, but each source owns its hold.
function pressMenuKey(owner,code){
  const bit=keyBits.get(code);if(bit===undefined)return false;
  owner.heldKeys.add(code);if(!(owner.held&bit))owner.pressed|=bit;owner.held|=bit;return true;
}
function releaseMenuKey(owner,code){
  const bit=keyBits.get(code);if(bit===undefined||!owner.heldKeys.delete(code))return;
  if(![...owner.heldKeys].some(key=>keyBits.get(key)===bit))owner.held&=~bit;
}
export class OriginalPauseController{
  constructor({module,isCurrent=()=>true,releaseInput,consumeSound,audio,onResume,onTitle,onRestart,onPresentation=async()=>{},onReplayOpen=async()=>{throw new Error("Original replay save consumer is unavailable");}}){
    for(const name of ['_th128_world_pause_open','_th128_world_pause_tick','_th128_world_pause_commit','_th128_world_pause_state'])
      if(typeof module?.[name]!=='function')throw new Error('Original pause menu core is unavailable');
    if([isCurrent,releaseInput,consumeSound,onResume,onTitle,onRestart].some(value=>typeof value!=='function')
      ||['enter','leave','suspend'].some(name=>typeof audio?.[name]!=='function'))throw new Error('Original menu consumer is incomplete');
    this.module=module;this.isCurrent=isCurrent;this.releaseInput=releaseInput;this.consumeSound=consumeSound;this.audio=audio;
    this.onResume=onResume;this.onTitle=onTitle;this.onRestart=onRestart;this.onPresentation=onPresentation;this.onReplayOpen=onReplayOpen;this.wake=null;
    this.active=false;this.suspended=false;this.heldKeys=new Set();this.releaseKeys();this.busy=false;this.closed=false;
  }
  current(){return !this.closed&&this.isCurrent();}
  checked(result){
    if(result!==1)throw new Error(this.module.UTF8ToString(this.module._th128_world_error())||'Original menu operation rejected');
  }
  snapshot(){return JSON.parse(this.module.UTF8ToString(this.module._th128_world_pause_state()));}
  async open({alreadyOpened=false,replayFinished=false}={}){
    if(this.active||this.busy||!this.current())throw new Error('Original menu is already owned');
    this.busy=true;
    try{
      this.releaseInput();this.releaseKeys();
      await this.audio.enter();
      if(!this.current())return;
      if(!alreadyOpened)this.checked(this.module._th128_world_pause_open(replayFinished?2:0));
      const snapshot=this.snapshot();
      if(!snapshot.active||(replayFinished&&snapshot.kind!==3))throw new Error('Original menu owner was not created');
      await this.consumeSound();if(!this.current())return;this.active=true;this.suspended=false;
    }catch(problem){if(this.current())throw problem;}finally{this.busy=false;}
  }
  keyDown(code){
    if(!this.active||!this.current()||!pressMenuKey(this,code))return;
    if(this.suspended){this.suspended=false;this.wake=this.audio.enter().catch(error=>{if(this.current())this.failure=error;});}
  }
  keyUp(code){releaseMenuKey(this,code);}
  releaseKeys(){this.heldKeys.clear();this.held=0;this.pressed=0;}
  async step(){
    if(!this.active||this.suspended||!this.current())return;
    if(this.failure)throw this.failure;
    if(this.busy)throw new Error('Original menu tick is not serial');
    this.busy=true;
    try{
      if(this.wake){await this.wake;this.wake=null;if(!this.current())return;if(this.failure)throw this.failure;}
      const pressed=this.pressed;this.pressed=0;
      this.checked(this.module._th128_world_pause_tick(this.held,pressed));
      await this.consumeSound();if(!this.current())return;
      const snapshot=this.snapshot();await this.onPresentation(snapshot);if(!this.current())return;
      const action=snapshot.action;
      if(action===4){if(!snapshot.replayPhase)await this.onReplayOpen(snapshot);return;}
      if(!action)return;
      if(![1,2,3,6].includes(action))throw new Error('Original menu action has no complete consumer');
      this.checked(this.module._th128_world_pause_commit(action));
      this.active=false;this.releaseKeys();this.releaseInput();await this.audio.leave();if(!this.current())return;
      if(action===1||action===2)await this.onResume();
      else if(action===3)await this.onTitle();
      else await this.onRestart();
    }catch(problem){if(this.current())throw problem;}finally{this.busy=false;}
  }
  canStepPresentation(){
    if(!this.active||!this.suspended||!this.current()||this.busy||this.failure)return false;
    const state=this.snapshot();
    return state.active&&state.ready&&!state.closing&&!state.confirm&&!state.manual&&!state.replayPhase&&!state.action;
  }
  async stepPresentation(){
    if(!this.canStepPresentation())return false;
    this.busy=true;
    try{
      const before=this.snapshot();
      this.checked(this.module._th128_world_pause_tick(0,0));
      const after=this.snapshot();
      if(after.serial!==before.serial||!after.active||after.action||after.selection!==before.selection)
        throw new Error('Presentation-only original pause owner changed');
      return true;
    }finally{this.busy=false;}
  }
  async suspend(){
    if(!this.current())return;
    this.releaseKeys();this.suspended=true;
    await this.audio.suspend();
  }
  async dispose(){
    this.closed=true;this.active=false;this.releaseKeys();
    await this.audio.leave();
  }
}

const menuAudioOwners=new WeakMap();
export class MenuAudioGate{
  constructor(queue,{createContext=()=>new AudioContext()}={}){
    if(!queue||typeof queue.stopChannel!=='function'||typeof createContext!=='function')
      throw new Error('Original sound queue is required');
    this.queue=queue;this.createContext=createContext;this.menu=null;this.gain=null;this.game=null;this.gameGain=null;
    this.entry=null;this.leaving=null;this.requestEpoch=0;
  }
  checkOwner(entry){
    if(entry.retired||this.entry!==entry||menuAudioOwners.get(this.queue)!==entry||this.queue.disposed)
      throw new DOMException('Menu audio owner retired','AbortError');
  }
  async completeEntry(entry){
    try{await entry.pending;this.checkOwner(entry);}
    catch(error){
      const retired=entry.retired;
      await this.retire(entry).catch(()=>{});
      if(retired)throw new DOMException('Menu audio owner retired','AbortError');
      throw error;
    }finally{entry.entering=false;}
  }
  async resumeEntry(entry){
    await Promise.race([entry.menu.resume(),entry.cancelled.then(()=>{throw new DOMException('Menu audio owner retired','AbortError');})]);
    this.checkOwner(entry);
  }
  async enter(){
    if(this.entry&&!this.entry.retired){
      const entry=this.entry;
      if(entry.entering){await entry.pending;this.checkOwner(entry);return;}
      this.checkOwner(entry);entry.entering=true;
      entry.pending=this.resumeEntry(entry);
      return this.completeEntry(entry);
    }
    const epoch=this.requestEpoch,previous=menuAudioOwners.get(this.queue);
    if(previous&&!previous.retired)throw new Error('Game audio already has a menu owner');
    if(previous?.cleanup)await previous.cleanup;
    if(this.requestEpoch!==epoch||menuAudioOwners.get(this.queue)!==previous)throw new DOMException('Menu audio owner changed','AbortError');
    const game=this.queue.context,gameGain=this.queue.gain;
    if(!game||!gameGain||game.state==='closed'||this.queue.disposed)throw new Error('Game audio is unavailable');
    const menu=this.createContext(),gain=menu.createGain();
    gain.gain.value=gameGain.gain.value;gain.connect(menu.destination);
    const entry={game,gameGain,menu,gain,retired:false,entering:true,pending:null,cleanup:null};
    entry.cancelled=new Promise(resolve=>{entry.cancel=resolve;});
    this.entry=entry;menuAudioOwners.set(this.queue,entry);
    this.game=game;this.gameGain=gameGain;this.menu=menu;this.gain=gain;
    entry.pending=(async()=>{
      await game.suspend();this.checkOwner(entry);
      if(this.queue.context!==game||this.queue.gain!==gameGain)throw new Error('Game audio output changed during suspension');
      this.queue.context=menu;this.queue.gain=gain;
      await this.resumeEntry(entry);
    })();
    return this.completeEntry(entry);
  }
  async suspend(){if(this.menu&&this.menu.state==='running')await this.menu.suspend();}
  retire(entry){
    if(entry.cleanup)return entry.cleanup;
    entry.retired=true;entry.cancel();
    const owned=this.entry===entry;
    if(owned){this.entry=null;this.menu=null;this.gain=null;this.game=null;this.gameGain=null;}
    // Finish the old context operation before restoring game audio or admitting its successor.
    entry.cleanup=(async()=>{
      await entry.pending.catch(()=>{});
      const {menu,gain,game,gameGain}=entry;
      for(const channel of this.queue.channels.values())
        if(channel.source?.context===menu||channel.gain?.context===menu)this.queue.stopChannel(channel);
      for(const channel of this.queue.retiredChannels)
        if(channel.source?.context===menu||channel.gain?.context===menu){this.queue.stopChannel(channel);this.queue.retiredChannels.delete(channel);}
      if(menuAudioOwners.get(this.queue)===entry&&this.queue.context===menu){
        this.queue.context=game;this.queue.gain=gameGain;
      }
      gain.disconnect();
      try{await menu.close();}
      finally{
        if(menuAudioOwners.get(this.queue)===entry&&this.queue.context===game&&this.queue.gain===gameGain
          &&!this.queue.disposed&&game.state!=='closed'){
          try{await game.resume();}catch(error){if(game.state!=='closed')throw error;}
        }
      }
    })();
    if(owned)this.leaving=entry.cleanup;
    return entry.cleanup;
  }
  async leave(){
    ++this.requestEpoch;
    if(this.entry)return this.retire(this.entry);
    await this.leaving;
  }
}

function transactionComplete(transaction){
  return new Promise((resolve,reject)=>{
    transaction.oncomplete=()=>resolve();
    transaction.onabort=()=>reject(transaction.error||new Error('Menu receipt transaction aborted'));
    transaction.onerror=()=>{};
  });
}
export class OwnedMenuSoundLedger{
  constructor(indexedDB=globalThis.indexedDB,crypto=globalThis.crypto){
    if(!indexedDB||!crypto?.subtle||typeof crypto.randomUUID!=='function')throw new Error('Durable menu sound storage is unavailable');
    this.indexedDB=indexedDB;this.crypto=crypto;this.token=crypto.randomUUID();this.database=null;
  }
  async open(){
    if(this.database)return this.database;
    this.database=await new Promise((resolve,reject)=>{
      const request=this.indexedDB.open('th128-owned-menu-sound-v1',1);
      request.onupgradeneeded=()=>request.result.createObjectStore('receipts');
      request.onsuccess=()=>resolve(request.result);
      request.onerror=()=>reject(request.error||new Error('Cannot open menu sound storage'));
      request.onblocked=()=>reject(new Error('Menu sound storage is blocked'));
    });
    return this.database;
  }
  async commit(receipt){
    const database=await this.open(),payload=JSON.stringify(receipt),encoded=new TextEncoder().encode(payload);
    const digest=Array.from(new Uint8Array(await this.crypto.subtle.digest('SHA-256',encoded)),value=>value.toString(16).padStart(2,'0')).join('');
    const key=this.token+':'+receipt.epoch+':'+receipt.serial;
    const write=database.transaction('receipts','readwrite',{durability:'strict'}),writeDone=transactionComplete(write);
    write.objectStore('receipts').add({payload,digest},key);await writeDone;
    const read=database.transaction('receipts','readonly'),readDone=transactionComplete(read);
    const observed=await new Promise((resolve,reject)=>{
      const request=read.objectStore('receipts').get(key);
      request.onsuccess=()=>resolve(request.result);request.onerror=()=>reject(request.error);
    });
    await readDone;
    if(!observed||observed.payload!==payload||observed.digest!==digest)throw new Error('Menu sound full payload readback differs');
    return {key,digest,payload,strictReadback:true};
  }
  close(){this.database?.close();this.database=null;}
}
export async function consumeMenuSoundWindow(module,queue,ledger,{present=true}={}){
  const epoch=module._th128_world_sound_epoch(),serial=module._th128_world_sound_begin_window();
  if(serial===0)return null;
  const opened=JSON.parse(module.UTF8ToString(module._th128_world_sound_window_state()));
  if(!Number.isInteger(epoch)||epoch<=0||opened.epoch!==epoch||opened.serial!==serial
    ||!Number.isSafeInteger(opened.count)||opened.count<0||opened.count>65536)
    throw new Error('Menu sound native window identity differs');
  const requests=[];
  for(let index=0;index<opened.count;++index){
    const id=module._th128_world_sound_id(index),pan=module._th128_world_sound_x(index),operation=module._th128_world_sound_operation(index);
    if(!Number.isInteger(id)||id< -1||id>=76||!Number.isFinite(pan)||![0,1,2].includes(operation)
      ||(id===-1&&operation!==0))throw new Error('Menu native sound request is invalid');
    requests.push({id,pan,operation,accepted:queue.consumeNativeRequest(id,pan,operation,{present})});
  }
  queue.flush({present});
  const receipt={serial,epoch,baseLow:opened.baseLow,baseHigh:opened.baseHigh,count:opened.count,requests};
  const committed=await ledger.commit(receipt);
  if(committed?.strictReadback!==true||committed.payload!==JSON.stringify(receipt))
    throw new Error('Menu sound receipt lacks complete durable readback');
  const remaining=module._th128_world_sound_ack_window(serial,opened.count,opened.baseLow,opened.baseHigh,epoch);
  if(!Number.isInteger(remaining)||remaining<0)throw new Error('Native menu sound acknowledgement rejected');
  return {...receipt,digest:committed.digest,remaining};
}

import {decodeReplay,encodeReplay} from './replay.mjs';
import {nativeTouchReplayBlock,readTouchReplay,installTouchReplay,touchReplaySnapshot} from './touch-replay.mjs';
export function nativeReplayToFile(native){
  if(!(native instanceof Uint8Array)||native.length<292)throw new Error('Complete native replay bytes required');
  const view=new DataView(native.buffer,native.byteOffset,native.byteLength),count=view.getUint32(36+88,true);
  if(count<1||count>3)throw new Error('Invalid native replay stage count');
  let offset=36+112;const stages=[];
  for(let index=0;index<count;++index){
    if(offset+144>native.length)throw new Error('Native replay stage header is truncated');
    const header=native.slice(offset,offset+144),stageView=new DataView(header.buffer,header.byteOffset,header.byteLength);
    const frames=stageView.getUint32(4,true),size=stageView.getUint32(8,true);
    if(frames>1000000||size<frames*6||offset+144+size>native.length)throw new Error('Native replay stage input is truncated');
    stages.push({rawHeader:header,inputRaw:native.slice(offset+144,offset+144+frames*6),fpsRaw:native.slice(offset+144+frames*6,offset+144+size)});
    offset+=144+size;
  }
  const touchBlock=nativeTouchReplayBlock(native.subarray(offset),stages);
  const file=encodeReplay({header:{raw:native.slice(0,36)},metadata:{raw:native.slice(36,148)},stages,
    payloadTailRaw:new Uint8Array(),dataGapRaw:new Uint8Array(),fileTailRaw:new Uint8Array(),userBlocks:touchBlock?[touchBlock]:[]},{recompress:true});
  const readback=decodeReplay(file);
  if(readback.rawPayload.length!==offset-36||readback.rawPayload.some((value,index)=>value!==native[index+36]))
    throw new Error('Original replay codec changed native input bytes');
  return file;
}
function replayTransaction(transaction){
  return new Promise((resolve,reject)=>{
    transaction.oncomplete=()=>resolve();
    transaction.onabort=()=>reject(transaction.error||new Error('Replay transaction aborted'));
    transaction.onerror=()=>{};
  });
}
export class OwnedReplayStore{
  constructor(indexedDB=globalThis.indexedDB,crypto=globalThis.crypto){
    if(!indexedDB||!crypto?.subtle)throw new Error('Replay storage is unavailable');
    this.indexedDB=indexedDB;this.crypto=crypto;this.database=null;this.opening=null;this.openEpoch=0;
  }
  static nativeChecksum(bytes){let value=2166136261;for(const byte of bytes)value=Math.imul(value^byte,16777619)>>>0;return value;}
  async hash(bytes){return Array.from(new Uint8Array(await this.crypto.subtle.digest('SHA-256',bytes)),value=>value.toString(16).padStart(2,'0')).join('');}
  async open(){
    if(this.database)return this.database;
    if(this.opening)return this.opening;
    const epoch=this.openEpoch;
    const pending=new Promise((resolve,reject)=>{
      const request=this.indexedDB.open('th128-owned-replays-v1',1);let failed=false;
      const failure=message=>{failed=true;reject(request.error||new Error(message));};
      request.onupgradeneeded=()=>{
        if(epoch!==this.openEpoch){request.transaction?.abort();return;}
        request.result.createObjectStore('slots');
      };
      request.onsuccess=()=>{
        if(failed||epoch!==this.openEpoch){request.result.close();reject(new DOMException('Replay store open owner closed','AbortError'));return;}
        this.database=request.result;resolve(this.database);
      };
      request.onerror=()=>failure('Cannot open replay store');
      request.onblocked=()=>failure('Replay store is blocked');
    });
    this.opening=pending;
    try{return await pending;}finally{if(this.opening===pending)this.opening=null;}
  }
  async observe(){
    const database=await this.open(),transaction=database.transaction('slots','readonly'),done=replayTransaction(transaction);
    const rows=await Promise.all(Array.from({length:25},(_,slot)=>new Promise((resolve,reject)=>{
      const request=transaction.objectStore('slots').get(slot);
      request.onsuccess=()=>resolve(request.result??null);request.onerror=()=>reject(request.error);
    })));
    await done;
    for(const row of rows)if(row){
      if(!(row.bytes instanceof Uint8Array)||await this.hash(row.bytes)!==row.digest)throw new Error('Stored replay bytes differ');
      decodeReplay(row.bytes);
    }
    return rows;
  }
  async save(slot,native,{expectedDigest=null,isCurrent=()=>true}={}){
    if(typeof isCurrent!=='function')throw new Error('Replay save owner unavailable');
    const current=()=>{if(!isCurrent())throw new DOMException('Replay save owner cancelled','AbortError');};current();
    if(!Number.isInteger(slot)||slot<0||slot>=25)throw new Error('Original replay slot outside25entries');
    if(!(native instanceof Uint8Array))throw new Error('Native full replay bytes unavailable');
    native=native.slice();
    const file=nativeReplayToFile(native),digest=await this.hash(file),decoded=decodeReplay(file);
    const record={bytes:file,digest,name:Array.from(decoded.metadata.nameRaw),timestamp:String(decoded.metadata.timestamp),stageCount:decoded.stages.length,score:decoded.metadata.scoreRaw};
    const database=await this.open();current();const write=database.transaction('slots','readwrite',{durability:'strict'}),writeDone=replayTransaction(write);
    if(write.durability!=='strict'){write.abort();try{await writeDone;}catch{}throw new Error('Replay save strict transaction durability was not granted');}
    const prior=write.objectStore('slots').get(slot);
    prior.onsuccess=()=>{
      if(!isCurrent()||(prior.result?.digest??null)!==expectedDigest){write.abort();return;}
      write.objectStore('slots').put(record,slot);
    };
    prior.onerror=()=>write.abort();
    await writeDone;current();
    const read=database.transaction('slots','readonly'),readDone=replayTransaction(read);
    const observed=await new Promise((resolve,reject)=>{
      const request=read.objectStore('slots').get(slot);request.onsuccess=()=>resolve(request.result);request.onerror=()=>reject(request.error);
    });await readDone;
    if(!observed||Reflect.ownKeys(observed).length!==Reflect.ownKeys(record).length
      ||Reflect.ownKeys(record).some(key=>!Object.hasOwn(observed,key))
      ||JSON.stringify(observed.name)!==JSON.stringify(record.name)||observed.timestamp!==record.timestamp
      ||observed.stageCount!==record.stageCount||observed.score!==record.score
      ||!(observed.bytes instanceof Uint8Array)||observed.digest!==digest||observed.bytes.length!==file.length
      ||observed.bytes.some((value,index)=>value!==file[index])||await this.hash(observed.bytes)!==digest)
      throw new Error('Replay full durable readback differs');
    current();const returned=decodeReplay(observed.bytes);
    if(returned.rawPayload.length!==decoded.rawPayload.length||returned.rawPayload.some((value,index)=>value!==native[index+36]))
      throw new Error('Saved replay native payload differs');
    return{slot,bytes:observed.bytes,digest,rawBytes:native.length,rawChecksum:OwnedReplayStore.nativeChecksum(native),strictFullReadback:true,strictDurability:true};
  }
  async saveThprac(slot,input,{expectedDigest=null,isCurrent=()=>true,signal}={}){
    if(!Number.isInteger(slot)||slot<0||slot>=25||!(input instanceof Uint8Array)||typeof isCurrent!=='function')throw new Error('Practice Replay file/slot/owner rejected');
    const current=()=>{if(signal?.aborted||!isCurrent())throw new DOMException('Practice Replay owner cancelled','AbortError');};current();
    const bytes=input.slice(),{readThprac128Replay}=await import('./thprac128-replay.mjs');
    const practice=readThprac128Replay(bytes);if(!practice)throw new Error('Practice PRAC metadata missing');
    const decoded=decodeReplay(bytes),digest=await this.hash(bytes);current();
    const record={bytes,digest,name:Array.from(decoded.metadata.nameRaw),timestamp:String(decoded.metadata.timestamp),stageCount:decoded.stages.length,score:decoded.metadata.scoreRaw,source:'thprac-rpy'};
    const result=await this.replaceFileSlot(slot,expectedDigest,record,{isCurrent,signal});current();
    const readback=readThprac128Replay(result.record.bytes,practice.identity);if(JSON.stringify(readback.session.params)!==JSON.stringify(practice.session.params))throw new Error('Practice Replay metadata durable readback differs');
    return{slot,bytes:result.record.bytes.slice(),digest,alreadyPresent:result.alreadyPresent,strictFullReadback:true,strictDurability:true,scoreProfileImported:false,practice:true};
  }
  async importOriginal(input,{fileName='original.rpy',isCurrent=()=>true}={}){
    if(!(input instanceof Uint8Array)||input.length<36||input.length>16*1024*1024
        ||typeof fileName!=='string'||fileName.length>180||/[\x00-\x1f\\/]/.test(fileName)
        ||!fileName.toLowerCase().endsWith('.rpy')||typeof isCurrent!=='function')
      throw new Error('请选择有效的原版 .rpy 录像文件（最大16MiB）');
    const current=()=>{if(!isCurrent())throw new DOMException('录像导入页面已变更','AbortError');};
    current();const file=input.slice(),decoded=decodeReplay(file);
    // This checks supported file/route/stage/input extents, not native playback completion.
    new OwnedReplayPlayback(file,0);
    const digest=await this.hash(file);current();
    const rows=await this.observe();current();
    const duplicate=rows.findIndex(row=>row?.digest===digest);
    if(duplicate>=0)return{slot:duplicate,bytes:rows[duplicate].bytes.slice(),digest,
      strictFullReadback:true,alreadyPresent:true,externalSource:true,scoreProfileImported:false};
    const slot=rows.findIndex(row=>row===null);
    if(slot<0)throw new Error('25个录像槽均已占用；导入不会覆盖已有录像');
    const record={bytes:file,digest,name:Array.from(decoded.metadata.nameRaw),timestamp:String(decoded.metadata.timestamp),
      stageCount:decoded.stages.length,score:decoded.metadata.scoreRaw,source:'original-rpy-import',originalFileName:fileName};
    const database=await this.open();current();
    const write=database.transaction('slots','readwrite',{durability:'strict'}),written=replayTransaction(write);
    if(write.durability!=='strict'){write.abort();await written;throw new Error('录像导入需要严格持久化事务');}
    const prior=write.objectStore('slots').get(slot);
    prior.onsuccess=()=>{
      if(!isCurrent()||prior.result!==undefined&&prior.result!==null){write.abort();return;}
      write.objectStore('slots').put(record,slot);
    };
    prior.onerror=()=>write.abort();await written;
    const read=database.transaction('slots','readonly'),readDone=replayTransaction(read);
    const observed=await new Promise((resolve,reject)=>{
      const request=read.objectStore('slots').get(slot);
      request.onsuccess=()=>resolve(request.result);request.onerror=()=>reject(request.error);
    });await readDone;
    if(!observed||!(observed.bytes instanceof Uint8Array)||observed.bytes.length!==file.length
        ||observed.bytes.some((value,index)=>value!==file[index])||observed.digest!==digest
        ||await this.hash(observed.bytes)!==digest||observed.source!==record.source
        ||observed.originalFileName!==fileName||observed.stageCount!==record.stageCount
        ||observed.score!==record.score||observed.timestamp!==record.timestamp
        ||JSON.stringify(observed.name)!==JSON.stringify(record.name))
      throw new Error('导入录像完整文件与来源信息持久化回读不一致');
    current();return{slot,bytes:observed.bytes.slice(),digest,strictFullReadback:true,
      alreadyPresent:false,externalSource:true,scoreProfileImported:false};
  }
  async importAtSlot(slot,input,{expectedDigest=null,fileName='original.rpy',isCurrent=()=>true,signal}={}){
    if(!Number.isInteger(slot)||slot<0||slot>=25)throw new Error('Replay target slot outside25entries');
    if(!(input instanceof Uint8Array)||input.length<36||input.length>16*1024*1024
      ||typeof fileName!=='string'||fileName.length>180||/[\x00-\x1f\\/]/.test(fileName)||!fileName.toLowerCase().endsWith('.rpy'))
      throw new Error('Original Replay file identity or size rejected');
    const current=()=>{if(signal?.aborted||!isCurrent())throw new DOMException('Replay file owner cancelled','AbortError');};
    current();const file=input.slice(),decoded=decodeReplay(file);new OwnedReplayPlayback(file,slot);
    const digest=await this.hash(file);current();
    const record={bytes:file,digest,name:Array.from(decoded.metadata.nameRaw),timestamp:String(decoded.metadata.timestamp),
      stageCount:decoded.stages.length,score:decoded.metadata.scoreRaw,source:'original-rpy-import',originalFileName:fileName};
    const result=await this.replaceFileSlot(slot,expectedDigest,record,{isCurrent,signal});
    return{slot,bytes:result.record.bytes.slice(),digest,alreadyPresent:result.alreadyPresent,externalSource:true,
      scoreProfileImported:false,strictFullReadback:true,strictDurability:true};
  }
  async removeSlot(slot,{expectedDigest,isCurrent=()=>true,signal}={}){
    if(!Number.isInteger(slot)||slot<0||slot>=25)throw new Error('Replay target slot outside25entries');
    if(typeof expectedDigest!=='string'||!/^[0-9a-f]{64}$/.test(expectedDigest))throw new Error('Replay removal requires the observed digest');
    await this.replaceFileSlot(slot,expectedDigest,null,{isCurrent,signal});
    return{slot,removed:true,strictFullReadback:true,strictDurability:true};
  }
  async replaceFileSlot(slot,expectedDigest,next,{isCurrent,signal}){
    if(expectedDigest!==null&&(typeof expectedDigest!=='string'||!/^[0-9a-f]{64}$/.test(expectedDigest)))
      throw new Error('Replay expected digest rejected');
    const current=()=>{if(signal?.aborted||!isCurrent())throw new DOMException('Replay file owner cancelled','AbortError');};
    current();const before=(await this.observe())[slot];current();
    if((before?.digest??null)!==expectedDigest)throw new Error('Replay slot digest conflict');
    const sameBytes=(a,b)=>a instanceof Uint8Array&&b instanceof Uint8Array&&a.length===b.length&&a.every((v,i)=>v===b[i]);
    const alreadyPresent=next!==null&&before?.digest===next.digest;
    const database=await this.open();current();
    const write=database.transaction('slots','readwrite',{durability:'strict'}),written=replayTransaction(write);
    let issue=null;
    const cancel=()=>{try{write.abort();}catch{}};
    signal?.addEventListener('abort',cancel,{once:true});
    try{
      if(write.durability!=='strict'){issue=new Error('Replay file strict durability was not granted');write.abort();}
      else{
        const request=write.objectStore('slots').get(slot);
        request.onerror=()=>{issue=request.error;write.abort();};
        request.onsuccess=()=>{
          try{
            current();const actual=request.result??null;
            if((actual?.digest??null)!==expectedDigest||(before===null?actual!==null:!sameBytes(actual?.bytes,before.bytes)))
              throw new Error('Replay slot changed before compare-and-swap');
            if(next===null)write.objectStore('slots').delete(slot);
            else if(!alreadyPresent)write.objectStore('slots').put(next,slot);
          }catch(error){issue=error;write.abort();}
        };
      }
      try{await written;}catch(error){throw issue??error;}
    }finally{signal?.removeEventListener('abort',cancel);}
    current();const actual=(await this.observe())[slot];current();
    if(next===null){if(actual!==null)throw new Error('Replay removal full absent readback differs');return{record:null,alreadyPresent:false};}
    const expected=alreadyPresent?before:next;
    if(!actual||!sameBytes(actual.bytes,expected.bytes)||actual.digest!==expected.digest
      ||JSON.stringify({...actual,bytes:null})!==JSON.stringify({...expected,bytes:null}))
      throw new Error('Replay file full metadata/readback differs');
    return{record:actual,alreadyPresent};
  }
  async export(slot){
    const record=(await this.observe())[slot];if(!record)throw new Error('Replay slot is empty');
    return record.bytes.slice();
  }
  close(){this.openEpoch++;this.database?.close();this.database=null;this.opening=null;}
}

export function originalReplayPreviewPacket(rows){
  if(!Array.isArray(rows)||rows.length!==25)throw new Error('Original replay preview requires25actualslots');
  const output=new Uint8Array(3412),view=new DataView(output.buffer);
  view.setUint32(0,0x31565052,true);view.setUint32(4,25,true);view.setUint32(8,136,true);
  for(let index=0;index<25;++index){
    const row=rows[index];if(row===null)continue;
    if(!row||!(row.bytes instanceof Uint8Array))throw new Error('Occupied replay slot lacks actual file bytes');
    const replay=decodeReplay(row.bytes),timestamp=replay.metadata.timestamp;
    if(timestamp>BigInt(Number.MAX_SAFE_INTEGER))throw new Error('Replay timestamp exceeds actual local calendar');
    const date=new Date(Number(timestamp)*1000),fields=[date.getFullYear(),date.getMonth()+1,date.getDate(),date.getHours(),date.getMinutes()];
    if(!fields.every(Number.isSafeInteger)||fields[0]<1970||fields[0]>9999)throw new Error('Replay local calendar is unavailable');
    const base=12+index*136;view.setUint32(base,1,true);output.set(replay.metadata.raw,base+4);
    for(let field=0;field<5;++field)view.setUint32(base+116+field*4,fields[field],true);
  }
  return output;
}

export function originalReplayNameContextPacket(snapshot,date=new Date()){
  if(snapshot?.replayPhase!==2||!Number.isSafeInteger(snapshot.replayNameEpoch)||snapshot.replayNameEpoch<=0||snapshot.replayNameEpoch>0xffffffff)
    throw new Error('Original replay name context requires its actual active entry epoch');
  if(!(date instanceof Date)||!Number.isFinite(date.getTime())||date.getTime()<0)
    throw new Error('Original replay name entry host clock is unavailable');
  const seconds=BigInt(Math.floor(date.getTime()/1000));
  if(seconds>0x793406fffn)throw new Error('Original replay name entry clock exceeds native metadata');
  const fields=[date.getFullYear(),date.getMonth()+1,date.getDate(),date.getHours(),date.getMinutes()];
  if(!fields.every(Number.isSafeInteger)||fields[0]<1970||fields[0]>9999)throw new Error('Original replay name entry local calendar is unavailable');
  const output=new Uint8Array(36),view=new DataView(output.buffer);
  view.setUint32(0,0x31434e52,true);view.setUint32(4,snapshot.replayNameEpoch,true);
  view.setUint32(8,Number(seconds&0xffffffffn),true);view.setUint32(12,Number(seconds>>32n),true);
  for(let index=0;index<5;++index)view.setUint32(16+4*index,fields[index],true);
  return output;
}
export {OwnedLastNameFieldStore,originalNameProfilePacket} from './last-name-field.mjs';

export class TitleSoundWindowConsumer{
  constructor(module,queue,ledger,{present=true}={}){
    for(const name of ['epoch','count','begin_window','window_state','id','x','operation','ack_window'])
      if(typeof module?.['_th128_title_sound_'+name]!=='function')throw new Error('Original Title sound journal API is unavailable');
    if(typeof module.UTF8ToString!=='function'||!(queue instanceof GameSoundQueue)
      ||typeof ledger?.commit!=='function'||typeof present!=='boolean')throw new Error('Original Title sound consumer requires its real queue and durable ledger');
    this.module=module;this.queue=queue;this.ledger=ledger;this.present=present;this.pending=null;this.busy=false;
  }
  snapshot(){return JSON.parse(this.module.UTF8ToString(this.module._th128_title_sound_window_state()));}
  async consume(){
    if(this.busy)throw new Error('Title sound delivery is already owned by a pending operation');
    this.busy=true;
    try{
      const module=this.module,epoch=module._th128_title_sound_epoch();
      if(!Number.isInteger(epoch)||epoch<=0)throw new Error('Original Title sound epoch is unavailable');
      if(!this.pending){
        const serial=module._th128_title_sound_begin_window();
        if(serial===0)return null;
        const opened=this.snapshot();
        if(!Number.isInteger(serial)||serial<=0||opened.serial!==serial||opened.epoch!==epoch
          ||!Number.isSafeInteger(opened.count)||opened.count<1||opened.count>65536
          ||!Number.isInteger(opened.baseLow)||!Number.isInteger(opened.baseHigh))
          throw new Error('Original Title sound window identity differs');
        this.pending={...opened,cursor:0,requests:[],flushed:false,receipt:null};
      }
      const pending=this.pending,observed=this.snapshot();
      for(const name of ['serial','epoch','count','baseLow','baseHigh'])
        if(observed[name]!==pending[name])throw new Error('Original Title sound window ownership changed');
      if(epoch!==pending.epoch)throw new Error('Original Title sound epoch changed during delivery');
      while(pending.cursor<pending.count){
        const index=pending.cursor,id=module._th128_title_sound_id(index),pan=module._th128_title_sound_x(index),operation=module._th128_title_sound_operation(index);
        if(!Number.isInteger(id)||id<0||id>=76||!Number.isFinite(pan)||![0,1,2].includes(operation))
          throw new Error('Original Title sound request is invalid');
        const accepted=this.queue.consumeNativeRequest(id,pan,operation,{present:this.present});
        if(typeof accepted!=='boolean')throw new Error('Original Title sound queue acceptance is not explicit');
        pending.requests.push({id,pan,operation,accepted});++pending.cursor;
      }
      if(!pending.flushed){this.queue.flush({present:this.present});pending.flushed=true;}
      if(!pending.receipt)pending.receipt={serial:pending.serial,epoch:pending.epoch,baseLow:pending.baseLow,baseHigh:pending.baseHigh,count:pending.count,requests:pending.requests,present:this.present};
      const receipt=pending.receipt,committed=await this.ledger.commit(receipt);
      if(committed?.strictReadback!==true||committed.payload!==JSON.stringify(receipt)
        ||typeof committed.digest!=='string'||!/^[a-f0-9]{64}$/.test(committed.digest))
        throw new Error('Original Title sound receipt lacks full durable payload readback');
      const remaining=module._th128_title_sound_ack_window(receipt.serial,receipt.count,receipt.baseLow,receipt.baseHigh,receipt.epoch);
      if(!Number.isInteger(remaining)||remaining<0)throw new Error('Original Title sound acknowledgement was rejected');
      this.pending=null;
      return {...receipt,digest:committed.digest,remaining};
    }finally{this.busy=false;}
  }
}


export function originalTitleVolumeRaw(kind,percent,baseRaw=0){
  if(!['bgm','se'].includes(kind)||!Number.isInteger(percent)||percent<0||percent>100||!Number.isInteger(baseRaw)||baseRaw< -10000||baseRaw>0)
    throw new Error('Original Title volume domain rejected');
  if(percent===0)return -10000;
  const remaining=Math.fround(1-Math.fround(percent/100));
  const power=kind==='bgm'?Math.fround(remaining*remaining):Math.fround(remaining*remaining*remaining);
  return Math.trunc(Math.fround(1-power)*(baseRaw+5000))-5000;
}
class OriginalTitleSoundQueue extends GameSoundQueue{
  constructor(context,gain){super(context,gain);this.percent=80;}
  setPercent(percent){
    originalTitleVolumeRaw('se',percent);this.percent=percent;
    for(const channel of this.channels.values())if(channel.gain)channel.gain.gain.value=Math.pow(10,originalTitleVolumeRaw('se',percent,channel.item.volumeRaw)/2000);
  }
  flush(options={}){
    const saved=Array.from(this.buffers.values(),item=>[item,item.volumeRaw]);
    for(const [item,raw]of saved)item.volumeRaw=originalTitleVolumeRaw('se',this.percent,raw);
    try{return super.flush(options);}finally{for(const [item,raw]of saved)item.volumeRaw=raw;}
  }
}
function validateOriginalMainSettings(value){
  if(!value||value.schema!=='th128-owned-main-settings/1'||!Number.isSafeInteger(value.revision)||value.revision<0
    ||!Number.isInteger(value.bgm)||value.bgm<0||value.bgm>100||!Number.isInteger(value.se)||value.se<0||value.se>100
    ||![0,1].includes(value.rapid)||!Array.isArray(value.mapping)||value.mapping.length!==5
    ||value.mapping.some(button=>!Number.isInteger(button)||button<0||button>30)||new Set(value.mapping).size!==5
    ||!Number.isInteger(value.seenMask)||value.seenMask<0||value.seenMask>1023||(value.debugClearMask!==undefined&&(!Number.isInteger(value.debugClearMask)||value.debugClearMask<0||value.debugClearMask>63))||!Array.isArray(value.results)||value.results.length>25)
    throw new Error('Owned main settings identity rejected');
  for(const row of value.results)if(!row||!Number.isInteger(row.stage)||row.stage<1||row.stage>23
    ||!Number.isInteger(row.rank)||row.rank<0||row.rank>4||!Number.isInteger(row.score)||row.score<0||row.score>999999999
    ||typeof row.complete!=='boolean')throw new Error('Owned result record rejected');
  return value;
}
export class OriginalMainSettingsStore{
  constructor(indexedDB=globalThis.indexedDB){if(!indexedDB)throw new Error('Owned main settings database unavailable');this.indexedDB=indexedDB;this.database=null;}
  static defaults(){return{schema:'th128-owned-main-settings/1',revision:0,bgm:100,se:80,rapid:0,mapping:[0,1,3,2,4],seenMask:0,results:[],debugClearMask:0};}
  async open(){
    if(this.database)return this.database;
    this.database=await new Promise((resolve,reject)=>{
      const request=this.indexedDB.open('th128-owned-main-settings-v1',1);
      request.onupgradeneeded=()=>request.result.createObjectStore('settings');
      request.onerror=()=>reject(request.error||new Error('Owned main settings open failed'));
      request.onblocked=()=>reject(new Error('Owned main settings open blocked'));
      request.onsuccess=()=>resolve(request.result);
    });return this.database;
  }
  async observe(){
    const database=await this.open(),transaction=database.transaction('settings','readonly'),done=transactionComplete(transaction);
    const value=await new Promise((resolve,reject)=>{const request=transaction.objectStore('settings').get('main');request.onsuccess=()=>resolve(request.result);request.onerror=()=>reject(request.error);});
    await done;return structuredClone(validateOriginalMainSettings(value??OriginalMainSettingsStore.defaults()));
  }
  async commit(revision,changes){
    if(!Number.isSafeInteger(revision)||revision<0||!changes||Object.keys(changes).some(key=>!['bgm','se','rapid','mapping','seenMask','results','debugClearMask'].includes(key)))throw new Error('Owned main settings compare-and-swap rejected');
    const database=await this.open(),transaction=database.transaction('settings','readwrite',{durability:'strict'}),done=transactionComplete(transaction);
    let expected=null;
    const request=transaction.objectStore('settings').get('main');
    request.onerror=()=>transaction.abort();
    request.onsuccess=()=>{
      try{
        const prior=validateOriginalMainSettings(request.result??OriginalMainSettingsStore.defaults());
        if(prior.revision!==revision)throw new Error('Owned main settings compare-and-swap conflict');
        expected=validateOriginalMainSettings({...prior,...structuredClone(changes),revision:revision+1});
        transaction.objectStore('settings').put(expected,'main');
      }catch{transaction.abort();}
    };
    await done;
    const observed=await this.observe();
    if(!expected||JSON.stringify(observed)!==JSON.stringify(expected))throw new Error('Owned main settings full durable readback differs');
    return observed;
  }
  close(){this.database?.close();this.database=null;}
}
export class OriginalMainMenuController{
  constructor({module,prepare,consumeSound,consumeMusic,syncText,onState=()=>{},onAction=()=>{},getButtons=()=>({buttons:0,held:0})}){
    for(const name of ['_th128_tick','_th128_title_menu_state','_th128_title_main_input'])if(typeof module?.[name]!=='function')throw new Error('Original complete Title API unavailable');
    if([prepare,consumeSound,consumeMusic,syncText,onState,onAction,getButtons].some(callback=>typeof callback!=='function'))throw new Error('Original complete Title consumers unavailable');
    Object.assign(this,{module,prepare,consumeSound,consumeMusic,syncText,onState,onAction,getButtons});
    this.heldKeys=new Set();this.held=0;this.pressed=0;this.busy=false;this.prepared=false;this.disposed=false;this.controllerHeld=0;this.controllerAdopted=false;this.lastInput=0;
    this.musicTask=null;this.musicFailure=null;
  }
  snapshot(){return JSON.parse(this.module.UTF8ToString(this.module._th128_title_menu_state()));}
  adoptHeld(keys){
    if(this.disposed||!(keys instanceof Set))throw new Error('Original Main held-key observation unavailable');
    this.release();for(const code of keys){const bit=keyBits.get(code);if(bit!==undefined){this.heldKeys.add(code);this.held|=bit;}}
  }
  keyDown(code){pressMenuKey(this,code);}
  keyUp(code){releaseMenuKey(this,code);}
  release(){this.heldKeys.clear();this.held=0;this.pressed=0;this.controllerHeld=0;this.controllerAdopted=false;}
  dispose(){this.disposed=true;this.release();}
  pumpMusicRoom(){
    if(this.musicTask)return true;
    const module=this.module;
    if(typeof module._th128_title_music_bgm_request!=='function'
      ||!JSON.parse(module.UTF8ToString(module._th128_title_music_bgm_request())).serial)return false;
    // Track decoding and durable playback receipts must not own the animation clock.
    this.musicTask=Promise.resolve().then(()=>{if(!this.disposed)return this.consumeMusic();});
    this.musicTask.then(()=>{this.musicTask=null;},problem=>{
      this.musicTask=null;if(!this.disposed)this.musicFailure=problem;
    });
    return true;
  }
  async step(){
    if(this.disposed)throw new Error('Original Title controller disposed');
    if(this.busy)throw new Error('Original Title step is not serial');
    this.busy=true;
    try{
      if(this.musicFailure)throw this.musicFailure;
      const before=this.snapshot();
      // Keep drawing on return, but don't hand the native owner to another
      // screen until the ordered preview and Title BGM restore are drained.
      if(before.page!==5&&this.pumpMusicRoom())return before;
      const pressed=this.pressed;this.pressed=0;
      const controller=this.getButtons();
      if(!controller||!Number.isInteger(controller.buttons)||controller.buttons<0||controller.buttons>0x7fffffff
        ||!Number.isInteger(controller.held)||(controller.held&~0x803ff))throw new Error('Actual controller snapshot rejected');
      const controllerPressed=this.controllerAdopted?controller.held&~this.controllerHeld:0;
      this.lastInput=this.held|pressed|controller.held|controllerPressed|controller.buttons;
      this.controllerHeld=controller.held;this.controllerAdopted=true;
      if(before.ready===1&&before.transitioning===0&&!this.prepared){await this.prepare(before);this.prepared=true;}
      if(this.disposed)return null;
      if(before.ready===1&&before.transitioning===0&&this.prepared
        &&this.module._th128_title_main_input(this.held|controller.held,pressed|controllerPressed,controller.buttons)!==1)
        throw new Error(this.snapshot().error||'Original Title input rejected');
      if(this.module._th128_tick()!==1)throw new Error(this.snapshot().error||this.module.UTF8ToString(this.module._th128_error()));
      await this.consumeSound();if(this.disposed)return null;
      if(before.page===5)this.pumpMusicRoom();else await this.consumeMusic();
      if(this.disposed)return null;
      await this.syncText();if(this.disposed)return null;
      const after=this.snapshot();await this.onState(after);
      if(!this.disposed&&after.mainAction)await this.onAction(after);
      return after;
    }catch(problem){if(this.disposed&&problem?.name==='AbortError')return null;throw problem;}
    finally{this.busy=false;}
  }
}
export class OriginalTitleAudio{
  constructor({context,gain,base,tracks,resolveUrl,signal,onStopTitle,onStartTitle,onPreview,bgmLedger}){
    if(!context||!gain||!Array.isArray(tracks)||[resolveUrl,onStopTitle,onStartTitle,onPreview].some(callback=>typeof callback!=='function')
      ||typeof bgmLedger?.commit!=='function')throw new Error('Original Title audio consumers unavailable');
    Object.assign(this,{context,gain,base,tracks,resolveUrl,signal,onStopTitle,onStartTitle,onPreview,bgmLedger});
    this.sounds=new OriginalTitleSoundQueue(context,gain);this.bgmGain=context.createGain();this.bgmGain.connect(musicOutput(context,gain));
    this.music=new StageMusic(context,this.bgmGain);this.disposed=false;this.pendingBgm=null;
  }
  checked(){if(this.disposed||this.signal?.aborted||this.context.state==='closed')throw new DOMException('Original Title audio cancelled','AbortError');}
  async load(bindings){
    const sounds=Object.fromEntries([7,9,10].map(identifier=>{const binding=bindings.sounds?.[identifier];if(!binding)throw new Error('Original Title sound binding unavailable');return[identifier,binding];}));
    await this.sounds.load({sounds},this.base,this.resolveUrl,{signal:this.signal});this.checked();
  }
  adoptOrdinaryTitleMusic(music){
    this.checked();if(!(music instanceof StageMusic)||music.disposed||music.context!==this.context||music.track?.name!=='th128_07.wav'||!music.source||!music.buffer||music.fade
        ||this.music.source||this.pendingBgm)throw new Error('Original Title requires the sealed running ordinary Music7 owner');
    this.music.dispose();music.output.disconnect();music.output.connect(this.bgmGain);music.gain=this.bgmGain;this.music=music;this.ordinaryTitleMusic=true;
    return{source:music.source,track:music.track,buffer:music.buffer};
  }
  setVolumes(bgm,se){this.checked();this.bgmGain.gain.value=Math.pow(10,originalTitleVolumeRaw('bgm',bgm)/2000);this.sounds.setPercent(se);}
  async consumeMusic(module){
    this.checked();
    const request=JSON.parse(module.UTF8ToString(module._th128_title_music_bgm_request()));
    if(!request.serial)return null;
    if(!Number.isInteger(request.epoch)||request.epoch<=0||!Number.isInteger(request.serial)||request.serial<=0
      ||![0,1,2].includes(request.operation)||!Number.isInteger(request.row)||request.row<0||request.row>9)throw new Error('Original Title BGM identity rejected');
    if(this.context.state!=='running')return null;
    if(!this.pendingBgm)this.pendingBgm={request,performed:false,receipt:null};
    const pending=this.pendingBgm;
    if(JSON.stringify(pending.request)!==JSON.stringify(request))throw new Error('Original Title BGM pending ownership changed');
    if(!pending.performed){
      const names=['07','00','02','01','05','06','10','11','09','08'];
      let track=null,buffer=null;
      if(request.operation===1){
        const name='th128_'+names[request.row]+'.wav';
        if(!this.music.preloaded.has(name))await this.music.load(this.tracks,this.base,this.resolveUrl,{signal:this.signal,tracks:[name]});
        this.checked();if(this.context.state!=='running')return null;
        this.music.stopSource();this.onStopTitle();this.music.track=this.music.preloaded.get(name).track;this.music.buffer=this.music.preloaded.get(name).buffer;
        this.music.start();track=this.music.track;buffer=this.music.buffer;
      }else{
        this.music.stopSource();this.onStopTitle();
        if(request.operation===2){const started=this.onStartTitle();if(!started?.source||!started.track||!started.buffer)throw new Error('Real Title BGM restart unavailable');track=started.track;buffer=started.buffer;}
      }
      pending.receipt={...request,kind:'original-title-bgm',performed:true,track:track?.name??null,loopStart:track?.loopStart??null,loopEnd:track?.loopEnd??null,
        sampleRate:buffer?.sampleRate??null,channels:buffer?.numberOfChannels??null,duration:buffer?.duration??null,startedAt:this.context.currentTime};
      pending.performed=true;
    }
    const receipt=pending.receipt,committed=pending.committed??await this.bgmLedger.commit(receipt);
    this.checked();
    if(committed?.strictReadback!==true||committed.payload!==JSON.stringify(receipt)||!/^[a-f0-9]{64}$/.test(committed.digest))throw new Error('Real Title BGM full durable receipt missing');
    pending.committed=committed;
    if(request.operation===1)await this.onPreview(request.row);
    this.checked();
    const remaining=module._th128_title_music_bgm_ack(request.epoch,request.serial,request.operation,request.row);
    if(!Number.isInteger(remaining)||remaining<0)throw new Error('Exact original Title BGM ACK rejected');
    this.pendingBgm=null;return {...receipt,digest:committed.digest,remaining};
  }
  dispose(){if(this.disposed)return;this.disposed=true;this.sounds.dispose();this.music.dispose();this.bgmGain.disconnect();}
}

const demoPlaybackToken=Symbol('Original DAT Demo');
export class OwnedReplayPlayback{
  #stages;
  #touch;
  #rank;
  #file;
  #route;
  #metadata;
  #index=0;
  #pendingIndex=0;
  #module=null;
  #closed=false;
  constructor(bytes,slot,demoToken){
    const demo=demoToken===demoPlaybackToken;
    if(!(bytes instanceof Uint8Array)||(demo?slot!==null:!Number.isInteger(slot)||slot<0||slot>24))throw new Error('Owned replay file and slot identity rejected');
    this.#file=bytes.slice();
    const decoded=decodeReplay(this.#file);this.#touch=readTouchReplay(decoded);
    if(demo&&(decoded.stages.length!==1||this.#touch))throw new Error('Original Demo must contain only its unassisted starting stage');
    this.#route=decoded.metadata.route;this.#metadata={scoreRaw:decoded.metadata.scoreRaw,slowdown:decoded.metadata.slowdown};
    if(!Number.isInteger(decoded.metadata.rank)||decoded.metadata.rank<0||decoded.metadata.rank>3)
      throw new Error('当前回放仅接受已保存的普通关卡录像；没有伪造其它回放分支。');
    if(!decoded.stages.length||![1,6,11].includes(decoded.stages[0].stage))throw new Error('录像不是从原普通路线起点录制');
    this.#stages=decoded.stages.map(stage=>{
      if(!Number.isInteger(stage.numFrames)||stage.numFrames<1||stage.numFrames>1000000||stage.inputRaw.length!==stage.numFrames*6)
        throw new Error('Owned replay input extent rejected');
      return{...stage,rawHeader:stage.rawHeader.slice(),inputRaw:stage.inputRaw.slice()};
    });
    const nextIds={1:[2,4],2:[3],4:[5],6:[7,9],7:[8],9:[10],11:[12,14],12:[13],14:[15]};
    for(let index=1;index<this.#stages.length;++index)if(!nextIds[this.#stages[index-1].stage]?.includes(this.#stages[index].stage))
      throw new Error('Owned replay natural stage order rejected');
    this.#rank=decoded.metadata.rank;this.slot=slot;this.totalFrames=this.#stages.reduce((sum,stage)=>sum+stage.numFrames,0);
  }
  static fromOriginalDemo(bytes,{stage,frames}){
    const owner=new OwnedReplayPlayback(bytes,null,demoPlaybackToken);
    if(owner.firstSelection().stage!==stage||owner.totalFrames!==frames)throw new Error('Original DAT Demo stage or extent differs');
    return owner;
  }
  static async fromStore(store,slot){
    if(!(store instanceof OwnedReplayStore))throw new Error('Replay playback requires the actual owned web replay store');
    const record=(await store.observe())[slot];if(!record?.bytes)throw new Error('所选录像槽为空');
    const digest=await store.hash(record.bytes);
    if(digest!==record.digest)throw new Error('Owned replay full stored file digest differs');
    return new OwnedReplayPlayback(record.bytes,slot);
  }
  stageChoices(){
    const paths=[[1,2,3],[1,4,5],[6,7,8],[6,9,10],[11,12,13],[11,14,15]],path=paths[this.#route];
    if(!path)throw new Error('Original three-stage replay route unavailable');
    return path.map(stage=>{
      const recordedIndex=this.#stages.findIndex(row=>row.stage===stage),record=this.#stages[recordedIndex];
      return{stage,available:Boolean(record),recordedIndex,frames:record?.numFrames??0,
        scoreRaw:record?(this.#stages[recordedIndex+1]?.scoreRaw??this.#metadata.scoreRaw):0};
    });
  }
  stageChoicePacket(){
    if(this.slot===null)throw new Error('Original DAT Demo has no player replay slot');
    const packet=new Uint8Array(72),view=new DataView(packet.buffer);
    view.setUint32(0,0x31545352,true);view.setUint32(4,this.slot,true);view.setUint32(8,this.#route,true);view.setUint32(12,this.#rank,true);
    this.stageChoices().forEach((row,index)=>[Number(row.available),row.stage,row.scoreRaw,row.frames].forEach((value,field)=>view.setUint32(24+index*16+field*4,value,true)));
    return packet;
  }
  chooseStage(choice){
    const selected=this.stageChoices()[choice];
    if(!selected?.available||this.#closed)throw new Error('该关卡没有录像数据');
    this.#pendingIndex=selected.recordedIndex;this.#index=selected.recordedIndex;this.selectedChoice=choice;
    return this.firstSelection();
  }
  stageInitial(){
    const stage=this.#stages[this.#pendingIndex];
    if(!stage||this.#closed)throw new Error('Recorded replay stage unavailable');
    return{stage:stage.stage,seed:stage.seed,horizontal:stage.positionRaw[0],vertical:stage.positionRaw[1],
      frozenArea:stage.freezeArea,motivation:stage.motivationRaw,perfect:stage.perfectFreezeRaw,rawHeader:stage.rawHeader.slice()};
  }
  firstSelection(){const stage=this.#stages[this.#pendingIndex];return{stage:stage.stage,difficulty:this.#rank,seed:stage.seed,progress:0,autoFlow:true};}
  restartSelected(){
    if(this.slot===null)throw new Error('Original DAT Demo cannot restart through the player replay menu');
    const next=new OwnedReplayPlayback(this.#file,this.slot);next.chooseStage(this.selectedChoice??0);return next;
  }
  stagePacket(index=this.#pendingIndex){
    if(this.#closed||index<0||index>=this.#stages.length)throw new Error('Owned replay stage packet unavailable');
    const stage=this.#stages[index],plan=this.#touch?.[index],auxiliary=plan?.auxiliary??[],base=144+stage.inputRaw.length;
    const packet=new Uint8Array(base+(auxiliary.length?16+auxiliary.length*16:0));
    packet.set(stage.rawHeader);packet.set(stage.inputRaw,144);
    if(auxiliary.length){const v=new DataView(packet.buffer);[0x58554150,plan.worldFrames,auxiliary.length,0].forEach((n,i)=>v.setUint32(base+i*4,n,true));
      auxiliary.forEach((r,i)=>r.forEach((n,k)=>v.setUint32(base+16+i*16+k*4,n,true)));}
    return packet;
  }
  bindNativeStage(module){
    if(this.#closed)throw new Error('Owned replay playback is closed');
    for(const name of ['_th128_world_replay_playback_load','_th128_world_replay_playback_state','_th128_world_replay_playback_cancel'])
      if(typeof module?.[name]!=='function')throw new Error('当前核心缺少实际录像回放接口；不会下载冒充观看');
    const packet=this.stagePacket(),pointer=module._malloc(packet.length);
    try{
      module.HEAPU8.set(packet,pointer);
      if(typeof module._th128_world_replay_playback_restore!=='function'||module._th128_world_replay_playback_restore(pointer,144,this.#rank)!==1)
        throw new Error(module.UTF8ToString(module._th128_world_error())||'Recorded selected-stage resources restoration rejected');
      if(module._th128_world_replay_playback_load(pointer,packet.length,this.#rank)!==1)
        throw new Error(module.UTF8ToString(module._th128_world_error())||'Actual recorded replay header rejected by native World');
    }finally{module._free(pointer);}
    installTouchReplay(module,this.#touch?.[this.#pendingIndex]);
    this.#module=module;this.#index=this.#pendingIndex;return this.snapshot();
  }
  snapshot(){
    if(this.#closed||!this.#module)throw new Error('Owned replay native input owner unavailable');
    const value=JSON.parse(this.#module.UTF8ToString(this.#module._th128_world_replay_playback_state())),stage=this.#stages[this.#index];
    if(value.active!==true||value.stage!==stage.stage||value.rank!==this.#rank||value.frames!==stage.numFrames
      ||!Number.isInteger(value.consumed)||value.consumed<0||value.consumed>value.frames||value.complete!==(value.consumed===value.frames)
      ||typeof value.auxiliaryKnown!=='boolean'||!Number.isInteger(value.generation)||value.generation<1)
      throw new Error('Owned replay native stage/read cursor identity differs');
    return touchReplaySnapshot({...value,slot:this.slot,stageIndex:this.#index,totalStages:this.#stages.length,
      totalFrames:this.totalFrames,consumedTotal:this.#stages.slice(0,this.#index).reduce((sum,row)=>sum+row.numFrames,0)+value.consumed},this.#touch?.[this.#index]);
  }
  hasNext(){return this.#index+1<this.#stages.length;}
  prepareNaturalHandoff(transition){
    const state=this.snapshot(),next=this.#stages[this.#index+1];
    if(!state.complete||!next||transition?.requested!==true||transition.currentId!==state.stage||transition.nextId!==next.stage)
      throw new Error('实际关卡切换与录像下一段不一致，回放已停止');
    this.#pendingIndex=this.#index+1;return{stage:next.stage,seed:next.seed};
  }
  cancel(){
    if(this.#closed)return;
    if(this.#module&&this.#module._th128_world_replay_playback_cancel()!==1)
      throw new Error(this.#module.UTF8ToString(this.#module._th128_world_error())||'Owned replay cancellation rejected');
    this.#closed=true;this.#module=null;
  }
}

export const ownedWebSaveDatabaseNames=Object.freeze(['th128-owned-main-settings-v1','th128-owned-replays-v1','th128-original-last-name-field-v1']);
export async function clearOwnedWebSaveData(indexedDB=globalThis.indexedDB){
  if(!indexedDB?.deleteDatabase)throw new Error('本网页存档删除接口不可用');
  for(const name of ownedWebSaveDatabaseNames){
    await new Promise((resolve,reject)=>{
      const request=indexedDB.deleteDatabase(name);
      request.onsuccess=()=>resolve();
      request.onerror=()=>reject(request.error??new Error('清理网页存档失败：'+name));
      request.onblocked=()=>reject(new Error('其它游戏页面仍占用存档，请关闭它后重试：'+name));
    });
  }
  return{clearedDatabases:ownedWebSaveDatabaseNames.slice(),windowsOriginalSaveTouched:false};
}

const PROFILE_BYTES=52232,ROUTE_BYTES=1644,STATS_OFFSET=13152,CLEAR_OFFSET=14220;
const DATABASE='th128-owned-native-title-records-v1',STORE='records',KEY='native-profile';
function nativeProfileView(profile){
  if(!(profile instanceof Uint8Array)||profile.byteLength!==PROFILE_BYTES)throw new Error('Original native title profile size differs');
  const view=new DataView(profile.buffer,profile.byteOffset,profile.byteLength);
  const block=(offset,tag,bytes)=>{if(view.getUint32(offset,true)!==tag||view.getUint32(offset+8,true)!==bytes)throw new Error('Original native title profile block differs');};
  for(let route=0;route<8;route++)block(route*ROUTE_BYTES,0x00035243,ROUTE_BYTES);
  block(STATS_OFFSET,0x00025453,1068);block(CLEAR_OFFSET,0x00014443,38012);
  return view;
}
export const SPELL_MEDAL_CARDS=250;
export function nativeSpellMedalFlags(profile){
  const flags=new Uint8Array(SPELL_MEDAL_CARDS);
  if(profile==null)return flags;
  const view=nativeProfileView(profile);
  for(let card=0;card<flags.length;card++){
    const row=CLEAR_OFFSET+12+card*152;
    flags[card]=(view.getInt32(row+132,true)>0?1:0)|(view.getInt32(row+128,true)>0?2:0)|(view.getInt32(row+136,true)>0?4:0);
  }
  return flags;
}

// Practice completion medals are local presentation history, not normal CD
// counts, replay input, score eligibility, or extra-stage qualification.
const PRACTICE_MEDAL_DB='th128-owned-practice-spell-medals-v1',PRACTICE_MEDAL_STORE='medals';
function checkedMedalFlags(flags){
  if(!(flags instanceof Uint8Array)||flags.length!==SPELL_MEDAL_CARDS||flags.some(bits=>bits>7))throw new Error('Practice medal table differs');
  return flags;
}
export class OwnedPracticeSpellMedalStore{
  constructor(indexedDB=globalThis.indexedDB){this.indexedDB=indexedDB;}
  open(create){return new Promise((resolve,reject)=>{
    const request=create?this.indexedDB.open(PRACTICE_MEDAL_DB,1):this.indexedDB.open(PRACTICE_MEDAL_DB);
    request.onupgradeneeded=()=>{
      if(!create){request.transaction.abort();return;}
      request.result.createObjectStore(PRACTICE_MEDAL_STORE);
    };
    request.onsuccess=()=>resolve(request.result);
    request.onerror=()=>reject(request.error??new Error('Practice medal store unavailable'));
    request.onblocked=()=>reject(new Error('Practice medal store open blocked'));
  });}
  async observe(){
    if(!(await this.indexedDB.databases()).some(row=>row.name===PRACTICE_MEDAL_DB))return new Uint8Array(SPELL_MEDAL_CARDS);
    const db=await this.open(false);
    try{
      const tx=db.transaction(PRACTICE_MEDAL_STORE,'readonly'),done=replayTransaction(tx);
      const value=new Promise((resolve,reject)=>{
        const request=tx.objectStore(PRACTICE_MEDAL_STORE).get('earned');
        request.onsuccess=()=>resolve(request.result);request.onerror=()=>reject(request.error);
      });
      const [stored]=await Promise.all([value,done]);
      return stored===undefined?new Uint8Array(SPELL_MEDAL_CARDS):checkedMedalFlags(stored).slice();
    }finally{db.close();}
  }
  async merge(earned){
    const additions=checkedMedalFlags(earned).slice();
    if(!additions.some(Boolean))return this.observe();
    const db=await this.open(true);let merged,problem;
    try{
      const tx=db.transaction(PRACTICE_MEDAL_STORE,'readwrite',{durability:'strict'}),done=replayTransaction(tx);
      if(tx.durability!=='strict'){
        tx.abort();try{await done;}catch{}
        throw new Error('Practice medal strict persistence unavailable');
      }
      const store=tx.objectStore(PRACTICE_MEDAL_STORE),request=store.get('earned');
      request.onsuccess=()=>{
        try{
          merged=request.result===undefined?new Uint8Array(SPELL_MEDAL_CARDS):checkedMedalFlags(request.result).slice();
          let changed=false;for(let card=0;card<merged.length;card++)if((merged[card]|additions[card])!==merged[card]){
            merged[card]|=additions[card];changed=true;
          }
          if(changed)store.put(merged,'earned');
        }catch(error){problem=error;tx.abort();}
      };
      try{await done;}catch(error){throw problem??error;}
    }finally{db.close();}
    const observed=await this.observe();
    if(!merged||merged.some((bits,card)=>(observed[card]&bits)!==bits))throw new Error('Practice medal durable readback differs');
    return observed;
  }
}
export function nativeRouteScorePacket(profile){
  const source=profile===null?null:nativeProfileView(profile),packet=new Uint8Array(428),view=new DataView(packet.buffer);
  view.setUint32(0,0x31485254,true);view.setUint32(4,35,true);
  for(let route=0;route<7;route++)for(let rank=0;rank<5;rank++){
    const base=route*ROUTE_BYTES,record=base+16+rank*320,target=8+(route*5+rank)*12;
    const score=source?source.getUint32(record,true):100000,continues=source?source.getUint8(record+5):0,clears=source?source.getUint32(base+0x658+rank*4,true):0;
    if(score>999999999||continues>9)throw new Error('Original native route score or continue byte differs');
    view.setUint32(target,score,true);view.setUint32(target+4,continues,true);view.setUint32(target+8,clears,true);
  }
  return packet;
}
export async function validatedOwnedNativeTitleRecord(record){return checkedStoredScoreProfile(record,false);}
async function checkedStoredScoreProfile(record,practice){
  if(record===null||record===undefined)return null;
  if(record.schema!=='th128-owned-native-title-record/1'||!Number.isSafeInteger(record.revision)||record.revision<1
    ||!(practice?record.provenance==='practice-native-postrun'&&record.tainted===true:
      (record.provenance==='ordinary-native-postrun'&&record.tainted===false)||(record.provenance==='extra-native-postrun'&&record.tainted===true))
    ||typeof record.sha256!=='string'||!(/^[a-f0-9]{64}$/).test(record.sha256))
    throw new Error('Owned native title record provenance differs');
  nativeRouteScorePacket(record.profile);
  const profile=record.profile.slice(),digest=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',profile)),b=>b.toString(16).padStart(2,'0')).join('');
  if(digest!==record.sha256)throw new Error('Owned native title record whole bytes differ');
  return {...record,profile};
}
function sameNativeRecordMetadata(first,second){
  if(!first||!second||typeof first!=='object'||typeof second!=='object'||Array.isArray(first)||Array.isArray(second))return false;
  const keys=Object.keys(second);
  return Object.keys(first).length===keys.length&&keys.every(key=>Object.hasOwn(first,key)&&JSON.stringify(first[key])===JSON.stringify(second[key]));
}
export class OwnedNativeTitleRecordStore{
  #practice;
  #key;
  constructor(indexedDB=globalThis.indexedDB,{practice=false}={}){
    if(!indexedDB||typeof indexedDB.databases!=='function')throw new Error('Owned native title record observation unavailable');
    this.indexedDB=indexedDB;this.#practice=practice===true;this.#key=this.#practice?'practice-profile':KEY;
  }
  async observe(){
    const databases=await this.indexedDB.databases();
    if(!databases.some(row=>row.name===DATABASE))return null;
    const database=await new Promise((resolve,reject)=>{
      const request=this.indexedDB.open(DATABASE);
      request.onupgradeneeded=()=>{request.transaction.abort();reject(new Error('Owned native title record disappeared before readonly open'));};
      request.onblocked=()=>reject(new Error('Owned native title record open blocked'));
      request.onerror=()=>reject(request.error||new Error('Owned native title record open failed'));
      request.onsuccess=()=>resolve(request.result);
    });
    try{
      if(!database.objectStoreNames.contains(STORE))throw new Error('Owned native title record store absent');
      const transaction=database.transaction(STORE,'readonly');
      const completed=new Promise((resolve,reject)=>{transaction.oncomplete=()=>resolve();transaction.onabort=()=>reject(transaction.error||new Error('Owned native title record observation aborted'));transaction.onerror=()=>reject(transaction.error||new Error('Owned native title record observation failed'));});
      const value=new Promise((resolve,reject)=>{const request=transaction.objectStore(STORE).get(this.#key);request.onsuccess=()=>resolve(request.result);request.onerror=()=>reject(request.error);});
      const [record]=await Promise.all([value,completed]);return await checkedStoredScoreProfile(record,this.#practice);
    }finally{database.close();}
  }
  async commit(expectedRevision,capture){
    if(!Number.isSafeInteger(expectedRevision)||expectedRevision<0||expectedRevision>=Number.MAX_SAFE_INTEGER)throw new Error('Owned native record revision rejected');
    const source=checkedNativeScoreCapture(capture,this.#practice),sha256=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',source.profile)),b=>b.toString(16).padStart(2,'0')).join('');
    const record={schema:'th128-owned-native-title-record/1',revision:expectedRevision+1,provenance:source.native.provenance,tainted:source.native.tainted,profile:source.profile,sha256,native:source.native,runId:source.runId};
    await checkedStoredScoreProfile(record,this.#practice);
    const database=await new Promise((resolve,reject)=>{const request=this.indexedDB.open(DATABASE,1);request.onupgradeneeded=()=>request.result.createObjectStore(STORE);request.onsuccess=()=>resolve(request.result);request.onerror=()=>reject(request.error||new Error('Owned native record write open failed'));request.onblocked=()=>reject(new Error('Owned native record write blocked'));});
    try{
      const transaction=database.transaction(STORE,'readwrite',{durability:'strict'});let issue=null;
      const done=new Promise((resolve,reject)=>{transaction.oncomplete=()=>resolve();transaction.onabort=()=>reject(issue||transaction.error||new Error('Owned native record commit aborted'));transaction.onerror=()=>{};});
      if(transaction.durability!=='strict'){
        transaction.abort();try{await done;}catch{}
        throw new Error('Owned native record strict transaction durability was not granted');
      }
      const request=transaction.objectStore(STORE).get(this.#key);
      request.onerror=()=>{issue=request.error;transaction.abort();};
      request.onsuccess=()=>{
        const previous=request.result;
        if(previous?.runId===source.runId&&previous.sha256===sha256)return;
        if((previous?.revision??0)!==expectedRevision){issue=new Error('Owned native record revision changed before commit');transaction.abort();return;}
        transaction.objectStore(STORE).put(record,this.#key);
      };
      await done;
    }finally{database.close();}
    const observed=await this.observe();
    if(!observed||observed.runId!==source.runId||observed.sha256!==sha256||observed.revision!==record.revision
      ||!sameNativeRecordMetadata(observed.native,source.native)
      ||observed.profile.length!==source.profile.length||observed.profile.some((b,i)=>b!==source.profile[i]))throw new Error('Owned native record full durable payload readback differs');
    return{...observed,strictFullReadback:true,strictDurability:true};
  }
}

const OriginalResultSpellRanks=Object.freeze([0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,4,4,4,4,4,4,4,4,4,4]);
const OriginalResultStageNames=Object.freeze(["init Stage","Stage A1-1","Stage A1-2","Stage A1-3","Stage A2-2","Stage A2-3","Stage B1-1","Stage B1-2","Stage B1-3","Stage B2-2","Stage B2-3","Stage C1-1","Stage C1-2","Stage C1-3","Stage C1-2","Stage C1-3","ExtraStage"," A1 Clear "," A2 Clear "," B1 Clear "," B2 Clear "," C1 Clear "," C2 Clear ","ExtraClear"]);
function originalResultFloat1(value){
  if(!Number.isFinite(value))throw new Error('Original RESULT slowdown is nonfinite');
  const b=new DataView(new ArrayBuffer(4));b.setFloat32(0,value,true);const bits=b.getUint32(0,true),sign=bits>>>31,exponent=(bits>>>23)&255;
  let numerator=BigInt((bits&0x7fffff)+(exponent?0x800000:0))*10n,denominator=1n;
  const power=exponent?exponent-150:-149;if(power>=0)numerator<<=BigInt(power);else denominator<<=BigInt(-power);
  let rounded=numerator/denominator;const remainder=numerator%denominator;if(remainder*2n>denominator||(remainder*2n===denominator&&(rounded&1n)))rounded++;
  return(sign?'-':'')+String(rounded/10n)+'.'+String(rounded%10n);
}
function originalResultName(profile,offset){
  const bytes=profile.subarray(offset,offset+9),end=bytes.indexOf(0);if(end<0)throw new Error('Original RESULT name has no native terminator');
  const name=bytes.subarray(0,end);if(name.some(c=>c<32||c>129))throw new Error('Original RESULT name glyph is outside native style0');
  return Array.from(name,c=>String.fromCharCode(c)).join('');
}
function originalResultCalendar(timestamp){
  if(timestamp===0n)return null;if(timestamp>0x793406fffn)throw new Error('Original RESULT time64 exceeds the native calendar domain');
  const date=new Date(Number(timestamp)*1000);if(!Number.isFinite(date.getTime()))throw new Error('Original RESULT local calendar unavailable');
  const pad=n=>String(n).padStart(2,'0');return String(date.getFullYear()).padStart(4,'0')+'/'+pad(date.getMonth()+1)+'/'+pad(date.getDate())+' '+pad(date.getHours())+':'+pad(date.getMinutes());
}
export function originalResultModel(profile){
  const view=profile===null?null:nativeProfileView(profile),routes=[],spells=Array.from({length:5},()=>[]);
  for(let route=0;route<7;route++){
    const ranks=[];
    for(let rank=0;rank<5;rank++){
      const rows=[];
      for(let row=0;row<10;row++){
        const offset=route*1644+16+rank*320+row*32,scoreRaw=view?view.getUint32(offset,true):100000-row*10000,continues=view?view.getUint8(offset+5):0,stage=view?view.getUint8(offset+4):1;
        const name=view?originalResultName(profile,offset+6):'--------',timestamp=view?view.getBigUint64(offset+16,true):0n,slowdown=view?view.getFloat32(offset+24,true):0;
        if(scoreRaw>999999999||continues>9||stage>=OriginalResultStageNames.length||!Number.isFinite(slowdown))throw new Error('Original RESULT score row scalar rejected');
        const calendar=originalResultCalendar(timestamp),first=String(row+1).padStart(2,' ')+'  '+name+'  '+String(scoreRaw).padStart(9,' ')+String(continues);
        const text=calendar?first+'  '+calendar+'  '+OriginalResultStageNames[stage]+'  '+originalResultFloat1(slowdown)+'%':first+'  ----/--/-- --:--  Stage ----  ---%';
        rows.push({index:row+1,route,rank,name,scoreRaw,continues,stage,timestamp:String(timestamp),calendar,slowdown,text,x:48,y:160+row*18,color:(0xff0000ff|((255-row*16)<<16)|((255-row*16)<<8))>>>0});
      }
      ranks.push(rows);
    }
    routes.push(ranks);
  }
  for(let index=0;index<250;index++){
    const rank=OriginalResultSpellRanks[index],base=14220+12+index*152,first=view?view.getInt32(base+128,true):0,second=view?view.getInt32(base+132,true):0,seen=view?view.getInt32(base+140,true):0;
    if(first<0||second<0||seen<0)throw new Error('Original RESULT spell counter rejected');
    const known=seen!==0;let raw=new Uint8Array(),name='';
    if(known){const record=profile.subarray(base,base+128),end=record.indexOf(0);if(end<0)throw new Error('Original RESULT spell name has no native terminator');raw=record.slice(0,end);name=new TextDecoder('gbk',{fatal:true}).decode(raw);}
    const colorRef=known?(second?0xffff80:first?0xffffa0:0xdfdfdf):0x808080,color=(0xff000000|((colorRef&255)<<16)|(colorRef&0xff00)|((colorRef>>>16)&255))>>>0;
    const label=known?name+' '.repeat(Math.max(0,42-raw.length)):'？'.repeat(21),text='No.'+String(index+1).padStart(3,' ')+' '+label+' '+String(second).padStart(3,' ')+'/'+String(first).padStart(4,' ')+'/'+String(seen).padStart(4,' ');
    const hexASCII=s=>Array.from(s,c=>c.charCodeAt(0).toString(16).padStart(2,'0')).join(''),prefixHex=hexASCII('No.'+String(index+1).padStart(3,' ')+' '),suffixHex=hexASCII(' '+String(second).padStart(3,' ')+'/'+String(first).padStart(4,' ')+'/'+String(seen).padStart(4,' '));
    const labelHex=known?Array.from(raw,b=>b.toString(16).padStart(2,'0')).join('')+'20'.repeat(Math.max(0,42-raw.length)):'a3bf'.repeat(21);
    spells[rank].push({id:index+1,rank,known,name,rawNameHex:Array.from(raw,b=>b.toString(16).padStart(2,'0')).join(''),counters:[second,first,seen],text,textHex:prefixHex+labelHex+suffixHex,colorRef,color});
  }
  let playCount=0,clearCount=0;
  for(let route=0;route<7;route++){
    const plays=view?view.getUint32(route*1644+0x650,true):0;if(plays>99999)throw new Error('Original RESULT native playcount exceeds cap');playCount+=plays;
    for(let rank=0;rank<5;rank++){const clears=view?view.getUint32(route*1644+0x658+rank*4,true):0;if(clears>99999)throw new Error('Original RESULT native clearcount exceeds cap');clearCount+=clears;}
  }
  const hundredths=view?view.getBigUint64(13224,true):0n,seconds=hundredths/100n,pad2=n=>String(n).padStart(2,'0');
  const totals={playCount,playTimeHundredths:String(hundredths),playTimeText:String(seconds/3600n).padStart(3,' ')+':'+pad2(seconds/60n%60n)+':'+pad2(seconds%60n),clearCount};
  return{source:profile===null?'original-defaults':'owned-native-profile',routes,spells,totals};
}
export function originalResultPage(model,{route,rank,page}){
  if(!model||!Array.isArray(model.routes)||model.routes.length!==7||!Array.isArray(model.spells)||model.spells.length!==5
    ||!Number.isInteger(route)||route<0||route>6||!Number.isInteger(rank)||rank<0||rank>(route===6?4:3)||!Number.isInteger(page)||page<0)
    throw new Error('Original RESULT page domain rejected');
  const effectiveRank=route===6?4:rank,pageCount=1+Math.ceil(model.spells[effectiveRank].length/10);if(page>=pageCount)throw new Error('Original RESULT page index rejected');
  return{route,rank:effectiveRank,page,pageCount,kind:page?'spell':'score',rows:page?model.spells[effectiveRank].slice((page-1)*10,page*10):model.routes[route][effectiveRank],totals:model.totals};
}

export function originalResultPacket(model){
  if(!model||!Array.isArray(model.routes)||model.routes.length!==7||!Array.isArray(model.spells)||model.spells.length!==5||!model.totals)throw new Error('Original RESULT snapshot model rejected');
  const parts=[],word=value=>{const bytes=new Uint8Array(4);new DataView(bytes.buffer).setUint32(0,value,true);parts.push(bytes);};
  const text=(bytes,max)=>{if(!(bytes instanceof Uint8Array)||bytes.length>max||bytes.some(c=>c===0))throw new Error('Original RESULT snapshot text rejected');word(bytes.length);parts.push(bytes);};
  const ascii=value=>{if(typeof value!=='string'||Array.from(value).some(c=>c.charCodeAt(0)<32||c.charCodeAt(0)>129))throw new Error('Original RESULT ASCII snapshot rejected');return Uint8Array.from(value,c=>c.charCodeAt(0));};
  word(0x314c5352);word(1);
  for(const route of model.routes){if(!Array.isArray(route)||route.length!==5)throw new Error('Original RESULT snapshot ranks rejected');for(const rank of route){if(!Array.isArray(rank)||rank.length!==10)throw new Error('Original RESULT snapshot score rows rejected');for(const row of rank)text(ascii(row.text),160);}}
  const spells=model.spells.flat().slice().sort((a,b)=>a.id-b.id);if(spells.length!==250)throw new Error('Original RESULT snapshot spell count rejected');
  for(let index=0;index<250;index++){const row=spells[index];if(row.id!==index+1||![0x808080,0xdfdfdf,0xffffa0,0xffff80].includes(row.colorRef)||!(/^(?:[a-f0-9]{2}){1,127}$/).test(row.textHex))throw new Error('Original RESULT spell snapshot rejected');word(row.colorRef);text(Uint8Array.from(row.textHex.match(/../g),b=>parseInt(b,16)),127);}
  const totals=model.totals;if(!Number.isInteger(totals.playCount)||totals.playCount<0||!Number.isInteger(totals.clearCount)||totals.clearCount<0)throw new Error('Original RESULT totals snapshot rejected');
  text(ascii('    '+String(totals.playCount).padStart(5,' ')),64);text(ascii(totals.playTimeText),64);text(ascii('    '+String(totals.clearCount).padStart(5,' ')),64);
  const size=parts.reduce((n,p)=>n+p.length,0);if(size>100000)throw new Error('Original RESULT snapshot byte budget exceeded');const packet=new Uint8Array(size);let offset=0;for(const part of parts){packet.set(part,offset);offset+=part.length;}return packet;
}

function nativeScoreDigest(profile){let value=2166136261;for(const byte of profile)value=Math.imul(value^byte,16777619)>>>0;return value;}
function checkedNativeScoreCapture(capture,practice=false){
  const native=capture?.native;
  if(!native||native.bound!==true||native.started!==true||native.prepared!==true||native.acknowledged!==false
    ||native.normalClear!==false||!(practice?native.provenance==='practice-native-postrun'&&native.tainted===true&&native.nameCommitted===true&&native.route<6&&native.rank<4:
      (native.provenance==='ordinary-native-postrun'&&native.tainted===false&&native.route<6&&native.rank<4)||(native.provenance==='extra-native-postrun'&&native.tainted===true&&native.route===6&&native.rank===4))
    ||native.serial!==1||native.bytes!==PROFILE_BYTES||!Number.isSafeInteger(native.frames)||native.frames<1
    ||!Number.isSafeInteger(native.generation)||native.generation<1||!Number.isInteger(native.route)||native.route<0||native.route>6
    ||!Number.isInteger(native.rank)||native.rank<0||native.rank>4||!Number.isInteger(native.digest)||native.digest<0||native.digest>0xffffffff
    ||typeof capture.runId!=='string'||!(/^[A-Za-z0-9][A-Za-z0-9._:-]{0,159}$/).test(capture.runId))throw new Error('Owned score capture lacks a genuine ordinary native postrun identity');
  nativeProfileView(capture.profile);
  if(nativeScoreDigest(capture.profile)!==native.digest)throw new Error('Owned native score complete profile digest differs');
  return{profile:capture.profile.slice(),native:{...native},runId:capture.runId};
}
export async function persistOwnedNativeScoreCapture(module,store,expectedRevision,capture){
  const source=checkedNativeScoreCapture(capture);
  const check=()=>{const bytes=module._th128_world_records_size(),pointer=module._th128_world_records_data();if(bytes!==PROFILE_BYTES||!Number.isSafeInteger(pointer)||pointer<0||pointer+bytes>module.HEAPU8.length||module.HEAPU8.subarray(pointer,pointer+bytes).some((b,i)=>b!==source.profile[i]))throw new Error('Native pending score payload changed before durable acknowledgement');};
  check();const receipt=await store.commit(expectedRevision,source);
  if(receipt.strictFullReadback!==true||receipt.strictDurability!==true)throw new Error('Native score storage did not attest strict full durable readback');
  if(receipt.runId!==source.runId||!sameNativeRecordMetadata(receipt.native,source.native)
    ||!(receipt.profile instanceof Uint8Array)||receipt.profile.length!==source.profile.length
    ||receipt.profile.some((value,index)=>value!==source.profile[index]))throw new Error('Native score complete receipt identity differs before ACK');
  check();if(module._th128_world_records_ack(source.native.serial,source.native.bytes,source.native.digest)!==1)throw new Error(module.UTF8ToString(module._th128_world_error())||'Native score durable acknowledgement rejected');
  return{...receipt,nativeAcknowledged:true};
}
