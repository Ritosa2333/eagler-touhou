// Appended to the frozen smoke module, so these are its native core/Module/app.
// Test instrumentation only; never loaded by the shipped Runtime shell.
const perfWorld=window.multiplayerSmoke;
const perfFixture=typeof core.mp_fixture_performance_mode==='function';
let perfState=null,perfLastCapture=-1;
let perfWallTouch=false,perfTouchEpoch=0,perfTargetFilter=true,perfBroadphase=true,perfBarrierCache=true,perfIdleInput=false;
const perfRead=()=>({net:perfWorld.netStatus(),driver:perfWorld.driverStatus(),
  pacing:core.mp_fixture_pacing?Array.from(new Float64Array(core.memory.buffer,core.mp_fixture_pacing(),3)):null,
  audioServices:perfWorld.audioServices(),audioClock:core.sdl_loop_time(),
  scene:core.status(app,0),loading:core.status(app,7),enemyFrame:core.status(app,6),
  profile:perfFixture?perfWorld.snapshotProfile():Array(10).fill(0),
  chain:perfFixture?Array.from(new Float64Array(core.memory.buffer,core.mp_fixture_chain_profile(),64)):Array(64).fill(0),
  bullet:perfFixture?Array.from(new Float64Array(core.memory.buffer,core.mp_fixture_bullet_profile(),8)):Array(8).fill(0),
  bulletUpdate:perfFixture?Array.from(new Float64Array(core.memory.buffer,core.mp_fixture_bullet_update_profile(),8)):Array(8).fill(0),
  collision:perfFixture?Array.from(new Float64Array(core.memory.buffer,core.mp_fixture_collision_counts(),4)):Array(4).fill(0),
  costs:perfFixture?Array.from(new Float64Array(core.memory.buffer,core.mp_fixture_performance_cost(app),4)):Array(4).fill(0),
  snapshotOwners:perfFixture?Array.from(new Float64Array(core.memory.buffer,core.mp_fixture_snapshot_owners(app),12)):Array(12).fill(0),
  snapshotModes:perfFixture?Array.from(new Uint32Array(core.memory.buffer,core.mp_fixture_snapshot_modes(app),4)):[0,0,0,0],
  graphics:Array.from(new Uint32Array(core.memory.buffer,core.sdl_stats(),17)),textureRestores:perfFixture?Array.from(new Uint32Array(core.memory.buffer,core.mp_fixture_texture_restore_stats(app),3)):null,heap:core.memory.buffer.byteLength});
