import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import assert from 'node:assert/strict';
import {fileURLToPath} from 'node:url';

// Offline evidence only. Never starts a compiler, browser, or benchmark.
const [reportArg, outputArg, musicRootArg] = process.argv.slice(2);
assert(reportArg && outputArg, 'Usage: node analyze-live-audio-profile.mjs REPORT.json OUTPUT.json [MUSIC_ASSET_ROOT]');
const reportPath = path.resolve(reportArg), outputPath = path.resolve(outputArg);
assert(!fs.existsSync(outputPath), 'Refusing to overwrite existing evidence');
const selfPath = fileURLToPath(import.meta.url);
const sha = b => crypto.createHash('sha256').update(b).digest('hex');
const identity = p => { const b=fs.readFileSync(p); return {path:p,bytes:b.length,sha256:sha(b)}; };
const raw = fs.readFileSync(reportPath), report = JSON.parse(raw);
const prefix = reportPath.replace(/-report\.json$/, '');
assert.notEqual(prefix, reportPath, 'Expected a -report.json input');
const add = (map,key,us) => {
  const v=map.get(key)||{sampledUs:0,samples:0}; v.sampledUs+=us; v.samples++; map.set(key,v);
};
const quantiles = values => {
  const v=[...values].sort((a,b)=>a-b), q=p=>v[Math.min(v.length-1,Math.floor((v.length-1)*p))]??null;
  return {count:v.length,total:v.reduce((a,b)=>a+b,0),min:v[0]??null,p50:q(.5),p95:q(.95),p99:q(.99),max:v.at(-1)??null};
};
const table = (map,total) => [...map].map(([category,v])=>({category,...v,percent:total?v.sampledUs/total*100:0}))
  .sort((a,b)=>b.sampledUs-a.sampledUs);
