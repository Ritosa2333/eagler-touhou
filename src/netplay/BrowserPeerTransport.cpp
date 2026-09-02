#include "BrowserPeerTransport.hpp"

#include <limits>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace Netplay
{
#ifdef __EMSCRIPTEN__
EM_JS(int, th06_peer_connect, (const char *relayUrlPtr, int localPlayer, int playerCount), {
    const relayEnd = HEAPU8.indexOf(0, relayUrlPtr);
    const relayUrl = new TextDecoder().decode(HEAPU8.subarray(relayUrlPtr, relayEnd >= 0 ? relayEnd : HEAPU8.length));
    try {
        if (globalThis.__th06PeerTransport?.close) globalThis.__th06PeerTransport.close();
        const relay = new URL(relayUrl, location.href);
        relay.searchParams.set('player', String(localPlayer));
        relay.searchParams.set('players', String(playerCount));
        const signalUrl = new URL(relay.href);
        signalUrl.searchParams.set('signal', '1');

        const state = {
            relay: null, signal: null, peers: new Map(), received: [], receivedHead: 0, pendingSignals: [],
            localPlayer, playerCount, route: null, failed: false, error: String(), closed: false,
            spectatorCount: Math.max(0, Number(Module.eaglerOptions?.netplaySpectatorCount) || 0),
            rtcReadySent: false, signalReconnectTimer: null, signalReconnectDelayMs: 500,
            iceServers: Array.isArray(Module.eaglerOptions?.netplayIceServers)
                ? Module.eaglerOptions.netplayIceServers : [],
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
            fail(message) {
                if (this.closed || this.failed) return;
                this.failed = true;
                this.error = String(message || 'peer transport failed');
            },
            setRoute(mode) {
                if (this.closed || this.route) return;
                this.route = mode;
                globalThis.__eaglerNetplayTransport = mode;
                if (mode === 'rtc') {
                    if (this.spectatorCount === 0) {
                        try { this.relay?.close(1000, 'RTC route selected'); } catch {}
                    }
                    this.updateRtcPath().catch(() => {});
                } else {
                    for (const peer of this.peers.values()) {
                        try { peer.inputDc?.close(); } catch {}
                        try { peer.controlDc?.close(); } catch {}
                        try { peer.pc?.close(); } catch {}
                    }
                    this.peers.clear();
                    try { this.signal?.close(1000, 'relay route selected'); } catch {}
                }
            },
            receiveBinary(data) {
                if (data instanceof ArrayBuffer) this.received.push(new Uint8Array(data));
                else if (ArrayBuffer.isView(data)) this.received.push(new Uint8Array(data.buffer.slice(data.byteOffset, data.byteOffset + data.byteLength)));
                else if (data instanceof Blob) data.arrayBuffer().then(buffer => {
                    if (!this.closed) this.received.push(new Uint8Array(buffer));
                }).catch(() => {});
            },
            setupChannel(peerId, dc, kind) {
                const peer = this.peers.get(peerId);
                if (!peer) return;
                if (kind === 'control') peer.controlDc = dc;
                else peer.inputDc = dc;
                dc.binaryType = 'arraybuffer';
                // Route release is broadcast to every endpoint, so one peer
                // can process `route=rtc` a task earlier than another and send
                // its first HELLO/input immediately. Keep RTC packets that
                // arrive while our local route is still undecided, exactly as
                // the WebSocket fallback path does. If the room ultimately
                // selects relay, no conforming peer can have sent RTC gameplay
                // traffic; if it selects RTC, the queued packet is already in
                // order for DrainPackets().
                dc.onmessage = event => {
                    if (!this.route || this.route === 'rtc') this.receiveBinary(event.data);
                };
                dc.onopen = () => {
                    if (kind === 'control') peer.controlOpen = true;
                    else peer.inputOpen = true;
                    this.maybeReportRtcReady();
                };
                dc.onclose = () => {
                    if (kind === 'control') peer.controlOpen = false;
                    else peer.inputOpen = false;
                    if (!this.route) this.sendSignal({ type: 'rtc-failed' });
                    else if (this.route === 'rtc' && !this.closed) this.fail(`RTC peer P${peerId + 1} ${kind} channel closed`);
                };
                dc.onerror = () => {
                    if (!this.route) this.sendSignal({ type: 'rtc-failed' });
                    else if (this.route === 'rtc') this.fail(`RTC peer P${peerId + 1} ${kind} channel error`);
                };
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
                        const sent = Number(report.bytesSent);
                        const received = Number(report.bytesReceived);
                        if (Number.isFinite(sent) && Number.isFinite(received)) traffic = sent + received;
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
            },
            async schedulePeerRecovery(peerId, immediate = false) {
                const peer = this.peers.get(peerId);
                if (!peer || this.closed || this.route !== 'rtc' || peer.recoveryTimer || peer.restartInFlight) return;
                const failed = peer.pc.connectionState === 'failed' || peer.pc.iceConnectionState === 'failed';
                const disconnected = peer.pc.connectionState === 'disconnected' || peer.pc.iceConnectionState === 'disconnected';
                if (!failed && !disconnected) return;
                if (failed || immediate) {
                    this.requestPeerIceRestart(peerId);
                    return;
                }
                const before = await this.peerProgressSnapshot(peer);
                if (this.closed || this.route !== 'rtc' || this.peers.get(peerId) !== peer) return;
                peer.recoveryTimer = setTimeout(async () => {
                    peer.recoveryTimer = null;
                    const stillDisconnected = peer.pc.connectionState === 'disconnected' || peer.pc.iceConnectionState === 'disconnected';
                    const nowFailed = peer.pc.connectionState === 'failed' || peer.pc.iceConnectionState === 'failed';
                    if (!stillDisconnected && !nowFailed) return;
                    const after = await this.peerProgressSnapshot(peer);
                    if (!nowFailed && this.peerProgressed(before, after)) {
                        this.schedulePeerRecovery(peerId).catch(() => {});
                        return;
                    }
                    this.requestPeerIceRestart(peerId);
                }, 2500);
            },
            requestPeerIceRestart(peerId) {
                const peer = this.peers.get(peerId);
                if (!peer || this.closed || this.route !== 'rtc' || peer.restartInFlight) return;
                if (this.localPlayer < peerId) {
                    this.restartPeerIce(peerId).catch(error => {
                        peer.restartInFlight = false;
                        this.fail(`RTC peer P${peerId + 1} ICE restart error: ${error}`);
                    });
                } else {
                    peer.restartInFlight = true;
                    this.sendSignal({ type: 'ice-restart-request', to: peerId });
                    peer.restartWatchdog = setTimeout(() => {
                        peer.restartWatchdog = null;
                        peer.restartInFlight = false;
                        this.schedulePeerRecovery(peerId, true).catch(() => {});
                    }, 7000);
                }
            },
            async restartPeerIce(peerId) {
                const peer = this.peers.get(peerId);
                if (!peer || this.closed || this.route !== 'rtc' || peer.restartInFlight) return;
                if (peer.restartAttempts >= 2) {
                    this.fail(`RTC peer P${peerId + 1} ICE restart exhausted`);
                    return;
                }
                peer.restartInFlight = true;
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
                    if (healthy) return;
                    this.schedulePeerRecovery(peerId, true).catch(() => {});
                }, 7000);
            },
            handlePeerConnectionState(peerId) {
                const peer = this.peers.get(peerId);
                if (!peer || this.closed) return;
                const healthy = peer.pc.connectionState === 'connected' || peer.pc.iceConnectionState === 'connected' || peer.pc.iceConnectionState === 'completed';
                if (healthy) {
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
                    if (summary) summaries.push({ peer: peerId, ...summary });
                }
                globalThis.__eaglerNetplayRtcPaths = summaries;
                const paths = new Set(summaries.map(summary => summary.path));
                globalThis.__eaglerNetplayPath = paths.size === 1 ? [...paths][0] : paths.size > 1 ? 'mixed' : 'rtc';
            },
            async ensurePeer(peerId) {
                if (peerId === this.localPlayer || this.peers.has(peerId) || typeof RTCPeerConnection !== 'function') return;
                const peer = {
                    pc: new RTCPeerConnection({ iceServers: this.iceServers }),
                    inputDc: null, controlDc: null, inputOpen: false, controlOpen: false,
                    pendingCandidates: [], recoveryTimer: null, restartWatchdog: null,
                    restartInFlight: false, restartAttempts: 0
                };
                this.peers.set(peerId, peer);
                peer.pc.onicecandidate = event => {
                    if (event.candidate) this.sendSignal({ type: 'signal', to: peerId, candidate: event.candidate.toJSON ? event.candidate.toJSON() : event.candidate });
                };
                peer.pc.onconnectionstatechange = () => this.handlePeerConnectionState(peerId);
                peer.pc.oniceconnectionstatechange = () => this.handlePeerConnectionState(peerId);
                if (this.localPlayer > peerId) {
                    peer.pc.ondatachannel = event => {
                        if (event.channel.label === 'th06-control') this.setupChannel(peerId, event.channel, 'control');
                        else if (event.channel.label === 'th06-input') this.setupChannel(peerId, event.channel, 'input');
                    };
                    return;
                }
                const controlDc = peer.pc.createDataChannel('th06-control', { ordered: true });
                const inputDc = peer.pc.createDataChannel('th06-input', { ordered: false, maxRetransmits: 0 });
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
                if (message.description) {
                    await peer.pc.setRemoteDescription(message.description);
                    while (peer.pendingCandidates.length) await peer.pc.addIceCandidate(peer.pendingCandidates.shift());
                    if (message.description.type === 'offer') {
                        const answer = await peer.pc.createAnswer();
                        await peer.pc.setLocalDescription(answer);
                        this.sendSignal({ type: 'signal', to: peerId, description: peer.pc.localDescription });
                    }
                }
                if (message.candidate) {
                    if (peer.pc.remoteDescription) await peer.pc.addIceCandidate(message.candidate);
                    else peer.pendingCandidates.push(message.candidate);
                }
            },
            scheduleSignalReconnect() {
                if (this.closed || this.route !== 'rtc' || this.signalReconnectTimer) return;
                const delay = this.signalReconnectDelayMs;
                this.signalReconnectDelayMs = Math.min(5000, delay * 2);
                this.signalReconnectTimer = setTimeout(() => {
                    this.signalReconnectTimer = null;
                    this.openSignal();
                }, delay);
            },
            openSignal() {
                if (this.closed || this.route === 'relay' || this.signal?.readyState === WebSocket.OPEN || this.signal?.readyState === WebSocket.CONNECTING) return;
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
                    }
                };
                socket.onerror = () => {
                    if (!this.route && this.relay?.readyState === WebSocket.OPEN) this.setRoute('relay');
                };
                socket.onclose = () => {
                    if (this.signal !== socket) return;
                    this.signal = null;
                    if (this.closed) return;
                    if (this.route === 'rtc') this.scheduleSignalReconnect();
                    else if (!this.route && this.relay?.readyState === WebSocket.OPEN) this.setRoute('relay');
                };
            },
            close() {
                this.closed = true;
                if (this.signalReconnectTimer) clearTimeout(this.signalReconnectTimer);
                try { this.signal?.close(1000, 'transport close'); } catch {}
                try { this.relay?.close(1000, 'transport close'); } catch {}
                for (const peer of this.peers.values()) {
                    this.clearPeerRecovery(peer);
                    try { peer.inputDc?.close(); } catch {}
                    try { peer.controlDc?.close(); } catch {}
                    try { peer.pc?.close(); } catch {}
                }
                this.peers.clear();
                this.received.length = 0;
                this.receivedHead = 0;
            },
        };
        globalThis.__th06PeerTransport = state;
        globalThis.__eaglerNetplayTransport = 'connecting';
        globalThis.__eaglerNetplayPath = 'connecting';
        globalThis.__eaglerNetplayRtcPaths = [];

        state.relay = new WebSocket(relay.href);
        state.relay.binaryType = 'arraybuffer';
        state.relay.onopen = () => {
            if (!state.route && state.signal && state.signal.readyState >= WebSocket.CLOSING)
                state.setRoute('relay');
        };
        // Once the server releases the relay barrier, one peer may process the
        // route message a task earlier than another. Buffer packets that arrive
        // while our local route is still undecided; no sender can legally send
        // before the server has selected relay, so these are safe to retain.
        state.relay.onmessage = event => {
            if (typeof event.data === 'string') {
                let message;
                try { message = JSON.parse(event.data); } catch { return; }
                if (message?.type === 'route' && (message.mode === 'rtc' || message.mode === 'relay'))
                    state.setRoute(message.mode);
                return;
            }
            if (!state.route || state.route === 'relay') state.receiveBinary(event.data);
        };
        state.relay.onerror = () => { if (state.route === 'relay') state.fail('WebSocket relay error'); };
        state.relay.onclose = event => {
            if (state.closed || (state.route === 'rtc')) return;
            if (state.route === 'relay') state.fail(event.reason || 'WebSocket relay closed');
        };

        state.openSignal();
        return 1;
    } catch (error) {
        globalThis.__th06PeerTransport = { failed: true, error: String(error), close() {} };
        return 0;
    }
});

