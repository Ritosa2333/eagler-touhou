import test from 'node:test';
import assert from 'node:assert/strict';
import {runInNewContext} from 'node:vm';
import {environmentOptions,browserLaunchSettings,inspectRuntimeWebGL,rendererIdentity,startCpuThrottle,coverRuntimeCpuThrottle,verifyCpuThrottleCoverage,closeCpuThrottle} from './performance-browser-environment.mjs';

test('default environment remains headless 1x SwiftShader and invalid rates/backends fail',()=>{
  assert.deepEqual(environmentOptions(),{cpuThrottle:1,renderer:'swiftshader',headless:true});
  assert.equal(environmentOptions({cpuThrottle:'4'}).cpuThrottle,4);
  for(const rate of [0,.5,Infinity,NaN,'bad',21])assert.throws(()=>environmentOptions({cpuThrottle:rate}),/CPU throttle/);
  assert.throws(()=>environmentOptions({renderer:'auto'}),/Renderer/);
});

test('hardware launch removes explicit and Playwright-default SwiftShader permission',()=>{
  const hardware=browserLaunchSettings({renderer:'hardware',headless:false}),software=browserLaunchSettings({renderer:'swiftshader'});
  assert.equal(hardware.headless,false);assert.ok(hardware.args.includes('--disable-software-rasterizer'));
  assert.ok(hardware.args.every(arg=>!arg.includes('swiftshader')));
  assert.deepEqual(hardware.ignoreDefaultArgs,['--enable-unsafe-swiftshader']);
  assert.ok(software.args.includes('--use-angle=swiftshader'));assert.equal(software.ignoreDefaultArgs,undefined);
});

function gpu(renderer='ANGLE (Intel, Intel(R) UHD Graphics Direct3D11 vs_5_0 ps_5_0, D3D11)',displayType='ANGLE_D3D11'){
  return {gpu:{auxAttributes:{glRenderer:renderer,displayType,glImplementationParts:'(gl=egl-angle,angle=d3d11)'},
    featureStatus:{webgl:'enabled'},devices:[{vendorString:'Intel',deviceString:'Intel UHD'}]}};
}

test('hardware identity requires the actual game context and browser GPU evidence to agree',()=>{
  const system=gpu(),actual={unmaskedRenderer:system.gpu.auxAttributes.glRenderer};
  assert.equal(rendererIdentity('hardware',system,actual).matched,true);
  system.gpu.devices.push({deviceString:'Unused SwiftShader Device'});
  assert.equal(rendererIdentity('hardware',system,actual).matched,true);
  assert.equal(rendererIdentity('swiftshader',system,actual).matched,false);
  const missing=rendererIdentity('hardware',system,{renderer:'WebKit WebGL'});
  assert.equal(missing.matched,false);assert.equal(missing.classification,'unknown');
  system.gpu.featureStatus.webgl='disabled_software';assert.equal(rendererIdentity('hardware',system,actual).matched,false);
});

test('software fallback or an unknown renderer never satisfies a hardware request',()=>{
  const swift='ANGLE (Google, Vulkan 1.3 (SwiftShader Device (Subzero)), SwiftShader driver)',system=gpu(swift,'ANGLE_SWIFTSHADER');
  assert.equal(rendererIdentity('hardware',system,{unmaskedRenderer:swift}).matched,false);
  assert.equal(rendererIdentity('swiftshader',system,{unmaskedRenderer:swift}).matched,true);
  const llvmpipe='ANGLE (Mesa, llvmpipe LLVM 19)';
  assert.equal(rendererIdentity('hardware',gpu(),{unmaskedRenderer:llvmpipe}).classification,'other-software');
  assert.equal(rendererIdentity('swiftshader',gpu(llvmpipe,'ANGLE_OPENGL'),{unmaskedRenderer:llvmpipe}).matched,false);
  assert.equal(rendererIdentity('hardware',gpu('Mystery GPU','ANGLE_VULKAN'),{unmaskedRenderer:'Mystery GPU'}).classification,'unknown');
});

function webglFixture({missing=false,lost=false}={}){
  const calls=[],canvas={id:'canvas',getContext(){throw Error('Must not create or reacquire a context');}},context={
    performance:{now:()=>12,timeOrigin:1000},document:{querySelector:()=>canvas},WebGL2RenderingContext:class {
      constructor(){this.canvas=canvas;this.VENDOR=1;this.RENDERER=2;this.VERSION=3;this.SHADING_LANGUAGE_VERSION=4;}
      isContextLost(){return lost;}
      getExtension(name){calls.push(['extension',name]);return {UNMASKED_VENDOR_WEBGL:5,UNMASKED_RENDERER_WEBGL:6};}
      getParameter(key){calls.push(['parameter',key]);return 'value-'+key;}
      getContextAttributes(){return {alpha:false};}
    }};
  context.Module={ctx:missing?null:new context.WebGL2RenderingContext()};
  return {context,calls};
}

test('renderer inspection reads Emscripten existing context without calling getContext',()=>{
  const f=webglFixture(),result=runInNewContext('('+inspectRuntimeWebGL.toString()+')()',f.context);
  assert.equal(result.unmaskedRenderer,'value-6');assert.equal(result.canvasId,'canvas');
  assert.equal(f.calls.filter(call=>call[0]==='parameter').length,6);
  assert.throws(()=>runInNewContext('('+inspectRuntimeWebGL.toString()+')()',webglFixture({missing:true}).context),/existing game WebGL2 context/);
  assert.throws(()=>runInNewContext('('+inspectRuntimeWebGL.toString()+')()',webglFixture({lost:true}).context),/context is lost/);
});

