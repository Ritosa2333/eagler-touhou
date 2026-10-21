import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {assessDriverReport,adaptationCoverage,adaptationReviewMarkdown,summarizeAdaptationCases} from '../../tests/adaptation/results.mjs';
import {identityDigest} from './dependency-snapshot.mjs';
import {replayOriginalImportCheck} from '../ports/replay-browser-semantics.mjs';

const selected={game:'th06',variant:'normal',checksMapped:['INPUT-01'],scenarioIds:['native-four-direction-arrows']};
const processResult={status:'passed',code:0,startedAt:'2026-10-19T01:00:00Z'};
const sealed=()=>({schema:'eagler-touhou/runtime-browser/1',runId:'test-run',startedAt:'2026-10-19T01:00:00Z',
  finishedAt:'2026-10-19T01:00:01Z',status:'passed',passed:true,targets:[{game:'th06',variant:'normal',status:'passed',checks:[]}]});
const observedProof=()=>({source:'driver-report',references:[{pointer:'/targets/0/checks/0/observation',
  sha256:identityDigest({before:{x:10,y:20},after:{x:14,y:20}})}]});

test('machine-only flags need independent evidence and never satisfy whole-check coverage',()=>{
  const report=sealed();
  report.targets[0].checks=[{id:'INPUT-01',status:'passed',complete:false,machineComplete:true,
    observation:{game:'th06',variant:'normal',adaptationIds:['INPUT-01'],observation:{before:{x:10},after:{x:14}}}}];
  const unsupported=assessDriverReport(selected,processResult,report);
  assert.equal(unsupported.status,'incomplete');
  assert.equal(unsupported.checks[0].complete,false);
  assert.equal(unsupported.checks[0].machineComplete,false);
  assert.equal(unsupported.checks[0].claimedMachineComplete,true);
  report.schema='eagler-touhou/diagnostic-browser/1';
  Object.assign(report.targets[0],{game:'th08',checks:[{id:'DISPLAY-01',lane:'presentation-paired',status:'passed',
    complete:false,machineComplete:true,observation:{game:'th08',variant:'normal',adaptationIds:['DISPLAY-01'],
      observation:{check:{status:'passed'},artifacts:{runs:{}}}}}]});
  const forged=assessDriverReport({game:'th08',variant:'normal',checksMapped:['DISPLAY-01'],scenarioIds:[]},processResult,report);
  assert.equal(forged.status,'failed');assert.equal(forged.checks[0].machineComplete,false);
  assert.equal(forged.checks[0].complete,false);
  assert.ok(forged.issues.some(issue=>issue.includes('Invalid complete evidence')));
});

test('zero exit cannot hide a failed assertion, room target or final audit',()=>{
  for(const breakReport of [
    report=>report.targets[0].checks.push({id:'movement',checks:['INPUT-01'],status:'failed',error:'no displacement'}),
    report=>{delete report.targets[0].status;report.targets[0].passed=false;},
    report=>report.targets[0].finalErrorAudit={status:'failed'},
  ]){const report=sealed();breakReport(report);assert.equal(assessDriverReport(selected,processResult,report).status,'failed');}
});

test('sealed reports must match the current invocation, title and variant',()=>{
  for(const breakReport of [report=>delete report.finishedAt,report=>report.targets[0].game='th07',
    report=>report.targets[0].variant='multiplayer',report=>delete report.targets[0].variant,report=>report.startedAt='2026-10-18T01:00:00Z']){
    const report=sealed();breakReport(report);assert.equal(assessDriverReport(selected,processResult,report).status,'failed');
  }
});

test('unavailable startup and incomplete checks preserve actionable reasons without hiding faults',()=>{
  const report=sealed();report.status='incomplete';report.passed=false;
  Object.assign(report.targets[0],{status:'incomplete',incompleteReason:'Native browser document remains hidden',
    checks:[{id:'input',checks:['INPUT-01'],status:'incomplete',reason:'No trusted input edge was captured'}]});
  const assessed=assessDriverReport(selected,{...processResult,status:'failed',code:1},report);
  assert.equal(assessed.status,'incomplete');
  assert.deepEqual(assessed.incompleteReasons,[
    'Native browser document remains hidden','INPUT-01: No trusted input edge was captured',
  ]);
  report.targets[0].checks.push({id:'protocol',status:'failed',error:'Runtime protocol error'});
  assert.equal(assessDriverReport(selected,{...processResult,status:'failed',code:1},report).status,'failed');
});

