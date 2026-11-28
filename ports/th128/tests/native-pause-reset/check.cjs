'use strict';const fs=require('node:fs'),path=require('node:path'),crypto=require('node:crypto'),assert=require('node:assert/strict'),{pathToFileURL}=require('node:url');const r=JSON.parse(fs.readFileSync(process.argv[2],'utf8'));
const sha=b=>crypto.createHash('sha256').update(b).digest('hex');let strictAudioOwner=null,hostModule=null,strictReceiptDB=null;let native,queue,music,audio,events=[],gameTicks=0,menuTicks=0,WorldStarts=0,Factories=0,soundACKs=0,musicACKs=0,continues=0,lastHeld=0,clockOrigin=0,lastClock=0,stage,trace=[],inputDigest=crypto.createHash('sha256'),opaqueCarries=[];
function durable(name,value){const b=Buffer.isBuffer(value)?value:Buffer.from(JSON.stringify(value,null,2)+'\n'),p=path.join(r.output,name);const fd=fs.openSync(p,'wx');try{fs.writeFileSync(fd,b);fs.fsyncSync(fd);}finally{fs.closeSync(fd);}}
function state(){return JSON.parse(native.UTF8ToString(native._th128_world_state()));}function error(){return native.UTF8ToString(native._th128_world_error());}
function need(value,label){if(value!==1)throw Error(label+': '+error());}
function memory(bytes,call){const p=native._malloc(bytes.length);assert(p>0);try{native.HEAPU8.set(bytes,p);return call(p,bytes.length);}finally{native._free(p);}}
function raw(name){const b=fs.readFileSync(path.join(r.assets,'raw',name)),row=r.manifest.files.find(x=>x.name===name);assert(row&&row.bytes===b.length&&row.sha256===sha(b),'originalasset:'+name);return b;}
function dataCall(name,bytes,...prefix){need(memory(bytes,(p,n)=>native[name](...prefix,p,n)),name);}
function bankProfiles(){const output=new Map();for(const file of r.profileJSON){const record=JSON.parse(fs.readFileSync(file,'utf8'));for(const row of record.profiles??[record])if(Number.isInteger(row.type)&&row.hex){const b=Buffer.from(row.hex,'hex');assert.equal(b.length,288);assert.equal(sha(b),row.sha256);output.set(row.type,b);}}
 for(const type of [18,20])output.set(type,fs.readFileSync(path.join(r.web,'local/runtime/bullet-profile-'+type+'.bin')));const type9=Buffer.from(r.type9.hex,'hex');assert.equal(sha(type9),r.type9.sha256);output.set(9,type9);return output;}
function decodedWave(arrayBuffer){const b=Buffer.from(arrayBuffer);assert.equal(b.toString('ascii',0,4),'RIFF');assert.equal(b.toString('ascii',8,12),'WAVE');let channels,rate,bits,block,dataBytes;for(let cursor=12;cursor+8<=b.length;){const id=b.toString('ascii',cursor,cursor+4),n=b.readUInt32LE(cursor+4),start=cursor+8;assert(start+n<=b.length);if(id==='fmt '){channels=b.readUInt16LE(start+2);rate=b.readUInt32LE(start+4);block=b.readUInt16LE(start+12);bits=b.readUInt16LE(start+14);}if(id==='data')dataBytes=n;cursor=start+n+(n&1);}assert(channels&&rate&&block&&dataBytes>=0);return{numberOfChannels:channels,sampleRate:rate,length:dataBytes/block,duration:dataBytes/block/rate,getChannelData(){throw Error('Synthetic decode never attests physicalPCMsamples');}};}
async function prepareAudio(){const sound=await import(pathToFileURL(r.sound).href),{webAudio,ReceiptsDatabase}=require('./audio-fixture.cjs');hostModule=await import(pathToFileURL(r.browserHost).href);strictReceiptDB=new ReceiptsDatabase(events);audio=webAudio(events);audio.decodeAudioData=async b=>decodedWave(b);audio.state='running';audio.resume=async()=>{audio.state='running';};audio.close=async()=>{audio.state='closed';};audio.suspend=async()=>{audio.state='suspended';};const gain=audio.createGain();queue=new sound.GameSoundQueue(audio,gain);music=new sound.StageMusic(audio,gain);
 globalThis.fetch=async url=>{const p=path.resolve(r.assets,String(url));if(!p.startsWith(path.resolve(r.assets)+path.sep)||!r.resources.some(row=>row.path===p))throw Error('Scoped asset notpinned:'+p);const bytes=fs.readFileSync(p);return{ok:true,async arrayBuffer(){return bytes.buffer.slice(bytes.byteOffset,bytes.byteOffset+bytes.byteLength);}};};
 await queue.load(r.soundBindings,r.assets,(base,file)=>file);return sound;}
