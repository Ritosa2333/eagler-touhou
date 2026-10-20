import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {copyFile,mkdir,mkdtemp,readFile,rm,writeFile} from 'node:fs/promises';
import {execFileSync} from 'node:child_process';
import {dirname,join,resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {captureRuntimePackageControlInputs,captureRuntimePackageFiles,RUNTIME_PACKAGE_CONTROL_FILES,resolveRuntimePackagePlan} from '../../launcher/lib/runtime-package-evidence.mjs';

const repository=resolve(import.meta.dirname,'../..');
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const publicInputs=[
  'ports/th075-mp/portable/package-eagler.mjs',
  'ports/th075-mp/portable/content-bundle.mjs',
  'ports/th075-mp/web/sdl-runtime/managed.html',
  'ports/th075-mp/web/sdl-runtime/shell.mjs',
  'ports/th075-mp/web/sdl-runtime/managed.css',
  'ports/th075-mp/web/sdl-runtime/keyboard.mjs',
  'ports/th075-mp/web/sdl-runtime/directory-keyboard.mjs',
  'ports/th075-mp/web/sdl-runtime/display.mjs',
  'ports/th075-mp/web/sdl-runtime/save-sync.mjs',
  'ports/th075-mp/web/sdl-runtime/multiplayer.mjs',
  'common/browser/adonis-calibration.mjs',
];

async function copyInput(root,path){
  const destination=resolve(root,path);
  await mkdir(dirname(destination),{recursive:true});
  await copyFile(resolve(repository,path),destination);
}

async function createFixture(t){
  const root=await mkdtemp(join(tmpdir(),'th075-package-fixture-'));
  t.after(()=>rm(root,{recursive:true,force:true}));
  for(const path of new Set([...RUNTIME_PACKAGE_CONTROL_FILES,...publicInputs]))await copyInput(root,path);
  return{root,source:resolve(root,'ports/th075-mp'),common:resolve(root,'common')};
}

async function writeBuildFixture(source,variant){
  const directory=resolve(source,'web/artifacts',variant==='multiplayer'?'sdl3-multiplayer':'sdl3');
  const wasm=Buffer.from([0x00,0x61,0x73,0x6d,0x01,0x00,0x00,0x00]);
  const builtAt='2026-10-20T00:00:00.000Z';
  await mkdir(directory,{recursive:true});
  await writeFile(resolve(directory,'th075.wasm'),wasm);
  await writeFile(resolve(directory,'th075.mjs'),'export default function createRuntime(){ return {}; }\n');
  const build={builtAt,multiplayer:variant==='multiplayer',completeGame:true,eagler:true,kind:'th075-sdl3-web-release',
    bytes:wasm.length,sha256:hash(wasm),sources:{},headers:{}};
  const buildPath=resolve(directory,'build.json');
  await writeFile(buildPath,JSON.stringify(build,null,2)+'\n');
  return{directory,buildPath,build,wasm};
}

function assertManifestIdentity(manifest,variant){
  const product=variant==='multiplayer'?'th075mp':'th075';
  assert.equal(manifest.profile,variant==='multiplayer'?'multiplayer':'production',`manifest profile for ${variant}`);
  assert.deepEqual({product:manifest.product,variant:manifest.variant,multiplayer:manifest.multiplayer},
    {product,variant,multiplayer:variant==='multiplayer'});
}

async function runOwnerPackager(source,common,variant,{checkManifestIdentity=true}={}){
  const script=resolve(source,'portable/package-eagler.mjs');
  const{build,wasm}=await writeBuildFixture(source,variant);
  const args=variant==='multiplayer'?['--multiplayer']:[];
  execFileSync(process.execPath,[script,...args],{cwd:source,windowsHide:true,stdio:'pipe',
    env:{...process.env,EAGLER_COMMON_ROOT:common}});
  const output=resolve(source,variant==='multiplayer'?'build-eagler-multiplayer':'build-eagler');
  const packageIdentity=captureRuntimePackageFiles(output,{game:'th075',variant,entry:'th075.html'});
  const expectedBase=['th075.html','manifest.json','shell.mjs','managed.css','keyboard.mjs','directory-keyboard.mjs',
    'display.mjs','content-bundle.mjs','save-sync.mjs','th075.mjs','th075.wasm'];
  const expectedFiles=[...expectedBase,...(variant==='multiplayer'?['multiplayer.mjs','adonis-calibration.mjs']:[]),'runtime-files.json'].sort();
  assert.deepEqual(Object.keys(packageIdentity.files).sort(),expectedFiles);
  assert.equal(packageIdentity.inventory.product,variant==='multiplayer'?'th075mp':'th075');
  assert.equal(packageIdentity.inventory.variant,variant);
  assert.equal(packageIdentity.inventory.files['th075.wasm'].sha256,build.sha256);
  assert.equal(packageIdentity.files['th075.wasm'].sha256,hash(wasm));
  if(variant==='multiplayer')assert.equal(packageIdentity.files['adonis-calibration.mjs'].sha256,
    hash(await readFile(resolve(common,'browser/adonis-calibration.mjs'))));
  const manifest=JSON.parse(await readFile(resolve(output,'manifest.json'),'utf8'));
  if(checkManifestIdentity)assertManifestIdentity(manifest,variant);
  const html=await readFile(resolve(output,'th075.html'),'utf8');
  assert.ok(html.includes(`<meta name="eagler-runtime-variant" content="${variant}">`));
  assert.ok(html.includes(`<meta name="eagler-runtime-product" content="${variant==='multiplayer'?'th075mp':'th075'}">`));
  return{build,output,script,manifest};
}

test('th075 MP source owner normal branch seals its package identity',async t=>{
  const{source,common}=await createFixture(t);
  await runOwnerPackager(source,common,'normal');
});

test('th075 MP source owner multiplayer branch seals its package identity and rejects a normal build receipt',async t=>{
  const{source,common}=await createFixture(t);
  const{script}=await runOwnerPackager(source,common,'multiplayer');
  const multiplayerBuild=await readFile(resolve(source,'web/artifacts/sdl3-multiplayer/build.json'),'utf8');
  const wrongMode={...JSON.parse(multiplayerBuild),multiplayer:false};
  await writeFile(resolve(source,'web/artifacts/sdl3-multiplayer/build.json'),JSON.stringify(wrongMode));
  assert.throws(()=>execFileSync(process.execPath,[script,'--multiplayer'],{cwd:source,windowsHide:true,stdio:'pipe',
    env:{...process.env,EAGLER_COMMON_ROOT:common}}),error=>String(error.stderr).includes('Runtime variant mismatch'));
});

test('th075 MP identity assertion rejects the prior production profile',async t=>{
  const{source,common}=await createFixture(t);
  const script=resolve(source,'portable/package-eagler.mjs');
  const original=await readFile(script,'utf8');
  const correctProfile="profile=multiplayer?'multiplayer':'production'";
  assert.ok(original.includes(correctProfile),'fixture packager must expose the multiplayer profile selection');
  await writeFile(script,original.replace(correctProfile,"profile='production'"));
  const{manifest}=await runOwnerPackager(source,common,'multiplayer',{checkManifestIdentity:false});
  assert.throws(()=>assertManifestIdentity(manifest,'multiplayer'),/manifest profile for multiplayer/);
});

test('th075 MP package recipe tracks the shared Adonis calibration control input',async t=>{
  const{root}=await createFixture(t);
  const plan=resolveRuntimePackagePlan('th075','multiplayer',root);
  assert.deepEqual(plan.sharedControlFiles,['common/browser/adonis-calibration.mjs']);
  const path=resolve(root,'common/browser/adonis-calibration.mjs');
  const before=captureRuntimePackageControlInputs(plan,root).find(input=>input.path==='common/browser/adonis-calibration.mjs');
  await writeFile(path,`${await readFile(path,'utf8')}\n// fixture change\n`);
  const after=captureRuntimePackageControlInputs(plan,root).find(input=>input.path==='common/browser/adonis-calibration.mjs');
  assert.notEqual(after.sha256,before.sha256);
});