function throttleFixture({oopif=false,missingContext=false,failure=null}={}){
  const events=[],url='http://127.0.0.1/runtime/th15.html',pageUrl='http://127.0.0.1/peer/0';
  function makeSession(targetId,role){
    const handlers=new Map(),runtime=role==='runtime',contextId=runtime?4:2;
    const frame={id:'runtime-frame',url},session={
      on(event,callback){handlers.set(event,callback);},
      emit(event,data){handlers.get(event)?.(data);},
      async send(method,args){
        events.push({targetId,method,args});if(method===failure)throw Error('Rejected '+method);
        if(method==='Target.getTargetInfo')return {targetInfo:{targetId,type:runtime?'iframe':'page',url:runtime?url:pageUrl}};
        if(method==='Runtime.enable'){
          if(!missingContext&&(!oopif||runtime))session.emit('Runtime.executionContextCreated',{context:{id:contextId,auxData:{frameId:'runtime-frame',isDefault:true}}});
          return {};
        }
        if(method==='Page.getFrameTree')return {frameTree:runtime?{frame}:{frame:{id:'page-frame',url:pageUrl},childFrames:[{frame}]}};
        if(method==='Runtime.evaluate')return {result:{value:{href:url,origin:'http://127.0.0.1',hasNativeRuntime:true}}};
        return {};
      },async detach(){events.push({targetId,method:'detach'});},
    };return session;
  }
  const pageSession=makeSession('page-target','page'),runtimeSession=makeSession('runtime-target','runtime');
  let liveUrl=url;const frame={url:()=>liveUrl},page={context:()=>({newCDPSession:async target=>{
    if(target===page)return pageSession;
    assert.equal(target,frame);if(!oopif)throw Error('No separate Runtime target');return runtimeSession;
  }})};
  return {page,frame,events,pageSession,runtimeSession,navigate:newUrl=>{liveUrl=newUrl;}};
}

test('same-target Runtime is bound by default execution context, frame ID and native export before rate reapplication',async()=>{
  const f=throttleFixture(),handle=await startCpuThrottle(f.page,4),coverage=await coverRuntimeCpuThrottle(handle,f.frame);
  assert.equal(coverage.sharedPageTarget,true);assert.equal(coverage.targetId,'page-target');
  assert.equal(coverage.runtimeFrameId,'runtime-frame');assert.equal(coverage.contextId,2);assert.equal(coverage.document.hasNativeRuntime,true);
  const applications=handle.metadata.targets[0].applications;
  assert.deepEqual(applications.map(value=>[value.phase,value.rate]),[['before-navigation',4],['runtime-context-verified',4]]);
  assert.ok(applications.every(value=>value.acknowledgedAt.monotonicMs>=value.requestedAt.monotonicMs));
  await verifyCpuThrottleCoverage(handle);assert.equal(handle.metadata.verification.contextId,2);
  await closeCpuThrottle(handle);assert.equal(handle.closed,true);assert.deepEqual(handle.metadata.cleanupErrors,[]);
  assert.deepEqual(f.events.slice(-3).map(event=>event.method),['Emulation.setCPUThrottlingRate','Runtime.disable','detach']);
  assert.equal(f.events.at(-3).args.rate,1);
});

test('OOPIF Runtime receives its own target rate and cannot inherit an unproved Page target',async()=>{
  const f=throttleFixture({oopif:true}),handle=await startCpuThrottle(f.page,4),coverage=await coverRuntimeCpuThrottle(handle,f.frame);
  assert.equal(coverage.sharedPageTarget,false);assert.equal(coverage.targetId,'runtime-target');assert.equal(coverage.contextId,4);
  assert.equal(handle.targets.length,2);
  assert.ok(f.events.some(event=>event.targetId==='runtime-target'&&event.method==='Emulation.setCPUThrottlingRate'&&event.args.rate===4));
  await verifyCpuThrottleCoverage(handle);await closeCpuThrottle(handle);
  assert.equal(f.events.filter(event=>event.method==='detach').length,2);
});

test('missing actual Runtime context fails rather than claiming same-origin coverage',async()=>{
  const f=throttleFixture({missingContext:true}),handle=await startCpuThrottle(f.page,4);
  await assert.rejects(coverRuntimeCpuThrottle(handle,f.frame),/No separate Runtime target/);
  assert.equal(handle.metadata.coverage,null);await closeCpuThrottle(handle);
});

test('a rejected CPU throttle command cleans up its session and produces no acknowledged rate',async()=>{
  const f=throttleFixture({failure:'Emulation.setCPUThrottlingRate'});
  await assert.rejects(startCpuThrottle(f.page,4),/Rejected Emulation/);
  assert.equal(f.events.at(-1).method,'detach');
});

test('Runtime navigation or replaced execution context invalidates the recorded throttle coverage',async()=>{
  const f=throttleFixture(),handle=await startCpuThrottle(f.page,4);await coverRuntimeCpuThrottle(handle,f.frame);
  f.pageSession.emit('Runtime.executionContextDestroyed',{executionContextId:2});
  await assert.rejects(verifyCpuThrottleCoverage(handle),/execution context changed/);
  f.navigate('http://127.0.0.1/other');await assert.rejects(verifyCpuThrottleCoverage(handle),/Runtime navigated/);
  await closeCpuThrottle(handle);
});
