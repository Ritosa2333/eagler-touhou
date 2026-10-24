import assert from 'node:assert/strict';
import test from 'node:test';
import {existsSync,readFileSync} from 'node:fs';
import {mkdtemp,writeFile,rm,mkdir} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join,resolve} from 'node:path';
import {PRODUCT_GAMES,PRODUCT_IDS} from '../../common/contracts/product-catalog.mjs';
import {MUSIC_BROWSER_GAMES} from '../../tools/ports/music-browser-helpers.mjs';
import {musicAdaptationIds} from '../../tools/ports/music-browser-result.mjs';
import {desktopAdaptationIdsForMode} from '../../tools/ports/runtime-desktop-lifecycle.mjs';
import {buildAdaptationPlan} from './catalog.mjs';
import {createDiagnosticsRegistry,resolveDiagnosticProfile} from './adapters/diagnostics.mjs';
import {resolveAdaptationAdapter,resolvePracticeAdapter} from './adapters/registry.mjs';
import {profiles,suites} from '../suites.mjs';

const root=resolve(import.meta.dirname,'../..');
const env=(values={})=>({EAGLER_TEST_RUNTIME_MANIFEST:'runtime-input.json',...values});

test('practice Host and UI requirements schedule real package inputs while parameter-only reruns stay focused',()=>{
  for(const check of ['PRACTICE-01','PRACTICE-02']){
    const blocked=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal',checks:[check]}).cases[0];
    assert.deepEqual(blocked.blockedInputs.map(item=>item.name),['EAGLER_TEST_PACKAGES','EAGLER_TEST_ARTWORK']);
    const ready=buildAdaptationPlan({root,environment:env({EAGLER_TEST_PACKAGES:'packages.json',EAGLER_TEST_ARTWORK:'artwork'}),games:['th08'],variant:'normal',checks:[check]}).cases[0];
    assert.equal(ready.requiresLauncherBuild,true);assert.ok(ready.task.args.includes('--launcher-ui'));
  }
  const parameters=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal',checks:['PRACTICE-04']}).cases[0];
  assert.equal(parameters.status,'ready');assert.equal(parameters.requiresLauncherBuild,false);assert.ok(!parameters.task.args.includes('--launcher-ui'));
});

test('runtime protocol and installed-resource ownership select their actual separate journeys',()=>{
  const plan=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal'});
  const runtime=plan.cases.find(item=>item.group==='runtime');
  assert.deepEqual(runtime.checks,['RUNTIME-01','RUNTIME-02','RUNTIME-03','RUNTIME-06','INPUT-04','INPUT-06','REPLAY-05','SAVE-01']);
  const gamepad=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal',groups:['runtime'],checks:['INPUT-04']});
  assert.deepEqual(gamepad.cases.map(item=>item.group),['runtime']);
  assert.ok(gamepad.cases[0].task.args.includes('--gamepad'));
  assert.ok(plan.coverageTargets[0].requirements.some(item=>item.id==='RUNTIME-05'&&!item.reason));
  const ownership=plan.cases.filter(item=>item.checks.includes('RUNTIME-05'));
  assert.equal(ownership.length,1);
  assert.equal(ownership[0].group,'offline');
  assert.ok(ownership[0].checks.includes('PACKAGE-01'));
  assert.equal(ownership[0].status,'blocked','Resource ownership needs real package and artwork inputs');
});

test('product and variant expansion preserves normal and multiplayer applicability',()=>{
  const plan=buildAdaptationPlan({root,output:join(tmpdir(),'adaptation-plan-test'),environment:env(),groups:['replay']});
  assert.equal(plan.cases.length,Object.keys(PRODUCT_GAMES).length*2);
  assert.equal(plan.cases.filter(item=>item.variant==='normal').length,Object.keys(PRODUCT_GAMES).length);
  assert.equal(plan.cases.filter(item=>item.variant==='multiplayer').length,Object.keys(PRODUCT_GAMES).length);
  assert.ok(plan.cases.filter(item=>item.variant==='multiplayer'&&!PRODUCT_GAMES[item.game].multiplayerRuntime)
    .every(item=>item.status==='not-applicable'&&item.task===null));
  assert.ok(plan.cases.every(item=>item.checks.every(check=>typeof check==='string')&&!Object.hasOwn(item,'complete')));
  assert.equal(new Set(plan.cases.map(item=>item.id)).size,plan.cases.length);
  assert.ok(PRODUCT_IDS.length>=Object.keys(PRODUCT_GAMES).length);
});

test('unknown product and group selections fail while unknown check mapping stays visible',()=>{
  assert.throws(()=>buildAdaptationPlan({root,environment:env(),games:['th99'],groups:['runtime']}),/Unknown adaptation product/);
  assert.throws(()=>buildAdaptationPlan({root,environment:env(),groups:['unknown']}),/unknown adaptation group/);
  const plan=buildAdaptationPlan({root,environment:env(),games:['th06'],groups:['runtime'],checks:['BUILD-03']});
  assert.deepEqual(plan.cases,[]);
  assert.deepEqual(plan.unmappedChecks,[{id:'BUILD-03',reason:'No selected scenario group maps this adaptation check'}]);
});

