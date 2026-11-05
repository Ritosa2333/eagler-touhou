// Explicit post-fence diagnostic only. The caller owns and attests the boundary:
// BOTH final state and audio fences have finished, and bulk export has not begun.
// This module never reads a game/runtime object, changes CPU throttling, schedules
// RAF/timers, or uses the game's WebAssembly module, memory, imports or exports.
// A normal result after stopping does not establish normal execution during play.

export const fixedWorkProbePolicy=Object.freeze({
 schema:1,boundary:'post-state-audio-fence/pre-bulk',iterations:2000000,
 warmupGroups:2,measurementGroups:5,initialSeed:123456789,indexMultiplier:0x9e3779b9,
 order:'wasm,js on even groups; js,wasm on odd groups',
 algorithm:'u32 xorshift(13,17,5), then x += i * 0x9e3779b9 modulo 2^32',
 seedSchedule:'(initialSeed + imul(globalGroupIndex,indexMultiplier)) >>> 0'
});

// The public kernels take u32 iteration/seed inputs. The production probe always
// uses the fixed policy count; it does not accept caller-selected work amounts.
export function fixedWorkJsKernel(iterations,seed){
 const n=iterations>>>0;let x=seed|0;
 for(let i=0;i<n;++i){x^=x<<13;x^=x>>>17;x^=x<<5;x=(x+Math.imul(i,0x9e3779b9))|0;}
 return x>>>0;
}

function uleb(value){
 const bytes=[];do{const b=value&127;value=Math.floor(value/128);bytes.push(b|(value?128:0));}while(value);
 return bytes;
}
function sleb32(value){
 const bytes=[];value|=0;
 for(;;){const b=value&127;value>>=7;
  const done=(value===0&&(b&64)===0)||(value===-1&&(b&64)!==0);
  bytes.push(b|(done?0:128));if(done)return bytes;
 }
}

// Readable binary assembly of exactly the JS loop above, not an opaque fixture.
// Type: kernel(n:i32, seed:i32)->i32. Local 2 is i (initially zero); parameter 1
// holds x. No import, memory, table, global, start, data or element section exists.
export function fixedWorkWasmBytes(){
 const op={block:0x02,loop:0x03,end:0x0b,br:0x0c,brIf:0x0d,get:0x20,set:0x21,
  constant:0x41,geU:0x4f,add:0x6a,mul:0x6c,xor:0x73,shl:0x74,shrU:0x76};
 const get=n=>[op.get,n],set=n=>[op.set,n],constant=n=>[op.constant,...sleb32(n)];
 const body=[1,1,0x7f, // One local declaration group, containing one i32 (i).
  op.block,0x40,op.loop,0x40,...get(2),...get(0),op.geU,op.brIf,1];
 for(const [bits,shift] of [[13,op.shl],[17,op.shrU],[5,op.shl]])
  body.push(...get(1),...get(1),...constant(bits),shift,op.xor,...set(1));
 body.push(...get(1),...get(2),...constant(0x9e3779b9),op.mul,op.add,...set(1),
  ...get(2),...constant(1),op.add,...set(2),op.br,0,op.end,op.end,...get(1),op.end);
 const section=(id,data)=>[id,...uleb(data.length),...data];
 return new Uint8Array([
  0x00,0x61,0x73,0x6d,0x01,0x00,0x00,0x00,
  ...section(1,[1,0x60,2,0x7f,0x7f,1,0x7f]), // One (i32,i32)->i32 type.
  ...section(3,[1,0]),                       // One function of type zero.
  ...section(7,[1,6,107,101,114,110,101,108,0,0]), // Export function 0 as kernel.
  ...section(10,[1,...uleb(body.length),...body])
 ]);
}

