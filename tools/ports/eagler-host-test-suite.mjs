import assert from 'node:assert/strict';
import test from 'node:test';
import {existsSync,readFileSync} from 'node:fs';
import {sep} from 'node:path';
import {fileURLToPath} from 'node:url';

function pathParts(value,label){
  const url=value instanceof URL?value:new URL(value);
  if(url.protocol!=='file:')throw new TypeError(label+' must be a local file URL');
  return fileURLToPath(url).split(sep).filter(Boolean);
}

function pathAfterPorts(value,label){
  const parts=pathParts(value,label),index=parts.lastIndexOf('ports');
  if(index<0||!parts[index+1])throw new TypeError(label+' must belong to a ports/<owner> source tree');
  return parts.slice(index+1);
}

function requireOwnerPath(value,owner,suffix,label){
  const rest=pathAfterPorts(value,label);
  if(rest[0]!==owner||rest.slice(1).join('/')!==suffix)
    throw new Error(label+' must be ports/'+owner+'/'+suffix);
}

function validateFontAliases(fontAliases){
  if(!Array.isArray(fontAliases)||fontAliases.length===0)
    throw new TypeError('Explicit non-empty fontAliases are required');
  const languages=new Set(),targets=new Set();
  return Object.freeze(fontAliases.map((item,index)=>{
    if(item===null||typeof item!=='object'||Array.isArray(item))
      throw new TypeError(`fontAliases[${index}] must be an object`);
    if(!Object.hasOwn(item,'language')||
      !(item.language===null||typeof item.language==='string'&&item.language.trim().length>0))
      throw new TypeError(`fontAliases[${index}].language must be null or a non-empty string`);
    for(const key of ['source','target']){
      if(typeof item[key]!=='string'||!item[key].startsWith('/')||item[key].includes('\\')||
        item[key].split('/').includes('..')||item[key].length===1)
        throw new TypeError(`fontAliases[${index}].${key} must be an absolute Runtime path`);
    }
    if(!item.target.startsWith('/fonts/'))
      throw new TypeError(`fontAliases[${index}].target must be under /fonts/`);
    if(item.source===item.target)throw new TypeError(`fontAliases[${index}] source and target must differ`);
    const languageKey=item.language===null?'\u0000':item.language;
    if(languages.has(languageKey))throw new Error('fontAliases languages must be unique');
    if(targets.has(item.target))throw new Error('fontAliases targets must be unique');
    languages.add(languageKey);targets.add(item.target);
    return Object.freeze({language:item.language,source:item.source,target:item.target});
  }));
}

export function validateEaglerHostTestTarget({owner,testUrl,runtimeUrl,shellUrl,fontAliases}={}){
  if(typeof owner!=='string'||!owner.trim())throw new TypeError('Explicit ports owner is required');
  const testParts=pathAfterPorts(testUrl,'testUrl');
  const game=owner.endsWith('-mp')?owner.slice(0,-3):owner;
  const variant=owner.endsWith('-mp')?'multiplayer':'normal';
  if(testParts[0]!==owner)throw new Error('Test owner does not match its source directory');
  if(testParts.slice(1).join('/')!=='portable/test-eagler-host.mjs')
    throw new Error('Host test must be the portable/test-eagler-host.mjs entry');
  requireOwnerPath(runtimeUrl,owner,'web/sdl-runtime/eagler-host.mjs','runtimeUrl');
  requireOwnerPath(shellUrl,owner,'web/sdl-runtime/shell.mjs','shellUrl');
  const paths={testUrl:new URL(testUrl),runtimeUrl:new URL(runtimeUrl),shellUrl:new URL(shellUrl)};
  for(const [label,url] of Object.entries(paths))
    if(!existsSync(fileURLToPath(url)))throw new Error(label+' does not exist in the declared ports owner');
  return Object.freeze({owner,game,variant,testUrl:new URL(testUrl).href,
    runtimeUrl:new URL(runtimeUrl).href,shellUrl:new URL(shellUrl).href,fontAliases:validateFontAliases(fontAliases)});
}

