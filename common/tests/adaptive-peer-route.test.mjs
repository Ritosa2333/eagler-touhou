import assert from 'node:assert/strict';
import {createAdaptivePeerRoute} from '../browser/adaptive-peer-route.mjs';

// Deterministic network, not a mock of the policy: both endpoints execute the
// production route manager, its real handshake, probes and control envelopes.
function fixture(config = {}) {
    let time = 1, id = 0, nextPc = 0;
    const tasks = new Map(), pcs = new Map(), deliveries = [[], []], selected = [[], []], errors = [];
    let dropPrimary = false, holdControl = false, visible = [true, true], running = true;
    let dropCommit = false, droppedCommit = 0;
    const held = [];
    const later = (fn, ms) => { tasks.set(++id, {fn, at: time + ms}); return id; };
    const cancel = id => tasks.delete(id);
    class Channel {
        readyState = 'connecting'; bufferedAmount = 0;
        constructor(label, path) { this.label = label; this.path = path; }
        send(data) {
            if (this.readyState !== 'open') throw Error('closed');
            const payload = typeof data === 'string' ? data : data.slice().buffer;
            if (dropCommit && typeof data === 'string' && data.startsWith('etr1:') &&
                JSON.parse(data.slice(5)).kind === 'commit' && droppedCommit++ === 0) return;
            if (this.path === 'primary' && dropPrimary && this.label === 'input') return;
            const deliver = () => { if (this.other.readyState === 'open') this.other.onmessage?.({data: payload}); };
            if (this.path === 'primary' && holdControl && typeof data !== 'string' && this.label === 'control') held.push(deliver);
            else later(deliver, 20);
        }
        close() { this.readyState = 'closed'; this.onclose?.(); }
    }
    function channels(label, path) {
        const a = new Channel(label, path), b = new Channel(label, path);
        a.other = b; b.other = a;
        return [a, b];
    }
    class Peer {
        channels = []; connectionState = 'new';
        constructor(config) { this.config = config; this.id = ++nextPc; pcs.set(this.id, this); }
        createDataChannel(label) { const dc = new Channel(label, 'turn'); this.channels.push(dc); return dc; }
        async createOffer() { return {type: 'offer', sdp: String(this.id)}; }
        async createAnswer() { return {type: 'answer', sdp: String(this.id)}; }
        async setLocalDescription(description) { this.localDescription = description; }
        async setRemoteDescription(description) {
            this.remoteDescription = description;
            if (description.type !== 'offer') return;
            const other = pcs.get(Number(description.sdp));
            for (const a of other.channels) {
                const b = new Channel(a.label, 'turn'); a.other = b; b.other = a;
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
        iceServers: () => [{urls: 'turn:fixture:3478', username: 'test', credential: 'test'}],
        alive: () => true, running: () => running, visible: () => visible[seat],
        now: () => time, later, cancel, PeerConnection: Peer,
        receive: bytes => deliveries[seat].push(Array.from(new Uint8Array(bytes.buffer || bytes, bytes.byteOffset || 0, bytes.byteLength))),
        selected: path => selected[seat].push(path === primary ? 'primary' : 'turn'),
        failed: message => errors.push(message),
    }));
    paths.forEach((path, seat) => {
        if (config.primaryPath) path.path = config.primaryPath;
        for (const kind of ['input', 'control']) {
            const dc = path[kind + 'Dc'];
            dc.onmessage = event => routes[seat].receivePrimary(kind, event.data);
            dc.readyState = 'open';
        }
        routes[seat].opened('control'); routes[seat].start();
    });
    const flush = async () => { for (let i = 0; i < 20; i++) await Promise.resolve(); };
    return {
        routes, paths, deliveries, selected, errors, tasks, pcs, later,
        drop(value = true) { dropPrimary = value; },
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
        close() { for (const route of routes) route.close(); },
    };
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
    const f = fixture({primaryPath: 'turn'}); await f.advance(6000);
    assert.equal(f.pcs.size, 0, 'an initial TURN route does not allocate another TURN connection');
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
console.log('adaptive peer routing: loss-triggered dual-end switch | reliable replay | suspension | bounded recovery | cleanup');