test('whole-product verification retains every checklist requirement, including missing drivers',()=>{
  const plan=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal'});
  assert.equal(plan.scope,'product');assert.equal(plan.coverageTargets.length,1);
  const target=plan.coverageTargets[0];
  assert.ok(target.requirements.some(check=>check.id==='DISPLAY-03'&&!check.reason));
  assert.ok(target.requirements.some(check=>check.id==='MP-01'&&check.reason));
  assert.ok(target.requirements.some(check=>check.id==='BUILD-03'));
  assert.ok(plan.cases.some(item=>item.checks.includes('DISPLAY-03')&&item.status==='blocked'),
    'a registered diagnostic driver still needs explicit local inputs');
  const build=plan.cases.find(item=>item.group==='build');
  assert.ok(build?.checks.includes('BUILD-03')&&build.task?.args.includes('--stage=verify-existing'),
    'build checks revalidate existing producer reports without invoking build stages');
  assert.ok(build?.task.resources.includes('runtime')&&!build?.task.resources.includes('browser'));
  const scenes=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal',groups:['touch-motion']});
  assert.equal(scenes.scope,'scenes');
  assert.deepEqual(scenes.coverageTargets[0].requirements.map(check=>check.id),scenes.cases[0].checks);
});

test('build group revalidates one selected existing report pair without browser or Launcher preparation',()=>{
  const ready=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal',groups:['build']});
  assert.equal(ready.cases.length,1);
  const item=ready.cases[0];
  assert.equal(item.group,'build');assert.equal(item.status,'ready');
  assert.deepEqual(item.checks,['BUILD-01','BUILD-02','BUILD-03','BUILD-04']);
  assert.ok(item.task.args.includes('--stage=verify-existing'));
  assert.ok(item.task.args.includes('--game=th08')&&item.task.args.includes('--variant=normal'));
  assert.ok(item.task.args.some(argument=>argument==='--runtime-manifest=runtime-input.json'));
  assert.deepEqual(item.task.resources,['runtime']);assert.equal(item.requiresLauncherBuild,false);
  const missing=buildAdaptationPlan({root,environment:{},games:['th08'],variant:'normal',groups:['build']}).cases[0];
  assert.equal(missing.status,'blocked');assert.equal(missing.task,null);
  assert.deepEqual(missing.blockedInputs.map(input=>input.name),['EAGLER_TEST_RUNTIME_MANIFEST']);
  const unsupported=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal',groups:['build'],checks:['BUILD-05']});
  assert.equal(unsupported.cases.length,0);assert.deepEqual(unsupported.unmappedChecks,[
    {id:'BUILD-05',reason:'No selected scenario group maps this adaptation check'},
  ]);
});

test('shared touch selection includes every applicable product and keeps missing MP adapters blocked',()=>{
  const plan=buildAdaptationPlan({root,environment:env(),groups:['touch-motion']});
  const applicable=Object.values(PRODUCT_GAMES).filter(product=>product.inputProfile!=='fighting');
  assert.equal(plan.cases.filter(item=>item.status!=='not-applicable').length,
    applicable.reduce((count,product)=>count+1+(product.multiplayerRuntime?1:0),0));
  assert.ok(plan.cases.filter(item=>item.variant==='normal'&&item.status!=='not-applicable')
    .every(item=>item.status==='blocked'&&item.task===null||item.task?.args.includes('--touch-only')));
  assert.ok(plan.cases.filter(item=>item.variant==='normal'&&!['th08','th10'].includes(item.game)&&item.status!=='not-applicable')
    .every(item=>item.status==='blocked'&&/missing:/.test(item.reason)));
  assert.ok(plan.cases.filter(item=>item.variant==='normal'&&['th08','th10'].includes(item.game))
    .every(item=>item.status==='ready'&&item.task.args.includes('--touch-only')&&!Object.hasOwn(item,'complete')));
  assert.ok(plan.cases.filter(item=>item.variant==='multiplayer'&&item.status!=='not-applicable')
    .every(item=>item.status==='blocked'&&item.task===null));
  const combined=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal',groups:['desktop-input','touch-motion']});
  assert.deepEqual(combined.cases.map(item=>item.group),['desktop-input'],'the full input scene already executes shared touch checks');
});

test('action scenes select the pilot consumer and expose every missing normal or MP interface',()=>{
  const plan=buildAdaptationPlan({root,environment:env(),groups:['touch-actions']});
  const pilot=plan.cases.find(item=>item.game==='th08'&&item.variant==='normal');
  assert.equal(pilot.status,'ready');assert.ok(pilot.task.args.includes('--actions-only'));
  assert.ok(pilot.checks.includes('TOUCH-08'));
  for(const item of plan.cases.filter(item=>item!==pilot&&PRODUCT_GAMES[item.game].inputProfile!=='fighting')){
    const unsupported=item.variant==='multiplayer'&&!PRODUCT_GAMES[item.game].multiplayerRuntime;
    assert.equal(item.status,unsupported?'not-applicable':'blocked');assert.equal(item.task,null);
  }
  const fighting=buildAdaptationPlan({root,environment:env(),games:['th075'],variant:'normal',groups:['touch-motion','touch-actions']});
  assert.equal(new Set(fighting.cases.map(item=>item.id)).size,2);
  assert.ok(fighting.cases.every(item=>item.status==='not-applicable'));
});

