import assert from 'node:assert/strict';
import {createAdaptivePeerRoute} from '../browser/adaptive-peer-route.mjs';

// Deterministic network, not a mock of the policy: both endpoints execute the
// production route manager, its real handshake, probes and control envelopes.
function fixture(config = {}) {
    let time = 1, id = 0, nextPc = 0;
    const tasks = new Map(), pcs = new Map(), deliveries = [[], []], selected = [[], []], errors = [];
    const reports = [[], []];
    let blockNetwork = false, dropTurn = false;
    let dropPrimary = false, dropStandbyPongs = false, holdControl = false, visible = [true, true], running = true;
    let dropCommit = false, droppedCommit = 0;
    const held = [];
    const later = (fn, ms) => { tasks.set(++id, {fn, at: time + ms}); return id; };
    const cancel = id => tasks.delete(id);
    class Channel {
        readyState = 'connecting'; bufferedAmount = 0;
        constructor(label, path, seat) { this.label = label; this.path = path; this.seat = seat; }
        send(data) {
            if (this.readyState !== 'open') throw Error('closed');
            if (blockNetwork || dropTurn && this.path === 'turn') return;
            const payload = typeof data === 'string' ? data : data.slice().buffer;
            if (dropCommit && typeof data === 'string' && data.startsWith('etr1:') &&
                JSON.parse(data.slice(5)).kind === 'commit' && droppedCommit++ === 0) return;
            if (this.path === 'primary' && dropPrimary && this.label === 'input') return;
            if (this.path === 'turn' && dropStandbyPongs && this.seat === 0 &&
                typeof data === 'string' && JSON.parse(data.slice(5)).kind === 'pong') return;
            const deliver = () => { if (this.other.readyState === 'open') this.other.onmessage?.({data: payload}); };
            if (this.path === 'primary' && holdControl && typeof data !== 'string' && this.label === 'control') held.push(deliver);
            else later(deliver, this.path === 'primary' ? (config.primaryDelay ?? 20) : (config.turnDelay ?? 20));
        }
        close() { this.readyState = 'closed'; this.onclose?.(); }
    }
    function channels(label, path) {
        const a = new Channel(label, path, 0), b = new Channel(label, path, 1);
        a.other = b; b.other = a;
        return [a, b];
    }
    class Peer {
        channels = []; connectionState = 'new';
        constructor(config) { this.config = config; this.id = ++nextPc; pcs.set(this.id, this); }
        createDataChannel(label) {
            const dc = new Channel(label, this.config.iceTransportPolicy === 'relay' ? 'turn' : 'direct', 0);
            this.channels.push(dc);
            return dc;
        }
        async createOffer() { return {type: 'offer', sdp: String(this.id)}; }
        async createAnswer() { return {type: 'answer', sdp: String(this.id)}; }
        async setLocalDescription(description) { this.localDescription = description; }
        async setRemoteDescription(description) {
            this.remoteDescription = description;
            if (description.type !== 'offer') return;
            const other = pcs.get(Number(description.sdp));
            for (const a of other.channels) {
                const b = new Channel(a.label, a.path, 1); a.other = b; b.other = a;
                this.channels.push(b); this.ondatachannel({channel: b});
                later(() => {
                    a.readyState = b.readyState = 'open';
                    this.connectionState = other.connectionState = 'connected';
                    a.onopen?.(); b.onopen?.();
                }, 60);
            }
        }
        async addIceCandidate() {}
        close() { this.closed = true; for (const dc of this.channels) dc.close(); }
    }
    const [ca, cb] = channels('control', 'primary'), [ia, ib] = channels('input', 'primary');
    const paths = [
        {pc: {}, controlDc: ca, inputDc: ia, path: 'direct'},
        {pc: {}, controlDc: cb, inputDc: ib, path: 'direct'},
    ];
    const routes = paths.map((primary, seat) => createAdaptivePeerRoute({
        primary, localPlayer: seat, peerId: 1 - seat, tag: 'test', enabled: config.enabled?.[seat] ?? true,
        directRetry: config.directRetry?.[seat],
        manualSwitch: config.manualSwitch === true,
        surveyEnabled: () => config.survey === true,
        surveyPending: () => config.surveyPending === true,
        initialRelayRoute: () => 'swarm-0',
        surveySignal: message => {
            if (message.type === 'rtc-standby-signal') later(() => routes[1-seat].relayStandby(message), 15);
            else reports[seat].push(message);
            return true;
        },
        iceServers: () => [{urls: 'stun:fixture:3478'}, {urls: 'turn:fixture:3478', username: 'test', credential: 'test'}],
        alive: () => true, running: () => running, visible: () => visible[seat],
        now: () => time, later, cancel, PeerConnection: Peer,
        receive: bytes => deliveries[seat].push(Array.from(new Uint8Array(bytes.buffer || bytes, bytes.byteOffset || 0, bytes.byteLength))),
        selected: path => selected[seat].push(path === primary ? 'primary' : path.path),
        failed: message => errors.push(message),
    }));
    paths.forEach((path, seat) => {
        if (config.primaryPath) path.path = config.primaryPath;
        for (const kind of ['input', 'control']) {
            const dc = path[kind + 'Dc'];
            dc.onmessage = event => routes[seat].receivePrimary(kind, event.data);
            if (!config.startup) dc.readyState = 'open';
        }
        if (config.startup) routes[seat].beginStartup();
        else {routes[seat].opened('control'); routes[seat].start();}
    });
    const flush = async () => { for (let i = 0; i < 20; i++) await Promise.resolve(); };
    return {
        routes, paths, deliveries, selected, errors, tasks, pcs, later, reports,
        openPrimary() {
            for (const path of paths) path.controlDc.readyState = path.inputDc.readyState = 'open';
        },
        block(value = true) { blockNetwork = value; },
        drop(value = true) { dropPrimary = value; },
        dropStandbyPongs(value = true) { dropStandbyPongs = value; },
        dropFirstCommit() { dropCommit = true; },
        hold(value = true) { holdControl = value; },
        release() { for (const fn of held.splice(0)) fn(); },
        hide(seat, value = true) { visible[seat] = !value; },
        stopGame() { running = false; },
        async advance(ms) {
            const end = time + ms;
            await flush();
            while (true) {
                const item = [...tasks.entries()].filter(([, task]) => task.at <= end).sort((a, b) => a[1].at - b[1].at)[0];
                if (!item) break;
                time = item[1].at; tasks.delete(item[0]); item[1].fn(); await flush();
            }
            time = end; await flush();
        },
        jump(ms) { time += ms; },
        dropTurn(value) { dropTurn = value; },
        close() { for (const route of routes) route.close(); },
    };
}

