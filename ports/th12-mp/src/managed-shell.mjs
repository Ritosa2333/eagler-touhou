import {validateMultiplayerOptions} from './multiplayer.mjs';
import {allowedPath} from './storage.mjs';
import {validateNativeProfile} from './native-profile.mjs';

const boolOptions=['limitPresentationTo60','touchEnabled','doubleTapBombEnabled','alwaysHitbox','thpracEnabled','focusHitboxEnabled','replayViewer','netplayAllowWebsocketFallback'];
const netplayOptions=['netplayMode','netplayUrl','netplayPlayer','netplayPlayerCount','netplaySeed','netplayDifficulty','netplayInputDelay','netplayInputDelayAuto','netplayAdonisMode','netplayPredictionLimit','netplayPredictionReserve','netplayChallengeMode','netplaySpectator','netplaySpectatorId','netplaySpectatorCount','netplaySpectatorTimeline','netplayIceServers','netplayPreferTurn','netplayLoadouts','multiplayerLocalPlayerVisibility','touchAlwaysHitbox'];
const acceptedOptions=new Set([...netplayOptions,...boolOptions,'touchMovementMode','touchSensitivity','touchFocusMode','oggDecodeMode','replayPath','debugHarness','thpracLocale']);
export function runtimeOptions(value={}){
  if(!value||typeof value!=='object'||Array.isArray(value))throw Error('运行配置无效');
  for(const key of Object.keys(value))if(!acceptedOptions.has(key))throw Error('此版本未支持的运行配置：'+key);
  for(const key of boolOptions)if(value[key]!==undefined&&typeof value[key]!=='boolean')throw Error('运行配置无效：'+key);
  if(value.touchMovementMode!==undefined&&!['touch','touch-unlimited','joystick','joystick-free'].includes(value.touchMovementMode))throw Error('触控模式无效');
  if(value.touchFocusMode!==undefined&&!['two-finger','hold-button','toggle-button'].includes(value.touchFocusMode))throw Error('低速模式无效');
  if(value.touchSensitivity!==undefined&&(!Number.isFinite(value.touchSensitivity)||value.touchSensitivity<100||value.touchSensitivity>300))throw Error('触控灵敏度无效');
  if(value.oggDecodeMode!==undefined&&!['stream','full'].includes(value.oggDecodeMode))throw Error('音乐解码模式无效');
  // These are deliberately rejected until their complete owners are connected.
  // A successful configure must not silently discard a required behavior.
  if(value.focusHitboxEnabled)throw Error('此接入版本尚未完成所选判定点增强');
  if(value.thpracLocale!==undefined&&!['ja-JP','en-US','zh-CN'].includes(value.thpracLocale))throw Error('练习工具语言无效');
  if(value.replayPath!==undefined&&!savePath(value.replayPath).toLowerCase().endsWith('.rpy'))throw Error('录像路径无效');
  if(value.debugHarness!==undefined&&value.debugHarness!==null)throw Error('诊断工具需要独立诊断构建');
  if(value.netplayMode||value.replayViewer)validateMultiplayerOptions(value);
  if(value.thpracEnabled)throw Error('TH12 多人版不启用 THPrac');
  return {...value,limitPresentationTo60:value.limitPresentationTo60??true};
}
export function savePath(value){
  if(typeof value!=='string')throw Error('存档路径无效');
  const path=allowedPath(value.startsWith('/savesth12/')?value:'/savesth12/'+value);
  if(path.toLowerCase().endsWith('/web-profile.json'))throw Error('此文件属于浏览器内部记录');return path;
}
async function importedFile(path,bytes,api){
  if(path.toLowerCase().endsWith('.rpy')){await api.validateReplay(bytes);return;}
  validateNativeProfile(path,bytes);
}
export function managedCommandHandler(api){
  let configured=false,configuredLanguage='ja',configuredPresentationLimit;
  return async message=>{
    switch(message.command){
    case 'configure':{
      if(api.running())throw Error('游戏运行中不能重新配置');
      if(!['ogg','none'].includes(message.music))throw Error('星莲船未支持此音乐模式');
      const language=message.language||message.runtimePack?.language||configuredLanguage;
      if(language!=='ja'&&!/^lang_[a-z0-9-]+$/i.test(language))throw Error('尚未安装此语言的星莲船资源');
      if(message.runtimePack&&message.runtimePack.language!==language)throw Error('语言包与所选语言不匹配');
      const options=runtimeOptions(message.options),resources=[];
      if(message.options?.limitPresentationTo60===undefined&&configuredPresentationLimit!==undefined)options.limitPresentationTo60=configuredPresentationLimit;
      for(const key of ['resources','runtimeResources','sharedResources']){if(message[key]!==undefined&&!Array.isArray(message[key]))throw Error('附加资源列表无效');resources.push(...(message[key]??[]));}
      await api.resources(resources);await api.configure({options,music:message.music,language,runtimePack:message.runtimePack??null});configured=true;configuredLanguage=language;configuredPresentationLimit=options.limitPresentationTo60;return {};
    }
    case 'resources':await api.resources(message.resources);return {};
    case 'prepare-room':if(!configured)throw Error('请先配置游戏');return api.prepareRoom();
    case 'return-room':return api.returnRoom();
    case 'spectator-playback':return api.spectatorPlayback(message);
    case 'network-cancel':return api.returnRoom();
    case 'launch':if(!configured)throw Error('请先配置游戏');if(api.running())throw Error('游戏已启动');await api.launch(message);return {};
    case 'keyboard':if(typeof message.code!=='string'||typeof message.down!=='boolean')throw Error('键盘输入无效');api.key(message.code,message.down,message);return {};
    case 'keyboard-clear':api.clearKeyboard();return {};
    case 'touch-cancel':api.cancelTouch();return {};
    case 'touch-controls':{
      if(typeof message.fireEnabled!=='boolean'||typeof message.focusEnabled!=='boolean'||!['bombSerial','escapeSerial'].every(key=>Number.isSafeInteger(message[key])&&message[key]>=0)||!['joystickX','joystickY'].every(key=>Number.isFinite(message[key])&&Math.abs(message[key])<=32767))throw Error('触控快照无效');
      if(message.touchSensitivity!==undefined&&(!Number.isFinite(message.touchSensitivity)||message.touchSensitivity<100||message.touchSensitivity>300))throw Error('触控灵敏度无效');
      for(const key of ['thpracMobilePanelsEnabled','thpracMobilePanelsVisible'])if(message[key]!==undefined&&typeof message[key]!=='boolean')throw Error('练习面板配置无效');
      api.touch(message);
      return {};
    }
    case 'direct-touch':if(!['down','move','up'].includes(message.type)||!Number.isSafeInteger(message.id)||!Number.isFinite(message.x)||!Number.isFinite(message.y))throw Error('触控坐标无效');api.pointer(message);return {};
    case 'thprac-mouse':if(!['down','move','up'].includes(message.type)||!Number.isFinite(message.x)||!Number.isFinite(message.y)||message.id!==undefined&&!Number.isSafeInteger(message.id)||message.button!==undefined&&(!Number.isInteger(message.button)||message.button<0||message.button>4))throw Error('练习面板坐标无效');api.practicePointer(message);return {};
    case 'playback':{
      if(!['inspect','open','pause','play','seek','rate'].includes(message.action))throw Error('录像操作无效');
      const command={action:message.action};
      if(['inspect','open'].includes(message.action)){command.path=savePath(message.path);if(!command.path.toLowerCase().endsWith('.rpy'))throw Error('需要录像文件');}
      if(message.action==='open'&&message.stage!==undefined){if(!Number.isInteger(message.stage)||message.stage<1||message.stage>7)throw Error('录像关卡无效');command.stage=message.stage;}
      if(message.action==='seek'){if(!Number.isSafeInteger(message.value)||message.value<0)throw Error('录像位置无效');command.value=message.value;}
      if(message.action==='rate'){if(![0,.5,1,2,4,8].includes(message.value))throw Error('录像速度无效');command.value=message.value;}
      return api.playback(command);
    }
    case 'list':{
      await api.sync();const files=[];for(const path of await api.store.list()){if(path.endsWith('/web-profile.json'))continue;const bytes=await api.store.read(savePath(path));files.push({path:savePath(path).slice('/savesth12/'.length),size:bytes?.byteLength??0});}return {files};
    }
    case 'read':{await api.sync();const bytes=await api.store.read(savePath(message.path));if(!bytes)throw Error('文件不存在');return {bytes:Array.from(bytes)};}
    case 'write':{
      const path=savePath(message.path);
      if(api.running()&&!(api.replayViewer?.()&&path.toLowerCase().endsWith('.rpy')))throw Error('请先退出游戏再导入文件');
      if(!Array.isArray(message.bytes)||!message.bytes.length||message.bytes.length>64*1024*1024||message.bytes.some(byte=>!Number.isInteger(byte)||byte<0||byte>255))throw Error('文件大小或字节无效');
      const bytes=new Uint8Array(message.bytes);await importedFile(path,bytes,api);await api.store.write(path,bytes);await api.store.sync();return {};
    }
    case 'remove':{const path=savePath(message.path);if(api.running()&&!(api.replayViewer?.()&&path.toLowerCase().endsWith('.rpy')))throw Error('请先退出游戏再删除文件');await api.store.remove(path);await api.store.sync();return {};}
    case 'sync':await api.sync();return {};
    default:throw Error('未支持的操作：'+message.command);
    }
  };
}

