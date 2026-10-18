// Run only by the root coordinator, serially with no live browser workload.
// Two genuine scalar/SIMD Wasm instances; no changed decoder, PCM cache, or tolerance.
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';
import {performance} from 'node:perf_hooks';
import {WASI} from 'node:wasi';

const gateWallStartedAtUtc=new Date().toISOString(),gateWallStart=performance.now();
const root=fileURLToPath(new URL('../../',import.meta.url)), workspace=path.resolve(root,'..');
const fixtureRoot=path.resolve(process.env.TH09_FIXTURE_ROOT||workspace);
const fixtures=path.resolve(fixtureRoot,'th09_web/reference/assets');
const musicRoot=path.resolve(process.env.TH09_MUSIC_ROOT||path.resolve(fixtureRoot,'th09_web/assets/sdl-native/music'));
const toolchain=process.env.TH09_AUDIO_GATE_TOOLCHAIN||'wasi';
assert(['wasi','emscripten'].includes(toolchain),'TH09_AUDIO_GATE_TOOLCHAIN must be wasi or emscripten');
const emscripten=toolchain==='emscripten';
const sdk=process.env[emscripten?'TH09_EMSDK':'WASI_SDK_BIN'];assert(sdk,emscripten?'Set TH09_EMSDK':'Set WASI_SDK_BIN');
const executable=name=>name+(process.platform==='win32'?'.exe':'');
const compiler=path.resolve(sdk,emscripten?'install/bin/'+executable('clang'):executable('clang++'));
const emcc=emscripten?path.resolve(sdk,'install/emscripten/emcc.py'):null;
const binaryen=emscripten?path.resolve(sdk,'install/bin/'+executable('wasm-opt')):null;
const environmentOverrides=emscripten?{EMSDK:path.resolve(sdk),EM_CONFIG:path.resolve(sdk,'.emscripten'),EMCC_CORES:process.env.EMCC_CORES??'4'}:{};
const buildEnvironment={...process.env,...environmentOverrides};
assert(fs.existsSync(compiler),'Compiler missing');
if(emscripten)for(const p of [emcc,binaryen,environmentOverrides.EM_CONFIG])assert(fs.existsSync(p),'Emscripten tool missing: '+p);
const label=process.env.RUN_LABEL||'audio-simd-equivalence-01';
assert(/^[A-Za-z0-9_-]+$/.test(label),'Safe immutable artifact label');
const directory=path.resolve(root,'artifacts/multiplayer-tests',label);
assert(!fs.existsSync(directory),'Retained artifact directory already exists');
fs.mkdirSync(directory,{recursive:true});
const output=path.resolve(directory,'report.json'), sourcePath=path.resolve(root,'tests/multiplayer/audio-simd-equivalence.cpp');
const defs=path.resolve(root,'cpp/game/SoundEffects.cpp');
const sha=b=>createHash('sha256').update(b).digest('hex');
const identify=p=>{const b=fs.readFileSync(p);return {path:p,bytes:b.length,sha256:sha(b)};};
const names=[...fs.readFileSync(defs,'utf8').matchAll(/"(se_[^"]+\.wav)"/g)].map(m=>m[1]);
assert.equal(names.length,39);assert.equal(new Set(names).size,39);
const wav=names.map(name=>({name,...identify(path.resolve(fixtures,name))}));
const fmt=fs.readFileSync(path.resolve(fixtures,'thbgm.fmt')),music=[];
for(let n=0;n+52<=fmt.length;n+=52){
  const name=fmt.subarray(n,n+16).toString().replace(/\0.*$/s,'').replace(/\.wav$/,'.ogg');
  const align=fmt.readUInt16LE(n+44);assert(align);
  music.push({name,index:music.length,totalFrames:fmt.readUInt32LE(n+28)/align,introFrames:fmt.readUInt32LE(n+24)/align,
    channels:fmt.readUInt16LE(n+34),rate:fmt.readUInt32LE(n+36),...identify(path.resolve(musicRoot,name))});
}
assert.equal(music.length,19);
const formatTail=fmt.subarray(music.length*52);
assert(formatTail.every(byte=>byte===0),'Expected only zero padding after complete music records');
const fullNames=(process.env.TH09_FULL_MUSIC||'th09_01,th09_00b').split(',').map(x=>x.endsWith('.ogg')?x:x+'.ogg');
assert(fullNames.length&&new Set(fullNames).size===fullNames.length);
for(const n of fullNames)assert(music.some(m=>m.name===n),'Unknown full-music fixture: '+n);
const fullMusic=music.filter(m=>fullNames.includes(m.name)), mainMusic=fullMusic[0];
const flags=['-std=c++17','-O2',emscripten?'-g0':'-g','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti',
  ...(emscripten?['-DTH_NATIVE_PLATFORM=1','-DTH_ENABLE_THCRAP=1','-DTH09_DEVELOPMENT_HARNESS=0']:[])];
