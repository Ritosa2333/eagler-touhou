#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <netplay/NetplayTransportConfig.hpp>

#include <limits>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace Netplay
{
#ifdef __EMSCRIPTEN__
EM_JS(int, eagler_peer_connect, (const char *relayUrlPtr, const char *tagPtr, int localPlayer, int playerCount, int stageSpectatorHistory), {
    const decode = ptr => {
        const end = HEAPU8.indexOf(0, ptr);
        return new TextDecoder().decode(HEAPU8.subarray(ptr, end >= 0 ? end : HEAPU8.length));
    };
    const relayUrl = decode(relayUrlPtr);
    const tag = decode(tagPtr);
    if (!/^[A-Za-z0-9_-]{1,32}$/.test(tag)) return 0;
    const legacyGlobal = `__${tag}PeerTransport`;
    try {
        if (globalThis.__eaglerPeerTransport?.close) globalThis.__eaglerPeerTransport.close();
        const relay = new URL(relayUrl, location.href);
        relay.searchParams.set('player', String(localPlayer));
        relay.searchParams.set('players', String(playerCount));
        const signalUrl = new URL(relay.href);
        signalUrl.searchParams.set('signal', '1');

        // BEGIN generated adaptive-peer-route
        // Shared game transport policy. Embedded into BrowserPeerTransport by
        // tools/sync-adaptive-peer-route.mjs; no runtime module download is required.
        // Design references and the wire contract live in docs/architecture/netplay-routing.md.
        function createAdaptivePeerRoute(options) {
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
        // END generated adaptive-peer-route
        const state = {
            relay: null, signal: null, peers: new Map(), received: [], receivedHead: 0, pendingSignals: [],
            receivedBytes: 0, pendingReceivedBytes: 0, pendingReceivedPackets: 0,
            maxReceivedPackets: 16384, maxReceivedBytes: 16 * 1024 * 1024,
            localPlayer, playerCount, tag, legacyGlobal,
            controlLabel: `${tag}-control`, inputLabel: `${tag}-input`,
            route: null, failed: false, disconnected: false, error: String(), closed: false,
            channelEvents: [],
            spectatorCount: Math.max(0, Number(Module.eaglerOptions?.netplaySpectatorCount) || 0),
            retainSpectatorHistory: !!stageSpectatorHistory && localPlayer === 0,
            flushSpectatorSession() {
                if (!this.pendingSpectatorSession) return true;
                if (this.relay?.readyState !== WebSocket.OPEN || this.relay.bufferedAmount > 64000) return false;
                try { this.relay.send(JSON.stringify(this.pendingSpectatorSession)); this.pendingSpectatorSession = null; return true; }
                catch { return false; }
            },
            rtcReadySent: false, signalReconnectTimer: null, signalReconnectDelayMs: 500,
            rtcHealthTimer: null,
            iceServers: Array.isArray(Module.eaglerOptions?.netplayIceServers)
                ? Module.eaglerOptions.netplayIceServers : [],
            iceTransportPolicy: Module.eaglerOptions?.netplayPreferTurn === true ? 'relay' : 'all',
            allowWebsocketFallback: Module.eaglerOptions?.netplayAllowWebsocketFallback === true,
            sendSignal(payload) {
                const message = JSON.stringify(payload);
                if (this.signal?.readyState === WebSocket.OPEN) {
                    this.signal.send(message);
                    return;
                }
                if (this.route === 'rtc' && !this.closed) {
                    this.pendingSignals.push(message);
                    if (this.pendingSignals.length > 64) this.pendingSignals.shift();
                    this.openSignal();
                }
            },
            fail(message, notify = true) {
                if (this.closed || this.failed || this.disconnected) return;
                if(notify && this.route==='rtc')this.sendSignal({type:'peer-disconnected'});
                // Closed SCTP channels cannot be reopened by an ICE restart.
                // Keep the world intact and report a connection-ended state.
                this.disconnected = true;
                this.error = String(message || 'peer transport failed');
                for(const peer of this.peers.values()) { this.clearPeerRecovery(peer); peer.adaptive?.close(); }
                this.onDisconnect?.();
                this.onReceive?.();
            },
            isRecovering() {
                return !this.disconnected && [...this.peers.values()].some(peer =>
                    peer.recoveryPending || peer.recoveryTimer || peer.restartInFlight || peer.adaptive?.recovering());
            },
            setRoute(mode) {
                if (this.closed || this.route) return;
                if (mode === 'failed' || (mode === 'relay' && !this.allowWebsocketFallback)) {
                    this.fail(mode === 'failed' ? 'WebRTC connection unavailable' : 'WebRTC connection unavailable; WebSocket gameplay fallback is disabled', false);
                    return;
                }
                this.route = mode;
                globalThis.__eaglerNetplayTransport = mode;
                if (mode === 'rtc') {
                    if (this.spectatorCount === 0 && !this.retainSpectatorHistory) {
                        try { this.relay?.close(1000, 'RTC route selected'); } catch {}
                    }
                    this.updateRtcPath().catch(() => {});
                    for (const peer of this.peers.values()) peer.adaptive?.start();
                    // An SCTP association can remain "connected" after its TURN
                    // path stops carrying data. Probe a backed-up input lane
                    // before the confirmed-input watchdog has to end the game.
                    this.rtcHealthTimer = setInterval(() => {
                        for (const peerId of this.peers.keys())
                            this.schedulePeerRecovery(peerId, false, true).catch(() => {});
                    }, 1000);
                } else {
                    for (const peer of this.peers.values()) {
                        peer.adaptive?.close();
                        try { peer.inputDc?.close(); } catch {}
                        try { peer.controlDc?.close(); } catch {}
                        try { peer.pc?.close(); } catch {}
                    }
                    this.peers.clear();
                    try { this.signal?.close(1000, 'relay route selected'); } catch {}
                }
            },
            receiveBinary(data) {
                if (this.closed || this.failed || this.disconnected) return;
                const size = data instanceof Blob ? data.size : data?.byteLength;
                if (!Number.isSafeInteger(size) || size <= 0) return;
                // Include in-flight Blob conversions and check before copying.
                // A suspended simulation must not accumulate unlimited input.
                if (this.received.length - this.receivedHead + this.pendingReceivedPackets >= this.maxReceivedPackets ||
                    size > this.maxReceivedBytes - this.receivedBytes - this.pendingReceivedBytes) {
                    this.failed = true;
                    this.error = 'Peer receive queue limit exceeded';
                    this.close();
                    this.onDisconnect?.();
                    this.onReceive?.();
                    return;
                }
                const enqueue = packet => {
                    if (this.closed || this.failed || this.disconnected) return;
                    this.receivedBytes += packet.byteLength;
                    this.received.push(packet);
                    this.onReceive?.();
                };
                if (data instanceof ArrayBuffer) enqueue(new Uint8Array(data));
                else if (ArrayBuffer.isView(data)) enqueue(new Uint8Array(data.buffer.slice(data.byteOffset, data.byteOffset + data.byteLength)));
                else if (data instanceof Blob) {
                    ++this.pendingReceivedPackets;
                    this.pendingReceivedBytes += size;
                    const release = () => {
                        if (this.closed) return false;
                        --this.pendingReceivedPackets;
                        this.pendingReceivedBytes -= size;
                        return true;
                    };
                    data.arrayBuffer().then(buffer => {
                        if (release()) enqueue(new Uint8Array(buffer));
                    }, release);
                }
            },
            recordChannelEvent(peerId, dc, kind, event, error) {
                const peer = this.peers.get(peerId), pc = peer?.pc;
                const text = value => typeof value === 'string' ? value.slice(0, 512) : null;
                const number = value => Number.isFinite(value) ? value : null;
                this.channelEvents.push({time: new Date().toISOString(), peer: peerId, kind, event,
                    frame: number(globalThis.__eaglerNetplayLanFrame),
                    channel: {label: text(dc.label), readyState: text(dc.readyState), bufferedAmount: number(dc.bufferedAmount)},
                    connection: {connectionState: text(pc?.connectionState), iceConnectionState: text(pc?.iceConnectionState),
                        signalingState: text(pc?.signalingState), sctpState: text(pc?.sctp?.state)},
                    error: error ? {name: text(error.name), message: text(error.message), errorDetail: text(error.errorDetail),
                        sctpCauseCode: number(error.sctpCauseCode), receivedAlert: number(error.receivedAlert), sentAlert: number(error.sentAlert)} : null});
                if (this.channelEvents.length > 8) this.channelEvents.shift();
            },
            setupChannel(peerId, dc, kind) {
                const peer = this.peers.get(peerId);
                if (!peer || (kind !== 'control' && kind !== 'input')) return;
                const primary = peer.primary;
                if (kind === 'control') peer.controlDc = dc;
                else peer.inputDc = dc;
                if (primary) primary[kind + 'Dc'] = dc;
                dc.binaryType = 'arraybuffer';
                // Events queued by a retired channel/peer cannot update its
                // replacement. Primary messages may still arrive during the
                // adaptive handoff's grace window; that route owns admission.
                const current = () => !this.closed && !this.disconnected && this.peers.get(peerId) === peer &&
                    (primary ? primary[kind + 'Dc'] === dc : peer[kind + 'Dc'] === dc);
                const activePrimary = () => current() && (!primary || peer.pc === primary.pc);
                // Route release is broadcast to every endpoint, so one peer
                // can process `route=rtc` a task earlier than another and send
                // its first HELLO/input immediately. Keep RTC packets that
                // arrive while our local route is still undecided, exactly as
                // the WebSocket fallback path does. If the room ultimately
                // selects relay, no conforming peer can have sent RTC gameplay
                // traffic; if it selects RTC, the queued packet is already in
                // order for DrainPackets().
                dc.onmessage = event => {
                    if (!current()) return;
                    if (peer.adaptive) { peer.adaptive.receivePrimary(kind, event.data); return; }
                    this.receivePeerData(peer, event.data);
                };
                dc.onopen = () => {
                    if (!activePrimary()) return;
                    if (kind === 'control') peer.controlOpen = true;
                    else peer.inputOpen = true;
                    peer.adaptive?.opened(kind);
                    this.maybeReportRtcReady();
                };
                dc.onclose = () => {
                    if (!activePrimary()) return;
                    this.recordChannelEvent(peerId, dc, kind, 'close');
                    if (kind === 'control') peer.controlOpen = false;
                    else peer.inputOpen = false;
                    if (!this.route) this.sendSignal({ type: 'rtc-failed' });
                    else if (this.route === 'rtc' && !this.closed && !peer.adaptive?.recover()) this.fail(`RTC peer P${peerId + 1} ${kind} channel closed`);
                };
                dc.onerror = event => {
                    if (!activePrimary()) return;
                    this.recordChannelEvent(peerId, dc, kind, 'error', event?.error);
                    if (!this.route) this.sendSignal({ type: 'rtc-failed' });
                    else if (this.route === 'rtc' && !peer.adaptive?.recover()) this.fail(`RTC peer P${peerId + 1} ${kind} channel error`);
                };
            },
            receivePeerData(peer, data) {
                peer.lastReceivedAt = Date.now();
                if ((peer.restartInFlight && peer.lastReceivedAt > peer.restartObservedAt) ||
                    (!peer.restartInFlight && (peer.recoveryPending || peer.recoveryTimer))) {
                    this.clearPeerRecovery(peer);
                    peer.restartAttempts = 0;
                }
                if (!this.route || this.route === 'rtc') this.receiveBinary(data);
            },
            maybeReportRtcReady() {
                if (this.rtcReadySent || this.peers.size !== this.playerCount - 1) return;
                if (![...this.peers.values()].every(peer => peer.inputOpen && peer.controlOpen)) return;
                this.rtcReadySent = true;
                this.sendSignal({ type: 'rtc-ready' });
            },
            async peerProgressSnapshot(peer) {
                let traffic = null;
                try {
                    const stats = await peer.pc.getStats();
                    let selected = null;
                    for (const report of stats.values()) {
                        if (report.type === 'transport' && report.selectedCandidatePairId) {
                            selected = stats.get(report.selectedCandidatePairId);
                            if (selected) break;
                        }
                    }
                    for (const report of stats.values()) {
                        if (!selected && report.type === 'candidate-pair' && report.nominated && report.state === 'succeeded') selected = report;
                    }
                    if (selected) {
                        const report = selected;
                        const received = Number(report.bytesReceived);
                        // Outbound bytes can keep increasing while the peer is
                        // unreachable, so only inbound traffic proves progress.
                        if (Number.isFinite(received)) traffic = received;
                    }
                } catch {}
                const confirmed = Number(globalThis.__eaglerNetplayLanConfirmed);
                return { traffic, confirmed: Number.isFinite(confirmed) ? confirmed : null };
            },
            peerProgressed(before, after) {
                return (before.traffic != null && after.traffic != null && after.traffic > before.traffic) ||
                    (before.confirmed != null && after.confirmed != null && after.confirmed > before.confirmed);
            },
            clearPeerRecovery(peer) {
                if (peer.recoveryTimer) clearTimeout(peer.recoveryTimer);
                if (peer.restartWatchdog) clearTimeout(peer.restartWatchdog);
                peer.recoveryTimer = null;
                peer.restartWatchdog = null;
                peer.restartInFlight = false;
                peer.recoveryPending = false;
            },
            async schedulePeerRecovery(peerId, immediate = false, silentStall = false) {
                const peer = this.peers.get(peerId);
                if (!peer || this.closed || this.disconnected || this.route !== 'rtc' || peer.recoveryTimer || peer.restartInFlight) return;
                const failed = peer.pc.connectionState === 'failed' || peer.pc.iceConnectionState === 'failed';
                const disconnected = peer.pc.connectionState === 'disconnected' || peer.pc.iceConnectionState === 'disconnected';
                const stalled = silentStall && peer.lastInputSentAt &&
                    Date.now() - peer.lastReceivedAt >= 3000 &&
                    (globalThis.__eaglerNetplayLanActive === true ||
                     Number(peer.inputDc?.bufferedAmount || 0) + Number(peer.controlDc?.bufferedAmount || 0) >= 32768);
                if (!failed && !disconnected && !stalled) return;
                if (peer.adaptive?.recover()) return;
                peer.recoveryPending = true;
                if (failed || immediate) {
                    this.requestPeerIceRestart(peerId);
                    return;
                }
                const receivedAt = peer.lastReceivedAt;
                const before = silentStall ? null : await this.peerProgressSnapshot(peer);
                if (this.closed || this.route !== 'rtc' || this.peers.get(peerId) !== peer) return;
                peer.recoveryTimer = setTimeout(async () => {
                    peer.recoveryTimer = null;
                    const stillDisconnected = peer.pc.connectionState === 'disconnected' || peer.pc.iceConnectionState === 'disconnected';
                    const nowFailed = peer.pc.connectionState === 'failed' || peer.pc.iceConnectionState === 'failed';
                    const stillStalled = stalled && peer.lastInputSentAt &&
                        Date.now() - peer.lastReceivedAt >= 3000 &&
                        (globalThis.__eaglerNetplayLanActive === true ||
                         Number(peer.inputDc?.bufferedAmount || 0) + Number(peer.controlDc?.bufferedAmount || 0) >= 32768);
                    if (!stillDisconnected && !nowFailed && !stillStalled) {this.clearPeerRecovery(peer);return;}
                    const after = silentStall ? null : await this.peerProgressSnapshot(peer);
                    const progressed = silentStall ? peer.lastReceivedAt > receivedAt :
                        this.peerProgressed(before, after);
                    if (!nowFailed && progressed) {
                        this.schedulePeerRecovery(peerId, false, silentStall).catch(() => {});
                        return;
                    }
                    this.requestPeerIceRestart(peerId);
                }, 2500);
            },
            requestPeerIceRestart(peerId) {
                const peer = this.peers.get(peerId);
                if (!peer || this.closed || this.disconnected || this.route !== 'rtc' || peer.restartInFlight) return;
                if (peer.primary && peer.pc !== peer.primary.pc) {
                    if (!peer.adaptive?.recover()) this.fail(`RTC peer P${peerId + 1} TURN path unavailable`);
                    return;
                }
                if (this.localPlayer < peerId) {
                    this.restartPeerIce(peerId).catch(error => {
                        peer.restartInFlight = false;
                        this.fail(`RTC peer P${peerId + 1} ICE restart error: ${error}`);
                    });
                } else {
                    if(peer.restartAttempts>=2){this.fail(`RTC peer P${peerId + 1} ICE restart exhausted`);return;}
                    peer.restartAttempts+=1;
                    peer.restartInFlight = true;
                    peer.restartObservedAt = peer.lastReceivedAt;
                    this.sendSignal({ type: 'ice-restart-request', to: peerId });
                    peer.restartWatchdog = setTimeout(() => {
                        peer.restartWatchdog = null;
                        peer.restartInFlight = false;
                        this.schedulePeerRecovery(peerId, true, true).catch(() => {});
                    }, 7000);
                }
            },
            async restartPeerIce(peerId) {
                const peer = this.peers.get(peerId);
                if (!peer || this.closed || this.disconnected || this.route !== 'rtc' || peer.restartInFlight) return;
                if (peer.primary && peer.pc !== peer.primary.pc) return;
                if (peer.restartAttempts >= 2) {
                    this.fail(`RTC peer P${peerId + 1} ICE restart exhausted`);
                    return;
                }
                peer.restartInFlight = true;
                peer.restartObservedAt = peer.lastReceivedAt;
                peer.restartAttempts += 1;
                this.openSignal();
                if (typeof peer.pc.restartIce === 'function') peer.pc.restartIce();
                const offer = await peer.pc.createOffer({ iceRestart: true });
                await peer.pc.setLocalDescription(offer);
                this.sendSignal({ type: 'signal', to: peerId, description: peer.pc.localDescription });
                peer.restartWatchdog = setTimeout(() => {
                    peer.restartWatchdog = null;
                    peer.restartInFlight = false;
                    const healthy = peer.pc.connectionState === 'connected' || peer.pc.iceConnectionState === 'connected' || peer.pc.iceConnectionState === 'completed';
                    if (healthy && peer.lastReceivedAt > peer.restartObservedAt) return;
                    this.schedulePeerRecovery(peerId, true, true).catch(() => {});
                }, 7000);
            },
            handlePeerConnectionState(peerId) {
                const peer = this.peers.get(peerId);
                if (!peer || this.closed) return;
                const healthy = peer.pc.connectionState === 'connected' || peer.pc.iceConnectionState === 'connected' || peer.pc.iceConnectionState === 'completed';
                if (healthy) {
                    // ICE state alone cannot clear a recovery attempt: the
                    // data channels must receive a packet on the new path.
                    if (peer.restartInFlight && peer.lastReceivedAt <= peer.restartObservedAt) return;
                    this.clearPeerRecovery(peer);
                    peer.restartAttempts = 0;
                    this.pendingSignals = this.pendingSignals.filter(message => {
                        try {
                            const payload = JSON.parse(message);
                            return payload.type !== 'ice-restart-request' || Number(payload.to) !== peerId;
                        } catch { return true; }
                    });
                    if (this.route === 'rtc') this.updateRtcPath().catch(() => {});
                    return;
                }
                if (!this.route && (peer.pc.connectionState === 'failed' || peer.pc.iceConnectionState === 'failed')) {
                    this.sendSignal({ type: 'rtc-failed' });
                    return;
                }
                this.schedulePeerRecovery(peerId).catch(() => {});
            },
            async selectedCandidateSummary(pc) {
                const stats = await pc.getStats();
                let pair = null;
                for (const report of stats.values()) {
                    if (report.type === 'transport' && report.selectedCandidatePairId)
                        pair = stats.get(report.selectedCandidatePairId);
                }
                if (!pair) {
                    for (const report of stats.values()) {
                        if (report.type === 'candidate-pair' && report.state === 'succeeded' && report.nominated) {
                            pair = report;
                            break;
                        }
                    }
                }
                if (!pair) return null;
                const local = stats.get(pair.localCandidateId);
                const remote = stats.get(pair.remoteCandidateId);
                const turn = local?.candidateType === 'relay' || remote?.candidateType === 'relay';
                const address = String(local?.address || local?.ip || remote?.address || remote?.ip || String());
                const ipv4Parts = address.split('.');
                const family = address.includes(':') ? 'IPv6' :
                    ipv4Parts.length === 4 && ipv4Parts.every(part => part.length && Number.isInteger(Number(part)) && Number(part) >= 0 && Number(part) <= 255) ? 'IPv4' : null;
                return { path: turn ? 'turn' : 'direct', protocol: local?.protocol || null, family };
            },
            async updateRtcPath() {
                const summaries = [];
                for (const [peerId, peer] of this.peers) {
                    const summary = await this.selectedCandidateSummary(peer.pc).catch(() => null);
                    if (summary) {
                        if (peer.pc === peer.primary?.pc) peer.primary.path = summary.path;
                        summaries.push({ peer: peerId, ...summary });
                    }
                }
                globalThis.__eaglerNetplayRtcPaths = summaries;
                const paths = new Set(summaries.map(summary => summary.path));
                globalThis.__eaglerNetplayPath = paths.size === 1 ? [...paths][0] : paths.size > 1 ? 'mixed' : 'rtc';
            },
            async ensurePeer(peerId) {
                if (peerId === this.localPlayer || this.peers.has(peerId) || typeof RTCPeerConnection !== 'function') return;
                const peer = {
                    pc: new RTCPeerConnection({ iceServers: this.iceServers, iceTransportPolicy: this.iceTransportPolicy }),
                    inputDc: null, controlDc: null, inputOpen: false, controlOpen: false,
                    pendingCandidates: [], recoveryTimer: null, restartWatchdog: null,
                    restartInFlight: false, restartAttempts: 0, recoveryPending: false,
                    lastReceivedAt: Date.now(), lastInputSentAt: 0, restartObservedAt: 0
                };
                this.peers.set(peerId, peer);
                peer.primary = {pc: peer.pc, inputDc: null, controlDc: null, path: null};
                peer.adaptive = createAdaptivePeerRoute({
                    primary: peer.primary, localPlayer: this.localPlayer, peerId, tag: this.tag,
                    enabled: Module.eaglerOptions?.netplayAdaptiveTurn !== false && this.iceTransportPolicy !== 'relay',
                    iceServers: () => this.iceServers,
                    alive: () => !this.closed && !this.failed && !this.disconnected && this.route !== 'relay',
                    running: () => this.route === 'rtc' && globalThis.__eaglerNetplayLanActive === true,
                    visible: () => !globalThis.document?.hidden && !globalThis.__eaglerNetplayCalibrationSuspended,
                    receive: data => this.receivePeerData(peer, data),
                    failed: message => this.fail(message),
                    selected: path => {
                        this.clearPeerRecovery(peer);
                        peer.pc = path.pc; peer.inputDc = path.inputDc; peer.controlDc = path.controlDc;
                        peer.inputOpen = peer.controlOpen = true;
                        peer.restartAttempts = 0;
                        this.updateRtcPath().catch(() => {});
                    },
                    changed: diagnostic => {
                        globalThis.__eaglerNetplayRouteSelection ||= {};
                        globalThis.__eaglerNetplayRouteSelection[peerId] = diagnostic;
                    },
                });
                const primaryPc = peer.pc;
                peer.pc.onicecandidate = event => {
                    if (event.candidate) this.sendSignal({ type: 'signal', to: peerId, candidate: event.candidate.toJSON ? event.candidate.toJSON() : event.candidate });
                };
                peer.pc.onconnectionstatechange = () => { if (peer.pc === primaryPc) this.handlePeerConnectionState(peerId); };
                peer.pc.oniceconnectionstatechange = () => { if (peer.pc === primaryPc) this.handlePeerConnectionState(peerId); };
                if (this.localPlayer > peerId) {
                    peer.pc.ondatachannel = event => {
                        if (event.channel.label === this.controlLabel) this.setupChannel(peerId, event.channel, 'control');
                        else if (event.channel.label === this.inputLabel) this.setupChannel(peerId, event.channel, 'input');
                    };
                    return;
                }
                const controlDc = peer.pc.createDataChannel(this.controlLabel, { ordered: true });
                const inputDc = peer.pc.createDataChannel(this.inputLabel, { ordered: false, maxRetransmits: 0 });
                this.setupChannel(peerId, controlDc, 'control');
                this.setupChannel(peerId, inputDc, 'input');
                const offer = await peer.pc.createOffer();
                await peer.pc.setLocalDescription(offer);
                this.sendSignal({ type: 'signal', to: peerId, description: peer.pc.localDescription });
            },
            async handleSignal(message) {
                const peerId = Number(message.from);
                if (!Number.isInteger(peerId) || peerId < 0 || peerId >= this.playerCount || peerId === this.localPlayer) return;
                await this.ensurePeer(peerId);
                const peer = this.peers.get(peerId);
                if (!peer) return;
                const path = peer.primary || peer;
                if (message.description) {
                    await path.pc.setRemoteDescription(message.description);
                    while (peer.pendingCandidates.length) await path.pc.addIceCandidate(peer.pendingCandidates.shift());
                    if (message.description.type === 'offer') {
                        const answer = await path.pc.createAnswer();
                        await path.pc.setLocalDescription(answer);
                        this.sendSignal({ type: 'signal', to: peerId, description: path.pc.localDescription });
                    }
                }
                if (message.candidate) {
                    if (path.pc.remoteDescription) await path.pc.addIceCandidate(message.candidate);
                    else peer.pendingCandidates.push(message.candidate);
                }
            },
            scheduleSignalReconnect() {
                if (this.closed || this.disconnected || this.route !== 'rtc' || this.signalReconnectTimer) return;
                const delay = this.signalReconnectDelayMs;
                this.signalReconnectDelayMs = Math.min(5000, delay * 2);
                this.signalReconnectTimer = setTimeout(() => {
                    this.signalReconnectTimer = null;
                    this.openSignal();
                }, delay);
            },
            openSignal() {
                if (this.closed || this.disconnected || this.route === 'relay' || this.signal?.readyState === WebSocket.OPEN || this.signal?.readyState === WebSocket.CONNECTING) return;
                const socket = new WebSocket(signalUrl.href);
                this.signal = socket;
                socket.onopen = () => {
                    if (this.signal !== socket || this.closed) return;
                    this.signalReconnectDelayMs = 500;
                    const pending = this.pendingSignals.splice(0);
                    for (const message of pending) socket.send(message);
                    if (typeof RTCPeerConnection !== 'function') this.sendSignal({ type: 'rtc-failed' });
                };
                socket.onmessage = event => {
                    let message;
                    try { message = JSON.parse(String(event.data)); } catch { return; }
                    if (message.type === 'peers') {
                        if (Array.isArray(message.iceServers)) this.iceServers = message.iceServers;
                        for (const peer of message.peers || []) this.ensurePeer(Number(peer)).catch(error => this.sendSignal({ type: 'rtc-failed', error: String(error) }));
                        if (message.route) this.setRoute(message.route);
                    } else if (message.type === 'peer-join') {
                        this.ensurePeer(Number(message.player)).catch(error => this.sendSignal({ type: 'rtc-failed', error: String(error) }));
                    } else if (message.type === 'signal') {
                        this.handleSignal(message).catch(error => {
                            if (!this.route) this.sendSignal({ type: 'rtc-failed', error: String(error) });
                            else this.fail(`RTC signaling error: ${error}`);
                        });
                    } else if (message.type === 'ice-restart-request') {
                        const peerId = Number(message.from);
                        if (Number.isInteger(peerId) && this.localPlayer < peerId)
                            this.restartPeerIce(peerId).catch(error => this.fail(`RTC peer P${peerId + 1} ICE restart error: ${error}`));
                    } else if (message.type === 'route') {
                        this.setRoute(message.mode);
                    } else if (message.type === 'peer-disconnected' && this.route === 'rtc') {
                        const peerId=Number(message.from);
                        if(Number.isInteger(peerId) && peerId>=0 && peerId<this.playerCount && peerId!==this.localPlayer)
                            this.fail(`RTC peer P${peerId + 1} disconnected`,false);
                    }
                };
                socket.onerror = () => {
                    if (!this.route && this.relay?.readyState !== WebSocket.OPEN) this.fail('WebRTC signaling unavailable', false);
                };
                socket.onclose = () => {
                    if (this.signal !== socket) return;
                    this.signal = null;
                    if (this.closed) return;
                    if (this.route === 'rtc') this.scheduleSignalReconnect();
                    else if (!this.route && this.relay?.readyState !== WebSocket.OPEN) this.fail('WebRTC signaling unavailable', false);
                };
            },
            openRelay() {
                if(this.closed || this.disconnected)return;
                const socket=new WebSocket(relay.href);
                this.relay=socket;socket.binaryType='arraybuffer';
                socket.onopen=()=>{
                    if(this.closed || this.relay!==socket)return;
                    this.onReceive?.();
                };
                socket.onmessage=event=>{
                    if(this.closed || this.relay!==socket)return;
                    if(typeof event.data==='string'){
                        let message;try{message=JSON.parse(event.data);}catch{return;}
                        if(message?.type==='route' && (message.mode==='rtc'||message.mode==='relay'||message.mode==='failed'))this.setRoute(message.mode);
                    }else if(!this.route || this.route==='relay')this.receiveBinary(event.data);
                };
                socket.onerror=()=>{if(this.route==='relay' && this.relay===socket)this.fail('WebSocket relay error');};
                socket.onclose=event=>{
                    if(this.closed || this.relay!==socket || this.route==='rtc')return;
                    if(!this.route && this.signal?.readyState!==WebSocket.OPEN) {
                        this.fail('WebRTC connection unavailable', false);
                        return;
                    }
                    if(this.route!=='relay')return;
                    // The relay retires a run when its stream closes. Reusing
                    // that run would invent missing history; offer room return.
                    this.fail(event.reason||'WebSocket relay session ended');
                };
            },
            close() {
                this.closed = true;
                if (this.signalReconnectTimer) clearTimeout(this.signalReconnectTimer);
                if (this.rtcHealthTimer) clearInterval(this.rtcHealthTimer);
                try { this.signal?.close(1000, 'transport close'); } catch {}
                try { this.relay?.close(1000, 'transport close'); } catch {}
                for (const peer of this.peers.values()) {
                    peer.adaptive?.close();
                    this.clearPeerRecovery(peer);
                    try { peer.inputDc?.close(); } catch {}
                    try { peer.controlDc?.close(); } catch {}
                    try { peer.pc?.close(); } catch {}
                    try { peer.primary?.pc?.close(); } catch {}
                }
                this.peers.clear();
                this.received.length = 0;
                this.receivedHead = 0;
                this.receivedBytes = this.pendingReceivedBytes = this.pendingReceivedPackets = 0;
            },
        };
        globalThis.__eaglerPeerTransport = state;
        globalThis[legacyGlobal] = state;
        globalThis.__eaglerNetplayTransport = 'connecting';
        globalThis.__eaglerNetplayPath = 'connecting';
        globalThis.__eaglerNetplayRtcPaths = [];
        globalThis.__eaglerNetplayRouteSelection = {};

        // Once the server releases the relay barrier, one peer may process the
        // route message a task earlier than another. Buffer packets that arrive
        // while our local route is still undecided; no sender can legally send
        // before the server has selected relay, so these are safe to retain.
        state.openRelay();
        state.openSignal();
        return 1;
    } catch (error) {
        const failed = { failed: true, error: String(error), close() {}, legacyGlobal };
        globalThis.__eaglerPeerTransport = failed;
        globalThis[legacyGlobal] = failed;
        return 0;
    }
});

EM_JS(int, eagler_spectator_connect,
      (const char *relayUrlPtr, const char *spectatorIdPtr, const char *tagPtr, int playerCount, int stageSpectatorHistory), {
    const decode = ptr => {
        const end = HEAPU8.indexOf(0, ptr);
        return new TextDecoder().decode(HEAPU8.subarray(ptr, end >= 0 ? end : HEAPU8.length));
    };
    const tag = decode(tagPtr);
    if (!/^[A-Za-z0-9_-]{1,32}$/.test(tag)) return 0;
    const legacyGlobal = `__${tag}PeerTransport`;
    try {
        if (globalThis.__eaglerPeerTransport?.close) globalThis.__eaglerPeerTransport.close();
        const relay = new URL(decode(relayUrlPtr), location.href);
        relay.searchParams.delete('player');
        relay.searchParams.delete('signal');
        relay.searchParams.set('spectator', decode(spectatorIdPtr));
        relay.searchParams.set('players', String(playerCount));
        const state = {
            relay: null, peers: new Map(), received: [], receivedHead: 0,
            playerCount, tag, legacyGlobal, route: 'spectator', failed: false, error: String(), closed: false,
            maxReceivedPackets: stageSpectatorHistory ? 131072 : 16384,
            // A complete stage can exceed the legacy packet count even while
            // remaining comfortably inside its byte budget. Keep both bounded.
            maxReceivedBytes: stageSpectatorHistory ? 16 * 1024 * 1024 : Infinity,
            receivedBytes: 0, pendingReceivedBytes: 0, pendingReceivedPackets: 0,
            receiveEpoch: 0, reconnectAttempt: 0, reconnectTimer: null, connectTimer: null,
            close() {
                this.closed = true;
                if (this.reconnectTimer !== null) clearTimeout(this.reconnectTimer);
                if (this.connectTimer !== null) clearTimeout(this.connectTimer);
                this.reconnectTimer = null;
                try { this.relay?.close(1000, 'spectator close'); } catch {}
                this.received.length = 0;
                this.receivedHead = 0;
                this.receivedBytes = this.pendingReceivedBytes = this.pendingReceivedPackets = 0;
            },
            receiveBinary(data) {
                if (!stageSpectatorHistory) {
                    // Keep the legacy lane (including TH09) on its existing
                    // packet admission and asynchronous Blob behavior.
                    const append = packet => {
                        if (this.closed || this.failed) return;
                        if (this.received.length - this.receivedHead >= this.maxReceivedPackets) {
                            this.failed = true; this.error = 'Spectator fell too far behind';
                            try {this.relay?.close(1008, 'spectator fell too far behind');} catch {}
                            return;
                        }
                        this.received.push(packet); this.receivedBytes += packet.byteLength;
                    };
                    if (data instanceof ArrayBuffer) append(new Uint8Array(data));
                    else if (data instanceof Blob) data.arrayBuffer().then(buffer => append(new Uint8Array(buffer))).catch(() => {});
                    return;
                }
                if (this.closed || this.failed) return;
                const epoch = this.receiveEpoch;
                const bytes = data instanceof Blob ? data.size : data?.byteLength;
                if (!Number.isSafeInteger(bytes) || bytes <= 0) return;
                if (this.received.length - this.receivedHead + this.pendingReceivedPackets >= this.maxReceivedPackets ||
                    bytes > this.maxReceivedBytes - this.receivedBytes - this.pendingReceivedBytes) {
                    this.failed = true; this.error = 'Spectator fell too far behind';
                    try {this.relay?.close(1008, this.error);} catch {}
                    return;
                }
                const append = packet => {
                    if (this.closed || this.failed || this.receiveEpoch !== epoch) return;
                    if (this.received.length - this.receivedHead >= this.maxReceivedPackets) {
                        this.failed = true;
                        this.error = 'Spectator fell too far behind';
                        try { this.relay?.close(1008, 'spectator fell too far behind'); } catch {}
                        return;
                    }
                    this.received.push(packet);
                    this.receivedBytes += packet.byteLength;
                };
                if (data instanceof ArrayBuffer) append(new Uint8Array(data));
                else if (data instanceof Blob) {
                    ++this.pendingReceivedPackets; this.pendingReceivedBytes += bytes;
                    const release = () => {
                        if (this.closed || this.receiveEpoch !== epoch) return false;
                        --this.pendingReceivedPackets; this.pendingReceivedBytes -= bytes; return true;
                    };
                    data.arrayBuffer().then(buffer => {if (release()) append(new Uint8Array(buffer));}, release);
                }
            },
        };
        globalThis.__eaglerPeerTransport = state;
        globalThis[legacyGlobal] = state;
        globalThis.__eaglerNetplayTransport = 'spectator';
        globalThis.__eaglerNetplayPath = 'relay';
        const open = () => {
            if (state.closed) return;
            const socket = state.relay = new WebSocket(relay.href);
            socket.binaryType = 'arraybuffer';
            if (stageSpectatorHistory && socket.readyState === WebSocket.CONNECTING)
                state.connectTimer = setTimeout(() => {if (state.relay === socket && socket.readyState === WebSocket.CONNECTING) socket.close();}, 8000);
            socket.onopen = () => {if (state.relay === socket && state.connectTimer !== null) {clearTimeout(state.connectTimer);state.connectTimer = null;}};
            socket.onmessage = event => {
                if (state.closed || state.relay !== socket) return;
                if(stageSpectatorHistory&&event.data instanceof ArrayBuffer){
                    const wire=new Uint8Array(event.data);
                    if(wire[0]===0xe9){
                        if(wire.length<=20||wire.length>32788||wire[1]!==1||wire[2]>1||wire[3])return;
                        const v=new DataView(wire.buffer),total=v.getUint32(12,true),offset=v.getUint32(16,true);
                        const session=v.getUint32(8,true).toString(16).padStart(8,'0')+v.getUint32(4,true).toString(16).padStart(8,'0');
                        if(session!==state.spectatorSession?.session||!total||total>8388608||offset>total||wire.length-20>total-offset)return;
                        if(!state.replayReceiving){if(offset)return;state.replayReceiving={session,bytes:new Uint8Array(total),offset:0,extended:!!wire[2]};}
                        const file=state.replayReceiving;
                        if(file.session!==session||file.offset!==offset||file.bytes.length!==total||file.extended!==!!wire[2])return;
                        file.bytes.set(wire.subarray(20),offset);file.offset+=wire.length-20;
                        if(file.offset===total){
                            const prefix=({th06:'th6',th07:'th7',th08:'th8',th10:'th10',th11:'th11',th15:'th15mp'})[tag]||tag;
                            const suffix=((parseInt(session.slice(0,8),16)^parseInt(session.slice(8),16))>>>0).toString(36).slice(-4).padStart(4,'0');
                            state.spectatorReplay={session,bytes:file.bytes,name:prefix+'_ud'+suffix+(file.extended?'.rpyx':'.rpy')};state.replayReceiving=null;
                        }
                        return;
                    }
                }
                if (typeof event.data !== 'string') { state.receiveBinary(event.data); return; }
                if (!stageSpectatorHistory) return;
                let message; try { message = JSON.parse(event.data); } catch { return; }
                if (message.type !== 'spectator-session') return;
                if (typeof message.session !== 'string' || !/^[a-f0-9]{16}$/.test(message.session) || /^0+$/.test(message.session) ||
                    !Number.isInteger(message.seed) || message.seed < 0 || message.seed > 65535 ||
                    !Number.isSafeInteger(message.generation) || message.generation < 0) return;
                ++state.receiveEpoch;
                state.received = []; state.receivedHead = 0;
                state.receivedBytes = state.pendingReceivedBytes = state.pendingReceivedPackets = 0;
                state.pendingSpectatorSession = message;
                state.spectatorSession = message;
                state.replayReceiving=null;state.spectatorReplay=null;
                state.failed = false; state.error = '';
            };
            socket.onerror = () => {
                if (!stageSpectatorHistory && !state.closed && state.relay === socket) {
                    state.failed = true; state.error = 'Spectator relay error';
                }
            };
            socket.onclose = event => {
                if (state.closed || state.relay !== socket) return;
                if (state.connectTimer !== null) {clearTimeout(state.connectTimer);state.connectTimer = null;}
                if (stageSpectatorHistory && !state.failed &&
                    [1006, 1012, 1013].includes(event.code) && state.reconnectAttempt < 3) {
                    state.reconnectTimer = setTimeout(() => {state.reconnectTimer = null; open();},
                        500 * 2 ** state.reconnectAttempt++);
                    return;
                }
                state.failed = true; state.error = event.reason || 'Spectator relay closed';
            };
        };
        open();
        return 1;
    } catch (error) {
        const failed = { failed: true, error: String(error), close() {}, legacyGlobal };
        globalThis.__eaglerPeerTransport = failed;
        globalThis[legacyGlobal] = failed;
        return 0;
    }
});

EM_JS(void, eagler_peer_close, (), {
    const state = globalThis.__eaglerPeerTransport;
    const legacyGlobal = state?.legacyGlobal;
    try { state?.close?.(); } catch {}
    globalThis.__eaglerPeerTransport = null;
    if (legacyGlobal) globalThis[legacyGlobal] = null;
});

EM_JS(int, eagler_peer_is_open, (), {
    const state = globalThis.__eaglerPeerTransport;
    if (!state || state.failed || state.disconnected || !state.route) return 0;
    if (state.route === 'relay' || state.route === 'spectator') return state.relay?.readyState === WebSocket.OPEN ? 1 : 0;
    if (state.route === 'rtc') return [...state.peers.values()].every(peer => peer.inputOpen && peer.controlOpen) ? 1 : 0;
    return 0;
});

EM_JS(int, eagler_peer_failed, (), { return globalThis.__eaglerPeerTransport?.failed ? 1 : 0; });
EM_JS(int, eagler_peer_recovering, (), { return globalThis.__eaglerPeerTransport?.isRecovering?.() ? 1 : 0; });
EM_JS(int, eagler_peer_disconnected, (), { return globalThis.__eaglerPeerTransport?.disconnected ? 1 : 0; });
EM_JS(int, eagler_calibration_suspended, (), { return globalThis.__eaglerNetplayCalibrationSuspended ? 1 : 0; });

EM_JS(int, eagler_peer_send, (const unsigned char *data, int size, int control), {
    const state = globalThis.__eaglerPeerTransport;
    if (!state || state.failed || state.disconnected || size <= 0) return 0;
    const payload = HEAPU8.slice(data, data + size);
    try {
        if (state.route === 'relay') {
            if (state.relay?.readyState !== WebSocket.OPEN) return 0;
            state.relay.send(payload);
            return 1;
        }
        if (state.route === 'rtc') {
            for (const peer of state.peers.values()) {
                const dc = control ? peer.controlDc : peer.inputDc;
                if (dc?.readyState !== 'open' || peer.adaptive && !peer.adaptive.canSend(!!control, payload.byteLength)) return 0;
            }
            for (const peer of state.peers.values()) {
                if (peer.adaptive) { if (!peer.adaptive.send(payload, !!control)) return 0; }
                else (control ? peer.controlDc : peer.inputDc).send(payload);
                if (!control) peer.lastInputSentAt = Date.now();
            }
            return 1;
        }
    } catch (error) {
        state.fail(String(error));
    }
    return 0;
});

EM_JS(int, eagler_peer_send_to, (int peerId, const unsigned char *data, int size), {
    const state = globalThis.__eaglerPeerTransport;
    if (!state || state.failed || state.disconnected || size <= 0 || !Number.isInteger(peerId) ||
        peerId < 0 || peerId >= state.playerCount || peerId === state.localPlayer) return 0;
    const payload = HEAPU8.slice(data, data + size);
    try {
        if (state.route === 'relay') {
            if (state.relay?.readyState !== WebSocket.OPEN) return 0;
            const envelope = new Uint8Array(payload.byteLength + 2);
            envelope[0] = 0xe7;
            envelope[1] = peerId;
            envelope.set(payload, 2);
            state.relay.send(envelope);
            return 1;
        }
        if (state.route === 'rtc') {
            const peer = state.peers.get(peerId), dc = peer?.inputDc;
            if (dc?.readyState !== 'open') return 0;
            if (peer.adaptive) { if (!peer.adaptive.send(payload, false)) return 0; }
            else dc.send(payload);
            state.peers.get(peerId).lastInputSentAt = Date.now();
            return 1;
        }
    } catch (error) {
        state.fail(String(error));
    }
    return 0;
});

EM_JS(int, eagler_peer_send_repair_to, (int peerId, const unsigned char *data, int size), {
    const state = globalThis.__eaglerPeerTransport;
    if (!state || state.failed || state.disconnected || state.closed || state.route !== 'rtc' ||
        !Number.isInteger(peerId) || peerId < 0 || peerId >= state.playerCount ||
        peerId === state.localPlayer || size <= 0 || size > 32768) return 0;
    const channel = state.peers.get(peerId)?.controlDc;
    // Session/control traffic has priority. Do not create unbounded reliable
    // head-of-line work when an RTC association is already unhealthy.
    if (channel?.readyState !== 'open' || channel.bufferedAmount > 32768) return 0;
    try {
        const peer = state.peers.get(peerId), payload = HEAPU8.slice(data, data + size);
        if (peer.adaptive) { if (!peer.adaptive.send(payload, true)) return 0; }
        else channel.send(payload);
        state.inputRepairSent = (state.inputRepairSent || 0) + 1;
        return 1;
    } catch {
        // The ordinary input/control health checks retain failure ownership.
        return 0;
    }
});

EM_JS(int, eagler_peer_send_spectator, (const unsigned char *data, int size), {
    const state = globalThis.__eaglerPeerTransport;
    if (!state || state.closed || state.spectatorStopped || size <= 0 ||
        state.relay?.readyState !== WebSocket.OPEN || (state.flushSpectatorSession && !state.flushSpectatorSession())) return 0;
    // Do not move an unbounded local backlog into the browser's socket queue.
    // A single large title packet is allowed only on an empty queue; subsequent
    // packets wait until it drains. TH09's 40/46-byte packets stay below 64 KiB.
    const queued = Number(state.relay.bufferedAmount || 0);
    if (queued > 0 && queued + size + 1 > 65536) return 0;
    try {
        const payload = HEAPU8.slice(data, data + size);
        const envelope = new Uint8Array(payload.byteLength + 1);
        envelope[0] = 0xe8;
        envelope.set(payload, 1);
        state.relay.send(envelope);
        return 1;
    } catch { return 0; }
});

EM_JS(void, eagler_peer_begin_spectator_session, (unsigned low, unsigned high, unsigned seed), {
    const state = globalThis.__eaglerPeerTransport;
    if (!state || state.closed || !state.retainSpectatorHistory || seed > 65535) return;
    const session = (high >>> 0).toString(16).padStart(8, '0') + (low >>> 0).toString(16).padStart(8, '0');
    if (state.spectatorSession?.session === session && state.spectatorSession.seed === seed) return;
    const generation = state.spectatorSession ? state.spectatorSession.generation + 1 : 0;
    state.spectatorSession = state.pendingSpectatorSession = {type: 'spectator-session', session, seed, generation};
    state.spectatorStopped = false;
    if(state.replayTimer)clearTimeout(state.replayTimer);state.replayTimer=null;state.sharedReplaySession=null;
    state.flushSpectatorSession();
});

EM_JS(void, eagler_peer_share_replay, (const unsigned char* data,int size,int extended), {
    const state=globalThis.__eaglerPeerTransport,session=state?.spectatorSession?.session;
    if(!state||state.closed||!state.retainSpectatorHistory||state.localPlayer!==0||!session||size<=0||size>8388608||state.sharedReplaySession===session)return;
    const bytes=HEAPU8.slice(data,data+size),deadline=Date.now()+15000;let offset=0;
    state.sharedReplaySession=session;
    const flush=()=>{
        state.replayTimer=null;
        if(state.closed||state.spectatorSession?.session!==session||Date.now()>deadline||offset===bytes.length)return;
        if(state.relay?.readyState===WebSocket.OPEN&&state.relay.bufferedAmount<=32768&&(!state.flushSpectatorSession||state.flushSpectatorSession())){
            const part=bytes.subarray(offset,offset+32768),wire=new Uint8Array(20+part.length),v=new DataView(wire.buffer);
            wire.set([0xe9,1,extended?1:0,0]);v.setUint32(4,parseInt(session.slice(8),16),true);v.setUint32(8,parseInt(session.slice(0,8),16),true);
            v.setUint32(12,size,true);v.setUint32(16,offset,true);wire.set(part,20);
            try{state.relay.send(wire);offset+=part.length;}catch{return;}
        }
        state.replayTimer=setTimeout(flush,16);
    };state.replayTimer=setTimeout(flush,0);
});

EM_JS(int, eagler_peer_take_spectator_session, (unsigned *words), {
    const state = globalThis.__eaglerPeerTransport, session = state?.pendingSpectatorSession;
    if (state?.route !== 'spectator' || !session) return 0;
    HEAPU32[words >>> 2] = parseInt(session.session.slice(8), 16);
    HEAPU32[(words >>> 2) + 1] = parseInt(session.session.slice(0, 8), 16);
    HEAPU32[(words >>> 2) + 2] = session.seed;
    state.pendingSpectatorSession = null;
    return 1;
});

EM_JS(int, eagler_peer_has_spectators, (), {
    return (globalThis.__eaglerPeerTransport?.spectatorCount || 0) > 0 ? 1 : 0;
});

EM_JS(int, eagler_peer_spectator_state, (), {
    const state = globalThis.__eaglerPeerTransport;
    if (!state || state.closed || state.spectatorStopped || !state.relay ||
        state.relay.readyState >= WebSocket.CLOSING) return -1;
    return state.relay.readyState === WebSocket.OPEN ? 1 : 0;
});

EM_JS(void, eagler_peer_stop_spectators, (int graceful), {
    const state = globalThis.__eaglerPeerTransport;
    if (!state || state.closed || state.spectatorStopped || state.localPlayer !== 0) return;
    state.spectatorStopped = true;
    state.spectatorCount = 0;
    // Never close a socket here: relay may carry player input too. The small
    // terminal marker is sent ONCE even under backpressure. RTC signaling can
    // notify the server without waiting behind a blocked spectator upload.
    try {
        if (!graceful && state.route === 'rtc') state.sendSignal?.({type: 'spectator-stop', generation: state.spectatorSession?.generation});
    } catch {}
    try {
        if (state.relay?.readyState === WebSocket.OPEN)
            state.relay.send(new Uint8Array([0xe8, 0x53, 0x54, 0x4f, 0x50, 1]));
    } catch {}
});

EM_JS(int, eagler_peer_poll_size, (), {
    const state = globalThis.__eaglerPeerTransport;
    const packet = state?.received?.[state.receivedHead || 0];
    return packet ? packet.byteLength : 0;
});

EM_JS(int, eagler_peer_poll_copy, (unsigned char *out, int capacity), {
    const state = globalThis.__eaglerPeerTransport;
    if (!state) return 0;
    const head = state.receivedHead || 0;
    const packet = state.received?.[head];
    if (!packet || packet.byteLength > capacity) return 0;
    HEAPU8.set(packet, out);
    state.received[head] = undefined;
    state.receivedHead = head + 1;
    if (typeof state.receivedBytes === 'number') state.receivedBytes -= packet.byteLength;
    // Compact only occasionally. Normal polling is O(1), while a peer that
    // wakes from a long Restart stall no longer pays Array.shift()'s repeated
    // whole-array moves for every queued control/input packet.
    if (state.receivedHead >= 256 && state.receivedHead * 2 >= state.received.length) {
        state.received = state.received.slice(state.receivedHead);
        state.receivedHead = 0;
    } else if (state.receivedHead >= state.received.length) {
        state.received.length = 0;
        state.receivedHead = 0;
    }
    return packet.byteLength;
});

EM_JS(double, eagler_peer_buffered_amount, (int lane), {
    const state = globalThis.__eaglerPeerTransport;
    if (!state) return 0;
    if (state.route === 'relay') return lane === 0 ? Number(state.relay?.bufferedAmount || 0) : 0;
    let total = 0;
    for (const peer of state.peers.values()) {
        if (lane !== 2) total += Number(peer.inputDc?.bufferedAmount || 0);
        if (lane !== 1) total += Number(peer.controlDc?.bufferedAmount || 0);
    }
    return total;
});

EM_JS(void, eagler_peer_copy_error, (char *out, int capacity), {
    if (capacity <= 0) return;
    const bytes = new TextEncoder().encode(globalThis.__eaglerPeerTransport?.error || String());
    const count = Math.min(bytes.byteLength, capacity - 1);
    HEAPU8.set(bytes.subarray(0, count), out);
    HEAPU8[out + count] = 0;
});

EM_JS(void, eagler_peer_copy_mode, (char *out, int capacity), {
    if (capacity <= 0) return;
    const bytes = new TextEncoder().encode(globalThis.__eaglerPeerTransport?.route || 'connecting');
    const count = Math.min(bytes.byteLength, capacity - 1);
    HEAPU8.set(bytes.subarray(0, count), out);
    HEAPU8[out + count] = 0;
});
#endif

BrowserPeerTransport::~BrowserPeerTransport() { Close(); }

bool BrowserPeerTransport::Connect(const char *relayUrl, std::uint8_t localPlayer, std::uint8_t playerCount,
                                   bool stageSpectatorHistory)
{
    Close();
#ifdef __EMSCRIPTEN__
    return relayUrl && relayUrl[0] &&
           eagler_peer_connect(relayUrl, TransportConfig::Tag, localPlayer, playerCount, stageSpectatorHistory) != 0;
#else
    (void)relayUrl; (void)localPlayer; (void)playerCount; (void)stageSpectatorHistory;
    lastError_ = "browser peer transport is Web-only";
    return false;
#endif
}

bool BrowserPeerTransport::ConnectSpectator(const char *relayUrl, const char *spectatorId,
                                            std::uint8_t playerCount, bool stageSpectatorHistory)
{
    Close();
#ifdef __EMSCRIPTEN__
    return relayUrl && relayUrl[0] && spectatorId && spectatorId[0] &&
           eagler_spectator_connect(relayUrl, spectatorId, TransportConfig::Tag, playerCount, stageSpectatorHistory) != 0;
#else
    (void)relayUrl; (void)spectatorId; (void)playerCount; (void)stageSpectatorHistory;
    return false;
#endif
}

bool BrowserPeerTransport::SendControl(const std::uint8_t *data, std::size_t size)
{
#ifdef __EMSCRIPTEN__
    if (!data || size == 0 || size > static_cast<std::size_t>(std::numeric_limits<int>::max())) return false;
    return eagler_peer_send(data, static_cast<int>(size), 1) != 0;
#else
    (void)data; (void)size;
    return false;
#endif
}

void BrowserPeerTransport::Close()
{
#ifdef __EMSCRIPTEN__
    eagler_peer_close();
#endif
}

bool BrowserPeerTransport::IsOpen() const
{
#ifdef __EMSCRIPTEN__
    return eagler_peer_is_open() != 0;
#else
    return false;
#endif
}

bool BrowserPeerTransport::Failed() const
{
#ifdef __EMSCRIPTEN__
    return eagler_peer_failed() != 0;
#else
    return true;
#endif
}

bool BrowserPeerTransport::Recovering() const
{
#ifdef __EMSCRIPTEN__
    return eagler_peer_recovering()!=0;
#else
    return false;
#endif
}
bool BrowserPeerTransport::Disconnected() const
{
#ifdef __EMSCRIPTEN__
    return eagler_peer_disconnected()!=0;
#else
    return false;
#endif
}
bool BrowserPeerTransport::CalibrationSuspended() const
{
#ifdef __EMSCRIPTEN__
    return eagler_calibration_suspended()!=0;
#else
    return false;
#endif
}

bool BrowserPeerTransport::Send(const std::uint8_t *data, std::size_t size)
{
#ifdef __EMSCRIPTEN__
    if (!data || size == 0 || size > static_cast<std::size_t>(std::numeric_limits<int>::max())) return false;
    return eagler_peer_send(data, static_cast<int>(size), 0) != 0;
#else
    (void)data; (void)size;
    return false;
#endif
}

bool BrowserPeerTransport::SendTo(std::uint8_t peer, const std::uint8_t *data, std::size_t size)
{
#ifdef __EMSCRIPTEN__
    if (!data || size == 0 || size > static_cast<std::size_t>(std::numeric_limits<int>::max())) return false;
    return eagler_peer_send_to(peer, data, static_cast<int>(size)) != 0;
#else
    (void)peer; (void)data; (void)size;
    return false;
#endif
}

bool BrowserPeerTransport::SendRepairTo(std::uint8_t peer, const std::uint8_t *data,
                                        std::size_t size)
{
#ifdef __EMSCRIPTEN__
    if (!data || size == 0 || size > 32768u) return false;
    return eagler_peer_send_repair_to(peer, data, static_cast<int>(size)) != 0;
#else
    (void)peer; (void)data; (void)size;
    return false;
#endif
}

bool BrowserPeerTransport::SendSpectator(const std::uint8_t *data, std::size_t size)
{
#ifdef __EMSCRIPTEN__
    if (!data || size == 0 || size > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        return false;
    return eagler_peer_send_spectator(data, static_cast<int>(size)) != 0;
#else
    (void)data; (void)size;
    return false;
#endif
}

void BrowserPeerTransport::BeginSpectatorSession(std::uint64_t sessionId, std::uint32_t seed)
{
#ifdef __EMSCRIPTEN__
    if (sessionId && seed <= 65535)
        eagler_peer_begin_spectator_session(std::uint32_t(sessionId), std::uint32_t(sessionId >> 32), seed);
#else
    (void)sessionId; (void)seed;
#endif
}

bool BrowserPeerTransport::TakeSpectatorSession(std::uint64_t *sessionId, std::uint32_t *seed)
{
#ifdef __EMSCRIPTEN__
    std::uint32_t words[3]{};
    if (!sessionId || !seed || !eagler_peer_take_spectator_session(words)) return false;
    *sessionId = words[0] | (std::uint64_t(words[1]) << 32); *seed = words[2]; return true;
#else
    (void)sessionId; (void)seed; return false;
#endif
}

bool BrowserPeerTransport::HasSpectators() const
{
#ifdef __EMSCRIPTEN__
    return eagler_peer_has_spectators() != 0;
#else
    return false;
#endif
}

int BrowserPeerTransport::SpectatorState() const
{
#ifdef __EMSCRIPTEN__
    return eagler_peer_spectator_state();
#else
    return -1;
#endif
}

void BrowserPeerTransport::StopSpectators(bool graceful)
{
#ifdef __EMSCRIPTEN__
    eagler_peer_stop_spectators(graceful ? 1 : 0);
#else
    (void)graceful;
#endif
}

void BrowserPeerTransport::ShareSpectatorReplay(const std::uint8_t* bytes,std::size_t size,bool extended){
#ifdef __EMSCRIPTEN__
    if(bytes&&size<=8388608)eagler_peer_share_replay(bytes,int(size),extended?1:0);
#else
    (void)bytes;(void)size;(void)extended;
#endif
}

bool BrowserPeerTransport::Poll(std::vector<std::uint8_t> *packet)
{
    if (!packet) return false;
#ifdef __EMSCRIPTEN__
    const int size = eagler_peer_poll_size();
    if (size <= 0) return false;
    packet->resize(static_cast<std::size_t>(size));
    const int copied = eagler_peer_poll_copy(packet->data(), size);
    if (copied != size) { packet->clear(); return false; }
    return true;
#else
    return false;
#endif
}

std::size_t BrowserPeerTransport::BufferedAmount() const
{
#ifdef __EMSCRIPTEN__
    const double value = eagler_peer_buffered_amount(0);
    return value > 0 ? static_cast<std::size_t>(value) : 0;
#else
    return 0;
#endif
}

std::size_t BrowserPeerTransport::BufferedInputAmount() const
{
#ifdef __EMSCRIPTEN__
    const double value = eagler_peer_buffered_amount(1);
    return value > 0 ? static_cast<std::size_t>(value) : 0;
#else
    return 0;
#endif
}

std::size_t BrowserPeerTransport::BufferedControlAmount() const
{
#ifdef __EMSCRIPTEN__
    const double value = eagler_peer_buffered_amount(2);
    return value > 0 ? static_cast<std::size_t>(value) : 0;
#else
    return 0;
#endif
}

const std::string &BrowserPeerTransport::LastError() const
{
#ifdef __EMSCRIPTEN__
    char buffer[256] = {};
    eagler_peer_copy_error(buffer, sizeof(buffer));
    lastError_ = buffer;
#endif
    return lastError_;
}

const char *BrowserPeerTransport::Mode() const
{
#ifdef __EMSCRIPTEN__
    static char buffer[32];
    buffer[0] = '\0';
    eagler_peer_copy_mode(buffer, sizeof(buffer));
    return buffer;
#else
    return "unsupported";
#endif
}
} // namespace Netplay
