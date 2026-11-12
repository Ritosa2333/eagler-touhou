import {createNativeMusic} from './native-music-player.mjs';
import {createManagedTransport} from './managed-transport.mjs';
import {createNativeMatch} from './native-match.mjs';
import {createRollbackMatch} from './native-match-rollback.mjs';
import {packTouch,MAX_INPUT} from './touch-input.js';
import {drawHitPoint,showHitPoint} from './native-hit-point.mjs';
import {encodeReplay,decodeReplay} from './native-replay.mjs';
import {createNativeReplayPlayer} from './native-replay-player.mjs';
import {createConfirmedDriver} from './native-confirmed-driver.mjs';
import {createPc98SpectatorPublisher} from './pc98-spectator-publisher.mjs';
import {createPc98SpectatorChannel} from './pc98-spectator-channel.mjs';
import {createLocalSpectator} from './pc98-local-spectator.mjs';

let engine, transport, match, timer, animation, publisher, viewer, reportTimer, replayPlayer, bgm, run, revision=0, frames=0, audio, audioTime=0, stopped=true, localSeat=0, localPoint=false, lastPlaying=false, menuChoice=0, language='ja',musicMode='ogg';
const canvas = () => document.querySelector('#canvas');
function report(error){if(!stopped){stop();globalThis.__eaglerPC98Emit({event:'error',message:String(error?.message||error)});}}
function readMusicTrack(id){return engine.FS.readFile('/content/YUME1.DAT/'+id.toUpperCase()+'.M');}
function unlock(){if(!audio) audio=new AudioContext({sampleRate:48000});bgm?.unlock(audio);if(audio.state==='suspended')void audio.resume();}
function draw(){
 if(stopped)return;
 if(!engine){animation=requestAnimationFrame(draw);return;}
 const pointer=engine._th03_classic_pixels();
 if(pointer){const context=canvas().getContext('2d',{alpha:false}), image=context.createImageData(640,400);image.data.set(engine.HEAPU8.subarray(pointer,pointer+image.data.length));context.putImageData(image,0,0);if(localPoint){const x=localSeat?engine._th03_classic_point_x2():engine._th03_classic_point_x(),y=localSeat?engine._th03_classic_point_y2():engine._th03_classic_point_y();drawHitPoint(context,x,y);}(match?.pause??viewer?.driver?.pause)?.draw(context,language);frames++;}
 animation=requestAnimationFrame(draw);
}
function play(saved){
 const n=saved?saved.length/2:engine._th03_classic_audio_frames();if(!audio||audio.state!=='running'||!n)return;
 const p=saved?0:engine._th03_classic_audio()>>>2,src=saved??engine.HEAP32.subarray(p,p+n*2),buf=audio.createBuffer(2,n,48000);
 const left=buf.getChannelData(0),right=buf.getChannelData(1);
 for(let i=0;i<n;i++){left[i]=src[i*2]/2147483648;right[i]=src[i*2+1]/2147483648;}
 if(audioTime<audio.currentTime||audioTime>audio.currentTime+.3)audioTime=audio.currentTime+.04;
 const node=audio.createBufferSource();node.buffer=buf;node.connect(audio.destination);node.start(audioTime);audioTime+=n/48000;
}
function stop(){bgm?.stop();bgm=undefined;replayPlayer?.stop();replayPlayer=undefined;stopped=true;clearInterval(reportTimer);publisher?.stop();publisher=undefined;viewer?.stop();viewer=undefined;clearInterval(timer);cancelAnimationFrame(animation);match?.stop();match=undefined;transport?.close();transport=undefined;engine?._th03_classic_destroy();engine=undefined;canvas()?.removeEventListener('pointerdown',clickMenu);canvas()?.removeEventListener('pointerdown',unlock);removeEventListener('keydown',unlock);delete globalThis.__eaglerPC98Input.menuPointer;void audio?.close();audio=undefined;}
function menuPointer(message){const state=match?.pause.state;if(!state||state.owner===null)return false;if(state.owner===localSeat&&message.type==='down'){const r=canvas().getBoundingClientRect(),scale=Math.min(r.width/640,r.height/400),x=(message.x*r.width-(r.width-640*scale)/2)/scale,y=(message.y*r.height-(r.height-400*scale)/2)/scale,left=16+localSeat*320,row=Math.floor((y-100)/46);if(x>=left&&x<left+288&&row>=0&&row<3&&(y-100)%46<40&&(row===0||state.allowed))menuChoice=row+1;}return true;}
function clickMenu(event){unlock();if(event.pointerType==='touch')return;const r=canvas().getBoundingClientRect(),scale=Math.min(r.width/640,r.height/400),left=r.left+(r.width-640*scale)/2,top=r.top+(r.height-400*scale)/2;menuPointer({type:'down',x:(event.clientX-left)/(640*scale),y:(event.clientY-top)/(400*scale)});}
async function loadContent(){
 const content=globalThis.__eaglerPC98Content, entries=[...content.files.keys()].filter(name=>name.startsWith('native-th03/'));
 if(!entries.length)throw Error('缺少原生梦时空资源，请重新构建游戏包');
 engine.FS.mkdir('/content');engine.FS.mkdir('/user');engine.FS.mkdir('/user/REPLAY');
 for(const path of entries){
  const target=path.slice('native-th03/'.length), parts=target.split('/');let dir='/content';
  for(const part of parts.slice(0,-1)){dir+='/'+part;if(!engine.FS.analyzePath(dir).exists)engine.FS.mkdir(dir);}
  const response=await content.fetch(path);if(!response.ok)throw Error('缺少游戏资源：'+target);
  engine.FS.writeFile('/content/'+target,new Uint8Array(await response.arrayBuffer()));
 }
}
async function moduleFactory(){
 if(!globalThis.createTh03Classic) await new Promise((resolve,reject)=>{const script=document.createElement('script');script.src='./native-classic.js';script.onload=resolve;script.onerror=()=>reject(Error('原生运行时加载失败'));document.head.append(script);});
 return globalThis.createTh03Classic({locateFile:name=>name.endsWith('.wasm')?'./native-classic.wasm':name});
}
async function bootToSelection(){
 for(let tick=0;tick<4000;tick++){
  const result=engine._th03_classic_step_pair(0,0,0,0);bgm?.commands(engine.HEAPU8.slice(engine._th03_classic_audio_commands(),engine._th03_classic_audio_commands()+engine._th03_classic_audio_command_count()*4));
  if(result!==0)throw Error('原生选人界面启动失败：'+result);
  if(engine._th03_classic_scene()===3&&engine._th03_classic_boot_state()===0)return;
  if(tick%32===31){await new Promise(resolve=>setTimeout(resolve,0));if(stopped)throw Error('游戏已关闭');}
 }
 throw Error('原生选人界面启动超时');
}