export function resolveEaglerHostTestTarget({owner,testUrl,fontAliases}={}){
  const entry=new URL(testUrl);
  const runtimeUrl=new URL('../web/sdl-runtime/eagler-host.mjs',entry);
  const shellUrl=new URL('../web/sdl-runtime/shell.mjs',entry);
  return validateEaglerHostTestTarget({owner,testUrl:entry,runtimeUrl,shellUrl,fontAliases});
}

function fakeFontModule(fontAliases){
  const files=new Set(),directories=new Set(['/']),links=new Map();
  const addParents=path=>{
    const parts=path.split('/').filter(Boolean);let current='';
    for(const part of parts.slice(0,-1)){current+='/'+part;directories.add(current);}
  };
  for(const item of fontAliases){files.add(item.source);addParents(item.source);}
  const parent=path=>path.slice(0,path.lastIndexOf('/'))||'/';
  const exists=path=>files.has(path)||directories.has(path)||links.has(path);
  const Module={FS:{
    mkdirTree(path){
      assert.equal(path,'/fonts');
      const parts=path.split('/').filter(Boolean);let current='';
      for(const part of parts){current+='/'+part;directories.add(current);}
    },
    stat(path){if(!exists(path))throw new Error('missing path '+path);return{kind:directories.has(path)?'directory':'file'};},
    unlink(path){if(!links.delete(path)&&!files.delete(path))throw new Error('missing path '+path);},
    symlink(source,target){
      if(!files.has(source)&&!links.has(source))throw new Error('missing font '+source);
      if(!directories.has(parent(target)))throw new Error('missing symlink parent '+parent(target));
      if(exists(target))throw new Error('target already exists '+target);
      links.set(target,source);
    },
  }};
  return{Module,files,directories,links};
}

export function assertFontAliasContract(fontAliases,ensureSharedFontAlias){
  const profile=validateFontAliases(fontAliases);
  if(typeof ensureSharedFontAlias!=='function')throw new TypeError('Runtime font alias function is required');
  const{Module,links}=fakeFontModule(profile),actual=[];
  for(const item of profile){
    const result=item.language===null?ensureSharedFontAlias(Module):ensureSharedFontAlias(Module,item.language);
    assert.deepEqual(result,{source:item.source,target:item.target},
      'Font alias for '+(item.language??'default')+' must match the declared Launcher paths');
    assert.equal(links.get(item.target),item.source,
      'Font alias for '+(item.language??'default')+' must link the declared source');
    actual.push(result);
  }
  return actual;
}

function filesystem(){
  const files=new Map();
  return{files,FS:{mkdirTree(){},writeFile(path,bytes){files.set(path,bytes);}}};
}