test('an executed incomplete numbered observation preserves its reason without satisfying completion',()=>{
  const planned={game:'th08',variant:'normal',checksMapped:['AUDIO-02'],scenarioIds:['desktop-input-effects']};
  const report=sealed();report.status='incomplete';report.passed=false;
  Object.assign(report.targets[0],{game:'th08',variant:'normal',status:'incomplete',checks:[{
    id:'audio-output-with-music-disabled',adaptationIds:['AUDIO-02'],status:'incomplete',
    observation:{reason:'Source intervals overlap on the same production voice output tap across the full input session.',
      nativeSfxCorrelation:{status:'incomplete',reason:'Source intervals overlap on the same production voice output tap across the full input session.'}},
  }]});
  const assessed=assessDriverReport(planned,processResult,report);
  assert.equal(assessed.status,'incomplete');
  assert.deepEqual(assessed.incompleteReasons,[
    'AUDIO-02: Source intervals overlap on the same production voice output tap across the full input session.',
  ]);
  assert.deepEqual(assessed.checks.map(({id,status,complete})=>({id,status,complete})),[
    {id:'AUDIO-02',status:'incomplete',complete:false},
  ]);

  report.targets[0].checks=[];
  const missing=assessDriverReport(planned,processResult,report);
  assert.equal(missing.status,'incomplete');
  assert.match(missing.incompleteReasons.join(' '),/numbered results for AUDIO-02/);
  assert.deepEqual(missing.checks,[]);
});

test('an incomplete Replay producer carries its natural endpoint reason into the adaptation summary',()=>{
  const sha256='a'.repeat(64),reason='The accepted original capture has no mapped natural ReplayStream cursor for this fixture.';
  const replayRecord={game:'th08',variant:'normal',status:'incomplete',finalErrorAudit:{status:'passed'},checks:[
    {id:'import-current-replay-through-shell',status:'passed',value:{path:'replay/original.rpy',bytes:128,sha256}},
    {id:'native-replay-plays-and-advances',status:'passed',value:{startFrame:0,frame:120,naturalFrames:120,observed:[{frame:120}]}},
    {id:'native-terminal-signal-observed',status:'passed'},
  ],playback:{status:'observed',scenarioStatus:'incomplete',fixture:{sha256},
    terminalSignalObserved:{status:'passed',value:true},fullInputConsumption:{status:'incomplete',reason}}};
  const report=sealed();report.status='incomplete';report.passed=false;
  Object.assign(report.targets[0],{game:'th08',variant:'normal',status:'incomplete',checks:[replayOriginalImportCheck(replayRecord)]});
  const assessed=assessDriverReport({game:'th08',variant:'normal',checksMapped:['REPLAY-01'],scenarioIds:[]},processResult,report);
  assert.equal(assessed.status,'incomplete');
  assert.deepEqual(assessed.incompleteReasons,[`REPLAY-01: ${reason}`]);
  assert.equal(assessed.checks[0].complete,false);

  replayRecord.checks[0].value.sha256='b'.repeat(64);
  report.targets[0].checks=[replayOriginalImportCheck(replayRecord)];
  const invalidImport=assessDriverReport({game:'th08',variant:'normal',checksMapped:['REPLAY-01'],scenarioIds:[]},processResult,report);
  assert.match(invalidImport.incompleteReasons[0],/successful hash-bound fixture observation/);
  assert.doesNotMatch(invalidImport.incompleteReasons[0],/natural ReplayStream cursor/);
});

test('desktop observations retain numbered evidence without self-certifying complete coverage',()=>{
  const report=sealed();report.targets[0].checks=[{id:'native-four-direction-arrows',adaptationIds:['INPUT-01'],status:'passed',complete:true,
    observation:{before:{x:10,y:20},after:{x:14,y:20}}}];
  const actual=assessDriverReport(selected,processResult,report);
  assert.deepEqual(actual.checks.map(item=>item.id),['INPUT-01']);assert.equal(actual.checks[0].complete,false);
  assert.equal(actual.status,'incomplete');assert.equal(actual.checks[0].completeEvidence.available,false);
  assert.deepEqual(actual.checks[0].proof,observedProof());
});