test('audio latency plans the one pilot and exposes all missing product adapters',()=>{
  const plan=buildAdaptationPlan({root,environment:env(),groups:['audio-latency']});
  assert.equal(plan.cases.length,Object.keys(PRODUCT_GAMES).length*2);
  const pilot=plan.cases.find(item=>item.game==='th08'&&item.variant==='normal');
  assert.equal(pilot.status,'ready');
  assert.ok(pilot.task.args.includes('--audio-latency-only'));
  assert.ok(pilot.checks.includes('AUDIO-02'));
  for(const item of plan.cases.filter(item=>item!==pilot)){
    assert.equal(item.status,item.variant==='multiplayer'&&!PRODUCT_GAMES[item.game].multiplayerRuntime?'not-applicable':'blocked');
    assert.equal(item.task,null);
  }
  const missingInput=buildAdaptationPlan({root,environment:{},games:['th08'],variant:'normal',groups:['audio-latency']}).cases[0];
  assert.equal(missingInput.status,'blocked');assert.equal(missingInput.task,null);
});

test('the adaptation profile exposes the catalog plan to the dedicated runner branch',()=>{
  assert.deepEqual(profiles.adaptation,['adaptation']);
  const suite=suites({root,output:join(tmpdir(),'adaptation-profile-test'),environment:{}}).adaptation;
  const plan=suite.plan({games:['th08'],variant:'normal',groups:['runtime']});
  assert.equal(plan.cases.length,1);
  assert.equal(plan.cases[0].id,'runtime-normal-th08');
  assert.equal(plan.cases[0].status,'blocked');
});

test('tooling discovery includes the catalog and title-flow adapter tests',()=>{
  const task=suites({root,environment:{}}).tooling.tasks()[0];
  assert.ok(task.args.includes('tests/adaptation/catalog.test.mjs'));
  assert.ok(task.args.includes('tests/adaptation/adapters/title-flow.test.mjs'));
});

test('implemented MIDI and music lifecycle operations are selectable through their real driver',()=>{
  for(const id of ['AUDIO-03','AUDIO-04','AUDIO-05','MIDI-01','MIDI-02','MIDI-03','MIDI-04']){
    const plan=buildAdaptationPlan({root,environment:env({EAGLER_TEST_MUSIC_MANIFEST:'music.json'}),
      games:['th08'],variant:'normal',checks:[id]});
    const music=plan.cases.find(item=>item.group==='music');
    assert.ok(music,`${id}: missing implemented music driver`);
    assert.ok(music.checks.includes(id));
    assert.ok(!plan.unmappedChecks.some(check=>check.id===id));
  }
});

test('diagnostic selections share one owned server while preserving each requested lane',()=>{
  const environment=env({EAGLER_TEST_DIAGNOSTIC_MANIFEST:'diagnostics.json'});
  const plan=buildAdaptationPlan({root,environment,games:['th08'],variant:'normal',
    groups:['presentation','verify-quick','verify-daily']});
  assert.equal(plan.cases.length,1);
  const item=plan.cases[0];
  assert.equal(item.group,'diagnostic');assert.equal(item.status,'ready');
  assert.deepEqual(item.diagnosticLanes,['presentation','quick','daily']);
  assert.ok(item.task.args.includes('--lanes=presentation,quick,daily'));
  assert.ok(item.checks.includes('DISPLAY-03')&&item.checks.includes('VERIFY-01')&&item.checks.includes('VERIFY-02'));
  assert.equal(item.checks.length,new Set(item.checks).size);
  assert.ok(!item.checks.includes('VERIFY-05')&&!item.checks.includes('VERIFY-07'),
    'golden maintenance and production/diagnostic parity need their own actual operations');
  assert.ok(item.task.resources.includes('browser'));
  const selected=buildAdaptationPlan({root,environment,games:['th08'],variant:'normal',checks:['VERIFY-01']}).cases;
  assert.equal(selected.length,1);assert.deepEqual(selected[0].diagnosticLanes,['quick']);
  assert.deepEqual(selected[0].checks,['VERIFY-01']);
});

test('practice parameter consumption, native controls and feature inventory select the actual practice driver',()=>{
  for(const id of ['PRACTICE-04','PRACTICE-05','PRACTICE-06','PRACTICE-08']){
    const plan=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal',checks:[id]});
    assert.equal(plan.cases.length,1);assert.equal(plan.cases[0].group,'practice');
    assert.deepEqual(plan.cases[0].checks,[id]);
    assert.ok(plan.cases[0].task.args[0].endsWith('practice-browser.mjs'));
    assert.deepEqual(plan.unmappedChecks,[]);
  }
  for(const [game,product] of Object.entries(PRODUCT_GAMES).filter(([game])=>!resolvePracticeAdapter(game,'normal'))){
    const item=buildAdaptationPlan({root,environment:env(),games:[game],variant:'normal',groups:['practice']}).cases[0];
    assert.equal(item.status,product.features?.thprac?'blocked':'not-applicable');
    assert.equal(item.task,null,'A declared product feature cannot substitute for a native practice interface');
  }
});

test('language obligations select one production journey and never inherit a normal interface into MP',()=>{
  const environment=env({EAGLER_TEST_LANGUAGE_MANIFEST:'language-input.json',EAGLER_TEST_ARTWORK:'artwork-input'});
  const normal=buildAdaptationPlan({root,environment,games:['th08'],variant:'normal',groups:['language']}).cases;
  assert.equal(normal.length,1);assert.equal(normal[0].status,'ready');
  assert.equal(normal[0].requiresLauncherBuild,true,'language journeys need one shared Launcher build');
  assert.deepEqual(normal[0].checks,['LANG-01','LANG-02','LANG-03','LANG-04','LANG-05','LANG-06']);
  assert.ok(normal[0].task.args[0].endsWith('language-browser.mjs'));
  const mp=buildAdaptationPlan({root,environment,games:['th08'],variant:'multiplayer',groups:['language']}).cases[0];
  assert.equal(mp.status,'blocked');assert.equal(mp.task,null);
  const missing=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal',checks:['LANG-06']}).cases[0];
  assert.equal(missing.status,'blocked');assert.ok(missing.blockedInputs.some(input=>input.name==='EAGLER_TEST_LANGUAGE_MANIFEST'));
  assert.deepEqual(missing.checks,['LANG-06']);
});

