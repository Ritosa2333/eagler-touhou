import test from 'node:test';
import assert from 'node:assert/strict';
import {existsSync} from 'node:fs';
import {mkdtemp,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {isAbsolute, resolve} from 'node:path';
import {parseWindowsCompilerHelpers,profiles, suites, trackedFiles, testFiles, repository} from '../../tests/suites.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {MUSIC_BROWSER_GAMES,validateMusicManifest} from '../ports/music-browser-helpers.mjs';
import {isNodeTest} from './discovery.mjs';

test('tooling discovery includes every executable tool regression exactly once and excludes fixture programs',()=>{
  const files = trackedFiles();
  const expected = [...files.filter(file=>file.startsWith('tools/') && isNodeTest(file)),
    ...files.filter(file=>file.startsWith('tests/adaptation/')&&isNodeTest(file))];
  const catalog = suites(), [task] = catalog.tooling.tasks();
  assert.ok(expected.length > 0);
  assert.deepEqual(task.args.filter(arg=>isNodeTest(arg)), expected);
  for (const file of ['tools/workspace.test.mjs','tools/ports/package-plan.test.mjs','tools/ports/acceptance-errors.test.mjs'])
    assert.ok(task.args.includes(file), `required regression is not executed: ${file}`);
  assert.equal(new Set(task.args).size, task.args.length);
  assert.deepEqual(testFiles(['tools/nested/check.test.mjs','tools/fixtures/do-not-run.test.mjs'], 'tools', /\.test\.mjs$/), ['tools/nested/check.test.mjs']);
  assert.throws(()=>testFiles(['tools/fixtures/do-not-run.test.mjs'], 'tools', /\.test\.mjs$/), /No test files/);
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

test('Windows compiler helper declarations are strict and attach only to declared native tasks',()=>{
  const helper={path:'C:\\VS\\VC\\Tools\\vctip.exe',sha256:'A'.repeat(64)};
  const uncHelper={path:'\\\\build-host\\tool-share\\vctip.exe',sha256:'B'.repeat(64)};
  const raw=JSON.stringify([helper,uncHelper]);
  const environment={EAGLER_TEST_WINDOWS_COMPILER_HELPERS:raw};
  const parsed=parseWindowsCompilerHelpers(environment,'win32');
  assert.deepEqual(parsed,[{path:helper.path,sha256:'a'.repeat(64)},{path:uncHelper.path,sha256:'b'.repeat(64)}]);
  assert.throws(()=>parseWindowsCompilerHelpers(environment,'linux'),/supported only on Windows/);
  assert.throws(()=>suites({environment,platform:'linux'}),/supported only on Windows/);
  for(const invalid of [
    '{',
    JSON.stringify({path:helper.path,sha256:helper.sha256}),
    JSON.stringify([{path:'relative\\vctip.exe',sha256:helper.sha256}]),
    JSON.stringify([{path:'\\vctip.exe',sha256:helper.sha256}]),
    JSON.stringify([{path:'/vctip.exe',sha256:helper.sha256}]),
    JSON.stringify([{path:'C:vctip.exe',sha256:helper.sha256}]),
    JSON.stringify([{path:'\\\\build-host',sha256:helper.sha256}]),
    JSON.stringify([{path:helper.path,sha256:'bad'}]),
    JSON.stringify([{...helper,extra:true}]),
    JSON.stringify([helper,helper]),
  ]) assert.throws(()=>parseWindowsCompilerHelpers({EAGLER_TEST_WINDOWS_COMPILER_HELPERS:invalid},'win32'),
    /EAGLER_TEST_WINDOWS_COMPILER_HELPERS/);
  assert.throws(()=>suites({environment:{EAGLER_TEST_WINDOWS_COMPILER_HELPERS:'{}'},platform:'win32'}),
    /must be a JSON array/);

  const catalog=suites({environment,platform:'win32'});
  const selected=[
    ...catalog['common-native'].tasks(),
    ...catalog['browser-shell'].tasks(),
    ...catalog['ports-source'].tasks(),
    ...catalog['ports-unit'].tasks(),
  ];
  const cleanupTasks=selected.filter(task=>task.cleanupImages);
  assert.deepEqual(cleanupTasks.map(task=>task.id).sort(),[
    'common-build','common-configure','common-test','native-gamepad','ports-source',
    'th06-cooperative-rules','th07-cooperative-rules',
  ].sort());
  for(const task of cleanupTasks)assert.deepEqual(task.cleanupImages,parsed,task.id);
  for(const task of catalog['common-native'].tasks())
    assert.equal(task.environment.MSBUILDDISABLENODEREUSE,'1',task.id);
  assert.ok(!selected.find(task=>task.id==='golden-integrity').cleanupImages);

  const noHelpers=suites({environment:{}});
  for(const [suiteId,taskId] of [['common-native','common-configure'],['browser-shell','native-gamepad'],['ports-source','ports-source'],['ports-unit','th06-cooperative-rules']])
    assert.ok(!noHelpers[suiteId].tasks().find(task=>task.id===taskId).cleanupImages,`${taskId} declared helpers by default`);
});

test('acceptance requires explicit private inputs and selects the complete product/player matrix',()=>{
  const empty = suites({environment:{}});
  assert.throws(()=>empty['runtime-browser'].tasks(), /EAGLER_TEST_RUNTIME_MANIFEST/);
  assert.throws(()=>empty['runtime-rooms'].tasks(), /EAGLER_TEST_RUNTIME_MANIFEST/);
  for(const id of ['runtime-desktop','runtime-practice','runtime-replay','runtime-music','runtime-offline','runtime-spectator'])
    assert.throws(()=>empty[id].tasks(), /EAGLER_TEST_RUNTIME_MANIFEST/, id);
  assert.throws(()=>suites({environment:{EAGLER_TEST_RUNTIME_MANIFEST:'private-inputs.json'}})['runtime-music'].tasks(), /EAGLER_TEST_MUSIC_MANIFEST/);
  assert.throws(()=>suites({environment:{EAGLER_TEST_RUNTIME_MANIFEST:'private-inputs.json'}})['runtime-rooms'].tasks(), /EAGLER_TEST_LAUNCHER_URL/);
  const catalog = suites({environment:{EAGLER_TEST_RUNTIME_MANIFEST:resolve(repository, 'private-inputs.json'), EAGLER_TEST_LAUNCHER_URL:'http://127.0.0.1:8130/'}});
  const [ordinary] = catalog['runtime-browser'].tasks();
  assert.deepEqual(ordinary.args.find(arg=>arg.startsWith('--game=')).slice(7).split(','), Object.keys(PRODUCT_GAMES));
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
  assert.equal(music.args[0],'tools/ports/music-browser.mjs');
  assert.deepEqual(music.args.find(arg=>arg.startsWith('--game=')).slice(7).split(','),MUSIC_BROWSER_GAMES);
  assert.ok(music.args.includes('--variant=normal'));
  assert.ok(music.args.includes('--manifest=private-inputs.json'));
  assert.ok(music.args.includes('--music-manifest=private-music-inputs.json'));
  assert.ok(music.resources.includes('browser')&&music.resources.includes('runtime'));
  assert.match(catalog['runtime-music'].scope,/title OGG Runtime PCM and native track identity where exposed/);
  assert.match(catalog['runtime-music'].scope,/AUDIO-01 full coverage is reported separately/);
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