EM_JS(int, th06_spectator_connect,
      (const char *relayUrlPtr, const char *spectatorIdPtr, int playerCount), {
    const decode = ptr => {
        const end = HEAPU8.indexOf(0, ptr);
        return new TextDecoder().decode(HEAPU8.subarray(ptr, end >= 0 ? end : HEAPU8.length));
    };
    try {
        if (globalThis.__th06PeerTransport?.close) globalThis.__th06PeerTransport.close();
        const relay = new URL(decode(relayUrlPtr), location.href);
        relay.searchParams.delete('player');
        relay.searchParams.delete('signal');
        relay.searchParams.set('spectator', decode(spectatorIdPtr));
        relay.searchParams.set('players', String(playerCount));
        const state = {
            relay: null, peers: new Map(), received: [], receivedHead: 0,
            playerCount, route: 'spectator', failed: false, error: String(), closed: false,
            maxReceivedPackets: 16384,
            close() {
                this.closed = true;
                try { this.relay?.close(1000, 'spectator close'); } catch {}
                this.received.length = 0;
                this.receivedHead = 0;
            },
            receiveBinary(data) {
                const append = packet => {
                    if (this.closed || this.failed) return;
                    if (this.received.length - this.receivedHead >= this.maxReceivedPackets) {
                        this.failed = true;
                        this.error = 'Spectator fell too far behind';
                        try { this.relay?.close(1008, 'spectator fell too far behind'); } catch {}
                        return;
                    }
                    this.received.push(packet);
                };
                if (data instanceof ArrayBuffer) append(new Uint8Array(data));
                else if (data instanceof Blob) data.arrayBuffer().then(buffer => {
                    append(new Uint8Array(buffer));
                }).catch(() => {});
            },
        };
        globalThis.__th06PeerTransport = state;
        globalThis.__eaglerNetplayTransport = 'spectator';
        globalThis.__eaglerNetplayPath = 'relay';
        state.relay = new WebSocket(relay.href);
        state.relay.binaryType = 'arraybuffer';
        state.relay.onmessage = event => {
            if (typeof event.data !== 'string') state.receiveBinary(event.data);
        };
        state.relay.onerror = () => {
            if (!state.closed) { state.failed = true; state.error = 'Spectator relay error'; }
        };
        state.relay.onclose = event => {
            if (!state.closed) { state.failed = true; state.error = event.reason || 'Spectator relay closed'; }
        };
        return 1;
    } catch (error) {
        globalThis.__th06PeerTransport = { failed: true, error: String(error), close() {} };
        return 0;
    }
});

