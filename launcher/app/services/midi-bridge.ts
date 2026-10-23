import type {MidiSynth, MidiSynthConstructor} from '../../src/launcher/app-types.mts';
import type {MusicMode} from '../../src/launcher/game-preferences.mts';
import type {ExternalMidiController} from './external-midi';
import type {RuntimeService, RuntimeMidiEventContext} from './runtime';

export interface MidiBridgeOptions {
  runtime: Pick<RuntimeService, 'getMidiEventContext' | 'getSnapshot' | 'subscribe'>;
  external: Pick<ExternalMidiController, 'prepare' | 'deliver' | 'panic'>;
  loadSynth(): Promise<MidiSynthConstructor | undefined>;
  webAudioAvailable: boolean;
  translate(key: string): string;
  frame: EventTarget;
  window: EventTarget;
  document: EventTarget & {readonly hidden: boolean; hasFocus(): boolean; readonly activeElement: unknown};
  playerOpen(): boolean;
}
type TinySynthNote = (time: number, channel: number, note: number, velocity: number, partials: unknown[]) => unknown;
type TinySynthAudioGraph = MidiSynth & {conv?: ConvolverNode; out?: AudioNode; rev?: GainNode; comp?: AudioNode;
  _note?: TinySynthNote};
type OwnedAudioSource = {
  source: AudioScheduledSourceNode;
  sourceId: number;
  channel: number;
  note: number;
  velocity: number;
  nodeType: string;
  startCount: number;
  startTime: number | null;
  stopCount: number;
  stopRequestedAt: number | null;
  stopTime: number | null;
  stopResetGeneration: number | null;
  resetStopCount: number;
  ended: boolean;
  endedAt: number | null;
  stopError: string | null;
};
function sameContext(a: RuntimeMidiEventContext | null, b: RuntimeMidiEventContext | null): boolean {
  return a === b || !!a && !!b && a.epoch === b.epoch && a.game === b.game && a.document === b.document && a.target === b.target;
}
/** One retained TinySynth and the existing external MIDI owner. The native
 * event bridge reauthenticates epoch/document on every event, even before a
 * pending iframe-load notification can trigger rebinding. */
