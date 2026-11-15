import test from 'node:test';
import assert from 'node:assert/strict';
import {acceptTh09MatchStats as accept,bindTh09Duel,reconcileTh09Duel,th09PlayerStats as stats} from '../src/th09-room-stats.mjs';

function start(room, ids = ['host', 'guest'], players = null) {
  room.lobby.seats = ids.map(clientId => ({clientId}));
  const run = {lobbyClientIds: ids};
  bindTh09Duel(room, run, players);
  return run;
}

test('whole-match wins and minutes accumulate for the same opponents and reset when a seat changes', () => {
  const room={id:'th09mp-1234',lobby:{startSerial:1}},run=start(room);
  const report=(frames,extra={})=>({serial:1,start:0,frames,...extra});
  assert.equal(accept(room,run,'guest',report(3600,{winner:0})),false);
  assert.equal(accept(room,run,'host',report(3600,{winner:2})),false);
  assert.equal(accept(room,run,'host',report(3600)),true);
  assert.deepEqual(stats(room,'host'),{wins:0,losses:0,minutes:1});
  assert.equal(accept(room,run,'host',report(7100,{winner:1})),true);
  assert.deepEqual(stats(room,'host'),{wins:0,losses:1,minutes:1});
  assert.deepEqual(stats(room,'guest'),{wins:1,losses:0,minutes:1});
  assert.equal(accept(room,run,'host',report(7100,{winner:1})),false);
  assert.equal(accept(room,run,'host',report(7300)),false);
  assert.equal(accept(room,run,'host',report(0,{start:7200})),true);
  assert.deepEqual(stats(room,'host'),{wins:0,losses:1,minutes:1});
  assert.deepEqual(stats(room,'guest'),{wins:1,losses:0,minutes:1});
  assert.equal(accept(room,run,'host',report(0,{start:7200})),false);
  assert.equal(accept(room,run,'host',report(3600,{start:7200})),true);
  assert.equal(accept(room,run,'host',report(4000,{start:7200,winner:0})),true);
  assert.deepEqual(stats(room,'host'),{wins:1,losses:1,minutes:3});
  assert.equal(accept(room,run,'host',report(7100,{winner:1})),false);
  room.lobby.startSerial=2;
  assert.deepEqual(stats(room,'host'),{wins:1,losses:1,minutes:3});
  assert.equal(accept(room,run,'host',report(8000,{start:12000,winner:0})),false);
  room.lobby.seats[1] = null;
  reconcileTh09Duel(room);
  assert.deepEqual(stats(room,'host'),{wins:0,losses:0,minutes:0});
  const replacement = start(room,['host','new']);
  assert.notEqual(run.th09Duel.id,replacement.th09Duel.id);
  assert.equal(accept(room,replacement,'host',{serial:2,start:0,frames:3600,winner:0}),true);
  assert.deepEqual(stats(room,'host'),{wins:1,losses:0,minutes:1});
  assert.deepEqual(stats(room,'new'),{wins:0,losses:1,minutes:1});
  assert.deepEqual(stats({id:'th09mp-5678'},'host'),{wins:0,losses:0,minutes:0});
  assert.equal(stats({id:'th08mp-1234'},'host'),undefined);
});

test('partial minutes survive character changes and duplicate progress is not counted twice', () => {
  const room={id:'th09mp-1234',lobby:{startSerial:1}},run=start(room);
  const report={serial:1,start:0,frames:1800};
  assert.equal(accept(room,run,'host',report),true);
  assert.equal(accept(room,run,'host',report),false);
  assert.equal(accept(room,run,'host',{...report,frames:1700}),false);
  assert.equal(stats(room,'host').minutes,0);
  room.lobby.startSerial=2;
  const next=start(room);
  assert.equal(next.th09Duel.id,run.th09Duel.id);
  assert.equal(accept(room,next,'host',{serial:2,start:0,frames:1800}),true);
  assert.equal(stats(room,'host').minutes,1);
  assert.equal(stats(room,'guest').minutes,1);
  assert.equal(accept(room,next,'host',{serial:2,start:0,frames:1800,winner:0}),true);
  assert.deepEqual(stats(room,'host'),{wins:1,losses:0,minutes:1});
});

test('account history needs the same final report from both admitted seats, in either arrival order', () => {
  const room = {id:'th09mp-1234',lobby:{startSerial:1}};
  const players = ['a','b'];
  const run = start(room, ['host','guest'], players);
  const recorded = [], record = value => recorded.push(value);
  const result = {serial:1,start:0,frames:4000,winner:0};
  assert.equal(accept(room,run,'host',result,record),true);
  assert.deepEqual(recorded,[]);
  accept(room,run,'spectator',result,record);
  assert.deepEqual(recorded,[]);
  accept(room,run,'guest',result,record);
  assert.deepEqual(recorded,[{duelId:run.th09Duel.id,roundId:'1:0',startedAt:run.th09Duel.startedAt,players,winner:0,frames:4000}]);
  accept(room,run,'guest',result,record);
  accept(room,run,'host',result,record);
  const second = {...result,start:5000,winner:1};
  accept(room,run,'guest',second,record);
  accept(room,run,'host',second,record);
  assert.deepEqual(recorded.map(item=>[item.roundId,item.winner]),[['1:0',0],['1:5000',1]]);
  const disputed = {...result,start:10000};
  accept(room,run,'guest',{...disputed,winner:1},record);
  accept(room,run,'host',disputed,record);
  assert.equal(recorded.length,2);
  room.lobby.seats[1]=null;
  reconcileTh09Duel(room);
  room.lobby.startSerial++;
  const rematch=start(room,['host','guest'],players);
  assert.notEqual(rematch.th09Duel.id,run.th09Duel.id);
  assert.deepEqual(stats(room,'host'),{wins:0,losses:0,minutes:0});
});

test('two guests can confirm one unrated result for the room', () => {
  const room = {id:'th09mp-1234',lobby:{startSerial:1}};
  const run = start(room, ['host','guest'], [null,null]);
  const recorded = [];
  const report = {serial:1,start:0,frames:3600,winner:0};
  for (const side of ['host','guest','guest']) accept(room,run,side,report,value=>recorded.push(value));
  assert.equal(recorded.length,1);
  assert.deepEqual(recorded[0].players,[null,null]);
});
