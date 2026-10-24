// Startup-only environment controls. These helpers do not run inside a measured
// rAF, install timers, or replace the game's existing WebGL context.
import assert from 'node:assert/strict';

export function environmentOptions({cpuThrottle=1,renderer='swiftshader',headless=true}={}){
  cpuThrottle=Number(cpuThrottle);
  assert.ok(Number.isFinite(cpuThrottle)&&cpuThrottle>=1&&cpuThrottle<=20,'CPU throttle must be a finite slowdown factor from 1 through 20');
  assert.ok(['swiftshader','hardware'].includes(renderer),'Renderer must be swiftshader or hardware');
  assert.equal(typeof headless,'boolean');
  return {cpuThrottle,renderer,headless};
}

export function browserLaunchSettings(options){
  const {renderer,headless}=environmentOptions(options);
  return {headless,args:[
    '--no-sandbox','--disable-extensions','--no-first-run','--autoplay-policy=no-user-gesture-required',
    ...(renderer==='swiftshader'?['--use-angle=swiftshader','--enable-unsafe-swiftshader']:['--disable-software-rasterizer']),
    '--disable-background-timer-throttling','--disable-renderer-backgrounding','--disable-backgrounding-occluded-windows'],
    // Playwright also supplies this flag by default. Hardware requests must not
    // retain that implicit permission to fall back to SwiftShader.
    ...(renderer==='hardware'?{ignoreDefaultArgs:['--enable-unsafe-swiftshader']}: {})};
}

// Serialized into the actual Runtime frame after warmup. Module.ctx is the
// context Emscripten already owns; calling getContext could create a new one in
// a broken setup and would not establish the identity of the measured context.
export function inspectRuntimeWebGL(){
  const gl=globalThis.Module?.ctx,canvas=document.querySelector('#canvas');
  if(!gl||!globalThis.WebGL2RenderingContext||!(gl instanceof WebGL2RenderingContext)||gl.canvas!==canvas)
    throw Error('The running Runtime does not expose its existing game WebGL2 context');
  if(gl.isContextLost())throw Error('The game WebGL2 context is lost');
  const debug=gl.getExtension('WEBGL_debug_renderer_info');
  return {at:performance.now(),timeOrigin:performance.timeOrigin,canvasId:canvas.id,
    vendor:gl.getParameter(gl.VENDOR),renderer:gl.getParameter(gl.RENDERER),version:gl.getParameter(gl.VERSION),
    shadingLanguageVersion:gl.getParameter(gl.SHADING_LANGUAGE_VERSION),
    unmaskedVendor:debug?gl.getParameter(debug.UNMASKED_VENDOR_WEBGL):null,
    unmaskedRenderer:debug?gl.getParameter(debug.UNMASKED_RENDERER_WEBGL):null,
    contextAttributes:gl.getContextAttributes()};
}

export function rendererIdentity(requested,systemInfo,webgl){
  environmentOptions({renderer:requested});
  const gpu=systemInfo?.gpu??{},aux=gpu.auxAttributes??{},feature=gpu.featureStatus?.webgl??null;
  const actual=String(webgl?.unmaskedRenderer??''),browser=String(aux.glRenderer??'');
  // Only active-backend evidence decides the match. The enumerated adapter
  // list can also contain an unused software device on a hardware-rendered run.
  const details=[actual,browser,aux.displayType,aux.glImplementationParts].filter(Boolean).join('\n');
  const software=/swiftshader|swangle|llvmpipe|lavapipe|softpipe|software rasterizer|software renderer|microsoft basic render|\bwarp\b/i.test(details);
  const physical=/\bintel\b|\bnvidia\b|\bamd\b|\bati\b|advanced micro devices|\bapple\b|\bqualcomm\b|\badreno\b|\bmali\b|\bpowervr\b|\bradeon\b|\bgeforce\b|\biris\b/i;
  const swiftshader=/swiftshader/i.test(actual)&&/swiftshader|swangle/i.test(browser+'\n'+aux.displayType);
  const hardware=!software&&physical.test(actual)&&physical.test(browser)&&typeof feature==='string'&&feature.startsWith('enabled');
  const classification=swiftshader?'swiftshader':software?'other-software':hardware?'hardware':'unknown';
  return {requested,classification,matched:classification===requested,
    evidence:{actualUnmaskedRenderer:actual||null,browserGlRenderer:browser||null,displayType:aux.displayType??null,
      glImplementationParts:aux.glImplementationParts??null,webglFeatureStatus:feature,softwareMarker:software,
      physicalRendererMarker:physical.test(actual),physicalBrowserMarker:physical.test(browser)},
    meaning:'The requested backend is accepted only when the existing game context and browser GPU information agree. Unknown or software fallback cannot satisfy a hardware request; these strings do not measure GPU execution time.'};
}

