import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import net from 'node:net';
import {setTimeout as delay} from 'node:timers/promises';
import {WebSocket, WebSocketServer} from 'ws';
import {createNat4CandidateExchange, readNat4PortSteps} from '../src/nat4-candidates.mjs';

import {createSwarmStandbyPlanner, rankSwarmCandidates, selectSwarmRoute} from '../src/swarm-route-quality.mjs';

{
  const workers = [0, 12, 9].map(id => ({id, ip:`192.0.2.${id + 1}`}));
  const pings = times => ({at:100, samples:new Map(workers.map(({id,ip}, i) => [id, {ip, rtt:times[i]}]))});
  const ranked = rankSwarmCandidates(workers, null, null, 101, [pings([10, 44, 31]), pings([95, 20, 42])]);
  assert.deepEqual(ranked.map(({worker, estimatedRtt}) => [worker, estimatedRtt]), [[12,64], [9,73], [0,105]]);
  const distant = rankSwarmCandidates(workers, null, null, 101, [pings([140, 130, 160]), pings([160, 165, 145])]);
  assert.deepEqual(selectSwarmRoute(distant, true), {mode:'turn'});
  assert.equal(selectSwarmRoute(distant, false), null);
  assert.deepEqual(rankSwarmCandidates(workers, null, null, 101, [pings([10, 44, 31])]).map(({worker}) => worker), [0,9,12]);
  assert.equal(rankSwarmCandidates(workers, null, null, 300_000, [pings([10, 44, 31])])[0].measuredEndpoints, 0);

  const probes = new Map(workers.map(({id})=>['swarm-'+id,{sent:16,received:16,p95:205,at:100}]));
  const unsuitable = rankSwarmCandidates(workers, probes, probes, 101);
  assert.deepEqual(selectSwarmRoute(unsuitable, true), {mode:'turn'});
  assert.equal(selectSwarmRoute(unsuitable, false), null);
  probes.delete('swarm-0');
  assert.equal(selectSwarmRoute(rankSwarmCandidates(workers, probes, probes, 101), true), null);
  probes.set('swarm-0',{sent:16,received:16,p95:60,at:100});
  assert.deepEqual(selectSwarmRoute(rankSwarmCandidates(workers, probes, probes, 101), true), {mode:'swarm',worker:0});

  const updates = [];
  const survey = createSwarmStandbyPlanner({workers:[12, 9, 0],ordinaryTurn:true,dispatch:update=>updates.push(update)});
  const current = {sent:16, received:16, p95:80};
  try {
    assert.equal(survey.consider('direct', current, true), false);
    assert.equal(survey.consider('swarm-0', current), false);
    assert.equal(survey.consider('swarm-0', current, true), true);
    assert.equal(updates[0].route, 'direct');
  } finally {
    survey.close();
  }
}

for (const healthy of [true, false]) {
  const dispatched = [];
  const survey = createSwarmStandbyPlanner({workers:[5, 12, 0, 2, 3],ordinaryTurn:true,dispatch:update=>dispatched.push(update)});
  const quality = p95=>({sent:16, received:16, p95});
  const measure = (name, left, right) => {
    const trial = dispatched.at(-1);
    assert.deepEqual([trial.phase,trial.route],['measure',name]);
    survey.report(0,trial.serial,left == null ? null : quality(left));
    survey.report(1,trial.serial,right == null ? null : quality(right));
  };
  try {
    assert.ok(survey.consider('swarm-5',quality(250)));
    measure('direct',null,null);
    measure('swarm-12',71,healthy ? 125 : 190);
    measure('swarm-0',94,healthy ? 102 : 210);
    if (!healthy) measure('turn',73,113);
    assert.deepEqual([dispatched.at(-1).phase,dispatched.at(-1).route],['select',healthy ? 'swarm-0' : 'turn']);
    assert.equal(dispatched.some(update=>update.route==='turn'), !healthy);
  } finally {
    survey.close();
  }
}

