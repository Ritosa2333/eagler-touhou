import {allowedPath} from './storage.mjs';
import {parseReplay} from './replay.mjs';

const boolOptions=['limitPresentationTo60','touchEnabled','doubleTapBombEnabled','alwaysHitbox','thpracEnabled','focusHitboxEnabled'];
const acceptedOptions=new Set([...boolOptions,'touchMovementMode','touchSensitivity','touchFocusMode','oggDecodeMode','replayPath','debugHarness']);
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
  if(value.touchEnabled||value.alwaysHitbox||value.thpracEnabled||value.focusHitboxEnabled)throw Error('此接入版本尚未完成所选触控、判定点增强或练习工具');
  if(value.replayPath!==undefined&&!savePath(value.replayPath).endsWith('.rpy'))throw Error('录像路径无效');
  if(value.debugHarness!==undefined&&value.debugHarness!==null&&typeof value.debugHarness!=='string')throw Error('诊断配置无效');
  return {...value,limitPresentationTo60:value.limitPresentationTo60??true};
}
export function savePath(value){
  if(typeof value!=='string')throw Error('存档路径无效');
  return allowedPath(value.startsWith('/savesth12/')?value:'/savesth12/'+value);
}
function importedFile(path,bytes){
  if(path.endsWith('.rpy')){parseReplay(bytes);return;}
  // The current profile codec preserves unknown retail bytes on local reads.
  // Do not claim a retail import succeeded while the game cannot consume it.
  let value;try{value=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(bytes));}catch{throw Error('原版成绩／配置文件编解码尚未接入');}
  if(value.schema!==(path.endsWith('th12.cfg')?'th12-web-config/1':'th12-web-score/1'))throw Error('存档格式无效');
}
export function managedCommandHandler(api){
  let configured=false;
  return async message=>{
    switch(message.command){
    case 'configure':{
      if(api.running())throw Error('游戏运行中不能重新配置');
      if(!['ogg','none'].includes(message.music))throw Error('星莲船未支持此音乐模式');
      if(message.language!==undefined&&!['ja',''].includes(message.language))throw Error('此版本未接入语言包');
      if(message.runtimePack)throw Error('此版本不使用独立运行资源包');
      const options=runtimeOptions(message.options),resources=[];
      if(message.music==='ogg'&&options.oggDecodeMode==='stream')throw Error('此接入版本尚未完成流式音乐，请选择完整 OGG 或无音乐');
      for(const key of ['resources','runtimeResources','sharedResources']){if(message[key]!==undefined&&!Array.isArray(message[key]))throw Error('附加资源列表无效');resources.push(...(message[key]??[]));}
      await api.resources(resources);await api.configure({options,music:message.music});configured=true;return {};
    }
    case 'resources':await api.resources(message.resources);return {};
    case 'launch':if(!configured)throw Error('请先配置游戏');if(api.running())throw Error('游戏已启动');await api.launch();return {};
    case 'keyboard':if(typeof message.code!=='string'||typeof message.down!=='boolean')throw Error('键盘输入无效');api.key(message.code,message.down,message);return {};
    case 'keyboard-clear':api.clearKeyboard();return {};
    case 'touch-cancel':api.cancelTouch();return {};
    case 'touch-controls':{
      if(typeof message.fireEnabled!=='boolean'||typeof message.focusEnabled!=='boolean'||!['bombSerial','escapeSerial'].every(key=>Number.isSafeInteger(message[key])&&message[key]>=0)||!['joystickX','joystickY'].every(key=>Number.isFinite(message[key])&&Math.abs(message[key])<=32767))throw Error('触控快照无效');
      // configure refuses enabled touch. Inactive control snapshots must still
      // be consumed without turning a disabled UI channel into a game error.
      return {};
    }
    case 'direct-touch':if(!['down','move','up'].includes(message.type)||!Number.isSafeInteger(message.id)||!Number.isFinite(message.x)||!Number.isFinite(message.y))throw Error('触控坐标无效');return {};
    case 'list':{
      await api.sync();const files=[];for(const path of await api.store.list()){const bytes=await api.store.read(savePath(path));files.push({path:savePath(path).slice('/savesth12/'.length),size:bytes?.byteLength??0});}return {files};
    }
    case 'read':{await api.sync();const bytes=await api.store.read(savePath(message.path));if(!bytes)throw Error('文件不存在');return {bytes:Array.from(bytes)};}
    case 'write':{
      if(api.running())throw Error('请先退出游戏再导入文件');
      if(!Array.isArray(message.bytes)||!message.bytes.length||message.bytes.length>64*1024*1024||message.bytes.some(byte=>!Number.isInteger(byte)||byte<0||byte>255))throw Error('文件大小或字节无效');
      const path=savePath(message.path),bytes=new Uint8Array(message.bytes);importedFile(path,bytes);await api.store.write(path,bytes);await api.store.sync();return {};
    }
    case 'remove':if(api.running())throw Error('请先退出游戏再删除文件');await api.store.remove(savePath(message.path));await api.store.sync();return {};
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
    queue=queue.then(async()=>{try{if(!announced)throw Error('运行资源尚未准备');const result=await command(message);if(typeof message.request==='string')emit({request:message.request,ok:true,...result});}catch(error){if(typeof message.request==='string')emit({request:message.request,ok:false,error:error.message});else emit({event:'error',message:error.message});}});
  });
  emit.ready=()=>{if(announced)return;announced=true;emit({event:'ready',saveRoot:'/savesth12'});};
  if(announceReady)emit.ready();return emit;
}