test('diagnostic inputs and missing per-variant interfaces remain explicit blockers',()=>{
  const missing=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal',groups:['presentation']}).cases[0];
  assert.equal(missing.status,'blocked');assert.equal(missing.task,null);
  assert.deepEqual(missing.blockedInputs.map(input=>input.name),['EAGLER_TEST_DIAGNOSTIC_MANIFEST']);
  const products=buildAdaptationPlan({root,environment:env(),games:['th08','th06'],variant:'both',groups:['verify-quick']}).cases;
  assert.equal(products.length,4);
  for(const item of products.filter(item=>!(item.game==='th08'&&item.variant==='normal'))){
    assert.equal(item.status, item.variant==='multiplayer'&&!PRODUCT_GAMES[item.game].multiplayerRuntime?'not-applicable':'blocked');
    assert.equal(item.task,null);
  }
  assert.match(products.find(item=>item.game==='th08'&&item.variant==='multiplayer').reason,/diagnostic scene/);
  const virtual=buildAdaptationPlan({root,environment:{},games:['th075'],variant:'normal',
    groups:['presentation','verify-quick','verify-daily']});
  assert.equal(virtual.cases.length,1);
  assert.equal(virtual.cases[0].status,'blocked');
  assert.deepEqual(virtual.cases[0].diagnosticLanes,['presentation','quick','daily']);
  assert.deepEqual(virtual.cases[0].scenarioIds,['diagnostic-presentation','diagnostic-quick','diagnostic-daily']);
  assert.deepEqual(virtual.cases[0].checks,['DISPLAY-03','VERIFY-01','VERIFY-03','VERIFY-04','VERIFY-06','VERIFY-08',
    'VERIFY-02']);
  assert.equal(virtual.cases[0].task,null);

  const nextProfile={...structuredClone(resolveDiagnosticProfile('th08','normal')),game:'th06',lanes:['presentation']};
  for(const key of ['quickCapture','quickCaptureSchema','quickGate','quickFixtureCount','quickInputs','quickPartialChecks',
    'dailyCapture','dailyCaptureSchema','dailyGate','dailyGateReplayOptions','dailyFixtures','dailyPartialChecks','goldenManifest'])delete nextProfile[key];
  delete nextProfile.laneScope.quick;delete nextProfile.laneScope.daily;delete nextProfile.laneScope.otherVerification;
  delete nextProfile.decisionInputs.quick;delete nextProfile.decisionInputs.daily;
  const nextRegistry=createDiagnosticsRegistry([nextProfile]);
  const staged=buildAdaptationPlan({root,environment:env({EAGLER_TEST_DIAGNOSTIC_MANIFEST:'diagnostics.json'}),games:['th06'],
    variant:'normal',groups:['presentation','verify-quick','verify-daily'],diagnosticRegistry:nextRegistry});
  assert.equal(staged.cases.length,2,'each selected lane must be runnable or explicitly blocked');
  const available=staged.cases.find(item=>item.status==='ready');
  const blocked=staged.cases.find(item=>item.status==='blocked');
  assert.deepEqual(available.diagnosticLanes,['presentation']);assert.ok(available.task);
  assert.deepEqual(blocked.diagnosticLanes,['quick','daily']);assert.equal(blocked.task,null);
  assert.deepEqual(blocked.checks,['VERIFY-01','VERIFY-03','VERIFY-04','VERIFY-06','VERIFY-08','VERIFY-02']);
});

test('missing private inputs block only the affected case and never become N/A',()=>{
  const plan=buildAdaptationPlan({root,environment:{},games:['th08'],variant:'normal',groups:['runtime']});
  assert.equal(plan.cases.length,1);
  assert.equal(plan.cases[0].status,'blocked');
  assert.deepEqual(plan.cases[0].blockedInputs.map(item=>item.name),['EAGLER_TEST_RUNTIME_MANIFEST']);
  assert.equal(plan.cases[0].task,null);
  const practice=buildAdaptationPlan({root,environment:env(),games:['th09'],variant:'normal',groups:['practice']}).cases[0];
  assert.equal(practice.status,'not-applicable');
  assert.equal(practice.blockedInputs.length,0);
});

test('desktop groups use distinct explicit driver modes',()=>{
  const cases=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal',groups:['desktop-input','desktop-background']}).cases;
  assert.equal(cases.length,2);
  const input=cases.find(item=>item.group==='desktop-input'),background=cases.find(item=>item.group==='desktop-background');
  assert.ok(input.task.args.includes('--input-only'));
  assert.ok(background.task.args.includes('--background-only'));
  assert.deepEqual([...input.checks].sort(),desktopAdaptationIdsForMode('input-only',{game:'th08'}).sort());
  assert.deepEqual([...background.checks].sort(),desktopAdaptationIdsForMode('background-only',{game:'th08'}).sort());
  assert.ok(!background.checks.includes('TOUCH-16'),'The background driver does not cancel or observe touch input');
  assert.ok(input.checks.includes('TOUCH-03'),'The eight-direction native joystick probe is required in the input group');
  assert.ok(input.checks.includes('TOUCH-01')&&input.checks.includes('TOUCH-02')&&input.checks.includes('TOUCH-04'));
  assert.ok(input.checks.every(check=>typeof check==='string')&&!Object.hasOwn(input,'complete'),
    'A scene plan records mapped checks without claiming adaptation completion');
  const th09=buildAdaptationPlan({root,environment:env(),games:['th09'],variant:'normal',groups:['desktop-input']}).cases[0];
  assert.deepEqual([...th09.checks].sort(),desktopAdaptationIdsForMode('input-only',{game:'th09'}).sort());
  assert.ok(!th09.checks.includes('INPUT-05'),'TH09 touch fire sampling does not establish mixed-input independence');
});

