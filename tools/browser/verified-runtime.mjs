import {createHash} from 'node:crypto';

const sha256 = bytes => createHash('sha256').update(bytes).digest('hex');

// Observe the responses consumed by the browser, including redirects. A
// separate preflight request cannot establish the identity of a running game.
export async function verifyRuntimeDelivery(page, {scriptUrl, scriptBytes, wasmName, wasmBytes}) {
  const receipts = [], failures = [], pending = [];
  const expected = {script: sha256(scriptBytes), wasm: sha256(wasmBytes)};
  page.on('response', response => {
    if (response.status() >= 300 && response.status() < 400) return;
    let request = response.request(), isScript = false;
    while (request) {
      isScript ||= request.url() === scriptUrl;
      request = request.redirectedFrom();
    }
    const kind = isScript ? 'script' : new URL(response.url()).pathname.split('/').at(-1) === wasmName ? 'wasm' : null;
    if (!kind) return;
    pending.push((async () => {
      try {
        const bytes = await response.body();
        const receipt = {kind, url: response.url(), bytes: bytes.length, sha256: sha256(bytes)};
        receipts.push(receipt);
        if (response.status() !== 200 || receipt.sha256 !== expected[kind])
          throw new Error(`${kind} response does not match the candidate: ${receipt.url}`);
      } catch (error) { failures.push(String(error)); }
    })());
  });
  return {
    receipts, failures,
    async assertComplete() {
      await Promise.all(pending);
      if (failures.length) throw new Error(failures.join('\n'));
      for (const kind of ['script', 'wasm'])
        if (!receipts.some(receipt => receipt.kind === kind)) throw new Error(`Missing verified ${kind} response`);
    },
  };
}
