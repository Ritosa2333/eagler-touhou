const firstLine = value => typeof value === 'string' ? value.split(/\r?\n/, 1)[0] : null;
const isSeat = value => Number.isSafeInteger(value) && value >= 0 && value <= 2;

/** Attach the host-page receipt time to the protocol payload without inventing one later. */
export function stampRoomProtocolEvent(event, receivedAt, nativeAtReceipt = null) {
  if (!event || typeof event !== 'object' || Array.isArray(event))
    return {payload: event, receivedAt, nativeAtReceipt};
  return {...event, receivedAt, nativeAtReceipt};
}

/** Convert stamped protocol errors into the same evidence shape as console errors. */
export function roomProtocolErrorEntries(snapshots, stage) {
  if (!Array.isArray(snapshots)) throw new TypeError('snapshots must be an array');
  return snapshots.flatMap((native, seat) => (native?.events ?? [])
    .filter(event => event.event === 'error' || event.ok === false)
    .map(event => ({seat, stage, checkerMs: event.receivedAt, kind: 'protocol-error',
      text: String(event.message ?? event.error ?? JSON.stringify(event)), native:event.nativeAtReceipt,
      sampledNative:native})));
}

function departedSeat(text, allowRawProtocolText) {
  const match = /^Error: player ([1-3]) left the run$/.exec(text) ??
    /^Error: RTC peer P([1-3]) (?:input|control) channel (?:closed|error)$/.exec(text) ??
    (allowRawProtocolText && (/^player ([1-3]) left the run$/.exec(text) ??
      /^RTC peer P([1-3]) (?:input|control) channel (?:closed|error)$/.exec(text)));
  return match ? Number(match[1]) - 1 : null;
}

function returnWasConfirmed(disconnect, seat) {
  return Array.isArray(disconnect?.returns) && disconnect.returns.some(result => result &&
    result.seat === seat && result.returnControlClicked === true && result.returnedToRoom === true);
}

function isValidCloseTime(closed, entry, disconnect) {
  if (!Number.isFinite(closed.checkerMs) || closed.checkerMs < 0 || closed.checkerMs >= entry.checkerMs) return false;
  if (closed.reason === 'peer-close') return true;
  return closed.reason === 'return-to-room' &&
    Number.isFinite(closed.requestedCheckerMs) && closed.requestedCheckerMs >= 0 &&
    closed.requestedCheckerMs <= closed.checkerMs && returnWasConfirmed(disconnect, closed.seat);
}

function isExpectedRoomDisconnect(entry, disconnect) {
  const closedSeats = Array.isArray(disconnect?.closedSeats) ? disconnect.closedSeats : [];
  if (!entry || typeof entry !== 'object' || !isSeat(entry.seat) ||
      !Number.isFinite(entry.checkerMs) || entry.checkerMs < 0 ||
      typeof entry.stage !== 'string' || entry.stage.length === 0) return false;

  const text = firstLine(entry.text), native = entry.native;
  if (!text || !native || typeof native !== 'object' ||
      (native.failed !== true && native.transportState?.disconnected !== true) ||
      firstLine(native.error) !== text) return false;

  const departed = departedSeat(text, entry.kind === 'protocol-error');
  if (departed === null || departed === entry.seat) return false;
  return closedSeats.some(closed => {
    return closed && isSeat(closed.seat) && closed.seat === departed &&
      isValidCloseTime(closed, entry, disconnect);
  });
}

/**
 * Partition observed room errors. A disconnect message is expected only when
 * its named one-based player/peer maps to a deliberately closed zero-based
 * seat, the close happened first, and that observer's native state confirms
 * the same error and a failed/disconnected transport.
 */
export function classifyRoomConsoleErrors({entries, disconnect} = {}) {
  if (!Array.isArray(entries)) throw new TypeError('entries must be an array');
  const expected = [], unexpected = [];
  for (const entry of entries) {
    (isExpectedRoomDisconnect(entry, disconnect) ? expected : unexpected).push(entry);
  }
  return {expected, unexpected};
}
