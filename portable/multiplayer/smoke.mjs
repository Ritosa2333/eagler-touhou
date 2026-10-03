import createModule from '/th08-sdl.mjs';

let core;
const buildIdentity=await fetch('/build.json').then(response=>{if(!response.ok)throw Error('Missing Runtime identity');return response.json();});
const fixtureBuildWords=Array.from({length:4},(_,i)=>Number.parseInt(buildIdentity.wasm.slice(i*8,i*8+8),16)>>>0);
let instantiatedSha256='';
const Module=await createModule({canvas:document.getElementById('screen'),noInitialRun:true,
  async instantiateWasm(imports,ready){
    const response=await fetch('/th08-sdl.wasm');if(!response.ok)throw Error('Missing Runtime WASM');
    const bytes=await response.arrayBuffer();instantiatedSha256=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),byte=>byte.toString(16).padStart(2,'0')).join('');
    if(instantiatedSha256!==buildIdentity.wasm)throw Error('Mixed Runtime identity');
    const {instance,module}=await WebAssembly.instantiate(bytes,imports);core=instance.exports;ready(instance,module);return core;
  }
});
if(core.sdl_files_variant(1)!==1)throw Error('Ordinary runtime rejected');
for(const path of ['/game','/fonts','/savesth08-multiplayer/replay'])Module.FS.mkdirTree(path);
for(const path of ['/input/th08.dat','/fonts/msgothic.ttc','/fonts/blend.bin','/fonts/cp932.bin']){
  const response=await fetch(path);if(!response.ok)throw Error(path);
  Module.FS.writeFile(path==='/input/th08.dat'?'/game/th08.dat':path,new Uint8Array(await response.arrayBuffer()));
}
function string(value,callback){const bytes=new TextEncoder().encode(value+'\0'),ptr=core.allocate(bytes.length);try{new Uint8Array(core.memory.buffer,ptr,bytes.length).set(bytes);return callback(ptr);}finally{core.deallocate(ptr);}}
string('#screen',core.sdl_canvas);core.sdl_music_enabled(0);
let app=0;
const nativeStatus=()=>Array.from(new Int32Array(core.memory.buffer,core.sdl_game_status(),10));
window.multiplayerSmoke={
  identity(){return {...buildIdentity,wasmSha256:instantiatedSha256,fixtureBuild:typeof core.mp_fixture_die==='function'};},
  traceEnable(frame){if(!core.multiplayer_resource_trace_enable?.(app,frame))throw Error('Trace enable rejected');},
  deathField(victim,bombs=3,scenario=0){if(!core.mp_fixture_death_field?.(app,victim,bombs,scenario))throw Error('Death field setup rejected');},
  traceRead(frame,detail=false){const p=core.multiplayer_resource_trace_read(app,frame,detail?1:0);const heap=new Uint8Array(core.memory.buffer);let end=p;while(heap[end])++end;return JSON.parse(new TextDecoder().decode(heap.subarray(p,end)));},
  async start(seed=1234,loadouts=null,local=0,sessionId=null,configuration=null,difficulty=1){
    if(app)throw Error('Use a fresh page for a new run');
    if(configuration){
      const bytes=new Uint8Array(60),v=new DataView(bytes.buffer);
      [0,1,2,4,-1,-1,-1,-1,3].forEach((n,i)=>v.setInt16(i*2,n,true));
      v.setUint32(20,0x80001,true);v.setInt16(24,600,true);v.setInt16(26,600,true);
      bytes[28]=2;bytes[29]=3;bytes[31]=2;bytes[32]=1;bytes[33]=1;bytes[36]=2;bytes[39]=100;bytes[40]=80;
      for(const [field,offset] of Object.entries({lives:28,frameskip:35,effects:36,slowMode:37,shotSlow:38}))
        if(configuration[field]!==undefined)bytes[offset]=configuration[field];
      if(configuration.options!==undefined)v.setUint32(56,configuration.options,true);
      Module.FS.writeFile('/savesth08-multiplayer/th08.cfg',bytes);
    }
    app=core.sdl_game_open(seed);if(!app)throw Error('Native app creation failed');
    if(loadouts){
      const words=[sessionId?3:1,loadouts.length,local,difficulty,seed,...(sessionId||[]),...(sessionId?fixtureBuildWords:[]),...loadouts.flatMap(character=>[character,0])];
      const count=sessionId?17:11;while(words.length<count)words.push(0);
      const ptr=core.allocate(count*4);try{
        new Uint32Array(core.memory.buffer,ptr,count).set(words);
        if(!core.multiplayer_configure(app,ptr,count))throw Error('Session rejected');
      }finally{core.deallocate(ptr);}
      if(!sessionId&&core.mp_fixture_local_route&&!core.mp_fixture_local_route(app,0))throw Error('Local fixture route rejected');
    }
    const total=core.sdl_prepare_total();
    for(let i=0;i<total;i++){
      window.__th08TestBoot={phase:'prepare',step:i,total};
      if(core.sdl_prepare_next()<0)throw Error('Prepare failed '+i);
      // Match the real Runtime shell: resource preparation is not a logical
      // gameplay frame, and must allow browser/GPU work to drain between lots.
      if(i%12===11)await new Promise(resolve=>setTimeout(resolve,0));
    }
    window.__th08TestBoot={phase:'initialize',step:total,total};
    if(!core.sdl_game_initialize())throw Error('Initialize failed');
    window.__th08TestBoot={phase:'ready',step:total,total};
    return nativeStatus();
  },
  ticks(count){for(let i=0;i<count;++i){const result=core.sdl_loop_tick(app,1/60,16);if(result)throw Error('Native tick failed '+result+' '+this.networkError()+' '+JSON.stringify(Array.from(new Int32Array(core.memory.buffer,core.diagnostics(app),16))));}return nativeStatus();},
  key(code,down){string(code,p=>core.sdl_key(p,down?1:0));},
  commit(buttons){
    const ptr=core.allocate(buttons.length*2);
    try{
      new Uint16Array(core.memory.buffer,ptr,buttons.length).set(buttons);
      if(!core.multiplayer_commit_inputs(app,ptr,buttons.length))return false;
      // Offline fixtures still give the local seat its physical input path.
      // Committed peer inputs do not override the local keyboard sample.
      for(const [mask,code] of [[1,'KeyZ'],[2,'KeyX'],[4,'ShiftLeft'],[16,'ArrowUp'],[32,'ArrowDown'],[64,'ArrowLeft'],[128,'ArrowRight']])
        this.key(code,(buttons[0]&mask)!==0);
      return true;
    }
    finally{core.deallocate(ptr);}
  },
  nativeStatus,
  stageClearSetup(){return !!core.mp_fixture_stage_clear_setup?.(app);},
  denseBullets(count){return !!core.mp_fixture_dense_bullets?.(app,count);},
  optimizationMode(target,broadphase,cache=true){return core.mp_fixture_target_filter(+target)===+target&&core.mp_fixture_collision_broadphase(+broadphase)===+broadphase&&core.mp_fixture_barrier_cache(+cache)===+cache;},
  chainProfile(){return Array.from(new Float64Array(core.memory.buffer,core.mp_fixture_chain_profile(),64));},
  bulletUpdateProfile(){return Array.from(new Float64Array(core.memory.buffer,core.mp_fixture_bullet_update_profile(),8));},
  collisionProbe(){return !!core.mp_fixture_collision_probe();},
  bulletBackend(mode=2,audit=false){return core.mp_fixture_bullet_backend(app,mode,audit?1:0);},
  bombLazy(enabled){return core.mp_fixture_bomb_lazy?.(app,+enabled);},
  shotSparse(enabled){return core.mp_fixture_shot_sparse?.(app,+enabled);},
  recordSparse(enabled){return core.mp_fixture_record_sparse?.(app,+enabled);},
  titleSpellsLazy(enabled){return core.mp_fixture_title_spells_lazy?.(app,+enabled);},
  projectionReuse(enabled){return core.mp_fixture_projection_reuse?.(+enabled);},
  backMetadataOnly(enabled){return core.mp_fixture_back_metadata_only?.(app,+enabled);},
  backMetadataState(){return core.mp_fixture_back_metadata_state?.(app)??0;},
  snapshotModes(){const ptr=core.mp_fixture_snapshot_modes?.(app);return ptr?Array.from(new Uint32Array(core.memory.buffer,ptr,4)):null;},
  spellRecordProbe(){const ptr=core.mp_fixture_spell_record_probe?.(app);return ptr?Array.from(new Uint32Array(core.memory.buffer,ptr,8)):null;},
  titleSpellsProbe(){const ptr=core.mp_fixture_title_spells_probe?.(app);return ptr?Array.from(new Uint32Array(core.memory.buffer,ptr,4)):null;},
  liveBulletProbe(){const p=core.mp_fixture_live_bullet_probe(app);return Array.from(new Uint32Array(core.memory.buffer,p,10));},
  snapshotProfile(policy=2){const p=core.mp_fixture_snapshot_profile(app,policy);return Array.from(new Float64Array(core.memory.buffer,p,10));},
  audioRouting(){if(!core.mp_fixture_audio_routing||!app)throw Error('Initialized diagnostic fixture required');return !!core.mp_fixture_audio_routing(app);},
  audioClockIndependent(){if(!core.mp_fixture_audio_clock||!app)throw Error('Initialized diagnostic fixture required');return !!core.mp_fixture_audio_clock(app);},
  correctionProbe(){if(!core.mp_fixture_native_correction||!app)throw Error('Initialized diagnostic fixture required');
    const ptr=core.mp_fixture_native_correction(app);return Array.from(new Uint32Array(core.memory.buffer,ptr,64));},
  enemyJournalProbe(){if(!core.mp_fixture_enemy_journal)throw Error('Diagnostic fixture required');
    const ptr=core.mp_fixture_enemy_journal();return Array.from(new Uint32Array(core.memory.buffer,ptr,10));},
  screenJournalProbe(){if(!core.mp_fixture_screen_journal||!app)throw Error('Initialized diagnostic fixture required');
    const ptr=core.mp_fixture_screen_journal(app);return Array.from(new Uint32Array(core.memory.buffer,ptr,10));},
  poolsJournalProbe(){if(!core.mp_fixture_pools_journal||!app)throw Error('Gameplay diagnostic fixture required');
    const ptr=core.mp_fixture_pools_journal(app);if(!ptr)throw Error('Native game is not ready');
    return Array.from(new Uint32Array(core.memory.buffer,ptr,10));},
  resourcesJournalProbe(){if(!core.mp_fixture_resources_journal||!app)throw Error('Initialized diagnostic fixture required');
    const ptr=core.mp_fixture_resources_journal(app);return Array.from(new Uint32Array(core.memory.buffer,ptr,10));},
  updateOptimizationProbe(mode=0){if(!core.mp_fixture_update_optimizations||!app)throw Error('Gameplay diagnostic fixture required');
    const ptr=core.mp_fixture_update_optimizations(app,mode);return Array.from(new Uint32Array(core.memory.buffer,ptr,64));},
  worldJournalProbe(mode=0){if(!core.mp_fixture_world_journal||!app)throw Error('Gameplay diagnostic fixture required');
    const ptr=core.mp_fixture_world_journal(app,mode);return Array.from(new Uint32Array(core.memory.buffer,ptr,64));},
  netStatus(){return Array.from(new Uint32Array(core.memory.buffer,core.multiplayer_netplay_status(app),12));},
  driverStatus(){const p=core.multiplayer_driver_status(app);return Array.from(new Uint32Array(core.memory.buffer,p,16));},
  canonical(){const p=core.multiplayer_canonical_state(app);return Array.from(new Uint32Array(core.memory.buffer,p,13));},
  rngState(){if(!core.mp_fixture_rng_state)return null;
    const p=core.mp_fixture_rng_state(app);return Array.from(new Uint32Array(core.memory.buffer,p,4));},
  backgroundIndex(enabled){return core.mp_fixture_background_index?.(enabled?1:0)??0;},
  textureCoalesce(enabled){return core.mp_fixture_texture_coalesce?.(enabled?1:0)??0;},
  worldInstancing(enabled){return core.mp_fixture_world_instancing?.(enabled?1:0)??0;},
  skipResimVisual(enabled){return core.mp_fixture_skip_resim_visual?.(app,enabled?1:0)??2;},
  skipResimGeometry(enabled){return core.mp_fixture_skip_resim_geometry?.(app,enabled?1:0)??2;},
  boundaryVisualRedraws(){return core.mp_fixture_boundary_visual_redraws?.(app)??0;},
  textureRestore(){return !!core.mp_fixture_texture_restore?.(app);},
  gpuHistory(){return !!core.mp_fixture_gpu_history?.(app);},
  presented(alpha){if(!core.mp_fixture_presentation_frame?.(app,alpha))throw Error('Native presentation failed');return true;},
  enemyDrawPurity(alpha){const p=core.mp_fixture_enemy_draw_purity(app,alpha);return Array.from(new Uint32Array(core.memory.buffer,p,128));},
  enemyCanonical(index){const p=core.mp_fixture_enemy_canonical(app,index),header=Array.from(new Uint32Array(core.memory.buffer,p,16));return {header,bytes:Array.from(new Uint8Array(core.memory.buffer,p+64,header[1]))};},
  audioServices(){return core.mp_fixture_audio_service_calls?.()??null;},
  async frameDigest(){
    const p=core.sdl_read_back(),bytes=new Uint8Array(core.memory.buffer,p,640*480*4),crop=new Uint8Array(384*448*4);
    for(let y=0;y<448;y++)crop.set(bytes.subarray(((y+16)*640+32)*4,((y+16)*640+416)*4),y*384*4);
    return Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',crop)),v=>v.toString(16).padStart(2,'0')).join('');
  },
  networkError(){const p=core.multiplayer_network_error(app);let end=p;const bytes=new Uint8Array(core.memory.buffer);while(bytes[end])++end;return new TextDecoder().decode(bytes.subarray(p,end));},
  connect(relay){return string(relay,p=>!!core.multiplayer_connect(app,p));},
  pollNetwork(){const result=!!core.multiplayer_network_poll(app);if(!result)throw Error(this.networkError());return true;},
  reconcile(){if(!core.multiplayer_reconcile(app))throw Error(this.networkError());return this.netStatus();},
  sessionPacket(phase){const p=core.allocate(128);try{
    const size=core.multiplayer_session_build(app,phase,p,128);if(!size)throw Error('Session packet unavailable');
    return Array.from(new Uint8Array(core.memory.buffer,p,size));
  }finally{core.deallocate(p);}},
  applyWire(bytes){const p=core.allocate(bytes.length);try{
    new Uint8Array(core.memory.buffer,p,bytes.length).set(bytes);return core.multiplayer_wire_apply(app,p,bytes.length);
  }finally{core.deallocate(p);}},
  markReady(){return !!core.multiplayer_session_ready(app);},
  capture(frame,buttons){return !!core.multiplayer_capture_local(app,frame,buttons);},
  captureInput(frame,buttons,mode=0,x=0,y=0,flags=0){return !!core.multiplayer_capture_input(app,frame,buttons,mode,x,y,flags);},
  touchConfigure(mode=0,unlimited=false){core.sdl_touch_options(1,unlimited?1:0,1);core.sdl_touch_mode(mode);core.sdl_touch_gestures(1,0);core.sdl_touch_controls(0,0,0,0,0,0);},
  touch(type,id,x,y){core.sdl_touch(type,id,x,y);},
  touchControls(shoot,focus,bomb,escape,x=0,y=0){core.sdl_touch_controls(shoot,focus,bomb,escape,x,y);},
  touchCancel(){core.sdl_touch_cancel();},
  practiceWriteProbe(){core.practice_enable(app,1);const accepted=!!core.practice_cheats(app,1),p=core.practice_status(app);return {accepted,state:Array.from(new Int32Array(core.memory.buffer,p,7))};},
  inputPacket(peer,frame,sequence=1){const p=core.allocate(2048);try{
    const size=core.multiplayer_input_build(app,peer,frame,sequence,p,2048);if(!size)throw Error('Input packet unavailable');
    return Array.from(new Uint8Array(core.memory.buffer,p,size));
  }finally{core.deallocate(p);}},
  status(){return Array.from(new Int32Array(core.memory.buffer,core.multiplayer_status(app),44));},
  fixtureDie(seat){if(!core.mp_fixture_die)throw Error('Not a fixture build');return core.mp_fixture_die(app,seat);},
  fixtureOrdinaryDeathSetup(seat){if(!core.mp_fixture_ordinary_death_setup)throw Error('Not a fixture build');return !!core.mp_fixture_ordinary_death_setup(app,seat);},
  fixtureGrazedBulletHit(seat){if(!core.mp_fixture_grazed_bullet_hit)throw Error('Not a fixture build');return !!core.mp_fixture_grazed_bullet_hit(app,seat);},
  fixtureCancelRewardOwner(){if(!core.mp_fixture_cancel_reward_owner)throw Error('Not a fixture build');return !!core.mp_fixture_cancel_reward_owner(app);},
  fixturePlace(seat,x,y,dx=1,dy=1){if(!core.mp_fixture_place)throw Error('Not a fixture build');return core.mp_fixture_place(app,seat,x,y,dx,dy);},
  fixturePower(seat,power){if(!core.mp_fixture_power)throw Error('Not a fixture build');return core.mp_fixture_power(app,seat,power);},
  fixtureItems(kind,seat=0){if(!core.mp_fixture_items)throw Error('Not a fixture build');return core.mp_fixture_items(app,kind,seat);},
  fixtureNativeBombs(seat){return core.mp_fixture_native_bombs(app,seat);},
  fixtureItemStatus(){if(!core.mp_fixture_item_status)throw Error('Not a fixture build');return Array.from(new Uint32Array(core.memory.buffer,core.mp_fixture_item_status(app),9));},
  fixturePowerDrops(kind,mode){const pointer=core.mp_fixture_power_drops(app,kind,mode);return Array.from(new Int32Array(core.memory.buffer,pointer,24));},
  fixturePocSetup(seat){if(!core.mp_fixture_poc_setup)throw Error('Not a fixture build');return !!core.mp_fixture_poc_setup(app,seat);},
  fixturePocCrossingSetup(seat){if(!core.mp_fixture_poc_crossing_setup)throw Error('Not a fixture build');return !!core.mp_fixture_poc_crossing_setup(app,seat);},
  fixturePowerTypeGuard(){if(!core.mp_fixture_power_type_guard)throw Error('Not a fixture build');return !!core.mp_fixture_power_type_guard(app);},
  fixtureStatus(){if(!core.mp_fixture_status)throw Error('Not a fixture build');return Array.from(new Int32Array(core.memory.buffer,core.mp_fixture_status(app),20));},
  fixtureRouteHistory(route){if(!core.mp_fixture_route_history)throw Error('Not a fixture build');return !!core.mp_fixture_route_history(app,route);},
  diagnostics(){return Array.from(new Int32Array(core.memory.buffer,core.diagnostics(app),16));},
  productHUD(){if(!core.mp_fixture_product_hud)throw Error('Product fixture required');return Array.from(new Float32Array(core.memory.buffer,core.mp_fixture_product_hud(app),160));},
  productGauge(viewer){return !!core.mp_fixture_product_gauge?.(app,viewer);},
  productGaugePure(){return !!core.mp_fixture_product_gauge_purity?.(app);},
  productGuestTarget(){return !!core.mp_fixture_product_guest_target?.(app);},
  productPresentationClock(){return !!core.mp_fixture_product_presentation_clock?.(app);},
};
