import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFileSync} from 'node:fs';
import {relative,resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {headerInputs} from './runtime-inputs.mjs';
import {targets,inspectInputs,root} from '../ports/inputs.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';

const QUERY_TIMEOUT_MS=10_000;
const TEST_TIMEOUT_MS=25_000;

function nodeInputConsumers(products,availableTargets){
 const consumers=[];
 for(const [game,product] of Object.entries(products)){
  if(product.inputProfile==='fighting')continue;
  const variants=['normal',...(product.multiplayerRuntime?['multiplayer']:[])];
  for(const variant of variants){
   const target=availableTargets.find(candidate=>candidate.game===game&&candidate.variant===variant);
   if(!target)throw new Error('Missing runtime input target for '+game+'/'+variant);
   if(target.recipe.kind==='node')consumers.push(target);
  }
 }
 if(!consumers.length)throw new Error('No STG Node runtime input consumers are registered');
 return consumers;
}

function assertSharedHeaders(target,inputFiles,expectedHeaders){
 const actual=new Set(inputFiles.map(path=>resolve(path)));
 for(const path of expectedHeaders)
  assert.ok(actual.has(path),target.game+'/'+target.variant+': undeclared shared input '+path);
}

function fixtureTarget(build){
 const target=targets({game:'th08',variant:'normal'})[0];
 return{...target,recipe:{...target.recipe,kind:'node',build}};
}

async function inspectBounded(target,options={}){
 return inspectInputs(target,{timeoutMs:QUERY_TIMEOUT_MS,...options});
}

const expectedHeaders=headerInputs(resolve(root,'common/include/eagler/input')).map(path=>resolve(path));
const consumers=nodeInputConsumers(PRODUCT_GAMES,targets());

test('large input inventories finish writing to a pipe before the builder exits',{
 timeout:TEST_TIMEOUT_MS,
},async()=>{
 const helper=pathToFileURL(resolve(root,'tools/build/runtime-inputs.mjs')).href;
 const flags=Array.from({length:20000},(_,n)=>'-DPIPE_INVENTORY_'+n+'='+('x'.repeat(64)));
 const source='import {printRuntimeInputs} from '+JSON.stringify(helper)+';'+
  'const flags=Array.from({length:20000},(_,n)=>\'-DPIPE_INVENTORY_\'+n+\'=\'+\'x\'.repeat(64));'+
  'await printRuntimeInputs({workspace:process.cwd(),root:process.cwd(),sources:[],headers:[],flags});'+
  'throw Error(\'Input inspection continued into the compiler\');';
 const report=await inspectBounded(fixtureTarget(['--input-type=module','-e',source,'--']));
 const serialized=JSON.stringify(report);
 assert.ok(Buffer.byteLength(serialized)>1024*1024,'the complete inventory must exceed 1 MiB');
 assert.equal(report.schema,'eagler-touhou/runtime-inputs/1');
 assert.deepEqual(report.compilerFlags,flags);
});

function assertTh08LanguageHeader(report,label){
 const workspace=resolve(root,'ports/th08');
 const commonRoot=resolve(process.env.EAGLER_COMMON_ROOT??resolve(workspace,'../../common'));
 const headerPath=relative(workspace,resolve(commonRoot,'include/eagler/diagnostics/LanguageTrace.hpp')).replaceAll('\\','/');
 assert.equal(report.schema,'eagler-touhou/runtime-inputs/1');
 assert.ok(Object.hasOwn(report.inputs,headerPath),label+' print-inputs must expose the production observer header');
 const header=readFileSync(resolve(workspace,headerPath));
 assert.equal(report.inputs[headerPath].bytes,header.length);
 assert.equal(report.inputs[headerPath].sha256,createHash('sha256').update(header).digest('hex'));
 assert.equal(report.compilerFlags.includes('-DTH_PRESENTATION_AUDIT=1'),label==='Lab');
}

const th08Normal=targets({game:'th08',variant:'normal'})[0];
test('th08 normal print-inputs include the language observation header in their compiler closure',{
 timeout:TEST_TIMEOUT_MS,
},async()=>{
 const report=await inspectBounded(th08Normal);
 assertTh08LanguageHeader(report,'normal');
});

test('th08 Lab print-inputs include the language observation header in their compiler closure',{
 timeout:TEST_TIMEOUT_MS,
},async()=>{
 const lab={...th08Normal,recipe:{...th08Normal.recipe,build:[...th08Normal.recipe.build,'--presentation-lab']}};
 const report=await inspectBounded(lab);
 assertTh08LanguageHeader(report,'Lab');
});

test('standalone input queries remove inherited Node test/debugger context',{
 timeout:TEST_TIMEOUT_MS,
},async()=>{
 const helper=pathToFileURL(resolve(root,'tools/build/runtime-inputs.mjs')).href;
 const source='import {printRuntimeInputs} from '+JSON.stringify(helper)+';'+
  'await printRuntimeInputs({workspace:process.cwd(),root:process.cwd(),sources:[],headers:[],'+
  'flags:[process.env.NODE_OPTIONS??null,process.env.NODE_TEST_CONTEXT??null]});';
 const target=fixtureTarget(['--input-type=module','-e',source,'--']);
 const input=await inspectBounded(target,{environment:{...process.env,
  NODE_OPTIONS:'--trace-warnings',NODE_TEST_CONTEXT:'parent-node-test-context'}});
 assert.deepEqual(input.compilerFlags,[null,null]);
});

test('STG Node input consumer selection is nonempty',()=>{
 assert.ok(expectedHeaders.length>0,'the shared input contract must declare at least one common header');
 assert.ok(consumers.length>0,'the registered STG Node owners must yield executable input consumers');
});

test('input inventory selection rejects a missing applicable consumer mode',()=>{
 const th08=PRODUCT_GAMES.th08;
 const ordinaryTarget=targets({game:'th08',variant:'normal'});
 assert.throws(()=>nodeInputConsumers({th08},ordinaryTarget),/Missing runtime input target for th08\/multiplayer/);
});

test('shared-header assertion rejects an omitted compiler input',()=>{
 const target=consumers[0];
 assert.throws(()=>assertSharedHeaders(target,[],expectedHeaders),/undeclared shared input/);
});

for(const target of consumers)test(target.game+'/'+target.variant+' compiler inventory declares shared input headers',{
 timeout:TEST_TIMEOUT_MS,
},async()=>{
 const input=await inspectBounded(target);
 assertSharedHeaders(target,input.inputFiles,expectedHeaders);
});