async function configure(stageId,carry=null){stage=r.stages.find(s=>s.id===stageId);assert(stage,'actual native request outside admitted A1 route');native._th128_world_reset();lastHeld=0;const parsed=JSON.parse(fs.readFileSync(path.join(r.assets,'ecl',stage.ecl.replace('.ecl','.json')),'utf8'));
 for(const name of [stage.ecl,...parsed.includes.ECLI]){const label=Buffer.from(name),bytes=raw(name);memory(label,(np,nn)=>memory(bytes,(bp,bn)=>need(native._th128_world_load(np,nn,bp,bn),'world_load:'+name)));}
 const label=Buffer.from(stage.ecl);memory(label,(p,n)=>need(native._th128_world_start(p,n,r.rank,r.seed),'world_start'));++WorldStarts;
 if(carry){memory(label,(p,n)=>need(native._th128_world_bind_stage_carry_target(carry.token,p,n,r.rank),'carry_bind_actualtarget'));need(native._th128_world_restore_stage_carry_random(carry.token,stageId,carry.progress,r.rank),'carry_restore_actualRNG');}
 const logo=raw(stage.logo),logoLabel=Buffer.from(stage.logo);memory(logoLabel,(np,nn)=>memory(logo,(bp,bn)=>need(native._th128_world_load_stage_intro(stage.id,np,nn,bp,bn),'original_stage_intro')));
 for(let index=0;index<parsed.includes.ANIM.length;++index)dataCall('_th128_world_load_enemy_anm',raw(parsed.includes.ANIM[index]),index+1);
 dataCall('_th128_world_enable_geometry526',raw('text.anm'));
 const p8=fs.readFileSync(path.join(r.web,'local/runtime/bullet-profile-8.bin')),bullet=raw('bullet.anm');memory(p8,(pp,pn)=>memory(bullet,(bp,bn)=>need(native._th128_world_enable_bullets(pp,pn,bp,bn,0,400),'original_bullets')));
 const shot=raw('pl00a.sht'),player=raw('pl00.anm'),front=raw('front.anm'),text=raw('text.anm');dataCall('_th128_world_enable_bullet_updates',shot);need(native._th128_world_enable_graze(0,10000,0x1234),'graze');
 memory(player,(p,n)=>need(native._th128_world_enable_player_context(p,n,1,0,120,0,0,0,0,0,0),'original_player_context'));need(native._th128_world_enable_player_motion(0,51200),'original_player_motion');
 const msg=raw(stage.message),boss=raw(parsed.includes.ANIM[2]);memory(msg,(mp,mn)=>memory(text,(tp,tn)=>memory(player,(pp,pn)=>memory(boss,(bp,bn)=>need(native._th128_world_enable_messages(mp,mn,tp,tn,pp,pn,bp,bn),'original_messages')))));dataCall('_th128_world_enable_message_front',front);
 if(stageId===1)memory(front,(p,n)=>need(native._th128_world_enable_route_choice(p,n,stageId,0,0),'original_routechoice'));
 if(r.ordinaryTransitionStages.includes(stageId))memory(front,(p,n)=>need(native._th128_world_enable_stage_transition(p,n,stageId,carry?.progress??0,0,0,1,0),'ordinary_transition'));
 for(const [type,bytes]of bankProfiles())dataCall('_th128_world_add_bullet_profile',bytes,type);
 memory(shot,(sp,sn)=>memory(player,(pp,pn)=>memory(Buffer.alloc(8000),(fp)=>need(native._th128_world_enable_player_shots(sp,sn,pp,pn,fp,2000,0,r.rank,0,0,0,0),'original_player_shots'))));
 need(native._th128_world_enable_shot_hits(20000,0,0,0,0),'original_shot_hits');need(native._th128_world_enable_rapid_shot(-1),'original_rapid');need(native._th128_world_enable_player_death(-1),'original_player_death');dataCall('_th128_world_enable_perfect_hud',front);dataCall('_th128_world_pause_load_manual',raw('help.anm'));
 memory(Buffer.alloc(32),p=>need(native._th128_world_enable_player_respawn(1,0,p,8),'original_respawn'));dataCall('_th128_world_load_score_font',raw('ascii.anm'));
 const inputs=[bullet,boss,text,raw('ascii.anm'),raw('card.anm'),player],allocated=[];try{for(const b of inputs){const p=native._malloc(b.length);native.HEAPU8.set(b,p);allocated.push(p);}need(native._th128_world_enable_spells(...inputs.flatMap((b,i)=>[allocated[i],b.length])),'original_spells');}finally{allocated.forEach(p=>native._free(p));}
 need(native._th128_world_enable_head_power(),'original_head_power');const masks=JSON.parse(fs.readFileSync(path.join(r.assets,'animations/ice-masks.json'),'utf8')),maskBytes=Buffer.concat(masks.profiles.map(row=>{const b=Buffer.from(row.hex,'hex');assert.equal(sha(b),row.sha256);return b;}));memory(raw('ice.anm'),(ip,in_)=>memory(maskBytes,(mp,mn)=>need(native._th128_world_enable_z_fire(ip,in_,mp,mn),'original_z_fire')));need(native._th128_world_enable_focus_indicator(),'original_focus');
 if(carry){need(native._th128_world_restore_stage_carry(carry.token),'carry_restore_actualScalars');need(native._th128_world_acknowledge_stage_transition(carry.token),'carry_exactACK');}else need(native._th128_world_records_restore(0,0),'fresh_isolated_native_record_without_user_store');
 const now=performance.now()/1000;if(!clockOrigin)clockOrigin=now;lastClock=now;need(native._th128_world_replay_begin(carry?65536:r.seed,now),'real_run_replay_begin');
 const initial=state();assert.equal(initial.frame,0);assert.equal(initial.debug.runTainted,false);assert.equal(initial.debugTerminal.configured,false);assert.equal(initial.normalRecord.started,true);assert.equal(initial.stageControl.current,7);
 if(stageId===3)assert.equal(initial.normalTerminal.configured,true,'ACTUAL_NATIVE_TERMINAL_RESOURCES_MUST_BE_BOUND_BY_PRODUCTION_CARRY_CALLER');
 await music.load(r.manifest.music,r.assets,(base,file)=>file,{tracks:stage.music});music.start();trace.push({kind:'actualWorldConfigured',stage:stageId,generation:initial.flow.worldGeneration,normalRecord:initial.normalRecord,stageControl:initial.stageControl});}
