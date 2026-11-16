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
    let planned = null, retained = null, standbySerial = 0, lastSlowReport = -Infinity;
    let started = false, closed = false, supported = false, helloSent = false;
    let peerSwitching = true;
    let directRetry = false, pairedQuality = false, verificationTicket = 0;
    let timer = null, lastTick = 0, quietUntil = 0, retryAt = 0, generation = 0;
    let sequence = 0, receivedSequence = 0, pendingBytes = 0, receivedBytes = 0;
    let replayQueue = [];
    let epoch = 0, proposal = null, verification = null, switchedAt = -Infinity;
    let recoveryUntil = 0, recoveryProbe = 0, lastNeed = -Infinity, verifyAfter = 0;
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
        path.lastSuccessfulProbe = 0;
        path.lastRoundTripAt = -Infinity;
        path.baseline = Infinity;
        return path;
    };
    initialize(primary);
    const open = dc => dc?.readyState === 'open';
    const ready = path => !!path && open(path.controlDc) && open(path.inputDc) && !path.closed;
    const alive = () => !closed && options.alive();
    const eligible = () => alive() && options.running() && options.visible();
    const paths = () => [primary, backup, planned, retained].filter(Boolean);
    const pathName = path => path === primary ? 'primary' : path.path;
    const wireGeneration = path => path.surveySerial || generation;
    const measuring = () => planned?.measuring && !planned.reported && !planned.closed;
    const otherPath = () => [planned, backup, retained, primary].find(path =>
        path !== active && ready(path) && !path.measuring);
    const switching = () => options.allowSwitching !== false && peerSwitching;
    const surveyEnabled = () => options.surveyEnabled?.() === true && options.enabled && switching();
    const activeRoute = () => active.surveySerial ? active.path :
        active.path === 'direct' ? 'direct' : options.initialRelayRoute?.() || 'turn';
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
    function stats(path, time, since = time - policy.windowMs, count = 128) {
        const samples = path.samples.filter(s => s.at >= since && time - s.at >= policy.probeTimeoutMs).slice(-count);
        const rtts = samples.filter(s => s.rtt != null).map(s => s.rtt).sort((a, b) => a - b);
        const loss = samples.length ? (samples.length - rtts.length) / samples.length : 1;
        const p95 = rtts.length ? rtts[Math.ceil(rtts.length * .95) - 1] : Infinity;
        return {count: samples.length, received: rtts.length, lost: samples.length - rtts.length,
            loss, p95, score: p95 + loss * 1000};
    }
    function qualityReport(quality) {
        return {count: quality.count, received: quality.received, p95: quality.received ? Math.ceil(quality.p95) : null};
    }
    function surveyQuality(quality) {
        return {sent: quality.count, received: quality.received, p95: quality.received ? Math.ceil(quality.p95) : null};
    }
    function readQuality(value) {
        if (!value || !Number.isInteger(value.count) || value.count < 0 || value.count > 128 ||
            !Number.isInteger(value.received) || value.received < 0 || value.received > value.count) return null;
        const {count, received, p95} = value;
        if (received ? !Number.isInteger(p95) || p95 < 0 || p95 > 10000 : p95 !== null) return null;
        return {count, received, p95: p95 ?? Infinity, loss: count ? (count - received) / count : 1};
    }
    function pairedScore(local, remote) {
        const loss = Math.max(local.loss, remote.loss);
        const p95 = Math.max(local.p95, remote.p95);
        return {loss, p95, received: Math.min(local.received, remote.received), score: p95 + loss * 1000};
    }
    function discardBackup() {
        const path = backup;
        if (!path || active === path || proposal?.path === path) return;
        sendMessage(primary, {kind: 'retire', generation: path.generation});
        closePath(path);
        backup = null;
        retryAt = now() + policy.cooldownMs;
        if (verification?.target === path) verification = null;
        diagnostic('standby-unavailable');
    }
    function closePath(path) {
        path.closed = true;
        try { path.pc.close(); } catch {}
        if (previous === path) previous = null;
        if (verification?.target === path) verification = null;
    }
    function backupServers(route) {
        const protocol = route === 'direct' ? /^stuns?:/i : /^turns?:/i;
        const servers = options.iceServers().flatMap(server => {
            const urls = (Array.isArray(server.urls) ? server.urls : [server.urls]).filter(url => protocol.test(String(url)));
            return urls.length ? [{...server, urls}] : [];
        });
        return route === 'direct' ? servers : servers.slice(0, 1);
    }
    async function ensureBackup() {
        if (!controller || !started || !supported || !switching() || !alive() || backup || planned || now() < retryAt) return;
        const route = primary.path === 'direct' ? 'turn' : primary.path === 'turn' && directRetry ? 'direct' : null;
        if (!route) return;
        const servers = backupServers(route);
        if (route === 'turn' && !servers.length) return;
        const path = makeBackup(++generation, route, servers);
        try {
            await path.pc.setLocalDescription(await path.pc.createOffer());
            if (!alive() || backup !== path) return;
            sendDescription(path);
        } catch { if (backup === path) discardBackup(); }
    }
    function sendDescription(path) {
        sendStandbySignal(path, {description: path.pc.localDescription});
        path.descriptionSent = true;
        for (const candidate of path.localCandidates.splice(0))
            sendStandbySignal(path, {candidate});
    }
    function sendStandbySignal(path, payload) {
        if (path.surveySerial) {
            options.surveySignal?.({type: 'rtc-standby-signal', serial: path.surveySerial, ...payload});
        } else {
            sendMessage(primary, {kind: payload.description ? 'description' : 'candidate', generation: path.generation, ...payload,
                ...(payload.description && path.path === 'direct' && {route: 'direct'})});
        }
    }
    function makeBackup(id, route, iceServers, phase) {
        const pc = new (options.PeerConnection || RTCPeerConnection)({iceServers, iceTransportPolicy: route === 'direct' ? 'all' : 'relay'});
        const path = initialize({pc, generation: id, inputDc: null, controlDc: null,
            pendingCandidates: [], localCandidates: [], createdAt: now(), path: route});
        if (phase) {
            path.surveySerial = id;
            path.measuring = phase === 'measure';
            planned = path;
        } else backup = path;
        pc.onicecandidate = event => {
            if (!event.candidate || path.closed || !alive()) return;
            const candidate = event.candidate.toJSON ? event.candidate.toJSON() : event.candidate;
            if (path.descriptionSent) sendStandbySignal(path, {candidate});
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
                pathUnavailable(path);
            };
        };
        pc.onconnectionstatechange = () => {
            if (pc.connectionState === 'failed' && !path.closed && alive()) pathUnavailable(path);
        };
        if (controller) {
            attach(pc.createDataChannel(options.tag + '-route-control', {ordered: true}));
            attach(pc.createDataChannel(options.tag + '-route-input', {ordered: false, maxRetransmits: 0}));
        } else pc.ondatachannel = event => attach(event.channel);
        return path;
    }
    function pathUnavailable(path) {
        if (active === path) {
            if (!recover()) options.failed('RTC active standby connection unavailable');
        } else if (path === backup) discardBackup();
        else if (path === planned && !path.reported) {
            if (path.measuring) options.surveySignal?.({type: 'rtc-route-report', serial: path.surveySerial, quality: null});
            path.reported = true;
        }
    }
    async function relayStandby(message) {
        if (!started || !surveyEnabled() || !alive()) return;
        const {serial, phase, route, iceServers} = message;
        if (!Number.isSafeInteger(serial) || serial <= 0 || serial > 0xffffffff) return;
        if (message.type === 'rtc-standby-signal') {
            if (planned?.surveySerial === serial) await applyStandbySignal(planned, message);
            return;
        }
        if (serial <= standbySerial || !['idle', 'measure', 'select'].includes(phase)) return;
        if (phase !== 'idle' && (!/^(direct|turn|swarm-[0-9]+)$/.test(route) || !Array.isArray(iceServers))) return;
        if (planned && proposal?.path === planned) return;
        standbySerial = serial;
        if (planned === active) {
            if (retained) closePath(retained);
            retained = planned;
        } else if (planned) closePath(planned);
        planned = null;
        if (phase === 'idle') return;
        try {
            const path = makeBackup(serial, route, iceServers, phase);
            if (controller) {
                await path.pc.setLocalDescription(await path.pc.createOffer());
                if (alive() && !path.closed) sendDescription(path);
            }
        } catch {
            if (phase === 'measure')
                options.surveySignal?.({type: 'rtc-route-report', serial, quality: null});
        }
    }
    async function applyStandbySignal(path, message) {
        if (path.closed) return;
        if (message.description) {
            if (message.description.type !== (controller ? 'answer' : 'offer')) return;
            await path.pc.setRemoteDescription(message.description);
            if (!alive() || path.closed) return;
            for (const candidate of path.pendingCandidates.splice(0)) await path.pc.addIceCandidate(candidate);
            if (!controller) {
                await path.pc.setLocalDescription(await path.pc.createAnswer());
                if (alive() && !path.closed) sendDescription(path);
            }
        } else if (message.candidate) {
            if (path.pc.remoteDescription) await path.pc.addIceCandidate(message.candidate);
            else if (path.pendingCandidates.length < 64) path.pendingCandidates.push(message.candidate);
        }
    }
    let negotiation = Promise.resolve();
    async function receiveNegotiation(message) {
        const id = message.generation;
        if (!supported || !options.enabled || !Number.isSafeInteger(id) || id < 1 || id > 1000000 || !alive()) return;
        if (message.kind === 'description') {
            const description = message.description;
            if (!description || typeof description.sdp !== 'string' || description.sdp.length > 16384 ||
                description.type !== (controller ? 'answer' : 'offer')) return;
            const route = message.route === 'direct' && directRetry ? 'direct' : message.route == null ? 'turn' : null;
            if (!route) return;
            if (!controller && id > generation && !backup) {
                const servers = backupServers(route);
                if (route === 'turn' && !servers.length) return;
                generation = id;
                makeBackup(id, route, servers);
            }
            const path = backup;
            if (!path || path.generation !== id || path.path !== route || path.closed) return;
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
    function verify(force = false, target = otherPath()) {
        if (!supported || !switching() || !eligible()) return;
        if (!ready(target)) return;
        if (!force && (now() < verifyAfter || (!target.surveySerial && now() - switchedAt < policy.cooldownMs))) return;
        if (!verification) {
            const ticket = pairedQuality && controller ? ++verificationTicket : 0;
            verification = {since: now(), force, ticket, target};
            if (target.surveySerial && ticket) sendMessage(target, {kind: 'verify', ticket, serial: target.surveySerial});
            else sendEverywhere({kind: 'verify', ...(ticket && {ticket})});
            diagnostic('verifying');
        } else if (force) verification.force = true;
    }
    function recover() {
        if (!started || !supported || !switching() || !eligible()) return false;
        const surveying = surveyEnabled() && ((planned && planned !== active) || options.surveyPending?.());
        if (!ready(otherPath()) && !surveying) return false;
        if (!recoveryUntil) {
            recoveryUntil = now() + (surveying ? 16000 : 7000);
            recoveryProbe = active.probeSequence;
        }
        verify(true);
        if (!controller && now() - lastNeed >= 1000) {
            lastNeed = now(); sendEverywhere({kind: 'need'});
        }
        return true;
    }
    function finishRecovery(path) {
        // A fresh input round trip and acknowledgement of all queued controls
        // prove progress. An open DataChannel or a pre-failure pong does not.
        // Preserve accepted epochs until the reliable handoff completes.
        if (!recoveryUntil || path !== active || !ready(path) || proposal ||
            pendingControl.size || path.lastSuccessfulProbe <= recoveryProbe ||
            now() - path.lastRoundTripAt > policy.probeTimeoutMs) return;
        recoveryUntil = 0;
        if (verification) verification.force = false;
        diagnostic('recovered');
    }
    function receive(path, kind, data) {
        if (!alive() || path.closed) return true;
        if (typeof data === 'string') {
            if (!data.startsWith(prefix) || data.length > 24576) return true;
            let m; try { m = JSON.parse(data.slice(prefix.length)); } catch { return true; }
            if (!m || typeof m !== 'object' || Array.isArray(m)) return true;
            if (m.kind === 'hello' && path === primary && kind === 'control') {
                supported = m.enabled === true && options.enabled;
                peerSwitching = m.switching !== false;
                directRetry = supported && m.directRetry === true && options.directRetry !== false;
                pairedQuality = supported && m.pairedQuality === true && options.pairedQuality !== false;
                if (!helloSent) hello();
                ensureBackup().catch(() => {});
            } else if (supported && switching() && ['description', 'candidate'].includes(m.kind) && path === primary && kind === 'control') {
                negotiation = negotiation.then(() => receiveNegotiation(m)).catch(() => discardBackup());
            } else if (supported && m.kind === 'ping' && kind === 'input' && Number.isSafeInteger(m.id) && m.id > 0) {
                if (!eligible()) { if (!path.measuring) sendMessage(path, {kind: 'pause'}); return true; }
                try { path.inputDc.send(prefix + JSON.stringify({kind: 'pong', id: m.id})); } catch {}
            } else if (supported && m.kind === 'pong' && kind === 'input') {
                const sample = path.samples.find(s => s.id === m.id);
                const receivedAt = now();
                if (sample && sample.rtt == null && receivedAt - sample.at <= policy.probeTimeoutMs) {
                    sample.rtt = Math.max(0, receivedAt - sample.at);
                    path.baseline = Math.min(path.baseline, sample.rtt);
                    path.lastSuccessfulProbe = Math.max(path.lastSuccessfulProbe, sample.id);
                    path.lastRoundTripAt = receivedAt;
                    finishRecovery(path);
                }
            } else if (supported && kind === 'control' && !path.measuring) {
                if (m.kind === 'pause') suspend();
                else if (m.kind === 'retire' && path === primary && backup?.generation === m.generation && active !== backup) discardBackup();
                else if (m.kind === 'verify' && switching() && eligible()) {
                    if (!pairedQuality && now() >= verifyAfter) verification ||= {since: now(), force: false, target: otherPath()};
                    else if (pairedQuality && controller && m.ticket == null) verify();
                    else if (pairedQuality && !controller && Number.isInteger(m.ticket) && m.ticket > verificationTicket &&
                        m.ticket <= 0xffffffff) {
                        const target = m.serial == null ? otherPath() : path;
                        if (!target || (m.serial != null && (path !== planned || path.surveySerial !== m.serial))) return true;
                        verificationTicket = m.ticket;
                        verification = {since: now(), force: !!recoveryUntil, ticket: m.ticket, target};
                    }
                }
                else if (m.kind === 'quality' && pairedQuality && controller && verification?.ticket && verification.ticket === m.ticket &&
                    path === verification.target && m.generation === wireGeneration(path) && m.epoch === epoch) {
                    const present = readQuality(m.present), candidate = readQuality(m.candidate);
                    if (present && candidate?.count >= policy.minimumSamples) verification.remote = {present, candidate};
                }
                else if (m.kind === 'need' && controller) verify(true);
                else if (m.kind === 'ack' && Number.isSafeInteger(m.sequence) && m.sequence >= 0 && m.sequence <= sequence) {
                    for (const [id, packet] of pendingControl) {
                        if (id <= m.sequence) { pendingBytes -= packet.byteLength; pendingControl.delete(id); }
                    }
                    finishRecovery(path);
                } else if (m.kind === 'switch' && switching() && !controller && Number.isSafeInteger(m.epoch) && m.epoch === epoch + 1 &&
                    m.generation === wireGeneration(path) && m.path === pathName(path)) {
                    if (!ready(path) || !eligible() || (proposal && proposal.path !== path)) return true;
                    proposal = {id: m.epoch, path, at: now()};
                    sendMessage(path, {kind: 'accept', epoch: m.epoch, generation: wireGeneration(path)});
                } else if (m.kind === 'accept' && controller && proposal && m.epoch === proposal.id &&
                    m.generation === wireGeneration(path) && path === proposal.path && ready(path)) {
                    // Commit travels on the new reliable channel before any
                    // replayed controls. Receiver ordering spans both paths.
                    if (sendMessage(path, {kind: 'commit', epoch: proposal.id, generation: wireGeneration(path)})) {
                        select(path, proposal.id);
                        proposal.committed = true; proposal.lastSent = now(); verification = null;
                    }
                } else if (m.kind === 'commit' && !controller && proposal && m.epoch === proposal.id &&
                    m.generation === wireGeneration(path) && path === proposal.path) {
                    select(path, proposal.id);
                    sendMessage(path, {kind: 'committed', epoch: proposal.id, generation: wireGeneration(path)});
                    proposal = null; verification = null;
                } else if (m.kind === 'commit' && !controller && m.epoch === epoch && m.generation === wireGeneration(path) && path === active) {
                    sendMessage(path, {kind: 'committed', epoch, generation: wireGeneration(path)});
                } else if (m.kind === 'committed' && controller && proposal?.committed &&
                    m.epoch === proposal.id && m.generation === wireGeneration(path) && path === proposal.path) {
                    proposal = null;
                }
            }
            return true;
        }
        if (path.measuring) return true;
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
        if (recoveryUntil) {
            // Hidden pages and scheduler gaps cannot spend the active recovery
            // budget. Require new probes after the existing settling window.
            recoveryUntil = quietUntil + 7000;
            recoveryProbe = active.probeSequence;
        }
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
        helloSent = sendMessage(primary, {kind: 'hello', enabled: !!options.enabled, switching: options.allowSwitching !== false,
            directRetry: options.directRetry !== false, pairedQuality: options.pairedQuality !== false});
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
            if (planned && planned !== active && proposal?.path !== planned && time - planned.createdAt > 15000) {
                closePath(planned); planned = null;
            }
            for (const path of paths()) {
                if (!ready(path) || path.measuring && path.reported) continue;
                const interval = path.measuring || verification ? policy.verificationIntervalMs :
                    path === active ? policy.primaryIntervalMs : policy.standbyIntervalMs;
                path.samples = path.samples.filter(s => time - s.at <= policy.windowMs).slice(-128);
                if (time - path.lastProbe < interval) continue;
                const id = ++path.probeSequence;
                if (path.inputDc.bufferedAmount <= 4096) {
                    try { path.inputDc.send(prefix + JSON.stringify({kind: 'ping', id})); } catch {}
                }
                path.samples.push({id, at: time, rtt: null});
                path.lastProbe = time;
            }
            const current = stats(active, time);
            const route = activeRoute();
            const initialSurvey = standbySerial === 0 && options.initialSwarmSurvey?.() === true && route !== 'direct';
            if (surveyEnabled() && pairedQuality && current.count >= policy.minimumSamples &&
                (current.p95 > 150 || current.loss > .05 || initialSurvey) && time - lastSlowReport >= 5000) {
                options.surveySignal?.({type: 'rtc-route-slow', route, quality: surveyQuality(stats(active, time, time - policy.windowMs, 40))});
                lastSlowReport = time;
            }
            if (measuring() && ready(planned)) {
                const result = stats(planned, time, planned.createdAt, 40);
                if (result.count >= policy.minimumSamples)
                    planned.reported = options.surveySignal?.({type: 'rtc-route-report', serial: planned.surveySerial, quality: surveyQuality(result)});
            }
            const selectPlanned = controller && planned && !planned.measuring && planned !== active && ready(planned);
            if (selectPlanned && !verification && !proposal) verify(false, planned);
            if (controller && !planned?.measuring && !verification && !proposal && current.count >= policy.minimumSamples &&
                ((active === primary && backup?.path === 'direct' && !backup.compared && ready(backup)) ||
                 (current.lost >= 3 && current.loss >= .1) ||
                 current.p95 > Math.max(120, active.baseline * 1.6 + 30))) verify();
            if (!controller && !planned?.measuring && !verification && !proposal && current.count >= policy.minimumSamples && current.lost >= 3 && current.loss >= .1)
                verify();
            if (verification && time - verification.since >= policy.probeTimeoutMs + 600) {
                const next = verification.target ?? otherPath();
                if (ready(next) && !proposal) {
                    const localCandidate = stats(next, time, verification.since);
                    const present = stats(active, time, verification.since);
                    if (!controller && pairedQuality && verification.ticket && !verification.reported &&
                        localCandidate.count >= policy.minimumSamples && (!ready(active) || present.count >= policy.minimumSamples)) {
                        verification.reported = sendMessage(next, {kind: 'quality', ticket: verification.ticket, epoch, generation: wireGeneration(next),
                            present: qualityReport(present), candidate: qualityReport(localCandidate)});
                    } else if (controller && localCandidate.count >= policy.minimumSamples && (!pairedQuality || verification.remote)) {
                        next.compared = true;
                        const candidate = pairedQuality ? pairedScore(localCandidate, verification.remote.candidate) : localCandidate;
                        const current = pairedQuality ? pairedScore(present, verification.remote.present) : present;
                        if (candidate.received >= policy.minimumSamples && candidate.loss <= .05 &&
                            (verification.force && !ready(active) || candidate.score * 1.1 + 5 < current.score)) {
                            proposal = {id: epoch + 1, path: next, at: time};
                            sendMessage(next, {kind: 'switch', path: pathName(next), epoch: proposal.id, generation: wireGeneration(next)});
                        }
                    }
                }
                if (time - verification.since > policy.verificationMs) { verification = null; verifyAfter = time + 10000; }
            }
            if (proposal?.committed) {
                if (time - proposal.at > 7000) options.failed('RTC route commit confirmation exhausted');
                else if (time - proposal.lastSent >= 250) {
                    sendMessage(proposal.path, {kind: 'commit', epoch: proposal.id, generation: wireGeneration(proposal.path)});
                    proposal.lastSent = time;
                }
            } else if (controller && proposal && time - proposal.at > 3000) { proposal = null; verification = null; verifyAfter = time + 10000; }
            if (recoveryUntil && time > recoveryUntil) { recoveryUntil = 0; options.failed('RTC route recovery exhausted'); }
        }
        timer = later(tick, verification || measuring() ? policy.verificationIntervalMs : policy.primaryIntervalMs);
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
        relayStandby,
        activeRelayMode() {
            const route = activeRoute();
            return route === 'direct' ? null : route === 'turn' ? 'turn' : 'swarm';
        },
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
            for (const path of paths()) if (path !== primary) closePath(path);
            pendingControl.clear(); receivedControl.clear();
            replayQueue.length = 0;
            pendingBytes = receivedBytes = 0;
            primary.samples.length = 0;
        },
    };
}
