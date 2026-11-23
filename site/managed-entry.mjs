import {openCoopReplaySave} from '../runtime/coop-replay-save.mjs';
import {createCoopLocalReplays} from '../runtime/coop-local-replays.mjs';
import {createCoopTitleMusic} from '../runtime/coop-title-music.mjs';
import {bindCoopResult} from '../runtime/coop-result.mjs';
import {openManagedPauseArt} from '../runtime/coop-pause-art.mjs';
import {mountLauncherSession} from '../../th128/site/runtime/launcher-session.mjs';
import {readManagedProgram,readManagedProgramBytes} from '../runtime/managed-program.mjs';
import {openManagedTh128StageResources} from '../runtime/managed-stage-resources.mjs';
import {mountManagedTh128Room} from '../runtime/managed-room.mjs';
import {openAuthorPeerLink} from '../runtime/author-peer.mjs';
import {createManagedView} from '../runtime/managed-view.mjs';
import {launchManagedRun} from '../runtime/managed-run.mjs';
import {makeTh128ReplayFile} from '../runtime/replay-file.mjs';
import {createManagedReplayMenu} from '../runtime/managed-replay-menu.mjs';
import {historyRules} from '../runtime/confirmed-history.mjs';

const canvas=document.querySelector('#game'),status=document.querySelector('#status'),titleControls=document.querySelector('#title-controls');
const owner=new AbortController(),{signal}=owner;
const byId=id=>document.getElementById(id);
const replayUI=Object.fromEntries(['menu','file','controls','toggle','restart','seat','seek','frame','return','status'].map(key=>[key,byId('replay-'+key)]));
let resources=null,view=null,bridge=null,replay=null,downloadUrl=null,downloadTimer=null,replayFrame=null,scrubbing=false;
function scheduleReplay(){
  const phase=replay?.status().playback?.phase;
  if(signal.aborted||document.hidden||!['playing','seeking'].includes(phase)){
    if(replayFrame!==null)cancelAnimationFrame(replayFrame);replayFrame=null;return;
  }
  if(replayFrame===null)replayFrame=requestAnimationFrame(now=>{replayFrame=null;replay.advance(now,!document.hidden);scheduleReplay();});
}
function refreshReplay(state){
  const current=state.playback,blocked=state.loading||state.returning||['seeking','opening'].includes(current?.phase);
  replayUI.menu.hidden=!state.enabled;titleControls.hidden=!roomTitle||state.enabled;
  if(state.enabled!==shelfShown){shelfShown=state.enabled;if(shelfShown)void refreshShelf();else ++shelfTicket;}
  byId('replay-slot-open').disabled=byId('replay-slot-download').disabled=blocked;
  replayUI.file.disabled=blocked;
  replayUI.controls.hidden=!current;
  replayUI.return.hidden=!state.away;replayUI.return.disabled=state.returning;
  replayUI.toggle.textContent=current?.phase==='playing'?'暂停':'播放';
  replayUI.toggle.disabled=blocked||!current||current.phase==='ended';
  replayUI.restart.disabled=blocked||!current?.total;
  replayUI.seat.disabled=blocked||!current;
  replayUI.seat.textContent=`切换到 P${(1+(current?.seat??0))%(current?.seatCount??2)+1}`;
  replayUI.seek.disabled=blocked||!current?.total;
  if(current){
    replayUI.seek.max=String(current.total);
    if(!scrubbing||blocked)replayUI.seek.value=String(current.target??current.frame);
    const label=`${scrubbing&&!blocked?replayUI.seek.value:current.frame} / ${current.total} 帧`;
    if(replayUI.frame.textContent!==label)replayUI.frame.textContent=label;
  }else scrubbing=false;
  const messageText=state.error||(state.loading?'正在读取多人录像…':state.returning?'正在返回标题…':current?.phase==='seeking'?'正在定位…':state.filename??'');
  if(replayUI.status.textContent!==messageText)replayUI.status.textContent=messageText;
  replayUI.status.hidden=!messageText;scheduleReplay();
}
replayUI.file.addEventListener('change',()=>{
  const file=replayUI.file.files?.[0];replayUI.file.value='';if(file)void replay?.open(file);
},{signal});
replayUI.toggle.addEventListener('click',()=>{void replay?.act(replay.status().playback?.phase==='playing'?'pause':'play');},{signal});
replayUI.restart.addEventListener('click',()=>{void replay?.act('restart').then(ready=>{if(ready)void replay.act('play');});},{signal});
replayUI.seat.addEventListener('click',()=>{void replay?.act('seat',((replay.status().playback?.seat??0)+1)%(replay.status().playback?.seatCount??2));},{signal});
replayUI.seek.addEventListener('input',()=>{const state=replay?.status();if(state){scrubbing=true;refreshReplay(state);}},{signal});
replayUI.seek.addEventListener('blur',()=>{scrubbing=false;const state=replay?.status();if(state)refreshReplay(state);},{signal});
replayUI.seek.addEventListener('change',()=>{
  const frame=Number(replayUI.seek.value);scrubbing=false;void replay?.act('seek',frame);
},{signal});
replayUI.return.addEventListener('click',()=>{void replay?.returnToTitle();},{signal});
document.addEventListener('visibilitychange',()=>{if(document.hidden)replay?.advance(performance.now(),false);scheduleReplay();},{signal});
function releaseDownload(){
  if(downloadTimer!==null)clearTimeout(downloadTimer);downloadTimer=null;
  if(downloadUrl!==null)URL.revokeObjectURL(downloadUrl);downloadUrl=null;
}
signal.addEventListener('abort',releaseDownload,{once:true});
let roomLaunch=0;
let activeRun=null,postrunReview=false,roomTitle=false,titleMusic=null,titleMusicEnabled=false,shelf=null,shelfTicket=0,shelfShown=false;
async function refreshShelf(){
  const token=++shelfTicket;byId('replay-shelf').hidden=true;
  try{const rows=await shelf.list();if(token!==shelfTicket||signal.aborted||!shelfShown||!rows.length)return;
    const slots=byId('replay-slots');slots.replaceChildren(...rows.map(row=>new Option(Number.isInteger(row.slot)?`No.${String(row.slot+1).padStart(2,'0')} · ${row.playerName||'--------'}`:`${new Date(row.savedAt).toLocaleString()} · ${row.name}`,row.id)));
    byId('replay-shelf').hidden=false;
  }catch(error){if(!signal.aborted)console.warn('Multiplayer replay shelf:',error.message);}
}
byId('replay-slot-open').addEventListener('click',()=>{
  const token=shelfTicket,key=byId('replay-slots').value;
  if(shelfShown&&key)void shelf.get(key).then(file=>{if(file&&token===shelfTicket&&shelfShown)void replay.open(file);}).catch(error=>console.warn(error.message));
},{signal});
byId('replay-slot-download').addEventListener('click',()=>{
  const token=shelfTicket,key=byId('replay-slots').value;
  if(shelfShown&&key)void shelf.get(key).then(file=>{if(file&&token===shelfTicket&&shelfShown)exportReplay(file);}).catch(error=>console.warn(error.message));
},{signal});
function exportReplay(file){
  if(signal.aborted||downloadUrl!==null)return;
  downloadUrl=URL.createObjectURL(file);
  const link=document.createElement('a');link.href=downloadUrl;link.download=file.name;
  try{link.click();downloadTimer=setTimeout(releaseDownload,1500);}catch(error){releaseDownload();report(error);}
}
const results=bindCoopResult({panel:byId('coop-result'),caption:byId('coop-result-label'),save:byId('save-replay'),download:byId('coop-download-replay'),review:byId('coop-review'),open:byId('coop-open-replay'),back:byId('coop-back-room')},{signal,
  makeFile:bytes=>makeTh128ReplayFile(bytes,resources.build),exportFile:exportReplay,returnRoom:returnToRoom,
  async save(file){
    const launch=roomLaunch,run=activeRun;
    try{await shelf.save(file);if(launch===roomLaunch&&run===activeRun&&shelfShown)void refreshShelf();}
    catch(error){if(launch===roomLaunch&&run===activeRun&&!signal.aborted)show('录像保存失败：'+String(error.message??error));}
  },
  enterReplay(file){
    activeRun?.close();postrunReview=true;replay.enter();
    if(file)void replay.open(file);
  }});