for (const healthy of [true, false]) {
  const updates = [];
  const planner = createSwarmStandbyPlanner({workers:[0,12], ordinaryTurn:true, dispatch:update=>updates.push(update)});
  try {
    assert.ok(planner.consider('swarm-0',{sent:12,received:12,p95:220}));
    const direct = updates.at(-1);
    assert.equal(direct.route,'direct');
    assert.ok(planner.consider('swarm-0',{sent:8,received:0,p95:null}));
    const standby = updates.at(-1);
    assert.equal(standby.retired,direct.serial);
    assert.equal(standby.route,'swarm-12');
    assert.equal(planner.acceptsSignal(direct.serial),false);
    planner.report(0,standby.serial,{sent:12,received:12,p95:40});
    planner.report(1,standby.serial,{sent:12,received:12,p95:healthy ? 80 : 225});
    if (!healthy) {
      const turn = updates.at(-1);
      assert.deepEqual([turn.phase,turn.route],['measure','turn']);
      planner.report(0,turn.serial,{sent:12,received:12,p95:90});
      planner.report(1,turn.serial,{sent:12,received:12,p95:100});
    }
    assert.deepEqual([updates.at(-1).phase,updates.at(-1).route],['select',healthy ? 'swarm-12' : 'turn']);
    assert.equal(planner.consider('swarm-0',{sent:8,received:0,p95:null}),false);
  } finally {
    planner.close();
  }
}

{
  const updates = [];
  const planner = createSwarmStandbyPlanner({workers:[0,12], ordinaryTurn:true, dispatch:update=>updates.push(update)});
  const report = (route, p95) => {
    const trial = updates.at(-1);
    assert.equal(trial.route, route);
    for (const player of [0,1])
      planner.report(player,trial.serial,p95 == null ? null : {sent:12,received:12,p95});
  };
  try {
    assert.ok(planner.consider('swarm-0',{sent:12,received:12,p95:400}));
    report('direct',null);
    report('swarm-12',190);
    report('turn',280);
    assert.deepEqual([updates.at(-1).phase,updates.at(-1).route],['select','swarm-12']);
    assert.equal(planner.consider('swarm-0',{sent:8,received:0,p95:null}),false);
  } finally {
    planner.close();
  }
}

{
  const updates = [];
  const planner = createSwarmStandbyPlanner({workers:[0,12], ordinaryTurn:true, dispatch:update=>updates.push(update)});
  try {
    assert.ok(planner.consider('swarm-0',{sent:12,received:12,p95:40},true));
    const direct = updates.at(-1);
    planner.report(0,direct.serial,null);
    const standby = updates.at(-1);
    assert.equal(standby.route,'swarm-12');
    assert.ok(planner.consider('swarm-0',{sent:8,received:0,p95:null}));
    assert.equal(updates.at(-1),standby);
    assert.ok(planner.acceptsSignal(standby.serial));
    planner.report(0,standby.serial,null);
    assert.deepEqual([updates.at(-1).phase,updates.at(-1).route],['measure','turn']);
  } finally {
    planner.close();
  }
}

const port = await new Promise(resolve => {
  const listener = net.createServer().listen(0, '127.0.0.1', () => {
    const {port} = listener.address();
    listener.close(() => resolve(port));
  });
});
const addresses = ['192.0.2.1', '198.51.100.2'];
const coordinated = [];
const asymmetric = createNat4CandidateExchange(addresses,
  readNat4PortSteps(`${addresses[0]}=0,${addresses[1]}=1`),
  (seat, signal) => coordinated.push({seat,...signal}));
const observedCandidate = address => ({sdpMid:'0',candidate:`candidate:42 1 UDP 1 ${address} 34000 typ srflx`});
try {
  for (const [seat, address] of addresses.entries()) {
    asymmetric.send(seat, {description:{type:seat?'answer':'offer',sdp:'v=0\r\n'}});
    asymmetric.send(seat, {candidate:observedCandidate(address)});
  }
  const delivered = coordinated.filter(signal => signal.candidate);
  assert.equal(delivered.length, 3);
  assert.deepEqual(delivered[0].candidate, observedCandidate(addresses[0]));
  assert.deepEqual(delivered[1].candidate, observedCandidate(addresses[1]));
  assert.match(delivered[2].candidate.candidate, /^candidate:42next 1 UDP 1 198\.51\.100\.2 34001 typ srflx$/);
} finally {
  asymmetric.close();
}
const observers = ['stun:a.example.test:3478', 'stun:b.example.test:3478', 'stun:c.example.test:3478'];
const partialSignals = [];
const partialExchange = createNat4CandidateExchange(addresses, new Map(),
  (seat, signal) => partialSignals.push({seat, ...signal}), observers);
partialExchange.send(0, {description:{type:'offer',sdp:'v=0\r\n'}});
partialExchange.send(1, {description:{type:'answer',sdp:'v=0\r\n'}});
partialExchange.send(0, {candidate:observedCandidate(addresses[0])});
const ordinaryRelay = {sdpMid:'0',candidate:'candidate:99 1 UDP 1 192.0.2.9 35000 typ relay'};
partialExchange.send(0, {candidate:ordinaryRelay});
assert.deepEqual(partialSignals.filter(signal => signal.candidate).map(signal => signal.candidate),
  [observedCandidate(addresses[0]), ordinaryRelay], 'direct and TURN candidates are forwarded while observations are pending');
