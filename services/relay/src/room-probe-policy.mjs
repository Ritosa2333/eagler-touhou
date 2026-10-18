// Only small, targeted lobby diagnostics. No broadcast or Runtime control.
export function roomProbeEnvelope(message, from, onlinePeers) {
  if (!message || typeof message !== 'object' || message.type !== 'room-probe' ||
      typeof message.to !== 'string' || message.to === from || !onlinePeers.has(message.to) ||
      !['direct', 'turn', 'relay'].includes(message.lane)) return null;
  if (message.lane === 'relay') {
    if (typeof message.echo !== 'string' || !/^(ping|pong):\d{1,12}$/.test(message.echo)) return null;
    return { type: 'room-probe', from, lane: 'relay', echo: message.echo };
  }
  if (typeof message.token !== 'string' || !/^[a-zA-Z0-9-]{1,64}$/.test(message.token)) return null;
  const result = { type: 'room-probe', from, lane: message.lane, token: message.token };
  if (message.restart === true) return { ...result, restart: true };
  if (message.description && ['offer', 'answer'].includes(message.description.type) &&
      typeof message.description.sdp === 'string' && message.description.sdp.length <= 16384) {
    return { ...result, description: { type: message.description.type, sdp: message.description.sdp } };
  }
  const candidate = message.candidate;
  if (candidate && typeof candidate.candidate === 'string' && candidate.candidate.length <= 2048 &&
      (candidate.sdpMid == null || typeof candidate.sdpMid === 'string' && candidate.sdpMid.length <= 32) &&
      (candidate.sdpMLineIndex == null || Number.isInteger(candidate.sdpMLineIndex) && candidate.sdpMLineIndex >= 0 && candidate.sdpMLineIndex <= 8)) {
    return { ...result, candidate: { candidate: candidate.candidate, sdpMid: candidate.sdpMid, sdpMLineIndex: candidate.sdpMLineIndex } };
  }
  return null;
}