for (const choice of ['confirm','outage','expire']) {
    const f = fixture({startup:true, manualSwitch:true, primaryDelay:8, turnDelay:50});
    await Promise.all(f.routes.map(route=>route.relayStandby({type:'rtc-standby',serial:1,phase:'measure',route:'turn',
        iceServers:[{urls:'turn:fixture:3478'}]})));
    await f.advance(1000);
    assert(f.routes.every(route=>route.selectStartup(1,true)));
    assert(f.paths.every(path=>!path.closed));
    if (choice==='expire') {
        await f.advance(12300);
        assert(f.paths.every(path=>path.closed));
        assert.deepEqual(f.selected,[['turn'],['turn']]);
    } else {
        await f.advance(2800);
        f.openPrimary();
        await f.advance(4900);
        const expected={from:'TURN',to:'直连',fromMs:100,toMs:16,switching:false};
        assert.deepEqual(f.routes.map(route=>route.routeSwitchStatus()),[expected,expected]);
        assert.deepEqual(f.selected,[['turn'],['turn']]);
        assert(f.routes[0].send(new Uint8Array([40]),true));
        await f.advance(200);
        assert.deepEqual(f.deliveries[1],[[40]]);
        if (choice==='confirm') {
            assert(f.routes[1].confirmOffer());
        } else {
            f.dropTurn(true);
            await f.advance(650);
            assert.deepEqual(f.selected,[['turn'],['turn']]);
            f.dropTurn(false);
            await f.advance(500);
            assert.deepEqual(f.selected,[['turn'],['turn']]);
            f.dropTurn(true);
            await f.advance(1450);
        }
        await f.advance(200);
        assert.deepEqual(f.selected,[['turn','primary'],['turn','primary']]);
        assert(f.routes[0].send(new Uint8Array([41]),true));
        await f.advance(200);
        assert.deepEqual(f.deliveries[1],[[40],[41]]);
    }
    assert.deepEqual(f.errors,[]);
    f.close();
}

