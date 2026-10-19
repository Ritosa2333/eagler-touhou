import assert from 'node:assert/strict';
import {test} from 'node:test';
import {selectedPortGames} from '../../launcher/tests/support/port-selection.mjs';
import {STATUS_SCHEMA,checkStates,retainedVerification,validateStatusReport} from './status.mjs';
import {targets,dependencyRevision,runtimeDependencyInputs,sourceIdentity,inspectInputs} from './inputs.mjs';
import {mkdtempSync,mkdirSync,writeFileSync,rmSync} from 'node:fs';
import {parseRuntimeDependencyArgs,planRuntimeDependencyPreparation,executeRuntimeDependencyCommands} from '../../launcher/scripts/prepare-runtime-dependencies.mjs';
import {tmpdir} from 'node:os';
import {resolve,dirname} from 'node:path';

test('th20 compiler include precedence uses the same module order on every platform',()=>{
  const input=inspectInputs(targets({game:'th20',variant:'normal'})[0]);
  const modules=input.compilerFlags.map(flag=>flag.replaceAll('\\','/'))
    .filter(flag=>/-I.*\/ports\/th20\/src\/[^/]+$/.test(flag));
  assert.ok(modules.some(flag=>flag.endsWith('/game_session')));
  assert.ok(modules.some(flag=>flag.endsWith('/gameplay')));
  assert.deepEqual(modules,[...modules].sort());
});

test('source identity normalizes line endings and preserves non-UTF-8 source bytes',()=>{
  const directory=mkdtempSync(resolve(tmpdir(),'eagler-source-identity-')),file=resolve(directory,'source.mjs');
  try {
    writeFileSync(file,'line\n');const lf=sourceIdentity([file]);
    writeFileSync(file,'line\r\n');assert.equal(sourceIdentity([file]),lf);
    writeFileSync(file,Buffer.from([0xff]));const first=sourceIdentity([file]);
    writeFileSync(file,Buffer.from([0xfe]));assert.notEqual(sourceIdentity([file]),first);
    writeFileSync(file,Buffer.from([0,0x0d,0x0a,0xff]));const binaryCrlf=sourceIdentity([file]);
    writeFileSync(file,Buffer.from([0,0x0a,0xff]));
    assert.notEqual(sourceIdentity([file]),binaryCrlf,'binary CRLF and LF bytes remain distinct');
  } finally {
    assert.equal(dirname(directory),resolve(tmpdir()));rmSync(directory,{recursive:true,force:true});
  }
});
import {adaptationMarkdown,preserveManualEvidence,MANUAL_EVIDENCE} from '../docs/new-adaptation.mjs';

test('regenerating source status preserves hand-written runtime acceptance records',()=>{
  const generated=adaptationMarkdown({game:'th20',directory:'ports/th20',variants:['normal']});
  const notes=`${MANUAL_EVIDENCE}\n\nDISPLAY-04: device, build identity, steps and evidence link\n`;
  const updated=preserveManualEvidence(generated,`old generated table\n${notes}`);
  assert.ok(updated.includes('| BUILD-01 |'));
  assert.equal(updated.slice(updated.indexOf(MANUAL_EVIDENCE)),notes);
  assert.equal(preserveManualEvidence(generated,''),generated);
});

test('single-game inspection is explicit and ordinary tests retain full coverage',()=>{
  assert.deepEqual(selectedPortGames(['th06','th07'],{}),['th06','th07']);
  assert.deepEqual(selectedPortGames(['th06','th07'],{EAGLER_PORT_CHECK_GAMES:'["th07"]'}),['th07']);
  for (const value of ['[]','["th99"]','"th06"','null'])
    assert.throws(()=>selectedPortGames(['th06'],{EAGLER_PORT_CHECK_GAMES:value}));
  assert.throws(()=>targets({game:'th99'}));
  assert.throws(()=>targets({game:'th20',variant:'multiplayer'}));
});

test('failed evidence wins, partial checks cannot complete an acceptance item',()=>{
  const state=gates=>checkStates({port:{game:'th08'},variant:'normal',checks:[{id:'RUNTIME-06'}],gates,notApplicableFor:()=>null})['RUNTIME-06'].status;
  const gate={id:'option-adapter',checks:['RUNTIME-06'],status:'passed'};
  assert.equal(state([{...gate,complete:false}]),'待核对');
  assert.equal(state([gate]),'已实现');
  assert.equal(state([gate,{...gate,id:'native-endpoint',status:'failed'}]),'检查失败');
});

