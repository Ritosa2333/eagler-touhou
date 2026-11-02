// Shared game transport policy. Embedded into BrowserPeerTransport by
// tools/sync-adaptive-peer-route.mjs; no runtime module download is required.
// Design references and the wire contract live in docs/architecture/netplay-routing.md.
export function createAdaptivePeerRoute(options) {
    const prefix = 'etr1:';
    const now = options.now || (() => globalThis.performance?.now?.() ?? Date.now());
    const later = options.later || setTimeout, cancel = options.cancel || clearTimeout;
    const controller = options.localPlayer < options.peerId;
    const primary = options.primary;
    const pendingControl = new Map(), receivedControl = new Map();
    let active = primary, backup = null, previous = null, previousUntil = 0;
    let started = false, closed = false, supported = false, helloSent = false;
    let timer = null, lastTick = 0, quietUntil = 0, retryAt = 0, generation = 0;
    let sequence = 0, receivedSequence = 0, pendingBytes = 0, receivedBytes = 0;
    let replayQueue = [];
    let epoch = 0, proposal = null, verification = null, switchedAt = -Infinity;
    let recoveryUntil = 0, lastNeed = -Infinity, verifyAfter = 0;
    let ackPending = false;
    const policy = Object.freeze({
        windowMs: 6000, probeTimeoutMs: 1000, primaryIntervalMs: 250,
        standbyIntervalMs: 10000, verificationIntervalMs: 50,
        verificationMs: 4000, minimumSamples: 12, cooldownMs: 60000,
        maxControlMessages: 256, maxControlBytes: 1024 * 1024,
        ...options.policy,
    });
    const initialize = path => {
        path.samples = [];
        path.probeSequence = 0;
        path.lastProbe = -Infinity;
        path.baseline = Infinity;
        return path;
    };
    initialize(primary);
    const open = dc => dc?.readyState === 'open';
    const ready = path => !!path && open(path.controlDc) && open(path.inputDc) && !path.closed;
    const alive = () => !closed && options.alive();
    const eligible = () => alive() && options.running() && options.visible();
    const paths = () => backup ? [primary, backup] : [primary];
    const pathName = path => path === primary ? 'primary' : 'turn';
    const diagnostic = reason => options.changed?.({
        selected: pathName(active), generation, epoch, reason,
        verifying: !!verification, standbyReady: ready(backup),
    });
    function sendMessage(path, message) {
        if (!alive() || !open(path?.controlDc) || path.controlDc.bufferedAmount > 32768) return false;
        try { path.controlDc.send(prefix + JSON.stringify(message)); return true; }
        catch { return false; }
    }
    function sendEverywhere(message) {
        let sent = false;
        for (const path of paths()) sent = sendMessage(path, message) || sent;
        return sent;
    }
    function envelope(payload, id) {
        const packet = new Uint8Array(payload.byteLength + 8);
        packet.set([69, 84, 82, 49]);
        new DataView(packet.buffer).setUint32(4, id, true);
        packet.set(payload, 8);
        return packet;
    }
    function replay(path) {
        if (!ready(path)) return;
        while (replayQueue.length) {
            if (path.controlDc.bufferedAmount > 32768) break;
            const id = replayQueue[0], packet = pendingControl.get(id);
            if (!packet) { replayQueue.shift(); continue; }
            try { path.controlDc.send(packet); replayQueue.shift(); } catch { break; }
        }
    }
    function stats(path, time, since = time - policy.windowMs) {
        const samples = path.samples.filter(s => s.at >= since && time - s.at >= policy.probeTimeoutMs);
        const rtts = samples.filter(s => s.rtt != null).map(s => s.rtt).sort((a, b) => a - b);
        const loss = samples.length ? (samples.length - rtts.length) / samples.length : 1;
        const p95 = rtts.length ? rtts[Math.ceil(rtts.length * .95) - 1] : Infinity;
        return {count: samples.length, received: rtts.length, lost: samples.length - rtts.length,
            loss, p95, score: p95 + loss * 1000};
    }
    function discardBackup() {
        const path = backup;
        if (!path || active === path) return;
        sendMessage(primary, {kind: 'retire', generation: path.generation});
        path.closed = true;
        try { path.pc.close(); } catch {}
        backup = null;
        retryAt = now() + policy.cooldownMs;
        verification = null;
        if (proposal?.path === path) proposal = null;
        diagnostic('standby-unavailable');
    }
    async function ensureBackup() {
        if (!started || !supported || !options.enabled || !alive() || backup || now() < retryAt) return;
        // The initial ICE race may already have selected TURN. In that case
        // keep that route and avoid allocating a redundant relay association.
        if (primary.path !== 'direct') return;
        const server = options.iceServers().find(server =>
            (Array.isArray(server.urls) ? server.urls : [server.urls]).some(url => /^turns?:/i.test(String(url))));
        if (!server) return;
        const urls = (Array.isArray(server.urls) ? server.urls : [server.urls]).filter(url => /^turns?:/i.test(String(url)));
        if (!controller) return;
        const path = makeBackup(++generation, [{...server, urls}]);
        try {
            await path.pc.setLocalDescription(await path.pc.createOffer());
            if (!alive() || backup !== path) return;
            sendDescription(path);
        } catch { if (backup === path) discardBackup(); }
    }
    function sendDescription(path) {
        sendMessage(primary, {kind: 'description', generation: path.generation, description: path.pc.localDescription});
        path.descriptionSent = true;
        for (const candidate of path.localCandidates.splice(0))
            sendMessage(primary, {kind: 'candidate', generation: path.generation, candidate});
    }
    function makeBackup(id, iceServers) {
        const pc = new (options.PeerConnection || RTCPeerConnection)({iceServers, iceTransportPolicy: 'relay'});
        const path = initialize({pc, generation: id, inputDc: null, controlDc: null,
            pendingCandidates: [], localCandidates: [], createdAt: now(), path: 'turn'});
        backup = path;
        pc.onicecandidate = event => {
            if (!event.candidate || backup !== path || !alive()) return;
            const candidate = event.candidate.toJSON ? event.candidate.toJSON() : event.candidate;
            if (path.descriptionSent) sendMessage(primary, {kind: 'candidate', generation: id, candidate});
            else if (path.localCandidates.length < 64) path.localCandidates.push(candidate);
        };
        const attach = dc => {
            const kind = dc.label === options.tag + '-route-control' ? 'control' :
                dc.label === options.tag + '-route-input' ? 'input' : null;
            if (!kind) { dc.close(); return; }
            path[kind + 'Dc'] = dc;
            dc.binaryType = 'arraybuffer';
            dc.onmessage = event => receive(path, kind, event.data);
            dc.onopen = () => { path.lastProbe = -Infinity; diagnostic('standby-open'); };
            dc.onclose = dc.onerror = () => {
                if (!alive() || path.closed) return;
                if (active === path) {
                    if (!recover()) options.failed('RTC active TURN channel unavailable');
                } else discardBackup();
            };
        };
        pc.onconnectionstatechange = () => {
            if (pc.connectionState !== 'failed' || backup !== path || !alive()) return;
            if (active === path) {
                if (!recover()) options.failed('RTC active TURN connection unavailable');
            } else discardBackup();
        };
        if (controller) {
            attach(pc.createDataChannel(options.tag + '-route-control', {ordered: true}));
            attach(pc.createDataChannel(options.tag + '-route-input', {ordered: false, maxRetransmits: 0}));
        } else pc.ondatachannel = event => attach(event.channel);
        return path;
    }
    let negotiation = Promise.resolve();
    async function receiveNegotiation(message) {
        const id = message.generation;
        if (!supported || !options.enabled || !Number.isSafeInteger(id) || id < 1 || id > 1000000 || !alive()) return;
        if (message.kind === 'description') {
            const description = message.description;
            if (!description || typeof description.sdp !== 'string' || description.sdp.length > 16384 ||
                description.type !== (controller ? 'answer' : 'offer')) return;
            if (!controller && id > generation && !backup) {
                const server = options.iceServers().find(server =>
                    (Array.isArray(server.urls) ? server.urls : [server.urls]).some(url => /^turns?:/i.test(String(url))));
                if (!server) return;
                generation = id;
                makeBackup(id, [{...server, urls: (Array.isArray(server.urls) ? server.urls : [server.urls]).filter(url => /^turns?:/i.test(String(url)))}]);
            }
            const path = backup;
            if (!path || path.generation !== id || path.closed) return;
            await path.pc.setRemoteDescription(description);
            if (!alive() || backup !== path) return;
            for (const candidate of path.pendingCandidates.splice(0)) await path.pc.addIceCandidate(candidate);
            if (!controller) {
                await path.pc.setLocalDescription(await path.pc.createAnswer());
                if (alive() && backup === path) sendDescription(path);
            }
        } else {
            const path = backup, candidate = message.candidate;
            if (!path || path.generation !== id || !candidate || typeof candidate.candidate !== 'string' ||
                candidate.candidate.length > 2048) return;
            if (path.pc.remoteDescription) await path.pc.addIceCandidate(candidate);
            else if (path.pendingCandidates.length < 64) path.pendingCandidates.push(candidate);
        }
    }
    function select(path, id) {
        if (!ready(path) || id < epoch) return false;
        if (active !== path) {
            previous = active;
            previousUntil = now() + 5000;
            active = path;
            switchedAt = now();
            options.selected(path);
        }
        epoch = id;
        recoveryUntil = 0;
        replayQueue = [...pendingControl.keys()];
        replay(path);
        diagnostic('selected');
        return true;
    }
    function verify(force = false) {
        if (!supported || !eligible()) return;
        const other = active === primary ? backup : primary;
        if (!ready(other)) return;
        if (!force && (now() < verifyAfter || now() - switchedAt < policy.cooldownMs)) return;
        if (!verification) {
            verification = {since: now(), force};
            sendEverywhere({kind: 'verify'});
            diagnostic('verifying');
        } else if (force) verification.force = true;
    }
    function recover() {
        if (!started || !supported || !eligible()) return false;
        const other = active === primary ? backup : primary;
        if (!ready(other)) return false;
        recoveryUntil ||= now() + 7000;
        verify(true);
        if (!controller && now() - lastNeed >= 1000) {
            lastNeed = now(); sendEverywhere({kind: 'need'});
        }
        return true;
    }
    function receive(path, kind, data) {
        if (!alive() || path.closed) return true;
        if (typeof data === 'string') {
            if (!data.startsWith(prefix) || data.length > 24576) return true;
            let m; try { m = JSON.parse(data.slice(prefix.length)); } catch { return true; }
            if (!m || typeof m !== 'object' || Array.isArray(m)) return true;
            if (m.kind === 'hello' && path === primary && kind === 'control') {
                supported = m.enabled === true && options.enabled;
                if (!helloSent) hello();
                ensureBackup().catch(() => {});
            } else if (supported && ['description', 'candidate'].includes(m.kind) && path === primary && kind === 'control') {
                negotiation = negotiation.then(() => receiveNegotiation(m)).catch(() => discardBackup());
            } else if (supported && m.kind === 'ping' && kind === 'input' && Number.isSafeInteger(m.id) && m.id > 0) {
                if (!eligible()) { sendMessage(path, {kind: 'pause'}); return true; }
                try { path.inputDc.send(prefix + JSON.stringify({kind: 'pong', id: m.id})); } catch {}
            } else if (supported && m.kind === 'pong' && kind === 'input') {
                const sample = path.samples.find(s => s.id === m.id);
                if (sample && sample.rtt == null) {
                    sample.rtt = Math.max(0, now() - sample.at);
                    path.baseline = Math.min(path.baseline, sample.rtt);
                }
            } else if (supported && kind === 'control') {
                if (m.kind === 'pause') suspend();
                else if (m.kind === 'retire' && path === primary && backup?.generation === m.generation && active !== backup) discardBackup();
                else if (m.kind === 'verify' && now() >= verifyAfter && eligible()) verification ||= {since: now(), force: false};
                else if (m.kind === 'need' && controller) verify(true);
                else if (m.kind === 'ack' && Number.isSafeInteger(m.sequence) && m.sequence >= 0 && m.sequence <= sequence) {
                    for (const [id, packet] of pendingControl) {
                        if (id <= m.sequence) { pendingBytes -= packet.byteLength; pendingControl.delete(id); }
                    }
                } else if (m.kind === 'switch' && !controller && Number.isSafeInteger(m.epoch) && m.epoch === epoch + 1 &&
                    m.generation === generation && ['primary', 'turn'].includes(m.path)) {
                    const next = m.path === 'primary' ? primary : backup;
                    if (!ready(next) || !eligible()) return true;
                    proposal = {id: m.epoch, path: next, at: now()};
                    sendMessage(next, {kind: 'accept', epoch: m.epoch, generation});
                } else if (m.kind === 'accept' && controller && proposal && m.epoch === proposal.id &&
                    m.generation === generation && path === proposal.path && ready(path)) {
                    // Commit travels on the new reliable channel before any
                    // replayed controls. Receiver ordering spans both paths.
                    if (sendMessage(path, {kind: 'commit', epoch: proposal.id, generation})) {
                        select(path, proposal.id);
                        proposal.committed = true; proposal.lastSent = now(); verification = null;
                    }
                } else if (m.kind === 'commit' && !controller && proposal && m.epoch === proposal.id &&
                    m.generation === generation && path === proposal.path) {
                    select(path, proposal.id);
                    sendMessage(path, {kind: 'committed', epoch: proposal.id, generation});
                    proposal = null; verification = null;
                } else if (m.kind === 'commit' && !controller && m.epoch === epoch && m.generation === generation && path === active) {
                    sendMessage(path, {kind: 'committed', epoch, generation});
                } else if (m.kind === 'committed' && controller && proposal?.committed &&
                    m.epoch === proposal.id && m.generation === generation && path === proposal.path) {
                    proposal = null;
                }
            }
            return true;
        }
        if (kind === 'control' && supported) {
            const bytes = data instanceof ArrayBuffer ? new Uint8Array(data) :
                ArrayBuffer.isView(data) ? new Uint8Array(data.buffer, data.byteOffset, data.byteLength) : null;
            if (bytes?.length >= 8 && bytes[0] === 69 && bytes[1] === 84 && bytes[2] === 82 && bytes[3] === 49) {
                const id = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength).getUint32(4, true);
                if (!id || id > receivedSequence + policy.maxControlMessages || bytes.length > policy.maxControlBytes) return true;
                if (id > receivedSequence && !receivedControl.has(id)) {
                    if (receivedBytes + bytes.length > policy.maxControlBytes) { options.failed('RTC route control receive limit exceeded'); return true; }
                    receivedControl.set(id, bytes.slice(8)); receivedBytes += bytes.length;
                    while (receivedControl.has(receivedSequence + 1)) {
                        const payload = receivedControl.get(++receivedSequence);
                        receivedControl.delete(receivedSequence); receivedBytes -= payload.length + 8;
                        options.receive(payload);
                    }
                }
                ackPending = !sendMessage(path, {kind: 'ack', sequence: receivedSequence});
                return true;
            }
        }
        if (path !== active && !(path === previous && now() <= previousUntil)) return true;
        // Existing game input/repair packets retain their native frame/session
        // identity. The native SessionChannel already owns their deduplication.
        options.receive(data);
        return true;
    }
    function suspend() {
        quietUntil = now() + 2000;
        verification = null;
        // An accepted/committed epoch is a promise, not a quality sample.
        // Keep it across browser suspension so a delayed commit cannot leave
        // the two endpoints permanently selecting different paths.
        if (proposal && controller && !proposal.committed) proposal = null;
        else if (proposal) proposal.at = now();
        for (const path of paths()) path.samples.length = 0;
    }
    function hello() {
        if (!open(primary.controlDc) || helloSent) return;
        helloSent = sendMessage(primary, {kind: 'hello', enabled: !!options.enabled});
    }
    function tick() {
        timer = null;
        if (!alive() || !started) return;
        const time = now();
        if (time - lastTick > 750 || !eligible()) {
            if (verification || time - lastTick > 750) sendEverywhere({kind: 'pause'});
            suspend();
        }
        lastTick = time;
        if (supported && time >= quietUntil && eligible()) {
            if (ackPending) ackPending = !sendMessage(active, {kind: 'ack', sequence: receivedSequence});
            replay(active);
            ensureBackup().catch(() => {});
            if (backup && !ready(backup) && time - backup.createdAt > 15000) discardBackup();
            for (const path of paths()) {
                if (!ready(path)) continue;
                const interval = verification ? policy.verificationIntervalMs :
                    path === active ? policy.primaryIntervalMs : policy.standbyIntervalMs;
                path.samples = path.samples.filter(s => time - s.at <= policy.windowMs).slice(-128);
                if (time - path.lastProbe < interval || path.inputDc.bufferedAmount > 4096) continue;
                const id = ++path.probeSequence;
                try {
                    path.inputDc.send(prefix + JSON.stringify({kind: 'ping', id}));
                    path.samples.push({id, at: time, rtt: null}); path.lastProbe = time;
                } catch {}
            }
            const current = stats(active, time);
            if (controller && !verification && current.count >= policy.minimumSamples &&
                ((current.lost >= 3 && current.loss >= .1) ||
                 current.p95 > Math.max(120, active.baseline * 1.6 + 30))) verify();
            if (!controller && !verification && current.count >= policy.minimumSamples && current.lost >= 3 && current.loss >= .1)
                verify();
            if (verification && time - verification.since >= policy.probeTimeoutMs + 600) {
                const next = active === primary ? backup : primary;
                if (controller && ready(next) && !proposal) {
                    const candidate = stats(next, time, verification.since);
                    const present = stats(active, time, verification.since);
                    if (candidate.count >= policy.minimumSamples && candidate.received >= policy.minimumSamples &&
                        candidate.loss <= .05 && (verification.force && !ready(active) ||
                        candidate.score * 1.1 + 5 < present.score)) {
                        proposal = {id: epoch + 1, path: next, at: time};
                        sendMessage(next, {kind: 'switch', path: pathName(next), epoch: proposal.id, generation});
                    }
                }
                if (time - verification.since > policy.verificationMs) { verification = null; verifyAfter = time + 10000; }
            }
            if (proposal?.committed) {
                if (time - proposal.at > 7000) options.failed('RTC route commit confirmation exhausted');
                else if (time - proposal.lastSent >= 250) {
                    sendMessage(proposal.path, {kind: 'commit', epoch: proposal.id, generation});
                    proposal.lastSent = time;
                }
            } else if (controller && proposal && time - proposal.at > 3000) { proposal = null; verification = null; verifyAfter = time + 10000; }
            if (recoveryUntil && time > recoveryUntil) { recoveryUntil = 0; options.failed('RTC route recovery exhausted'); }
        }
        timer = later(tick, verification ? policy.verificationIntervalMs : policy.primaryIntervalMs);
    }
    return {
        start() {
            if (started || closed) return;
            started = true; lastTick = now(); hello();
            timer = later(tick, policy.primaryIntervalMs);
            ensureBackup().catch(() => {});
        },
        opened(kind) { if (kind === 'control') hello(); },
        receivePrimary(kind, data) { return receive(primary, kind, data); },
        canSend(control, size = 0) {
            return open(control ? active.controlDc : active.inputDc) && (!control ||
                (!replayQueue.length && active.controlDc.bufferedAmount <= 32768)) && (!control || !supported ||
                (pendingControl.size < policy.maxControlMessages && pendingBytes + size + 8 <= policy.maxControlBytes));
        },
        send(payload, control) {
            if (!alive() || !this.canSend(control, payload.byteLength)) return false;
            const dc = control ? active.controlDc : active.inputDc;
            if (control && supported) {
                if (sequence >= 0xffffffff) { options.failed('RTC route control sequence exhausted'); return false; }
                const id = ++sequence, packet = envelope(payload, id);
                pendingControl.set(id, packet); pendingBytes += packet.byteLength;
                try { dc.send(packet); } catch {
                    // Accepted reliable data owns this sequence even when the
                    // browser temporarily refuses the send. Retry it before
                    // admitting another control; a standby may not exist yet.
                    replayQueue.push(id);
                    recover();
                }
                return true;
            }
            try { dc.send(payload); return true; } catch { return false; }
        },
        recover,
        recovering: () => recoveryUntil > 0,
        snapshot: () => ({selected: pathName(active), epoch, generation, supported,
            pendingControl: pendingControl.size, pendingBytes, receivedBytes, standbyReady: ready(backup),
            primary: stats(primary, now()), turn: backup ? stats(backup, now()) : null}),
        close() {
            if (closed) return;
            closed = true;
            if (timer != null) cancel(timer);
            if (backup) { backup.closed = true; try { backup.pc.close(); } catch {} }
            pendingControl.clear(); receivedControl.clear();
            replayQueue.length = 0;
            pendingBytes = receivedBytes = 0;
            primary.samples.length = 0;
        },
    };
}