const perfMainBlocks=()=>{
  if(!perfFixture)return [];
  const memory=new Uint8Array(core.memory.buffer),decode=new TextDecoder();
  return Array.from({length:core.mp_fixture_snapshot_block_count(app)},(_,index)=>{
    const address=core.mp_fixture_snapshot_block_name(app,index);
    let end=address;while(memory[end])++end;
    return {index,name:decode.decode(memory.subarray(address,end)),bytes:core.mp_fixture_snapshot_block_bytes(app,index)};
  }).sort((a,b)=>b.bytes-a.bytes);
};
window.th08Performance={
  bombLazy(value){return perfFixture?core.mp_fixture_bomb_lazy(app,+value):value?2:0;},
  checkpointSpan(span){return perfFixture?core.mp_fixture_checkpoint_span(app,span):span===1?1:0;},
  configure(mode,bullets,limit60=false,wallTouch=false,earlyInput=true,liveBullets=true,targetFilter=true,broadphase=true,cache=true,idleInput=false,backgroundIndex=false,bombLazy=false,shotSparse=false,recordSparse=false,worldInstancing=true,textureCoalesce=true,skipResimVisual=false,skipResimGeometry=false,titleSpellsLazy=true,backMetadataOnly=false){
    if(!perfFixture&&(mode||bullets||!earlyInput||!liveBullets||!targetFilter||!broadphase||!cache))
      throw Error('Production measurement requires unchanged defaults and authored bullets');
    if(perfFixture){
    if(core.mp_fixture_bomb_lazy(app,+bombLazy)!==+bombLazy)throw Error('Bomb journal mode not applied');
    if(core.mp_fixture_shot_sparse(app,+shotSparse)!==+shotSparse)throw Error('Shot journal mode not applied');
    if(core.mp_fixture_record_sparse(app,+recordSparse)!==+recordSparse)throw Error('Record journal mode not applied');
    if(core.mp_fixture_title_spells_lazy(app,+titleSpellsLazy)!==+titleSpellsLazy)throw Error('Title spells journal mode not applied');
    if(core.mp_fixture_world_instancing(+worldInstancing)!==+worldInstancing)throw Error('World instancing mode not applied');
    if(core.mp_fixture_texture_coalesce(+textureCoalesce)!==+textureCoalesce)throw Error('Texture restore mode not applied');
    if(core.mp_fixture_back_metadata_only(app,+backMetadataOnly)!==+backMetadataOnly)throw Error('Backbuffer metadata mode not applied');
    if(core.mp_fixture_skip_resim_visual(app,+skipResimVisual)!==+skipResimVisual)throw Error('Resimulation visual mode not applied');
    if(core.mp_fixture_skip_resim_geometry(app,+skipResimGeometry)!==+skipResimGeometry)throw Error('Resimulation geometry mode not applied');
    if(core.mp_fixture_background_index(+backgroundIndex)!==+backgroundIndex)throw Error('Background index mode not applied');
    if(core.mp_fixture_bullet_backend(app,liveBullets?1:0,0)!==1+Number(liveBullets))throw Error('Bullet backend not applied');
    if(core.mp_fixture_early_input(app,earlyInput?1:0)!==Number(earlyInput))throw Error('Early input mode not applied');
    if(core.mp_fixture_target_filter(targetFilter?1:0)!==Number(targetFilter))throw Error('Target filter not applied');
    if(core.mp_fixture_collision_broadphase(broadphase?1:0)!==Number(broadphase))throw Error('Collision broadphase not applied');
    if(core.mp_fixture_barrier_cache(+cache)!==+cache)throw Error('Barrier cache mode not applied');
    perfBarrierCache=cache;
    if(!core.mp_fixture_collision_probe())throw Error('Collision broadphase oracle failed');
    }
    perfTargetFilter=targetFilter;perfBroadphase=broadphase;
    perfWallTouch=wallTouch;perfIdleInput=idleInput;
    if(wallTouch){perfWorld.touchConfigure(0,false);perfWorld.touchControls(true,false,0,0);}
    Module.eaglerOptions={...Module.eaglerOptions,limitPresentationTo60:limit60};
    if(perfFixture&&!core.mp_fixture_performance_mode(app,mode))throw Error('Performance mode rejected');
    if(bullets&&!perfWorld.denseBullets(bullets))throw Error('Dense workload rejected');
    const cost=perfRead().costs;
    if(!!cost[0]!== (mode>=2)||!!cost[1]!== (mode===1||mode===3))throw Error('Mode not applied');
  },
  start(end,measureStart,seat){
    perfState={end,measureStart,seat,rows:[],issues:[],before:null,after:null,done:false,
      lastReceive:performance.now(),lastReceived:-1,lastConfirm:performance.now(),lastConfirmed:-1,
      touch:{events:0,pathPixels:0,minX:Infinity,maxX:-Infinity}};
    if(perfWallTouch){
      // Reuse TH07 netplay-performance-browser-host.html:startBridgeDrag's
      // wall-clock stimulus: 2.5 s period, 4.5% canvas amplitude, seat phase.
      // TH08-specific seam is the existing native SDL touch entry point.
      const epoch=++perfTouchEpoch,id=991+seat;let active=false,first=0,lastX=0;
      const gesture=()=>{
        if(epoch!==perfTouchEpoch)return;
        const frame=perfWorld.netStatus()[3];
        if(frame>=180&&frame<end-60){
          if(!first)first=performance.now();
          const phase=(performance.now()-first)/2500*Math.PI*2+seat*.8;
          const x=.5+.045*Math.sin(phase);
          perfWorld.touch(active?1:0,id,x,.8);
          if(active)perfState.touch.pathPixels+=Math.abs(x-lastX)*640;
          lastX=x;active=true;++perfState.touch.events;
          const position=perfWorld.status()[8+seat*12+5];
          perfState.touch.minX=Math.min(perfState.touch.minX,position);
          perfState.touch.maxX=Math.max(perfState.touch.maxX,position);
        }else if(active){perfWorld.touch(2,id,lastX,.8);active=false;}
        if(!perfState.done&&!perfState.issues.length)requestAnimationFrame(gesture);
      };
      requestAnimationFrame(gesture);
    }
    Module.runtimePrepare=()=>{
      try{
        const frame=perfWorld.netStatus()[3];
        if(frame>=end){
          perfWorld.pollNetwork();perfWorld.reconcile();
          const n=perfWorld.netStatus();
          if(n[3]===end&&n[5]>=end-1){perfState.done=true;perfState.after=perfRead();}
          return 0;
        }
        if(frame>perfLastCapture&&!(perfWallTouch&&frame>=180)){
          // Constant tick workload across policies; no frame-rate-dependent
          // input and no duplicate sample while admission is waiting.
          const moving=!perfIdleInput&&frame>=180;
          const x=moving?Math.sin(frame/31+seat)*0.19:0;
          const ok=frame===0&&seat===0?perfWorld.captureInput(frame,0,2,8188,-8192,0):
            perfWorld.captureInput(frame,moving?1:0,2,x,0,moving?2:0);
          if(!ok)throw Error('Capture rejected '+frame);
          if(perfFixture&&!core.mp_fixture_network_capture(app,frame))throw Error('Captured input announcement rejected '+frame);
          perfLastCapture=frame;
        }
        return 1;
      }catch(error){perfState.issues.push(String(error));core.sdl_loop_stop();return 0;}
    };
    Module.runtimeFinish=(code,ms)=>{
      const now=performance.now(),sample=perfRead(),frame=sample.net[3];
      if(code){perfState.issues.push('Native loop '+code+' '+perfWorld.networkError());core.sdl_loop_stop();}
      if(sample.driver[14]!==perfState.lastReceived){perfState.lastReceived=sample.driver[14];perfState.lastReceive=now;}
      if(sample.net[5]!==perfState.lastConfirmed){perfState.lastConfirmed=sample.net[5];perfState.lastConfirm=now;}
      if(frame<measureStart||frame>end)return;
      if(!perfState.before)perfState.before={time:now,...sample,impairment:{...globalThis.__th08Impairment.stats}};
      perfState.rows.push({time:now,ms,receiveAge:now-perfState.lastReceive,confirmedAge:now-perfState.lastConfirm,...sample});
    };
    core.sdl_loop_start();
  },
  stop(){core.sdl_loop_stop();++perfTouchEpoch;if(perfWallTouch)perfWorld.touchCancel();},
  status(){return {done:perfState?.done,issues:perfState?.issues,net:perfWorld.netStatus()};},
  report(){return {...perfState,canonical:perfWorld.canonical(),state:perfWorld.status(),
    mainBlocks:perfMainBlocks(),
    profileAvailable:perfFixture,timeOrigin:performance.timeOrigin,
    identity:perfWorld.identity(),limit60:!!Module.eaglerOptions?.limitPresentationTo60,wallClockTouch:perfWallTouch,
    earlyInput:perfFixture?!!core.mp_fixture_early_input(app,2):true,
    targetFilter:perfTargetFilter,broadphase:perfBroadphase,barrierCache:perfBarrierCache,
    liveBullets:perfFixture?(core.mp_fixture_bullet_backend(app,2,0)&3)===2:true,
    impairment:{...globalThis.__th08Impairment.stats}};},
};
