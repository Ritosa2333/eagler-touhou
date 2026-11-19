import {test} from 'node:test';
import assert from 'node:assert/strict';
import {signTh09ReplayReceipt,verifyTh09ReplayReceipt} from '../contracts/th09-replay-receipt.mjs';
import {uploadTh09Replay} from '../browser/th09-replay-upload.mjs';

test('a saved replay receipt binds the duel, round and time and expires', () => {
  const secret='test-relay-bridge-secret-at-least-32-bytes';
  const now=Date.now();
  const claim={duelId:'9dcc33a7-ffe0-4d60-99c1-a4c0009da553',roundId:'1:1000',uploadedAt:now};
  const receipt=signTh09ReplayReceipt(secret,claim);
  assert.deepEqual(verifyTh09ReplayReceipt(secret,receipt,now),claim);
  assert.equal(verifyTh09ReplayReceipt('different-key',receipt,now),null);
  assert.equal(verifyTh09ReplayReceipt(secret,receipt.replace('1:1000','1:1001'),now),null);
  assert.equal(verifyTh09ReplayReceipt(secret,receipt,now+300001),null);
  assert.equal(verifyTh09ReplayReceipt(secret,receipt,now-5001),null);
});

test('the browser forwards a receipt only after a successful upload', async t => {
  const grant={token:'test-token',expiresAt:Date.now()+60000};
  const bytes=new Uint8Array([1,2,3]);
  t.mock.method(globalThis,'fetch',async () => Response.json({recorded:true,receipt:'saved-receipt'}));
  assert.equal(await uploadTh09Replay(grant,bytes),'saved-receipt');
  globalThis.fetch=async () => Response.json({recorded:false,receipt:'saved-receipt'});
  assert.equal(await uploadTh09Replay(grant,bytes),'saved-receipt');
  globalThis.fetch=async () => Response.json({error:'rejected'}, {status:400});
  assert.equal(await uploadTh09Replay(grant,bytes),undefined);
});