test('bare complete claims and prose cannot certify behavior without structured observations',()=>{
  for(const evidence of [undefined,'Everything passed',{},[]]){
    const report=sealed();report.targets[0].checks=[{id:'INPUT-01',status:'passed',complete:true,evidence}];
    const assessed=assessDriverReport(selected,processResult,report);
    assert.equal(assessed.status,'incomplete');assert.equal(assessed.checks[0].complete,false);
    assert.equal(adaptationCoverage([{...selected,status:'passed',checks:assessed.checks}]).complete,false);
  }
});

test('JSON evidence references require a same-product observation envelope bound to the claimed ID',()=>{
  const report=sealed();
  const observation={game:'th06',variant:'normal',adaptationIds:['INPUT-01'],
    observation:{before:{x:10,y:20},after:{x:14,y:20}}};
  report.observations=[observation];
  report.targets[0].checks=[{id:'INPUT-01',status:'passed',complete:false,evidenceRefs:['/observations/0']}];
  const valid=assessDriverReport(selected,processResult,report);assert.equal(valid.status,'passed');
  assert.deepEqual(valid.checks[0].proof.references,[{pointer:'/observations/0',sha256:identityDigest(observation)}]);

  const invalidReferences=['/missing','/targets/0','/targets/0/checks/0','/targets/0/checks/0/evidenceRefs','/status'];
  for(const reference of invalidReferences){
    const invalid=structuredClone(report);invalid.targets[0].checks[0].evidenceRefs=[reference];
    assert.equal(assessDriverReport(selected,processResult,invalid).status,'failed',reference);
  }
  for(const wrongOwner of [
    {...observation,game:'th07'},
    {...observation,variant:'multiplayer'},
    {...observation,adaptationIds:['INPUT-02']},
    {...observation,observation:{status:'passed'}},
  ]){
    const invalid=structuredClone(report);invalid.observations=[wrongOwner];
    assert.equal(assessDriverReport(selected,processResult,invalid).status,'failed',JSON.stringify(wrongOwner));
  }
});

test('a prose placeholder and an unknown complete declaration cannot certify a product',()=>{
  const report=sealed();
  report.targets[0].checks=[{id:'INPUT-01',status:'passed',complete:true,observation:{note:'checked'}}];
  const assessed=assessDriverReport(selected,processResult,report);
  assert.equal(assessed.status,'incomplete');assert.equal(assessed.checks[0].complete,false);
  assert.equal(adaptationCoverage([{...selected,...assessed}]).complete,false);
  report.schema='eagler-touhou/diagnostic-browser/1';
  report.targets[0].checks=[{id:'DISPLAY-03',status:'passed',complete:true,
    observation:{game:'th06',variant:'normal',adaptationIds:['DISPLAY-03'],observation:{note:'checked'}}}];
  assert.notEqual(assessDriverReport({...selected,checksMapped:['DISPLAY-03']},processResult,report).status,'passed');
});

test('a passing product shell without any planned check or scenario result is incomplete',()=>{
  const report=sealed();
  assert.equal(assessDriverReport(selected,processResult,report).status,'incomplete');
  assert.match(assessDriverReport(selected,processResult,report).incompleteReasons.join(' '),/INPUT-01/);
  report.targets[0].checks=[{id:'native-arrow-motion',checks:['INPUT-01'],status:'observed',
    observation:{before:{x:10,y:20},after:{x:14,y:20}}}];
  assert.equal(assessDriverReport(selected,processResult,report).status,'passed');
});

test('every mapped obligation needs its own result; an unrelated passing row cannot fill the gap',()=>{
  const planned={...selected,checksMapped:['INPUT-01','INPUT-02'],scenarioIds:['native-keyboard-and-focus']};
  const report=sealed();report.targets[0].checks=[
    {id:'keyboard-motion',checks:['INPUT-01'],status:'observed',observation:{before:{x:10,y:20},after:{x:14,y:20}}},
    {id:'unrelated-room-result',checks:['MP-15'],status:'passed',observation:{players:[1,2]}},
  ];
  report.targets[0].scenarios=[{id:'unrelated-scene',status:'passed',passed:true}];
  const partial=assessDriverReport(planned,processResult,report);
  assert.equal(partial.status,'incomplete');
  assert.match(partial.incompleteReasons.join(' '),/INPUT-02/);
  assert.deepEqual(partial.checks.map(check=>check.id),['INPUT-01','MP-15']);

  report.targets[0].checks.push({id:'focus-transition',checks:['INPUT-02'],status:'passed',
    observation:{before:{focused:false},after:{focused:true}}});
  assert.equal(assessDriverReport(planned,processResult,report).status,'passed',
    'all mapped rows satisfy the execution scope without requiring one shared scenario ID');
});