const sum = map => [...map.values()].reduce((s,v)=>s+v.sampledUs,0);
const isWasm = f => /(?:^wasm:|\.wasm(?:$|\?))/.test(f.url||'');
const sourceClass = f => {
  if(f.functionName==='(idle)')return 'idle';
  if(f.functionName==='(program)')return 'program-unattributed';
  if(f.functionName==='(garbage collector)')return 'garbage-collector';
  if(!f.functionName)return 'anonymous-leaf';
  return isWasm(f)?'wasm-leaf':'named-js-native-other';
};
const functionKey = f => JSON.stringify([f.functionName||'(anonymous)',f.url||'',f.lineNumber??null,f.columnNumber??null]);
const functionInfo = key => { const [functionName,url,lineNumber,columnNumber]=JSON.parse(key); return {functionName,url,lineNumber,columnNumber}; };
const functionsTable = (map,n=30) => [...map].sort((a,b)=>b[1].sampledUs-a[1].sampledUs).slice(0,n).map(([k,v])=>({...functionInfo(k),...v}));
function analyzeProfile(side) {
  const file=prefix+'-'+side+'.cpuprofile', bytes=fs.readFileSync(file), p=JSON.parse(bytes);
  assert.equal(p.samples.length,p.timeDeltas.length);
  const nodes=new Map(p.nodes.map(n=>[n.id,n])), parents=new Map(), stacks=new Map();
  assert.equal(nodes.size,p.nodes.length);
  for(const n of p.nodes)for(const c of n.children||[]){
    assert(nodes.has(c)); assert(!parents.has(c),'Profile call tree must have unique parents');parents.set(c,n.id);
  }
  const stack=id=>{
    if(stacks.has(id))return stacks.get(id);
    const seen=new Set(), result=[];
    for(let n=id;n!==undefined;n=parents.get(n)){
      assert(!seen.has(n),'Profile tree cycle');seen.add(n);assert(nodes.has(n));result.push(nodes.get(n).callFrame);
    }
    result.reverse();stacks.set(id,result);return result;
  };
  const all=new Map(), pump=new Map(), mix=new Map(), leafFunctions=new Map(), mixFunctions=new Map(), anchors=new Map();
  const longest=[], deltaClasses=new Map();
  let total=0, elapsed=0, pumpUs=0, mixUs=0, negative=0;
  for(let i=0;i<p.samples.length;i++){
    const id=p.samples[i], us=p.timeDeltas[i];assert(Number.isFinite(us));if(us<0)negative++;
    const frames=stack(id), names=frames.map(f=>f.functionName||''), leaf=frames.at(-1), contains=re=>names.some(n=>re.test(n));
    const inPump=contains(/AudioDevice::pump\(/);
    const inMix=inPump&&contains(/ma_node_(?:graph_read_pcm_frames|read_pcm_frames)(?:\(|$)/);
    const endOffset=elapsed+us;elapsed=endOffset;total+=us;
    add(all,sourceClass(leaf),us);add(leafFunctions,functionKey(leaf),us);
    add(deltaClasses,us<0?'negative':us===0?'zero':us<=1000?'0-1ms':us<=5000?'1-5ms':us<=20000?'5-20ms':'>20ms',us);
    if(inPump){
      pumpUs+=us;
      const group=inMix?'mix-attributed':contains(/(?:_emscripten_get_now|^now$)/)?'clock-calls':
        contains(/(?:SDL_PutAudioStreamData|PutAudioStreamBuffer)/)?'sdl-put':
        contains(/SDL_GetAudioStreamQueued/)?'sdl-queue-read':'pump-other-or-inlined';
      add(pump,group,us);
    }
    if(inMix){
      mixUs+=us;
      const group=contains(/vorbis|codebook_|imdct_/i)?'bgm-vorbis-subtree':
        contains(/ma_wav|ma_dr_wav/)?'sfx-wav-subtree':
        contains(/ma_resampling|ma_resampler|ma_linear_resampler|ma_biquad|ma_lpf/)?'resampler-and-inlined-filter':
        contains(/ma_channel_converter|ma_data_converter|ma_convert_pcm_frames/)?'channel-format-residue':
        contains(/ma_data_source|ma_decoder/)?'data-source-decoder-residue':
        contains(/ma_engine|ma_node_|ma_mix_/)?'nodegraph-mixing-residue':'other';
      add(mix,group,us);add(mixFunctions,functionKey(leaf),us);
    }
    const tests={
      audioPump:/AudioDevice::pump\(/,musicSelect:/AudioDevice::music\(/,
      vorbisRead:/ma_stbvorbis_read_pcm_frames\(/,vorbisSeek:/ma_stbvorbis_seek_to_pcm_frame\(/,
      vorbisLength:/stb_vorbis_stream_length_in_samples\(/,
      sdlPlayback:/SDL_PlaybackAudioThreadIterate/,
      sdlOutputResampler:/ResampleFrame_/,
      clockNamed:/^(?:_emscripten_get_now|now)$/,
      packetId:/^packetId$/,sendNamed:/^send$/,
    };
    for(const [key,re]of Object.entries(tests))if(contains(re))add(anchors,key,us);
    longest.push({sampleIndex:i,nodeId:id,deltaUs:us,intervalEndOffsetUs:endOffset,sourceClass:sourceClass(leaf),inPump,inMix,path:names});
  }
  assert.equal(negative,0,'Negative deltas require manual interpretation, not silent clipping');
  assert.equal(sum(all),total);assert.equal(sum(pump),pumpUs);assert.equal(sum(mix),mixUs);
  const symbols=functionsTable(leafFunctions,Number.MAX_SAFE_INTEGER);
  const semanticSelf=symbols.filter(x=>/vorbis|codebook_|imdct_|ma_resampl|ma_linear|ma_biquad|ma_lpf|ma_wav|ma_dr_wav|ma_.*(?:u8|s16).*f32|ResampleFrame_/i.test(x.functionName));
  const overheadSelf=symbols.filter(x=>/^(?:packetId|send|_emscripten_get_now|now)$|probe|snapshot|performanceCosts|readCosts/i.test(x.functionName));
  return {
    side,input:{path:file,bytes:bytes.length,sha256:sha(bytes)},
    window:{startTimeUs:p.startTime,endTimeUs:p.endTime,spanUs:p.endTime-p.startTime,
      sampledUs:total,unsampledTailUs:p.endTime-p.startTime-total,nodeCount:p.nodes.length,sampleCount:p.samples.length,
      requestedSamplingIntervalUs:500,deltaUs:quantiles(p.timeDeltas),deltaClasses:table(deltaClasses,total)},
    entireProfileLeafPartition:table(all,total),
    audioPump:{sampledUs:pumpUs,partition:table(pump,pumpUs)},
    audioMix:{sampledUs:mixUs,partition:table(mix,mixUs),topLeafFunctions:functionsTable(mixFunctions,35)},
    overlappingSubtreeAnchors:table(anchors,total),
    relevantLeafFunctions:semanticSelf,
    namedDiagnosticAndNetworkLeafCandidates:overheadSelf,
    topWasmLeafFunctions:symbols.filter(isWasm).slice(0,35),
    topNonWasmLeafFunctions:symbols.filter(x=>!isWasm(x)).slice(0,35),
    longestSamplingIntervals:longest.sort((a,b)=>b.deltaUs-a.deltaUs).slice(0,20),
    separateLeafSymbolsFound:{
      biquadOrLpf:semanticSelf.filter(x=>/ma_biquad|ma_lpf/.test(x.functionName)),
      wavIntegerToFloat:semanticSelf.filter(x=>/ma_.*(?:u8|s16).*f32/.test(x.functionName))
    },
  };
}
function nativeContext(x){
  const l=x.liveCallbackCosts;assert(l&&l.valid,'Need valid live counters');
  const decode=v=>Object.fromEntries(l.fieldNames.map((n,i)=>[n,v[i]]));
  const keep=v=>Object.fromEntries(Object.entries(decode(v)).filter(([n])=>
    /^(?:audio|music)|^counter\.(?:pcm|music|audio|published|queueEmpty|sfxDecoder|timingErrors|replayTicks|restoredCheckpoints)/.test(n)));
  return {valid:l.valid,errors:l.errors,overflow:l.overflow,fieldNames:l.fieldNames,
    final:keep(l.finalValues),marks:l.marks.map(m=>({label:m.label,observedAt:m.observedAt,values:keep(m.values)})),
    firstFrame:l.frames[0]?{sampleIndex:l.frames[0].sampleIndex,frame:l.frames[0].frame,observedAt:l.frames[0].observedAt}:null,
    lastFrame:l.frames.at(-1)?{sampleIndex:l.frames.at(-1).sampleIndex,frame:l.frames.at(-1).frame,observedAt:l.frames.at(-1).observedAt,values:keep(l.frames.at(-1).values)}:null,
    audioContext:x.audio,audioOutput:{...x.audioOutput,blocks:undefined},memoryBytes:x.memoryBytes};
}
function hostContext(h){
  if(!h)return null;
  const map=new Map();
  for(const s of h.processSamples||[])for(const [pid,name,cpuSeconds]of s.rows||[]){
    const key=pid+':'+name,v=map.get(key)||{pid,name,firstCpuSeconds:cpuSeconds,lastCpuSeconds:cpuSeconds,firstSourceUtcMs:s.sourceUtcMs,samples:0};
    v.lastCpuSeconds=cpuSeconds;v.lastSourceUtcMs=s.sourceUtcMs;v.samples++;map.set(key,v);
  }
  const processes=[...map.values()].map(v=>({...v,observedDeltaCpuSeconds:v.lastCpuSeconds-v.firstCpuSeconds})).sort((a,b)=>b.observedDeltaCpuSeconds-a.observedDeltaCpuSeconds);
  return {valid:h.valid,complete:h.complete,counts:h.counts,overflow:h.overflow,errors:h.errors,scope:h.scope,limitations:h.limitations,
    marks:h.marks,knownProcesses:h.knownProcesses,busyFraction:quantiles((h.cpuSamples||[]).map(s=>s.busyFraction).filter(Number.isFinite)),
    observedProcesses:processes,observedCompilerProcesses:processes.filter(x=>/clang|wasm-opt|ld|emcc/i.test(x.name)),
    caveat:'Host sampling spans setup and report extraction as well as profiles. Process filtering and finite sample cadence cannot prove host isolation or assign a stall to another process.'};
}
function assets(base){
  if(!base)return null;
  base=path.resolve(base);const fmtPath=path.join(base,'reference/assets/thbgm.fmt'),b=fs.readFileSync(fmtPath),rows=[];
  for(let n=0;n+52<=b.length;n+=52){
    const file=b.subarray(n,n+16).toString().replace(/\0.*$/s,''),channels=b.readUInt16LE(n+34),rate=b.readUInt32LE(n+36),align=b.readUInt16LE(n+44);
    const totalFrames=b.readUInt32LE(n+28)/align,oggPath=path.join(base,'assets/sdl-native/music',file.replace(/\.wav$/,'.ogg')),ogg=fs.readFileSync(oggPath);
    const at=ogg.indexOf(Buffer.from('\x01vorbis','binary'));assert(at>=0);
    rows.push({file,channels,rate,totalFrames,introFrames:b.readUInt32LE(n+24)/align,durationSeconds:totalFrames/rate,
      fullF32Bytes:totalFrames*channels*4,ogg:{path:oggPath,bytes:ogg.length,sha256:sha(ogg),channels:ogg[at+11],rate:ogg.readUInt32LE(at+12)}});
  }
  return {scope:'Current fixture metadata read after capture; the raw live report does not record the selected music cue or music asset hashes.',
    format:identity(fmtPath),tracks:rows};
}
const result={
  version:1,scope:'Offline attribution of the complete V8 sampling windows of one diagnostic live session, not release acceptance or a process CPU benchmark.',
  inputs:{report:{path:reportPath,bytes:raw.length,sha256:sha(raw)},analyzer:identity(selfPath)},
  identity:report.identity,parameters:report.parameters,
  correctness:{passed:report.passed,errors:report.errors,finalComparable:report.finalComparable,hashes:report.results.map(r=>r.hash),audited:report.results.map(r=>r.audited)},
  method:{
    units:'Sample weights are raw V8 timeDeltas in microseconds. They include descheduling and profiler perturbation; no sample is discarded and no CPU-time claim is made.',
    partition:'Each sample belongs to exactly one entire-profile source category. Within AudioDevice::pump, each sample belongs to one pump group; within its ma_node_graph_read_pcm_frames or ma_node_read_pcm_frames descendants, each sample belongs to one mix group. Partition sums are asserted.',
    precedence:'Mix groups use ancestor membership: Vorbis/codebook/IMDCT, then WAV, then resampler/filter, then channel/data converter, then data-source/decoder, then node/engine/mix residue. A descendant of a group is charged only once.',
    inclusive:'Subtree anchors are unions of matching ancestors per sample, preventing recursion/name aggregation double-counting. Different anchors overlap and must not be added.',
    inlining:'WAV includes read/interface/metadata and inlined conversion, not just u8/s16 conversion. Linear resampler includes inlined filter work. No separate biquad/lpf/u8-to-f32/s16-to-f32 share is inferred without symbols.',
    diagnostics:'Named clock/probe/network leaf candidates are retained, not subtracted. now/send names also serve runtime/product work. Audio-output, transport, live-counter and 500us V8 sampling probes are enabled; their causal overhead is not isolated.',
    window:'Profiler starts/stops sequentially for each side after netplay setup and around formal polling. Complete profile windows differ by side and include more than report.summary frames 180..899. No recorded mapping from CDP profile timestamps to browser performance.now permits exact per-callback alignment.',
    counterContext:'Native counters span before-open through final capture, which differs from the profile window. Their inclusive phases overlap and must not be summed. Startup/diagnostic checks can be present in cumulative counters even when the formal summary has zero replay.',
    limits:'One 900-frame tape, one selected BGM, no impaired relay delivery, profile build with diagnostics. This cannot accept release smoothness, attribute earlier 2s RAF gaps, establish every music boundary, or measure PCM-cache/SIMD gains.',
  },
  formalSummary:report.summary,host:hostContext(report.hostLoad),
  nativeCounterContext:report.results.map(nativeContext),
  currentMusicAssets:assets(musicRootArg),
  sides:['P1','P2'].map(analyzeProfile),
};
fs.writeFileSync(outputPath,JSON.stringify(result,null,2)+'\n',{flag:'wx'});
console.log(JSON.stringify({output:outputPath,sha256:sha(fs.readFileSync(outputPath)),
  sides:result.sides.map(s=>({side:s.side,window:s.window,audioPump:s.audioPump,audioMix:{sampledUs:s.audioMix.sampledUs,partition:s.audioMix.partition}}))},null,2));
