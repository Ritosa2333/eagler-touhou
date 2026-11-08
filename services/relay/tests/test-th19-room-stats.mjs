import test from 'node:test';
import assert from 'node:assert/strict';
import {acceptTh19MatchStats,th19PlayerStats as stats} from '../src/th19-room-stats.mjs';

function fixture() {
  const room={id:'th19mp-1234',lobby:{}};
  const start=(serial,clientIds=['host','guest'])=>{
    room.lobby={startSerial:serial,seats:clientIds.map(clientId=>({clientId}))};
    room.th19StatsRun={serial,clientIds,reports:[null,null]};
  };
  start(1);
  const report=(id,frames,extra={})=>acceptTh19MatchStats(room,id,{
    serial:room.lobby.startSerial,start:0,frames,...extra,
  });
  return {room,start,report};
}

test('single-seat progress cannot change the public score or the shared minutes',()=>{
  const {room,report}=fixture();
  assert.equal(report('host',216000,{winner:0}),false);
  assert.deepEqual(stats(room,'host'),{wins:0,losses:0,minutes:0});
  assert.equal(report('guest',3600),true);
  assert.deepEqual(stats(room,'host'),{wins:0,losses:0,minutes:1});
  assert.equal(report('host',216000,{winner:0}),false);
  assert.equal(report('guest',3500),false);
});

test('matching final reports settle exactly once in either delivery order',()=>{
  for(const seats of [['host','guest'],['guest','host']]){
    const {room,report}=fixture();
    assert.equal(report(seats[0],7200,{winner:1}),false);
    assert.equal(report(seats[1],7200,{winner:1}),true);
    assert.equal(report(seats[1],7200,{winner:1}),false);
    assert.equal(report(seats[0],1,{start:7200,winner:0}),false);
    assert.deepEqual(stats(room,'host'),{wins:0,losses:1,minutes:2});
    assert.deepEqual(stats(room,'guest'),{wins:1,losses:0,minutes:2});
  }
});

test('disagreeing finals leave wins unchanged and invalid reports cannot poison the run',()=>{
  for(const result of [{frames:3600,winner:0},{frames:3700,winner:1}]){
    const {room,report}=fixture();
    assert.equal(report('outsider',3600,{winner:0}),false);
    assert.equal(report('host',0,{start:7200,winner:0}),false);
    assert.equal(report('host',0,{winner:0}),false);
    assert.deepEqual(room.th19StatsRun.reports,[null,null]);
    assert.equal(report('host',3600,{winner:1}),false);
    assert.equal(report('guest',result.frames,{winner:result.winner}),true);
    assert.equal(report('guest',3600,{winner:1}),false);
    assert.deepEqual(stats(room,'guest'),{wins:0,losses:0,minutes:1});
  }
});

test('only the current run and its admitted seats can contribute to room totals',()=>{
  const {room,start,report}=fixture();
  report('host',1800);
  report('guest',1800);
  start(2,['host','new']);
  assert.equal(report('guest',1800,{serial:1,winner:1}),false);
  assert.equal(report('guest',1800,{winner:1}),false);
  assert.equal(report('host',1800,{winner:0}),false);
  assert.equal(report('new',1800,{winner:0}),true);
  assert.deepEqual(stats(room,'host'),{wins:1,losses:0,minutes:1});
  assert.deepEqual(stats(room,'new'),{wins:0,losses:1,minutes:0});
  assert.equal(stats({id:'th08mp-1234'},'host'),undefined);
});