test('room execution rows must cover the declared route, participants, lifecycle and disconnect operations',()=>{
  const planned={game:'th08',variant:'multiplayer',group:'rooms',checksMapped:['MP-01','MP-10'],
    scenarioIds:['rooms-2p-rtc']};
  const required=['P1-directory','P1-room','P2-directory','P2-room','P1-ready','P2-ready','synchronized-start',
    'actual-native-frame-180','native-gamepad-sampling','multiplayer-native-pause-and-resume',
    'multiplayer-native-restart-and-second-game','capture-gameplay','peer-departure-and-freeze'];
  const room=()=>({schema:'eagler-touhou/room-browser/1',runId:'room-run',startedAt:'2026-10-19T01:00:00Z',
    finishedAt:'2026-10-19T01:00:01Z',status:'passed',passed:true,route:'rtc',players:2,
    disconnectRequested:true,lifecycleRequested:true,targets:[{game:'th08',variant:'multiplayer',status:'passed',passed:true,
      players:2,route:'rtc',runtimePassed:true,steps:required.map(name=>({name})),gamepad:[{passed:true},{passed:true}],
      lifecycle:{coverage:{complete:true},checks:[{id:'synchronized-pause-resume',passed:true},
        {id:'synchronized-restart-second-game',passed:true}]}}]});
  const complete=room();
  assert.equal(assessDriverReport(planned,processResult,complete).status,'passed');
  const truncated=room();truncated.targets[0].steps=truncated.targets[0].steps.filter(step=>step.name!=='peer-departure-and-freeze');
  const assessed=assessDriverReport(planned,processResult,truncated);
  assert.equal(assessed.status,'incomplete');
  assert.match(assessed.incompleteReasons.join(' '),/peer-departure-and-freeze/);
});

test('blocked or unreported execution cannot hide behind a passed outer report',()=>{
  for(const status of ['blocked','not-run',undefined]){
    const report=sealed();report.targets[0].status=status;
    assert.equal(assessDriverReport(selected,processResult,report).status,'incomplete');
  }
  const report=sealed();report.targets[0].scenarios=[{id:'required-scene',status:'blocked',passed:false}];
  assert.equal(assessDriverReport(selected,processResult,report).status,'incomplete');
});

test('incomplete scope, optional NONE observation and scenario success remain distinct',()=>{
  const report=sealed();report.status='incomplete';report.passed=false;
  assert.equal(assessDriverReport(selected,{...processResult,status:'failed',code:1},report).status,'incomplete');
  report.status='passed';report.passed=true;
  report.targets[0].checks=[{id:'native-arrow-motion',checks:['INPUT-01'],status:'observed'}];
  report.targets[0].modes=[{mode:'none',status:'recorded',passed:false},{mode:'ogg',status:'observed',passed:true}];
  assert.equal(assessDriverReport(selected,processResult,report).status,'passed');
  report.targets[0].modes[0].status='failed';
  assert.equal(assessDriverReport(selected,processResult,report).status,'failed');
});

test('a completed scenario cannot certify an adaptation ID without complete proof',()=>{
  const cases=[{...selected,id:'a',status:'passed',execution:'retained',checks:[{id:'INPUT-01',status:'passed',complete:false}]}];
  assert.equal(summarizeAdaptationCases(cases).passed,true);
  const coverage=adaptationCoverage(cases);assert.equal(coverage.complete,false);assert.equal(coverage.rows[0].status,'partial');
  assert.equal(coverage.rows[0].evidence[0].execution,'retained');
});

test('product-declared N/A combinations do not invalidate complete supported evidence',()=>{
  const cases=[{...selected,id:'2p',status:'passed',checks:[{id:'INPUT-01',status:'passed',complete:true,proof:observedProof()}]},
    {...selected,id:'3p',status:'not-applicable',checks:[]}];
  assert.equal(adaptationCoverage(cases).complete,true);
});