export function createMidiBridge(options: MidiBridgeOptions) {
  let synth: MidiSynth | null = null;
  let loading: Promise<MidiSynth | null> | null = null;
  let bound: RuntimeMidiEventContext | null = null;
  let removeBound: (() => void) | null = null;
  let disposed = false;
  const ownedAudioSources = new Set<OwnedAudioSource>();
  const audioSourceHistory: OwnedAudioSource[] = [];
  const audioSourceResetHistory:Array<{generation:number;contextTime:number;sourceRecords:OwnedAudioSource[];sourceOmissions:number}> = [];
  const audioSourceTrackingErrors: string[] = [];
  const audioSourceHistoryLimit = 64, audioSourceSnapshotLimit = 128;
  let audioSourceHistoryDrops = 0, audioSourceCount = 0, audioResetGeneration = 0, audioResetInProgress = false;
  const resetSynth = () => {synth?.reset();};
  const audioSourceSnapshot = () => {
    const describe = (record: OwnedAudioSource) => ({sourceId:record.sourceId,channel:record.channel,note:record.note,
      velocity:record.velocity,nodeType:record.nodeType,startCount:record.startCount,startTime:record.startTime,
      stopCount:record.stopCount,stopRequestedAt:record.stopRequestedAt,stopTime:record.stopTime,
      stopResetGeneration:record.stopResetGeneration,resetStopCount:record.resetStopCount,ended:record.ended,
      endedAt:record.endedAt,stopError:record.stopError});
    const active=[...ownedAudioSources];
    return {trackingInstalled:audioSourceTrackingErrors.length===0,sourceCount:audioSourceCount,activeCount:active.length,
      activeSourceOmissions:Math.max(0,active.length-audioSourceSnapshotLimit),historyLimit:audioSourceHistoryLimit,
      historyDrops:audioSourceHistoryDrops,trackingErrors:audioSourceTrackingErrors.slice(),
      activeSources:active.slice(-audioSourceSnapshotLimit).map(describe),
      recentSources:audioSourceHistory.map(describe),
      resetHistory:audioSourceResetHistory.map(reset=>({generation:reset.generation,contextTime:reset.contextTime,
        sourceCount:reset.sourceRecords.length+reset.sourceOmissions,sourceOmissions:reset.sourceOmissions,
        sources:reset.sourceRecords.map(describe)}))};
  };
  function trackAudioSource(source: AudioScheduledSourceNode, context: AudioContext, channel: number, note: number, velocity: number,
    factoryName: string) {
    const record: OwnedAudioSource = {source,sourceId:++audioSourceCount,channel,note,velocity,
      nodeType:source.constructor?.name??factoryName,startCount:0,startTime:null,stopCount:0,stopRequestedAt:null,stopTime:null,
      stopResetGeneration:null,resetStopCount:0,ended:false,endedAt:null,stopError:null};
    ownedAudioSources.add(record);
    if(audioSourceHistory.length===audioSourceHistoryLimit){audioSourceHistory.shift();audioSourceHistoryDrops++;}
    audioSourceHistory.push(record);
    const originalStart=source.start.bind(source),originalStop=source.stop.bind(source);
    try{source.start=function(...args:number[]){record.startCount++;record.startTime=Number.isFinite(args[0])?args[0]:context.currentTime;
      return originalStart(...args);};}
    catch(error){audioSourceTrackingErrors.push(`source ${record.sourceId} start observer: ${String(error)}`);}
    try{source.stop=function(...args:number[]){record.stopCount++;record.stopRequestedAt=context.currentTime;
      record.stopTime=Number.isFinite(args[0])?args[0]:context.currentTime;
      record.stopResetGeneration=audioResetInProgress?audioResetGeneration:null;
      if(audioResetInProgress)record.resetStopCount++;
      try{return originalStop(...args);}catch(error){record.stopError=String(error);throw error;}};}
    catch(error){audioSourceTrackingErrors.push(`source ${record.sourceId} stop observer: ${String(error)}`);}
    try{source.addEventListener('ended',()=>{record.ended=true;record.endedAt=context.currentTime;ownedAudioSources.delete(record);},{once:true});}
    catch(error){audioSourceTrackingErrors.push(`source ${record.sourceId} ended observer: ${String(error)}`);}
  }
  function installAudioSourceTracking(instance: MidiSynth) {
    const graph=instance as TinySynthAudioGraph,context=instance.getAudioContext(),note=graph._note;
    if(typeof note!=='function'){
      audioSourceTrackingErrors.push('TinySynth _note source factory is unavailable');
      return;
    }
    graph._note=function(time,channel,noteNumber,velocity,partials){
      const restorers:Array<()=>void>=[];
      let factoriesReady=true;
      for(const factoryName of ['createOscillator','createBufferSource'] as const){
        const original=context[factoryName],descriptor=Object.getOwnPropertyDescriptor(context,factoryName);
        try{
          Object.defineProperty(context,factoryName,{configurable:true,enumerable:descriptor?.enumerable??false,writable:true,
            value:function(this:AudioContext,...args:unknown[]){
              const source=Reflect.apply(original,context,args) as AudioScheduledSourceNode;
              trackAudioSource(source,context,channel,noteNumber,velocity,factoryName);
              return source;
            }});
          restorers.push(()=>{if(descriptor)Object.defineProperty(context,factoryName,descriptor);else delete (context as unknown as Record<string,unknown>)[factoryName];});
        }catch(error){factoriesReady=false;audioSourceTrackingErrors.push(`${factoryName} observer: ${String(error)}`);break;}
      }
      if(!factoriesReady){while(restorers.length)restorers.pop()?.();return note.call(graph,time,channel,noteNumber,velocity,partials);}
      try{return note.call(graph,time,channel,noteNumber,velocity,partials);}
      finally{while(restorers.length)restorers.pop()?.();}
    };
  }
  function installImmediateAudioStop(instance: MidiSynth) {
    const reset = instance.reset.bind(instance);
    instance.reset = () => {
      audioResetGeneration++;audioResetInProgress=true;
      const context = instance.getAudioContext(),sourceRecordsAtReset=[...ownedAudioSources],resetTime=context.currentTime;
      try{reset();}
      finally{
        for(const record of ownedAudioSources){
          if(record.ended||record.stopTime!==null&&record.stopTime<=context.currentTime)continue;
          try{record.source.stop();}catch(error){record.stopError=String(error);}
        }
        audioResetInProgress=false;
        audioSourceResetHistory.push({generation:audioResetGeneration,contextTime:resetTime,
          sourceRecords:sourceRecordsAtReset.slice(-audioSourceSnapshotLimit),
          sourceOmissions:Math.max(0,sourceRecordsAtReset.length-audioSourceSnapshotLimit)});
        if(audioSourceResetHistory.length>8)audioSourceResetHistory.shift();
        const graph = instance as TinySynthAudioGraph;
        const oldConvolver = graph.conv, output = graph.out, oldReverb = graph.rev, compressor = graph.comp;
        const impulse = oldConvolver?.buffer;
        if(oldConvolver&&output&&oldReverb&&compressor&&impulse){
          const replacementConvolver = context.createConvolver();
          const replacementReverb = context.createGain();
          replacementConvolver.normalize = oldConvolver.normalize;
          replacementConvolver.buffer = impulse;
          replacementReverb.gain.value = oldReverb.gain.value;
          output.disconnect(oldConvolver);
          oldConvolver.disconnect();
          oldReverb.disconnect();
          output.connect(replacementConvolver);
          replacementConvolver.connect(replacementReverb);
          replacementReverb.connect(compressor);
          graph.conv = replacementConvolver;
          graph.rev = replacementReverb;
        }
      }
    };
  }
  function installSynth(instance: MidiSynth): MidiSynth {
    installAudioSourceTracking(instance);
    installImmediateAudioStop(instance);
    return instance;
  }
  function reset() {resetSynth(); options.external.panic();}
  function authenticated(context: RuntimeMidiEventContext) {
    return !disposed && sameContext(context, options.runtime.getMidiEventContext());
  }
  function rebind() {
    if (disposed) return;
    const next = options.runtime.getMidiEventContext();
    if (sameContext(bound, next)) return;
    removeBound?.(); removeBound = null;
    if (bound) reset();
    bound = next;
    if (!next) return;
    const onMidi: EventListener = event => {
      if (!authenticated(next)) return;
      const current = options.runtime.getMidiEventContext();
      if (current?.music !== 'midi' && current?.music !== 'ogg') return;
      const detail: unknown = (event as CustomEvent<unknown>).detail;
      const bytes = detail && typeof detail === 'object' && !Array.isArray(detail) && 'bytes' in detail ? detail.bytes : null;
      if (!Array.isArray(bytes)) return;
      const message = bytes.filter((value): value is number => typeof value === 'number');
      // The existing controller resets the synth via beforeExternalTakeover.
      // A hardware output and browser synth must never double the same stream.
      if (!options.external.deliver(message)) synth?.send(message);
    };
    const onClose: EventListener = () => {if (authenticated(next)) reset();};
    next.target.addEventListener('touhou-midi', onMidi);
    next.target.addEventListener('touhou-midi-close', onClose);
    removeBound = () => {
      next.target.removeEventListener('touhou-midi', onMidi);
      next.target.removeEventListener('touhou-midi-close', onClose);
    };
  }
  async function prepare(music: MusicMode | 'ogg'): Promise<void> {
    if (disposed || music === 'none') return;
    if (!options.webAudioAvailable) throw new Error(options.translate('music.webAudioUnsupported'));
    if (!synth) {
      if (!loading) loading = options.loadSynth().then(Constructor => {
        if (disposed) return null;
        if (!Constructor) throw new Error(options.translate('music.synthMissing'));
        return synth ??= installSynth(new Constructor({quality: 1, useReverb: 1, voices: 64}));
      }).finally(() => {loading = null;});
      await loading;
    }
    if (disposed || !synth) return;
    const context = synth.getAudioContext();
    if (context.state === 'suspended') void context.resume().catch(() => {});
    await options.external.prepare();
  }
  function suspend() {
    const context = synth?.getAudioContext();
    if (context?.state === 'running') void context.suspend().catch(() => {});
  }
  function resume() {
    if (disposed || !options.runtime.getSnapshot().launched || !options.playerOpen() || options.document.hidden ||
      !options.document.hasFocus() || options.document.activeElement !== options.frame) return;
    const context = synth?.getAudioContext();
    if (context?.state === 'suspended') void context.resume().catch(() => {});
  }
  const visibility = () => {if (options.document.hidden) suspend(); else resume();};
  const pagehide = () => {options.external.panic();};
  options.frame.addEventListener('blur', suspend);
  options.frame.addEventListener('focus', resume);
  options.frame.addEventListener('load', rebind);
  options.window.addEventListener('blur', suspend);
  options.window.addEventListener('focus', resume);
  options.window.addEventListener('pagehide', pagehide);
  options.document.addEventListener('visibilitychange', visibility);
  const unsubscribe = options.runtime.subscribe(rebind);
  rebind();
  return {
    prepare, resetSynth, reset, rebind, suspend, resume, getOwnedAudioSourceSnapshot:audioSourceSnapshot,
    dispose() {
      if (disposed) return;
      disposed = true; unsubscribe(); removeBound?.(); removeBound = null; bound = null;
      options.frame.removeEventListener('blur', suspend);
      options.frame.removeEventListener('focus', resume);
      options.frame.removeEventListener('load', rebind);
      options.window.removeEventListener('blur', suspend);
      options.window.removeEventListener('focus', resume);
      options.window.removeEventListener('pagehide', pagehide);
      options.document.removeEventListener('visibilitychange', visibility);
      reset(); suspend();
    },
  };
}
export type MidiBridge = ReturnType<typeof createMidiBridge>;
