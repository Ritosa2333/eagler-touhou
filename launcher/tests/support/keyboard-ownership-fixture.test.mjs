/** Current keyboard fixture wiring only. Synthetic DOM/protocol ports; no
 * candidate Runtime, SDL state, multiplayer or confirmed-hash proof. */
import test, {before, after} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {readFile, mkdir, mkdtemp, writeFile, rm} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {buildOriginalComponentFixture} from './original-component-fixture.mjs';
import {installMountedDom} from '../react-main/mounted-dom-environment.mjs';
const project=fileURLToPath(new URL('../../',import.meta.url)), path='tests/test-keyboard-ownership-browser.py';
const python=source=>execFileSync(process.env.PYTHON||'python',['-c',source],{cwd:project,encoding:'utf8',env:{...process.env,PYTHONDONTWRITEBYTECODE:'1'}});
const fingerprint=async path=>createHash('sha256').update(await readFile(resolve(project,path))).digest('hex');
let fixture,adapter,work,env;
before(async()=>{
  await mkdir(resolve(project,'.cache'),{recursive:true});work=await mkdtemp(resolve(project,'.cache/keyboard-adapter-'));
  const sources=['app/services/player-keyboard.ts','src/launcher/hosted-keyboard.mts','src/launcher/touch-runtime-protocol.mts'];
  const beforeHashes=await Promise.all(sources.map(fingerprint));
  fixture=await buildOriginalComponentFixture('keyboard-ownership');
  assert.deepEqual(await Promise.all(sources.map(fingerprint)),beforeHashes,'fixture compilation must not change its input owners');
  const file=resolve(work,'fixture.mjs');await writeFile(file,fixture.module);
  env=installMountedDom();adapter=await import(pathToFileURL(file).href);
});
after(async()=>{try{if(env){assert.deepEqual(env.errors,[]);env.close();}}finally{if(work)await rm(work,{recursive:true,force:true});}});

function hostHtml(){
  return python(`import ast,json\nfrom pathlib import Path\ns=Path('${path}').read_text();t=ast.parse(s)\nf=next(n for n in t.body if isinstance(n,ast.FunctionDef) and n.name=='host_html')\ncontext={'json':json}\nexec(compile(ast.Module(body=[f],type_ignores=[]),'keyboard-host-setup','exec'),context)\nprint(context['host_html']('th06').decode())`);
}

test('keyboard fixture bundles current owners without a retired application or inherited Launcher cache',()=>{
  for(const path of ['app/services/player-keyboard.ts','src/launcher/hosted-keyboard.mts','src/launcher/touch-runtime-protocol.mts','src/launcher/touch-function-key.mts'])assert.ok(fixture.inputs.includes(path),path);
  assert.equal(fixture.inputs.some(path=>path.endsWith('/app.mts')||path.includes('original-bootstrap')),false);
  assert.equal(typeof adapter.installKeyboardOwnershipFixture,'function');
  assert.equal(typeof adapter.createFunctionKeyOwner,'function');
});