async function drain(sound){const actual=state(),identity=hostModule.ordinaryWorldAudioIdentity(actual);if(identity){if(!strictAudioOwner)strictAudioOwner=new hostModule.OrdinaryWorldAudioConsumer({module:native,audio,soundQueue:queue,legacyMusic:music,receipts:new hostModule.OwnedEndingReceipts(strictReceiptDB),isCurrent:()=>true,presentAudio:true});await strictAudioOwner.sound();await strictAudioOwner.music();return;}if(native._th128_world_sound_count()){const receipt=await sound.consumeMenuSoundWindow(native,queue,{async commit(payload){const body=JSON.stringify(payload);return{payload:body,digest:sha(Buffer.from(body)),strictReadback:true,syntheticStorage:true};}},{present:true});if(receipt)++soundACKs;}
 const control=JSON.parse(native.UTF8ToString(native._th128_world_music_control_state()));if(control.serial&&!control.acknowledged){assert.equal(control.operation,5);if(control.immediate)music.fadeOut(0);else music.fadeOutFrames(control.durationFrames);need(native._th128_world_acknowledge_music_control(control.serial),'native_music_consumed_syntheticAudioACK');++musicACKs;}
 const message=JSON.parse(native.UTF8ToString(native._th128_world_message_state()));if(message.musicSerial&&message.musicSerial!==drain.serial){music.switchTo(stage.music[1]);drain.serial=message.musicSerial;}}

