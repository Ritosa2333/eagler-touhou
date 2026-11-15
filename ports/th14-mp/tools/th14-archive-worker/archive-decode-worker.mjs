import createModule from './archive-decoder.mjs';
import {createArchiveDecodeEndpoint} from './archive-decode-endpoint.mjs';
// The endpoint is identical in the browser and the Node component test. Only
// the message transport adapter differs; Node is not browser acceptance.
if (typeof self !== 'undefined' && typeof self.postMessage === 'function') {
  const accept = createArchiveDecodeEndpoint(createModule, (message, transfer=[]) => self.postMessage(message, transfer));
  self.addEventListener('message', event => accept(event.data));
} else {
  const {parentPort} = await import('node:worker_threads');
  if (!parentPort) throw new Error('Archive decoder requires a worker');
  const accept = createArchiveDecodeEndpoint(createModule, (message, transfer=[]) => parentPort.postMessage(message, transfer));
  parentPort.on('message', accept);
}
