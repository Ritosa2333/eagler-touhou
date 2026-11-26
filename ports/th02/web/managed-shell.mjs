import {mountGameFps} from './pc98-game-fps.mjs';
import {ManagedContent} from './managed-data.mjs';
import {managedInput} from './managed-input.mjs';
import {createDirectInputBridge} from './direct-input-bridge.mjs';
import {createNativeSolo} from './managed-native-solo.mjs';
import {createNativeRoom,createNativeRoomPreview,createNativeRoomSpectator} from './managed-native-room.mjs';
const protocol='eagler-touhou/1',game='th02';
const query=new URL(location.href).searchParams,epoch=Number(query.get('runtimeEpoch'));
if(parent===window||!Number.isSafeInteger(epoch)||epoch<=0)throw Error('请从 Eagler Touhou 启动游戏');
const emit=payload=>parent.postMessage({protocol,game,epoch,...payload,
  ...(payload.event==='error'?{error:payload.message}:{}),
  ...(payload.event==='exit'?{status:payload.code===0?'success':'error'}:{})},location.origin);
const multiplayer=query.get('runtimeVariant')==='multiplayer';
let ready=false,configured=false,launched=false,options={},music='ogg',language='ja',api,preview,launchSerial=0,firstFrame=false,nativeSolo;
let levels=[1,1];
const audioLevels=()=>levels;
globalThis.Module={eaglerSetAudioLevels(music,effects){levels=[music,effects];api?.setAudioLevels?.(music,effects);}};
const nativeActive=()=>launched&&nativeSolo&&api===nativeSolo;
const input=managedInput({game,epoch,active:()=>launched,options:()=>options,
  onKey:(code,down,event)=>{if(nativeActive())nativeSolo.key(code,down,event);},
  allowNativeKey:event=>!(nativeActive()&&(event.metaKey||event.ctrlKey&&event.code==='KeyV')),
  scene:()=>api?.inputScene?.()??0,panelPointer:message=>nativeSolo?.panelPointer(message)});