// Returns null without reading performance/WebAssembly when disabled. An enabled
// call is synchronous: compilation, instantiation and all fixed work run on the
// calling page's thread. `await runFixedWorkProbe(...)` is also safe for callers.
// The boundary token is a CALLER DECLARATION, not an independent game-state check.
// `passed` means only complete, structurally isolated, matching probe results;
// there is deliberately no timing threshold or inferred gameplay/CPU-time PASS.
export function runFixedWorkProbe(options={}){
 if(!options||typeof options!=='object'||Array.isArray(options))throw TypeError('Fixed work probe options must be an object');
 const enabled=options.enabled===undefined?false:options.enabled;
 if(typeof enabled!=='boolean')throw TypeError('Fixed work probe enabled must be boolean');
 if(!enabled)return null;
 const {boundary,seat=null,label=null}=options;
 if(boundary!==fixedWorkProbePolicy.boundary)throw Error('Fixed work probe requires the explicit post-state/audio-fence, pre-bulk boundary');
 if(seat!==null&&(!Number.isInteger(seat)||seat<0||seat>2))throw Error('Fixed work probe seat must be null or 0..2');
 if(label!==null&&(typeof label!=='string'||label.length>120))throw Error('Fixed work probe label must be null or at most 120 characters');
 const performance=globalThis.performance;
 if(!performance||typeof performance.now!=='function'||!Number.isFinite(performance.timeOrigin))
  throw Error('Fixed work probe requires this realm performance clock and timeOrigin');
 const now=()=>{const value=performance.now();if(!Number.isFinite(value))throw Error('Non-finite performance clock');return value;};
 const report={
  schema:1,kind:'same-page-post-fence-fixed-work',enabled:true,boundary,
  boundaryAuthority:'caller-declared; the probe does not inspect game or fence state',seat,label,
  scope:'Wall-clock measurements after final state/audio fences, before bulk export. '
   +'Private fixed JS/WASM arithmetic only. A normal post-fence result does not establish normal gameplay. '
   +'No CPU/thread-time measurement or performance pass threshold is provided.',
  threadTimeMeasured:false,policy:{...fixedWorkProbePolicy},timeOriginMs:performance.timeOrigin,
  startedAtPerformanceMs:now(),finishedAtPerformanceMs:null,totalElapsedMs:null,
  construction:{},compilation:{},instantiation:{},module:null,
  warmup:{groups:[]},measurements:{groups:[]},errors:[],completed:false,passed:false
 };
 let stage='construction';
 const timed=(entry,action)=>{
  entry.startedAtPerformanceMs=now();entry.completed=false;
  try{const value=action();entry.completed=true;return value;}
  finally{
   entry.finishedAtPerformanceMs=now();entry.wallMs=entry.finishedAtPerformanceMs-entry.startedAtPerformanceMs;
   if(entry.wallMs<0)throw Error('Performance clock moved backwards');
  }
 };
 try{
  const bytes=timed(report.construction,fixedWorkWasmBytes);
  report.construction.byteLength=bytes.byteLength;
  stage='compilation';
  const module=timed(report.compilation,()=>new WebAssembly.Module(bytes));
  stage='module-isolation';
  const imports=WebAssembly.Module.imports(module),exports=WebAssembly.Module.exports(module);
  report.module={byteLength:bytes.byteLength,imports,exports,definedMemories:0,
   sectionIds:[1,3,7,10],isolation:'fresh module; no imports, memory, table, globals or start; one function export'};
  if(imports.length!==0||exports.length!==1||exports[0].name!=='kernel'||exports[0].kind!=='function')
   throw Error('Fixed work module import/export boundary differs from the assembled kernel');
  stage='instantiation';
  const instance=timed(report.instantiation,()=>new WebAssembly.Instance(module));
  const wasm=(n,seed)=>instance.exports.kernel(n,seed)>>>0;
  const group=(phase,index,globalIndex)=>{
   const seed=(fixedWorkProbePolicy.initialSeed+Math.imul(globalIndex,fixedWorkProbePolicy.indexMultiplier))>>>0;
   const order=index%2?['js','wasm']:['wasm','js'];
   const row={phase,index,globalIndex,iterations:fixedWorkProbePolicy.iterations,seed,order,runs:[],matches:null};
   report[phase].groups.push(row);
   for(const engine of order){
    const run={engine,result:null};row.runs.push(run);
    run.result=timed(run,()=>engine==='wasm'?wasm(row.iterations,seed):fixedWorkJsKernel(row.iterations,seed));
   }
   row.matches=row.runs[0].result===row.runs[1].result;
   if(!row.matches)report.errors.push({stage:phase,index,message:'JS/WASM u32 results differ'});
  };
  stage='warmup';
  timed(report.warmup,()=>{for(let i=0;i<fixedWorkProbePolicy.warmupGroups;++i)group('warmup',i,i);});
  stage='measurements';
  timed(report.measurements,()=>{for(let i=0;i<fixedWorkProbePolicy.measurementGroups;++i)
   group('measurements',i,i+fixedWorkProbePolicy.warmupGroups);});
  report.completed=true;
 }catch(error){report.errors.push({stage,message:String(error?.message??error).slice(0,600)});}
 finally{
  report.finishedAtPerformanceMs=now();report.totalElapsedMs=report.finishedAtPerformanceMs-report.startedAtPerformanceMs;
  report.finalTimeOriginMs=performance.timeOrigin;
  if(report.totalElapsedMs<0||report.finalTimeOriginMs!==report.timeOriginMs)
   report.errors.push({stage:'clock',message:'Clock moved backwards or realm timeOrigin changed'});
  report.passed=report.completed&&report.errors.length===0;
 }
 return report;
}