export async function registerEaglerHostTests({owner,testUrl,fontAliases}={}){
  const target=resolveEaglerHostTestTarget({owner,testUrl,fontAliases});
  const host=await import(target.runtimeUrl);
  const required=['normalizeOptions','applyTouchOptions','touchControls','suspendRuntimeAudio',
    'resumeRuntimeAudio','directTouch','resourcePath','ensureSharedFontAlias','installResources',
    'observeMusicWrites','mountManagedData','isSupersededRuntimeError'];
  const missing=required.filter(key=>typeof host[key]!=='function');
  assert.deepEqual(missing,[],target.owner+' host is missing required functions: '+missing.join(', '));
  const {normalizeOptions,applyTouchOptions,touchControls,suspendRuntimeAudio,resumeRuntimeAudio,
    directTouch,resourcePath,ensureSharedFontAlias,installResources,observeMusicWrites,
    mountManagedData,isSupersededRuntimeError}=host;
  const game=target.game,base='http://localhost/runtime/'+game+'/entry.html';

  test(target.owner+' touch snapshots do not reset gesture mode',()=>{
    const calls=[],core=new Proxy({}, {get:(_,name)=>(...args)=>calls.push([name,...args])});
    const options=normalizeOptions({touchEnabled:true,touchMovementMode:'touch',touchSensitivity:100});
    applyTouchOptions(core,options);calls.length=0;
    for(let i=0;i<60;i++)touchControls(core,options,{touchSensitivity:100,fireEnabled:true,joystickX:0,joystickY:0});
    assert.equal(calls.length,60);assert(calls.every(c=>c[0]==='sdl_touch_controls'));
    touchControls(core,options,{touchSensitivity:150,joystickX:65535,joystickY:-65535});
    assert.equal(calls.at(-2)[0],'sdl_touch_options');assert.deepEqual(calls.at(-1).slice(-2),[32767,-32767]);
    touchControls(core,options,{joystickX:16384,joystickY:NaN});
    assert.deepEqual(calls.at(-1).slice(-2),[16384,0]);
    assert(!calls.some(c=>c[0]==='sdl_touch_mode'));
  });

  test(target.owner+' foreground audio resumes before Runtime loop',async()=>{
    const calls=[],core={sdl_loop_pause:value=>calls.push(value)};
    const context={state:'suspended',async resume(){this.state='running';}};
    assert.equal(await resumeRuntimeAudio({SDL3:{audioContext:context}},core,()=>true),true);
    assert.deepEqual(calls,[0]);calls.length=0;context.state='running';
    assert.equal(await resumeRuntimeAudio({SDL3:{audioContext:context}},core,()=>true),true);
    assert.deepEqual(calls,[0]);calls.length=0;context.state='suspended';
    context.resume=async()=>{throw Error('gesture required');};
    assert.equal(await resumeRuntimeAudio({SDL3:{audioContext:context}},core,()=>true),false);
    assert.deepEqual(calls,[]);calls.length=0;
    let foreground=true;context.state='suspended';
    context.resume=async()=>{context.state='running';foreground=false;};
    assert.equal(await resumeRuntimeAudio({SDL3:{audioContext:context}},core,()=>foreground),false);
    assert.deepEqual(calls,[]);
  });

  test(target.owner+' background audio detaches and reconnects',async()=>{
    const calls=[],events=[],destination={};
    const node={disconnect(){events.push('disconnect');},connect(value){assert.equal(value,destination);events.push('connect');}};
    const context={state:'running',destination,async resume(){events.push('resume');}};
    const Module={SDL3:{audioContext:context,audio_playback:{scriptProcessorNode:node}}};
    const core={sdl_loop_pause:value=>calls.push(value)};
    assert.equal(suspendRuntimeAudio(Module,core),true);assert.deepEqual(events,['disconnect']);assert.deepEqual(calls,[1]);
    assert.equal(suspendRuntimeAudio(Module,core),true);assert.deepEqual(events,['disconnect']);assert.deepEqual(calls,[1,1]);
    calls.length=0;assert.equal(await resumeRuntimeAudio(Module,core,()=>true),true);
    assert.deepEqual(events,['disconnect','connect']);assert.deepEqual(calls,[0]);
  });

  test(target.owner+' Shell startup resumes an already-running AudioContext',()=>{
    const shell=readFileSync(new URL(target.shellUrl),'utf8').replaceAll('\r','');
    assert.match(shell,/if\(forcePause\)core\.sdl_loop_pause\(1\);\s*return resumeRuntimeAudio/);
    assert.doesNotMatch(shell,/if\(!forcePause&&\(!context\|\|context\.state==='running'\)\)return true/);
  });

  test(target.owner+' direct touch honors letterbox and viewport updates',()=>{
    const calls=[],core={sdl_touch:(...args)=>calls.push(args)};
    const canvas={getBoundingClientRect:()=>({left:80,top:0,width:640,height:480})};
    directTouch(core,canvas,{type:'down',id:3,x:.5,y:.5},{width:800,height:480});
    assert.deepEqual(calls[0],[0,3,.5,.5]);
    directTouch(core,canvas,{type:'bogus',id:3,x:0,y:0},{width:800,height:480});
    directTouch(core,canvas,{type:'move',id:3,x:NaN,y:0},{width:800,height:480});
    assert.equal(calls.length,1);
  });

  test(target.owner+' resources bind to the explicit product identity',()=>{
    assert(resourcePath('/bgm-ogg/'+game+'_01.ogg',game));
    for(const item of target.fontAliases)assert(resourcePath(item.source,game));
    for(const path of ['/bgm-ogg/../save.dat','/fonts/../../shell.mjs','/savesth08/score.dat',
      '/bgm-ogg/th06_01.ogg'])assert(!resourcePath(path,game));
  });

  test(target.owner+' shared font aliases are exercised without a title-name skip',()=>{
    assertFontAliasContract(target.fontAliases,ensureSharedFontAlias);
  });

  test(target.owner+' OGG resources install and notify native retry',async()=>{
    const module=filesystem(),path='/bgm-ogg/'+game+'_01.ogg';let notices=0;
    observeMusicWrites(module,{sdl_music_resource_changed:()=>notices++},game);
    await installResources(module,[{path,url:'./track.ogg',bytes:4}],
      {game,base,fetcher:async()=>new Response(new Uint8Array([79,103,103,83]))});
    assert.equal(notices,1);assert.equal(module.files.get(path).length,4);
    await assert.rejects(installResources(module,[{path,url:'https://remote.invalid/track.ogg'}],{game,base}),
      /same-origin/);
    await assert.rejects(installResources(module,[{path,url:'./x',bytes:5}],
      {game,base,fetcher:async()=>new Response(new Uint8Array(4))}),/size mismatch/);
  });

  test(target.owner+' managed DATA stays separate from immutable fonts',async()=>{
    const module=filesystem(),buffer=new Uint8Array(32).buffer;let request;
    await mountManagedData(module,{game,base,
      query:new URLSearchParams('managedData=1&gameGeneration=g1&runtimeEpoch=23'),
      parentWindow:{__eaglerPrepareManagedRuntimeDataV1:async value=>{request=value;return{buffer};}},
      fetcher:async()=>Response.json({schema:'eagler-sdl-resources/1',game,resources:[]})});
    assert.deepEqual(request,{game,generation:'g1',epoch:23});
    assert.equal(module.files.get('/game/'+game+'.dat').byteLength,32);
    await assert.rejects(mountManagedData(module,{game,base,
      query:new URLSearchParams('managedData=1&gameGeneration=g1'),
      parentWindow:{__eaglerPrepareManagedRuntimeDataV1:async()=>({buffer})},
      fetcher:async()=>Response.json({schema:'eagler-sdl-resources/1',game,resources:[]})}),/epoch unavailable/);
    const superseded=new DOMException('EAGLER_RUNTIME_SESSION_SUPERSEDED','AbortError');
    assert.equal(isSupersededRuntimeError(superseded),true);
    assert.equal(isSupersededRuntimeError(new DOMException('network aborted','AbortError')),false);
    await assert.rejects(mountManagedData(module,{game,base,
      query:new URLSearchParams('managedData=1&gameGeneration=g1&runtimeEpoch=23'),
      parentWindow:{__eaglerPrepareManagedRuntimeDataV1:async()=>{throw superseded;}},
      fetcher:async()=>Response.json({schema:'eagler-sdl-resources/1',game,resources:[]})}),
      error=>isSupersededRuntimeError(error));
    await assert.rejects(mountManagedData(module,{game,query:new URLSearchParams(),parentWindow:{}}),
      /eagler-touhou/);
  });

  test(target.owner+' normalizes invalid sensitivity and movement mode',()=>{
    const options=normalizeOptions({touchSensitivity:NaN,touchMovementMode:'invalid'});
    assert.equal(options.touchSensitivity,100);assert.equal(options.touchMovementMode,'touch');
    assert.equal(normalizeOptions({touchSensitivity:50}).touchSensitivity,100);
    assert.equal(normalizeOptions({touchSensitivity:99}).touchSensitivity,100);
    assert.equal(normalizeOptions({touchSensitivity:350}).touchSensitivity,300);
  });

  test(target.owner+' mobile practice panels preserve held keyboard keys',()=>{
    const core={sdl_touch_controls(){},sdl_touch_options(){}};
    const options={touchEnabled:true,touchSensitivity:100};
    const Module={eaglerControls:{thpracKeyboardBits:512}};
    touchControls(core,options,{controls:{thpracMobilePanelsEnabled:true,thpracMobilePanelsVisible:true}},Module);
    assert.deepEqual(Module.eaglerControls,
      {thpracKeyboardBits:512,thpracMobilePanelsEnabled:true,thpracMobilePanelsVisible:true});
    touchControls(core,options,{},Module);
    assert.equal(Module.eaglerControls.thpracMobilePanelsEnabled,false);
    assert.equal(Module.eaglerControls.thpracMobilePanelsVisible,false);
    assert.equal(Module.eaglerControls.thpracKeyboardBits,512);
  });
}