test('numbered selection reaches only its shared desktop assertions',()=>{
  const plan=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal',groups:['desktop-input'],checks:['TOUCH-03']});
  assert.equal(plan.cases.length,1);
  assert.deepEqual(plan.cases[0].checks,['TOUCH-03']);
  assert.ok(plan.cases[0].task.args.includes('--check=TOUCH-03'));
});

test('legacy navigation names cannot schedule native groups without the declared interfaces',()=>{
  const plan=buildAdaptationPlan({root,environment:env(),games:['th11'],variant:'normal',
    groups:['runtime','desktop-input','touch-motion','desktop-background']});
  assert.equal(plan.cases.length,3,'desktop-input owns the shared touch-motion selection');
  assert.ok(plan.cases.every(item=>item.status==='blocked'&&item.task===null));
  assert.ok(plan.cases.every(item=>/missing: runtime, observation, flow/.test(item.reason)));
  assert.ok(plan.cases.find(item=>item.group==='desktop-input').reason.includes('actions'));
});

test('a newly catalogued product stays visible and blocked until its driver interface is registered',()=>{
  const plan=buildAdaptationPlan({root,environment:env(),games:['th075'],variant:'normal',groups:['runtime']});
  assert.equal(plan.cases.length,1);assert.equal(plan.cases[0].status,'blocked');assert.equal(plan.cases[0].task,null);
  assert.match(plan.cases[0].reason,/native observations and scenario flow/);
  assert.ok(plan.cases[0].checks.includes('INPUT-04'));assert.ok(plan.cases[0].checks.includes('INPUT-06'));
});

test('music duties are catalog-derived and every normal and multiplayer product is runnable, blocked or N/A',()=>{
  const workspace=JSON.parse(readFileSync(resolve(root,'launcher/config/workspace.json'),'utf8')).repositories;
  const plan=buildAdaptationPlan({root,environment:env({EAGLER_TEST_MUSIC_MANIFEST:'music.json',EAGLER_TEST_PACKAGES:'packages.json',EAGLER_TEST_ARTWORK:'artwork'}),groups:['music'],variant:'both'});
  assert.equal(new Set(plan.cases.filter(item=>item.variant==='normal').map(item=>item.game)).size,Object.keys(PRODUCT_GAMES).length);
  assert.equal(plan.cases.filter(item=>item.variant==='multiplayer').length,Object.keys(PRODUCT_GAMES).length);
  assert.equal(new Set(plan.cases.map(item=>item.id)).size,plan.cases.length);
  for(const [game,product] of Object.entries(PRODUCT_GAMES)){
    const duties=['AUDIO-01','AUDIO-03','AUDIO-04','AUDIO-05',
      ...(product.musicCapabilities?.midi===true?['MIDI-01','MIDI-02','MIDI-03','MIDI-04']:[])];
    const owner=workspace[game];
    const hasOwner=typeof owner==='string'&&existsSync(resolve(root,owner));
    const hasOgg=typeof product.package?.musicSourceDirectories?.ogg==='string'&&
      typeof product.package?.musicMounts?.ogg==='string';
    const driverAvailable=MUSIC_BROWSER_GAMES.includes(game)&&hasOwner&&hasOgg;
    const implemented=new Set(musicAdaptationIds(game));
    const normal=plan.cases.filter(item=>item.game===game&&item.variant==='normal');
    const runnable=normal.find(item=>item.task);
    const blocked=normal.find(item=>item.status==='blocked');
    const expectedRunnable=driverAvailable?duties.filter(id=>implemented.has(id)):[];
    const expectedBlocked=duties.filter(id=>!expectedRunnable.includes(id));
    if(expectedRunnable.length){
      assert.ok(runnable,`${game}: declared browser music scene needs a runnable case`);
      assert.equal(runnable.status,'ready');
      assert.deepEqual(runnable.checks,expectedRunnable);
      assert.ok(runnable.blockedInputs.length===0);
      assert.ok(runnable.requiredInputs.includes('EAGLER_TEST_RUNTIME_MANIFEST'));
      assert.ok(runnable.requiredInputs.includes('EAGLER_TEST_MUSIC_MANIFEST'));
      if(game==='th08'){
        assert.ok(runnable.task.args.includes('--launcher-ui'));
        assert.ok(runnable.requiresLauncherBuild);
        assert.ok(runnable.requiredInputs.includes('EAGLER_TEST_PACKAGES'));
        assert.ok(runnable.requiredInputs.includes('EAGLER_TEST_ARTWORK'));
      }
    }else assert.equal(runnable,undefined,`${game}: missing interface must not receive a fabricated task`);
    if(expectedBlocked.length){
      assert.ok(blocked,`${game}: missing music duties must remain visible`);
      assert.equal(blocked.task,null);
      assert.deepEqual(blocked.checks,expectedBlocked);
    }else assert.equal(blocked,undefined);
    assert.equal(normal.flatMap(item=>item.checks).length,duties.length,`${game}: each applicable normal duty appears once`);

    const multiplayer=plan.cases.find(item=>item.game===game&&item.variant==='multiplayer');
    assert.ok(multiplayer,`${game}: multiplayer music applicability must be explicit`);
    assert.equal(multiplayer.status,product.multiplayerRuntime?'blocked':'not-applicable');
    assert.equal(multiplayer.task,null);
    assert.deepEqual(multiplayer.checks,duties);
  }
  assert.deepEqual(plan.unmappedChecks,[]);
});

