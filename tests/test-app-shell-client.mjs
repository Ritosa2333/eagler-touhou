import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { createAppShellClient } from "../.cache/build/browser/assets/launcher/app-shell-client.mjs";
import { readStorageStatus, requestPersistentStorage, settleWithin } from "../.cache/build/browser/assets/launcher/pwa.mjs";
class EventTargetStub {
  listeners = new Map();
  addEventListener(type, callback) { const list = this.listeners.get(type) || []; list.push(callback); this.listeners.set(type, list); }
  emit(type) { for (const callback of this.listeners.get(type) || []) callback(); }
}
class WorkerStub extends EventTargetStub {
  state = "installing";
  setState(state) { this.state = state; this.emit("statechange"); }
}
class RegistrationStub extends EventTargetStub {
  waiting = null; installing = null; updates = 0; updateError = null;
  async update() { this.updates++; if (this.updateError) throw this.updateError; }
}
const registration = new RegistrationStub();
const serviceWorker = { controller: {}, registerCalls: [],
  async register(url, options) { this.registerCalls.push({ url, options }); return registration; },
  async getRegistration() { return null; },
};
const scheduled = []; let defer = true, reloads = 0;
const client = createAppShellClient({ serviceWorker, secureContext: true,
  shouldDeferReload: () => defer, schedule: task => scheduled.push(task), reload: () => reloads++, logger: { warn() {} },
});
await client.ready;
assert.deepEqual(serviceWorker.registerCalls, [{ url: "./app-shell-sw.js", options: { scope: "./", updateViaCache: "none" } }]);
assert.equal(registration.updates, 1);
const worker = new WorkerStub(); registration.installing = worker; registration.emit("updatefound");
worker.setState("installed");
assert.equal(client.snapshot().updateWaiting, true);
defer = false;
assert.equal(client.maybeReload(), false, "waiting must not cause a refresh loop");
assert.equal(client.snapshot().reloadPending, false);
worker.setState("activated");
assert.equal(scheduled.length, 1);
defer = true; scheduled.shift()();
assert.equal(reloads, 0, "recheck activity at the scheduled task boundary");
defer = false; assert.equal(client.maybeReload(), true); scheduled.shift()();
assert.equal(reloads, 1); assert.equal(client.maybeReload(), false);

const firstRegistration = new RegistrationStub(); const firstWorker = new WorkerStub();
firstRegistration.installing = firstWorker;
const firstContainer = { controller: null, async register() { return firstRegistration; }, async getRegistration() { return null; } };
const first = createAppShellClient({ serviceWorker: firstContainer, secureContext: true });
let initialReady = false;
void first.ready.then(() => { initialReady = true; });
await new Promise(resolve => setTimeout(resolve, 0));
assert.equal(initialReady, false, "registration alone is not an active first-install cache worker");
firstContainer.controller = {}; firstWorker.setState("installed"); firstWorker.setState("activated");
await first.ready;
assert.equal(initialReady, true);
assert.equal(firstRegistration.updates, 0, "first installation must not start a redundant update check");
const nestedRegistration = new RegistrationStub(); const nestedWorker = new WorkerStub();
nestedRegistration.installing = nestedWorker;
const nestedContainer = {
  controller: { scriptURL: "https://example.invalid/app-shell-sw.js" },
  async register() { return nestedRegistration; }, async getRegistration() { return null; },
};
const nested = createAppShellClient({ serviceWorker: nestedContainer, secureContext: true,
  workerUrl: "https://example.invalid/nested/app-shell-sw.js" });
nestedWorker.setState("installed"); nestedWorker.setState("activated");
await nested.ready;
assert.equal(nestedRegistration.updates, 0, "a parent-scope controller must not turn a nested first install into an update");
assert.equal(nested.snapshot().updateReady, false, "a nested first install must not schedule a reload");
const stuck = new RegistrationStub(); stuck.installing = new WorkerStub();
const bounded = createAppShellClient({ serviceWorker: { ...firstContainer, controller: null, async register() { return stuck; } },
  secureContext: true, activationTimeoutMs: 5 });
assert.equal(await bounded.ready, stuck, "initial activation must not wait forever");
assert.equal(first.snapshot().updateReady, false, "a late first-install controller is not a replacement");
const waitingRegistration = new RegistrationStub(); waitingRegistration.waiting = {};
const waiting = createAppShellClient({ serviceWorker: { ...serviceWorker, async register() { return waitingRegistration; } }, secureContext: true });
await waiting.ready; assert.equal(waiting.snapshot().updateWaiting, true); assert.equal(waiting.maybeReload(), false);
const existingRegistration = new RegistrationStub(); existingRegistration.installing = new WorkerStub();
const existing = createAppShellClient({ serviceWorker: { ...serviceWorker, async register() { return existingRegistration; } }, secureContext: true });
await existing.ready; existingRegistration.installing.setState("installed");
assert.equal(existing.snapshot().updateWaiting, true, "observe updatefound that raced register resolution");

const fallback = createAppShellClient({ secureContext: true, logger: { warn() {} }, serviceWorker: {
  controller: {}, register() { throw new Error("synchronous SecurityError"); }, async getRegistration() { return registration; },
} });
assert.equal(await fallback.ready, registration);
const absent = createAppShellClient({ secureContext: true, logger: { warn() {} }, serviceWorker: {
  controller: null, async register() { throw new Error("blocked"); }, async getRegistration() { throw new Error("blocked"); },
} });
assert.equal(await absent.ready, null);
const insecure = createAppShellClient({ secureContext: false, serviceWorker: {
  controller: null, register() { throw new Error("must not register"); }, async getRegistration() { return null; },
} });
assert.equal(await insecure.ready, null);
registration.updateError = new Error("offline"); assert.equal(await client.checkForUpdate(), false);
assert.equal(client.snapshot().updateCheckFailed, true);

assert.deepEqual(await readStorageStatus({}), { persistent: null, usage: null, quota: null });
assert.deepEqual(await readStorageStatus({ persisted: async () => false, estimate: async () => ({ usage: 10, quota: 100 }) }), { persistent: false, usage: 10, quota: 100 });
assert.equal(await requestPersistentStorage({ persist() { throw new Error("denied"); } }), null);
let invoked = false;
const requested = requestPersistentStorage({ persist() { invoked = true; return Promise.resolve(true); } });
assert.equal(invoked, true, "persistent permission is requested in the user-activation task");
assert.equal(await requested, true);
assert.equal(await settleWithin(() => new Promise(() => {}), "timeout", 5), "timeout");
assert.equal(await settleWithin(() => { throw new Error("restricted"); }, "fallback"), "fallback");
const manifest = JSON.parse(await readFile(new URL("../public/site.webmanifest", import.meta.url), "utf8"));
assert.equal(manifest.id, "./"); assert.equal(manifest.scope, "./"); assert.equal(manifest.start_url, "./");
for (const size of [192, 512]) {
  const icon = manifest.icons.find(icon => icon.sizes === `${size}x${size}` && icon.purpose === "any");
  assert.ok(icon);
  const bytes = await readFile(new URL(`../public/${icon.src}`, import.meta.url));
  assert.equal(bytes.readUInt32BE(16), size); assert.equal(bytes.readUInt32BE(20), size);
}
assert.ok(manifest.icons.some(icon => icon.purpose === "maskable"));
console.log("app shell client, optional storage and PWA icons: PASS");