for (const choice of [0, 1, 'outage']) {
    const f = fixture({primaryPath:'turn',primaryDelay:105,directRetry:[false,false],survey:true,manualSwitch:true});
    await f.advance(2700);
    await Promise.all(f.routes.map(route=>route.relayStandby({type:'rtc-standby',serial:3,phase:'select',route:'turn',
        iceServers:[{urls:'turn:fixture:3478'}]})));
    await f.advance(4200);
    const expected={from:'Swarm',to:'TURN',fromMs:210,toMs:40,switching:false};
    assert.deepEqual(f.routes.map(route=>route.routeSwitchStatus()),[expected,expected]);
    assert.deepEqual(f.selected,[[],[]], 'quality alone leaves both players on the existing connection');
    assert(f.routes[0].send(new Uint8Array([88]),true));
    await f.advance(500);
    assert.deepEqual(f.deliveries[1],[[88]], 'the match advances while the decision is pending');
    if (choice==='outage') {
        f.drop();
        assert(f.routes[1].recover());
    } else assert(f.routes[choice].confirmOffer());
    await f.advance(200);
    assert.deepEqual(f.selected,[['turn'],['turn']], 'a confirmed or emergency switch uses the already prepared path');
    assert.deepEqual(f.routes.map(route=>route.routeSwitchStatus()),[null,null]);
    f.close();
}

{
    const f = fixture({primaryPath:'turn',primaryDelay:100,directRetry:[false,false],survey:true});
    const iceServers = [{urls:'turn:fixture:3478'}];
    const standby = (serial,phase,route) => Promise.all(f.routes.map(r=>r.relayStandby({type:'rtc-standby',serial,phase,route,iceServers})));
    await f.advance(4500);
    assert(f.reports.every(reports=>reports.some(event=>event.type==='rtc-route-slow'&&event.route==='swarm-0')));
    await standby(1,'measure','swarm-12');
    assert(f.routes[0].send(new Uint8Array([70]),true));
    await f.advance(2800);
    assert(f.reports.every(reports=>reports.some(event=>event.type==='rtc-route-report'&&event.serial===1&&event.quality.p95===40)));
    assert.deepEqual(f.deliveries[1],[[70]]);
    assert.deepEqual(f.selected,[[],[]]);
    await standby(2,'select','swarm-12');
    await f.advance(6500);
    assert.deepEqual(f.selected,[['swarm-12'],['swarm-12']]);
    assert(f.routes[0].send(new Uint8Array([71]),true));
    await f.advance(200);
    assert.deepEqual(f.deliveries[1],[[70],[71]]);
    assert.equal(f.errors.length,0);
    f.close();
}

{
    const config = {primaryPath:'turn',directRetry:[false,false],survey:true,surveyPending:true};
    const f = fixture(config);
    await f.advance(300);
    f.drop();
    assert(!f.routes[1].snapshot().standbyReady);
    assert(f.routes[1].recover(), 'a Relay offer already received still counts while standby signaling is queued');
    await f.advance(3000);
    config.surveyPending = false;
    await Promise.all(f.routes.map(route => route.relayStandby({
        type:'rtc-standby',serial:1,phase:'select',route:'turn',iceServers:[{urls:'turn:fixture:3478'}],
    })));
    await f.advance(5000);
    assert.deepEqual(f.selected,[['turn'],['turn']]);
    assert.deepEqual(f.errors,[]);
    f.close();
}