test('th08 music cannot silently omit its real Launcher UI when package inputs are absent',()=>{
  const plan=buildAdaptationPlan({root,environment:env({EAGLER_TEST_MUSIC_MANIFEST:'music.json'}),groups:['music'],games:['th08'],variant:'normal'});
  const scene=plan.cases.find(item=>item.id==='music-normal-th08');
  assert.equal(scene.status,'blocked');assert.equal(scene.task,null);
  assert.deepEqual(scene.blockedInputs.map(input=>input.name),['EAGLER_TEST_PACKAGES','EAGLER_TEST_ARTWORK']);
});

test('selected music duties outside the current scene remain explicitly blocked',()=>{
  const audio=buildAdaptationPlan({root,environment:env({EAGLER_TEST_MUSIC_MANIFEST:'music.json'}),
    games:['th11'],variant:'normal',groups:['music'],checks:['AUDIO-03']});
  assert.equal(audio.cases.length,1);
  assert.equal(audio.cases[0].status,'blocked');assert.equal(audio.cases[0].task,null);
  assert.deepEqual(audio.cases[0].checks,['AUDIO-03']);assert.deepEqual(audio.unmappedChecks,[]);
  const midi=buildAdaptationPlan({root,environment:env({EAGLER_TEST_MUSIC_MANIFEST:'music.json'}),
    games:['th06'],variant:'normal',groups:['music'],checks:['MIDI-01']});
  assert.equal(midi.cases.length,1);
  assert.equal(midi.cases[0].status,'blocked');assert.equal(midi.cases[0].task,null);
  assert.deepEqual(midi.cases[0].checks,['MIDI-01']);assert.deepEqual(midi.unmappedChecks,[]);
  const multiplayer=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'multiplayer',
    groups:['music'],checks:['AUDIO-03']}).cases[0];
  assert.equal(multiplayer.status,'blocked');assert.equal(multiplayer.task,null);
  assert.deepEqual(multiplayer.checks,['AUDIO-03']);
});

test('music obligations do not vanish when a selected product has no registered owner',async()=>{
  const directory=await mkdtemp(join(tmpdir(),'adaptation-music-owner-'));
  try{
    const workspace=JSON.parse(readFileSync(resolve(root,'launcher/config/workspace.json'),'utf8'));
    delete workspace.repositories.th08;
    delete workspace.repositories.th08mp;
    const workspacePath=join(directory,'launcher','config','workspace.json');
    await mkdir(join(directory,'docs','adaptation'),{recursive:true});
    await mkdir(join(directory,'launcher','config'),{recursive:true});
    await writeFile(join(directory,'docs','adaptation','checklist.json'),readFileSync(resolve(root,'docs/adaptation/checklist.json')));
    await writeFile(workspacePath,JSON.stringify(workspace));
    const plan=buildAdaptationPlan({root:directory,environment:env({EAGLER_TEST_MUSIC_MANIFEST:'music.json'}),
      games:['th08'],variant:'normal',groups:['music']});
    assert.equal(plan.cases.length,1);
    assert.equal(plan.cases[0].status,'blocked');assert.equal(plan.cases[0].task,null);
    assert.deepEqual(plan.cases[0].checks,['AUDIO-01','AUDIO-03','AUDIO-04','AUDIO-05','MIDI-01','MIDI-02','MIDI-03','MIDI-04']);
    assert.match(plan.cases[0].reason,/workspace\.json/);
    const mp=buildAdaptationPlan({root:directory,environment:env(),games:['th08'],variant:'multiplayer',groups:['music']}).cases[0];
    assert.equal(mp.status,'blocked');assert.equal(mp.task,null);
    assert.match(mp.reason,/th08mp production owner/);
  }finally{await rm(directory,{recursive:true,force:true});}
});

