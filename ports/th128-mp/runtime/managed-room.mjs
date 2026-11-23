import {makeCoopRules,validFreezeMotivationMultiplier} from './coop-rules.mjs';
import {cirnoPaletteSet} from './coop-cirno-palettes.mjs';
import {mountLauncherSession} from '../../th128/site/runtime/launcher-session.mjs';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';

const token=/^[A-Za-z0-9_-]{8,64}$/,digest=/^[a-f0-9]{64}$/;
const int=(n,min,max)=>Number.isSafeInteger(n)&&n>=min&&n<=max;
const invalid=message=>Object.assign(new Error(message),{errno:'invalid-field'});

// Adapter for the author Launcher gameplay URL and configure.options, not a
// competing lobby protocol. The first native room owns Normal / A1-3 only.
export function readTh128RoomOptions(options,identity){
  const cooperative=identity?.gameplay==='same-stage-coop',seatCount=options?.netplayPlayerCount;
  if(options?.netplayMode!=='lan'||(cooperative?!int(seatCount,2,3):seatCount!==2)||typeof options.netplaySpectator!=='boolean')
    throw invalid('A TH128 room must declare LAN and its supported seats');
  if(typeof options.netplayUrl!=='string'||options.netplayUrl.length>2048)throw invalid('Invalid TH128 room URL');
  const endpoint=new URL(options.netplayUrl),query=endpoint.searchParams;
  if(!['ws:','wss:'].includes(endpoint.protocol)||endpoint.username||endpoint.password||endpoint.hash||
     ['lobby','directory','signal','diagnostic'].some(key=>query.has(key)))throw invalid('Invalid TH128 room gameplay endpoint');
  const one=(key,optional=false)=>{
    const values=query.getAll(key);
    if(values.length>1)throw invalid(`Invalid duplicate TH128 room ${key}`);
    if(!values.length&&!optional)throw invalid(`TH128 room ${key} is missing`);
    return values[0]??null;
  };
  const room=one('room'),code=/^th128mp-(\d{4,8})$/.exec(room)?.[1],runText=one('run'),run=Number(runText);
  if(!code||!/^\d{1,10}$/.test(runText)||!int(run,1,2147483647)||!token.test(one('member')))throw invalid('Invalid TH128 room membership');
  const generation=one('runtimeGeneration',true);
  if((cooperative&&generation===null)||(generation!==null&&(!digest.test(generation)||identity?.generation!==generation)))throw invalid('TH128 Runtime generation differs from the room');
  const players=one('players',true);
  if(players!==null&&players!==String(seatCount))throw invalid('TH128 room seat count differs from the URL');
  endpoint.searchParams.set('players',String(seatCount));
  const spectator=options.netplaySpectator,spectatorId=one('spectator',true),seatId=one('player',true),seat=spectator?null:options.netplayPlayer;
  if(spectator){
    if(seatId!==null||!token.test(spectatorId??'')||spectatorId!==options.netplaySpectatorId||
       options.netplayPlayer!==0&&options.netplayPlayer!==null)throw invalid('TH128 spectator ownership differs from the room');
  }else if(!int(seat,0,seatCount-1)||seatId!==String(seat)||spectatorId!==null||options.netplaySpectatorId!=='')throw invalid('TH128 player seat differs from the room');
  const seed=Number(code)&0xffff;
  if(options.netplaySeed!==seed)throw invalid('TH128 seed differs from the author room code');
  if(cooperative?!int(options.netplayDifficulty,0,4):options.netplayDifficulty!==1)throw invalid('Only Normal difficulty has a managed A1-3 engine');
  const loadouts=options.netplayLoadouts;
  if(!Array.isArray(loadouts)||loadouts.length!==seatCount||[...loadouts].some(item=>item?.character!==0||item?.shot!==0))
    throw invalid('TH128 room supports only original Cirno');
  if((cooperative?options.netplayChallengeMode!==undefined&&typeof options.netplayChallengeMode!=='boolean':options.netplayChallengeMode===true)||options.netplayChallengeKeepBullets===true||(options.thpracEnabled!==undefined&&options.thpracEnabled!==false))
    throw invalid('TH128 multiplayer does not accept practice or challenge overrides');
  if(options.netplayRandomCirnoPalette!==undefined&&(!cooperative||typeof options.netplayRandomCirnoPalette!=='boolean'))
    throw invalid('Invalid Cirno palette room option');
  if(Object.hasOwn(options,'netplayFreezeMotivationArea')||
     options.netplayFreezeMotivationMultiplier!==undefined&&(!cooperative||!validFreezeMotivationMultiplier(options.netplayFreezeMotivationMultiplier)))
    throw invalid('Invalid freeze motivation room option');
  const mode=options.netplayAdonisMode??0,inputDelay=options.netplayInputDelay??0,limit=options.netplayPredictionLimit??2;
  if(![0,1].includes(mode)||(mode===0&&inputDelay!==0)||!int(inputDelay,0,8)||!int(limit,0,2)||options.netplayInputDelayAuto===true)
    throw invalid('Unsupported TH128 input timing');
  const rules=cooperative?makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:`${room}-${run}`,epoch:1,players:seatCount,seed,
    difficulty:options.netplayDifficulty,route:options.netplayDifficulty===4?'EX':'A',challengeMode:options.netplayChallengeMode??false,
    ...(options.netplayRandomCirnoPalette===true?{randomCirnoPalette:true,cirnoPaletteSet}:{}),
    freezeMotivationMultiplier:options.netplayFreezeMotivationMultiplier??1,
    inputDelay,rollbackWindow:mode===0?limit:0,coreSHA256:identity.coreSHA256,resourceSHA256:identity.resourceSHA256}):createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId:`${room}-${run}`,epoch:1,mode:'parallel-stage',route:'A1-3',seed,
    inputDelay,rollbackWindow:mode===0?limit:0,coreSHA256:identity?.coreSHA256,resourceSHA256:identity?.resourceSHA256});
  const ice=options.netplayIceServers??[],count=options.netplaySpectatorCount??0;
  if(!Array.isArray(ice)||ice.length>32||!int(count,0,128))throw invalid('Invalid TH128 room transport settings');
  const choice=options.netplayRouteChoice;
  if(choice!==undefined&&!['auto','direct','swarm','turn'].includes(choice))throw invalid('Unsupported TH128 route preference');
  const transport=Object.freeze({netplayIceServers:structuredClone(ice),netplaySpectatorCount:count,
    netplayAllowWebsocketFallback:options.netplayAllowWebsocketFallback===true,netplayPreferTurn:options.netplayPreferTurn===true,
    ...(choice===undefined?{}:{netplayRouteChoice:choice})});
  return Object.freeze({role:spectator?'spectator':'player',seat,spectatorId,room,run,relayUrl:endpoint.href,rules,transport});
}