EM_JS(void, th06_peer_close, (), {
    try { globalThis.__th06PeerTransport?.close?.(); } catch {}
    globalThis.__th06PeerTransport = null;
});

EM_JS(int, th06_peer_is_open, (), {
    const state = globalThis.__th06PeerTransport;
    if (!state || state.failed || !state.route) return 0;
    if (state.route === 'relay' || state.route === 'spectator') return state.relay?.readyState === WebSocket.OPEN ? 1 : 0;
    if (state.route === 'rtc') return [...state.peers.values()].every(peer => peer.inputOpen && peer.controlOpen) ? 1 : 0;
    return 0;
});

EM_JS(int, th06_peer_failed, (), { return globalThis.__th06PeerTransport?.failed ? 1 : 0; });

EM_JS(int, th06_peer_send, (const unsigned char *data, int size, int control), {
    const state = globalThis.__th06PeerTransport;
    if (!state || state.failed || size <= 0) return 0;
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
                if (dc?.readyState !== 'open') return 0;
            }
            for (const peer of state.peers.values()) (control ? peer.controlDc : peer.inputDc).send(payload);
            return 1;
        }
    } catch (error) {
        state.fail(String(error));
    }
    return 0;
});

EM_JS(int, th06_peer_send_to, (int peerId, const unsigned char *data, int size), {
    const state = globalThis.__th06PeerTransport;
    if (!state || state.failed || size <= 0 || !Number.isInteger(peerId) ||
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
            const dc = state.peers.get(peerId)?.inputDc;
            if (dc?.readyState !== 'open') return 0;
            dc.send(payload);
            return 1;
        }
    } catch (error) {
        state.fail(String(error));
    }
    return 0;
});