byId('title-replay').addEventListener('click',()=>{if(roomTitle&&canvas.dataset.scene==='title'){titleMusic?.hide();replay?.enter();}},{signal});
function returnToRoom(){void bridge.requestRoomReturn().catch(report);}
function nativeResult(action){if(action==='end'||action==='restart')returnToRoom();}
function show(message){status.textContent=message;status.hidden=!message;}
function report(error){if(!signal.aborted){show(String(error.message??error));bridge?.reportError(error);}}
async function dispose(){
  if(signal.aborted)return;
  owner.abort();scheduleReplay();try{view?.close();}finally{resources?.close();}
}
window.addEventListener('pagehide',()=>{void dispose();},{once:true});

try{
  const page=location.href,program=await readManagedProgram(page,{signal});
  const executable=new Map(Object.values(program).filter(pin=>pin?.url).map(pin=>[pin.url,pin]));
  resources=await openManagedTh128StageResources({program,signal,
    async loadProgram(path,childSignal){return readManagedProgramBytes(executable.get(path),page,{signal:childSignal});}});
  shelf=createCoopLocalReplays({build:resources.build,signal});
  const {default:factory}=await import(resources.url('core'));
  // These files are in the same independently identified Runtime generation
  // as the entry. Do not accept the package's data files as program code.
  await readManagedProgramBytes(program.peer,page,{signal});
  const peerWasm=await readManagedProgramBytes(program.peerWasm,page,{signal});
  const {default:peerFactory}=await import(program.peer.url);
  const coopFactory=program.gameplay==='same-stage-coop'?(await import(resources.url('coopCore'))).default:null;
  view=await createManagedView({canvas,textCanvas:byId('stage-text'),resources,factory,coopFactory,signal,onError:report});
  const gestureTargets=[window];try{if(window.parent!==window&&window.parent.location.origin===location.origin)gestureTargets.push(window.parent);}catch{}
  titleMusic=createCoopTitleMusic({resources,signal,eventTargets:gestureTargets,onState:value=>{canvas.dataset.roomMusic=value;},onError:error=>console.warn('Room title audio:',error.message)});
  replay=createManagedReplayMenu({build:resources.build,signal,
    openWorld:async(wire,replaySignal)=>{
      const scene=await view.openWorld(historyRules(wire),replaySignal,{confirmedOnly:true});return {scene,engine:scene};
    },
    render({scene},seat,frame){scene.display(seat,frame,0);canvas.dataset.scene='replay';},
    restoreTitle:async replaySignal=>{const shown=await view.presentTitle(replaySignal,{once:postrunReview});if(shown&&postrunReview){postrunReview=false;results.restore();}else if(shown&&roomTitle)titleMusic.show(titleMusicEnabled);return shown;},onChange:refreshReplay});
  bridge=mountManagedTh128Room({identity:resources.identity,window,document,location,verifyMounts:resources.verifyMounts,
    async presentTitle({config,signal:roomSignal}){
      titleMusicEnabled=config?.music==='ogg';
      roomTitle=false;activeRun=null;postrunReview=false;results.suspend(false);results.hide();replay.suspend();const drawn=await view.presentTitle(roomSignal);
      if(drawn){roomTitle=true;titleControls.hidden=false;titleMusic.show(titleMusicEnabled);show('');}return drawn;
    },
    async launchRoom({room,present,signal,config}){
      const launch=++roomLaunch;
      roomTitle=false;titleMusic.hide();view.stopTitle();replay.suspend();titleControls.hidden=true;show('正在连接房间…');
      activeRun=null;postrunReview=false;results.suspend(false);results.hide();activeRun=await launchManagedRun({room,canvas,view,onLocalResult:value=>{if(!signal.aborted)void results.show(value.result==='team-over'?null:value);},
        onNativeResult:nativeResult,
        onReplaySaveActive(active){if(launch===roomLaunch&&!signal.aborted)results.suspend(active);},
        openReplaySave:(bytes,options)=>openCoopReplaySave({bytes,...options,resources,factory:coopFactory,shelf,
          onSaved(){if(shelfShown)void refreshShelf();},
          onError(error){show('录像保存失败：'+String(error.message??error));}}),
        openPauseArt:owner=>openManagedPauseArt({resources,factory,signal:owner}),build:resources.build,signal,soundResources:resources,audioEventTargets:gestureTargets,musicEnabled:config.music==='ogg',
        present(actual,path){const acknowledged=present(actual,path);if(acknowledged){show('');canvas.focus({preventScroll:true});}return acknowledged;},
        onError:report,onStatus:value=>{if(value.replaySaveError)show('录像保存菜单无法打开：'+value.replaySaveError);else bridge.runtimeInfo(value);},
        async openPeer(selection,onMessage,ownerSignal){
          const module=await peerFactory({wasmBinary:peerWasm,eaglerOptions:selection.transport});ownerSignal.throwIfAborted();
          return openAuthorPeerLink({module,room:selection,onMessage,signal:ownerSignal});
        }});return activeRun;
    },disposeResources:dispose,
  });
  if(!bridge?.markReady())throw new Error('多人游戏需要由启动器加载');
  show('等待启动器准备房间…');
  bridge.runtimeInfo({version:'th128-managed-mp/1',packageRevision:resources.revision,runtimeGeneration:resources.identity.generation});
}catch(error){
  if(!signal.aborted){bridge??=mountLauncherSession();report(error);await dispose();}
}
