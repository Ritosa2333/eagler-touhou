import {createHmac, timingSafeEqual} from 'node:crypto';

const receiptPattern = /^([a-f0-9-]{36})\.(\d{1,16}:\d{1,16})\.(\d{13})\.([A-Za-z0-9_-]{43})$/;
const signature = (secret, claim) => createHmac('sha256', secret).update('th09-replay-saved-v1:').update(claim).digest();

export function signTh09ReplayReceipt(secret, {duelId, roundId, uploadedAt}) {
  const claim = [duelId, roundId, uploadedAt].join('.');
  return claim + '.' + signature(secret, claim).toString('base64url');
}

export function verifyTh09ReplayReceipt(secret, receipt, now = Date.now()) {
  if (typeof receipt !== 'string' || receipt.length > 160) return null;
  const parts = receipt.match(receiptPattern);
  if (!parts) return null;
  const [, duelId, roundId, at, signed] = parts;
  const uploadedAt = Number(at);
  if (uploadedAt > now + 5000 || now - uploadedAt > 300000) return null;
  const expected = signature(secret, [duelId, roundId, at].join('.'));
  const actual = Buffer.from(signed, 'base64url');
  return actual.length === expected.length && timingSafeEqual(actual, expected)
    ? {duelId, roundId, uploadedAt} : null;
}
