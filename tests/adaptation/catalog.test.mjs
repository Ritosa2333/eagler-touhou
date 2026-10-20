import assert from 'node:assert/strict';
import test from 'node:test';
import {mkdtemp,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join,resolve} from 'node:path';
import {PRODUCT_GAMES,PRODUCT_IDS} from '../../common/contracts/product-catalog.mjs';
import {MUSIC_BROWSER_GAMES} from '../../tools/ports/music-browser-helpers.mjs';
import {buildAdaptationPlan} from './catalog.mjs';
import {profiles,suites} from '../suites.mjs';

const root=resolve(import.meta.dirname,'../..');
const env=(values={})=>({EAGLER_TEST_RUNTIME_MANIFEST:'runtime-input.json',...values});

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
  const cases=buildAdaptationPlan({root,environment:env(),games:['th06'],variant:'normal',groups:['desktop-input','desktop-background']}).cases;
  assert.equal(cases.length,2);
  assert.ok(cases.find(item=>item.group==='desktop-input').task.args.includes('--input-only'));
  assert.ok(cases.find(item=>item.group==='desktop-background').task.args.includes('--background-only'));
});

test('music plan selects exactly catalogued OGG products and requires both manifests',()=>{
  const plan=buildAdaptationPlan({root,environment:{},groups:['music'],variant:'normal'});
  assert.deepEqual(plan.cases.filter(item=>item.status!=='not-applicable').map(item=>item.game).sort(),[...MUSIC_BROWSER_GAMES].sort());
  for(const item of plan.cases.filter(item=>item.status!=='not-applicable')){
    assert.ok(item.blockedInputs.some(input=>input.name==='EAGLER_TEST_RUNTIME_MANIFEST'));
    assert.ok(item.blockedInputs.some(input=>input.name==='EAGLER_TEST_MUSIC_MANIFEST'));
    assert.deepEqual(item.inputs,['EAGLER_TEST_RUNTIME_MANIFEST','EAGLER_TEST_MUSIC_MANIFEST']);
    assert.equal(item.task,null);
    assert.deepEqual(item.checks,['AUDIO-01']);
  }
});

test('offline base scenarios remain runnable while absent update evidence is explicit and merged',()=>{
  const values={EAGLER_TEST_RUNTIME_MANIFEST:'inputs.json',EAGLER_TEST_PACKAGES:'packages.json',EAGLER_TEST_ARTWORK:'artwork'};
  const plan=buildAdaptationPlan({root,environment:values,games:MUSIC_BROWSER_GAMES,variant:'normal',groups:['offline']});
  assert.equal(plan.cases.length,MUSIC_BROWSER_GAMES.length);
  for(const item of plan.cases){
    assert.equal(item.status,'ready');assert.ok(item.task);
    assert.ok(item.scenarioIds.includes('offline-import-reopen'));
    assert.ok(item.scenarioIds.includes('package-update-and-user-data-recovery'));
    assert.deepEqual(item.blockedChecks,['PACKAGE-03','SAVE-06']);
    assert.deepEqual(item.inputs,['EAGLER_TEST_RUNTIME_MANIFEST','EAGLER_TEST_PACKAGES','EAGLER_TEST_ARTWORK','EAGLER_TEST_UPDATE_PACKAGES']);
    assert.deepEqual(item.requiredInputs,['EAGLER_TEST_RUNTIME_MANIFEST','EAGLER_TEST_PACKAGES','EAGLER_TEST_ARTWORK']);
    assert.ok(!item.task.args.some(argument=>argument.startsWith('--update-manifest=')));
    assert.ok(item.task.args.includes('--artwork-dir=artwork'));
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
      assert.deepEqual(item.checks,['PACKAGE-02','PACKAGE-04','PACKAGE-05','SAVE-02','SAVE-03','SAVE-04','SAVE-05','PACKAGE-03','SAVE-06']);
    }
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