for (const [index, observer] of observers.entries()) {
  const candidate = observedCandidate(addresses[1]);
  partialExchange.send(1, {observer, candidate:{...candidate,
    candidate:candidate.candidate.replace('34000', String(34000 + index))}});
}
const partialResult = delay(4600).then(() => {
  try {
    const firstCandidate = partialSignals.find(signal => signal.candidate?.candidate.includes('42next'));
    assert.equal(firstCandidate?.seat, 1);
    assert.match(firstCandidate?.candidate.candidate, / 34003 typ srflx$/);
  } finally {
    partialExchange.close();
  }
});
const inferred = [];
const automaticExchange = createNat4CandidateExchange(addresses, new Map(),
  (seat, signal) => inferred.push({seat, ...signal}), observers);
try {
  for (const seat of [0, 1])
    automaticExchange.send(seat, {description: {type: seat ? 'answer' : 'offer', sdp: 'v=0\r\n'}});
  for (const observer of observers)
    automaticExchange.send(0, {candidate: observedCandidate(addresses[0]), observer});
  // A different local ICE base reporting first must not consume an observer
  // slot in the base that has all three observations.
  automaticExchange.send(1, {candidate: {sdpMid:'0',
    candidate:`candidate:other 1 UDP 1 ${addresses[1]} 36000 typ srflx`}, observer: observers[0]});
  for (const [index, port] of [[1, 34002], [0, 34000], [1, 34002], [2, 34004]]) {
    const candidate = observedCandidate(addresses[1]);
    automaticExchange.send(1, {candidate: {...candidate, candidate: candidate.candidate.replace('34000', String(port))},
      observer: observers[index]});
  }
  assert.deepEqual(inferred.find(signal => signal.seat === 0 && signal.candidate)?.candidate, observedCandidate(addresses[0]));
  assert.match(inferred.find(signal => signal.seat === 1 && signal.candidate)?.candidate.candidate, / 34006 typ srflx$/);
} finally {
  automaticExchange.close();
}
const irregular = [];
const irregularExchange = createNat4CandidateExchange(addresses, new Map([[addresses[0], 0]]),
  (seat, signal) => irregular.push({seat, ...signal}), observers);
try {
  for (const seat of [0, 1])
    irregularExchange.send(seat, {description: {type: seat ? 'answer' : 'offer', sdp: 'v=0\r\n'}});
  irregularExchange.send(0, {candidate: observedCandidate(addresses[0])});
  for (const [index, port] of [[0, 34000], [1, 34002], [2, 34005]]) {
    const candidate = observedCandidate(addresses[1]);
    irregularExchange.send(1, {candidate: {...candidate, candidate: candidate.candidate.replace('34000', String(port))},
      observer: observers[index]});
  }
  assert.deepEqual(irregular.filter(signal => signal.candidate).map(signal => signal.candidate),
    [observedCandidate(addresses[0])], 'irregular observations produce no predicted candidate');
} finally {
  irregularExchange.close();
}
const stunUrl = 'stun:stun.example.test:3478';
const ordinaryTurnUrl = 'turn:ordinary.example.test:3478';
const swarmUrl = 'turn:swarm.example.test:3478?transport=udp';
const control = new WebSocketServer({host:'127.0.0.1', port:0});
let refuseSwarmGrant = false;
let swarmAllocations = 0;
await new Promise(resolve => control.once('listening', resolve));
control.on('connection', ws => ws.once('message', raw => {
  const {data} = JSON.parse(raw);
  if (data.n === 'new_naive_udp_relay') {
    ++swarmAllocations;
    assert.equal(data.p.worker_id, 12);
    assert.equal(data.p.access_token, 'integration-swarm-access-token');
    ws.send(JSON.stringify({event:'result', data:{i:data.i, r:refuseSwarmGrant ? null : {relay_addr:'198.51.100.2:35000', relay_code:0}}}));
    return;
  }
  assert.equal(data.n, 'list_workers');
  ws.send(JSON.stringify({event:'result', data:{i:data.i, r:[
    {id:0, ip:addresses[0], active_at:Date.now()}, {id:12, ip:addresses[1], active_at:Date.now()},
  ]}}));
}));
const relay = spawn(process.execPath, ['src/netplay-relay.mjs'], {
  windowsHide: true,
  env: {
    ...process.env,
    EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1',
    EAGLER_NETPLAY_RELAY_PORT: String(port),
    EAGLER_NETPLAY_MAX_ROOMS_PER_IP: '4',
    EAGLER_NETPLAY_LOBBY_RECONNECT_GRACE_MS: '1000',
    EAGLER_NETPLAY_TH09_SPECTATOR_TIMELINE: '1',
    EAGLER_NETPLAY_TRUSTED_PROXIES: '127.0.0.1',
    EAGLER_NETPLAY_NAT4_PORT_STEPS: addresses.map(address => address + '=1').join(','),
    EAGLER_NETPLAY_NAT4_OBSERVERS: observers.join(','),
    EAGLER_NETPLAY_STUN_URLS: stunUrl,
    EAGLER_NETPLAY_TURN_URLS: ordinaryTurnUrl,
    EAGLER_NETPLAY_TURN_CONFIG_FILE: '',
    EAGLER_NETPLAY_TURN_SHARED_SECRET: 'integration-ordinary-credential-secret',
    EAGLER_NETPLAY_SWARM_TURN_URLS: swarmUrl,
    EAGLER_NETPLAY_SWARM_CONTROL_URL: `ws://127.0.0.1:${control.address().port}/core`,
    EAGLER_NETPLAY_SWARM_TURN_SHARED_SECRET: 'integration-swarm-credential-secret',
    SWARM_ACCESS_TOKEN: 'integration-swarm-access-token',
  },
  stdio: ['ignore', 'pipe', 'pipe'],
});
const sockets = [];
let log = '';
for (const stream of [relay.stdout, relay.stderr]) stream.on('data', chunk => { log += chunk; });

