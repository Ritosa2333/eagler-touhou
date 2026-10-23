import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {existsSync,readFileSync} from 'node:fs';
import {mkdir,mkdtemp,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {isAbsolute, resolve} from 'node:path';
import {parseWindowsCompilerHelpers,profiles, suites, trackedFiles, testFiles, repository, runtimeBrowserStgGames, touchBombFixtureOwners} from '../../tests/suites.mjs';
import {parseWindowsCompilerHelpers as parseCompilerHelpers} from './compiler-helpers.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {MUSIC_BROWSER_GAMES,validateMusicManifest} from '../ports/music-browser-helpers.mjs';
import {gamepadTargetFor} from '../ports/gamepad-probe.mjs';
import {isNodeTest} from './discovery.mjs';

test('tooling discovery includes every executable tool regression exactly once and excludes fixture programs',()=>{
  const files = trackedFiles();
  const expected = [...files.filter(file=>file.startsWith('tools/') && isNodeTest(file)),
    ...files.filter(file=>file.startsWith('tests/adaptation/')&&isNodeTest(file))];
  const catalog = suites(), [task] = catalog.tooling.tasks();
  assert.ok(task.args.includes('--test-timeout=30000'),'tooling must bound individual test execution');
  assert.equal(task.timeoutMs,180000,'the outer tooling task retains its independent deadline');
  assert.ok(expected.length > 0);
  assert.deepEqual(task.args.filter(arg=>isNodeTest(arg)), expected);
  for (const file of ['tools/workspace.test.mjs','tools/ports/package-plan.test.mjs','tools/ports/acceptance-errors.test.mjs'])
    assert.ok(task.args.includes(file), `required regression is not executed: ${file}`);
  assert.equal(new Set(task.args).size, task.args.length);
  assert.deepEqual(testFiles(['tools/nested/check.test.mjs','tools/fixtures/do-not-run.test.mjs'], 'tools', /\.test\.mjs$/), ['tools/nested/check.test.mjs']);
  assert.throws(()=>testFiles(['tools/fixtures/do-not-run.test.mjs'], 'tools', /\.test\.mjs$/), /No test files/);
});

test('Presentation Lab regressions enter the CI component suite once, including newly discovered shared tests',()=>{
  const catalog=suites(),tasks=catalog['ports-unit'].tasks();
  const expected=[...trackedFiles().filter(file=>file.startsWith('common/testkit/presentation-lab/')&&isNodeTest(file)),
    'ports/th08/portable/presentation-lab/test-analyzer.mjs'];
  const task=tasks.find(item=>item.id==='presentation-lab-components');
  assert.ok(task);assert.deepEqual(task.args.slice(1),expected);
  for(const file of expected)assert.equal(tasks.flatMap(item=>item.args).filter(arg=>arg===file).length,1,file);
});

test('catalog-derived touch-bomb WASI tasks execute each production owner once and preserve variant identity',()=>{
  const output=resolve(repository,'artifacts','touch-bomb-catalog-test');
  const tasks=suites({output})['ports-touch-wasi'].tasks(),owners=touchBombFixtureOwners();
  const production=owners.filter(owner=>owner.status==='production-fixture');
  const references=owners.filter(owner=>owner.status==='reference-only');
  assert.ok(references.length>0);
  assert.deepEqual(tasks.map(task=>task.id),production.map(owner=>`touch-bomb-${owner.sourceOwner}`));
  for(let index=0;index<production.length;index++){
    const owner=production[index],task=tasks[index];
    assert.deepEqual(task.args,[resolve(owner.sourceRoot,'portable/input/touch-bomb-check.mjs')]);
    assert.equal(task.environment.EAGLER_TOUCH_BOMB_TASK_ID,task.id);
    assert.equal(task.environment.EAGLER_TOUCH_BOMB_SOURCE_OWNER,owner.sourceOwner);
    assert.deepEqual(task.environment.EAGLER_TOUCH_BOMB_VARIANTS.split(','),owner.variants);
    assert.equal(task.environment.EAGLER_TOUCH_BOMB_REPORT_DIRECTORY,
      resolve(output,'touch-bomb-reports',owner.sourceOwner));
    assert.equal(task.timeoutMs,240000);
  }
  assert.ok(references.every(owner=>!tasks.some(task=>task.id===`touch-bomb-${owner.sourceOwner}`)),
    'reference-only owners stay references and do not receive invented executables');
});

test('a new production touch owner cannot enter the catalog without its old C++ fixture and wrapper',async t=>{
  const root=await mkdtemp(resolve(tmpdir(),'touch-bomb-new-owner-'));
  t.after(()=>rm(root,{recursive:true,force:true}));
  const repositoryPath='ports/future-stg',input=resolve(root,repositoryPath,'portable/input');
  await mkdir(input,{recursive:true});
  await writeFile(resolve(input,'TouchController.hpp'),'// production controller marker\n');
  await writeFile(resolve(input,'MotionTrack.hpp'),'// production motion marker\n');
  const options={root,products:{'future-stg':{inputProfile:'stg'}},repositories:{'future-stg':repositoryPath}};
  assert.throws(()=>touchBombFixtureOwners(options),/Production touch-bomb owner future-stg is missing: portable\/input\/touch-bomb-check\.cpp, portable\/input\/touch-bomb-check\.mjs/);
  await writeFile(resolve(input,'touch-bomb-check.cpp'),'// existing fixture marker\n');
  assert.throws(()=>touchBombFixtureOwners(options),/Production touch-bomb owner future-stg is missing: portable\/input\/touch-bomb-check\.mjs/);
  await writeFile(resolve(input,'touch-bomb-check.mjs'),'// wrapper marker\n');
  assert.equal(touchBombFixtureOwners(options)[0].status,'production-fixture');
});

test('TH09 frozen smoothness comparison has a private-input package entry, not a synthetic unit task',()=>{
  const evidenceFile='ports/th09/web/tests/multiplayer/smoothness-window.test.mjs';
  const unit=suites()['ports-unit'].tasks().find(item=>item.id==='per-title-module-unit-fixtures');
  const webPackage=JSON.parse(readFileSync(resolve(repository,'ports/th09/web/package.json'),'utf8'));
  assert.ok(unit);
  assert.ok(!unit.args.includes(evidenceFile));
  assert.equal(webPackage.scripts['test:smoothness-window-evidence'],
    'node --test tests/multiplayer/smoothness-window.test.mjs');
});

test('profiles resolve to real bounded commands and never silently select zero tests',()=>{
  const catalog = suites();
  for (const [name, selected] of Object.entries(profiles)) {
    assert.ok(selected.length, name);
    assert.equal(new Set(selected).size, selected.length, name);
    for (const id of selected) assert.ok(catalog[id], `${name}: missing ${id}`);
  }
  for (const [id, suite] of Object.entries(catalog)) {
    if (suite.tier === 'acceptance') continue;
    const tasks = suite.tasks();
    assert.ok(tasks.length, id);
    for (const task of tasks) {
      assert.ok(Number.isSafeInteger(task.timeoutMs) && task.timeoutMs > 0, id);
      assert.ok(isAbsolute(task.cwd) && existsSync(task.cwd), id);
      if (task.command === process.execPath) for (const file of task.args.filter(arg=>/\.(?:mjs|py)$/.test(arg)))
        assert.ok(existsSync(resolve(task.cwd, file)), `${id}: missing entrypoint ${file}`);
    }
  }
});

test('Windows compiler helper parser remains exported and cleanup attaches to individual native tasks',()=>{
  const helper={path:'C:\\VS\\VC\\Tools\\vctip.exe',sha256:'A'.repeat(64)};
  const raw=JSON.stringify([helper]);
  const environment={EAGLER_TEST_WINDOWS_COMPILER_HELPERS:raw};
  const parsed=parseWindowsCompilerHelpers(environment,'win32');
  assert.equal(parseWindowsCompilerHelpers,parseCompilerHelpers);
  assert.deepEqual(parsed,[{path:helper.path,sha256:'a'.repeat(64)}]);
  assert.throws(()=>suites({environment,platform:'linux'}),/supported only on Windows/);

  const catalog=suites({environment,platform:'win32'});
  const selected=[
    ...catalog.tooling.tasks(),
    ...catalog['common-native'].tasks(),
    ...catalog['browser-shell'].tasks(),
    ...catalog['ports-source'].tasks(),
    ...catalog['ports-unit'].tasks(),
  ];
  const cleanupTasks=selected.filter(task=>task.cleanupImages);
  assert.deepEqual(cleanupTasks.map(task=>task.id).sort(),[
    'common-build','common-configure','common-test','native-gamepad','touch-native-invariants',
    'th06-cooperative-rules','th07-cooperative-rules','tooling',
  ].sort());
  assert.ok(!selected.find(task=>task.id==='ports-source').cleanupImages,
    'ports-source delegates helper cleanup to its executable child gates');
  for(const task of cleanupTasks)assert.deepEqual(task.cleanupImages,parsed,task.id);
  for(const task of catalog['common-native'].tasks())
    assert.equal(task.environment.MSBUILDDISABLENODEREUSE,'1',task.id);
  assert.ok(!selected.find(task=>task.id==='golden-integrity').cleanupImages);

  const noHelpers=suites({environment:{}});
  for(const [suiteId,taskId] of [['tooling','tooling'],['common-native','common-configure'],['browser-shell','native-gamepad'],['browser-shell','touch-native-invariants'],['ports-source','ports-source'],['ports-unit','th06-cooperative-rules']])
    assert.ok(!noHelpers[suiteId].tasks().find(task=>task.id===taskId).cleanupImages,`${taskId} declared helpers by default`);
});

test('acceptance requires explicit private inputs and selects the complete product/player matrix',()=>{
  const empty = suites({environment:{}});
  assert.throws(()=>empty['runtime-browser'].tasks(), /EAGLER_TEST_RUNTIME_MANIFEST/);
  assert.throws(()=>empty['runtime-rooms'].tasks(), /EAGLER_TEST_RUNTIME_MANIFEST/);
  for(const id of ['runtime-desktop','runtime-practice','runtime-replay','runtime-offline','runtime-spectator'])
    assert.throws(()=>empty[id].tasks(), /EAGLER_TEST_RUNTIME_MANIFEST/, id);
  const [music]=empty['runtime-music'].tasks();
  assert.deepEqual(music.args.slice(0,4),['tests/run.mjs','--profile=adaptation','--suite=music','--variant=normal'],
    'music input preflight and blocked products are reported by the canonical adaptation queue');
  assert.deepEqual(music.environment,{EAGLER_TEST_RUNTIME_MANIFEST:'',EAGLER_TEST_MUSIC_MANIFEST:''},
    'an empty selection cannot borrow private inputs from the parent process');
  assert.throws(()=>suites({environment:{EAGLER_TEST_RUNTIME_MANIFEST:'private-inputs.json'}})['runtime-rooms'].tasks(), /EAGLER_TEST_LAUNCHER_URL/);
  const catalog = suites({environment:{EAGLER_TEST_RUNTIME_MANIFEST:resolve(repository, 'private-inputs.json'), EAGLER_TEST_LAUNCHER_URL:'http://127.0.0.1:8130/'}});
  const [ordinary] = catalog['runtime-browser'].tasks();
  assert.deepEqual(ordinary.args.find(arg=>arg.startsWith('--game=')).slice(7).split(','), runtimeBrowserStgGames());
  assert.match(catalog['runtime-browser'].scope,/ordinary STG/);
  assert.ok(ordinary.args.includes('--gamepad'));
  assert.ok(ordinary.args.includes('--variant=normal'));
  const rooms = catalog['runtime-rooms'].tasks();
  assert.equal(rooms.length, 4);
  for (const task of rooms) {
    const players = Number(task.args.find(arg=>arg.startsWith('--players=')).slice(10));
    const expected = Object.entries(PRODUCT_GAMES).filter(([, product])=>product.multiplayer?.playerCounts.includes(players)).map(([game])=>game);
    assert.deepEqual(task.args.find(arg=>arg.startsWith('--game=')).slice(7).split(','), expected);
    assert.ok(task.args.includes('--disconnect') && task.args.includes('--gamepad') && task.args.includes('--lifecycle'));
  }
});

test('runtime-browser and gamepad targets follow new STG product registration',()=>{
  const products={...PRODUCT_GAMES,'future-stg':{inputProfile:'stg'},'future-fighting':{inputProfile:'fighting'}};
  assert.deepEqual(runtimeBrowserStgGames(products),[...runtimeBrowserStgGames(),'future-stg']);
  assert.ok(!runtimeBrowserStgGames().includes('th075'));
  assert.throws(()=>gamepadTargetFor('future-stg','normal',0),/Unsupported gamepad target/);
  assert.deepEqual(gamepadTargetFor('future-stg','normal',0,{products}),
    {game:'future-stg',variant:'normal',localSeat:0,mappingSeat:0});
  assert.throws(()=>gamepadTargetFor('future-fighting','normal',0,{products}),/Unsupported gamepad target/);
  assert.throws(()=>gamepadTargetFor('future-stg','multiplayer',0,{products}),/multiplayer gamepad seats/);
  const multiplayerProducts={'new-coop':{multiplayerRuntime:'./new-coop-mp.html',
    multiplayer:{gameplay:'cooperative',playerCounts:[2,4]}}};
  assert.equal(gamepadTargetFor('new-coop','multiplayer',3,{products:multiplayerProducts}).mappingSeat,0);
  assert.throws(()=>gamepadTargetFor('new-coop','multiplayer',4,{products:multiplayerProducts}),/invalid local seat/);
});

test('behavior acceptance schedules each title and keeps distinct user journeys mandatory',()=>{
  const environment={EAGLER_TEST_RUNTIME_MANIFEST:'private-inputs.json',EAGLER_TEST_LAUNCHER_URL:'http://127.0.0.1:8130/',
    EAGLER_TEST_PACKAGES:'private-packages.json',EAGLER_TEST_ARTWORK:'private-artwork',EAGLER_TEST_MUSIC_MANIFEST:'private-music-inputs.json'};
  const catalog=suites({environment}),games=Object.keys(PRODUCT_GAMES);
  for(const id of ['runtime-desktop','runtime-practice','runtime-replay']){
    assert.ok(profiles.acceptance.includes(id), `${id} is absent from acceptance`);
    const [task]=catalog[id].tasks();
    assert.deepEqual(task.args.find(arg=>arg.startsWith('--game=')).slice(7).split(','),games);
    assert.ok(task.args.includes('--variant=normal'));
    assert.ok(task.resources.includes('browser')&&task.resources.includes('runtime'));
  }
  const [music]=catalog['runtime-music'].tasks();
  assert.ok(profiles.acceptance.includes('runtime-music'));
  assert.equal(music.args[0],'tests/run.mjs');
  assert.ok(music.args.includes('--profile=adaptation')&&music.args.includes('--suite=music'));
  assert.ok(!music.args.some(arg=>arg.startsWith('--game=')),'the canonical catalog owns product enumeration');
  assert.ok(music.args.includes('--variant=normal'));
  assert.deepEqual(music.environment,{EAGLER_TEST_RUNTIME_MANIFEST:'private-inputs.json',
    EAGLER_TEST_MUSIC_MANIFEST:'private-music-inputs.json'});
  assert.ok(music.resources.includes('browser')&&music.resources.includes('runtime'));
  assert.match(catalog['runtime-music'].scope,/shared adaptation plan/);
  const offline=catalog['runtime-offline'].tasks();
  assert.deepEqual(offline.map(task=>task.args.find(arg=>arg.startsWith('--game=')).slice(7)),games);
  assert.equal(new Set(offline.map(task=>task.args.find(arg=>arg.startsWith('--output=')))).size,games.length);
  assert.throws(()=>suites({environment:{EAGLER_TEST_RUNTIME_MANIFEST:'private-inputs.json'}})['runtime-offline'].tasks(),/EAGLER_TEST_PACKAGES/);
  const [spectator]=catalog['runtime-spectator'].tasks();
  assert.ok(spectator.args.includes('--spectator'));
  assert.deepEqual(spectator.args.find(arg=>arg.startsWith('--game=')).slice(7).split(','),
    Object.entries(PRODUCT_GAMES).filter(([,product])=>product.multiplayer?.playerCounts.includes(2)).map(([game])=>game));
  for(const id of ['runtime-offline','runtime-spectator'])assert.ok(profiles.acceptance.includes(id));
});

test('standard music entry records every product as blocked when private inputs are absent',async()=>{
  const output=await mkdtemp(resolve(tmpdir(),'eagler-music-catalog-'));
  try{
    const [task]=suites({output,environment:{}})['runtime-music'].tasks();
    const result=spawnSync(task.command,task.args,{cwd:task.cwd,encoding:'utf8',windowsHide:true,timeout:10000,
      env:{...process.env,...task.environment,NODE_OPTIONS:''},maxBuffer:1024*1024});
    assert.equal(result.error,undefined,String(result.error));
    assert.equal(result.status,1,result.stdout+'\n'+result.stderr);
    const report=JSON.parse(readFileSync(resolve(output,'runtime-music-adaptation/summary.json'),'utf8'));
    assert.equal(report.status,'incomplete');assert.equal(report.passed,false);
    assert.equal(report.executed,0);
    assert.deepEqual([...new Set(report.cases.map(item=>item.game))].sort(),Object.keys(PRODUCT_GAMES).sort());
    assert.ok(report.cases.every(item=>item.status==='blocked'));
    assert.equal(report.coverage.complete,false);
  }finally{await rm(output,{recursive:true,force:true});}
});

test('music acceptance selects every registered input and rejects any missing selected game',()=>{
  const games=Object.fromEntries(MUSIC_BROWSER_GAMES.map(game=>{
    const contract=PRODUCT_GAMES[game].package;
    const sourceDirectory=resolve(repository,'games','web-content',game,
      ...contract.musicSourceDirectories.ogg.split(/[\\/]+/).filter(Boolean));
    return [game,{sourceDirectory,mount:contract.musicMounts.ogg,files:[{
      name:'fixture.ogg',path:resolve(sourceDirectory,'fixture.ogg'),bytes:1,sha256:'0'.repeat(64),
    }]}];
  }));
  const manifest={schema:'eagler-touhou/music-inputs/1',resourcePolicy:'private-local-paths',games};
  assert.deepEqual(Object.keys(validateMusicManifest(manifest,{repoRoot:repository,games:MUSIC_BROWSER_GAMES})),MUSIC_BROWSER_GAMES);
  for(const missingGame of MUSIC_BROWSER_GAMES){
    const incomplete=structuredClone(manifest);delete incomplete.games[missingGame];
    assert.throws(()=>validateMusicManifest(incomplete,{repoRoot:repository,games:MUSIC_BROWSER_GAMES}),
      new RegExp(`${missingGame}: OGG resources are unavailable`));
  }
});

test('offline update is an explicit five-title suite with its own second-generation input',async t=>{
  const directory=await mkdtemp(resolve(tmpdir(),'offline-update-suite-'));t.after(()=>rm(directory,{recursive:true,force:true}));
  const updateManifest=resolve(directory,'updates.json');
  const games=['th08','th10','th11','th15','th20'];
  await writeFile(updateManifest,JSON.stringify({games:Object.fromEntries(games.map(game=>[game,{}]))}));
  const environment={EAGLER_TEST_RUNTIME_MANIFEST:'private-inputs.json',EAGLER_TEST_PACKAGES:'private-packages.json',
    EAGLER_TEST_ARTWORK:'private-artwork',EAGLER_TEST_UPDATE_PACKAGES:updateManifest};
  const suite=suites({environment})['runtime-offline-update'];
  const tasks=suite.tasks();
  assert.deepEqual(tasks.map(task=>task.args.find(arg=>arg.startsWith('--game=')).slice(7)),games);
  assert.ok(tasks.every(task=>task.args.includes(`--update-manifest=${updateManifest}`)));
  assert.ok(tasks.every(task=>task.resources.includes('browser')&&task.resources.includes('launcher-build')));
  assert.ok(!profiles.acceptance.includes('runtime-offline-update'));
  assert.throws(()=>suites({environment:{}})['runtime-offline-update'].tasks(),/EAGLER_TEST_UPDATE_PACKAGES/);
});
