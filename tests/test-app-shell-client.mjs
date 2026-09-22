import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { createAppShellClient } from "../.cache/build/browser/assets/launcher/app-shell-client.mjs";
import { HOST_SITE_ARTWORK_FILES } from "../lib/frontend-manifest.mjs";
class EventTargetStub {
  listeners = new Map();
  addEventListener(type, callback) { const list = this.listeners.get(type) || []; list.push(callback); this.listeners.set(type, list); }
  emit(type) { for (const callback of this.listeners.get(type) || []) callback(); }
}
class WorkerStub extends EventTargetStub {
  state = "installing";
  activationClients = 1; messages = [];
  setState(state) { this.state = state; this.emit("statechange"); }
  postMessage(message, ports = []) {
    this.messages.push(message.type);
    const response = message.type === "CHECK_APP_SHELL_ACTIVATION"
      ? { ok: true, soleClient: this.activationClients === 1, clients: this.activationClients }
      : message.type === "ACTIVATE_APP_SHELL" && this.activationClients === 1
        ? { ok: true, activating: true }
        : { ok: false, code: "MultipleClients", clients: this.activationClients };
    ports[0]?.postMessage(response);
  }
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
const createMessageChannel = () => {
  const port1 = { onmessage: null, close() {} };
  const port2 = { postMessage(data) { queueMicrotask(() => port1.onmessage?.({ data })); } };
  return { port1, port2 };
};
const client = createAppShellClient({ serviceWorker, secureContext: true,
  shouldDeferReload: () => defer, schedule: task => scheduled.push(task), reload: () => reloads++, logger: { warn() {} },
  createMessageChannel, activationRetryMs: 0,
});
await client.ready;
assert.deepEqual(serviceWorker.registerCalls, [{ url: "./app-shell-sw.js", options: { scope: "./", updateViaCache: "none" } }]);
assert.equal(registration.updates, 1);
const worker = new WorkerStub(); registration.installing = worker; registration.emit("updatefound");
worker.setState("installed");
assert.equal(client.snapshot().updateWaiting, true);
defer = false;
assert.equal(client.maybeReload(), false, "waiting must not cause a refresh loop");
await new Promise(resolve => setTimeout(resolve, 0));
assert.deepEqual(worker.messages, ["CHECK_APP_SHELL_ACTIVATION", "ACTIVATE_APP_SHELL"]);
assert.equal(client.snapshot().activationPending, true, "the sole idle client blocks input before activating the candidate");
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
const waitingRegistration = new RegistrationStub(); waitingRegistration.waiting = new WorkerStub();
const waiting = createAppShellClient({ serviceWorker: { ...serviceWorker, async register() { return waitingRegistration; } }, secureContext: true,
  shouldDeferReload: () => true, activationRetryMs: 0, createMessageChannel });
await waiting.ready; assert.equal(waiting.snapshot().updateWaiting, true); assert.equal(waiting.maybeReload(), false);
// A refreshed page did not see updatefound: the candidate is already waiting.
const resumedRegistration = new RegistrationStub();
const resumedWorker = new WorkerStub(); resumedWorker.state = "installed";
resumedRegistration.waiting = resumedWorker;
let resumedReloads = 0;
const resumed = createAppShellClient({
  serviceWorker: { ...serviceWorker, async register() { return resumedRegistration; } }, secureContext: true,
  createMessageChannel, activationRetryMs: 0, schedule: task => task(), reload: () => resumedReloads++,
});
await resumed.ready;
await new Promise(resolve => setTimeout(resolve, 0));
assert.deepEqual(resumedWorker.messages, ["CHECK_APP_SHELL_ACTIVATION", "ACTIVATE_APP_SHELL"]);
resumedWorker.setState("activated");
assert.equal(resumedReloads, 1, "an already-waiting replacement must finish updating without closing the browser");
resumedWorker.emit("statechange");
assert.equal(resumedReloads, 1, "duplicate activation notifications must not reload twice");
for (const fault of ["check-timeout", "check-throw", "activation-timeout", "missing-statechange", "stalled-handoff", "other-window", "slow-activation"]) {
  const retryRegistration = new RegistrationStub();
  const retryWorker = new WorkerStub(); retryWorker.state = "installed";
  retryRegistration.waiting = retryWorker;
  let checks = 0, activations = 0, retryReloads = 0;
  retryWorker.postMessage = function(message, ports) {
    if (message.type === "CHECK_APP_SHELL_ACTIVATION") {
      checks++;
      if (checks === 1 && fault === "check-timeout") return;
      if (checks === 1 && fault === "check-throw") throw new Error("suspended worker");
      this.activationClients = checks === 1 && fault === "other-window" ? 2 : 1;
    } else {
      activations++;
      if (fault !== "stalled-handoff" || activations > 1) this.state = "activated";
      if (fault === "slow-activation") {
        this.state = "activating";
        setTimeout(() => { this.state = "activated"; }, 35);
        return; // Lost reply while the browser is already switching workers.
      }
      if (fault === "activation-timeout") return;
    }
    WorkerStub.prototype.postMessage.call(this, message, ports);
  };
  let retryDeferred = false, unlockedDuringActivation = false;
  const retry = createAppShellClient({
    serviceWorker: { ...serviceWorker, async register() { return retryRegistration; } }, secureContext: true,
    createMessageChannel, activationRetryMs: 2, activationRequestTimeoutMs: 5, activationHandoffTimeoutMs: 10,
    onChange: snapshot => {
      if (retryWorker.state === "activating" && !snapshot.activationPending) unlockedDuringActivation = true;
    },
    shouldDeferReload: () => retryDeferred, schedule: task => task(), reload: () => retryReloads++,
  });
  try {
    await retry.ready;
    const deadline = Date.now() + 2000;
    while (!retryReloads && Date.now() < deadline) await new Promise(resolve => setTimeout(resolve, 5));
    assert.equal(retryReloads, 1, `${fault}: must recover without restarting the browser`);
    assert.equal(retry.snapshot().updateWaiting, false);
    assert.equal(unlockedDuringActivation, false, "an in-progress activation must keep input locked even after a timeout");
    assert.equal(activations, fault === "stalled-handoff" ? 2 : 1);
    assert.equal(checks, ["check-timeout", "check-throw", "stalled-handoff", "other-window"].includes(fault) ? 2 : 1);
  } finally {
    retryDeferred = true;
    retryWorker.setState("activated");
  }
}
const busyRegistration = new RegistrationStub();
const busyWorker = new WorkerStub(); busyWorker.state = "installed";
busyRegistration.waiting = busyWorker;
let becameBusy = false;
busyWorker.postMessage = function(message, ports) {
  // Activity can begin while an asynchronous eligibility request is in flight.
  becameBusy = true;
  WorkerStub.prototype.postMessage.call(this, message, ports);
};
const busyClient = createAppShellClient({
  serviceWorker: { ...serviceWorker, async register() { return busyRegistration; } }, secureContext: true,
  createMessageChannel, activationRetryMs: 0, shouldDeferReload: () => becameBusy,
});
await busyClient.ready;
await new Promise(resolve => setTimeout(resolve, 0));
assert.deepEqual(busyWorker.messages, ["CHECK_APP_SHELL_ACTIVATION"], "new game/operation must cancel activation after eligibility reply");
assert.equal(busyClient.snapshot().activationPending, false);
const existingRegistration = new RegistrationStub(); existingRegistration.installing = new WorkerStub();
const existing = createAppShellClient({ serviceWorker: { ...serviceWorker, async register() { return existingRegistration; } },
  secureContext: true, activationRetryMs: 0 });
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

const manifest = JSON.parse(await readFile(new URL("../public/site.webmanifest", import.meta.url), "utf8"));
assert.equal(manifest.id, "./"); assert.equal(manifest.scope, "./"); assert.equal(manifest.start_url, "./");
for (const size of [192, 512]) {
  const icon = manifest.icons.find(icon => icon.sizes === `${size}x${size}` && icon.purpose === "any");
  assert.ok(icon);
  assert.ok(HOST_SITE_ARTWORK_FILES.includes(icon.src.replace(/^assets\//, "")),
    `${icon.src}: manifest icon must be supplied by Host artwork assembly`);
}
const maskable = manifest.icons.find(icon => icon.sizes === "512x512" && icon.purpose === "maskable");
assert.ok(maskable);
assert.ok(HOST_SITE_ARTWORK_FILES.includes(maskable.src.replace(/^assets\//, "")));
console.log("app shell client and PWA icons: PASS");
