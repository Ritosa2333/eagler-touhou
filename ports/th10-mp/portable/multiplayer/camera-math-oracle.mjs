// Diagnostic reads and controls shared by the live harness and stopped-world oracle.
// No per-callback scans, readback, or reference math runs are enabled by default.
import {readDefault, pixelDifference} from './model-quad-oracle.mjs';

const requireValue=(value,message)=>{if(!value)throw Error(message);};
export const cameraMathCounterNames=['calls','hits','misses','evictions','bypasses','verified','mismatches'];
export const cameraMathStatusSchema=['schema','enabled','supported','verifyEvery','invalidations',
 ...['lookAt','perspective'].flatMap(name=>cameraMathCounterNames.map(field=>name+'.'+field))];

export function readCameraMathStatus(core,required=false){
 const fn=core.multiplayer_camera_math_cache_status;
 if(!fn){requireValue(!required,'Camera math status export required');return null;}
 const pointer=fn(),buffer=core.memory.buffer;
 requireValue(pointer%8===0&&pointer+19*8<=buffer.byteLength,'Camera math status range');
 const words=Array.from(new Float64Array(buffer,pointer,19));
 requireValue(words.every(Number.isSafeInteger)&&words.every(x=>x>=0),'Camera math counters must be exact integers');
 requireValue(words[0]===1&&words[1]<=1&&words[2]<=1&&words[3]<=0xffffffff,'Camera math status schema');
 for(const at of [5,12]){
  const [calls,hits,misses,evictions,bypasses,verified,mismatches]=words.slice(at,at+7);
  requireValue(calls===hits+misses+bypasses&&evictions<=misses&&verified<=hits&&mismatches<=verified,
   'Camera math counters do not partition');
 }
 return words;
}
export function readCameraMathVerification(core,firstMismatch=true){
 if(!core.multiplayer_camera_math_cache_verification)return null;
 const pointer=core.multiplayer_camera_math_cache_verification(firstMismatch?1:0),buffer=core.memory.buffer;
 requireValue(pointer%4===0&&pointer+56*4<=buffer.byteLength,'Camera math verification range');
 const words=Array.from(new Uint32Array(buffer,pointer,56));
 requireValue(words[0]===1&&words[3]<=2&&words[4]<=14&&words[5]<=1,'Camera math verification schema');
 return words;
}
export function configureCameraMath(core,enabled,verifyEvery=0){
 requireValue(Number.isInteger(verifyEvery)&&verifyEvery>=0&&verifyEvery<=0xffffffff,'Invalid camera math verification interval');
 const requested=enabled!==null&&enabled!==undefined;
 const status=readCameraMathStatus(core,requested||verifyEvery>0);
 if(!status)return;
 requireValue(typeof core.multiplayer_camera_math_cache_set_enabled==='function'&&
  typeof core.multiplayer_camera_math_cache_set_verify_every==='function','Camera math controls required');
 if(requested)requireValue(core.multiplayer_camera_math_cache_set_enabled(enabled?1:0)===Number(!!enabled),
  'Camera math cache option rejected');
 requireValue((core.multiplayer_camera_math_cache_set_verify_every(verifyEvery)>>>0)===verifyEvery,
  'Camera math verification option rejected');
}
const equalWords=(a,b)=>a.length===b.length&&a.every((value,index)=>value===b[index]);

