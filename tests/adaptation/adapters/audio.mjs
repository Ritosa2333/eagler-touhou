import {ADAPTATION_ADAPTERS,resolveAdaptationAdapter} from './registry.mjs';

/** Compatibility export for the source-derived th08 verifier. The declaration has one title owner. */
export const TH08_NORMAL_SFX_PROBE=resolveAdaptationAdapter('th08','normal').audioLatency;

/** Products with a registered, production-anchored SFX latency scene. */
export const SFX_LATENCY_PRODUCTS=Object.freeze(ADAPTATION_ADAPTERS.products.filter(({game,variant})=>
  resolveAdaptationAdapter(game,variant).audioLatency));

/** Return only an explicitly implemented product contract; unsupported variants stay unregistered. */
export function resolveSfxLatencyContract(game,variant){
  if(!['normal','multiplayer'].includes(variant))return null;
  return resolveAdaptationAdapter(game,variant)?.audioLatency??null;
}

export function deriveSfxLatencyContract(game,variant,sources){
  if(!resolveSfxLatencyContract(game,variant))throw new RangeError(`No SFX latency adapter for ${game}/${variant}`);
  const derive=resolveAdaptationAdapter(game,variant)?.deriveSfxContract;
  if(typeof derive!=='function')throw new RangeError(`Register the independent production sound mapping derivation for ${game}/${variant}`);
  return derive(sources);
}

/** Resolve scene roles to stable report IDs and one production target identity. */
export function resolveSfxLatencyScenes(contract){
  if(!contract||!Array.isArray(contract.scenes)||contract.scenes.length!==2)
    throw new TypeError('SFX latency needs exactly one positive and one negative-control scene');
  const positive=contract.scenes.filter(scene=>scene?.role==='positive');
  const negative=contract.scenes.filter(scene=>scene?.role==='negative-control');
  if(positive.length!==1||negative.length!==1)throw new TypeError('SFX latency scene roles must contain one positive and one negative-control');
  if(positive[0].sameTargetVoice!==true||negative[0].sameTargetVoice!==true)
    throw new TypeError('SFX latency scenes must identify the same production target voice');
  if(positive[0].mutation!==undefined)throw new TypeError('The positive SFX latency scene cannot mutate its target voice');
  if(negative[0].mutation!=='measured-queue-tail')
    throw new TypeError('The negative-control SFX latency scene must apply measured-queue-tail');
  if(!Number.isInteger(contract.logicalIndex)||!Number.isInteger(contract.handle)||
     !Array.isArray(contract.targetHandles)||!contract.targetHandles.includes(contract.handle))
    throw new TypeError('SFX latency scenes must share the mapped production logical sound and voice handle');
  const targetVoice={game:contract.game,variant:contract.variant,logicalIndex:contract.logicalIndex,handle:contract.handle};
  return Object.freeze({
    positive:Object.freeze({...positive[0],checkId:'native-sfx-before-mixed-queue-tail',targetVoice}),
    negative:Object.freeze({...negative[0],checkId:'negative-control-sfx-delayed-to-mixed-queue-tail',targetVoice}),
  });
}

export function sfxLatencySceneRequest({game,variant}){
  const adapter=resolveAdaptationAdapter(game,variant);
  return{game,variant,contract:adapter?.audioLatency?{runtime:adapter.runtime,state:adapter.audioLatency.state}:null};
}

/** Read only the title-owned navigation fields. Serializable for frame.evaluate. */
export function readSfxLatencySceneState({game,variant,contract}){
  if(!contract)return {available:false,reason:`No native audio scene adapter for ${game}/${variant}`};
  const mapping=contract.runtime,runtime=globalThis[mapping.global],module=runtime?.[mapping.module];
  const core=runtime?.[mapping.core],status=core?.[contract.state.export]??module?.[`_${contract.state.export}`];
  const app=runtime?.[mapping.app];
  if(typeof status!=='function'||app==null)return{available:false,reason:'Native title status is unavailable'};
  const values=Object.fromEntries(Object.entries(contract.state.fields).map(([name,index])=>[name,status(app,index)]));
  if(Object.values(values).some(value=>!Number.isFinite(value)))throw Error('Native title status returned an invalid observation');
  if(![0,1].includes(values.inputReady))throw Error('Native audio input readiness must be 0 or 1');
  return{available:true,...values,activeApp:contract.state.activeApps[values.activeApp]??String(values.activeApp),
    bridgeActive:globalThis.__eaglerKeyboardInputBridge?.active?.()===true,
    audioState:module?.SDL3?.audioContext?.state??null};
}

export {deriveTh08NormalSfxContract} from './titles/th08.mjs';
