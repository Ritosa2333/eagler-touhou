import assert from 'node:assert/strict';

export const REQUIRED_RUNTIME_CHECKS=Object.freeze(['runtime-input-identity','runtime-ready','configure-launch-first-frame',
  'reject-unknown-command','save-list-and-flush','hosted-keyboard-native-effect','native-gamepad-sampling','native-gamepad-effect',
  'native-pause-and-restart','resize-and-continued-rendering','runtime-errors','reload-storage-and-epoch','replay-file-roundtrip-persistence']);

export function runtimeScenarioResult({checks=[],failure}={}) {
  const missing=REQUIRED_RUNTIME_CHECKS.filter(id=>!checks.some(check=>check.id===id&&check.status==='passed'));
  const failed=Boolean(failure)||checks.some(check=>check.status==='failed');
  return {status:failed?'failed':missing.length?'incomplete':'passed',passed:!failed&&!missing.length,unverifiedChecks:missing};
}

/** Read only established Runtime exports. This function is self-contained so it can be passed to frame.evaluate. */
export function readRuntimeObservation(target) {
  const th11Phases = ['title', 'stage', 'paused', 'game_over', 'finished', 'ending'];
  const th09Phases = ['inactive', 'match', 'game_over', 'match_complete', 'ending', 'finished'];
  const th15Phases = ['title', 'stage', 'paused', 'ending', 'exit'];
  const game = typeof target === 'string' ? target : target?.game;
  const variant = typeof target === 'object' ? target.variant : undefined;
  const root = typeof window === 'object' ? window : globalThis;
  const runtime = root[`__${game}Runtime`];
  const module = runtime?.Module ?? root.Module;
  // Shells expose a few different Emscripten/WebAssembly shapes: a named
  // runtime wrapper, `window.core` as raw instance.exports, or only Module.
  // Keep the reader tolerant of those real mounting shapes without probing
  // or invoking game actions.
  const core = runtime?.core ?? root.core ?? module;
  const nativeExport = name => {
    for (const owner of [core, module]) {
      if (!owner) continue;
      const fn = owner[name] ?? owner[`_${name}`];
      if (typeof fn === 'function') return fn.bind(owner);
    }
    return undefined;
  };
  const buffer = core?.memory?.buffer ?? module?.HEAPU8?.buffer ?? root.HEAPU8?.buffer ?? null;
  const finite = value => typeof value === 'number' && Number.isFinite(value) ? value : null;
  const wasmArray = (fn, count, Type) => {
    if (typeof fn !== 'function' || !buffer) return null;
    const pointer = fn();
    if (!Number.isInteger(pointer) || pointer <= 0 || pointer + count * 4 > buffer.byteLength) return null;
    return Array.from(new Type(buffer, pointer, count));
  };
  const unsupported = [];
  const result = {
    schema: 'eagler-touhou/runtime-observation/1', game, variant: variant ?? null,
    frames: {}, phase: null, input: null, position: null, pause: null,
    render: {presentationFrame: null, renderCallbacks: null}, replay: null,
    status: null, motionSample: null, touchInput: null,
    capabilities: {phase: false, input: false, position: false, pause: false,
      restartFrame: null, presentationFrame: false},
    unsupported,
  };
  const setPresentation = value => {
    const n = finite(value);
    if (n !== null) {
      result.render.presentationFrame = n;
      result.capabilities.presentationFrame = true;
    }
  };
  const readPresentation = () => {
    const values = wasmArray(nativeExport('sdl_stats'), 6, Uint32Array);
    if (values) setPresentation(values[5]);
  };

  if (game === 'th06' || game === 'th07') {
    const ready = nativeExport('eagler_replay_ready');
    const replayState = nativeExport('eagler_replay_state');
    if (typeof ready === 'function') {
      const pointer = ready();
      result.replay = {ready: pointer === 1};
      if (typeof replayState === 'function') {
        const values = wasmArray(replayState, 5, Uint32Array);
        if (values) result.replay.state = {frame: values[0], total: values[1], ended: values[2] !== 0,
          stage: values[3], startStage: values[4]};
      }
    }
    if (game === 'th07') result.render.renderCallbacks = finite(root.__eaglerDebugRenderCallbacks);
    unsupported.push('gameplay-phase', 'gameplay-frame', 'keyboard-gameplay-effect', 'pause-state', 'restart-state', 'player-position');
  } else if (game === 'th08') {
    const app = runtime?.app;
    const statusExport = nativeExport('status'), traceExport = nativeExport('trace'), diagnosticsExport = nativeExport('diagnostics');
    let status=null,trace=null,tracePtr=null,diagnostics=null,multiplayer=null,netplay=null,netProgress=null,touch=null;
    const memoryBuffer=()=>core?.memory?.buffer??module?.HEAPU8?.buffer??root.HEAPU8?.buffer??null;
    const float2FromCurrentBuffer=(pointer,offset=0)=>{
      const current=memoryBuffer();
      if(!current||!Number.isInteger(pointer)||pointer<=0||pointer+(offset+2)*4>current.byteLength)return null;
      const values=Array.from(new Float32Array(current,pointer+offset*4,2));
      return values.every(Number.isFinite)?{x:values[0],y:values[1]}:null;
    };
    const statusFn=(name,count,Type)=>{
      const fn=nativeExport(name);
      if(typeof fn!=='function')return null;
      const pointer=fn(app);
      const currentBuffer=memoryBuffer();
      return Number.isInteger(pointer)&&pointer>0&&currentBuffer&&pointer+count*4<=currentBuffer.byteLength
        ?Array.from(new Type(currentBuffer,pointer,count)):null;
    };
    if(app!=null){
      if(statusExport)status=Array.from({length:11},(_,index)=>finite(statusExport(app,index)));
      tracePtr=traceExport?.(app);const currentBuffer=memoryBuffer();
      if(Number.isInteger(tracePtr)&&tracePtr>0&&currentBuffer&&tracePtr+68*4<=currentBuffer.byteLength)
        trace=Array.from(new Uint32Array(currentBuffer,tracePtr,68));
      if(diagnosticsExport)diagnostics=wasmArray(()=>diagnosticsExport(app),16,Int32Array);
      if(variant==='multiplayer'){
        multiplayer=statusFn('multiplayer_status',44,Int32Array);
        netplay=statusFn('multiplayer_netplay_status',15,Uint32Array);
        netProgress=statusFn('multiplayer_progress',6,Uint32Array);
      }
      // touch_state writes a read-only snapshot into caller-owned memory. Use
      // the title's paired allocator so this never borrows or overwrites a
      // production buffer, and release it immediately after copying the data.
      const touchExport=nativeExport('touch_state'),allocate=nativeExport('allocate'),deallocate=nativeExport('deallocate');
      if(touchExport&&allocate&&deallocate){
        let pointer=0;
        try{
          pointer=allocate(12*4);
          const current=memoryBuffer();
          if(Number.isInteger(pointer)&&pointer>0&&current&&pointer+12*4<=current.byteLength){
            touchExport(app,pointer);
            const after=memoryBuffer();
            if(after&&pointer+12*4<=after.byteLength){
              const values=Array.from(new Float32Array(after,pointer,12));
              if(values.every(Number.isFinite))touch=values;
            }
          }
        }catch{touch=null;}
        finally{if(Number.isInteger(pointer)&&pointer>0)deallocate(pointer);}
      }
    }
    if(trace){
      result.frames.enemy=finite(status?.[6])??finite(trace[2]);
      result.frames.control=finite(trace[3]);
      result.position=float2FromCurrentBuffer(tracePtr,7);
      result.capabilities.position=result.position!==null;
      result.input=diagnostics?{kind:variant==='multiplayer'?'native-consumed-buttons':'logical-buttons',value:diagnostics[11]}:null;
      result.capabilities.input=result.input!==null;
      result.pause=diagnostics?{active:diagnostics[6]!==0,raw:diagnostics[6]}:null;
      result.capabilities.pause=result.pause!==null;
      if(status)result.status={supervisorActive:status[0],active:status[1],invalid:status[2],stage:status[3],
        loading:status[7],titleScreen:status[8],cursor:status[9],supervisorState:diagnostics?.[9]??null};
      result.capabilities.restartFrame=variant==='multiplayer'?null:'enemy';
      if(variant==='normal'&&status&&status[0]===2&&status[1]===1&&!status[2]&&!status[7]){
        const resultScreen=Boolean(diagnostics?.[7]||diagnostics?.[8]);
        result.phase={value:resultScreen?3:result.pause?.active?2:1,
          name:resultScreen?'game_over':result.pause?.active?'paused':'stage'};
        result.capabilities.phase=true;result.localSeat=0;
      }
    }
    if(variant==='multiplayer'){
      result.input=null;result.position=null;result.localSeat=null;
      result.capabilities.input=result.capabilities.position=false;
      result.capabilities.restartFrame=null;
      if(multiplayer&&netplay){
        const seat=multiplayer[2],count=multiplayer[1];
        if(multiplayer[0]&&count>=2&&count<=3&&seat>=0&&seat<count){
          result.localSeat=seat;
          if(netplay[4]!==0xffffffff)result.frames.multiplayer=netplay[4];
          const offset=8+12*seat;
          if(multiplayer[3]&&!multiplayer[4]&&netplay[7]&&multiplayer[offset+9]){
            result.input={kind:'native-consumed-buttons',value:multiplayer[offset+10]};
            result.position={x:multiplayer[offset+5],y:multiplayer[offset+6]};
            result.capabilities.input=result.capabilities.position=true;
            result.phase={value:result.pause?.active?2:1,name:result.pause?.active?'paused':'stage'};
            result.capabilities.phase=true;
          }
        }
      }
    }
    const statsExport=nativeExport('sdl_stats'),statsPointer=statsExport?.(),statsBuffer=memoryBuffer();
    const stats=Number.isInteger(statsPointer)&&statsPointer>0&&statsBuffer&&statsPointer+24<=statsBuffer.byteLength
      ?Array.from(new Uint32Array(statsBuffer,statsPointer,6)):null;
    if(stats)setPresentation(stats[5]);

    const flag=value=>value===null||value===undefined?null:value!==0;
    const invalidFrame=value=>value===0xffffffff?null:finite(value);
    const touchMode=touch?touch[0]:null;
    const touchName=touchMode===0?'inactive-or-blocked':touchMode===1?'gameplay':touchMode===2?'dialogue':touchMode===3?'special-game-mode':null;
    const stage=variant==='multiplayer'?finite(multiplayer?.[6]):finite(status?.[3]);
    const enemyFrame=variant==='multiplayer'?finite(multiplayer?.[7]):finite(status?.[6])??finite(trace?.[2]);
    const controlTick=finite(trace?.[3]);
    const loading=flag(diagnostics?.[3]??status?.[7]??null);
    const controlLoading=finite(diagnostics?.[4]);
    const pauseState=finite(diagnostics?.[6]);
    const retrying=flag(diagnostics?.[7]??null);
    const gameOver=flag(diagnostics?.[8]??null);
    const dialogue=touch?touchMode===2:null;
    const transitionFlags=finite(trace?.[4]);
    const inTransition=transitionFlags===null?null:Boolean(transitionFlags&0x60);
    let localPlayer={seat:null,eligible:null,movementAvailable:null,inputButtons:null,position:null,lifeState:null};
    if(variant==='normal'&&result.localSeat===0){
      const playerAvailable=touch&&(touchMode===1||touchMode===2)?touch[1]===1:null;
      localPlayer={seat:0,eligible:null,movementAvailable:playerAvailable,
        inputButtons:finite(diagnostics?.[11]),position:result.position?{...result.position}:null,
        lifeState:finite(trace?.[9])};
    }else if(variant==='multiplayer'&&multiplayer&&netplay&&result.localSeat!==null){
      const offset=8+12*result.localSeat,eligible=multiplayer[offset+9]===1;
      const playerReady=Boolean(multiplayer[3]&&!multiplayer[4]&&netplay[7]&&eligible);
      localPlayer={seat:result.localSeat,eligible,movementAvailable:null,
        inputButtons:playerReady?finite(multiplayer[offset+10]):null,
        position:playerReady?{x:multiplayer[offset+5],y:multiplayer[offset+6]}:null,
        lifeState:playerReady?finite(multiplayer[offset+4]):null};
    }
    let network=null,frontier=null,correctionIdle=null,allFramesConfirmed=null,networkSettled=null;
    if(variant==='multiplayer'&&netplay){
      network={version:finite(netplay[0]),configured:flag(netplay[1]),canStart:flag(netplay[2]),
        nextFrame:finite(netplay[3]),lastCompletedFrame:invalidFrame(netplay[4]),
        confirmedThrough:invalidFrame(netplay[5]),rollbackFrame:invalidFrame(netplay[6]),worldReady:flag(netplay[7]),
        frameOpen:flag(netplay[8]),waiting:flag(netplay[9]),generation:finite(netplay[10]),retired:flag(netplay[11]),
        inputDelay:finite(netplay[12]),nextCaptureFrame:invalidFrame(netplay[13]),mode:finite(netplay[14])};
      if(netProgress){
        frontier={predictionFrontier:invalidFrame(netProgress[0]),nextFrame:finite(netProgress[1]),
          correcting:flag(netProgress[2]),callbackYielded:flag(netProgress[3]),correctionYields:finite(netProgress[4]),
          correctionEnd:invalidFrame(netProgress[5])};
      }
      if(network.lastCompletedFrame!==null&&network.confirmedThrough!==null)
        allFramesConfirmed=network.confirmedThrough>=network.lastCompletedFrame;
      if(frontier)correctionIdle=network.rollbackFrame===null&&frontier.correcting===false&&frontier.correctionEnd===null;
      networkSettled=Boolean(network.configured&&network.worldReady&&!network.retired&&!network.frameOpen&&!network.waiting&&
        correctionIdle===true&&allFramesConfirmed===true&&frontier?.nextFrame===network.nextFrame&&
        frontier?.predictionFrontier===frontier?.nextFrame);
    }
    const normalRunning=variant==='normal'&&status?.[0]===2&&status?.[1]===1&&status?.[2]===0&&loading===false&&
      controlLoading===0&&pauseState!==null&&retrying!==null&&gameOver!==null&&dialogue!==null&&inTransition===false;
    const multiplayerRunning=variant==='multiplayer'&&loading===false&&controlLoading===0&&pauseState!==null&&retrying!==null&&
      gameOver!==null&&dialogue!==null&&inTransition===false;
    result.worldProgress={schema:'eagler-touhou/th08-world-progress/1',
      counters:{stage,enemySceneFrame:enemyFrame,controlTick,
        lastCompletedNetworkFrame:network?.lastCompletedFrame??null,confirmedNetworkFrame:network?.confirmedThrough??null,
        nextNetworkFrame:network?.nextFrame??null},
      state:{loading,controlLoadState:controlLoading,pauseState,paused:pauseState===null?null:pauseState!==0,
        retrying,gameOver,dialogue,touchContext:touchName,touchContextRaw:touchMode,inTransition},
      localPlayer,netplay:network?{...network,...(frontier?{frontier}:{}),correctionIdle,allFramesConfirmed,settled:networkSettled}:null,
      comparable:variant==='normal'?Boolean(normalRunning&&localPlayer.position&&localPlayer.inputButtons!==null&&
        Number.isFinite(stage)&&Number.isFinite(enemyFrame)&&Number.isFinite(controlTick)):
        Boolean(multiplayerRunning&&networkSettled&&localPlayer.seat!==null&&localPlayer.inputButtons!==null&&localPlayer.position&&
          Number.isFinite(stage)&&Number.isFinite(enemyFrame)&&Number.isFinite(controlTick))};
    if(!result.capabilities.phase)unsupported.push('standardized-phase');
    if (!result.capabilities.input) unsupported.push('keyboard-gameplay-effect');
    if (!result.capabilities.position) unsupported.push('player-position');
  } else if (game === 'th09') {
    let status = typeof runtime?.status === 'function' ? runtime.status() : null;
    if (!status && core) {
      const title = wasmArray(nativeExport('th09_title_status'), 8, Int32Array);
      const session = wasmArray(nativeExport('th09_session_status'), 8, Int32Array);
      const touch = wasmArray(nativeExport('th09_touch_state'), 4, Int32Array);
      if (title && session && touch) status = {title, session, touch};
    }
    if (status && Array.isArray(status.session)) {
      const session = status.session;
      const phaseValue = finite(session[0]);
      result.phase = {value: phaseValue, name: phaseValue === null ? null : th09Phases[phaseValue] ?? null};
      result.frames.session = finite(session[1]);
      result.capabilities.phase = result.phase.name !== null;
      result.capabilities.restartFrame = 'session';
      const appProbe = wasmArray(nativeExport('th09_probe_status'), 16, Int32Array);
      if (appProbe) {
        result.frames.runtime = finite(appProbe[0]);
        result.pause = {active: appProbe[9] === 1, raw: appProbe[9]};
        result.capabilities.pause = true;
      }
      const touchProbe = wasmArray(nativeExport('th09_touch_probe'), 8, Float32Array);
      if (touchProbe && phaseValue === 1) {
        result.position = {x: touchProbe[0], y: touchProbe[1]};
        result.motionSample = {enabled: touchProbe[4] !== 0, target: touchProbe[5] !== 0,
          x: touchProbe[6], y: touchProbe[7]};
        result.capabilities.position = true;
      }
      if (Array.isArray(status.touch) && status.touch.length >= 4) {
        const context = finite(status.touch[0]), ready = finite(status.touch[1]);
        const active = finite(status.touch[2]), inTitle = finite(status.touch[3]);
        const contexts = ['inactive', 'gameplay', 'dialogue-or-ending', 'replay'];
        result.touchInput = {
          context: context === null ? null : contexts[context] ?? null,
          contextValue: context,
          ready: ready === null ? null : ready === 1,
          readyValue: ready,
          active: active === null ? null : active !== 0,
          activeValue: active,
          inTitle: inTitle === null ? null : inTitle === 1,
          inTitleValue: inTitle,
        };
      }
      const metrics = wasmArray(nativeExport('th09_game_metrics'), 10, Uint32Array);
      if (metrics) setPresentation(metrics[9]);
    }
    if (!result.capabilities.phase) unsupported.push('session-phase');
    if (!result.capabilities.position) unsupported.push('player-position-outside-match');
    unsupported.push('keyboard-button-mask');
  } else if (game === 'th10') {
    const status = typeof runtime?.status === 'function' ? runtime.status() : null;
    if (Array.isArray(status)) {
      result.status = {screen: finite(status[0]), stage: finite(status[1]), error: finite(status[2]),
        lives: finite(status[3]), power: finite(status[4]), touchContext: finite(status[5]),
        touchActive: finite(status[6]), fire: finite(status[7]), focus: finite(status[8])};
      result.input = {kind: 'native-raw-input', value: finite(status[9])};
      result.capabilities.input = result.input.value !== null;
      const applicationWorld = nativeExport('application_world'), worldActor = nativeExport('world_actor');
      if (applicationWorld && worldActor && buffer) {
        const world = applicationWorld(runtime.app);
        const actor = world ? worldActor(world, 1) : 0;
        if (Number.isInteger(actor) && actor > 0 && actor + 0x3c8 <= buffer.byteLength) {
          const view = new DataView(buffer);
          const x = view.getFloat32(actor + 0x3c0, true), y = view.getFloat32(actor + 0x3c4, true);
          if (Number.isFinite(x) && Number.isFinite(y)) {
            result.position = {x, y};
            result.capabilities.position = true;
          }
        }
      }
    }
    readPresentation();
    unsupported.push('semantic-phase', 'pause-state', 'restart-frame');
    if (!result.capabilities.position) unsupported.push('player-position');
  } else if (game === 'th11') {
    const frame = nativeExport('th11_frame'), phase = nativeExport('th11_phase');
    if (typeof frame === 'function' && typeof phase === 'function') {
      const value = finite(phase());
      result.phase = {value, name: value === null ? null : th11Phases[value] ?? null};
      result.frames.session = finite(frame());
      result.capabilities.phase = result.phase.name !== null;
      result.pause = {active: result.phase.value === 2, raw: result.phase.value};
      result.capabilities.pause = true;
      if (variant === 'normal') result.capabilities.restartFrame = 'session';
    }
    unsupported.push('keyboard-gameplay-effect', 'player-position', 'presentation-frame');
    if (variant === 'multiplayer') unsupported.push('restart');
  } else if (game === 'th15') {
    const frame = nativeExport('th15_frame'), phase = nativeExport('th15_phase');
    if (typeof frame === 'function' && typeof phase === 'function') {
      const value = finite(phase());
      result.phase = {value, name: value === null ? null : th15Phases[value] ?? null};
      result.frames.application = finite(frame());
      result.capabilities.phase = result.phase.name !== null;
      result.pause = {active: result.phase.value === 2, raw: result.phase.value};
      result.capabilities.pause = true;
    }
    unsupported.push('keyboard-gameplay-effect', 'player-position', 'restart-frame', 'presentation-frame');
  } else if (game === 'th20') {
    const status = typeof runtime?.status === 'function' ? runtime.status() : null;
    if (Array.isArray(status)) {
      result.status = {scene: finite(status[0]), stage: finite(status[1]), error: finite(status[2]),
        titleState: finite(status[3]), titlePhase: finite(status[4]), titleCursor: finite(status[7]),
        pageCursor: finite(status[8])};
      result.input = {kind: 'native-raw-input', value: finite(status[9])};
      result.capabilities.input = result.input.value !== null;
    }
    readPresentation();
    unsupported.push('semantic-phase', 'player-position', 'pause-state', 'restart-frame');
  } else {
    unsupported.push('unknown-runtime');
  }

  // Versioned exports copy title-owned state without advancing a tick or
  // draining diagnostic/input queues. Legacy packages retain their readers.
  const native=wasmArray(nativeExport(`${game}_native_observation_v1`),12,Uint32Array);
  if(native){
    if(native[0]!==1||(native[1]&~0x1ff)!==0)throw new Error(`${game}: unsupported native observation ABI`);
    const valid=native[1];result.nativeObservation={version:native[0],validMask:valid};
    for(const [index,bit] of [[2,1],[3,2],[4,8],[5,0x10],[6,0x20],[7,0x40],[10,0x80],[11,0x100]]){
      if((valid&bit)&&native[index]===0xffffffff)throw new Error(`${game}: missing native observation field ${index} has a valid bit`);
      if(!(valid&bit)&&native[index]!==0xffffffff)throw new Error(`${game}: invalid native observation field ${index} has data`);
    }
    if(!(valid&4)&&(native[8]!==0x7fc00000||native[9]!==0x7fc00000))throw new Error(`${game}: invalid native position has data`);
    Object.assign(result.capabilities,{phase:false,input:false,position:false,pause:false,restartFrame:null});
    result.phase=result.input=result.position=result.pause=null;
    for(const name of ['simulation','restart','multiplayer','application'])delete result.frames[name];
    if(valid&1){
      const names=['title',game==='th09'?'match':'stage','paused','game_over','ending','finished','inactive','transition'];
      if(!names[native[2]])throw new Error(`${game}: invalid observed native phase`);
      result.phase={value:native[2],name:names[native[2]]};result.capabilities.phase=true;
    }
    if(valid&2){result.input={kind:'native-consumed-buttons',value:native[3]};result.capabilities.input=true;}
    if(valid&4){
      const words=new Uint32Array([native[8],native[9]]),values=new Float32Array(words.buffer);
      if(!values.every(Number.isFinite))throw new Error(`${game}: invalid observed player position`);
      result.position={x:values[0],y:values[1]};result.capabilities.position=true;
    }
    if(valid&8)result.frames.simulation=native[4];
    if(valid&0x10){result.frames.restart=native[5];if(variant==='normal')result.capabilities.restartFrame='restart';}
    if(valid&0x20){
      if(native[6]>1)throw new Error(`${game}: invalid observed pause state`);
      result.pause={active:native[6]===1,raw:native[6]};result.capabilities.pause=true;
    }
    if(valid&0x40)result.frames.multiplayer=native[7];
    if(valid&0x80)result.frames.application=native[10];
    if(valid&0x100){
      if(native[11]>2||(variant==='normal'&&native[11]!==0))throw new Error(`${game}: observed input belongs to an invalid local seat`);
      result.localSeat=native[11];
    }
    if(!result.capabilities.restartFrame&&!unsupported.includes('restart-frame'))unsupported.push('restart-frame');
    for(let i=unsupported.length-1;i>=0;i--){
      const item=unsupported[i];
      if((result.capabilities.phase&&['phase','semantic-phase','standardized-phase','session-phase','gameplay-phase'].includes(item))||
         (result.capabilities.input&&['input','keyboard-gameplay-effect','keyboard-button-mask'].includes(item))||
         (result.capabilities.position&&item.includes('position'))||
         (result.capabilities.pause&&item==='pause-state')||
         (result.capabilities.restartFrame&&['restart','restart-frame','restart-state'].includes(item))||
         (Number.isFinite(result.frames.simulation)&&item==='gameplay-frame'))unsupported.splice(i,1);
    }
  }

  if(game==='th15'){
    const context=wasmArray(nativeExport('th15_input_context_v1'),12,Uint32Array);
    if(context){
      if(context[0]!==1||(context[1]&~0x3ff)!==0)throw new Error('th15: unsupported input context ABI');
      const valid=context[1];
      for(let index=2;index<12;index++){
        const available=Boolean(valid&(1<<(index-2)));
        if(available===(context[index]===0xffffffff))throw new Error(`th15: inconsistent input context field ${index}`);
      }
      for(const index of [2,3,4,6,7,8,11])if((valid&(1<<(index-2)))&&context[index]>1)
        throw new Error(`th15: invalid input context boolean ${index}`);
      if((valid&8)&&(context[5]>2||(variant==='normal'&&context[5]!==0)||
        (Number.isInteger(result.localSeat)&&context[5]!==result.localSeat)))
        throw new Error('th15: input context belongs to a different local seat');
      const value=index=>valid&(1<<(index-2))?context[index]:null;
      const flag=index=>value(index)===null?null:value(index)===1;
      result.inputContext={version:context[0],validMask:valid,initialized:flag(2),scenePresent:flag(3),
        multiplayer:flag(4),localSeat:value(5),pilotPresent:flag(6),playerPresent:flag(7),ghost:flag(8),
        lifeState:value(9),sceneFlags:value(10),messagesActive:flag(11)};
    }
  }

  if(game==='th20'){
    const ui=wasmArray(nativeExport('th20_ui_observation_v1'),9,Int32Array);
    if(ui){
      if(ui[0]!==1||ui[1]<0||(ui[1]&~3)!==0)throw new Error('th20: unsupported UI observation ABI');
      for(const [bit,indices] of [[1,[2,3,4,5]],[2,[6,7,8]]]){
        if(!(ui[1]&bit)&&indices.some(index=>ui[index]!==-1))throw new Error('th20: invalid UI observation group has data');
      }
      if((ui[1]&1)&&![0,1].includes(ui[5]))throw new Error('th20: invalid retry-confirmation readiness');
      result.ui={version:ui[0],validMask:ui[1],
        pause:ui[1]&1?{state:ui[2],substate:ui[3],cursor:ui[4],retryConfirmInputReady:ui[5]===1}:null,
        stone:ui[1]&2?{state:ui[6],visible:ui[7],category:ui[8]}:null};
    }
  }

  if (!result.capabilities.phase && !unsupported.includes('semantic-phase') && !unsupported.includes('standardized-phase')) unsupported.push('phase');
  if (!result.capabilities.input && !unsupported.some(value => value.includes('input') || value.includes('keyboard'))) unsupported.push('input');
  if (!result.capabilities.position && !unsupported.some(value => value.includes('position'))) unsupported.push('position');
  if (!result.capabilities.pause && !unsupported.includes('pause-state')) unsupported.push('pause-state');
  if (!result.capabilities.presentationFrame && !unsupported.includes('presentation-frame')) unsupported.push('presentation-frame');
  return result;
}

