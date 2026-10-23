import {createAdonisCalibration} from './adonis-calibration.mjs';
export function multiplayerBuildWords(manifest){
 const digest=manifest?.execution?.sha256;
 if(manifest?.game!=='th15'||manifest?.product!=='th15mp'||manifest?.variant!=='multiplayer'||manifest?.features?.multiplayer!==true||!/^[a-f0-9]{64}$/i.test(digest||''))throw Error('TH15 多人 Runtime 身份无效');
 return Array.from({length:4},(_,i)=>parseInt(digest.slice(i*8,i*8+8),16)>>>0);
}
export function validateMultiplayerOptions(options={}){
 if(options.thpracEnabled)throw Error('TH15 多人模式不支持 THPrac');
 if(options.replayViewer===true){if(options.netplayMode==='lan'||options.netplaySpectator)throw Error('录像与联机不能同时启动');return {replay:true};}
 if(options.netplayMode!=='lan')throw Error('请从多人房间启动');
 const url=new URL(options.netplayUrl),room=url.searchParams.get('room'),run=url.searchParams.get('run');
 const count=options.netplayPlayerCount,spectator=options.netplaySpectator===true,seat=spectator?0:options.netplayPlayer;
 const delay=options.netplayInputDelay??0,automatic=options.netplayInputDelayAuto??false,reserve=options.netplayPredictionReserve??2,mode=options.netplayAdonisMode;
 if(!['ws:','wss:'].includes(url.protocol)||url.username||url.password||url.hash||!room?.startsWith('th15mp-')||!run||![2,3].includes(count)||!Number.isInteger(seat)||seat<0||seat>=count||
 !Number.isInteger(options.netplayDifficulty)||options.netplayDifficulty<0||options.netplayDifficulty>4||!Number.isInteger(options.netplaySeed)||options.netplaySeed<0||options.netplaySeed>65535||
 !Number.isInteger(delay)||delay<0||delay>9||typeof automatic!=='boolean'||(automatic&&delay)||![0,1,2].includes(mode)||(mode===0&&(delay||automatic))||!Number.isInteger(reserve)||reserve<1||reserve>2||
 (options.netplayPredictionLimit!==undefined&&options.netplayPredictionLimit!==(mode===1?0:12)))throw Error('TH15 多人配置无效');
 const loadouts=options.netplayLoadouts;
 if(!Array.isArray(loadouts)||loadouts.length!==count||loadouts.some(v=>!v||!Number.isInteger(v.character)||v.character<0||v.character>3||v.shot!==0))throw Error('TH15 多人机体配置无效');
 if(spectator&&(typeof options.netplaySpectatorId!=='string'||!options.netplaySpectatorId))throw Error('观战身份缺失');
 if(options.netplayChallengeMode!==undefined&&typeof options.netplayChallengeMode!=='boolean')throw Error('挑战模式配置无效');
 const chapterThreshold=options.netplayChapterThreshold??(count===3?1500000:1250000);
 if(!Number.isInteger(chapterThreshold)||chapterThreshold<500000||chapterThreshold>5000000||chapterThreshold%250000)throw Error("奖残所需得分无效");
 return {url,room,run,count,seat,spectator,delay,automatic,reserve,mode,loadouts,chapterThreshold,replay:false};
}
export async function multiplayerSetupWords(options,buildWords,cryptoProvider=globalThis.crypto){
 const c=validateMultiplayerOptions(options);if(c.replay||!Array.isArray(buildWords)||buildWords.length!==4||!buildWords.some(Boolean))throw Error('构建身份或连接配置无效');
 const digest=new DataView(await cryptoProvider.subtle.digest('SHA-256',new TextEncoder().encode('th15mp:'+c.url.origin+c.url.pathname+':'+c.room+':'+c.run)));
 const words=[7,c.count,c.seat,options.netplayDifficulty,options.netplaySeed,digest.getUint32(0,true),digest.getUint32(4,true)||1,c.delay];
 for(let i=0;i<3;i++){const p=c.loadouts[i]||{character:0,shot:0};words.push(p.character,p.shot);}
 words.push(c.mode,+c.automatic,c.reserve,...buildWords,+!!options.netplayChallengeMode,c.chapterThreshold);return {config:c,words};
}
export function createMultiplayerBridge({core,getOptions,manifest,emit,onError,onExit,target=globalThis}){
 const buildWords=multiplayerBuildWords(manifest);let active=false,calibration=null;
 const error=()=>{const p=core._th15_mp_error();return new TextDecoder().decode(core.HEAPU8.subarray(p,core.HEAPU8.indexOf(0,p)));};
 const bytes=(value,call)=>{const p=core._malloc(value.length);if(!p)throw Error('多人内存不足');try{core.HEAPU8.set(value,p);return call(p,value.length);}finally{core._free(p);}};
 const string=(value,call)=>bytes(new TextEncoder().encode(value+'\0'),call);
 const fail=e=>{target.__eaglerNetplayFailed=true;target.__eaglerNetplayError=e?.message||String(e);calibration?.stop();core._th15_loop_stop();onError(e);};
 const memory={get buffer(){return core.HEAPU8.buffer;}};
 function startCalibration(){
  calibration=createAdonisCalibration({core:{memory,multiplayer_calibration_status:()=>core._th15_mp_calibration_status(),multiplayer_network_poll:()=>core._th15_mp_pump()},getApp:()=>active?1:0,getOptions,emit,game:'th15',build:manifest.execution.sha256,onError:fail,pause:()=>{if(core._th15_phase()===0||getOptions().netplaySpectator)return;core._th15_loop_stop();onExit();},target});
  calibration.start();
 }
 async function launch(){
  const options=getOptions(),checked=validateMultiplayerOptions(options);
  if(checked.replay)return false;
  const {config,words}=await multiplayerSetupWords(options,buildWords);const data=new Uint32Array(words);
  if(!bytes(new Uint8Array(data.buffer),p=>core._th15_mp_configure(p,words.length)))throw Error(error());
  if(config.mode!==1&&(manifest?.netplay?.stateBackend!=='write-journal-32'||manifest.netplay.instrumentation!==0x52424a31||manifest.netplay.predictionLimit!==12))throw Error('TH15 回滚 Runtime 状态后端缺失');
  const connected=config.spectator?string(options.netplayUrl,url=>string(options.netplaySpectatorId,id=>core._th15_mp_spectator_connect(url,id))):string(options.netplayUrl,url=>core._th15_mp_connect(url));
  if(!connected||!core._th15_mp_start())throw Error(error());
  active=true;target.__eaglerNetplayLanActive=true;target.__eaglerNetplaySpectator=config.spectator;target.__eaglerNetplayFailed=false;
  target.__eaglerNetplayInputDelayFrames=config.delay;target.__eaglerNetplayAdonisMode=config.mode;startCalibration();return true;
 }
 function frame(){if(!active)return;if(core._th15_phase()===0)calibration?.stop();const s=Array.from(core.HEAPU32.subarray(core._th15_mp_status()>>>2,(core._th15_mp_status()>>>2)+24));target.__eaglerNetplayLanConfirmed=s[4];target.__eaglerNetplayLanFrame=s[3];if(typeof core._th15_mp_rollback_status==='function'){const p=core._th15_mp_rollback_status()>>>2,r=Array.from(core.HEAPU32.subarray(p,p+20));target.__eaglerNetplayLanRollback=r[8];target.__eaglerNetplayLanResimulated=r[9];target.__eaglerNetplayDiagnostics={frame:s[3],confirmed:s[4],inputDelay:s[7],prediction:s[8],correcting:!!r[1],capture:r[3],committed:r[4],snapshots:r[5],undoKiB:r[13],peakDirtyBlocks:r[7],heapKiB:r[15],barriers:r[10]};}calibration?.pump();}
 function save(){if(active&&getOptions().replayViewer!==true&&!getOptions().netplaySpectator&&!core._th15_mp_replay_save())throw Error(error());}
 function close(){calibration?.stop();core._th15_mp_close();active=false;target.__eaglerNetplayLanActive=false;}
 function openReplay(path,stage){
  close();core._th15_loop_stop();if(!core._th15_initialize())throw Error(error());
  if(!bytes(core.FS.readFile('/save/'+path),(p,n)=>bytes(new Uint8Array(new Uint32Array(buildWords).buffer),b=>core._th15_mp_replay_load(p,n,b)))||!core._th15_mp_start()||!core._eagler_replay_select_stage(stage))throw Error(error());
  active=true;target.__eaglerNetplaySpectator=true;
 }
 return {launch,frame,save,close,openReplay};
}
