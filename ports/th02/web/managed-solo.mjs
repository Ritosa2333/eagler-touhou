import {createSoloAttempt} from './practice-attempt.js';
import {mountPractice} from './practice-ui.js';
import {normalizePractice} from './practice-config.js';
import {mountManagedPractice} from './pc98-practice.mjs';
import {Fat12} from './solo-fat12.js';
import {decodeExternalReplay} from './replay/external-codec.js';
import {userPath} from './managed-storage.mjs';

const host=document.getElementById('game'),input=globalThis.__eaglerPC98Input;
const nativeKeys=[['ArrowUp',1],['ArrowDown',2],['ArrowLeft',4],['ArrowRight',8],['KeyX',16],['KeyZ',32],['Escape',128],['Enter',256]];
const residentSignature=new TextEncoder().encode('MIKOConfig\0');
const sources=['../practice-attempt.js','../direct-drag.js','../player-ui.js','../solo.js','../dosbox-adapter.js','../dosbox-disk.js','../frame-presenter.js','../solo-patch.json','../vendor/dosbox-x/wdosbox-x-fast.wasm','../vendor/dosbox-x/emulators.js','../vendor/dosbox-x/dosbox-jspi.mjs',
  '../touch-input.js','../control-settings.js','./external-host.js','./external-loader.js','./external-codec.js','./external-recorder.js','./external-ui.js','./external-lifecycle.js',
  '../practice-config.js','../practice-engine.js','../practice-assets.js','../practice-assets.json','../practice-core.json','../practice-start.com','../practice-cn.exe.gz','../practice-jp.exe.gz','../vendor/dosbox-x/wdosbox-x-practice.wasm',
  '../solo-music.js','../solo-music.json','../solo-music.com','../solo-native-music.js','../netplay/local-bgm.js','../bgm/manifest.json'];
let canvas=document.getElementById('canvas'),attempt,emulator,music,recorder,patch,mailbox=-1,cursor=0,lastKeys=0,loop=0,playback=false,options={},language='jp',musicMode='ogg',recordBlob,lastPractice;
const report=error=>globalThis.__eaglerPC98Emit({event:'error',message:String(error?.message||error)});
function valid(heap,at){
  if(!patch||at<0||at+104>heap.length)return false;
  for(let i=0;i<patch.signature.length;i++)if(heap[at+i]!==patch.signature.charCodeAt(i))return false;
  const ds=heap[at+20]|heap[at+21]<<8,data=at-patch.entries.mailbox+(patch.dataSegment-patch.codeSegment)*16,base=data-ds*16;
  if(ds<=patch.dataSegment||base<0||base+0x100000>heap.length||data<base||data+0x2800>base+0xa0000||heap[at+22]>2)return false;
  const view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength),resident=base+view.getUint16(data+0x27fc,true)+view.getUint16(data+0x27fe,true)*16;
  return resident>=base&&resident+48<=base+0xa0000&&residentSignature.every((b,i)=>heap[resident+i]===b)&&heap[resident+11]<=5;
}
function readState(){
  if(!emulator)return null;
  const heap=emulator.module.HEAPU8;
  if(!valid(heap,mailbox)){
    mailbox=-1;const end=Math.min(heap.length,cursor+2*1024*1024);
    for(let at=heap.indexOf(84,cursor);at>=0&&at<end;at=heap.indexOf(84,at+1))if(valid(heap,at)){mailbox=at;break;}
    cursor=end===heap.length?0:end;
  }
  if(mailbox<0)return null;
  const view=new DataView(heap.buffer),data=mailbox-patch.entries.mailbox+(patch.dataSegment-patch.codeSegment)*16;
  return {mode:heap[mailbox+22],generation:view.getUint16(mailbox+24,true),ticks:view.getUint32(mailbox+16,true),x:view.getInt16(data+0x2b86,true),y:view.getInt16(data+0x2b88,true),lives:heap[data+0xb2c],bombs:heap[data+0xb2d],hit:heap[data+0x2b94]|heap[data+0xaa7],power:heap[data+0xaa6]};
}
function applyAssist(value){
  if(!emulator)return;const at=value.address,heap=emulator.module.HEAPU8;if(!valid(heap,at))return;
  const view=new DataView(heap.buffer),base=at-patch.entries.mailbox;if(value.generation!==view.getUint16(at+24,true))return;
  view.setUint16(base+patch.entries.currentInput,value.currentInput,true);heap[at+100]=50;heap[at+102]=value.focusEnabled?1:0;heap[at+103]=value.points;
  const touch=base+patch.entries.touch;
  if(value.touchAction==='add'){
    const clamp=n=>Math.max(-8192,Math.min(8191,n));if(value.begin){view.setInt32(touch+2,0,true);view.setInt32(touch+8,0,true);}
    view.setUint16(touch,value.unlimited?3:1,true);view.setInt16(touch+2,clamp(view.getInt16(touch+2,true)+value.x),true);view.setInt16(touch+4,clamp(view.getInt16(touch+4,true)+value.y),true);
  }else if(value.touchAction==='clear'){view.setUint16(touch,0,true);view.setInt32(touch+2,0,true);}
}
function release(){input.releaseGameplay();if(emulator)for(const [code,bit] of nativeKeys)if(lastKeys&bit)emulator.key(code,false);lastKeys=0;}
async function saveReplay(blob){recordBlob=blob;const name=`replay/th02_${Date.now()}_${crypto.randomUUID().slice(0,8)}.th02r`;await globalThis.__eaglerPC98Files.write(name,await blob.arrayBuffer());}
async function syncFiles(){if(!emulator||playback)return;const fat=new Fat12(await emulator.getDiskImage('th02-solo.hdi'));
  for(const name of ['HUUHI.DAT','HUUMA.CFG']){const entry=fat.find('HUMAROKU/'+name,{optional:true});if(entry!==undefined)await globalThis.__eaglerPC98Files.write(name,fat.read(entry));}}