test('Python harness selects the production keyboard even with the historical default target and exposes its real module URL',()=>{
  const result=JSON.parse(python(`import json,sys,os\nfrom pathlib import Path\nsys.path.insert(0,'tests')\nfrom support.original_component_fixture import original_component_fixture\nos.environ['EAGLER_COMPONENT_TEST_TARGET']='main'\nf=original_component_fixture(Path.cwd(),'keyboard-ownership',target='react')\nprint(json.dumps({'inputs':f['inputs'],'module':bool(f['module'])}))`));
  assert.equal(result.module,true);assert.ok(result.inputs.includes('app/services/player-keyboard.ts'));
  const html=hostHtml();
  assert.match(html,/from '\/__original-component-fixture\.mjs'/);
  assert.match(html,/frame\.src='\/runtime\/'.*runtimeVariant=multiplayer&runtimeEpoch=1/);
  assert.doesNotMatch(html,/const hostedKeyboard|\/modules\//);
});

test('keyboard harness resolves fonts and candidate runtimes from its supplied repository roots',()=>{
  const paths=JSON.parse(python(`import ast,json,http.server\nfrom pathlib import Path\nfrom urllib.parse import urlparse\ns=Path('${path}').read_text();tree=ast.parse(s)\nnode=next(n for n in tree.body if isinstance(n,ast.ClassDef) and n.name=='Handler')\nscope={'http':http,'ROOT':Path.cwd(),'urlparse':urlparse}\nexec(compile(ast.Module(body=[node],type_ignores=[]),'keyboard-resource-routing','exec'),scope)\nHandler=scope['Handler'];custom=Path.cwd()/'unused-custom-input'\nHandler.repositories={'th06':custom/'game-source'};Handler.runtimes={'th06':custom/'candidate-runtime'}\nh=Handler.__new__(Handler)\nprint(json.dumps([h.translate_path('/assets/th06/msgothic.ttc'),h.translate_path('/runtime/th06/th06.wasm')]))`));
  assert.deepEqual(paths,[resolve(project,'unused-custom-input/game-source/assets/msgothic.ttc'),resolve(project,'unused-custom-input/candidate-runtime/th06.wasm')]);
});

test('native keyboard scenarios retain their independent pre-migration assertions',()=>{
  const info=JSON.parse(python(`import ast,json,subprocess\nfrom pathlib import Path\nold=subprocess.check_output(['git','show','edee9633:${path}'],text=True);new=Path('${path}').read_text()\ndef assertions(s):return [ast.get_source_segment(s,n) for n in ast.walk(ast.parse(s)) if isinstance(n,ast.Assert)]\nprint(json.dumps({'count':len(assertions(old)),'same':assertions(old)==assertions(new)}))`));
  assert.ok(info.count>0);assert.equal(info.same,true);
});

test('compiled production keyboard attaches to original fixture elements and uses the real canonical delivery boundary',()=>{
  const {document,window}=env;document.body.innerHTML='<div id="player" class="open"><iframe id="runtime"></iframe></div><button id="control">Launcher control</button>';
  const player=document.getElementById('player'),frame=document.getElementById('runtime'),control=document.getElementById('control');
  const state={launched:false,game:'th06'},messages=[];let spectator=false,touchSends=0;
  const context=()=>({target:frame.contentWindow,targetOrigin:window.location.origin,protocol:'eagler-touhou/1',game:'th06',epoch:1,launched:state.launched,ready:state.launched,spectator});
  frame.contentWindow.__eaglerDirectInputBridge={schema:'eagler-touhou/direct-input/1',protocol:'eagler-touhou/1',game:'th06',epoch:1,origin:window.location.origin,submit:message=>{messages.push(message);return true;}};
  const originalPost=frame.contentWindow.postMessage;let fallback=0;frame.contentWindow.postMessage=()=>fallback++;
  const owner=adapter.installKeyboardOwnershipFixture({state,frame,player,touchRuntimeMessageContext:context,releaseHeldTouchFire(){},
    touchFunctionOwner:adapter.createFunctionKeyOwner(()=>{touchSends++;throw new Error('Unexpected fixture touch input');})});
  const key=(type,target,changes={})=>target.dispatchEvent(new window.KeyboardEvent(type,{bubbles:true,cancelable:true,key:'Shift',code:'ShiftLeft',keyCode:16,location:1,...changes}));
  try {
    key('keydown',player);assert.equal(messages.length,0);
    state.launched=true;owner.sync();key('keydown',player);key('keyup',control,{code:'Unidentified',location:0,keyCode:0});
    assert.deepEqual(messages.map(({command,down,code})=>({command,down,code})),[{command:'keyboard',down:true,code:'ShiftLeft'},{command:'keyboard',down:false,code:'ShiftLeft'}]);
    key('keydown',player);window.dispatchEvent(new window.Event('pagehide'));assert.equal(messages.at(-1).command,'keyboard-clear');
    const before=messages.length;key('keydown',player,{repeat:true});key('keyup',control,{code:'Unidentified',location:0,keyCode:0});assert.equal(messages.length,before);
    key('keydown',player);assert.equal(messages.length,before+1,'pagehide must not dispose the owner');
    spectator=true;const count=messages.length;key('keydown',player);key('keyup',control);assert.equal(messages.length,count);
    assert.equal(touchSends,0);assert.equal(fallback,0);assert.equal(document.getElementById('runtime'),frame);
  } finally {owner.dispose();frame.contentWindow.postMessage=originalPost;}
});
