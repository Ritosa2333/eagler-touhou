import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,mkdir,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {buildDiagnosticServerEnvironment,inspectDiagnosticProduct,loadDiagnosticInputs,validateDiagnosticInputs} from './diagnostic-inputs.mjs';

async function fixture(t){
  const root=await mkdtemp(join(tmpdir(),'diagnostic-inputs-'));
  t.after(()=>rm(root,{recursive:true,force:true}));
  const inputRoot=join(root,'inputs');await mkdir(inputRoot);
  const font=join(root,'font.ttc'),data=join(root,'game.dat'),lunatic=join(root,'lunatic.rpy'),extra=join(root,'extra.rpy');
  for(const [path,value] of [[font,'font'],[data,'data'],[lunatic,'lunatic'],[extra,'extra']])await writeFile(path,value);
  const product={font,data,inputRoot,replayFiles:{lunatic,extra}};
  return{root,product,inputRoot,document:{schema:'eagler-touhou/diagnostic-inputs/1',products:{th08:{normal:product}}}};
}

test('diagnostic input schema selects one explicit product and binds its private files',async t=>{
  const f=await fixture(t);
  assert.equal(validateDiagnosticInputs(f.document,{game:'th08',variant:'normal'}).product.inputRoot,f.inputRoot);
  await writeFile(join(f.inputRoot,'demo0.rpy'),'demo');
  const inspected=await inspectDiagnosticProduct(f.product,{quickInputs:[{id:'demo0',index:0,file:'demo0.rpy'}],lanes:['quick']});
  assert.equal(inspected.data.bytes,4);
  assert.match(inspected.data.sha256,/^[a-f0-9]{64}$/);
  assert.equal(inspected.quickInputs.length,1);
  assert.equal(inspected.quickInputs[0].path,join(f.inputRoot,'demo0.rpy'));
  assert.equal(inspected.quickInputs[0].id,'demo0');assert.equal(inspected.quickInputs[0].index,0);
  assert.equal(inspected.dailyReplays,undefined);
});

test('diagnostic input manifest rejects unknown schema, missing selection and malformed product paths',async t=>{
  const f=await fixture(t);
  assert.throws(()=>validateDiagnosticInputs({...f.document,schema:'wrong'},{game:'th08',variant:'normal'}),/schema/);
  assert.throws(()=>validateDiagnosticInputs(f.document,{game:'th08',variant:'multiplayer'}),/missing th08\/multiplayer/);
  const relative=structuredClone(f.document);relative.products.th08.normal.font='font.ttc';
  assert.throws(()=>validateDiagnosticInputs(relative,{game:'th08',variant:'normal'}),/absolute path/);
  const incomplete=structuredClone(f.document);delete incomplete.products.th08.normal.replayFiles.extra;
  assert.equal(validateDiagnosticInputs(incomplete,{game:'th08',variant:'normal'}).product.replayFiles.extra,undefined);
  const extraKey=structuredClone(f.document);extraKey.products.th08.normal.unused='';
  assert.throws(()=>validateDiagnosticInputs(extraKey,{game:'th08',variant:'normal'}),/may declare only/);
});

test('diagnostic input loader requires an absolute readable JSON path',async t=>{
  const f=await fixture(t),path=join(f.root,'diagnostics.json');
  await writeFile(path,JSON.stringify(f.document));
  const loaded=await loadDiagnosticInputs(path,{game:'th08',variant:'normal'});
  assert.equal(loaded.path,path);
  await assert.rejects(()=>loadDiagnosticInputs(join(f.root,'absent.json'),{game:'th08',variant:'normal'}),/Could not read/);
  await assert.rejects(()=>loadDiagnosticInputs('relative.json',{game:'th08',variant:'normal'}),/absolute path/);
});