globalThis.__eaglerPC98Options=()=>options;
function live(message){
  switch(message.command){
    case 'keyboard':if(typeof message.code!=='string'||typeof message.down!=='boolean')throw Error('键盘输入无效');input.key(message);break;
    case 'keyboard-clear':input.clearKeyboard();break;
    case 'touch-cancel':input.cancelTouch();break;
    case 'touch-controls':api?.unlock?.();input.touch(message);break;
    case 'direct-touch':api?.unlock?.();input.pointer(message);break;
    default:return false;
  }return true;
}
globalThis.__eaglerDirectInputBridge=createDirectInputBridge({protocol,game,epoch,origin:location.origin,ready:()=>ready&&launched,dispatch:live});
let queue=Promise.resolve();
async function command(message){
  if(!ready)throw Error('游戏资源尚未就绪');
  if(live(message))return {};
  switch(message.command){
    case 'configure':{
      if(launched)throw Error('请先结束本局');
      if(!['ogg','none'].includes(message.music))throw Error('此版本支持独立音乐或静音');
      if(message.language&&!['ja','lang_en','lang_zh-hans','lang_zh-hant'].includes(message.language))throw Error('游戏语言无效');
      for(const key of ['resources','runtimeResources','sharedResources'])if(message[key]!==undefined&&(!Array.isArray(message[key])||message[key].length))throw Error('此安装包已包含运行所需的资源');
      if(message.runtimePack)throw Error('此入口使用已打包的 PC-98 语言资源');
      if(message.options!==undefined&&(!message.options||typeof message.options!=='object'||Array.isArray(message.options)))throw Error('运行配置无效');
      const next={...message.options};
      if(next.touchSensitivity!==undefined&&(!Number.isFinite(next.touchSensitivity)||next.touchSensitivity<100||next.touchSensitivity>300))throw Error('触控灵敏度无效');
      if(next.musicSource!==undefined&&!['original','akyu'].includes(next.musicSource))throw Error('音乐音源无效');
      const nextLanguage=message.language??'ja';if(nextLanguage!==language)preview?.stop();
      options=next;music=message.music;language=nextLanguage;configured=true;
      globalThis.Module ??= {}; globalThis.Module.eaglerOptions=options;
      return {};
    }
    case 'prepare-room':if(!multiplayer||!configured||launched)throw Error('房间预载状态无效');return preview.prepare(language);
    case 'launch':
      if(!configured||launched)throw Error('游戏启动状态无效');
      if(multiplayer&&options.multiplayerPreflight&&!options.netplaySpectator){
        launchSerial=message.launchSerial??0;
        await preview.prepare(language);launched=true;firstFrame=true;
        reportNative();
        emit({event:'first-frame',launchSerial});return {};
      }
      preview?.stop();launchSerial=message.launchSerial??0;firstFrame=false;launched=true;
      {
        const roomEmit=event=>{if(event.event==='exit'){launched=false;input.clear();}emit(event);};
        api=multiplayer?(options.netplaySpectator?createNativeRoomSpectator(roomEmit,audioLevels):createNativeRoom({input,emit:roomEmit,audioLevels})):nativeSolo;
      try{await api.start({options,music,language});}catch(error){launched=false;await api.stop();throw error;}
        reportNative();return {};
      }
    case 'network-cancel':await api?.sync?.();input.clear();preview?.stop();await api?.stop();launched=false;emit({event:'exit',code:0});return {};
    case 'list':return {files:[]};
    case 'sync':await api?.sync?.();return {};
    case 'spectator-playback':if(!options.netplaySpectator)throw Error('当前不是观战模式');return api.playback(message);
    default:throw Error('未支持的运行命令：'+message.command);
  }
}
function reportNative(){emit({event:'runtime-info',renderer:'SoEW Native',architecture:'wasm',version:'soew-native/1'});}
addEventListener('message',event=>{
  const message=event.data;
  if(event.source!==parent||event.origin!==location.origin||message?.protocol!==protocol||message.game!==game||message.epoch!==epoch||typeof message.command!=='string')return;
  queue=queue.then(async()=>{
    try{const result=await command(message);if(typeof message.request==='string')emit({request:message.request,ok:true,...result});}
    catch(error){if(typeof message.request==='string')emit({request:message.request,ok:false,error:error.message});else emit({event:'error',message:error.message});}
  });
});
try{
  const provider=parent.__eaglerPrepareManagedRuntimeDataV1;
  if(typeof provider!=='function')throw Error('启动器未提供游戏资源');
  const session={game,generation:query.get('gameGeneration'),epoch};
  const result=await provider.call(parent,session);
  const content=await ManagedContent.open(result.buffer,game);
  content.install(globalThis,new URL('.',location.href).href,(path,signal)=>parent.__eaglerReadRuntimeMusicV1({...session,path,signal}));
  if(multiplayer){
    document.getElementById('canvas').remove();
    preview=createNativeRoomPreview(document.getElementById('game'));
  }else{
    nativeSolo=createNativeSolo({canvas:document.getElementById('canvas'),input,audioLevels,
      async exit(pending){launched=false;await pending;input.clear();emit({event:'exit',code:0});},
      fail:error=>emit({event:'error',message:error.message})});
  }
  const fps=mountGameFps();
  ready=true;emit({event:'ready',saveRoot:'/soew/save'+(multiplayer?'mp':''),roomPreview:multiplayer});
  let lastFrames=0,lastAt=performance.now(),lastHealth=0;
  setInterval(()=>{
    if(!launched||!api||options.multiplayerPreflight&&!options.netplaySpectator)return;
    const health=api.health(),now=performance.now();fps.update(health.gameFrame,now);
    if(!firstFrame&&health.frames>0){firstFrame=true;emit({event:'first-frame',launchSerial});}
    if(now-lastHealth<1000)return;lastHealth=now;
    emit({event:'frame-health',fps:(health.frames-lastFrames)*1000/(now-lastAt),frameMs:health.frameMs??0});
    emit({event:'audio-health',backend:'webaudio',...health.audio});lastFrames=health.frames;lastAt=now;
  },100);
  setInterval(()=>{if(launched)api?.sync?.()?.catch(error=>emit({event:'error',message:'存档同步失败：'+error.message}));},10000);
}catch(error){emit({event:'error',message:error.message});const status=document.getElementById('status');if(status)status.textContent=error.message;}
addEventListener('pagehide',()=>{ready=false;input.clear();preview?.stop();api?.stop();});

