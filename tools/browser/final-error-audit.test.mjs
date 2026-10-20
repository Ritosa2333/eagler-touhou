import test from 'node:test';
import assert from 'node:assert/strict';
import {auditBrowserFinalErrors,gateReplayRecordOnFinalErrors} from './final-error-audit.mjs';

const cleanChannels=()=>({pageErrors:[],consoleErrors:[],httpErrors:[],protocolErrors:[],responseErrors:[],teardownErrors:[]});

test('final browser audit passes only with every raw error channel empty',()=>{
  const channels=cleanChannels();
  const result=auditBrowserFinalErrors(channels);
  assert.equal(result.status,'passed');
  assert.deepEqual(result.failedChannels,[]);
  for(const name of Object.keys(channels))assert.strictEqual(result.channels[name],channels[name]);
});

test('each late browser or teardown failure remains fatal and its raw event is retained',()=>{
  const cases=[
    ['pageErrors',{message:'late page error'}],
    ['consoleErrors','late console error'],
    ['httpErrors',{url:'https://127.0.0.1/runtime/th06/main.wasm',failure:'net::ERR_ABORTED',epoch:1}],
    ['protocolErrors',{protocol:'eagler-touhou/1',game:'th06',epoch:1,event:'abort',reason:'late abort'}],
    ['responseErrors',{kind:'identity',url:'https://127.0.0.1/host/th06',expectedSha256:'a',actualSha256:'b'}],
    ['teardownErrors',{phase:'responseProbe.stop',error:'CDP session did not close'}],
  ];
  for(const [channel,event] of cases){
    const channels=cleanChannels();channels[channel].push(event);
    const record={status:'passed',playback:{scenarioStatus:'passed',scenarioEvidence:{browserAndRuntimeErrors:'pending-final-audit'}}};
    const audit=gateReplayRecordOnFinalErrors(record,channels);
    assert.equal(audit.status,'failed',channel);
    assert.deepEqual(audit.failedChannels,[channel]);
    assert.strictEqual(audit.channels[channel][0],event);
    assert.equal(record.status,'failed',channel);
    assert.equal(record.playback.preTeardownScenarioStatus,'passed','retain the scenario result observed before teardown');
    assert.equal(record.playback.scenarioStatus,'failed','late failures must fail the final scenario result');
    assert.equal(record.playback.scenarioEvidence.finalErrorAudit,'failed');
    assert.equal(record.playback.scenarioEvidence.browserAndRuntimeErrors,'present');
    assert.match(record.error,new RegExp(channel));
  }
});

test('a clean final audit closes the pending browser error evidence',()=>{
  const record={status:'passed',playback:{scenarioStatus:'passed',scenarioEvidence:{browserAndRuntimeErrors:'pending-final-audit'}}};
  const result=gateReplayRecordOnFinalErrors(record,cleanChannels());
  assert.equal(result.status,'passed');
  assert.equal(record.playback.scenarioStatus,'passed');
  assert.equal(record.playback.scenarioEvidence.finalErrorAudit,'passed');
  assert.equal(record.playback.scenarioEvidence.browserAndRuntimeErrors,'none');
});

test('audit rejects missing or malformed channels instead of treating them as empty',()=>{
  const channels=cleanChannels();delete channels.httpErrors;
  assert.throws(()=>auditBrowserFinalErrors(channels),/httpErrors must be an array/);
  assert.throws(()=>auditBrowserFinalErrors({...cleanChannels(),protocolErrors:null}),/protocolErrors must be an array/);
});

test('a failed required cross-origin asset is fatal and is not treated as outside the browser scenario',()=>{
  const channels=cleanChannels();
  channels.httpErrors.push({url:'https://cdn.example.test/fonts/runtime.woff2',failure:'net::ERR_NAME_NOT_RESOLVED'});
  const result=auditBrowserFinalErrors(channels);
  assert.equal(result.status,'failed');
  assert.deepEqual(result.failedChannels,['httpErrors']);
  assert.deepEqual(result.channels.httpErrors,channels.httpErrors);
});
