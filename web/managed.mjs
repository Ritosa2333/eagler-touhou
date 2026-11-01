import {SAVE_ROOT,userPath,validateUserFile} from './storage.mjs';
import {displayOptions} from './display.mjs';
import {multiplayerOptions} from './multiplayer.mjs';
export const protocol='eagler-touhou/1';
const bools=['thpracEnabled','limitPresentationTo60','touchEnabled','doubleTapBombEnabled','alwaysHitbox','focusHitboxEnabled','replayViewer'];
const accepted=new Set([...bools,'touchMovementMode','touchSensitivity','touchFocusMode','oggDecodeMode','musicSource','replayPath','thpracLocale','debugHarness']);
const networkKeys=['th05MultiplayerDisplay','multiplayerLocalPlayerVisibility','netplayChallengeMode','netplayMode','netplayUrl','netplayPlayer','netplayPlayerCount','netplaySeed','netplayDifficulty','netplayInputDelay','netplayInputDelayAuto','netplayPredictionReserve','netplayAdonisMode','netplayPredictionLimit','netplaySpectator','netplaySpectatorId','netplaySpectatorCount','netplaySpectatorTimeline','netplayReplayAutoSave','netplayReplayNames','netplayIceServers','netplayPreferTurn','netplayAllowWebsocketFallback','netplayLoadouts'];
export function runtimeOptions(value={},multiplayer=false){
 if(!value||typeof value!=='object'||Array.isArray(value))throw Error('Invalid runtime options');
 for(const key of Object.keys(value))if(!accepted.has(key)&&!(multiplayer&&networkKeys.includes(key)))throw Error('Unsupported TH05 option: '+key);
 for(const key of bools)if(value[key]!==undefined&&typeof value[key]!=='boolean')throw Error('Invalid '+key);
 if(value.touchSensitivity!==undefined&&(!Number.isFinite(value.touchSensitivity)||value.touchSensitivity<100||value.touchSensitivity>300))throw Error('Invalid touch sensitivity');
 for(const [key,choices] of [['touchMovementMode',['touch','touch-unlimited','joystick','joystick-free']],['touchFocusMode',['two-finger','hold-button','toggle-button']],['oggDecodeMode',['stream','full']],['musicSource',['original','akyu']],['thpracLocale',['ja-JP','en-US','zh-CN']]])if(value[key]!==undefined&&!choices.includes(value[key]))throw Error('Invalid '+key);
 if(value.replayPath!==undefined&&!userPath(value.replayPath).endsWith('.rpy'))throw Error('Invalid Replay path');
 if(value.multiplayerLocalPlayerVisibility!==undefined&&typeof value.multiplayerLocalPlayerVisibility!=='boolean')throw Error('Invalid TH05 player visibility');
 if(value.th05MultiplayerDisplay!==undefined)displayOptions(value.th05MultiplayerDisplay);
 if(value.netplayMode!==undefined){if(!multiplayer)throw Error('TH05 multiplayer requires its own runtime');multiplayerOptions(value);}
 if(value.debugHarness!=null)throw Error('Diagnostic harness requires a diagnostic build');
 return {touchSensitivity:150,touchMovementMode:'touch',touchFocusMode:'two-finger',oggDecodeMode:'stream',...value,limitPresentationTo60:true};
}
export function commandHandler(api){let configured=false;const savedPath=value=>userPath(value,api.saveRoot??SAVE_ROOT);return async message=>{
 switch(message.command){
 case 'configure':{
  if(api.running())throw Error('Game is already running');
  if(!['ogg','none'].includes(message.music))throw Error('TH05 music mode must be ogg or none');
  const language=message.language??message.runtimePack?.language??'ja';if(!['ja','lang_zh-hans'].includes(language))throw Error('Unsupported TH05 language');
  const options=runtimeOptions(message.options,api.multiplayer===true),resources=[];
  for(const key of ['sharedResources','runtimeResources','resources']){if(message[key]!==undefined&&!Array.isArray(message[key]))throw Error('Invalid resource list');resources.push(...message[key]??[]);}
  await api.resources(resources);if(message.runtimePack)await api.languagePack(message.runtimePack,language);
  await api.configure({options,music:message.music,language});configured=true;return {};
 }
 case 'resources':await api.resources(message.resources);return {};
 case 'prepare-room':if(!configured||!api.multiplayer||api.running())throw Error('Invalid room preview phase');return api.prepareRoom(message);
 case 'return-room':if(!api.multiplayer)throw Error('Not a TH05 multiplayer runtime');return api.returnRoom(message);
 case 'launch':if(!configured||api.running())throw Error('Invalid launch phase');await api.launch(message);return {};
 case 'keyboard':if(typeof message.code!=='string'||typeof message.down!=='boolean')throw Error('Invalid keyboard transition');api.key(message);return {};
 case 'keyboard-clear':api.clearKeyboard();return {};
 case 'touch-cancel':api.cancelTouch();return {};
 case 'touch-controls':{
  for(const key of ['thpracMobilePanelsEnabled','thpracMobilePanelsVisible'])if(message[key]!==undefined&&typeof message[key]!=='boolean')throw Error('Invalid practice panel controls');
  if(typeof message.fireEnabled!=='boolean'||typeof message.focusEnabled!=='boolean'||!['bombSerial','escapeSerial'].every(k=>Number.isSafeInteger(message[k])&&message[k]>=0)||!['joystickX','joystickY'].every(k=>Number.isFinite(message[k])&&Math.abs(message[k])<=32767))throw Error('Invalid touch controls');
  if(message.touchSensitivity!==undefined&&(!Number.isFinite(message.touchSensitivity)||message.touchSensitivity<100||message.touchSensitivity>300))throw Error('Invalid touch sensitivity');api.touch(message);return {};
 }
 case 'direct-touch':if(!['down','move','up'].includes(message.type)||!Number.isSafeInteger(message.id)||!Number.isFinite(message.x)||!Number.isFinite(message.y))throw Error('Invalid touch point');api.pointer(message);return {};
 case 'thprac-mouse':if(!['down','move','up'].includes(message.type)||!Number.isFinite(message.x)||!Number.isFinite(message.y))throw Error('Invalid practice pointer');api.practicePointer(message);return {};
 case 'list':await api.sync();return {files:api.store.list()};
 case 'read':await api.sync();return {bytes:Array.from(api.store.read(savedPath(message.path)))};
 case 'write':{
  if(api.running()&&!api.viewer())throw Error('Exit the game before importing files');const path=savedPath(message.path);
  if(!Array.isArray(message.bytes)||message.bytes.some(n=>!Number.isInteger(n)||n<0||n>255))throw Error('Invalid user bytes');
  const bytes=new Uint8Array(message.bytes);validateUserFile(path,bytes);api.store.write(path,bytes);await api.store.sync();return {};
 }
 case 'remove':if(api.running()&&!api.viewer())throw Error('Exit the game before removing files');api.store.remove(savedPath(message.path));await api.store.sync();return {};
 case 'sync':await api.sync();return {};
 case 'playback':return api.playback(message);
 default:throw Error('Unsupported TH05 command: '+message.command);
 }
};}
export function installManagedShell(api,environment){
 const epoch=Number(new URL(environment.location.href).searchParams.get('runtimeEpoch'));
 if(environment.parent===environment||!Number.isSafeInteger(epoch)||epoch<=0)throw Error('Launch TH05 from Eagler Touhou');
 const emit=(event,payload={})=>environment.parent.postMessage({protocol,game:'th05',epoch,event,...payload},environment.location.origin);
 const handle=commandHandler(api);let queue=Promise.resolve(),ready=false,closed=false;
 const listener=event=>{const message=event.data;
  if(closed||event.source!==environment.parent||event.origin!==environment.location.origin||message?.protocol!==protocol||message.game!=='th05'||message.epoch!==epoch||typeof message.command!=='string')return;
  queue=queue.then(async()=>{try{if(!ready)throw Error('Runtime is not ready');const result=await handle(message);if(typeof message.request==='string')environment.parent.postMessage({protocol,game:'th05',epoch,request:message.request,ok:true,...result},environment.location.origin);}catch(error){if(typeof message.request==='string')environment.parent.postMessage({protocol,game:'th05',epoch,request:message.request,ok:false,error:error.message},environment.location.origin);else emit('error',{message:error.message});}});
 };
 environment.addEventListener('message',listener);
 return {epoch,emit,ready(){ready=true;emit('ready',{saveRoot:api.multiplayer?'/savesth05mp':'/savesth05',...(api.multiplayer?{roomPreview:true,roomReturn:true}:{})});},close(){closed=true;environment.removeEventListener('message',listener);},idle:()=>queue};
}