export async function verifyCameraMathDraw({core,canonical,presentationDraw}){
 const report={schema:1,passed:false,scope:'Stopped-world exact pixels and full 44-word canonical; not performance',
  alphas:[0,.25,.5,.75,1],sequence:[
   {enabled:false,verifyEvery:0},{enabled:true,verifyEvery:0},{enabled:true,verifyEvery:1},
   {enabled:true,verifyEvery:0},{enabled:false,verifyEvery:0},{enabled:true,verifyEvery:0}],
  unverifiedHits:{lookAt:0,perspective:0},samples:[],failures:[]};
 let gl,initial,initialCanonical,nativeTime;
 const pending=[];
 const fail=(where,error)=>report.failures.push({where,message:String(error.message||error)});
 const checkGL=where=>{
  const errors=[];for(let at=0;at<32;++at){const value=gl.getError();if(!value)break;errors.push(value);}
  requireValue(!errors.length&&!gl.isContextLost(),'GL error/context loss at '+where+': '+errors.join(','));
 };
 const graphics=()=>{const pointer=core.sdl_stats();return Array.from(new Uint32Array(core.memory.buffer,pointer,17));};
 try{
  requireValue(core.mp_fixture_presentation_draw&&core.sdl_stats&&core.sdl_loop_time,'Native diagnostic Draw and clock exports required');
  initial=readCameraMathStatus(core,true);initialCanonical=canonical();nativeTime=core.sdl_loop_time();
  requireValue(initialCanonical.length===44&&initialCanonical[0]===2,'Full canonical schema 2 required');
  report.initialStatus=initial;report.canonicalBefore=initialCanonical;report.nativeTimeBefore=nativeTime;
  gl=document.getElementById('screen').getContext('webgl2');requireValue(gl,'Existing WebGL2 renderer required');
  checkGL('entry');
  const width=gl.drawingBufferWidth,height=gl.drawingBufferHeight;
  requireValue(width>0&&height>0&&width*height*4<=16*1024*1024,'Bounded RGBA8 framebuffer required');
  report.framebuffer={width,height,format:'RGBA8',origin:'bottom-left'};
  configureCameraMath(core,false,0);
  for(const alpha of report.alphas){
   let reference=null;
   for(let phase=0;phase<report.sequence.length;++phase){
    const {enabled,verifyEvery}=report.sequence[phase];
    configureCameraMath(core,enabled,verifyEvery);
    const before=readCameraMathStatus(core,true),canonicalBefore=canonical(),graphicsBefore=graphics();
    requireValue(equalWords(initialCanonical,canonicalBefore),'Canonical changed before Draw');
    const drawn=presentationDraw(alpha);
    requireValue(drawn===true||drawn===1,'Synchronous presentation Draw rejected');
    // Copy immediately in the same JS task; no promise or screenshot can run
    // between native presentation and reading its default framebuffer.
    const pixels=readDefault(gl,width,height),after=readCameraMathStatus(core,true),canonicalAfter=canonical();
    const graphicsAfter=graphics(),committed=(graphicsAfter[5]-graphicsBefore[5])>>>0;
    const row={alpha,phase,enabled,verifyEvery,statusBefore:before,statusAfter:after,canonicalBefore,canonicalAfter,
     committedPresentations:committed,pixelComparison:reference?pixelDifference(reference,pixels,width):
      {equal:true,differingBytes:0,first:null}};
    report.samples.push(row);
    for(const [label,at] of [['lookAt',5],['perspective',12]])
     report.unverifiedHits[label]+=(after[at+1]-before[at+1])-(after[at+5]-before[at+5]);
    if(!reference)reference=pixels;
    pending.push({row,pixels:row.pixelComparison.equal?reference:pixels});
    requireValue(after[11]===0&&after[18]===0,'Camera math reference mismatch');
    requireValue(committed===1,'Draw must commit exactly one new default framebuffer');
    requireValue(equalWords(canonicalBefore,canonicalAfter),'Draw changed full 44-word canonical');
    requireValue(row.pixelComparison.equal,'Same-alpha RGBA bytes differ');
    requireValue(gl.drawingBufferWidth===width&&gl.drawingBufferHeight===height,'Framebuffer resized');
    checkGL('Draw '+alpha+'/'+phase);
   }
  }
  const after=readCameraMathStatus(core,true);
  report.finalStatus=after;
  report.verified={lookAt:after[10]-initial[10],perspective:after[17]-initial[17]};
  requireValue(report.verified.lookAt>0&&report.verified.perspective>0,'Both camera paths must hit and verify');
  requireValue(report.unverifiedHits.lookAt>0&&report.unverifiedHits.perspective>0,
   'Both camera paths must also present pixels from pure cached hits');
 }catch(error){fail('oracle',error);}
 finally{
  try{
   report.firstMismatch=readCameraMathVerification(core,true);
   report.lastVerification=readCameraMathVerification(core,false);
  }catch(error){fail('verification read',error);}
  if(initial)try{
   configureCameraMath(core,!!initial[1],initial[3]);
   report.restoredStatus=readCameraMathStatus(core,true);
   requireValue(report.restoredStatus[1]===initial[1]&&report.restoredStatus[3]===initial[3],'Camera options not restored');
  }catch(error){fail('option restore',error);}
  if(initialCanonical)try{
   report.canonicalAfter=canonical();report.nativeTimeAfter=core.sdl_loop_time();
   requireValue(equalWords(initialCanonical,report.canonicalAfter),'Full canonical changed during cleanup');
   requireValue(nativeTime===report.nativeTimeAfter,'Native clock advanced in stopped-world oracle');
  }catch(error){fail('native cleanup',error);}
  if(gl)try{checkGL('cleanup');}catch(error){fail('GL cleanup',error);}
 }
 // Full byte comparisons already decided equality. Hashing is deferred until
 // every native setting and GL readback binding has been restored.
 try{
  const hashes=new Map();
  for(const item of pending)if(!hashes.has(item.pixels))hashes.set(item.pixels,
   crypto.subtle.digest('SHA-256',item.pixels).then(hash=>
    Array.from(new Uint8Array(hash),x=>x.toString(16).padStart(2,'0')).join('')));
  await Promise.all(pending.map(async item=>{item.row.pixelSha256=await hashes.get(item.pixels);}));
 }catch(error){fail('pixel digest',error);}
 report.passed=report.failures.length===0;
 return report;
}
