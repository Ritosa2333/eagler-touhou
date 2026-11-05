import {mountPauseMenu} from './pc98-pause-menu.mjs';
import {createSoloAttempt} from './practice-attempt.js';
import {mountPractice} from './practice-ui.js';
import {normalizePractice} from './practice-config.js';
import {mountManagedPractice} from './pc98-practice.mjs';
import {Fat12} from './solo-fat12.js';
import {decodeExternalReplay} from './replay/external-codec.js';
import {userPath} from './managed-storage.mjs';

const host=document.getElementById('game'),input=globalThis.__eaglerPC98Input;
const nativeKeys=[['ArrowUp',1,38],['ArrowDown',2,40],['ArrowLeft',4,37],['ArrowRight',8,39],['KeyX',16,88],['KeyZ',32,90],['ShiftLeft',64,16],['Escape',128,27],['Enter',256,13]];
const sources=['../managed-gameover.mjs','../practice-attempt.js','../direct-drag.js','../player-ui.js','../solo.js','../solo-worker.js','../solo-worker-client.js','../dosbox-adapter.js','../game-version.js','../solo-native-music.js',
  '../touch-input.js','../vendor/dosbox-x/runtime.json','../vendor/dosbox-x/dosbox-jspi.mjs','../vendor/dosbox-x/wdosbox-x-fast.wasm',
  '../dosbox-disk.js','../frame-presenter.js','../solo-worker-environment.js','../solo-patch.json','../solo-patch-jp.json','../vendor/dosbox-x/emulators.js',
  './external-host.js','./external-loader.js','./external-codec.js','./external-recorder.js','./external-ui.js','./external-lifecycle.js',
  '../practice-config.js','../practice-engine.js','../practice-assets.js','../practice-assets.json','../practice-core.json','../practice-start.com','../practice-cn.exe.gz','../practice-jp.exe.gz','../vendor/dosbox-x/wdosbox-x-practice.wasm',
  '../solo-music.js','../solo-music.json','../solo-music.com','../bgm/manifest.json','../netplay/local-bgm.js'];
let canvas=document.getElementById('canvas'),attempt,retrySlot,retryBlocked=false,lastRetry=null,emulator,music,recorder,lastKeys=0,loop=0,playback=false,options={},language='jp',musicMode='ogg',recordBlob,lastPractice,currentSettings;
const report=error=>globalThis.__eaglerPC98Emit({event:'error',message:String(error?.message||error)});
const readState=()=>emulator?.snapshot.state??null;
function key(code,number,down){emulator.send('key',[{type:down?'keydown':'keyup',code,key:code==='KeyZ'?'z':code==='KeyX'?'x':code,keyCode:number,which:number}]);}
function release(){input.releaseGameplay();if(emulator)for(const [code,bit,number] of nativeKeys)if(lastKeys&bit)key(code,number,false);lastKeys=0;emulator?.clearInput();}
let replayResult=null;
let pausePrevious=0;
const pauseMenu=mountPauseMenu(host.querySelector('.solo-screen'),{
  resume(){pauseMenu.close();input.clear();pausePrevious=0;emulator?.run();music?.resume();void attempt?.sfx?.resume();canvas.focus();},
  exit(){pauseMenu.close();void naturalExit().catch(report);},
  discard(){pauseMenu.close();managedAPI.stop();globalThis.__eaglerPC98Emit({event:'exit',code:0});},
  restart(){pauseMenu.close();void restart({...lastPractice,options:practiceUI.options}).catch(report);},
});
const savedReplays=new WeakMap();
async function saveReplay(blob){recordBlob=blob;if(savedReplays.has(blob))return savedReplays.get(blob);const task=(async()=>{const name=`replay/th04_${Date.now()}_${crypto.randomUUID().slice(0,8)}.th04r`;await globalThis.__eaglerPC98Files.write(name,await blob.arrayBuffer());return name;})();savedReplays.set(blob,task);return task;}
async function syncFiles(){if(!emulator||playback)return;const fat=new Fat12(await emulator.getDiskImage('th04-solo.hdi'));
  for(const name of ['GENSOU.SCR','MIKO.CFG']){const entry=fat.find('GENSO/'+name,{optional:true});if(entry!==undefined)await globalThis.__eaglerPC98Files.write(name,fat.read(entry));}}
