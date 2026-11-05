// Offline CDP sampling analysis. Never imported by the game or measurement harness.
// Usage: node portable/multiplayer/analyze-cpu-sampling.mjs <run-directory>
// Reads the recorded profiles and frozen artifacts; writes only cpu-analysis.json.
import {readFileSync,writeFileSync} from 'node:fs';
import {resolve,relative,isAbsolute} from 'node:path';
import {createHash} from 'node:crypto';

const HELP=[
  'Usage: node portable/multiplayer/analyze-cpu-sampling.mjs <run-directory> [options]',
  '  --report <path>   Report relative to the run directory (default 1-frontier.json).',
  '  --symbols <path>  Frozen index:name map (default frozen/th08-sdl.mjs.symbols).',
  '  --wasm <path>     Matching frozen binary (default frozen/th08-sdl.wasm).',
  '  --top <n>         Rows per ranking, 1..200 (default 20).',
  '  --help',
  '',
  'Output: <run-directory>/cpu-analysis.json.',
  'Samples include setup and frames before logical frame 240; this is diagnostic',
  'sampling, not performance acceptance, physical CPU self time, or a GPU trace.',
].join('\n');
const finite=value=>typeof value==='number'&&Number.isFinite(value);
const round=value=>finite(value)?Math.round(value*1e6)/1e6:null;
const ms=us=>round(us/1000);
const percent=(value,total)=>total>0?round(value*100/total):null;
const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const hash=value=>typeof value==='string'&&/^[a-f0-9]{64}$/i.test(value)?value.toLowerCase():null;
const pathKey=path=>process.platform==='win32'?path.toLowerCase():path;

function options(argv){
  const result={directory:null,report:'1-frontier.json',symbols:'frozen/th08-sdl.mjs.symbols',
    wasm:'frozen/th08-sdl.wasm',top:20};
  for(let i=0;i<argv.length;++i){
    const arg=argv[i];
    if(arg==='--help'||arg==='-h'){result.help=true;continue;}
    if(!arg.startsWith('--')){
      if(result.directory)throw Error('Provide exactly one run directory.');
      result.directory=resolve(arg);continue;
    }
    const equals=arg.indexOf('='),key=(equals<0?arg:arg.slice(0,equals)).slice(2);
    if(!['report','symbols','wasm','top'].includes(key))throw Error('Unknown option '+arg);
    const value=equals<0?argv[++i]:arg.slice(equals+1);
    if(!value||value.startsWith('--'))throw Error('Missing value for --'+key);
    result[key]=key==='top'?Number(value):value;
  }
  if(result.help)return result;
  if(!result.directory)throw Error('Provide a run directory. Use --help.');
  if(!Number.isInteger(result.top)||result.top<1||result.top>200)throw Error('--top must be 1..200.');
  result.output=resolve(result.directory,'cpu-analysis.json');
  for(const key of ['report','symbols','wasm']){
    result[key]=resolve(result.directory,result[key]);
    if(pathKey(result[key])===pathKey(result.output))throw Error('Output cannot overwrite an input.');
  }
  return result;
}
function readJson(path,allowPythonNonFinite=false){
  const bytes=readFileSync(path),text=bytes.toString('utf8').replace(/^\uFEFF/,'');
  let value,nonFiniteTokens=0;
  try{value=JSON.parse(text);}catch(original){
    if(!allowPythonNonFinite)throw original;
    // Python report rows can contain bare Infinity/NaN. Never rewrite strings.
    const normalized=text.replace(/("(?:[^"\\]|\\.)*")|(?:-?Infinity|NaN)(?=\s*[,}\]])/g,
      (match,quoted)=>{if(quoted)return quoted;++nonFiniteTokens;return 'null';});
    if(!nonFiniteTokens)throw original;
    value=JSON.parse(normalized);
  }
  return {value,sha256:sha256(bytes),nonFiniteTokens};
}
function recordedProfilePath(directory,name){
  if(typeof name!=='string'||!name||isAbsolute(name))throw Error('Profile path must be relative.');
  const path=resolve(directory,name),part=relative(directory,path);
  if(!part||part==='..'||part.startsWith('../')||part.startsWith('..\\')||isAbsolute(part))
    throw Error('Profile path escapes its run directory: '+name);
  if(pathKey(path)===pathKey(resolve(directory,'cpu-analysis.json')))
    throw Error('Analysis output cannot also be a profile input.');
  return path;
}

