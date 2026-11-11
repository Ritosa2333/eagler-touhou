import {ManagedContent,installManagedWorkers} from './managed-data.mjs';
import {managedInput} from './managed-input.mjs';
import {openUserFiles,userPath} from './managed-storage.mjs';
import {createDirectInputBridge} from './direct-input-bridge.mjs';
import {titlePreview} from './managed-preview.mjs';
const protocol='eagler-touhou/1',game='th01';
const query=new URL(location.href).searchParams,epoch=Number(query.get('runtimeEpoch'));
if(parent===window||!Number.isSafeInteger(epoch)||epoch<=0)throw Error('请从 Eagler Touhou 启动游戏');
const emit=payload=>parent.postMessage({protocol,game,epoch,...payload,
  ...(payload.event==='error'?{error:payload.message}:{}),
  ...(payload.event==='exit'?{status:payload.code===0?'success':'error'}:{})},location.origin);
const multiplayer=query.get('runtimeVariant')==='multiplayer';
let ready=false,configured=false,launched=false,options={},music='ogg',language='ja',api,preview,store,launchSerial=0,firstFrame=false;
const input=managedInput({game,epoch,active:()=>launched,options:()=>options});
globalThis.__eaglerPC98Input=input;globalThis.__eaglerPC98Emit=emit;globalThis.__eaglerPC98Options=()=>options;
function live(message){
  switch(message.command){
    case 'keyboard':if(typeof message.code!=='string'||typeof message.down!=='boolean')throw Error('键盘输入无效');input.key(message);break;
    case 'keyboard-clear':case 'touch-cancel':input.clear();break;
    case 'touch-controls':input.touch(message);break;
    case 'direct-touch':input.pointer(message);break;
    default:return false;
  }return true;
}
globalThis.__eaglerDirectInputBridge=createDirectInputBridge({protocol,game,epoch,origin:location.origin,ready:()=>ready&&launched,dispatch:live});
globalThis.__eaglerPC98FileCommands=async message=>{
  if(message.command==='sync')return {};
  if(message.command==='list')return {files:[...store.memory].map(([path,bytes])=>({path,size:bytes.byteLength}))};
  const path=userPath(message.path);
  if(message.command==='read'){const bytes=store.memory.get(path);if(!bytes)throw Error('文件不存在');return {bytes:Array.from(bytes)};}
  if(launched)throw Error('请退出游戏后管理文件');
  if(message.command==='remove'){await store.remove(path);return {};}
  if(message.command==='write'){
    if(!Array.isArray(message.bytes)||message.bytes.some(n=>!Number.isInteger(n)||n<0||n>255))throw Error('用户文件字节无效');
    const bytes=new Uint8Array(message.bytes);
    if(path.endsWith('.th01r')){throw Error('TH01 命令录像解码尚未接入');}
    await store.write(path,bytes);return {};
  }throw Error('用户文件操作无效');
};
let queue=Promise.resolve();
async function command(message){
  if(!ready)throw Error('游戏资源尚未就绪');
  if(live(message))return {};
  switch(message.command){
    case 'configure':{
      if(launched)throw Error('请先结束本局');
      if(!['ogg','none'].includes(message.music))throw Error('此版本支持独立音乐或静音');
      if(message.language&&!['ja','lang_zh-hans'].includes(message.language))throw Error('游戏语言无效');
      for(const key of ['resources','runtimeResources','sharedResources'])if(message[key]!==undefined&&(!Array.isArray(message[key])||message[key].length))throw Error('此安装包已包含运行所需的资源');
      if(message.runtimePack)throw Error('此入口使用已打包的 PC-98 语言资源');
      if(message.options!==undefined&&(!message.options||typeof message.options!=='object'||Array.isArray(message.options)))throw Error('运行配置无效');
      const next={...message.options};
      if(next.touchSensitivity!==undefined&&(!Number.isFinite(next.touchSensitivity)||next.touchSensitivity<100||next.touchSensitivity>300))throw Error('触控灵敏度无效');
      if(next.netplaySpectator||next.replayViewer)throw Error('此接入预览尚未启用多人观战与录像工作台');
      const nextLanguage=message.language??'ja';if(nextLanguage!==language)preview?.stop();
      options=next;music=message.music;language=nextLanguage;configured=true;
      // Keep the legacy Runtime diagnostics surface available to the main
      // Launcher even though this adapter is hosted by a managed shell.
      globalThis.Module ??= {}; globalThis.Module.eaglerOptions=options;
      return {};
    }
    case 'resources':if(!Array.isArray(message.resources)||message.resources.length)throw Error('此资源包已包含运行所需的资源');return {};
    case 'prepare-room':if(!multiplayer||!configured||launched)throw Error('房间预载状态无效');return preview.prepare(language);
    case 'launch':
      if(!configured||launched)throw Error('游戏启动状态无效');
      if(multiplayer&&options.multiplayerPreflight){
        launchSerial=message.launchSerial??0;
        await preview.prepare(language);launched=true;firstFrame=true;
        emit({event:'runtime-info',renderer:'TH01 native WASM',architecture:'wasm',version:'pc98-managed-preview/1'});
        emit({event:'first-frame',launchSerial});return {};
      }
      preview?.stop();launchSerial=message.launchSerial??0;firstFrame=false;launched=true;
      try{await api.start({options,music,language});}catch(error){launched=false;api.stop();throw error;}
      emit({event:'runtime-info',renderer:'TH01 native WASM',architecture:'wasm',version:'pc98-managed-preview/1'});return {};
    case 'network-cancel':await api.files({command:'sync'});input.clear();preview?.stop();api.stop();launched=false;emit({event:'exit',code:0});return {};
    case 'list':case 'read':case 'write':case 'remove':case 'sync':return api.files(message);
    case 'playback':return api.playback(message);
    default:throw Error('未支持的运行命令：'+message.command);
  }
}
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
  const result=await provider.call(parent,{game,generation:query.get('gameGeneration'),epoch});
  const content=await ManagedContent.open(result.buffer,game),base=new URL('.',location.href).href;
  content.install(globalThis,base);installManagedWorkers(globalThis,content,base);
  store=globalThis.__eaglerPC98Files=await openUserFiles({multiplayer});
  if(multiplayer){
    const html=await(await fetch('./managed-room.html')).text(),source=new DOMParser().parseFromString(html,'text/html');
    for(const link of source.querySelectorAll('link[rel="stylesheet"]'))document.head.append(link.cloneNode());
    document.body.replaceChildren(...source.body.childNodes);
    const style=document.createElement('style');style.textContent='#site-header,main>:not(#player),#player>:not(#screen),#screen>:not(canvas),.diagnostics{display:none!important}main{margin:0;padding:0}#player{position:fixed;inset:0;margin:0;padding:0;border:0}#cover,#menu-touch,.toolbar{display:none!important}#screen{position:relative;width:100%;height:100%;aspect-ratio:auto}#canvas{width:100%;height:100%;object-fit:contain}.player-stage{position:relative;width:100%;height:100%}.player-stage iframe{width:100%;height:100%;border:0}.player-toolbar,.player-actions,.player-movement,.player-note,[class^="touch-"],[data-layout-control],.room-ui-return{display:none!important}';document.head.append(style);
    api=(await import('./netplay/app.js')).managedAPI;preview=titlePreview(document.querySelector('#screen'));
  }else api=(await import('./app.js')).managedAPI;
  ready=true;emit({event:'ready',saveRoot:'/savesth01'+(multiplayer?'mp':''),roomPreview:multiplayer});
  let lastFrames=0,lastAt=performance.now(),lastHealth=0;
  setInterval(()=>{
    if(!launched||options.multiplayerPreflight)return;
    const health=api.health(),now=performance.now();
    if(!firstFrame&&health.frames>0){firstFrame=true;emit({event:'first-frame',launchSerial});}
    if(now-lastHealth<1000)return;lastHealth=now;
    emit({event:'frame-health',fps:(health.frames-lastFrames)*1000/(now-lastAt),frameMs:health.frameMs??0});
    emit({event:'audio-health',backend:'webaudio',...health.audio});lastFrames=health.frames;lastAt=now;
  },100);
  setInterval(()=>{if(launched)api.files({command:'sync'}).catch(error=>emit({event:'error',message:'存档同步失败：'+error.message}));},10000);
}catch(error){emit({event:'error',message:error.message});const status=document.getElementById('status');if(status)status.textContent=error.message;}
addEventListener('pagehide',()=>{ready=false;input.clear();preview?.stop();api?.stop();store?.close();});