const emscriptenLinkFlags=['--no-entry','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
  '-sFILESYSTEM=1','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824',
  '-sEXPORTED_RUNTIME_METHODS=FS,HEAPU8','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-lnodefs.js'];
const variants=[{name:'scalar',extra:[]},{name:'simd',extra:['-msimd128']}];
const report={
  version:1,passed:false,scope:'Component bit-equivalence gate for standard Wasm SIMD in the miniaudio/stb translation unit with unchanged scalar SoundEffects.o. Not a 4x live/release smoothness or physical audio continuity result.',
  gateWall:{startedAtUtc:gateWallStartedAtUtc,scope:'Includes manifest reads, both builds, instantiation and all cases; stops immediately before report serialization.'},
  parameters:{fullMusic:fullNames,allMusicBoundaryCases:19,wavCases:39,mixedVoices:54,mixSteps:256,chunkFrames:1024},
  toolchain:{kind:toolchain,sdk:path.resolve(sdk),target:emscripten?'wasm32-unknown-emscripten':'wasm32-unknown-wasip1',
    environmentOverrides,nodeVersion:process.version,v8Version:process.versions.v8,driver:emscripten?emcc:compiler},
  musicLayout:{bytes:fmt.length,recordBytes:52,recordCount:music.length,trailingBytes:formatTail.length,trailingHex:formatTail.toString('hex'),trailingAllZero:true},
  sourceInference:'Default network characters [0,1] with selector=-1 can choose cue2/th09_01 or cue3/th09_00b. Both are full-read by default; the prior live report did not record the selected cue.',
  measurementNotes:[
    'Only the gate implementation translation unit is compiled with -msimd128 in the candidate. SoundEffects.o and all strict floating-point flags are identical; no SSE, fast-math, Huffman change, new resampler or PCM backend.',
    'Each operation is timed by host performance.now around one synchronous Wasm call. A/B order alternates per operation type, so repeated read/clamp pairs do not fix one instance first for every read. Setup, PCM buffer clearing, queries, comparison and hashing occur outside read/mix timers. Open, command, seek, clamp and close have separate samples.',
    'All raw elapsed samples remain, including cold module initialization, first reads, decoder opens and warm reads. No performance threshold, outlier removal or CPU-throttle inference.',
    'PCM is compared byte for byte before and after the same in-place clamp. Public decoder/sound status, length, cursor, format, playing/looping, gain/pan/pitch, engine time and every SoundEffects queue element are serialized without pointers and compared. No PCM output files are stored.',
    'Source stream EOF and hot seek0/odd/nonzero seek retain existing miniaudio resampler/filter history, including any baseline quirks. Two independent actual decoders are compared, never a cold PCM substitute.',
    'OGG uses the same file pull decoder as AudioDevice, frame-length validation and intro/end loop points. Music-only and 54-SFX-plus-music engines keep original 44100 stereo no-pitch/no-spatialization configuration.',
    emscripten?'Emscripten uses the selected production SDK, strict O2/g0 compilation and Binaryen post-link optimization. NodeFS mounts preserve the file pull decoder; the gate still is a separate component TU and excludes SDL queue/refill, output-device conversion, confirmed publication and live scheduling. It does not replace release acceptance.':'Node WASI compiler/runtime differs from Emscripten/browser. Passing this component gate is necessary evidence, not a substitute for production build feature inspection and subsequent live/release acceptance.',
    'instantiateAndInitializeMs begins after WebAssembly.Module construction. Emscripten additionally includes importing its generated loader and mounting NodeFS before gate_init; gateWall includes all construction and setup. Cold samples are retained, not interpreted as isolated startup comparisons.',
    'The module initialization timer now stops before artifact identity hashing; the older WASI -03 metadata field included those identity reads. All 84-case export timers and byte/state comparison scopes are unchanged.',
    'Emscripten optimized output may strip target_features. Object custom sections, compiledSimd, compiler diagnostics and exact binary hashes are retained independently; a missing final feature section alone is not SIMD failure.',
    'Host competition is not isolated in this component runner. Microsecond-scale Node elapsed pairs do not establish a 4x live improvement.',
    'This fixed workload is not a proof for all malformed assets, NaN inputs, device configurations, or all possible command sequences. Stereo/mono WAV rate and seek boundaries use actual fixtures.',
  ],
  identities:{runner:identify(fileURLToPath(import.meta.url)),test:identify(sourcePath),soundDefinitions:identify(defs),
    soundHeader:identify(path.resolve(root,'cpp/game/SoundEffects.hpp')),typesHeader:identify(path.resolve(root,'cpp/game/Types.hpp')),
    audioBaseline:identify(path.resolve(root,'cpp/sdl/AudioDevice.cpp')),
    miniaudio:identify(path.resolve(workspace,'portable/sdl/third_party/miniaudio.h')),
    stbVorbis:identify(path.resolve(workspace,'portable/sdl/third_party/stb_vorbis.h')),
    compiler:identify(compiler),node:identify(process.execPath),format:identify(path.resolve(fixtures,'thbgm.fmt')),
    ...(emscripten?{emscriptenDriver:identify(emcc),emscriptenConfig:identify(environmentOverrides.EM_CONFIG),binaryen:identify(binaryen)}:{})},
  compilerVersion:null,emscriptenVersion:null,binaryenVersion:null,flags,fixtures:{wav,music},builds:[],modules:[],cases:[],operations:[],errors:[],
};
let instances=[],current=null,digests=null,ordinal=0;
const operationOccurrences=new Map();
const fds=[];
const q=values=>{const a=[...values].sort((a,b)=>a-b),at=p=>a[Math.floor((a.length-1)*p)]??null;
  return {count:a.length,total:a.reduce((x,y)=>x+y,0),p50:at(.5),p95:at(.95),p99:at(.99),max:a.at(-1)??null};};