test('executed build evidence survives regeneration and expires when its source changes',()=>{
  const gate={id:'compiled-runtime',checks:['BUILD-03'],level:'build',status:'passed',complete:true};
  const previous={sourceDigest:'a'.repeat(64),gates:[gate]};
  assert.deepEqual(retainedVerification(previous,'a'.repeat(64)),[gate]);
  assert.deepEqual(retainedVerification(previous,'b'.repeat(64)),[]);
  const state=checkStates({port:{game:'th08'},variant:'normal',checks:[{id:'BUILD-03'}],gates:[gate],notApplicableFor:()=>null});
  assert.equal(state['BUILD-03'].status,'已验证');
});

test('required dependency pins and captured source bytes affect status identity',()=>{
  const directory=mkdtempSync(resolve(tmpdir(),'eagler-status-dependencies-'));
  const path='ports/th06/vendored/SDL_image',sourcePath=`${path}/src/image.c`,file=resolve(directory,sourcePath);
  try {
    mkdirSync(dirname(file),{recursive:true});
    writeFileSync(file,'image source\n');
    const required=[{path,commit:'a'.repeat(40)}],captured=[{path:sourcePath,sha256:'0'.repeat(64)}];
    const identity=(modules=required,sources=captured)=>{
      const inputs=runtimeDependencyInputs(modules,sources,directory);
      return sourceIdentity(inputs.inputFiles,{dependencies:inputs.dependencies});
    };
    const baseline=identity();
    assert.deepEqual(runtimeDependencyInputs(required,captured,directory).dependencies,[{path,revision:'a'.repeat(40)}]);
    assert.notEqual(identity([{path,commit:'b'.repeat(40)}]),baseline,'a required Git pin is part of status identity');
    writeFileSync(file,'changed required source\n');
    assert.notEqual(identity(),baseline,'required dependency source bytes are part of status identity');
  } finally {
    assert.equal(dirname(directory),resolve(tmpdir()));
    rmSync(directory,{recursive:true,force:true});
  }
});

test('dependency preparation deduplicates shared source commands using a synthetic plan',()=>{
  const sourceRoot=resolve(tmpdir(),'missing-runtime-source');
  const requiredSubmodules=[{path:'vendored/SDL'},{path:'vendored/SDL_image',requiredSubmodules:[
    {path:'external/libpng'},{path:'external/zlib'}]},
    {path:'vendored/SDL_ttf',requiredSubmodules:[{path:'external/freetype'},
      {path:'external/plutovg'},{path:'external/plutosvg'}]}];
  const config={games:{
    th06:{workspaceRepository:'shared',sourceBuild:{requiredSubmodules},variants:{normal:{}}},
    th07:{workspaceRepository:'shared',sourceBuild:{requiredSubmodules},variants:{normal:{}}},
  }};
  const layout={path:repository=>resolve(sourceRoot,repository)};
  const options=parseRuntimeDependencyArgs(['--game=th06,th07,th06','--variant=normal']);
  const plan=planRuntimeDependencyPreparation({...options,config,layout});
  assert.deepEqual(plan.games,['th06','th07']);
  assert.equal(plan.sources.length,1,'the same workspace source is planned once');
  assert.equal(plan.commands.length,3,'shared parent and nested module commands are deduplicated');
  assert.deepEqual(plan.commands[0].args,['submodule','update','--init','--',
    'vendored/SDL','vendored/SDL_image','vendored/SDL_ttf']);
  assert.ok(plan.commands.every(command=>!command.args.includes('--recursive')));
  const calls=[];
  executeRuntimeDependencyCommands(plan.commands,command=>calls.push(command));
  assert.deepEqual(calls,plan.commands);
  assert.throws(()=>parseRuntimeDependencyArgs(['--game=th06,,th07']),/empty/);
  assert.throws(()=>planRuntimeDependencyPreparation({...options,games:['th99'],config,layout}),/unknown Runtime build/);
});

test('reports reject invented targets, duplicate gates and nonexistent acceptance items',()=>{
  const target={game:'th08',variant:'normal',sourceDigest:'a'.repeat(64),gates:[
    {id:'native',checks:['INPUT-04'],level:'source-contract',status:'passed'}]};
  const check=targets=>validateStatusReport({schema:STATUS_SCHEMA,targets},{expectedTargets:[target],checkIds:new Set(['INPUT-04'])});
  assert.deepEqual(check([target]),[]);
  assert.ok(check([{...target,game:'th99'}]).some(issue=>issue.includes('未登记')));
  assert.ok(check([{...target,gates:[...target.gates,...target.gates]}]).some(issue=>issue.includes('重复')));
  assert.ok(check([{...target,gates:[{...target.gates[0],checks:['INPUT-99']}]}]).some(issue=>issue.includes('未登记适配')));
  assert.ok(check([{...target,sourceDigest:null}]).some(issue=>issue.includes('源码身份')));
});
