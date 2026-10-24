import {TH08_MIDI_PCM_ROLE,TH08_MIDI_VOICE_PCM_ROLE} from '../music-browser-helpers.mjs';

export function runtimePcm(role,{windowCount=1,nonZeroPerWindow=3,startTime=0}={}){
  const sourceIdentity=role===TH08_MIDI_PCM_ROLE?{nodeId:10,nodeType:'DynamicsCompressorNode',role:'tinysynth-compressor-output'}:
    role===TH08_MIDI_VOICE_PCM_ROLE?{nodeId:11,nodeType:'AudioNode',role:'tinysynth-voice-output'}:null;
  const identity={role,contextIndex:0,tapIds:[0],sampleRate:48000,...(sourceIdentity?{sourceIdentity}:{})};
  const windows=Array.from({length:windowCount},(_,index)=>({index,target:{role,available:true,contextIndex:0,state:'running',sampleRate:48000,
    ...(sourceIdentity?{sourceIdentity}:{}),currentTime:startTime+index/100,stateChangeCount:0,stateChangeDrops:0,stateChanges:[]},taps:[{tapId:0,contextState:'running',sampleRate:48000,
      ...(sourceIdentity?{sourceIdentity}:{}),currentTime:startTime+index/100,samples:2048,nonZero:nonZeroPerWindow,peakAbs:nonZeroPerWindow?0.25:0,rms:nonZeroPerWindow?0.1:0}]}));
  const nonZeroSamples=windowCount*nonZeroPerWindow;
  return{status:'complete',role,identity,windowCount,captureCount:windowCount,sampleRate:48000,contextIndices:[0],tapIds:[0],
    contextStates:windows.map(()=> 'running'),firstCurrentTime:windows[0]?.target.currentTime??null,
    lastCurrentTime:windows.at(-1)?.target.currentTime??null,elapsedFrames:windowCount>1?(windows.at(-1).target.currentTime-windows[0].target.currentTime)*48000:null,
    nonZeroSamples,peakAbs:nonZeroPerWindow?0.25:0,maxRms:nonZeroPerWindow?0.1:0,outputObserved:nonZeroSamples>0,windows,issues:[],reason:null};
}