EM_JS(int, th06_peer_send_spectator, (const unsigned char *data, int size), {
    const state = globalThis.__th06PeerTransport;
    if (!state || state.closed || size <= 0 ||
        state.relay?.readyState !== WebSocket.OPEN) return 0;
    try {
        const payload = HEAPU8.slice(data, data + size);
        const envelope = new Uint8Array(payload.byteLength + 1);
        envelope[0] = 0xe8;
        envelope.set(payload, 1);
        state.relay.send(envelope);
        return 1;
    } catch { return 0; }
});

EM_JS(int, th06_peer_has_spectators, (), {
    return (globalThis.__th06PeerTransport?.spectatorCount || 0) > 0 ? 1 : 0;
});

EM_JS(int, th06_peer_poll_size, (), {
    const state = globalThis.__th06PeerTransport;
    const packet = state?.received?.[state.receivedHead || 0];
    return packet ? packet.byteLength : 0;
});

EM_JS(int, th06_peer_poll_copy, (unsigned char *out, int capacity), {
    const state = globalThis.__th06PeerTransport;
    if (!state) return 0;
    const head = state.receivedHead || 0;
    const packet = state.received?.[head];
    if (!packet || packet.byteLength > capacity) return 0;
    HEAPU8.set(packet, out);
    state.received[head] = undefined;
    state.receivedHead = head + 1;
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

EM_JS(double, th06_peer_buffered_amount, (), {
    const state = globalThis.__th06PeerTransport;
    if (!state) return 0;
    if (state.route === 'relay') return Number(state.relay?.bufferedAmount || 0);
    let total = 0;
    for (const peer of state.peers.values()) {
        total += Number(peer.inputDc?.bufferedAmount || 0);
        total += Number(peer.controlDc?.bufferedAmount || 0);
    }
    return total;
});

EM_JS(void, th06_peer_copy_error, (char *out, int capacity), {
    if (capacity <= 0) return;
    const bytes = new TextEncoder().encode(globalThis.__th06PeerTransport?.error || String());
    const count = Math.min(bytes.byteLength, capacity - 1);
    HEAPU8.set(bytes.subarray(0, count), out);
    HEAPU8[out + count] = 0;
});

EM_JS(void, th06_peer_copy_mode, (char *out, int capacity), {
    if (capacity <= 0) return;
    const bytes = new TextEncoder().encode(globalThis.__th06PeerTransport?.route || 'connecting');
    const count = Math.min(bytes.byteLength, capacity - 1);
    HEAPU8.set(bytes.subarray(0, count), out);
    HEAPU8[out + count] = 0;
});
#endif

BrowserPeerTransport::~BrowserPeerTransport() { Close(); }

bool BrowserPeerTransport::Connect(const char *relayUrl, std::uint8_t localPlayer, std::uint8_t playerCount)
{
    Close();
#ifdef __EMSCRIPTEN__
    return relayUrl && relayUrl[0] && th06_peer_connect(relayUrl, localPlayer, playerCount) != 0;
#else
    (void)relayUrl; (void)localPlayer; (void)playerCount;
    lastError_ = "browser peer transport is Web-only";
    return false;
#endif
}

bool BrowserPeerTransport::ConnectSpectator(const char *relayUrl, const char *spectatorId,
                                            std::uint8_t playerCount)
{
    Close();
#ifdef __EMSCRIPTEN__
    return relayUrl && relayUrl[0] && spectatorId && spectatorId[0] &&
           th06_spectator_connect(relayUrl, spectatorId, playerCount) != 0;
#else
    (void)relayUrl; (void)spectatorId; (void)playerCount;
    return false;
#endif
}

bool BrowserPeerTransport::SendControl(const std::uint8_t *data, std::size_t size)
{
#ifdef __EMSCRIPTEN__
    if (!data || size == 0 || size > static_cast<std::size_t>(std::numeric_limits<int>::max())) return false;
    return th06_peer_send(data, static_cast<int>(size), 1) != 0;
#else
    (void)data; (void)size;
    return false;
#endif
}

void BrowserPeerTransport::Close()
{
#ifdef __EMSCRIPTEN__
    th06_peer_close();
#endif
}

bool BrowserPeerTransport::IsOpen() const
{
#ifdef __EMSCRIPTEN__
    return th06_peer_is_open() != 0;
#else
    return false;
#endif
}

bool BrowserPeerTransport::Failed() const
{
#ifdef __EMSCRIPTEN__
    return th06_peer_failed() != 0;
#else
    return true;
#endif
}

bool BrowserPeerTransport::Send(const std::uint8_t *data, std::size_t size)
{
#ifdef __EMSCRIPTEN__
    if (!data || size == 0 || size > static_cast<std::size_t>(std::numeric_limits<int>::max())) return false;
    return th06_peer_send(data, static_cast<int>(size), 0) != 0;
#else
    (void)data; (void)size;
    return false;
#endif
}

bool BrowserPeerTransport::SendTo(std::uint8_t peer, const std::uint8_t *data, std::size_t size)
{
#ifdef __EMSCRIPTEN__
    if (!data || size == 0 || size > static_cast<std::size_t>(std::numeric_limits<int>::max())) return false;
    return th06_peer_send_to(peer, data, static_cast<int>(size)) != 0;
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
    return th06_peer_send_spectator(data, static_cast<int>(size)) != 0;
#else
    (void)data; (void)size;
    return false;
#endif
}

bool BrowserPeerTransport::HasSpectators() const
{
#ifdef __EMSCRIPTEN__
    return th06_peer_has_spectators() != 0;
#else
    return false;
#endif
}

bool BrowserPeerTransport::Poll(std::vector<std::uint8_t> *packet)
{
    if (!packet) return false;
#ifdef __EMSCRIPTEN__
    const int size = th06_peer_poll_size();
    if (size <= 0) return false;
    packet->resize(static_cast<std::size_t>(size));
    const int copied = th06_peer_poll_copy(packet->data(), size);
    if (copied != size) { packet->clear(); return false; }
    return true;
#else
    return false;
#endif
}

std::size_t BrowserPeerTransport::BufferedAmount() const
{
#ifdef __EMSCRIPTEN__
    const double value = th06_peer_buffered_amount();
    return value > 0 ? static_cast<std::size_t>(value) : 0;
#else
    return 0;
#endif
}

const std::string &BrowserPeerTransport::LastError() const
{
#ifdef __EMSCRIPTEN__
    char buffer[256] = {};
    th06_peer_copy_error(buffer, sizeof(buffer));
    lastError_ = buffer;
#endif
    return lastError_;
}

const char *BrowserPeerTransport::Mode() const
{
#ifdef __EMSCRIPTEN__
    static char buffer[32];
    buffer[0] = '\0';
    th06_peer_copy_mode(buffer, sizeof(buffer));
    return buffer;
#else
    return "unsupported";
#endif
}
} // namespace Netplay