test('every selected applicable route must carry its own complete numbered proof',()=>{
  const cases=[{...selected,id:'rtc',status:'passed',checks:[{id:'INPUT-01',status:'passed',complete:true,proof:observedProof()}]},
    {...selected,id:'relay',status:'passed',checks:[]}];
  assert.equal(adaptationCoverage(cases).complete,false);
  cases[1].checks=[{id:'INPUT-01',status:'passed',complete:false}];assert.equal(adaptationCoverage(cases).complete,false);
  cases[1].checks[0].complete=true;assert.equal(adaptationCoverage(cases).complete,false);
  cases[1].checks[0].proof=observedProof();assert.equal(adaptationCoverage(cases).complete,true);
});

test('coverage preserves requirements with no driver and product applicability',()=>{
  const cases=[{...selected,id:'input',status:'passed',checks:[{id:'INPUT-01',status:'passed',complete:true,proof:observedProof()}]}];
  const targets=[{game:'th06',variant:'normal',requirements:[{id:'INPUT-01'},{id:'DISPLAY-03'},
    {id:'MP-01',reason:'single player product'}]}];
  const result=adaptationCoverage(cases,{targets});
  assert.deepEqual(result.rows.map(row=>row.status),['complete','unmapped','not-applicable']);
  assert.equal(result.complete,false);
  assert.equal(adaptationCoverage([],{targets}).rows.length,3,'missing tasks cannot remove selected requirements');
});