async function naturalExit(){recorder?.finish();const until=performance.now()+10000;while(recorder?.state.ending&&!recorder.state.ready&&!recorder.state.failed&&performance.now()<until)await new Promise(resolve=>setTimeout(resolve,50));await syncFiles();globalThis.__eaglerPC98Emit({event:'exit',code:0});}
const practiceUI=mountPractice({host,onRelease:release,onFocus:()=>canvas.focus(),onChange:value=>{emulator?.practiceOptions(value);},
  onStart:value=>void restart(value).catch(report),onRetry:()=>void restart({...lastPractice,options:practiceUI.options},true).catch(report),onSave:()=>{if(recordBlob)void saveReplay(recordBlob).catch(report);},
  onPause:()=>{if(emulator){key('Escape',27,true);setTimeout(()=>emulator&&key('Escape',27,false),120);}}});
const managedPractice=mountManagedPractice(practiceUI,input,{release,readState:()=>({...readState(),practice:emulator?.snapshot.practice})});
function tick(){
  if(!emulator)return;const frameState=readState(),sample=input.sample({play:!pauseMenu.visible&&frameState?.mode===1&&!frameState.flags}),edge=sample&~pausePrevious;pausePrevious=sample;
  if(pauseMenu.visible){if(edge&1)pauseMenu.move(-1);if(edge&2)pauseMenu.move(1);if(edge&(32|256))pauseMenu.confirm();else if(edge&(128|16))pauseMenu.confirmResume();loop=requestAnimationFrame(tick);return;}
  const state=frameState,ps=emulator.snapshot.practice;practiceUI.update({practice:ps,retryReady:!!retrySlot?.ready});prepareRetry(ps);
  if(ps?.outcome&&!playback)practiceUI.complete({outcome:ps.outcome,recording:!!recorder,ready:!!recordBlob,failed:recorder?.state.failed});
  if((edge&128)&&state?.mode===1&&!document.documentElement.dataset.browserPracticeOpen?.includes('true')&&!playback){release();emulator.pause();music?.suspend();void attempt?.sfx?.context.suspend();input.clear();pausePrevious=0;pauseMenu.open();loop=requestAnimationFrame(tick);return;}
  if(emulator.state==='running'&&!playback){
    const playing=state?.mode===1&&!state.flags;let bits=sample;
    if(ps?.options.disableBomb)bits&=~16;if(ps?.options.disableShot)bits&=~32;if(ps?.options.disableFocus)bits&=~64;else if(ps?.options.forceFocus)bits|=64;
    if((bits&3)===3)bits&=~3;if((bits&12)===12)bits&=~12;
    for(const [code,bit,number] of nativeKeys)if(!!(bits&bit)!==!!(lastKeys&bit))key(code,number,!!(bits&bit));lastKeys=bits;
    const motion=input.motion();emulator.setInput({address:state?.at,generation:state?.generation,points:(bits&64?1:0)|(options.alwaysHitbox||ps?.options.alwaysPoint?8:0),touch:playing&&motion.active?motion:null});
  }
  if(music){if(document.hidden||emulator.state!=='running'||state?.mode===2||ps?.mode==='target'&&(!ps.ready||ps.waiting||ps.outcome))music.suspend();else music.resume();}
  loop=requestAnimationFrame(tick);
}
function discardRetry(){retrySlot?.stop();retrySlot=undefined;}
function makeAttempt(target,practice,settings,replay=null,standby=false,effects=null){return createSoloAttempt({canvas:target,practice,settings,replay,standby,effects,sources,callbacks:{onError:report,onFile:blob=>void saveReplay(blob).catch(report),onExit:()=>void naturalExit().catch(report),onPlaybackEnd:matches=>{replayResult=matches;if(!matches)report(Error('Replay 终态内存校验不一致'));}}});}
function prepareRetry(ps){
  if(playback||lastPractice?.mode!=='target'){discardRetry();return;}
  if(retrySlot?.cancelled||retrySlot?.error){discardRetry();retryBlocked=true;}
  if(retrySlot||retryBlocked||document.hidden||!ps?.ready||ps.error)return;
  const slot=makeAttempt(canvas.cloneNode(false),{...lastPractice,options:practiceUI.options},currentSettings,null,true);retrySlot=slot;
  slot.task.catch(()=>{if(retrySlot===slot){retrySlot=undefined;retryBlocked=true;}});
}
async function launch(config,replay=null,cached=null,effects=null){
  playback=!!replay;replayResult=null;lastPractice=normalizePractice(replay?.header.practice||config);practiceUI.setRunning(!playback,lastPractice);
  const settings={language:replay?.header.language||language,audioMode:replay?.header.audioMode||'independent',runtime:'worker',recordEnabled:!playback,volume:musicMode==='none'?0:.7};
  currentSettings=settings;attempt=cached||makeAttempt(canvas,lastPractice,settings,replay,false,effects);
  const started=performance.now(),cachedReady=!!cached?.ready;await attempt.task;if(cached)await attempt.core.activatePractice(practiceUI.options);
  if(cached){canvas.replaceWith(attempt.canvas);canvas=attempt.canvas;lastRetry={cached:true,cachedReady,milliseconds:Math.round(performance.now()-started)};}
  emulator=attempt.core;music=attempt.music;recorder=attempt.recorder;attempt.foreground=true;
  canvas.focus();managedPractice.start({readOnly:playback});loop=requestAnimationFrame(tick);
}
async function restart(config,retry=false){const cached=retry?retrySlot:undefined;if(cached)retrySlot=undefined;discardRetry();release();cancelAnimationFrame(loop);const effects=!cached&&!playback?attempt?.takeEffects():null;attempt?.stop();emulator=music=recorder=undefined;recordBlob=undefined;retryBlocked=false;const next=canvas.cloneNode(false);canvas.replaceWith(next);canvas=next;await launch(config,null,cached,effects);}
export const managedAPI={
  async start(value){options=value.options;language=value.language==='ja'?'jp':'cn';musicMode=value.music;let replay;
    if(options.replayPath){const bytes=globalThis.__eaglerPC98Files.memory.get(userPath(options.replayPath));if(!bytes)throw Error('录像不存在');replay=await decodeExternalReplay(new Blob([bytes]),'04');}
    await launch(practiceUI.configuration(),replay);},
  health(){return {replayResult,gameFrame:readState()?.ticks,frames:emulator?.module.frameUploads??emulator?.module.renderMetrics?.drawn??0,audio:{state:emulator?.context?.state,musicStatus:music?.player.status??null,effectsPeak:attempt?.sfx?.peak??0}};},
  practiceState(){return {...managedPractice.state(),loading:attempt?.timings,retry:{state:retrySlot?(retrySlot.ready?'ready':'preparing'):retryBlocked?'unavailable':'idle',last:lastRetry}};},
  finishRecording(){recorder?.finish();},
  saveRecording(){return recordBlob?saveReplay(recordBlob):null;},
  async files(message){const store=globalThis.__eaglerPC98Files;if(message.command==='sync'){await syncFiles();return {};}
    if(message.command==='list'){await syncFiles();return {files:[...store.memory].map(([path,bytes])=>({path,size:bytes.length}))};}
    const path=userPath(message.path);if(message.command==='read'){await syncFiles();const bytes=store.memory.get(path);if(!bytes)throw Error('文件不存在');return {bytes:Array.from(bytes)};}
    if(emulator)throw Error('请退出游戏后管理文件');if(message.command==='remove'){await store.remove(path);return {};}
    if(message.command==='write'){const bytes=new Uint8Array(message.bytes);if(path.endsWith('.th04r'))await decodeExternalReplay(new Blob([bytes]),'04');await store.write(path,bytes);return {};}
    throw Error('用户文件操作无效');},
  async playback(message){if(message.action!=='open'||emulator)throw Error('请从启动器打开录像');const bytes=globalThis.__eaglerPC98Files.memory.get(userPath(message.path));if(!bytes)throw Error('录像不存在');await launch(practiceUI.configuration(),await decodeExternalReplay(new Blob([bytes]),'04'));return {};},
  stop(){pauseMenu.close();pausePrevious=0;managedPractice.stop();discardRetry();release();cancelAnimationFrame(loop);attempt?.stop();emulator=music=recorder=undefined;const next=canvas.cloneNode(false);canvas.replaceWith(next);canvas=next;},
};
