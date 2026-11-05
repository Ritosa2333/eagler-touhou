import {writeFile} from 'node:fs/promises';

// Keep the former script URL updateable when offline mode is disabled. Existing
// Launchers authorize activation only after gameplay/file operations finish.
// Do not claim other tabs, navigate running games, or delete user/cache data.
export const APP_SHELL_RETIREMENT_WORKER = `"use strict";
self.addEventListener("message", event => {
  const type = event.data && event.data.type;
  const port = event.ports && event.ports[0];
  if (type === "CHECK_APP_SHELL_ACTIVATION") {
    if (port) port.postMessage({ok: true, soleClient: true});
  } else if (type === "ACTIVATE_APP_SHELL") {
    event.waitUntil(self.skipWaiting().then(() => {
      if (port) port.postMessage({ok: true, activating: true});
    }));
  }
});
self.addEventListener("activate", event => {
  event.waitUntil(self.registration.unregister());
});
// Controlled old tabs can finish using the network until their own reload.
self.addEventListener("fetch", event => {
  event.respondWith(fetch(event.request));
});
`;

export function writeAppShellRetirementWorker(path) {
  return writeFile(path, APP_SHELL_RETIREMENT_WORKER, 'utf8');
}