function spectatorReport(){if(!viewer)return;const position=viewer.frame,received=viewer.received;
 globalThis.__eaglerPC98Emit({event:'runtime-info',spectatorPlayback:{version:1,run,revision:++revision,min:viewer.min,position,received,edge:received,target:viewer.target,paused:viewer.paused,seeking:viewer.seeking,following:viewer.following,ended:viewer.ended,nodes:viewer.nodes}});
}
async function startSpectator(options,music){bgm=createNativeMusic({onError:error=>__eaglerPC98Emit({event:'music-error',message:error.message}),source:options.musicSource,muted:music==='none',readTrack:readMusicTrack});
 const url=new URL(options.netplayUrl);run='pc98_03_'+url.searchParams.get('run');revision=0;
 viewer=createLocalSpectator({game:'03',options,smoothPresentation:true,
  stable:JSON.stringify,
  validateHeader(h){if(!h||h.game!=='03'||h.players!==2||h.spectatorInputOnly!==true||h.version!==1||h.tickMs!==1000*9328/526315||typeof h.focus!=='boolean')throw Error('观战开局数据无效');},
  validateRow(row){if(!row||!Array.isArray(row.inputs)||row.inputs.length!==2||row.inputs.some(v=>!Number.isSafeInteger(v)||v<0||v>MAX_INPUT)||![0,1,2].includes(row.scene)||!Number.isInteger(row.round)||row.round< -1||row.round>255||row.scene!==1&&row.round!==-1)throw Error('观战输入无效');},
  channelFactory:()=>{const channel=createPc98SpectatorChannel(options,report);globalThis.__th03PeerTransport={peers:new Map(),get relay(){return channel.relay;}};return channel;},
  async createReplica(header,{onFrame}){
   engine=await moduleFactory();await loadContent();engine._th03_classic_external_music(1);if(engine._th03_classic_create()!==1)throw Error('原生游戏启动失败');engine._th03_classic_start_versus();await bootToSelection();
   const driver=createConfirmedDriver(engine,{focus:header.focus});
   return {get frame(){return driver.frame;},get executed(){return driver.frame;},driver,advance(row){driver.step(row.inputs);if(viewer?.following&&!viewer?.catching){bgm?.commands(engine.HEAPU8.slice(engine._th03_classic_audio_commands(),engine._th03_classic_audio_commands()+engine._th03_classic_audio_command_count()*4));play();}},present(){onFrame();},stop(){engine?._th03_classic_destroy();engine=undefined;}};
  },onError:report});
 reportTimer=setInterval(spectatorReport,200);document.querySelector('#cover').hidden=true;canvas().addEventListener('pointerdown',unlock);addEventListener('keydown',unlock,{once:true});animation=requestAnimationFrame(draw);
}