test('offline base scenarios remain runnable while absent update evidence is explicit and merged',()=>{
  const values={EAGLER_TEST_RUNTIME_MANIFEST:'inputs.json',EAGLER_TEST_PACKAGES:'packages.json',EAGLER_TEST_ARTWORK:'artwork'};
  const plan=buildAdaptationPlan({root,environment:values,games:MUSIC_BROWSER_GAMES,variant:'normal',groups:['offline']});
  assert.equal(plan.cases.length,MUSIC_BROWSER_GAMES.length);
  for(const item of plan.cases){
    assert.equal(item.status,'ready');assert.ok(item.task);
    assert.equal(item.requiresLauncherBuild,true);
    assert.ok(item.scenarioIds.includes('offline-import-reopen'));
    assert.ok(item.scenarioIds.includes('package-update-and-user-data-recovery'));
    assert.deepEqual(item.blockedChecks,['PACKAGE-03','SAVE-06']);
    assert.deepEqual(item.inputs,['EAGLER_TEST_RUNTIME_MANIFEST','EAGLER_TEST_PACKAGES','EAGLER_TEST_ARTWORK','EAGLER_TEST_UPDATE_PACKAGES']);
    assert.deepEqual(item.requiredInputs,['EAGLER_TEST_RUNTIME_MANIFEST','EAGLER_TEST_PACKAGES','EAGLER_TEST_ARTWORK']);
    assert.ok(!item.task.args.some(argument=>argument.startsWith('--update-manifest=')));
    assert.ok(item.task.args.includes('--artwork-dir=artwork'));
  }
  for(const target of plan.coverageTargets){
    const planned=plan.cases.filter(item=>item.game===target.game&&item.variant===target.variant).flatMap(item=>item.checks);
    assert.ok(planned.every(id=>target.requirements.some(check=>check.id===id)),
      'merged update obligations must remain visible in scene-selected coverage and review');
    assert.ok(target.requirements.some(check=>check.id==='PACKAGE-03'));
    assert.ok(target.requirements.some(check=>check.id==='SAVE-06'));
  }
  const updateOnly=buildAdaptationPlan({root,environment:{},games:['th08'],variant:'normal',groups:['offline-update'],checks:['PACKAGE-03','SAVE-06']}).cases[0];
  assert.equal(updateOnly.status,'blocked');assert.equal(updateOnly.task,null);
  assert.deepEqual(updateOnly.checks,['PACKAGE-03','SAVE-06']);
  assert.deepEqual(updateOnly.blockedChecks,['PACKAGE-03','SAVE-06']);
  assert.ok(!updateOnly.checks.includes('PACKAGE-05'));
});

test('available update manifest merges into one offline task for each selected update product',async()=>{
  const directory=await mkdtemp(join(tmpdir(),'adaptation-update-manifest-'));
  try{
    const path=join(directory,'update.json');
    await writeFile(path,JSON.stringify({games:Object.fromEntries(MUSIC_BROWSER_GAMES.map(game=>[game,{}]))}));
    const plan=buildAdaptationPlan({root,output:directory,environment:{
      EAGLER_TEST_RUNTIME_MANIFEST:'inputs.json',EAGLER_TEST_PACKAGES:'packages.json',EAGLER_TEST_ARTWORK:'artwork',
      EAGLER_TEST_UPDATE_PACKAGES:path,
    },games:MUSIC_BROWSER_GAMES,variant:'normal',groups:['offline']});
    assert.equal(plan.cases.length,MUSIC_BROWSER_GAMES.length);
    assert.equal(new Set(plan.cases.map(item=>item.id)).size,plan.cases.length);
    for(const item of plan.cases){
      assert.equal(item.status,'ready');assert.equal(item.blockedScenarios.length,0);
      assert.deepEqual(item.inputs,['EAGLER_TEST_RUNTIME_MANIFEST','EAGLER_TEST_PACKAGES','EAGLER_TEST_ARTWORK','EAGLER_TEST_UPDATE_PACKAGES']);
      assert.deepEqual(item.requiredInputs,['EAGLER_TEST_RUNTIME_MANIFEST','EAGLER_TEST_PACKAGES','EAGLER_TEST_ARTWORK','EAGLER_TEST_UPDATE_PACKAGES']);
      assert.ok(item.task.args.some(argument=>argument===`--update-manifest=${path}`));
      assert.deepEqual(item.checks,['RUNTIME-05','PACKAGE-01','PACKAGE-02','PACKAGE-04','PACKAGE-05','SAVE-02','SAVE-03','SAVE-04','SAVE-05','PACKAGE-03','SAVE-06']);
    }
  }finally{await rm(directory,{recursive:true,force:true});}
});

test('offline update obligations are visible for non-OGG products and depend on update inputs',async()=>{
  const directory=await mkdtemp(join(tmpdir(),'adaptation-update-any-product-'));
  try{
    const base={EAGLER_TEST_RUNTIME_MANIFEST:'inputs.json',EAGLER_TEST_PACKAGES:'packages.json',EAGLER_TEST_ARTWORK:'artwork'};
    const missing=buildAdaptationPlan({root,environment:{},games:['th06'],variant:'normal',groups:['offline-update']}).cases;
    assert.equal(missing.length,1,'a selected title with update checks must not disappear');
    assert.equal(missing[0].status,'blocked');assert.deepEqual(missing[0].checks,['PACKAGE-03','SAVE-06']);
    assert.deepEqual(missing[0].blockedChecks,['PACKAGE-03','SAVE-06']);assert.equal(missing[0].task,null);
    assert.deepEqual(missing[0].blockedInputs.map(input=>input.name),[
      'EAGLER_TEST_RUNTIME_MANIFEST','EAGLER_TEST_PACKAGES','EAGLER_TEST_ARTWORK',
    ],'missing base package inputs remain visible alongside the missing optional update manifest');

    const path=join(directory,'update.json');
    await writeFile(path,JSON.stringify({games:{th08:{}}}));
    const absent=buildAdaptationPlan({root,environment:{...base,EAGLER_TEST_UPDATE_PACKAGES:path},games:['th06'],
      variant:'normal',groups:['offline']}).cases[0];
    assert.equal(absent.status,'ready','the base offline import remains executable');
    assert.ok(absent.blockedScenarios.some(scene=>scene.checks.includes('PACKAGE-03')&&/no second package for th06/.test(scene.reason)));
    assert.ok(absent.checks.includes('SAVE-06'));

    await writeFile(path,JSON.stringify({games:{th06:{}}}));
    const ready=buildAdaptationPlan({root,environment:{...base,EAGLER_TEST_UPDATE_PACKAGES:path},games:['th06'],
      variant:'normal',groups:['offline-update']}).cases[0];
    assert.equal(ready.status,'ready');assert.equal(ready.task.args[0].endsWith('offline-runtime-browser.mjs'),true);
    assert.ok(ready.requiredInputs.includes('EAGLER_TEST_UPDATE_PACKAGES'));
  }finally{await rm(directory,{recursive:true,force:true});}
});

