import {ManagedContent, installManagedWorkers} from './managed-data.mjs';
const queued = [];
let started = false;
const receive = async event => {
  event.stopImmediatePropagation();
  if (started) { queued.push(event); return; }
  const data = event.data;
  if (data?.type !== 'eagler-content-init') return;
  started = true;
  try {
    const content = new ManagedContent(data.blob, data.entries, data.game);
    content.install(globalThis, data.base);
    installManagedWorkers(globalThis, content, data.base);
    await import(data.url);
    removeEventListener('message', receive);
    for (const message of queued) dispatchEvent(new MessageEvent('message', {data:message.data,ports:message.ports}));
  } catch (error) { postMessage({type: 'error', message: error.message}); }
};
addEventListener('message', receive);