const clock=()=>({utc:new Date().toISOString(),unixMs:Date.now(),monotonicMs:performance.now()});
async function applyRate(target,rate,phase){
  const application={phase,rate,requestedAt:clock(),acknowledgedAt:null};target.metadata.applications.push(application);
  await target.session.send('Emulation.setCPUThrottlingRate',{rate});application.acknowledgedAt=clock();
}
async function addTarget(handle,session,role){
  const target={session,contexts:new Map(),metadata:{role,target:null,applications:[]}};
  handle.targets.push(target);handle.metadata.targets.push(target.metadata);
  session.on('Runtime.executionContextCreated',({context})=>target.contexts.set(context.id,context));
  session.on('Runtime.executionContextDestroyed',({executionContextId})=>target.contexts.delete(executionContextId));
  session.on('Runtime.executionContextsCleared',()=>target.contexts.clear());
  const {targetInfo}=await session.send('Target.getTargetInfo');
  assert.ok(targetInfo?.targetId&&['page','iframe'].includes(targetInfo.type),'Unexpected CPU-throttle target');
  target.metadata.target=targetInfo;
  await session.send('Runtime.enable');
  await applyRate(target,handle.metadata.rate,role==='page'?'before-navigation':'runtime-target-attached');
  return target;
}

export async function startCpuThrottle(page,rate){
  environmentOptions({cpuThrottle:rate});
  const handle={page,targets:[],closed:false,runtime:null,metadata:{rate,targets:[],coverage:null,verification:null,
    method:'Emulation.setCPUThrottlingRate',
    meaning:'Requested renderer slowdown factor acknowledged by CDP. Page and actual Runtime execution contexts are bound to recorded target IDs. Sessions remain attached through observation and save validation. This is not a CPU clock measurement, a physical-machine equivalence, or a GPU-process throttle; CDP provides no rate readback.'}};
  try{await addTarget(handle,await page.context().newCDPSession(page),'page');return handle;}
  catch(error){await closeCpuThrottle(handle).catch(()=>{});throw error;}
}

function frameWithUrl(tree,url){
  if(tree.frame?.url===url)return tree.frame;
  for(const child of tree.childFrames??[]){const found=frameWithUrl(child,url);if(found)return found;}
  return null;
}
function defaultContext(target,frameId){
  return [...target.contexts.values()].find(context=>context.auxData?.frameId===frameId&&context.auxData?.isDefault===true);
}
async function verifyDocument(target,context,url){
  const {result,exceptionDetails}=await target.session.send('Runtime.evaluate',{contextId:context.id,returnByValue:true,
    expression:'({href:location.href,origin:location.origin,hasNativeRuntime:typeof globalThis.Module?._th15_mp_status==="function"})'});
  assert.ok(!exceptionDetails,'Runtime CPU-throttle context evaluation failed');
  assert.equal(result?.value?.href,url,'CPU-throttle context is not the actual Runtime document');
  assert.equal(result.value.hasNativeRuntime,true,'CPU-throttle context does not contain the native Runtime');
  return result.value;
}

export async function coverRuntimeCpuThrottle(handle,frame){
  assert.ok(!handle.closed&&!handle.runtime,'Runtime CPU-throttle coverage was already established or closed');
  const url=frame.url(),pageTarget=handle.targets[0];
  const {frameTree}=await pageTarget.session.send('Page.getFrameTree');
  let target=pageTarget,document=frameWithUrl(frameTree,url),context=document&&defaultContext(target,document.id);
  if(!context){
    // A same-process frame has no distinct Playwright CDP session. We accept
    // the page target only with its default execution-context/frame-ID proof;
    // otherwise an actual separate iframe target must acknowledge the rate.
    target=await addTarget(handle,await handle.page.context().newCDPSession(frame),'runtime');
    const tree=await target.session.send('Page.getFrameTree');document=frameWithUrl(tree.frameTree,url);
    context=document&&defaultContext(target,document.id);
  }
  assert.ok(context&&document,'No default Runtime execution context belongs to the throttled CDP target');
  const actual=await verifyDocument(target,context,url);
  await applyRate(target,handle.metadata.rate,'runtime-context-verified');
  handle.runtime={frame,target,contextId:context.id,url,frameId:document.id};
  return handle.metadata.coverage={runtimeUrl:url,runtimeFrameId:document.id,contextId:context.id,
    targetId:target.metadata.target.targetId,pageTargetId:pageTarget.metadata.target.targetId,
    sharedPageTarget:target===pageTarget,pageUrl:frameTree.frame?.url??null,document:actual,verifiedAt:clock()};
}

export async function verifyCpuThrottleCoverage(handle){
  assert.ok(handle.runtime&&!handle.closed,'Runtime CPU-throttle coverage is unavailable');
  const {frame,target,contextId,url,frameId}=handle.runtime;
  assert.equal(frame.url(),url,'Runtime navigated during CPU-throttle observation');
  const {targetInfo}=await target.session.send('Target.getTargetInfo');
  assert.equal(targetInfo.targetId,target.metadata.target.targetId,'Runtime CPU-throttle target changed');
  const context=target.contexts.get(contextId);
  assert.equal(context?.auxData?.frameId,frameId,'Runtime CPU-throttle execution context changed');
  const document=await verifyDocument(target,context,url);
  return handle.metadata.verification={targetId:targetInfo.targetId,contextId,runtimeFrameId:frameId,document,verifiedAt:clock()};
}

export async function closeCpuThrottle(handle){
  if(handle.closed)return;handle.closed=true;
  const errors=[];
  for(const target of handle.targets){
    try{await target.session.send('Emulation.setCPUThrottlingRate',{rate:1});}catch(error){errors.push(String(error));}
    try{await target.session.send('Runtime.disable');}catch(error){errors.push(String(error));}
    try{await target.session.detach();}catch(error){errors.push(String(error));}
  }
  handle.metadata.closedAt=clock();handle.metadata.cleanupErrors=errors;
  if(errors.length)throw Error('CPU-throttle cleanup: '+errors.join('; '));
}