test('human review includes unmapped requirements, retained evidence and no fabricated verdict',()=>{
  const report={runId:'example',status:'incomplete',cases:[{id:'input',game:'th06',variant:'normal',group:'input',
    status:'passed',execution:'retained',report:{path:'/evidence/old.json'}}],coverage:{rows:[
    {game:'th06',variant:'normal',id:'INPUT-01',status:'partial',caseIds:['input']},
    {game:'th06',variant:'normal',id:'DISPLAY-03',status:'unmapped',caseIds:[]},
    {game:'th06',variant:'normal',id:'MP-01',status:'not-applicable',caseIds:[],reason:'单人模式'},
  ]}};
  const text=adaptationReviewMarkdown(report,{root:'/repo',checklist:{sections:[{id:'display',file:'presentation.md'}],
    checks:[{id:'DISPLAY-03',section:'display',label:'重复绘制'}]}});
  assert.match(text,/DISPLAY-03 \\?\| 重复绘制/);assert.match(text,/presentation\.md#display-03/);
  assert.match(text,/未关联自动任务/);assert.match(text,/沿用旧证据/);assert.match(text,/old\.json/);
  assert.doesNotMatch(text,/\| 待填写 \||### 人工审查场景集|人工通过/);
  report.cases[0].status='incomplete';report.cases[0].incompleteReasons=['Missing result for INPUT-02'];
  assert.match(adaptationReviewMarkdown(report),/Missing result for INPUT-02/);
});

test('human review carries canonical operations and expectations without accepting hidden rows',()=>{
  const report={runId:'readable',status:'planned',cases:[],coverage:{rows:[
    {game:'th08',variant:'normal',id:'DISPLAY-02',status:'unmapped',caseIds:[]},
    {game:'th08',variant:'normal',id:'DISPLAY-03',status:'unmapped',caseIds:[]},
  ]}};
  const guides={'presentation.md':[
    '| <a id="display-02"></a>DISPLAY-02 \\| 高刷 | 在实际高刷显示器移动角色，比较开关。 | 运动平滑且逻辑速度一致。 |',
    '<!--',
    '| <a id="display-03"></a>DISPLAY-03 \\| hidden | hidden operation | hidden expectation |',
    '-->',
    '```md',
    '| <a id="display-03"></a>DISPLAY-03 \\| example | example operation | example expectation |',
    '```',
  ].join('\n')};
  const text=adaptationReviewMarkdown(report,{root:'/repo',guides,checklist:{humanReviewBundles:[{
    id:'display',label:'人工显示审查表',checks:['DISPLAY-02','DISPLAY-04'],method:'presentation.md#人工显示审查表',
    judgement:'集中观察高刷运动。',preparation:'准备同一构建的运动素材。',
  }]}});
  assert.match(text,/在实际高刷显示器移动角色，比较开关。/);
  assert.match(text,/运动平滑且逻辑速度一致。/);
  assert.match(text,/人工显示审查表/);
  assert.doesNotMatch(text,/hidden operation|example operation|人工通过/);
  assert.equal((text.match(/\| 待审查 \|/g)??[]).length,1);
  assert.doesNotMatch(text,/\| 待填写 \|/);
});

test('canonical methods remain available inside collapsed machine detail, with a small human summary',async()=>{
  const checklist=JSON.parse(await readFile(new URL('../../docs/adaptation/checklist.json',import.meta.url),'utf8'));
  const guides=Object.fromEntries(await Promise.all(checklist.sections.map(async section=>[section.file,
    await readFile(new URL('../../docs/adaptation/'+section.file,import.meta.url),'utf8')])));
  const report={runId:'all-methods',status:'planned',cases:[],coverage:{rows:checklist.checks.map(({id})=>({
    game:'th08',variant:'normal',id,status:'unmapped',caseIds:[]}))}};
  const text=adaptationReviewMarkdown(report,{root:'/repo',checklist,guides});
  assert.doesNotMatch(text,/见方法链接/);
  assert.equal((text.match(/\| 待审查 \|/g)??[]).length,checklist.humanReviewBundles.length);
  assert.doesNotMatch(text,/\| 待填写 \|/);
  assert.ok(text.includes(`自动编号 ${checklist.checks.length} 项 | 未关联自动任务 ${checklist.checks.length}`));
  const detail=text.match(/<details>\s*<summary>展开全部自动结果与检验方法<\/summary>([\s\S]*?)<\/details>/)?.[1];
  assert.ok(detail,'full machine methods are available on demand');
  assert.match(detail,/固定逻辑状态重复绘制，比较状态摘要/);
  assert.doesNotMatch(text.slice(0,text.indexOf('<details>')),/固定逻辑状态重复绘制，比较状态摘要/);
  const humanSection=text.split('### 自动结果与方法')[0];
  assert.doesNotMatch(humanSection,/DISPLAY-01|DISPLAY-03|DISPLAY-07|INPUT-01/,
    'missing automated checks never become human sign-off tasks');
  assert.match(text,/固定逻辑状态重复绘制，比较状态摘要/);
  assert.match(text,/额外绘制只改变画面，逻辑与存档状态相同/);
});

test('automatic failures and absent drivers cannot inflate human work or create a human pass',async()=>{
  const checklist=JSON.parse(await readFile(new URL('../../docs/adaptation/checklist.json',import.meta.url),'utf8'));
  const automaticOnly={runId:'automatic-failed',status:'failed',cases:[],coverage:{rows:[
    {game:'th08',variant:'normal',id:'INPUT-01',status:'failed',caseIds:[]},
    {game:'th08',variant:'normal',id:'DISPLAY-03',status:'unmapped',caseIds:[]},
    {game:'th08',variant:'normal',id:'DISPLAY-05',status:'not-applicable',caseIds:[]},
  ]}};
  const text=adaptationReviewMarkdown(automaticOnly,{root:'/repo',checklist});
  assert.match(text,/失败/);assert.match(text,/未关联自动任务/);
  assert.doesNotMatch(text,/\| 待审查 \||### 人工审查场景集|人工通过/);
});

test('a misspelled or duplicated manual binding cannot silently remove human scope',async()=>{
  const checklist=JSON.parse(await readFile(new URL('../../docs/adaptation/checklist.json',import.meta.url),'utf8'));
  const unknown=structuredClone(checklist);unknown.humanReviewBundles[0].checks.push('DISPLAY-99');
  assert.throws(()=>adaptationReviewMarkdown({},{checklist:unknown}),/unknown adaptation ID/);
  const duplicate=structuredClone(checklist);duplicate.humanReviewBundles.push(duplicate.humanReviewBundles[0]);
  assert.throws(()=>adaptationReviewMarkdown({},{checklist:duplicate}),/unique IDs/);
  const missing=structuredClone(checklist);delete missing.humanReviewBundles[0].preparation;
  assert.throws(()=>adaptationReviewMarkdown({},{checklist:missing}),/missing preparation/);
});