// Read only the import/function/code section structure. Do not compile or
// instantiate WebAssembly. Function indices include imported functions.
function wasmFunctionRanges(bytes){
  if(bytes.length<8||bytes.readUInt32LE(0)!==0x6d736100||bytes.readUInt32LE(4)!==1)
    throw Error('Expected a version-1 WebAssembly binary.');
  let p=8,limit=bytes.length,imports=0,declared=null;
  const ranges=new Map();
  const byte=()=>{if(p>=limit)throw Error('Truncated WASM section.');return bytes[p++];};
  const leb=()=>{
    let result=0,factor=1;
    for(let n=0;n<10;++n){
      const b=byte();result+=(b&127)*factor;
      if(!Number.isSafeInteger(result))throw Error('WASM integer exceeds safe range.');
      if(!(b&128))return result;
      factor*=128;
    }
    throw Error('Invalid WASM LEB integer.');
  };
  const skip=count=>{if(!Number.isSafeInteger(count)||count<0||p+count>limit)
    throw Error('Invalid WASM section extent.');p+=count;};
  const string=()=>skip(leb());
  const type=()=>{
    const tag=byte();
    // Typed references carry a signed heap-type LEB; its value is not needed.
    if(tag===0x63||tag===0x64){
      for(let n=0;n<10;++n){if(!(byte()&128))return;}
      throw Error('Invalid WASM reference type.');
    }
    if(![0x7f,0x7e,0x7d,0x7c,0x7b,0x70,0x6f].includes(tag))
      throw Error('Unsupported imported WASM value/reference type: '+tag);
  };
  const limits=()=>{
    const flags=leb();
    if(flags>7)throw Error('Unsupported WASM limits flags: '+flags);
    leb();if(flags&1)leb();
  };
  while(p<bytes.length){
    limit=bytes.length;
    const section=byte(),size=leb(),end=p+size;
    if(end>bytes.length)throw Error('Truncated WASM section payload.');
    limit=end;
    if(section===2){
      const count=leb();
      for(let i=0;i<count;++i){
        string();string();
        switch(byte()){
          case 0:leb();++imports;break;
          case 1:type();limits();break;
          case 2:limits();break;
          case 3:type();byte();break;
          case 4:byte();leb();break;
          default:throw Error('Unsupported WASM import kind.');
        }
      }
      if(p!==end)throw Error('Unexpected bytes in WASM import section.');
    }else if(section===3){
      declared=leb();
    }else if(section===10){
      const count=leb();
      if(declared!==count)throw Error('WASM function/code count mismatch.');
      for(let i=0;i<count;++i){
        const size=leb(),start=p;skip(size);
        ranges.set(imports+i,{start,end:p});
      }
      if(p!==end)throw Error('Unexpected bytes in WASM code section.');
    }
    p=end;
  }
  if(declared===null||ranges.size!==declared)throw Error('Missing WASM function/code section.');
  return {imports,defined:declared,total:imports+declared,ranges};
}
function artifacts(args){
  const warnings=[],symbols=new Map(),conflicts=new Set();
  let manifest={},manifestSha256=null,wasm=null,wasmInfo=null,symbolBytes=null;
  try{const loaded=readJson(resolve(args.directory,'manifest.json'));
    manifest=loaded.value;manifestSha256=loaded.sha256;}
  catch(error){warnings.push('Frozen manifest unavailable: '+error.message);}
  const expected=path=>hash(manifest?.[relative(resolve(args.directory,'frozen'),path).replaceAll('\\','/')]);
  let ignoredSymbolLines=0,duplicateSymbolLines=0;
  try{
    symbolBytes=readFileSync(args.symbols);
    for(const raw of symbolBytes.toString('utf8').replace(/^\uFEFF/,'').split(/\r?\n/)){
      if(!raw.trim())continue;
      const match=/^\s*(\d+):(.*)$/.exec(raw);
      if(!match||!Number.isSafeInteger(Number(match[1]))||!match[2].trim()){
        ++ignoredSymbolLines;continue;
      }
      const index=Number(match[1]),name=match[2].trim();
      if(symbols.has(index)){
        ++duplicateSymbolLines;
        if(symbols.get(index)!==name)conflicts.add(index);
      }else symbols.set(index,name);
    }
    for(const index of conflicts)symbols.delete(index);
  }catch(error){warnings.push('Symbol map unavailable: '+error.message);}
  try{wasm=readFileSync(args.wasm);wasmInfo=wasmFunctionRanges(wasm);}
  catch(error){warnings.push('WASM function ranges unavailable: '+error.message);}
  const wasmHash=wasm?sha256(wasm):null,symbolHash=symbolBytes?sha256(symbolBytes):null;
  const wasmExpected=expected(args.wasm),symbolExpected=expected(args.symbols);
  let declaredWasmHash=null,buildMatches=null;
  try{
    const build=readJson(resolve(args.directory,'frozen/build.json'));
    declaredWasmHash=hash(build.value.wasm);
    if(declaredWasmHash)buildMatches=declaredWasmHash===wasmHash;
  }catch(error){warnings.push('Optional frozen build identity unavailable: '+error.message);}
  const pairVerified=!!(wasmInfo&&wasmExpected&&symbolExpected&&
    wasmExpected===wasmHash&&symbolExpected===symbolHash&&buildMatches!==false);
  if(!pairVerified)warnings.push('Unverified frozen WASM/symbol pair: preserve original WASM function names.');
  if(ignoredSymbolLines||conflicts.size)warnings.push('Some symbol records are malformed or ambiguous; those records are not guessed.');
  return {symbols,wasmInfo,pairVerified,warnings,report:{
    manifestSha256,pairVerified,declaredWasmHash,buildMatches,
    wasm:{path:args.wasm,sha256:wasmHash,manifestSha256:wasmExpected,
      importedFunctions:wasmInfo?.imports??null,definedFunctions:wasmInfo?.defined??null},
    symbols:{path:args.symbols,sha256:symbolHash,manifestSha256:symbolExpected,
      usableEntries:symbols.size,ignoredLines:ignoredSymbolLines,duplicateLines:duplicateSymbolLines,
      conflictingIndices:[...conflicts].sort((a,b)=>a-b)},
    rule:'Exact wasm-function[N] index including imports; frozen manifest hashes plus matching module byte ranges. No node-id, line-number, import-offset, or column-number-to-index guess.',
    warnings,
  }};
}
const pseudoState=name=>name==='(idle)'?'idle':name==='(garbage collector)'?'gc':
  name==='(program)'?'program-unattributed':name==='(root)'?'root':'execution';