export const managedAPI={
 async start({options,music,language:requestedLanguage}){musicMode=music;
  if(options.replayViewer){stopped=false;frames=1;language=requestedLanguage;document.querySelector('#cover').hidden=true;return;}
  if(options.netplayPlayerCount!==2||!options.netplaySpectator&&![0,1].includes(options.netplayPlayer))throw Error('梦时空需要双人房间');
  stopped=false;frames=0;language=requestedLanguage;if(options.netplaySpectator){await startSpectator(options,music);return;}engine=await moduleFactory();await loadContent();
  bgm=createNativeMusic({onError:error=>__eaglerPC98Emit({event:'music-error',message:error.message}),source:options.musicSource,muted:music==='none',readTrack:readMusicTrack});engine._th03_classic_external_music(1);
  const result=engine._th03_classic_create();if(result!==1)throw Error('原生游戏启动失败：'+result);
  engine._th03_classic_start_versus();
  await bootToSelection();
  const roles=['host','guest'],seat=options.netplayPlayer;localSeat=seat;localPoint=false;lastPlaying=false;menuChoice=0;language=requestedLanguage;
  transport=createManagedTransport(options,roles,report);
  const channel=transport.channelFor(roles[1-seat]);
  const input=globalThis.__eaglerPC98Input;
  input.menuPointer=menuPointer;
  const rows=[];let replaySerial=0,reportedSerial=0,matchStart=0;const tape={get length(){return rows.length;},slice(a,b){return rows.slice(a,b);},row(i){return rows[i];}};
  if(seat===0)publisher=createPc98SpectatorPublisher({game:'03',players:2,version:1,spectatorInputOnly:true,tickMs:1000*9328/526315,focus:options.netplayDisableFocus!==true},tape,()=>globalThis.__th03PeerTransport?.relay,report);
  const factory=options.netplayAdonisMode===0?createRollbackMatch:createNativeMatch;
  match=factory({seat,channel,engine,sample:()=>{const playing=engine._th03_classic_scene()===9&&match?.pause.state.owner===null;if(playing!==lastPlaying){input.discardMotion();lastPlaying=playing;}let bits=input.sample({play:playing});if(menuChoice){bits=2048|menuChoice;menuChoice=0;}const motion=options.touchEnabled&&playing?input.motion():{};localPoint=showHitPoint({playing,focus:!!(bits&64),focusEnabled:options.netplayDisableFocus!==true,focusPoint:options.focusHitboxEnabled,always:options.alwaysHitbox});return packTouch(bits,{...motion});},options:{rapid:options.th03RapidFire!==false,focus:options.netplayDisableFocus!==true},onConfirmedInput:(frame,inputs,meta)=>{if(rows.length<216000&&frame!==rows.length)throw Error('确认历史无效');if(rows.length<216000){rows.push({inputs,scene:meta?.scene===3?2:meta?.scene===9?1:0,round:meta?.scene===9?meta.round:-1});publisher?.kick();if(seat===0&&meta?.serial>reportedSerial){reportedSerial=meta.serial;if(meta.winner===0||meta.winner===1)globalThis.__eaglerPC98Emit({event:'runtime-info',th03Match:{start:matchStart,frames:frame+1-matchStart,winner:meta.winner}});matchStart=frame+1;}}else{publisher?.stop();publisher=undefined;}},onFrame:(_frame,pcm,commands)=>{bgm?.commands(commands);play(pcm);const serial=engine._th03_classic_replay_serial();if(serial>replaySerial){replaySerial=serial;if(rows.length>=216000)return;const size=engine._th03_classic_replay_size(),ptr=engine._th03_classic_replay_data(),native=engine.HEAPU8.slice(ptr,ptr+size),saved=rows.map(row=>row.inputs);void (async()=>{const bytes=await encodeReplay(saved,options.netplayDisableFocus!==true,native);let path;do{path='replay/native-'+Date.now()+'-'+crypto.getRandomValues(new Uint32Array(1))[0].toString(16)+'.th03r';}while(__eaglerPC98Files.memory.has(path));await __eaglerPC98Files.write(path,bytes);})().catch(report);}},onError:report});
  const connected=new Promise((resolve,reject)=>{const timeout=setTimeout(()=>reject(Error('连接玩家超时')),45000);channel.onopen=()=>{clearTimeout(timeout);resolve();};});
  transport.connect();await connected;if(stopped)throw Error('连接已关闭');
  document.querySelector('#cover').hidden=true;
  canvas().addEventListener('pointerdown',clickMenu);canvas().addEventListener('pointerdown',unlock);canvas().focus();
  addEventListener('keydown',unlock,{once:true});
  timer=setInterval(()=>{if(stopped)return;try{match.tick();}catch(error){report(error);}},1000*9328/526315);
  animation=requestAnimationFrame(draw);
 },
 unlock,
 health(){globalThis.th03NativeFrame=match?.frame??viewer?.frame??0;const scene=engine?._th03_classic_scene()??0,pause=match?.pause?.state,paused=pause?.owner!=null;globalThis.th03SyncState={phase:scene===9?1:0,selecting:scene===3,paused,pauseSeat:pause?.owner??null,pauseSelection:pause?.choice??0};const players=engine?[{seat:0,x:engine._th03_classic_point_x(),y:engine._th03_classic_point_y()},{seat:1,x:engine._th03_classic_point_x2(),y:engine._th03_classic_point_y2()}].filter(p=>p.x>=0&&p.y>=0):[];return {frames,gameFrame:match?.frame??viewer?.frame??0,frame:match?.frame??viewer?.frame??0,confirmed:match?.confirmed??match?.frame??0,confirmedInput:match?.confirmedInput??[],rollbacks:match?.rollbacks??0,running:!!match?.running,scene,paused,pause,players,localHitPoint:localPoint,audio:{state:audio?.state??'suspended',music:bgm?.track??null,musicPlaying:bgm?.playing??false}};},
 files:message=>globalThis.__eaglerPC98FileCommands(message),
 async playback(message){if(__eaglerPC98Options().replayViewer){
  if(['pause','play','seek','rate'].includes(message.action)){if(!replayPlayer)throw Error('请先选择录像');return {playback:await replayPlayer.control(message.action,message.value)};}
  const path=message.path?.replace(/^\/savesth03(?:mp)?\//,'');const bytes=__eaglerPC98Files.memory.get(path);if(!bytes)throw Error('录像文件不存在');const replay=await decodeReplay(bytes);if(replay.mode==='solo')throw Error('录像不属于多人模式');
  if(message.action==='inspect')return {description:{name:path.split('/').pop(),character:-1,difficulty:-1,stages:[{id:0,score:null,frames:replay.rows.length}]}};
  if(message.action!=='open'||message.stage!==undefined&&message.stage!==0)throw Error('录像命令无效');
  replayPlayer?.stop();clearInterval(timer);cancelAnimationFrame(animation);let driver;
  replayPlayer=createNativeReplayPlayer({replay,path,async boot(){engine?._th03_classic_destroy();engine=await moduleFactory();await loadContent();const opts=__eaglerPC98Options();bgm?.stop();bgm=createNativeMusic({onError:error=>__eaglerPC98Emit({event:'music-error',message:error.message}),source:opts.musicSource,muted:musicMode==='none',readTrack:readMusicTrack});engine._th03_classic_external_music(1);if(engine._th03_classic_create()!==1)throw Error('原生游戏启动失败');engine._th03_classic_start_versus();await bootToSelection();bgm.unlock(audio);driver=createConfirmedDriver(engine,{focus:replay.focus});},step(row,_frame,audible){driver.step(row);localSeat=0;localPoint=showHitPoint({playing:engine._th03_classic_point_x()>=0,focus:driver.focus[0],focusPoint:__eaglerPC98Options().focusHitboxEnabled,always:__eaglerPC98Options().alwaysHitbox});if(audible){bgm?.commands(engine.HEAPU8.slice(engine._th03_classic_audio_commands(),engine._th03_classic_audio_commands()+engine._th03_classic_audio_command_count()*4));play();}},onState:state=>globalThis.__eaglerPC98Emit({event:'runtime-info',playback:state}),onFrame(){},onError:report});
  match={get pause(){return driver?.pause;},stop(){},get frame(){return replayPlayer?.state().frame??0;}};canvas().addEventListener('pointerdown',unlock);addEventListener('keydown',unlock,{once:true});animation=requestAnimationFrame(draw);return {playback:await replayPlayer.start()};
 }if(!viewer||message.run!==run)throw Error('观战对局已改变');if(message.action==='pause')viewer.pause(true);else if(message.action==='play')viewer.pause(false);else if(message.action==='live')viewer.live();else if(message.action==='seek')viewer.seek(message.frame);else throw Error('观战命令无效');spectatorReport();return {};},
 stop,
};
