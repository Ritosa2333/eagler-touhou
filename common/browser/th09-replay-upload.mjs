/** @typedef {{token: string, expiresAt: number, notify?: boolean}} Th09ReplayGrant */

/** @param {Th09ReplayGrant} grant */
export function validTh09ReplayGrant(grant) {
  const now = Date.now();
  return typeof grant?.token === 'string' && /^[A-Za-z0-9_.:-]{1,153}$/.test(grant.token) &&
    Number.isSafeInteger(grant.expiresAt) && grant.expiresAt > now && grant.expiresAt <= now + 180_000;
}

export function reportTh09ReplayUpload(grant, receipt, socket) {
  if (grant?.notify !== true || typeof receipt !== 'string' || socket?.readyState !== 1) return false;
  socket.send(JSON.stringify({type:'th09-replay-saved',receipt}));
  return true;
}

/**
 * @param {Th09ReplayGrant} grant
 * @param {Uint8Array} bytes
 */
export async function uploadTh09Replay(grant, bytes) {
  if (!validTh09ReplayGrant(grant) || !ArrayBuffer.isView(bytes) || bytes.BYTES_PER_ELEMENT !== 1 ||
      bytes.byteLength < 1 || bytes.byteLength > 1024 * 1024) return;
  const {token, expiresAt} = grant;
  // Copy out of the game frame before the Launcher unloads it.
  const body = new Uint8Array(bytes);
  for (let delay = 250; Date.now() < expiresAt; delay = Math.min(delay * 2, 5000)) {
    try {
      const response = await fetch('/api/v1/th09/replays', {
        method: 'POST',
        headers: {'Authorization': 'Bearer ' + token, 'Content-Type': 'application/octet-stream'},
        body,
        credentials: 'omit',
        redirect: 'error',
        keepalive: body.length <= 60 * 1024,
        signal: AbortSignal.timeout(8000),
      });
      if (response.ok) {
        const result = await response.json();
        return typeof result.receipt === 'string' ? result.receipt : undefined;
      }
      if (response.status < 500 && response.status !== 408 && response.status !== 429) return;
    } catch {
      // Retry independently of the game connection.
    }
    if (Date.now() + delay >= expiresAt) return;
    await new Promise(resolve => setTimeout(resolve, delay));
  }
}