{
    const f = fixture(); await f.advance(50);
    assert(f.routes[0].snapshot().supported);
    assert(!f.routes[0].snapshot().standbyReady, 'a negotiated peer need not have a ready standby');
    const dc = f.paths[0].controlDc, send = dc.send.bind(dc);
    let refused = false;
    dc.send = data => {
        if (typeof data !== 'string' && !refused) { refused = true; throw Error('temporary SCTP send failure'); }
        send(data);
    };
    assert(f.routes[0].send(new Uint8Array([41]), true), 'the reliable queue accepts the refused packet');
    assert.equal(f.routes[0].send(new Uint8Array([42]), true), false, 'later controls wait behind the unsent sequence');
    await f.advance(1000);
    assert.deepEqual(f.deliveries[1], [[41]], 'timer retry delivers over the same healthy primary path');
    assert(f.routes[0].send(new Uint8Array([42]), true));
    await f.advance(12000);
    assert.deepEqual(f.deliveries[1], [[41], [42]], 'the receiver has no permanent reliable-sequence gap');
    assert.equal(f.routes[0].snapshot().pendingControl, 0);
    assert.equal(f.routes[0].snapshot().selected, 'primary');
    assert.equal(f.errors.length, 0);
    f.close();
}
{
    const f = fixture(); await f.advance(6000);
    f.dropStandbyPongs(); f.drop(); await f.advance(10000);
    assert.deepEqual(f.selected, [[], []], 'one healthy measurement cannot hide the other peer’s standby loss');
    f.dropStandbyPongs(false); await f.advance(20000);
    assert.deepEqual(f.selected, [['turn'], ['turn']], 'both peers approve the same standby after the loss clears');
    assert.equal(f.errors.length, 0);
    f.close();
}
{
    const f = fixture(); await f.advance(6000); f.hold();
    for (const pc of f.pcs.values()) for (const dc of pc.channels) if (dc.label.endsWith('control')) {
        const send = dc.send.bind(dc);
        dc.send = data => {
            send(data);
            if (typeof data !== 'string') {
                dc.bufferedAmount += data.byteLength;
                const bytes = data.byteLength;
                // Timer-driven browser drain: replay must continue after the
                // first batch reaches the SCTP send buffer limit.
                const wait = () => { dc.bufferedAmount = Math.max(0, dc.bufferedAmount - bytes); };
                f.later(wait, 100);
            }
        };
    }
    for (let i = 0; i < 100; i++) {
        const bytes = new Uint8Array(4096); bytes[0] = i;
        assert(f.routes[0].send(bytes, true));
    }
    f.drop(); await f.advance(20000);
    assert.equal(f.deliveries[1].length, 100, 'replay continues after temporary new-path backpressure');
    assert.deepEqual(f.deliveries[1].map(p => p[0]), Array.from({length: 100}, (_, i) => i));
    assert.equal(f.routes[0].snapshot().pendingControl, 0);
    f.close();
}
{
    const f = fixture(); await f.advance(6000);
    f.dropFirstCommit(); f.drop();
    for (let i = 0; i < 1000 && f.routes[0].snapshot().selected !== 'turn'; i++) await f.advance(10);
    assert.equal(f.routes[0].snapshot().selected, 'turn');
    assert.equal(f.routes[1].snapshot().selected, 'primary');
    f.hide(0); await f.advance(5000); f.hide(0, false); await f.advance(4000);
    assert(f.routes.every(route => route.snapshot().selected === 'turn'), 'accepted epoch survives a paused controller and delayed commit');
    assert.equal(f.errors.length, 0);
    f.close();
}
{
    const f = fixture(); await f.advance(6000);
    f.dropFirstCommit(); f.drop(); await f.advance(10000);
    assert.deepEqual(f.selected, [['turn'], ['turn']], 'commit retry completes the same epoch on both ends');
    assert(f.routes.every(route => route.snapshot().epoch === 1));
    assert.equal(f.errors.length, 0);
    f.close();
}
{
    const f = fixture(); await f.advance(6000);
    [...f.pcs.values()][0].close(); await f.advance(200);
    assert(f.routes.every(route => !route.snapshot().standbyReady), 'failed standby is retired on both endpoints');
    assert.equal(f.errors.length, 0, 'standby failure cannot end the healthy game');
    await f.advance(65000);
    assert(f.routes.every(route => route.snapshot().standbyReady), 'a fresh standby generation can be negotiated after backoff');
    assert.equal(f.pcs.size, 4, 'one replacement, not a connection creation loop');
    f.close();
}
{
    const f = fixture({enabled: [true, false]}); await f.advance(6000);
    assert.equal(f.pcs.size, 0, 'an endpoint without route support retains the original protocol');
    f.routes[0].send(new Uint8Array([7, 8]), true); await f.advance(50);
    assert.deepEqual(f.deliveries[1], [[7, 8]], 'legacy control bytes are unchanged');
    assert.equal(f.routes[0].snapshot().pendingControl, 0);
    f.close();
}
{
    const f = fixture({primaryPath: 'turn', directRetry: [true, false]}); await f.advance(6000);
    assert.equal(f.pcs.size, 0, 'an older peer can continue to use the original TURN route');
    f.close();
}
{
    const f = fixture({primaryPath: 'turn', primaryDelay: 110});
    f.routes[0].send(new Uint8Array([8]), true);
    await f.advance(350);
    assert.deepEqual(f.deliveries[1], [[8]], 'the first relay carries gameplay while direct gathers');
    assert(f.routes.every(route => route.snapshot().selected === 'primary'));
    await f.advance(10000);
    assert.equal(f.pcs.size, 2);
    for (const pc of f.pcs.values()) {
        assert.equal(pc.config.iceTransportPolicy, 'all');
        assert.deepEqual(pc.config.iceServers, [{urls: ['stun:fixture:3478']}]);
    }
    assert.deepEqual(f.selected, [['direct'], ['direct']], 'both peers switch to verified direct');
    f.routes[0].send(new Uint8Array([9]), true);
    await f.advance(1000);
    assert.deepEqual(f.deliveries[1], [[8], [9]], 'reliable controls span the handoff');
    assert.equal(f.errors.length, 0);
    f.close();
}
{
    const f = fixture(); await f.advance(6000);
    f.drop(); await f.advance(350); f.drop(false); await f.advance(7000);
    assert.deepEqual(f.selected, [[], []], 'a brief disturbance is not enough to change paths');
    f.close();
}
{
    const f = fixture(); await f.advance(6000);
    assert(f.routes.every(route => route.snapshot().standbyReady));
    assert.equal(f.pcs.size, 2, 'exactly one relay connection per endpoint');
    assert(f.routes.every(route => route.snapshot().selected === 'primary'), 'healthy direct route stays selected');
    assert(f.routes[0].send(new Uint8Array([1, 2, 3]), false));
    await f.advance(100);
    assert.deepEqual(f.deliveries[1], [[1, 2, 3]]);
    f.hold();
    f.routes[0].send(new Uint8Array([10]), true);
    await f.advance(100);
    assert.equal(f.routes[0].snapshot().pendingControl, 1);
    f.drop(); await f.advance(10000);
    assert.deepEqual(f.selected, [['turn'], ['turn']], 'sustained loss moves both peers to the verified relay');
    assert.equal(f.errors.length, 0);
    f.routes[0].send(new Uint8Array([11]), true);
    f.routes[0].send(new Uint8Array([9]), false);
    await f.advance(200);
    assert.deepEqual(f.deliveries[1], [[1, 2, 3], [10], [11], [9]], 'unconfirmed control is replayed before new control, input continues');
    f.release(); await f.advance(100);
    assert.deepEqual(f.deliveries[1], [[1, 2, 3], [10], [11], [9]], 'late control from the old path is deduplicated');
    assert.equal(f.routes[0].snapshot().pendingControl, 0);
    f.drop(false); await f.advance(20000);
    assert.equal(f.selected[0].length, 1, 'cooldown prevents bouncing back');
    f.close(); await f.advance(200);
    assert.equal(f.tasks.size, 0);
    assert([...f.pcs.values()].every(pc => pc.closed));
}
{
    const f = fixture(); await f.advance(6000);
    f.hide(1); f.drop(); await f.advance(8000);
    assert.equal(f.selected[0].length, 0, 'remote browser suspension is not a network loss diagnosis');
    f.hide(1, false); f.drop(false); await f.advance(6000);
    assert.equal(f.selected[0].length, 0);
    f.close();
}
{
    const f = fixture(); await f.advance(6000);
    f.paths[0].inputDc.close(); f.drop();
    assert(f.routes[0].recover());
    await f.advance(6500);
    assert.equal(f.routes[0].snapshot().selected, 'turn', 'ready standby can replace a closed input channel');
    assert.equal(f.errors.length, 0);
    f.close();
}
{
    const f = fixture(); await f.advance(6000);
    f.hold();
    for (let i = 0; i < 256; i++) assert(f.routes[0].send(new Uint8Array(1), true));
    assert.equal(f.routes[0].send(new Uint8Array(1), true), false, 'unacknowledged reliable data is bounded');
    assert.equal(f.routes[0].snapshot().pendingControl, 256);
    f.close(); assert.equal(f.routes[0].snapshot().pendingBytes, 0);
}
{
    const f = fixture(); await f.advance(6000);
    assert(f.routes[0].recover());
    await f.advance(9000);
    assert.equal(f.routes[0].recovering(), false, 'fresh active round trips finish recovery without switching paths');
    assert.equal(f.routes[0].snapshot().selected, 'primary');
    assert.deepEqual(f.errors, [], 'a completed recovery must not retain a fatal deadline');
    f.close();
}
{
    const f = fixture(); await f.advance(6000);
    const dc = f.paths[0].controlDc, send = dc.send.bind(dc);
    let refused = false;
    dc.send = data => {
        if (typeof data !== 'string' && !refused) { refused = true; throw Error('temporary SCTP send failure'); }
        send(data);
    };
    assert(f.routes[0].send(new Uint8Array([81]), true));
    assert(f.routes[0].recovering(), 'a ready standby arms recovery after the refused send');
    await f.advance(9000);
    assert.deepEqual(f.deliveries[1], [[81]]);
    assert.equal(f.routes[0].snapshot().pendingControl, 0);
    assert.equal(f.routes[0].recovering(), false);
    assert.deepEqual(f.errors, [], 'successful retry and acknowledgement retire recovery');
    f.close();
}
{
    const f = fixture(); await f.advance(6000);
    f.paths[0].inputDc.close(); f.drop();
    assert(f.routes[0].recover());
    f.hide(0); await f.advance(10000); f.hide(0, false);
    await f.advance(9000);
    assert(f.routes.every(route => route.snapshot().selected === 'turn'), 'recovery has a fresh foreground budget after browser suspension');
    assert.deepEqual(f.errors, []);
    f.close();
}
{
    const f = fixture(); await f.advance(6000); f.hold();
    assert(f.routes[0].send(new Uint8Array([82]), true));
    assert(f.routes[0].recover()); await f.advance(8500);
    assert(f.errors.includes('RTC route recovery exhausted'), 'healthy input probes cannot hide an unacknowledged reliable control');
    f.close();
}
{
    const f = fixture(); await f.advance(6000); f.block();
    assert(f.routes[0].recover()); await f.advance(8500);
    assert(f.errors.includes('RTC route recovery exhausted'), 'open channel state alone cannot hide a real outage');
    f.close();
}
{
    const f = fixture(); await f.advance(6000);
    assert(f.routes[0].recover());
    [...f.pcs.values()][0].close();
    await f.advance(9000);
    assert(!f.routes[0].snapshot().standbyReady);
    assert.equal(f.routes[0].snapshot().selected, 'primary');
    assert.equal(f.routes[0].recovering(), false);
    assert.deepEqual(f.errors, [], 'retiring a failed standby cannot kill the recovered primary');
    f.close();
}
{
    const f = fixture(); await f.advance(6000);
    // A pending sample sent before recover() is not evidence of recovery.
    const path = f.paths[0], dc = path.inputDc, send = dc.send.bind(dc);
    let pending;
    dc.send = data => {
        if (typeof data === 'string' && data.startsWith('etr1:') && JSON.parse(data.slice(5)).kind === 'ping') {
            pending = JSON.parse(data.slice(5)); return;
        }
        send(data);
    };
    await f.advance(300); assert(pending);
    assert(f.routes[0].recover());
    f.routes[0].receivePrimary('input', 'etr1:' + JSON.stringify({kind:'pong',id:pending.id}));
    assert(f.routes[0].recovering(), 'a late pre-failure pong cannot cancel the deadline');
    f.block(); await f.advance(8500);
    assert(f.errors.includes('RTC route recovery exhausted'));
    f.close();
}
{
    const f = fixture(); await f.advance(6000); f.hold();
    assert(f.routes[0].send(new Uint8Array([83]), true));
    assert(f.routes[0].recover()); await f.advance(500);
    [...f.pcs.values()][0].close(); f.drop(); await f.advance(1500);
    f.release(); await f.advance(100);
    assert.equal(f.routes[0].snapshot().pendingControl, 0);
    assert(f.routes[0].recovering(), 'a delayed control acknowledgement cannot reuse stale input health');
    f.block(); await f.advance(8000);
    assert(f.errors.includes('RTC route recovery exhausted'));
    f.close();
}
console.log('adaptive peer routing: loss-triggered dual-end switch | reliable replay | suspension | bounded recovery | cleanup');