// Keep this bridge separate from the normal page. Neither configuring nor
// reporting a title frame creates a network session; launch owns that boundary.
export function mountManagedTh128Room({window,document,location,identity,verifyMounts,presentTitle,launchRoom,disposeResources,
  configureLocal=()=>{}}){
  for(const method of [verifyMounts,presentTitle,launchRoom,disposeResources,configureLocal])
    if(typeof method!=='function')throw new TypeError('Incomplete TH128 multiplayer Launcher owner');
  const build=Object.freeze({...identity});
  let session,room=null,active=null,localTitleConfig=null;
  async function closeRun(){
    const run=active;if(!run)return;active=null;run.controller.abort();await run.instance?.close();
  }
  function player(){
    if(!active?.instance||active.room.role!=='player')throw Object.assign(new Error('No local player is bound'),{errno:'invalid-state'});
    return active.instance;
  }
  session=mountLauncherSession({window,document,location,room:{
    prepare:({signal})=>presentTitle({config:localTitleConfig,signal}),
    async restore({signal}){await closeRun();room=null;signal.throwIfAborted();return presentTitle({config:localTitleConfig,signal});},
  },hooks:{
    async configure(config){
      if(config.music!=='ogg'&&config.music!=='none')throw invalid('TH128 multiplayer supports OGG or no music');
      if(config.runtimePack!=null)throw invalid('TH128 multiplayer resources may not supply a Runtime');
      const owned=config.options?.netplayMode===undefined?null:readTh128RoomOptions(config.options,build);
      await verifyMounts([...(config.resources??[]),...(config.runtimeResources??[]),...(config.sharedResources??[])],session.signal);
      session.signal.throwIfAborted();await configureLocal(config,session.signal);session.signal.throwIfAborted();room=owned;
      if(!owned)localTitleConfig=config;
    },
    resources:rows=>verifyMounts(rows,session.signal),
    async launch({config,launchSerial,signal}){
      if(!room||!localTitleConfig||!int(launchSerial,1,Number.MAX_SAFE_INTEGER)||active)throw invalid('No owned TH128 multiplayer room launch serial');
      const run={room,controller:new AbortController(),instance:null};active=run;
      const runSignal=AbortSignal.any([signal,run.controller.signal]);
      const present=(actual,path)=>active===run&&!runSignal.aborted&&
        (run.room.role==='spectator'?path==='spectator-relay':path==='rtc'||path==='relay')&&session.notePresented(actual,path);
      try{
        const instance=await launchRoom({room:run.room,config,launchSerial,signal:runSignal,present});
        if(typeof instance?.close!=='function')throw Error('TH128 multiplayer launch returned no close owner');
        if(active!==run||runSignal.aborted){await instance.close();runSignal.throwIfAborted();throw Error('TH128 multiplayer launch was retired');}
        run.instance=instance;
      }catch(error){if(active===run)await closeRun();throw error;}
    },
    keyboard:value=>{
      if(active?.room.role==='spectator'&&active.instance?.key?.(value)===true)return true;
      return player().key(value);
    },keyboardClear:()=>active?.instance?.clearInput?.(),
    directTouch:value=>player().directTouch(value),touchControls:value=>player().touchControls(value),touchCancel:()=>active?.instance?.touchCancel?.(),
    spectatorPlayback(value){
      if(active?.room.role!=='spectator'||typeof active.instance?.spectatorPlayback!=='function')
        throw Object.assign(new Error('No owned TH128 spectator playback'),{errno:'invalid-state'});
      return active.instance.spectatorPlayback(value);
    },
    files({command}){
      if(command!=='sync')throw Object.assign(new Error('TH128 multiplayer does not own single-player files'),{errno:'unsupported-command'});
      // The managed match is ephemeral; its normal-game recorder and save files
      // are never opened. A room return must not request a Package Store write.
      return {changed:false};
    },
    async exit(){try{await closeRun();}finally{await disposeResources();}},
  }});
  return session;
}
