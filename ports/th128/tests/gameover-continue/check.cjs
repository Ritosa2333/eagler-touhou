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
async function prepareAudio(){const sound=await import(pathToFileURL(r.sound).href),{webAudio,ReceiptsDatabase}=require('./audio-fixture.cjs');hostModule=await import(pathToFileURL(r.browserHost).href);strictReceiptDB=new ReceiptsDatabase(events);audio=webAudio(events);audio.decodeAudioData=async b=>decodedWave(b);audio.state='running';audio.resume=async()=>{audio.state='running';};audio.close=async()=>{audio.state='closed';};const gain=audio.createGain();queue=new sound.GameSoundQueue(audio,gain);music=new sound.StageMusic(audio,gain);
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
let naturalGameOvers=0,titleCommitted=false,scoreReceipt=null,scoreWrites=0,controller;
function pauseState(){return JSON.parse(native.UTF8ToString(native._th128_world_pause_state()));}
async function consumeNaturalGameOver(sound){
  ++naturalGameOvers;assert(naturalGameOvers<=2);const before=state(),frozenFrame=before.frame,frozenRandom=before.flow.gameRandom;
  assert(before.motivationRaw<0);assert.equal(before.debug.runTainted,false);assert.equal(before.normalRecord.frames,frozenFrame);
  const initial=pauseState();assert.equal(initial.kind,1);assert.equal(initial.continues,naturalGameOvers-1);assert.equal(initial.active,true);
  assert.equal(initial.disabled[0],false);if(naturalGameOvers===2)assert.equal(initial.disabled[2],true);
  durable('natural-gameover-'+naturalGameOvers+'.json',{gameTicks,continues,actualState:before,pause:initial,syntheticAudioStorage:true});
  if(naturalGameOvers===2){
    assert.equal(native._th128_world_replay_save_open(),0,'continued native run must reject replay saving');
    trace.push({kind:'continuedReplayRejected',error:error()});
  }
  let resumed=false;
  controller=new sound.OriginalPauseController({module:native,
    releaseInput(){need(native._th128_world_input(0,0,763),'release_original_pause_input');lastHeld=0;},
    consumeSound:()=>drain(sound),
    audio:{async enter(){events.push('synthetic-menu-audio-enter');},async leave(){events.push('synthetic-menu-audio-leave');},async suspend(){}},
    onResume(){resumed=true;++continues;},onTitle(){titleCommitted=true;},
    onRestart(){throw Error('Restart outside598scope');},
    async onPresentation(snapshot){assert.equal(snapshot.serial,initial.serial);}
  });
  await controller.open({alreadyOpened:true});
  const choose=naturalGameOvers===1?0:1;
  for(let steps=0;controller.active&&steps<100;++steps){
    const menu=pauseState();
    if(menu.ready&&!menu.closing){const code=menu.selection===choose?'KeyZ':'ArrowDown';controller.keyDown(code);controller.keyUp(code);}
    await controller.step();++menuTicks;
    const after=state();assert.equal(after.frame,frozenFrame,'pause must not advance World frame');
    assert.deepEqual(after.flow.gameRandom,frozenRandom,'pause must not advance gameplay RNG');
  }
  assert.equal(controller.active,false,'bounded original menu must commit its selected action');
  assert.equal(pauseState().active,false);
  if(naturalGameOvers===1){
    assert.equal(resumed,true);assert.equal(continues,1);assert.equal(pauseState().continues,1);
    const after=state();assert.equal(after.scoreRaw,0);assert.equal(after.motivationRaw,20000);
    assert.equal(after.normalRecord.frames,before.normalRecord.frames);assert.equal(after.normalRecord.prepared,false);
    durable('actual-continue-commit.json',{before:initial,after:pauseState(),actualState:after,menuTicks,continues});
  }else{
    assert.equal(titleCommitted,true);assert.equal(resumed,false);assert.equal(continues,1);
    durable('actual-failed-title-action.json',{pause:pauseState(),actualState:state(),menuTicks,continues});
  }
  await controller.dispose();controller=null;
}
(async()=>{
  fs.mkdirSync(r.output,{recursive:false});
  const factory=(await import(pathToFileURL(r.entry).href)).default;
  native=await factory({wasmBinary:fs.readFileSync(r.wasm),print:s=>events.push({stdout:String(s)}),printErr:s=>events.push({stderr:String(s)})});++Factories;
  const sound=await prepareAudio();await configure(1);const started=Date.now();
  while(!titleCommitted&&gameTicks<r.maxGameTicks&&Date.now()-started<r.maxWallMs){
    const message=JSON.parse(native.UTF8ToString(native._th128_world_message_state()));
    const confirm=message.active&&message.waiting&&gameTicks%12===0,holding=512|(confirm?1:0),pressed=holding&~lastHeld,released=lastHeld&~holding;
    need(native._th128_world_input(holding,pressed,released),'actual_input');lastHeld=holding;
    inputDigest.update(Buffer.from(Uint16Array.from([holding,pressed,released]).buffer));
    const now=performance.now()/1000,dt=now-lastClock;need(native._th128_world_replay_clock(now,dt>0?1/dt:0),'actual_replay_clock');lastClock=now;
    const tick=native._th128_world_tick();++gameTicks;await drain(sound);
    need(native._th128_world_records_present(performance.now()/1000),'actual_record_presentation');
    if(tick===2)await consumeNaturalGameOver(sound);else need(tick,'actual_World_tick');
    if(gameTicks%500===0)process.stdout.write(JSON.stringify({progress:{gameTicks,naturalGameOvers,continues,elapsedMs:Date.now()-started}})+'\n');
  }
  assert(titleCommitted&&naturalGameOvers===2&&continues===1,'bounded natural failure/continue/Title chain incomplete');
  const before=state();assert.equal(before.normalRecord.frames,gameTicks-naturalGameOvers);assert.equal(before.normalTerminal.begun,false);
  assert.equal(before.normalRecord.normalClear,false);assert.equal(before.normalRecord.prepared,false);assert.equal(before.debug.runTainted,false);
  const timestamp=BigInt(Math.floor(Date.now()/1000)),bytes=native._th128_world_records_prepare(Number(timestamp&0xffffffffn),Number(timestamp>>32n),performance.now()/1000);
  assert.equal(bytes,52232,error());const actual=state(),pointer=native._th128_world_records_data();
  const profile=native.HEAPU8.slice(pointer,pointer+bytes),metadata={...actual.normalRecord,generation:actual.flow.worldGeneration};
  const capture={profile,native:metadata,runId:'case598-actual-failed-A1-continue1'};
  assert.equal(metadata.terminalStatsPrepared,false);assert.equal(metadata.normalClear,false);
  const view=new DataView(profile.buffer,profile.byteOffset,profile.byteLength);
  for(let route=0;route<7;++route)for(let rank=0;rank<5;++rank)assert.equal(view.getUint32(route*1644+0x658+rank*4,true),0,'failure must not grant any route clear');
  const store={async commit(revision,source){
    assert.equal(revision,0);++scoreWrites;
    const written={revision:1,runId:source.runId,native:{...source.native},profile:source.profile.slice(),strictFullReadback:true,strictDurability:true};
    const readback=structuredClone(written);assert.deepEqual(readback.profile,source.profile);
    durable('actual-failed-native-profile-52232.bin',Buffer.from(source.profile));return readback;
  }};
  scoreReceipt=await sound.persistOwnedNativeScoreCapture(native,store,0,capture);
  assert.equal(scoreReceipt.nativeAcknowledged,true);assert.equal(scoreWrites,1);
  const final=state();assert.equal(final.normalRecord.acknowledged,true);assert.equal(final.normalRecord.normalClear,false);
  assert.equal(final.frame,before.frame);assert.deepEqual(final.flow.gameRandom,before.flow.gameRandom);
  const result={passed:true,Factories,WorldStarts,gameTicks,recordedFrames:final.normalRecord.frames,naturalGameOvers,continues,menuTicks,titleActionCommitted:titleCommitted,
    scoreWrites,profileBytes:bytes,profileSHA256:sha(Buffer.from(profile)),nativeACK:true,zeroClearCounters:true,gameRandom:final.flow.gameRandom,
    inputSHA256:inputDigest.digest('hex'),extraWorldTicksAfterTitle:0,nativeError:error(),pause:pauseState(),
    actualNativeWorldAndPauseController:true,syntheticAudioOutputAndScoreStore:true,browser:0,GPU:0,realDurableACK:0,fullTitleMainPrepared:false,
    purpose:'natural failure/oneContinue/failedTitle action/native score ACK; no unassisted clear gate, no rerun accepted fullroutes',trace};
  assert.equal(result.nativeError,'');durable('branch-result.json',result);console.log(JSON.stringify(result));
})().catch(e=>{
  durable('first-failure.json',{message:e.message,stack:e.stack,Factories,WorldStarts,gameTicks,menuTicks,naturalGameOvers,continues,titleCommitted,
    actualState:native?state():null,pause:native?pauseState():null,nativeError:native?error():null,trace,syntheticAudioOutputAndScoreStore:true});
  console.error(e.stack);process.exitCode=1;
}).finally(async()=>{await controller?.dispose();strictAudioOwner?.close();queue?.dispose();music?.dispose();if(audio)audio.state='closed';});