export function installManagedShell(api,environment=window,{announceReady=true}={}){
  const epoch=Number(new URL(environment.location.href).searchParams.get('runtimeEpoch'));
  if(environment.parent===environment||!Number.isSafeInteger(epoch)||epoch<=0)throw Error('运行时导航身份无效');
  const emit=payload=>environment.parent.postMessage({protocol:'eagler-touhou/1',game:'th12',epoch,...payload},environment.location.origin);
  const command=managedCommandHandler(api);let queue=Promise.resolve(),announced=false;
  environment.addEventListener('message',event=>{
    const message=event.data;
    if(event.source!==environment.parent||event.origin!==environment.location.origin||message?.protocol!=='eagler-touhou/1'||message.game!=='th12'||message.epoch!==epoch||typeof message.command!=='string')return;
    if(['keyboard','keyboard-clear','touch-controls','touch-cancel','direct-touch'].includes(message.command)){
      Promise.resolve().then(()=>command(message)).then(result=>{if(typeof message.request==='string')emit({request:message.request,ok:true,...result});}).catch(error=>emit({event:'error',error:error.message}));return;
    }
    queue=queue.then(async()=>{try{if(!announced)throw Error('运行资源尚未准备');const result=await command(message);if(typeof message.request==='string')emit({request:message.request,ok:true,...result});}catch(error){if(typeof message.request==='string')emit({request:message.request,ok:false,error:error.message});else emit({event:'error',error:error.message});}});
  });
  emit.ready=()=>{if(announced)return;announced=true;emit({event:'ready',saveRoot:'/savesth12',roomPreview:true,roomReturn:true});};
  if(announceReady)emit.ready();return emit;
}