/** Assert only a behavior for which the selected Runtime publishes a suitable observer. */
export function assertRuntimeObservation(observation, behavior, expected = {}) {
  const {game, capabilities} = observation ?? {};
  const missing = (capability, detail = capability) => {
    if (!capabilities?.[capability]) throw new Error(`${game ?? 'runtime'} has no read-only observer for ${detail}`);
  };
  switch (behavior) {
    case 'phase':
      missing('phase');
      assert.equal(observation.phase?.name, expected.name, `${game}: unexpected native phase`);
      break;
    case 'input-held':
      missing('input', 'the requested native input state');
      assert.ok(Number.isFinite(observation.input?.value), `${game}: native input state is unavailable`);
      if (expected.mask !== undefined) assert.equal(observation.input.value & expected.mask, expected.mask,
        `${game}: expected logical input mask was not present`);
      else assert.notEqual(observation.input.value, 0, `${game}: no native input is held`);
      break;
    case 'input-released':
      missing('input', 'the requested native input release');
      assert.ok(Number.isFinite(observation.input?.value), `${game}: native input state is unavailable`);
      if (expected.mask !== undefined) assert.equal(observation.input.value & expected.mask, 0,
        `${game}: native input mask remains held after release`);
      else assert.equal(observation.input.value, expected.value ?? 0, `${game}: native input was not released`);
      break;
    case 'pause-active':
      missing('pause', 'native pause state');
      assert.equal(observation.pause?.active, true, `${game}: native pause state is inactive`);
      if(observation.nativeObservation?.validMask&1)assert.equal(observation.phase?.name,'paused',`${game}: a result or transition screen is not a paused game`);
      break;
    case 'pause-inactive':
      missing('pause', 'native pause state');
      assert.equal(observation.pause?.active, false, `${game}: native pause state remains active`);
      break;
    case 'presentation-advanced':
      missing('presentationFrame', 'a native presentation counter');
      assert.ok(observation.render.presentationFrame > expected.after,
        `${game}: no new native presentation after resize`);
      break;
    case 'position-moved': {
      missing('position', 'native player position');
      const axis = expected.axis;
      assert.ok(axis === 'x' || axis === 'y', 'expected axis must be x or y');
      const before = expected.before?.[axis], after = observation.position?.[axis];
      assert.ok(Number.isFinite(before) && Number.isFinite(after), `${game}: player position is unavailable`);
      const delta = after - before;
      if (expected.direction === 'positive') assert.ok(delta > 0, `${game}: player did not move in positive ${axis}`);
      else if (expected.direction === 'negative') assert.ok(delta < 0, `${game}: player did not move in negative ${axis}`);
      else assert.notEqual(delta, 0, `${game}: player position did not change`);
      if (expected.minDelta !== undefined) assert.ok(Math.abs(delta) >= expected.minDelta,
        `${game}: player movement was below the requested threshold`);
      break;
    }
    case 'frame-advanced':
    case 'frame-frozen': {
      const key = expected.key;
      if (!key || !Object.hasOwn(observation?.frames ?? {}, key)) throw new Error(`${game}: frame ${key ?? '(unspecified)'} is not observable`);
      const value = observation.frames[key], before = expected.before;
      assert.ok(Number.isFinite(value) && Number.isFinite(before), `${game}: native frame is unavailable`);
      if (behavior === 'frame-advanced') assert.ok(value > before, `${game}: ${key} did not advance`);
      else assert.equal(value, before, `${game}: ${key} advanced while the scenario expected it frozen`);
      break;
    }
    case 'restart-frame-reset': {
      const key = capabilities?.restartFrame;
      if (!key || !Object.hasOwn(observation?.frames ?? {}, key)) throw new Error(`${game}: restart frame is not observable`);
      const value = observation.frames[key], before = expected.before;
      assert.ok(Number.isFinite(value) && Number.isFinite(before), `${game}: restart frame is unavailable`);
      assert.ok(value < before, `${game}: observed ${key} did not reset after restart`);
      break;
    }
    default: throw new Error(`Unknown Runtime observation assertion: ${behavior}`);
  }
  return {passed: true, game, behavior};
}