function location(frame){
  return {url:frame.url,scriptId:frame.scriptId,lineNumber:frame.lineNumber,columnNumber:frame.columnNumber};
}
function profileFrames(nodes,artifact){
  const frames=new Map(),modules=new Map();
  for(const node of nodes){
    if(!Number.isSafeInteger(node.id)||frames.has(node.id))throw Error('Missing/duplicate CPU profile node id.');
    const call=node.callFrame||{},rawName=String(call.functionName??''),url=String(call.url??'');
    const match=/^wasm-function\[(\d+)\]$/.exec(rawName);
    const index=match&&Number.isSafeInteger(Number(match[1]))?Number(match[1]):null;
    const wasmUrl=url.startsWith('wasm://'),bridge=/^(?:js-to-wasm(?::|$)|wasm-to-js(?::|$))/.test(rawName);
    const frame={id:node.id,rawName,name:rawName||('(anonymous) @ '+url+':'+call.lineNumber+':'+call.columnNumber),
      url,scriptId:String(call.scriptId??''),lineNumber:call.lineNumber??null,columnNumber:call.columnNumber??null,
      index,wasmUrl,bridge,resolved:false,resolution:index===null?'profile-name':'unverified-module'};
    frame.moduleKey=wasmUrl?url+'\0'+frame.scriptId:null;
    frames.set(node.id,frame);
    if(index===null||!frame.moduleKey)continue;
    let group=modules.get(frame.moduleKey);
    if(!group){group={url,scriptId:frame.scriptId,indexedNodes:0,verifiedOffsets:0,missingOffsets:0,
      importedNodes:0,mismatches:0,mismatchExamples:[]};modules.set(frame.moduleKey,group);}
    ++group.indexedNodes;
    if(!artifact.wasmInfo){++group.missingOffsets;continue;}
    const range=artifact.wasmInfo.ranges.get(index);
    if(index<artifact.wasmInfo.imports){++group.importedNodes;continue;}
    const offset=frame.columnNumber;
    if(!range||Number.isInteger(offset)&&offset>=0&&!(offset>=range.start&&offset<range.end)){
      ++group.mismatches;
      if(group.mismatchExamples.length<8)group.mismatchExamples.push({nodeId:node.id,index,offset,expected:range??null});
    }else if(!Number.isInteger(offset)||offset<0)++group.missingOffsets;
    else ++group.verifiedOffsets;
  }
  for(const group of modules.values())
    group.verified=artifact.pairVerified&&group.verifiedOffsets>0&&group.mismatches===0;
  for(const frame of frames.values()){
    if(frame.index!==null){
      if(modules.get(frame.moduleKey)?.verified&&artifact.symbols.has(frame.index)){
        frame.name=artifact.symbols.get(frame.index);frame.resolved=true;frame.resolution='frozen-index-map';
      }else if(modules.get(frame.moduleKey)?.verified)frame.resolution='missing-or-conflicting-symbol';
    }
    frame.kind=pseudoState(frame.rawName)!=='execution'?'profiler-pseudo':
      frame.bridge?'wasm-bridge':frame.index!==null?(frame.resolved?'wasm-resolved':'wasm-unresolved'):
      frame.wasmUrl?'wasm-profile-name':frame.url?'javascript':'native-or-unknown';
  }
  return {frames,modules:[...modules.values()]};
}
const STACK_RULES=[
  ['native-game-frame',/\bBrowserRuntime::step\(|::\(anonymous namespace\)::(?:tick|loop|frame)\(|^rollback_tick\(/],
  ['audio-callback',/onaudioprocess|audio_playback|AudioWorklet|SDL_(?:Playback|Capture)Audio|EMSCRIPTENAUDIO_/i],
  ['network-callback',/NetworkConnection::Pump|SessionChannel::Pump|::pump_network\(|network_pump|^eagler_peer_|RTCDataChannel|WebSocket.*onmessage/],
  ['measurement-observer',/^perf(?:Read|Advance|Frontier|LogicalPosition|Progress|Snapshot|Driver|Body)|^driverStatus$|^netStatus$/],
  ['input-callback',/touch|pointer|keyboard|gamepad/i],
];
function category(stack,leaf){
  const state=pseudoState(leaf.rawName);
  if(state!=='execution')return state;
  // Ordered stack evidence, not phase instrumentation. Game-frame membership
  // takes priority over calls to audio/network/input made inside that frame.
  for(const [label,pattern] of STACK_RULES)
    if(stack.some(frame=>pattern.test(frame.name)))return label;
  return stack.some(frame=>frame.wasmUrl||frame.index!==null)?'other-wasm-stack':'other-js-or-native';
}
function addBucket(map,name,delta){
  let value=map.get(name);if(!value){value={name,microseconds:0,samples:0};map.set(name,value);}
  value.microseconds+=delta;++value.samples;
}
function bucketRows(map,total,limit=Infinity){
  return [...map.values()].sort((a,b)=>b.microseconds-a.microseconds||a.name.localeCompare(b.name))
    .slice(0,limit).map(value=>({name:value.name,ms:ms(value.microseconds),
      percent:percent(value.microseconds,total),samples:value.samples}));
}
function distribution(values){
  const sorted=[...values].sort((a,b)=>a-b),count=sorted.length;
  if(!count)return {count:0,minMs:null,p50Ms:null,p95Ms:null,p99Ms:null,maxMs:null};
  const at=p=>ms(sorted[Math.floor((count-1)*p)]);
  return {count,minMs:at(0),p50Ms:at(.5),p95Ms:at(.95),p99Ms:at(.99),maxMs:at(1)};
}
function analyzeProfile(data,record,artifact,top){
  const nodes=data.nodes,samples=data.samples,deltas=data.timeDeltas;
  if(!Array.isArray(nodes)||!Array.isArray(samples)||!samples.length||!Array.isArray(deltas))
    throw Error('Expected nonempty samples, nodes, and timeDeltas; hitCount/nominal interval are never substituted.');
  const {frames,modules}=profileFrames(nodes,artifact),parents=new Map(),warnings=[],stackCache=new Map();
  let unknownChildren=0;
  for(const node of nodes)for(const child of node.children||[]){
    if(!frames.has(child)){++unknownChildren;continue;}
    if(parents.has(child)&&parents.get(child)!==node.id)throw Error('CPU profile has an ambiguous multi-parent node.');
    parents.set(child,node.id);
  }
  const stackFor=id=>{
    if(stackCache.has(id))return stackCache.get(id);
    const result=[],seen=new Set();let next=id;
    while(frames.has(next)){
      if(seen.has(next))throw Error('CPU profile contains a parent cycle.');
      seen.add(next);result.push(frames.get(next));next=parents.get(next);
    }
    result.reverse();stackCache.set(id,result);return result;
  };
  const functions=new Map();
  for(const frame of frames.values()){
    let value=functions.get(frame.name);
    if(!value){value={name:frame.name,selfUs:0,inclusiveUs:0,selfSamples:0,inclusiveSamples:0,
      nodeCount:0,rawNames:new Set(),indices:new Set(),locations:new Map(),kinds:new Set()};functions.set(frame.name,value);}
    ++value.nodeCount;value.rawNames.add(frame.rawName);value.kinds.add(frame.kind);
    if(frame.index!==null)value.indices.add(frame.index);
    const loc=location(frame);value.locations.set(JSON.stringify(loc),loc);
  }
  const states=new Map(),categories=new Map(),entries=new Map(),domains=new Map(),unresolved=new Map();
  let validDeltaUs=0,pairedUs=0,unpairedUs=0,attributedUs=0,missingNodeUs=0;
  let invalidDeltas=0,zeroDeltas=0,attributedSamples=0,missingNodeSamples=0,recursiveDuplicates=0;
  let indexedLeafUs=0,resolvedLeafUs=0,wasmStackUs=0,fullyResolvedStackUs=0;
  const validPairedDeltas=[];
  for(let i=0;i<deltas.length;++i){
    const delta=deltas[i];
    if(!finite(delta)||delta<0){++invalidDeltas;continue;}
    validDeltaUs+=delta;if(delta===0)++zeroDeltas;
    if(i>=samples.length){unpairedUs+=delta;continue;}
    pairedUs+=delta;validPairedDeltas.push(delta);
    const leaf=frames.get(samples[i]);
    if(!leaf){missingNodeUs+=delta;++missingNodeSamples;continue;}
    const stack=stackFor(leaf.id);attributedUs+=delta;++attributedSamples;
    const own=functions.get(leaf.name);own.selfUs+=delta;++own.selfSamples;
    const names=new Set();
    for(const frame of stack){
      if(names.has(frame.name)){++recursiveDuplicates;continue;}
      names.add(frame.name);const value=functions.get(frame.name);
      value.inclusiveUs+=delta;++value.inclusiveSamples;
    }
    addBucket(states,pseudoState(leaf.rawName),delta);
    addBucket(domains,leaf.kind,delta);
    addBucket(categories,category(stack,leaf),delta);
    const entry=stack.find(frame=>frame.rawName!=='(root)')||leaf;
    addBucket(entries,entry.name,delta);
    if(leaf.index!==null){
      indexedLeafUs+=delta;
      if(leaf.resolved)resolvedLeafUs+=delta;else addBucket(unresolved,leaf.rawName,delta);
    }
    const indexed=stack.filter(frame=>frame.index!==null);
    if(indexed.length){wasmStackUs+=delta;if(indexed.every(frame=>frame.resolved))fullyResolvedStackUs+=delta;}
  }
  const windowUs=finite(data.startTime)&&finite(data.endTime)&&data.endTime>=data.startTime?
    data.endTime-data.startTime:null;
  if(samples.length!==deltas.length)warnings.push('samples/timeDeltas length mismatch; only existing valid pairs are attributed.');
  if(invalidDeltas)warnings.push('Invalid or negative timeDeltas omitted, with no replacement estimate.');
  if(missingNodeSamples)warnings.push('Some sample node IDs are absent; their intervals remain unattributed.');
  if(unknownChildren)warnings.push('Unknown child IDs in the profile: '+unknownChildren);
  if(windowUs!==null&&validDeltaUs>windowUs)warnings.push('Sum of valid timeDeltas exceeds the profile window; retained without normalization.');
  if(record.samples!==samples.length)warnings.push('Recorded sample count differs from profile.samples.length.');
  if(record.startTime!==data.startTime||record.endTime!==data.endTime)warnings.push('Recorded/profile start or end timestamps differ.');
  if(modules.some(module=>!module.verified))warnings.push('At least one WASM module failed exact artifact/offset verification; its raw names are retained.');
  const row=value=>({name:value.name,selfMs:ms(value.selfUs),selfPercent:percent(value.selfUs,attributedUs),
    inclusiveMs:ms(value.inclusiveUs),inclusivePercent:percent(value.inclusiveUs,attributedUs),
    selfSamples:value.selfSamples,inclusiveSamples:value.inclusiveSamples,nodeCount:value.nodeCount,
    kinds:[...value.kinds],wasmFunctionIndices:[...value.indices].sort((a,b)=>a-b),
    rawNames:[...value.rawNames],locationCount:value.locations.size,locationExamples:[...value.locations.values()].slice(0,8)});
  const rank=key=>[...functions.values()].filter(value=>value.name!=='(root)'&&value[key]>0)
    .sort((a,b)=>b[key]-a[key]||a.name.localeCompare(b.name)).slice(0,top).map(row);
  const indexedNodes=[...frames.values()].filter(frame=>frame.index!==null);
  const mappedNodes=indexedNodes.filter(frame=>frame.resolved);
  return {
    seat:record.seat,status:'analyzed',startTimeUs:data.startTime,endTimeUs:data.endTime,
    durations:{profileWindowMs:windowUs===null?null:ms(windowUs),sumValidTimeDeltasMs:ms(validDeltaUs),
      pairedValidDurationMs:ms(pairedUs),attributedDurationMs:ms(attributedUs),
      missingNodeDurationMs:ms(missingNodeUs),unpairedTimeDeltaMs:ms(unpairedUs),
      windowMinusTimeDeltasMs:windowUs===null?null:ms(windowUs-validDeltaUs),
      timeDeltaDistribution:distribution(validPairedDeltas),
      attribution:'timeDeltas[i] is charged to samples[i], including the first interval from startTime. The final unsampled tail is not assigned to any function or idle.'},
    counts:{nodes:nodes.length,samples:samples.length,timeDeltas:deltas.length,attributedSamples,
      samplesMissingTimeDelta:Math.max(0,samples.length-deltas.length),invalidDeltas,zeroDeltas,missingNodeSamples,
      sameNameAncestorOccurrencesDeduplicated:recursiveDuplicates},
    states:bucketRows(states,attributedUs),selfByFrameKind:bucketRows(domains,attributedUs),
    topSelf:rank('selfUs'),topInclusive:rank('inclusiveUs'),
    topLevelStacks:{categories:bucketRows(categories,attributedUs),entryPoints:bucketRows(entries,attributedUs,top),
      scope:'Mutually exclusive sample categories using the ordered stack-name rules; entryPoints are actual first non-root ancestors. Inlined/missing frames can limit classification.'},
    wasmMapping:{modules,indexedNodes:indexedNodes.length,resolvedIndexedNodes:mappedNodes.length,
      indexedNodeCoveragePercent:percent(mappedNodes.length,indexedNodes.length),
      indexedLeafDurationMs:ms(indexedLeafUs),resolvedIndexedLeafDurationMs:ms(resolvedLeafUs),
      indexedLeafTimeCoveragePercent:percent(resolvedLeafUs,indexedLeafUs),
      stackWithIndexedWasmDurationMs:ms(wasmStackUs),fullyResolvedIndexedStackDurationMs:ms(fullyResolvedStackUs),
      indexedStackTimeCoveragePercent:percent(fullyResolvedStackUs,wasmStackUs),
      wasmNamesWithoutExplicitIndex:[...frames.values()].filter(frame=>frame.wasmUrl&&frame.index===null&&!frame.bridge).length,
      unresolvedSelfTop:bucketRows(unresolved,attributedUs,top),
      unresolvedNodeExamples:indexedNodes.filter(frame=>!frame.resolved).slice(0,20)
        .map(frame=>({nodeId:frame.id,rawName:frame.rawName,index:frame.index,reason:frame.resolution,...location(frame)})),
      scope:'Coverage denominators include only explicit wasm-function[N] names. JS/WASM bridges and already-named WASM frames are retained without guessing an index.'},
    warnings,
  };
}
function main(){
  const args=options(process.argv.slice(2));
  if(args.help){console.log(HELP);return;}
  const source=readJson(args.report,true),report=source.value,sampling=report.cpuSampling;
  if(!sampling||!Array.isArray(sampling.profiles)||!sampling.profiles.length)
    throw Error('Report has no cpuSampling.profiles; no profiles are inferred from directory contents.');
  const artifact=artifacts(args),profiles=[],seenPaths=new Set(),errors=[];
  for(const record of sampling.profiles){
    try{
      const path=recordedProfilePath(args.directory,record.path);
      if(seenPaths.has(pathKey(path)))throw Error('Duplicate recorded CPU profile path.');
      seenPaths.add(pathKey(path));
      const loaded=readJson(path),expected=hash(record.sha256);
      if(!expected||loaded.sha256!==expected)throw Error('CPU profile SHA-256 does not match its report record.');
      profiles.push({...analyzeProfile(loaded.value,record,artifact,args.top),
        path,sha256:loaded.sha256,recordedSha256:expected});
    }catch(error){
      const issue={seat:record?.seat??null,path:record?.path??null,status:'error',error:error.message};
      profiles.push(issue);errors.push(issue);
    }
  }
  const result={schemaVersion:1,analysisCompleted:errors.length===0,performanceAcceptance:false,
    scope:'CDP sampling across the active phase, including setup and frames before the measured body starts at logical frame 240; diagnostic only, not smoothness acceptance. The 180-frame warmup precedes sampling, so this is not a fresh 240-frame lead-in. No game/harness code is executed.',
    inputs:{report:args.report,reportSha256:source.sha256,runCorrectnessPassed:report.passed===true,
      cpuRate:report.cpuRate,oneWayMs:report.oneWayMs,jitterMs:report.jitterMs,limit60:report.limit60,
      requestedSamplingIntervalUs:sampling.intervalUs,recordedSamplingScope:sampling.scope,
      reportNonFiniteTokensNormalized:source.nonFiniteTokens},
    accounting:{units:'Microseconds in CDP; milliseconds in duration/ranking output. Never divide or multiply samples by CPU throttling rate.',
      denominator:'All attributed valid sample intervals in that profile, including explicit idle/GC/program. Profiles are separate processes and are not a shared wall-clock total.',
      inclusive:'Each sample interval contributes at most once to each exact displayed function name in its ancestor stack, including the leaf. Different function inclusives overlap; do not sum them as total CPU.',
      grouping:'Exact displayed name; named functions from different locations are combined and location examples retained. Anonymous functions include their source location.',
      root:'The (root) pseudo-frame is omitted from rankings only; all its sample intervals remain in duration/state accounting.',
      program:'(program) is unattributed runtime/native activity, not automatically idle or game work.',
      limits:'Statistical wall-time attribution under a sampler, with observer overhead and optimized/inlined stacks; not exact per-call CPU time, GPU cost, frame cadence, or lead-in-trimmed gameplay.'},
    classificationRules:STACK_RULES.map(([category,pattern])=>({category,pattern:pattern.source,flags:pattern.flags})),
    artifacts:artifact.report,topLimit:args.top,profiles,errors};
  writeFileSync(args.output,JSON.stringify(result,null,2)+'\n');
  console.log(JSON.stringify({output:args.output,analysisCompleted:result.analysisCompleted,
    artifactPairVerified:artifact.pairVerified,profiles:profiles.map(profile=>({
      seat:profile.seat,status:profile.status,attributedDurationMs:profile.durations?.attributedDurationMs??null,
      wasmIndexedLeafCoveragePercent:profile.wasmMapping?.indexedLeafTimeCoveragePercent??null,
      warnings:profile.warnings?.length??0,error:profile.error??null}))}));
  if(errors.length)process.exitCode=1;
}
try{main();}catch(error){console.error('CPU sampling analysis: '+error.message);process.exitCode=1;}