async function naturalExit(){recorder?.finish();const until=performance.now()+10000;while(recorder?.state.ending&&!recorder.state.ready&&!recorder.state.failed&&performance.now()<until)await new Promise(resolve=>setTimeout(resolve,50));await syncFiles();globalThis.__eaglerPC98Emit({event:'exit',code:0});}
const practiceUI=mountPractice({host,onRelease:release,onFocus:()=>canvas.focus(),onChange:value=>emulator?.practiceOptions(value),
  onStart:value=>void restart(value),onRetry:()=>void restart(lastPractice),onSave:()=>{if(recordBlob)void saveReplay(recordBlob).catch(report);},
  onPause:()=>{if(emulator){emulator.key('Escape',true);setTimeout(()=>emulator?.key('Escape',false),120);}}});
const managedPractice=mountManagedPractice(practiceUI,input,{release,readState:()=>({...readState(),practice:emulator?.practice?.status})});
function tick(now){
  if(!emulator)return;const state=readState(),ps=emulator.practice?.status;practiceUI.update({practice:ps,retryReady:false});
  if(ps?.outcome&&!playback)practiceUI.complete({outcome:ps.outcome,recording:!!recorder,ready:!!recordBlob,failed:recorder?.state.failed});
  if(emulator.state==='running'&&!playback){
    const playing=state?.mode===1&&!state.hit;let bits=input.sample({play:playing});
    if(ps?.options.disableBomb)bits&=~16;if(ps?.options.disableShot)bits&=~32;if(ps?.options.disableFocus)bits&=~64;else if(ps?.options.forceFocus)bits|=64;
    if((bits&3)===3)bits&=~3;if((bits&12)===12)bits&=~12;
    for(const [code,bit] of nativeKeys)if(!!(bits&bit)!==!!(lastKeys&bit))emulator.key(code,!!(bits&bit));lastKeys=bits;
    if(state){const motion=input.motion();emulator.assist({address:mailbox,generation:state.generation,currentInput:bits&64?0x100:0,focusEnabled:!ps?.options.disableFocus,points:options.alwaysHitbox||ps?.options.alwaysPoint?8:0,
      touchAction:playing&&motion.active?'add':'clear',begin:!!motion.begin,unlimited:!!motion.unlimited,x:motion.x||0,y:motion.y||0});}
    music?.poll(emulator.module.HEAPU8,now);
  }
  if(music){if(document.hidden||emulator.state!=='running'||state?.mode===2||ps&&(ps.preparing||ps.waiting||ps.outcome))music.suspend();else music.resume();}
  loop=requestAnimationFrame(tick);
}
async function launch(config,replay=null){
  playback=!!replay;lastPractice=normalizePractice(replay?.header.practice||config);practiceUI.setRunning(!playback,lastPractice);
  const settings={language:replay?.header.language||language,audioMode:replay?.header.audioMode||'independent',runtime:'main',recordEnabled:!playback,volume:musicMode==='none'?0:.7};
  attempt=createSoloAttempt({canvas,practice:lastPractice,settings,replay,sources,applyReplayInput:applyAssist,callbacks:{onError:report,onFile:blob=>void saveReplay(blob).catch(report),onExit:()=>void naturalExit().catch(report),onPlaybackEnd:matches=>{if(!matches)report(Error('Replay 终态内存校验不一致'));}}});
  await attempt.task;emulator=attempt.core;patch=attempt.patch;music=attempt.music;recorder=attempt.recorder;attempt.foreground=true;
  if(emulator.practice?.debug.csBase>=0)mailbox=emulator.practice.debug.csBase+patch.entries.mailbox;
  canvas.focus();managedPractice.start({readOnly:playback});loop=requestAnimationFrame(tick);
}
async function restart(config){release();cancelAnimationFrame(loop);attempt?.stop();emulator=music=recorder=undefined;mailbox=-1;cursor=0;recordBlob=undefined;await launch(config);}
export const managedAPI={
  async start(value){options=value.options;language=value.language==='ja'?'jp':'cn';musicMode=value.music;let replay;
    if(options.replayPath){const bytes=globalThis.__eaglerPC98Files.memory.get(userPath(options.replayPath));if(!bytes)throw Error('录像不存在');replay=await decodeExternalReplay(new Blob([bytes]),'02');}
    await launch(practiceUI.configuration(),replay);},
  health(){return {frames:emulator?.module.renderMetrics?.drawn??emulator?.module.framePresenter?.info().drawn??0,audio:{state:emulator?.audio.context.state,musicStatus:music?.player.status??null,effectsPeak:attempt?.sfx?.peak??0}};},
  practiceState(){return {...managedPractice.state(),retry:{state:'idle',last:null}};},
  async files(message){const store=globalThis.__eaglerPC98Files;if(message.command==='sync'){await syncFiles();return {};}
    if(message.command==='list'){await syncFiles();return {files:[...store.memory].map(([path,bytes])=>({path,size:bytes.length}))};}
    const path=userPath(message.path);if(message.command==='read'){await syncFiles();const bytes=store.memory.get(path);if(!bytes)throw Error('文件不存在');return {bytes:Array.from(bytes)};}
    if(emulator)throw Error('请退出游戏后管理文件');if(message.command==='remove'){await store.remove(path);return {};}
    if(message.command==='write'){const bytes=new Uint8Array(message.bytes);if(path.endsWith('.th02r'))await decodeExternalReplay(new Blob([bytes]),'02');await store.write(path,bytes);return {};}
    throw Error('用户文件操作无效');},
  async playback(message){if(message.action!=='open'||emulator)throw Error('请从启动器打开录像');const bytes=globalThis.__eaglerPC98Files.memory.get(userPath(message.path));if(!bytes)throw Error('录像不存在');await launch(practiceUI.configuration(),await decodeExternalReplay(new Blob([bytes]),'02'));return {};},
  stop(){managedPractice.stop();release();cancelAnimationFrame(loop);attempt?.stop();emulator=music=recorder=undefined;mailbox=-1;},
};