test('presentation does not require Replay files and daily validates only profile-selected fixture keys',async t=>{
  const f=await fixture(t);
  const withoutReplays={font:f.product.font,data:f.product.data,inputRoot:f.product.inputRoot};
  const shown=await inspectDiagnosticProduct(withoutReplays,{lanes:['presentation']});
  assert.equal(shown.dailyReplays,undefined);assert.equal(shown.quickInputs,undefined);
  await assert.rejects(()=>inspectDiagnosticProduct({...withoutReplays,font:f.inputRoot},{lanes:['presentation']}),/Font must be a non-empty regular file/);
  await assert.rejects(()=>inspectDiagnosticProduct(f.product,{quickInputs:[{id:'escape',index:0,file:'../escape.rpy'}],lanes:['quick']}),/plain filenames/);
  await writeFile(join(f.inputRoot,'demo0.rpy'),'demo');
  await assert.rejects(()=>inspectDiagnosticProduct(f.product,{quickInputs:['missing.rpy'],lanes:['quick']}),/ENOENT/);
  const inspected=await inspectDiagnosticProduct(f.product,{dailyFixtures:[{id:'lunatic',replayKey:'lunatic'}],lanes:['daily']});
  assert.deepEqual(Object.keys(inspected.dailyReplays),['lunatic']);
  await assert.rejects(()=>inspectDiagnosticProduct(withoutReplays,{dailyFixtures:[{id:'extra',replayKey:'extra'}],lanes:['daily']}),/missing daily Replay extra/);
});

test('paired presentation binds the explicit Replay bytes and bounded logical extent separately from ordinary presentation',async t=>{
  const f=await fixture(t);await writeFile(join(f.inputRoot,'demo0.rpy'),'original demo bytes');
  const pairedReplay={schema:'eagler-presentation/replay-pair-capability/3',fixture:'demo0',inputFile:'demo0.rpy',ticks:120,
    expectedAuthorityGroups:17,cadencesHz:[60,120,144],
    cadenceDifferenceGroups:['host-frame-cadence','synchronous-audio-operations'],checks:['REPLAY-07']};
  const first=await inspectDiagnosticProduct(f.product,{lanes:['presentation'],pairedReplay});
  assert.equal(first.pairedReplay.ticks,120);assert.equal(first.pairedReplay.path,join(f.inputRoot,'demo0.rpy'));
  await writeFile(join(f.inputRoot,'demo0.rpy'),'changed demo bytes');
  const changed=await inspectDiagnosticProduct(f.product,{lanes:['presentation'],pairedReplay});
  assert.notEqual(changed.pairedReplay.sha256,first.pairedReplay.sha256);
  await assert.rejects(inspectDiagnosticProduct(f.product,{lanes:['quick'],pairedReplay}),/presentation lane/);
  await assert.rejects(inspectDiagnosticProduct(f.product,{lanes:['presentation'],pairedReplay:{...pairedReplay,inputFile:'../demo0.rpy'}}),/plain Replay/);
  await assert.rejects(inspectDiagnosticProduct(f.product,{lanes:['presentation'],pairedReplay:{...pairedReplay,expectedAuthorityGroups:undefined}}),/authority-group count/);
  await assert.rejects(inspectDiagnosticProduct(f.product,{lanes:['presentation'],pairedReplay:{...pairedReplay,cadencesHz:[60,120]}}),/callback rates/);
  await assert.rejects(inspectDiagnosticProduct(f.product,{lanes:['presentation'],pairedReplay:{...pairedReplay,
    cadenceDifferenceGroups:[...pairedReplay.cadenceDifferenceGroups,'authoritative-world']}}),/cadence difference groups/);
});

test('Lab server environment always selects this product and removes inherited snapshots',()=>{
  const base={PATH:'tools',TH08_LAB_SNAPSHOT:'stale-build',th08_lab_snapshot:'also-stale',KEEP_ME:'yes'};
  const profile={envNames:{font:'TH08_LAB_FONT',data:'TH08_LAB_DATA',inputRoot:'TH08_LAB_INPUT_ROOT',snapshot:'TH08_LAB_SNAPSHOT'}};
  const product={font:'C:/font.ttc',data:'C:/game.dat',inputRoot:'C:/inputs'};
  const {environment,clearedSnapshotEnvironment}=buildDiagnosticServerEnvironment(base,profile,product,8123);
  assert.equal(clearedSnapshotEnvironment,'TH08_LAB_SNAPSHOT');
  assert.equal(environment.PORT,'8123');assert.equal(environment.TH08_LAB_FONT,product.font);
  assert.equal(environment.TH08_LAB_DATA,product.data);assert.equal(environment.TH08_LAB_INPUT_ROOT,product.inputRoot);
  assert.equal(environment.KEEP_ME,'yes');
  assert.equal(Object.keys(environment).some(name=>name.toUpperCase()==='TH08_LAB_SNAPSHOT'),false);
  assert.throws(()=>buildDiagnosticServerEnvironment(base,{envNames:{font:'FONT',data:'DATA',inputRoot:'ROOT'}},product,8123),
    /inherited snapshot selector/);
});