test('rooms expand each product by player count and route, with product-derived N/A',()=>{
  const cases=buildAdaptationPlan({root,environment:env({EAGLER_TEST_LAUNCHER_URL:'https://launcher.example'}),
    games:['th09','th20'],variant:'multiplayer',groups:['rooms']}).cases;
  assert.equal(cases.length,8);
  assert.equal(cases.filter(item=>item.game==='th20').length,4);
  assert.ok(cases.filter(item=>item.game==='th20').every(item=>item.status==='not-applicable'));
  assert.equal(cases.filter(item=>item.game==='th09'&&item.status==='ready').length,2);
  assert.equal(cases.filter(item=>item.game==='th09'&&item.status==='not-applicable').length,2);
  assert.ok(cases.filter(item=>item.game==='th09'&&item.status==='ready').every(item=>
    item.inputs.includes('EAGLER_TEST_RUNTIME_MANIFEST')&&item.inputs.includes('EAGLER_TEST_LAUNCHER_URL')&&
    item.requiredInputs.includes('EAGLER_TEST_LAUNCHER_URL')&&
    item.task.args.includes('--url=https://launcher.example')&&!item.inputs.includes('EAGLER_TEST_PACKAGES')));
  assert.equal(new Set(cases.map(item=>item.id)).size,cases.length);
});

test('finite Replay recording selects its own driver and never inherits a normal interface into MP',()=>{
  const plan=buildAdaptationPlan({root,environment:env(),games:['th08','th06','th20'],groups:['replay-recording']});
  const pilot=plan.cases.find(item=>item.game==='th08'&&item.variant==='normal');
  assert.equal(pilot.status,'ready');assert.ok(pilot.task.args.includes('--replay-recording-only'));
  assert.deepEqual(pilot.checks,['REPLAY-02','REPLAY-03','REPLAY-04','REPLAY-06']);
  assert.equal(plan.cases.find(item=>item.game==='th08'&&item.variant==='multiplayer').status,'blocked');
  assert.equal(plan.cases.find(item=>item.game==='th06'&&item.variant==='normal').status,'blocked');
  assert.equal(plan.cases.find(item=>item.game==='th20'&&item.variant==='multiplayer').status,'not-applicable');
});

test('every remaining pilot gap has a concrete selectable scene or an explicit external-input prerequisite',()=>{
  const remaining=['BUILD-05','BUILD-06','INPUT-03','INPUT-07','TOUCH-12','TOUCH-17','TOUCH-19',
    'DISPLAY-01','DISPLAY-02','DISPLAY-04','DISPLAY-05','DISPLAY-06','DISPLAY-07','DISPLAY-08','REPLAY-07','VERIFY-05','VERIFY-07','AUDIO-06','PACKAGE-06'];
  const plan=buildAdaptationPlan({root,environment:env(),games:['th08'],variant:'normal',checks:remaining});
  assert.deepEqual(plan.unmappedChecks,[]);
  const mapped=new Set(plan.cases.flatMap(item=>item.checks));for(const id of remaining)assert.ok(mapped.has(id),id);
  assert.ok(plan.cases.find(item=>item.group==='source-review').blockedInputs.some(item=>item.name==='EAGLER_TEST_SOURCE_REVIEW'));
  assert.ok(plan.cases.find(item=>item.group==='single-game').blockedInputs.some(item=>item.name==='EAGLER_TEST_MINIMAL_RUNTIME'));
  const presentation=plan.cases.find(item=>item.diagnosticPairedReplay);
  assert.ok(presentation.task===null&&presentation.blockedInputs.some(item=>item.name==='EAGLER_TEST_DIAGNOSTIC_MANIFEST'));
  assert.ok(plan.cases.find(item=>item.group==='installation-authority').status==='blocked');
});

test('menu scene selection requires each title to register its own original-menu and loaded-dialogue semantics',()=>{
  const plan=buildAdaptationPlan({root,environment:env(),games:['th08','th10','th11'],variant:'normal',checks:['INPUT-07']});
  for(const game of ['th08','th10']){
    const entry=plan.cases.find(item=>item.game===game);
    assert.equal(entry.status,'ready');assert.ok(entry.task.args.includes('--check=INPUT-07'));
    assert.equal(Object.hasOwn(entry,'complete'),false,'Registration schedules the native scenes; it does not certify their result');
  }
  assert.equal(plan.cases.find(item=>item.game==='th11').status,'blocked');
  assert.match(plan.cases.find(item=>item.game==='th11').reason,/th11\/normal.*missing:/);
  const multiplayer=buildAdaptationPlan({root,environment:env(),games:['th10'],variant:'multiplayer',checks:['INPUT-07']});
  assert.equal(resolveAdaptationAdapter('th10','multiplayer'),null);
  assert.deepEqual(multiplayer.cases,[],'A normal original-menu registration does not schedule the room-owned MP input scene');
  assert.deepEqual(multiplayer.unmappedChecks,[{id:'INPUT-07',reason:'No selected scenario group maps this adaptation check'}]);
});