async function until(find) {
  const deadline = Date.now() + 5000;
  for (let result; Date.now() < deadline; await delay(10)) if ((result = find())) return result;
  throw new Error('room exchange timed out\n' + log);
}

async function connect(seat, query, address = addresses[seat]) {
  const ws = new WebSocket(`ws://127.0.0.1:${port}/?${query}`, {
    headers: {'x-real-ip': address},
  });
  sockets.push(ws);
  const received = [];
  ws.on('message', data => received.push(JSON.parse(data)));
  await until(() => ws.readyState === WebSocket.OPEN);
  return {
    ws,
    received,
    send: message => ws.send(JSON.stringify(message)),
    state: () => received.findLast(message => ['state', 'start'].includes(message.type))?.room,
  };
}

try {
  await until(() => log.includes('listening'));
  const query = 'room=th09mp-native-routing';
  const generation = 'b'.repeat(64);
  const host = await connect(0, query + '&lobby=client_host&member=private_host');
  const guest = await connect(1, query + '&lobby=client_guest&member=private_guest');
  host.send({type: 'take-seat', seat: 0, loadout: 0});
  guest.send({type: 'take-seat', seat: 1, loadout: 1});
  await until(() => host.state()?.seats[1]);
  assert.equal(host.state().swarmAvailable, true);
  guest.send({type: 'resource-progress', status: 'ready', stage: 'runtime', percent: 100, nativeRuntime: true});
  await until(() => host.state()?.seats[1]?.nativeRuntime);
  assert.equal(host.state().seats[1].ready, false);
  assert.equal(host.state().seats[1].preferSwarm, undefined);
  host.send({type: 'resource-progress', status: 'ready', stage: 'runtime', percent: 100, nativeRuntime: true});
  await until(() => guest.state()?.seats[0]?.preferSwarm);
  host.send({type: 'set-ready', ready: true, nativeRuntime: true, runtimeGeneration: generation});
  guest.send({type: 'set-ready', ready: true, nativeRuntime: true, runtimeGeneration: generation});
  await until(() => host.state()?.seats.slice(0, 2).every(seat => seat?.ready));
  await until(() => host.state()?.swarmWorkers?.length === 2);
  host.send({type: 'relay-preference', swarm: false});
  await until(() => guest.state()?.seats[0]?.preferSwarm === undefined);
  host.send({type: 'relay-preference', swarm: true, worker:12});
  await until(() => guest.state()?.seats[0]?.preferSwarm);
  assert.equal(guest.state().seats[0].ready, true);
  host.send({type: 'start'});
  await until(() => guest.state()?.swarmSelected);
  const signal = query + '&run=1&players=2&signal=1&runtimeGeneration=' + generation;
  const browser = await connect(0, signal + '&player=0&member=private_host');
  const native = await connect(1, signal + '&player=1&member=private_guest');
  for (const client of [browser, native]) {
    const peers = await until(() => client.received.find(message => message.type === 'peers'));
    assert.deepEqual(peers.iceServers.map(server => server.urls), [[stunUrl], [swarmUrl]]);
    assert.equal(peers.turnMode, 'swarm');
    assert.ok(peers.iceServers[1].credential);
    assert.match(peers.iceServers[1].username, /^\d+:swarm-12\//);
  }
  await until(() => browser.received.some(message => message.type === 'peer-join'));
  const offer = 'v=0\r\na=candidate:host 1 UDP 1 10.0.0.3 12345 typ host\r\na=end-of-candidates\r\n';
  browser.send({type: 'signal', to: 1, description: {type: 'offer', sdp: offer}});
  native.send({type: 'signal', to: 0, description: {type: 'answer', sdp: 'v=0\r\n'}});
  const candidate = address => ({sdpMid: '0', candidate: `candidate:42 1 UDP 1 ${address} 34000 typ srflx raddr 0.0.0.0 rport 0`});
  browser.send({type: 'signal', to: 1, candidate: candidate(addresses[0])});
  native.send({type: 'signal', to: 0, candidate: candidate('203.0.113.3')});
  native.send({type: 'signal', to: 0, candidate: candidate(addresses[1])});
  assert.equal((await until(() => native.received.find(message => message.description))).description.sdp,
    offer.replace('a=end-of-candidates\r\n',''));
  for (const [client, address] of [[browser, addresses[1]], [native, addresses[0]]]) {
    const predicted = await until(() => client.received.find(message => message.candidate?.candidate.includes('42next')));
    assert.match(predicted.candidate.candidate, new RegExp(`candidate:42next 1 UDP 1 ${address.replaceAll('.', '\\.')} 34001 typ srflx`));
    await until(() => client.received.some(message => message.candidate?.candidate === candidate(address).candidate));
  }
  const automatic = await connect(0, 'room=th09mp-native-automatic&run=0&players=2&signal=1&player=0');
  const peers = await until(() => automatic.received.find(message => message.type === 'peers'));
  assert.deepEqual(peers.iceServers.map(server => server.urls), [[stunUrl], [ordinaryTurnUrl]]);
  assert.equal(peers.turnMode, undefined);

  const mixed = 'room=th09mp-swarm-direct';
  const desktop = await connect(0, mixed + '&lobby=client_desktop&member=private_desktop');
  const web = await connect(1, mixed + '&lobby=client_web&member=private_web');
  desktop.send({type:'take-seat',seat:0,loadout:0});
  web.send({type:'take-seat',seat:1,loadout:1});
  await until(() => desktop.state()?.seats[1]);
  desktop.send({type:'relay-preference',swarm:true,worker:12});
  desktop.send({type:'relay-preference',swarm:true,worker:0,automatic:true});
  desktop.send({type:'take-seat',seat:0,loadout:1});
  await until(() => web.state()?.seats[0]?.loadout === 1);
  desktop.send({type:'set-ready',ready:true,nativeRuntime:true,swarmDirect:true,runtimeGeneration:generation});
  web.send({type:'set-ready',ready:true,runtimeGeneration:generation});
  await until(() => desktop.state()?.seats.slice(0, 2).every(seat => seat?.ready));
  desktop.send({type:'start'});
  await until(() => web.state()?.swarmSelected);
  await until(() => swarmAllocations === 1);
  const mixedSignal = mixed + '&run=1&players=2&signal=1&runtimeGeneration=' + generation;
  const webSignal = await connect(1, mixedSignal + '&player=1&member=private_web');
  const desktopSignal = await connect(0, mixedSignal + '&player=0&member=private_desktop');
  const desktopPeers = await until(() => desktopSignal.received.find(message => message.type === 'peers'));
  const webPeers = await until(() => webSignal.received.find(message => message.type === 'peers'));
  assert.equal(swarmAllocations, 1, 'signaling reuses the port requested at room start');
  assert.deepEqual(desktopPeers.iceServers, [{urls:[stunUrl]}]);
  assert.deepEqual(webPeers.iceServers.map(server => server.urls), [[stunUrl], [swarmUrl]]);
  assert.equal(desktopPeers.turnMode, 'swarm');
  assert.equal(webPeers.turnMode, 'swarm');
  assert.equal(desktopPeers.nat4Observers, undefined, 'the configured mapping does not use STUN observers');
  assert.equal(desktopPeers.swarmGrant.address, '198.51.100.2:35000');
  assert.equal(desktopPeers.swarmGrant.code, 0);
  assert.ok(desktopPeers.swarmGrant.credential);
  assert.equal(webPeers.swarmGrant, undefined);
  desktopSignal.send({type:'signal',to:1,description:{type:'offer',sdp:'v=0\r\n'}});
  webSignal.send({type:'signal',to:0,description:{type:'answer',sdp:'v=0\r\n'}});
  desktopSignal.send({type:'signal',to:1,candidate:candidate(addresses[0])});
  webSignal.send({type:'signal',to:0,candidate:candidate(addresses[1])});
  await until(() => webSignal.received.some(message => message.candidate?.candidate.includes('42next')));
  desktopSignal.send({type:'rtc-ready',rtcReplace:true,swarmDirect:true});
  webSignal.send({type:'rtc-ready',rtcReplace:true});
  await until(() => webSignal.received.some(message => message.type === 'route' && message.mode === 'rtc'));
  assert.ok(desktopSignal.received.some(message => message.type === 'rtc-replace-capable'));
  webSignal.send({type:'rtc-replace-request',generation:0});
  for (const peer of [desktopSignal,webSignal]) {
    await until(() => peer.received.some(message => message.type === 'rtc-replacing' && message.generation === 1));
    await until(() => peer.received.some(message => message.type === 'rtc-replace' && message.generation === 1));
  }
  const newGrant = desktopSignal.received.find(message => message.type === 'rtc-replace').swarmGrant;
  assert.ok(newGrant && newGrant.credential !== desktopPeers.swarmGrant.credential);
  assert.equal(webSignal.received.find(message => message.type === 'rtc-replace').swarmGrant, undefined);
  desktopSignal.send({type:'signal',to:1,generation:0,description:{type:'offer',sdp:'v=0\r\na=stale\r\n'}});
  desktopSignal.send({type:'signal',to:1,generation:1,description:{type:'offer',sdp:'v=0\r\n'}});
  webSignal.send({type:'signal',to:0,generation:1,description:{type:'answer',sdp:'v=0\r\n'}});
  desktopSignal.send({type:'signal',to:1,generation:1,candidate:candidate(addresses[0])});
  webSignal.send({type:'signal',to:0,generation:1,candidate:candidate(addresses[1])});
  await until(() => webSignal.received.some(message => message.generation === 1 && message.candidate?.candidate.includes('42next')));
  assert.ok(!webSignal.received.some(message => message.description?.sdp.includes('a=stale')));
  desktopSignal.send({type:'rtc-ready',generation:1});
  webSignal.send({type:'rtc-ready',generation:0});
  await delay(50);
  assert.ok(!desktopSignal.received.some(message => message.type === 'rtc-replaced'));
  webSignal.send({type:'rtc-ready',generation:1});
  await until(() => desktopSignal.received.some(message => message.type === 'rtc-replaced' && message.generation === 1));

  // A simultaneous network interruption can drop both signaling sockets
  // before either peer detects that its UDP path also stopped responding.
  desktopSignal.ws.terminate();
  webSignal.ws.terminate();
  await until(() => log.includes('SIGNAL LEAVE room=th09mp-swarm-direct run=1 player=0') &&
    log.includes('SIGNAL LEAVE room=th09mp-swarm-direct run=1 player=1'));
  const desktopReconnected = await connect(0, mixedSignal + '&player=0&member=private_desktop');
  const retained = await until(() => desktopReconnected.received.find(message => message.type === 'peers'));
  assert.equal(retained.route, 'rtc');
  assert.equal(retained.rtcReplace, true);
  desktopReconnected.send({type:'rtc-replace-request',generation:1});
  await delay(40);
  assert.ok(!desktopReconnected.received.some(message => message.type === 'rtc-replacing'));
  const webReconnected = await connect(1, mixedSignal + '&player=1&member=private_web');
  for (const peer of [desktopReconnected, webReconnected])
    await until(() => peer.received.some(message => message.type === 'rtc-replace' && message.generation === 2));
  assert.ok(desktopReconnected.received.find(message => message.type === 'rtc-replace').swarmGrant);
  desktopReconnected.send({type:'rtc-ready',generation:2});
  webReconnected.send({type:'rtc-ready',generation:2});
  await until(() => desktopReconnected.received.some(message => message.type === 'rtc-replaced' && message.generation === 2));
  desktopReconnected.ws.terminate();
  webReconnected.ws.terminate();
  await until(() => desktop.state()?.phase === 'lobby' && web.state()?.phase === 'lobby');
  assert.equal(web.state().seats[1].ready, false);

  refuseSwarmGrant = true;
  desktop.send({type:'set-ready',ready:true,nativeRuntime:true,swarmDirect:true,runtimeGeneration:generation});
  web.send({type:'set-ready',ready:true,runtimeGeneration:generation});
  await until(() => desktop.state()?.seats.slice(0, 2).every(seat => seat?.ready));
  desktop.send({type:'start'});
  await until(() => web.received.some(message => message.type === 'start' && message.serial === 2));
  const retrySignal = mixed + '&run=2&players=2&signal=1&runtimeGeneration=' + generation;
  const retryDesktop = await connect(0, retrySignal + '&player=0&member=private_desktop');
  const retryWeb = await connect(1, retrySignal + '&player=1&member=private_web');
  for (const peer of [retryDesktop, retryWeb]) {
    const retryPeers = await until(() => peer.received.find(message => message.type === 'peers'));
    assert.equal(retryPeers.swarmGrant, undefined);
    assert.equal(retryPeers.turnMode, 'swarm');
    assert.deepEqual(retryPeers.iceServers.map(server => server.urls), [[stunUrl], [swarmUrl]]);
    peer.send({type:'rtc-ready'});
  }
  await until(() => retryWeb.received.some(message => message.type === 'route' && message.mode === 'rtc'));
  const priorSwarmAllocations = swarmAllocations;

  retryDesktop.ws.terminate();
  retryWeb.ws.terminate();
  await until(() => desktop.state()?.phase === 'lobby');
  desktop.send({type:'relay-preference',swarm:true});
  desktop.send({type:'set-ready',ready:true,nativeRuntime:true,swarmDirect:true,runtimeGeneration:generation});
  web.send({type:'set-ready',ready:true,runtimeGeneration:generation});
  await until(() => desktop.state()?.seats.slice(0,2).every(seat => seat?.ready));
  for (const [lane, quality] of [
    ['swarm-0',{sent:12,received:0,p95:null}],
    ['swarm-12',{sent:12,received:12,p95:205}],
  ]) {
    const token = 'fallback-'+lane;
    desktop.send({type:'room-probe-report',to:'client_web',lane,token,quality});
    web.send({type:'room-probe-report',to:'client_desktop',lane,token,quality});
    for (const peer of [desktop,web])
      await until(() => peer.received.some(message => message.type === 'room-probe-report' && message.token === token));
  }
  desktop.send({type:'start'});
  const fallback = await until(() => web.received.find(message => message.type === 'start' && message.serial === 3));
  assert.equal(fallback.room.swarmSelected, undefined);
  const fallbackSignal = mixed + '&run=3&players=2&signal=1&runtimeGeneration=' + generation;
  const fallbackDesktop = await connect(0, fallbackSignal + '&player=0&member=private_desktop');
  const fallbackWeb = await connect(1, fallbackSignal + '&player=1&member=private_web');
  for (const peer of [fallbackDesktop,fallbackWeb]) {
    const firstPartyPeers = await until(() => peer.received.find(message => message.type === 'peers'));
    assert.equal(firstPartyPeers.turnMode, 'first-party');
    assert.equal(firstPartyPeers.swarmGrant, undefined);
    assert.deepEqual(firstPartyPeers.iceServers.map(server => server.urls), [[stunUrl], [ordinaryTurnUrl]]);
  }
  assert.equal(swarmAllocations, priorSwarmAllocations);
  fallbackDesktop.ws.terminate();
  fallbackWeb.ws.terminate();
  await until(() => desktop.state()?.phase === 'lobby');
  for (const [mode0,mode1,turnMode,turn,relayOnly] of [
    ['auto','turn','first-party',ordinaryTurnUrl,true],
    ['direct','turn','direct',undefined,false],
    ['auto','swarm','swarm',swarmUrl,true],
  ]) {
    const serial = desktop.state().startSerial + 1;
    desktop.send({type:'set-ready',ready:true,nativeRuntime:true,runtimeGeneration:generation});
    web.send({type:'set-ready',ready:true,runtimeGeneration:generation});
    await until(() => desktop.state()?.seats.slice(0,2).every(seat => seat?.ready));
    desktop.send({type:'start'});
    await until(() => web.received.some(message => message.type === 'start' && message.serial === serial));
    const join = mixed + '&run=' + serial + '&players=2&signal=1&rtcStartup=1&runtimeGeneration=' + generation;
    const chosen = await Promise.all([
      connect(0, join + '&player=0&member=private_desktop&rtcRoute=' + mode0),
      connect(1, join + '&player=1&member=private_web&rtcRoute=' + mode1),
    ]);
    for (const peer of chosen) {
      const peers = await until(() => peer.received.find(message => message.type === 'peers'));
      assert.equal(peers.turnMode, turnMode);
      assert.equal(peers.initialIceRelayOnly === true, relayOnly);
      assert.deepEqual(peers.iceServers.map(server => server.urls), turn ? [[turn]] : [[stunUrl]]);
      assert.ok(peers.fallbackIceServers.flatMap(server => server.urls).includes(ordinaryTurnUrl));
      assert.equal(peer.received.some(message => message.type === 'rtc-startup'), false);
      peer.send({type:'rtc-ready'});
    }
    await until(() => chosen[1].received.some(message => message.type === 'route' && message.mode === 'rtc'));
    for (const peer of chosen) peer.ws.terminate();
    await until(() => desktop.state()?.phase === 'lobby');
  }

  const shared = 'room=th07mp-route-choice';
  const sharedAddresses = ['192.0.2.17','198.51.100.18'];
  const sharedSeats = await Promise.all([0, 1].map(seat =>
    connect(seat, shared + '&lobby=route_seat_' + seat + '&member=route_member_' + seat, sharedAddresses[seat])));
  await until(() => sharedSeats[0].state()?.startupRoutes?.includes('turn'));
  for (const [seat, connection] of sharedSeats.entries()) connection.send({type:'take-seat', seat, loadout:0});
  await until(() => sharedSeats[0].state()?.seats.slice(0, 2).every(Boolean));
  sharedSeats[1].send({type:'startup-route', route:'turn'});
  await until(() => sharedSeats[0].state()?.seats[1]?.startupRoute === 'turn');
  for (const connection of sharedSeats) connection.send({type:'set-ready', ready:true, runtimeGeneration:generation});
  await until(() => sharedSeats[0].state()?.seats.slice(0, 2).every(seat => seat?.ready));
  sharedSeats[0].send({type:'start'});
  await until(() => sharedSeats[1].received.some(message => message.type === 'start'));
  const sharedSignals = await Promise.all([0, 1].map(seat => connect(seat,
    shared + '&run=1&players=2&signal=1&player=' + seat + '&member=route_member_' + seat + '&runtimeGeneration=' + generation,
    sharedAddresses[seat])));
  for (const connection of sharedSignals) {
    const peers = await until(() => connection.received.find(message => message.type === 'peers'));
    assert.equal(peers.turnMode, 'first-party');
    assert.deepEqual(peers.iceServers.map(ice => ice.urls), [[ordinaryTurnUrl]]);
  }
  const hostCandidate = 'candidate:host 1 UDP 2122252543 192.0.2.1 34000 typ host';
  const turnCandidate = 'candidate:turn 1 UDP 1677734911 192.0.2.1 34001 typ relay raddr 0.0.0.0 rport 0';
  sharedSignals[0].send({type:'signal', to:1, description:{type:'offer', sdp:'v=0\r\na=' + hostCandidate + '\r\na=' + turnCandidate + '\r\n'}});
  for (const candidate of [hostCandidate, turnCandidate]) sharedSignals[0].send({type:'signal', to:1, candidate:{candidate, sdpMid:'0'}});
  await until(() => sharedSignals[1].received.some(message => message.candidate?.candidate === turnCandidate));
  assert.equal(sharedSignals[1].received.find(message => message.description)?.description.sdp, 'v=0\r\na=' + turnCandidate + '\r\n');
  assert.ok(!sharedSignals[1].received.some(message => message.candidate?.candidate === hostCandidate));
  for (const {ws} of [...sharedSignals, ...sharedSeats]) ws.terminate();

  const abandoned = 'room=th09mp-abandoned';
  const abandonedHost = await connect(0, abandoned + '&lobby=abandoned_host&member=abandoned_host_key');
  const abandonedGuest = await connect(1, abandoned + '&lobby=abandoned_guest&member=abandoned_guest_key');
  abandonedHost.send({type:'take-seat',seat:0,loadout:0});
  abandonedGuest.send({type:'take-seat',seat:1,loadout:1});
  await until(() => abandonedHost.state()?.seats[1]);
  abandonedHost.send({type:'set-ready',ready:true,runtimeGeneration:generation});
  abandonedGuest.send({type:'set-ready',ready:true,runtimeGeneration:generation});
  await until(() => abandonedHost.state()?.seats.slice(0,2).every(seat => seat?.ready));
  abandonedHost.send({type:'start'});
  await until(() => abandonedGuest.received.some(message => message.type === 'start'));
  for (const {ws} of [abandonedHost, abandonedGuest]) {
    await new Promise(resolve => {ws.once('close', resolve); ws.close(1000,'leave room');});
  }
  const replacement = await connect(0, 'room=th09mp-replacement&lobby=replacement_host&member=replacement_key');
  await until(() => replacement.state());
} finally {
  for (const ws of sockets) ws.terminate();
  relay.kill();
  for (const ws of control.clients) ws.terminate();
  control.close();
}
await partialResult;