let controller=null,ui=null,actualNativeCalls=[],heldSessions=[],rows=[];
const limits={worldTicks:6,menuTicks:220};
function pauseState(){return JSON.parse(native.UTF8ToString(native._th128_world_pause_state()));}
function exact(file,startMarker,endMarker){const s=fs.readFileSync(file,'utf8'),a=s.indexOf(startMarker),b=s.indexOf(endMarker,a);assert(a>=0&&b>a);assert.equal(s.indexOf(startMarker,a+1),-1);return s.slice(a,b).trimEnd();}
function barrier(){let release,mark;const held=new Promise(resolve=>{release=resolve;}),entered=new Promise(resolve=>{mark=resolve;});return{held,entered,release,mark,arrived:false};}
function check(name,work){work();rows.push({name,passed:true});}
async function bounded(p,label){let timer;try{return await Promise.race([p,new Promise((_,reject)=>{timer=setTimeout(()=>reject(Error('Bounded wait expired: '+label)),5000);})]);}finally{clearTimeout(timer);}}
function makeConsumer(sound){
 const {webAudio}=require('./audio-fixture.cjs');
 const instrumented=new Proxy(native,{get(target,key){
   const value=Reflect.get(target,key);if(typeof value!=='function')return value;
   return (...args)=>{if(String(key).startsWith('_th128_'))actualNativeCalls.push({name:String(key),args:[...args]});return Reflect.apply(value,target,args);};
 }});
 ui={module:instrumented,action:1,stageSelection:{stage:{id:1},difficulty:r.rank},gameSounds:queue,nativePause:null,pressed:new Set(),worldMode:true,active:true,paused:true,replayPlayback:false,nameFieldProfile:null};
 const forbidden=label=>()=>{throw Error('Outside admitted lifecycle scope: '+label);};
 class Ledger{async commit(receipt){const payload=JSON.stringify(receipt);return{payload,digest:sha(Buffer.from(payload)),strictReadback:true,syntheticStorage:true};}close(){}}
 class Gate extends sound.MenuAudioGate{constructor(q){super(q,{createContext:()=>{const a=webAudio(events);a.resume=async()=>{a.state='running';};a.suspend=async()=>{a.state='suspended';};a.close=async()=>{a.state='closed';};return a;}});}}
 const body=exact(r.play,'function releaseOriginalPauseInput(){','\nasync function openOriginalNativePause');
 const deps={state:ui,suppressedKeys:new Set(),originalReplayPreviewPacket:sound.originalReplayPreviewPacket,
  originalReplayNameContextPacket:sound.originalReplayNameContextPacket,originalNameProfilePacket:sound.originalNameProfilePacket,
  OriginalPauseController:sound.OriginalPauseController,MenuAudioGate:Gate,OwnedMenuSoundLedger:Ledger,
  OwnedReplayStore:forbidden('automaticReplayStore'),OwnedLastNameFieldStore:forbidden('automaticNameStore'),
  ordinaryWorldAudioLane:async()=>null,consumeMenuSoundWindow:sound.consumeMenuSoundWindow,window:{__TH128_FAST_REPLAY__:false},
  menu:{pauseOpen:true,page:'pause',fault:false},clock:{reset(){}},originalMenuClock:{reset(){}},
  resetHighFrameratePresentation:forbidden('resume'),find:forbidden('DOM'),worldMovementInput:forbidden('movement'),
  status:forbidden('status'),returnToTitle:forbidden('Title'),returnOwnedReplayToTitle:forbidden('ReplayTitle'),
  restartOwnedReplay:forbidden('ReplayRestart'),recordOwnedMainSession:forbidden('recordOwnedMainSession'),
  prepareStage:forbidden('prepareStage'),startLaunch:forbidden('startLaunch'),loadOriginalHelpPage:forbidden('graphics'),
  originalHelpPagePins:{},safeAssetUrl:forbidden('assetURL')};
 durable('extracted-production-functions.json',{path:r.play,bytes:Buffer.byteLength(body),sha256:sha(Buffer.from(body)),wholeFunctions:['releaseOriginalPauseInput','bindOriginalReplayPreviews','bindOriginalReplayNameContext','bindOriginalNameProfile','originalPauseConsumer']});
 return new Function(...Object.keys(deps),body+'\nreturn originalPauseConsumer;')(...Object.values(deps));
}
function installStores(label){
 const gate=barrier(),slots=Array(25).fill(null),profile={syntheticOwner:label};
 const replayStore={async observe(){events.push({store:label,op:'observe-replay'});return slots;},save(){throw Error('No replay save transaction admitted');}};
 const nameStore={async observe(){events.push({store:label,op:'observe-name-enter'});gate.arrived=true;gate.mark();await gate.held;events.push({store:label,op:'observe-name-release'});return profile;},save(){throw Error('No name save transaction admitted');}};
 ui.replayStore=replayStore;ui.nameFieldStore=nameStore;ui.replaySlots=null;ui.nameFieldProfile={syntheticSuccessorSentinel:label};
 return{gate,slots,replayStore,nameStore,profile,label};
}
async function recordThree(sound){
 for(let i=0;i<3;++i){
  need(native._th128_world_input(0,0,0),'actual neutral input');
  const now=performance.now()/1000,dt=now-lastClock;need(native._th128_world_replay_clock(now,dt>0?1/dt:0),'actual clock');lastClock=now;
  ++gameTicks;assert(gameTicks<=limits.worldTicks);need(native._th128_world_tick(),'actual World tick');await drain(sound);
  need(native._th128_world_records_present(performance.now()/1000),'actual recording presentation');
 }
 const s=state();assert.equal(s.frame,3);assert.equal(s.normalRecord.frames,3);assert.equal(s.debug.runTainted,false);assert.equal(s.normalRecord.normalClear,false);
 return s;
}
async function navigateToHeldName(consumer,stores,label){
 controller=consumer();const current=controller;await current.open();
 const frozen=state();assert.equal(pauseState().disabled[2],false);
 for(let step=0;step<110;++step){
  const p=pauseState();
  if(p.ready&&!p.closing){
   let key;if(p.confirm)key=p.confirmSelection===0?'KeyZ':'ArrowDown';else key=p.selection===2?'KeyZ':'ArrowDown';
   current.keyDown(key);current.keyUp(key);
  }
  let settled=false;const pending=current.step().then(()=>{settled=true;return{ok:true};},error=>{settled=true;return{ok:false,error};});
  ++menuTicks;assert(menuTicks<=limits.menuTicks);
  const {until}=require('./audio-fixture.cjs');await until(()=>settled||stores.gate.arrived);
  const now=state();assert.equal(now.frame,frozen.frame);assert.deepEqual(now.flow.gameRandom,frozen.flow.gameRandom);
  if(stores.gate.arrived){
   await stores.gate.entered;await Promise.resolve();assert.equal(settled,false,'explicit name observation is still awaited');
   const opened=pauseState();assert.equal(opened.active,true);assert.equal(opened.action,4);assert.equal(opened.replayPhase,1);assert.equal(opened.replayProfileBound,false);
   const held={...stores,controller:current,pending,settled:()=>settled,opened,world:now};heldSessions.push(held);
   durable(label+'-save-owner-entered.json',{pause:opened,world:now,gameTicks,menuTicks,nativeCallCount:actualNativeCalls.length,explicitEntered:true,pending:true});
   return held;
  }
  const outcome=await pending;assert.equal(outcome.ok,true,outcome.error?.stack);
 }
 throw Error('Original ReplaySave menu did not reach name barrier');
}
function resetAndProve(label){
 strictAudioOwner?.close();strictAudioOwner=null;drain.serial=undefined;
 native._th128_world_reset();
 check(label+' reset removes native pause',()=>assert.deepEqual(pauseState(),{configured:false,active:false}));
 check(label+' reset removes recording',()=>{const s=state();assert.equal(s.normalRecord.started,false);assert.equal(s.normalRecord.frames,0);assert.equal(s.normalRecord.normalClear,false);});
 const rejects={pauseTick:native._th128_world_pause_tick(0,0),saveOpen:native._th128_world_replay_save_open(),pauseCommit:native._th128_world_pause_commit(1)};
 check(label+' no-owner calls rejected',()=>assert.deepEqual(rejects,{pauseTick:0,saveOpen:0,pauseCommit:0}));
 durable(label+'-reset.json',{pause:pauseState(),world:state(),rejects,meaning:'No-owner operations, not a stale valid save ACK'});
}
(async()=>{
 fs.mkdirSync(r.output,{recursive:false});const start=Date.now();
 const factory=(await import(pathToFileURL(r.entry).href)).default;
 native=await factory({wasmBinary:fs.readFileSync(r.wasm),print:s=>events.push({stdout:String(s)}),printErr:s=>events.push({stderr:String(s)})});++Factories;
 const sound=await prepareAudio();const consumer=makeConsumer(sound);
 await configure(1);const firstRecord=await recordThree(sound),oldStore=installStores('old');
 const old=await navigateToHeldName(consumer,oldStore,'old');const firstGeneration=firstRecord.flow.worldGeneration;
 await bounded(old.controller.dispose(),'old dispose');
 check('dispose does not claim native close',()=>{assert.equal(old.controller.closed,true);assert.equal(pauseState().active,true);assert.equal(pauseState().replayPhase,1);});
 ui.nativePause=null;ui.action++;ui.stageSelection={stage:{id:1},difficulty:r.rank};
 resetAndProve('between-worlds');
 music.dispose();music=new sound.StageMusic(audio,queue.gain);
 await configure(1);const secondRecord=await recordThree(sound);
 check('new World generation',()=>assert.notEqual(secondRecord.flow.worldGeneration,firstGeneration));
 const nextStore=installStores('successor'),next=await navigateToHeldName(consumer,nextStore,'successor');
 check('both real native save entries',()=>assert.equal(actualNativeCalls.filter(x=>x.name==='_th128_world_replay_save_open').length,2));
 const beforeCalls=actualNativeCalls.length,beforeState=state(),beforePause=pauseState(),beforeSlots=ui.replaySlots,beforeProfile=ui.nameFieldProfile,beforeOwner=ui.nativePause;
 check('old await remains held after successor open',()=>assert.equal(old.settled(),false));
 old.gate.release();const outcome=await bounded(old.pending,'old callback settle');
 check('retired callback settles cleanly',()=>assert.equal(outcome.ok,true,outcome.error?.stack));
 check('retired callback issues no native call',()=>assert.equal(actualNativeCalls.length,beforeCalls));
 check('retired callback preserves successor refs',()=>{assert.equal(ui.nativePause,beforeOwner);assert.equal(ui.replaySlots,beforeSlots);assert.equal(ui.nameFieldProfile,beforeProfile);assert.equal(ui.replayStore,nextStore.replayStore);assert.equal(ui.nameFieldStore,nextStore.nameStore);});
 check('retired callback preserves actual native state',()=>{assert.deepEqual(state(),beforeState);assert.deepEqual(pauseState(),beforePause);});
 await bounded(next.controller.dispose(),'successor dispose');ui.nativePause=null;++ui.action;
 resetAndProve('final');
 const finalCalls=actualNativeCalls.length;next.gate.release();const nextOutcome=await bounded(next.pending,'second callback settle');
 check('second disposed callback makes no native call',()=>{assert.equal(nextOutcome.ok,true,nextOutcome.error?.stack);assert.equal(actualNativeCalls.length,finalCalls);});
 check('no save ACK or name commit',()=>{assert.equal(actualNativeCalls.filter(x=>x.name==='_th128_world_replay_saved').length,0);assert.equal(actualNativeCalls.filter(x=>x.name==='_th128_world_replay_snapshot').length,0);});
 check('one Factory two Worlds six ticks',()=>{assert.equal(Factories,1);assert.equal(WorldStarts,2);assert.equal(gameTicks,6);});
 check('no stderr from native',()=>assert.equal(events.filter(x=>x&&x.stderr).length,0));
 const result={passed:true,Factories,WorldStarts,gameTicks,recordedFrames:[firstRecord.normalRecord.frames,secondRecord.normalRecord.frames],menuTicks,
  worldGenerations:[firstGeneration,secondRecord.flow.worldGeneration],realNativeSaveOpens:2,lateNativeCalls:0,menuFrameAndGameRNGFrozen:true,
  checks:rows.length,rows,fullOriginalConsumer:true,actualCore:102,actualWorldReset:true,
  nativeRetiredByDispose:false,nativeResetAndReopenProven:true,storageAndAudioSynthetic:true,browser:0,GPU:0,nativeCompiles:0,
  realDurableACK:0,fullTitleMainPrepared:false,normalClearClaimed:false,fullReplayClaimed:false,
  sameWorldReopenWithoutResetProven:false,fullSaveTransactionProven:false,elapsedMs:Date.now()-start};
 assert(result.elapsedMs<r.maxWallMs);durable('branch-result.json',result);durable('native-calls.json',actualNativeCalls);durable('synthetic-events.json',events);
 console.log(JSON.stringify(result));
})().catch(e=>{
 durable('first-failure.json',{message:e.message,stack:e.stack,Factories,WorldStarts,gameTicks,menuTicks,rows,world:native?state():null,pause:native?pauseState():null,nativeError:native?error():null,nativeCalls:actualNativeCalls,events});
 console.error(e.stack);process.exitCode=1;
}).finally(async()=>{
 for(const h of heldSessions){h.gate.release();await bounded(h.controller.dispose(),'cleanup dispose').catch(()=>{});await bounded(h.pending,'cleanup pending').catch(()=>{});}
 if(controller)await bounded(controller.dispose(),'last controller').catch(()=>{});
 strictAudioOwner?.close();strictReceiptDB?.close();queue?.dispose();music?.dispose();if(audio)await audio.close();if(native)native._th128_world_reset();
});
