import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {test} from 'node:test';
import {projectEmscriptenWebLoader,projectLocalOnlineDiscovery,projectManagedProgramIndex} from '../tools/project-browser-programs.mjs';

const probe='globalThis.process?.versions?.node&&globalThis.process?.type!="renderer"';
const guard='if(ENVIRONMENT_IS_NODE){const{createRequire}=await import("module");var require=createRequire(import.meta.url)}';
const loader=(initializer=probe,body=guard)=>`async function Module(){var ENVIRONMENT_IS_NODE=${initializer};${body}return {node:ENVIRONMENT_IS_NODE,require:typeof require}}export default Module;`;

test('local browser projection removes the pinned Node bootstrap even with a real Node global',async()=>{
  const code=projectEmscriptenWebLoader(loader());
  assert.doesNotMatch(code,/import\("module"\)/);
  const {default:factory}=await import(`data:text/javascript;base64,${Buffer.from(code).toString('base64')}`);
  assert.deepEqual(await factory(),{node:false,require:'undefined'});
});

test('unrecognized Node initialization or side effects are rejected instead of being deleted',()=>{
  for(const source of [loader('true'),loader(probe,guard.replace('var require=','sideEffect();var require=')),
    loader(probe,guard+guard),loader().replace('export default Module','export default other')])
    assert.throws(()=>projectEmscriptenWebLoader(source),/Unrecognized Emscripten/);
});

test('only the isolated import Host disables online resource discovery without a phantom dependency',async()=>{
  const source=await readFile(new URL('../../th128/site/runtime/online-resources.mjs',import.meta.url),'utf8');
  const projected=projectLocalOnlineDiscovery(source);
  assert.doesNotMatch(projected,/new URL\('deployment\.json'/);
  assert.match(projected,/current\(signal,isCurrent\);return null;/);
  assert.match(projected,/export async function loadOnlineResources/);
  assert.throws(()=>projectLocalOnlineDiscovery('export async function readOnlineDeployment(){}'),/Unrecognized/);
});

test('packed managed program index has the same owned URL and a statically verifiable path',async()=>{
  const source=await readFile(new URL('../runtime/managed-program.mjs',import.meta.url),'utf8');
  const projected=projectManagedProgramIndex(source);
  assert.match(projected,/new URL\('\.\.\/\.\.\/\.\.\/runtime\/program\.json',import\.meta\.url\)/);
  assert.match(projected,/index\.href===base\.href\+'runtime\/program\.json'/);
  assert.throws(()=>projectManagedProgramIndex(projected),/Unrecognized/);
});
