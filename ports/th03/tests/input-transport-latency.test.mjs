import {test} from 'node:test';
import assert from 'node:assert/strict';
const transports = await Promise.all(['th01', 'th02', 'th03', 'th04'].map(async game =>
  [game, (await import(`../../${game}/web/netplay/input-transport.js`)).InputTransport]));

function setup(InputTransport) {
  let now = 0;
  const queues = [[], []], timers = [], received = [[], []];
  const peers = [0, 1].map(() => new InputTransport('test', {now: () => now,
    repeat: (fn, ms) => {timers.push(ms); return fn;}, cancel: () => {}}));
  for (let id = 0; id < 2; id++) {
    const lane = fast => ({label: fast ? 'test-fast' : 'test', ordered: !fast,
      maxRetransmits: fast ? 0 : null, maxPacketLifeTime: null, readyState: 'open', bufferedAmount: 0,
      send: data => queues[id ^ 1].push({data, fast}), close() {}});
    peers[id].bind(lane(false), false); peers[id].bind(lane(true), true);
    peers[id].onmessage = event => received[id].push(JSON.parse(event.data));
  }
  const deliver = id => {while (queues[id].length) {const {data, fast} = queues[id].shift(); peers[id].receive(data, fast);}};
  deliver(0); deliver(1); deliver(0);
  return {peers, queues, received, timers, deliver, time: value => {now = value;}, close: () => peers.forEach(p => p.close())};
}

for (const [game, InputTransport] of transports) test(`${game}: lost last frame is retried within one 60 Hz tick without breaking control order`, () => {
  const f = setup(InputTransport);
  try {
    assert.deepEqual(f.timers, [8, 8]);
    f.peers[0].send(JSON.stringify({type: 'frame', frame: 0}));
    assert.equal(f.queues[1].shift().fast, true); // drop first fast packet
    f.peers[0].send(JSON.stringify({type: 'control', value: 'after frame'}));
    f.deliver(1);
    assert.deepEqual(f.received[1], []); // fence waits for missing frame
    f.time(15); f.peers[0].pump(); assert.equal(f.queues[1].length, 0);
    f.time(16); f.peers[0].pump();
    const retry = f.queues[1][0];
    f.deliver(1); f.peers[1].receive(retry.data, true); // delayed duplicate
    assert.deepEqual(f.received[1], [{type:'frame',frame:0}, {type:'control',value:'after frame'}]);
    assert.equal(f.peers[0].transportStats.retransmitted, 1);
  } finally {f.close();}
});

test('th03 packet size accounting matches encoded JSON including unicode and escaped data', () => {
  const f=setup(transports.find(([game])=>game==='th03')[1]);
  try {
    for(let frame=0;frame<30;frame++)f.peers[0].send(JSON.stringify({type:'native-rollback-input',frame,input:frame,label:'夢"\\時空'}));
    const transport=f.peers[0], encoder=new TextEncoder();
    for(const limit of [100,200,500,1150,16384])for(const preferred of [undefined,1,15,30]){
      const actual=transport.packet(limit,preferred), expected=[];
      const append=entry=>{expected.push({id:entry.id,barrier:entry.barrier,data:entry.data});if(encoder.encode(JSON.stringify({wire:'pc98-input/1',kind:'inputs',ack:transport.receivedInput,entries:expected})).length>limit){expected.pop();return false;}return true;};
      if(preferred)append(transport.pending.get(preferred));
      for(const entry of transport.pending.values())if(entry.id!==preferred&&!append(entry))break;
      assert.deepEqual(actual.entries,expected);
    }
  } finally {f.close();}
});

for (const [game, InputTransport] of transports) test(`${game}: newer fast inputs do not cancel a reliable ACK during one-way loss`, () => {
  const f=setup(InputTransport);
  try {
    const send=frame=>f.peers[0].send(JSON.stringify({type:'frame',frame}));
    send(0); f.deliver(1);
    // The return fast lane is down; forward traffic still gets through.
    f.time(80); f.peers[0].pump(); f.deliver(1);
    f.time(81); send(1); f.deliver(1); f.peers[1].pump();
    f.queues[0].splice(0,f.queues[0].length,...f.queues[0].filter(packet=>!packet.fast));
    f.deliver(0);
    assert.equal(f.peers[0].transportStats.pending,0);
    assert.deepEqual(f.received[1].map(input=>input.frame),[0,1]);
  } finally {f.close();}
});

test('th03 delivers recovered inputs together before their fenced control', () => {
  const f=setup(transports.find(([game])=>game==='th03')[1]);
  try {
    const order=[];
    f.peers[1].oninputbatch=batch=>order.push(batch.map(text=>JSON.parse(text).frame));
    f.peers[1].onmessage=event=>order.push(JSON.parse(event.data).type);
    f.peers[0].send(JSON.stringify({type:'native-rollback-input',frame:0,input:0}));
    f.peers[0].send(JSON.stringify({type:'native-charge',frame:0,sample:[0,0,-1,0]}));
    const recovered=f.queues[1].pop();f.queues[1].length=0;
    f.peers[0].send(JSON.stringify({type:'native-rollback-proof'}));
    f.deliver(1);assert.deepEqual(order,[]);
    f.peers[1].receive(recovered.data,true);
    assert.deepEqual(order,[[0,0],'native-rollback-proof']);
  } finally {f.close();}
});