function build(args,kind){
  const command=emscripten?{executable:'python',arguments:[emcc,...args]}:{executable:compiler,arguments:args};
  const began=performance.now(),r=spawnSync(command.executable,command.arguments,{encoding:'utf8',windowsHide:true,env:buildEnvironment});
  report.builds.push({kind,args,command,elapsedMs:performance.now()-began,status:r.status,signal:r.signal,stdout:r.stdout,stderr:r.stderr,error:r.error?String(r.error):null});
  assert(!r.error&&r.status===0,kind+' failed: '+(r.stderr||r.error||''));
}
function begin(label,kind,index){
  assert(!current);
  current={label,kind,index,operations:0,comparedStateBytes:0,comparedPcmBytes:0,pcmFloatValues:0,nonfiniteValues:0,
    differingStateBytes:0,differingPcmBytes:0,mismatchingReturns:0,maximumAbsolutePcmError:0,firstDifference:null,
    nonzeroBeforeClamp:[0,0],outsideUnitBeforeClamp:[0,0]};
  digests=[createHash('sha256'),createHash('sha256')];
}
function note(value){if(!current.firstDifference)current.firstDifference=value;}
function bytesOf(side,ptr,n){const buffer=instances[side].memory.buffer;assert(ptr>=0&&ptr+n<=buffer.byteLength);return Buffer.from(buffer,ptr,n);}
function compare(kind,buffers,where,pcm=false){
  const [a,b]=buffers;
  if(a.length!==b.length){note({where,kind,reason:'length',lengths:[a.length,b.length]});current.mismatchingReturns++;return;}
  if(pcm){current.comparedPcmBytes+=a.length;current.pcmFloatValues+=a.length/4;}
  else current.comparedStateBytes+=a.length;
  for(let i=0;i<2;i++){digests[i].update(kind+':'+buffers[i].length+':');digests[i].update(buffers[i]);}
  if(!a.equals(b)){
    let count=0,first=-1;for(let i=0;i<a.length;i++)if(a[i]!==b[i]){count++;if(first<0)first=i;}
    current[pcm?'differingPcmBytes':'differingStateBytes']+=count;
    note({where,kind,reason:'bytes',firstByte:first,scalar:a[first],simd:b[first]});
  }
  if(pcm)for(let i=0;i<a.length;i+=4){
    const x=a.readFloatLE(i),y=b.readFloatLE(i);
    if(kind==='pcm-before-clamp'){
      current.nonzeroBeforeClamp[0]+=x!==0?1:0;current.nonzeroBeforeClamp[1]+=y!==0?1:0;
      current.outsideUnitBeforeClamp[0]+=Math.abs(x)>1?1:0;current.outsideUnitBeforeClamp[1]+=Math.abs(y)>1?1:0;
    }
    if(kind==='pcm-after-clamp'&&(Math.abs(x)>1||Math.abs(y)>1))note({where,kind,reason:'clamp-range',floatIndex:i/4});
    if(!Number.isFinite(x)||!Number.isFinite(y)){current.nonfiniteValues++;note({where,kind,reason:'nonfinite',floatIndex:i/4});}
    else current.maximumAbsolutePcmError=Math.max(current.maximumAbsolutePcmError,Math.abs(x-y));
  }
}
function observe(where){
  const sizes=instances.map(e=>e.gate_observe());
  if(sizes[0]!==sizes[1]){current.mismatchingReturns++;note({where,kind:'state-size',sizes});}
  compare('state',instances.map((e,i)=>bytesOf(i,e.gate_state(),sizes[i]*4)),where);
}
function call(name,args,phase){
  const occurrence=operationOccurrences.get(name)||0;operationOccurrences.set(name,occurrence+1);
  const order=occurrence%2?[1,0]:[0,1],elapsedMs=[0,0],returns=[0,0],id=ordinal++;
  for(const side of order){const start=performance.now();returns[side]=instances[side][name](...args);elapsedMs[side]=performance.now()-start;}
  const row={index:id,case:current.label,operation:name,args,phase,order:order.map(i=>variants[i].name),elapsedMs,returns,
    memoryBytes:instances.map(e=>e.memory.buffer.byteLength)};
  report.operations.push(row);current.operations++;
  if(returns[0]!==returns[1]){current.mismatchingReturns++;note({where:id,kind:'return',returns});}
  observe(id);
  return row;
}
function read(frames=1024,phase='warm-read'){
  for(const e of instances)e.gate_prepare(frames);
  const row=call('gate_read',[],phase),counts=instances.map(e=>e.gate_frames());
  row.framesRead=counts;
  if(counts[0]!==counts[1]){current.mismatchingReturns++;note({where:row.index,kind:'frames',counts});}
  for(let i=0;i<2;i++)assert(row.returns[i]===0||row.returns[i]===instances[i].gate_at_end(),'unexpected decoder read error');
  compare('pcm-before-clamp',instances.map((e,i)=>bytesOf(i,e.gate_pcm(),frames*8)),row.index,true);
  const clamp=call('gate_clamp',[],'clamp');
  compare('pcm-after-clamp',instances.map((e,i)=>bytesOf(i,e.gate_clamped(),frames*8)),clamp.index,true);
  return Math.max(...counts);
}
function end(close=true){
  if(close)call('gate_close',[],'close');
  current.streamSha256=digests.map(x=>x.digest('hex'));
  current.passed=!current.firstDifference&&!current.nonfiniteValues;
  report.cases.push(current);current=null;digests=null;
}
function open(label,kind,index){
  begin(label,kind,index);const r=call('gate_open',[kind,index],'cold-open');
  assert(r.returns.every(x=>x===0),'decoder open failed');
  const lengths=instances.map(e=>e.gate_length());assert.equal(lengths[0],lengths[1]);current.declaredFrames=lengths[0];return lengths[0];
}
function seek(frame,label='seek'){
  const r=call('gate_seek',[frame],label);
  for(let i=0;i<2;i++)assert(r.returns[i]===0||r.returns[i]===instances[i].gate_at_end(),'unexpected seek status');
}
function musicAction(action,value,phase){
  const r=call('gate_music_action',[action,value],phase);assert(r.returns.every(x=>x===0),'music action failed');
}
function objectTargetFeatures(filename){
  const b=fs.readFileSync(filename),sections=[];let cursor=8;
  assert(b.subarray(0,8).equals(Buffer.from([0,97,115,109,1,0,0,0])),'Expected a Wasm object');
  const leb=()=>{let n=0,shift=0,byte;do{assert(cursor<b.length&&shift<=28);byte=b[cursor++];n+=(byte&127)*2**shift;shift+=7;}while(byte&128);return n;};
  while(cursor<b.length){const id=b[cursor++],length=leb(),end=cursor+length;assert(end<=b.length);
    if(id===0){const size=leb();assert(cursor+size<=end);const name=b.subarray(cursor,cursor+size).toString();cursor+=size;
      if(name==='target_features')sections.push(b.subarray(cursor,end).toString('hex'));}
    cursor=end;
  }return sections;
}
async function loadEmscripten(loader,module,stdout,stderr){
  const {default:factory}=await import(pathToFileURL(loader).href);assert.equal(typeof factory,'function');
  const app=await factory({
    instantiateWasm(imports,receiveInstance){const instance=new WebAssembly.Instance(module,imports);receiveInstance(instance,module);return instance.exports;},
    print:value=>fs.writeSync(stdout,String(value)+'\n'),printErr:value=>fs.writeSync(stderr,String(value)+'\n'),
  });
  // Mount the same explicit fixture roots after runtime initialization and
  // before gate_init. Keep fopen/seek and the original file pull OGG backend.
  assert(app.FS?.filesystems?.NODEFS,'Generated loader must expose NodeFS');
  for(const [point,host] of [['/fixtures',fixtures],['/music',musicRoot]]){
    app.FS.mkdir(point);app.FS.mount(app.FS.filesystems.NODEFS,{root:host},point);
  }
  const exports={memory:{get buffer(){return app.HEAPU8.buffer;}}};
  for(const suffix of ['compiled_simd','init','open','seek','music_action','command','prepare','read','clamp','pcm','clamped','frames','length','cursor','at_end','observe','state','close']){
    const name='gate_'+suffix;assert.equal(typeof app['_'+name],'function','Missing export: '+name);exports[name]=app['_'+name];
  }return exports;
}
try{
  const version=spawnSync(compiler,['--version'],{encoding:'utf8',windowsHide:true});
  assert.equal(version.status,0);report.compilerVersion=version.stdout.trim();
  if(emscripten){
    const driverVersion=spawnSync('python',[emcc,'--version'],{encoding:'utf8',windowsHide:true,env:buildEnvironment});
    assert.equal(driverVersion.status,0);report.emscriptenVersion=driverVersion.stdout.trim();
    const optimizerVersion=spawnSync(binaryen,['--version'],{encoding:'utf8',windowsHide:true});
    assert.equal(optimizerVersion.status,0);report.binaryenVersion=optimizerVersion.stdout.trim();
  }
  const definitionsObject=path.resolve(directory,'SoundEffects.scalar.o');
  build([...flags,'-c',defs,'-o',definitionsObject],'shared-scalar-sound-definitions');
  report.identities.soundDefinitionsObject=identify(definitionsObject);
  for(const v of variants){
    const object=path.resolve(directory,v.name+'.o'),wasm=path.resolve(directory,v.name+'.wasm');
    const loader=emscripten?path.resolve(directory,v.name+'.mjs'):null;
    build([...flags,...v.extra,'-Rpass=(loop-vectorize|slp-vectorizer)','-c',sourcePath,'-o',object],v.name+'-compile');
    build([...flags,...(emscripten?emscriptenLinkFlags:['-mexec-model=reactor','-Wl,-z,stack-size=1048576']),object,definitionsObject,'-o',loader||wasm],v.name+'-link');
    const bytes=fs.readFileSync(wasm),module=new WebAssembly.Module(bytes);
    const stdout=fs.openSync(path.resolve(directory,v.name+'-stdout.txt'),'wx'),stderr=fs.openSync(path.resolve(directory,v.name+'-stderr.txt'),'wx');fds.push(stdout,stderr);
    const wasi=emscripten?null:new WASI({version:'preview1',args:['audio-simd-equivalence'],env:{},preopens:{'/fixtures':fixtures,'/music':musicRoot},stdout,stderr,returnOnExit:true});
    const started=performance.now();let e;
    if(emscripten)e=await loadEmscripten(loader,module,stdout,stderr);
    else{
      const instance=new WebAssembly.Instance(module,{wasi_snapshot_preview1:wasi.wasiImport});wasi.initialize(instance);e=instance.exports;
    }
    assert.equal(e.gate_compiled_simd(),v.name==='simd'?1:0);
    const instantiateAndInitializeMs=performance.now()-started;
    report.modules.push({name:v.name,object:identify(object),wasm:identify(wasm),instantiateAndInitializeMs,
      runtime:emscripten?'emscripten-nodefs':'node-wasi-preview1',...(loader?{loader:identify(loader)}:{}),objectTargetFeatures:objectTargetFeatures(object),
      compiledSimd:e.gate_compiled_simd(),imports:WebAssembly.Module.imports(module),exports:WebAssembly.Module.exports(module),
      targetFeatures:WebAssembly.Module.customSections(module,'target_features').map(s=>Buffer.from(s).toString('hex')),initialMemoryBytes:e.memory.buffer.byteLength});
    instances.push(e);
  }
  begin('cold-fixture-init',-1,-1);call('gate_init',[],'cold-init');end(false);
  // Every real WAV is streamed to EOF; the same hot decoder is then retriggered.
  for(let i=0;i<wav.length;i++){
    const length=open('wav/'+wav[i].name,0,i);let total=0,n=0,reads=0;
    do{n=read(1024,reads?'warm-continuous':'first-read');total+=n;assert(++reads<8192,'bounded WAV stream');}while(n);
    current.continuousFrames=total;read(1024,'after-end');seek(0,'after-end-retrigger');
    for(let k=0;k<6;k++)read(1024,'retrigger-read');
    const positions=[1,2,17,511,512,1023,1024,Math.floor(length/2),Math.floor(length/2)+1,length-3,length-1,length];
    for(const pos of positions){read(513,'seek-prime');seek(pos);for(let k=0;k<3;k++)read(1024,'seek-read');seek(0,'hot-seek0');read(127,'odd-short-read');read(1024,'hot-retrigger-read');}
    end();
  }
  // Full source streaming for both possible default network BGM cues.
  for(const m of fullMusic){
    open('ogg-full/'+m.name,1,m.index);let total=0,n=0,reads=0;
    do{n=read(1024,reads?'warm-continuous':'first-read');total+=n;assert(++reads<32768,'bounded OGG stream');}while(n);
    current.continuousFrames=total;assert.equal(total,m.totalFrames,'OGG complete source extent');
    read(1024,'after-end');seek(0,'after-end-retrigger');for(let k=0;k<6;k++)read(1024,'retrigger-read');end();
  }
  // Every music asset: cold prefix, odd/nonzero seek, intro, tail, zero restart.
  for(const m of music){
    open('ogg-boundaries/'+m.name,1,m.index);
    for(let k=0;k<8;k++)read(k%3===0?127:1024,k?'warm-prefix':'first-read');
    for(const pos of [0,1,17,1023,Math.floor(m.totalFrames/2),m.introFrames-1,m.introFrames,m.totalFrames-3,m.totalFrames-1,m.totalFrames]){
      seek(pos);for(let k=0;k<3;k++)read(1024,'boundary-read');seek(0,'same-track-restart');read(513,'restart-short');read(1024,'restart-read');
    }end();
    open('ogg-engine-loop/'+m.name,3,m.index);
    read(1024,'first-read');musicAction(2,m.totalFrames-1537,'seek-before-loop');
    for(let k=0;k<8;k++)read(1024,'loop-crossing-read');
    current.cursorAfterLoop=instances.map(e=>e.gate_cursor());
    assert(current.cursorAfterLoop.every(n=>n>=m.introFrames&&n<m.introFrames+16384),'music engine must actually wrap to the configured intro');
    musicAction(0,0,'stop');read(1024,'stopped-silence');musicAction(2,0,'same-track-seek0');musicAction(1,0,'same-track-start');
    for(let k=0;k<6;k++)read(1024,'retrigger-read');
    musicAction(3,-2500,'fade-gain');read(1024,'faded-read');musicAction(4,-375,'pan');read(1024,'panned-read');musicAction(3,0,'gain-restore');
    musicAction(2,m.introFrames+17,'odd-loop-seek');for(let k=0;k<4;k++)read(1024,'loop-seek-read');end();
  }
  for(const withMusic of [false,true])for(const nonzero of [false,true]){
    open('mix54/'+(withMusic?'music-':'sfx-')+(nonzero?'nonzero':'seek0'),2,withMusic?mainMusic.index:-1);
    for(let step=0;step<256;step++){
      call('gate_command',[step,Number(nonzero)],'command');
      if(withMusic&&step===80){musicAction(0,0,'pause');}
      if(withMusic&&step===84){musicAction(1,0,'resume');musicAction(3,-2100,'fade-gain');}
      if(withMusic&&step===90){musicAction(2,0,'same-track-restart');musicAction(3,0,'gain-restore');musicAction(1,0,'restart');}
      if(withMusic&&step===128)musicAction(2,mainMusic.totalFrames-513,'mixed-loop-seek');
      read(1024,step?'warm-mix':'first-mix');
    }end();
  }
  assert(report.cases.some(c=>c.label.startsWith('mix54/')&&c.outsideUnitBeforeClamp.every(n=>n>0)),'mixed workload must exercise clipping');
  assert(report.cases.every(c=>c.kind<0||c.nonzeroBeforeClamp.every(n=>n>0)),'every audio case must contain nonzero source output');
  report.passed=report.cases.every(c=>c.passed);
}catch(error){
  report.errors.push({message:String(error),stack:error?.stack});
  if(current){current.passed=false;current.incomplete=true;current.streamSha256=digests.map(h=>h.digest('hex'));report.cases.push(current);current=null;}
}finally{
  for(const fd of fds)fs.closeSync(fd);
  report.elapsedByPhase=Object.fromEntries([...new Set(report.operations.map(r=>r.phase))].map(phase=>{
    const rows=report.operations.filter(r=>r.phase===phase);return [phase,{scalar:q(rows.map(r=>r.elapsedMs[0])),simd:q(rows.map(r=>r.elapsedMs[1])),
      pairedDeltaMs:q(rows.map(r=>r.elapsedMs[1]-r.elapsedMs[0]))}];
  }));
  report.finalMemoryBytes=instances.map(e=>e.memory.buffer.byteLength);
  report.gateWall.endedAtUtc=new Date().toISOString();report.gateWall.elapsedMs=performance.now()-gateWallStart;
  fs.writeFileSync(output,JSON.stringify(report,null,2)+'\n',{flag:'wx'});
}
console.log(JSON.stringify({output,sha256:sha(fs.readFileSync(output)),passed:report.passed,
  cases:report.cases.length,operations:report.operations.length,failures:report.cases.filter(c=>!c.passed).map(c=>({label:c.label,first:c.firstDifference,incomplete:c.incomplete})),
  errors:report.errors,elapsedByPhase:report.elapsedByPhase,finalMemoryBytes:report.finalMemoryBytes},null,2));
if(!report.passed)process.exitCode=1;
