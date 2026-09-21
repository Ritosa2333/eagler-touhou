export interface AppShellClientState {
  readonly registration: ServiceWorkerRegistrationLike | null;
  readonly updateReady: boolean;
  readonly updateWaiting: boolean;
  readonly updateCheckFailed: boolean;
  readonly updateError: unknown | null;
  readonly reloadPending: boolean;
  readonly reloadScheduled: boolean;
}
interface EventTargetLike {
  addEventListener(type: string, callback: () => void): void;
  removeEventListener?(type: string, callback: () => void): void;
}
interface ServiceWorkerLike extends EventTargetLike { readonly state: string; }
interface ServiceWorkerRegistrationLike extends EventTargetLike {
  readonly active?: ServiceWorkerLike | null;
  readonly waiting: unknown | null;
  readonly installing: ServiceWorkerLike | null;
  update(): Promise<unknown>;
}
interface ServiceWorkerContainerLike {
  readonly controller: unknown | null;
  register(url: string, options: { scope: string; updateViaCache: "none" }): Promise<ServiceWorkerRegistrationLike>;
  getRegistration(scope: string): Promise<ServiceWorkerRegistrationLike | null | undefined>;
}
interface LoggerLike { warn?(message: string, error: unknown): void; }
interface AppShellClientOptions {
  serviceWorker?: ServiceWorkerContainerLike | null;
  secureContext?: boolean;
  workerUrl?: string;
  scope?: string;
  shouldDeferReload?: () => boolean;
  onChange?: (state: Readonly<AppShellClientState>) => void;
  reload?: () => void;
  schedule?: (callback: () => void) => unknown;
  logger?: LoggerLike;
  activationTimeoutMs?: number;
}
function browserServiceWorker(): ServiceWorkerContainerLike | null {
  // The getter itself may throw in a restricted/embedded browser context.
  try { return globalThis.navigator?.serviceWorker as ServiceWorkerContainerLike || null; }
  catch { return null; }
}
export function createAppShellClient({
  serviceWorker = browserServiceWorker(),
  secureContext = globalThis.isSecureContext === true,
  workerUrl = "./app-shell-sw.js", scope = "./",
  shouldDeferReload = () => false, onChange = () => {},
  reload = () => globalThis.location?.reload(),
  schedule = callback => globalThis.setTimeout(callback, 0), logger = globalThis.console,
  activationTimeoutMs = 120000,
}: AppShellClientOptions = {}) {
  const state: { -readonly [K in keyof AppShellClientState]: AppShellClientState[K] } = {
    registration: null, updateReady: false, updateWaiting: false,
    updateCheckFailed: false, updateError: null, reloadPending: false, reloadScheduled: false,
  };
  const snapshot = (): Readonly<AppShellClientState> => Object.freeze({ ...state });
  const notify = () => onChange(snapshot());
  function maybeReload() {
    if (!state.updateReady || state.updateWaiting || !state.reloadPending || state.reloadScheduled || shouldDeferReload()) return false;
    state.reloadScheduled = true;
    notify();
    schedule(() => {
      // An operation may start after scheduling, before the next task runs.
      if (shouldDeferReload()) { state.reloadScheduled = false; notify(); return; }
      reload();
    });
    return true;
  }
  async function checkForUpdate() {
    if (!state.registration) return false;
    try {
      await state.registration.update();
      state.updateCheckFailed = false; state.updateError = null; notify(); return true;
    } catch (error) {
      state.updateCheckFailed = true; state.updateError = error; notify();
      logger?.warn?.("App Shell update check unavailable", error); return false;
    }
  }
  const watched = new WeakSet<ServiceWorkerLike>();
  function watchWorker(worker: ServiceWorkerLike | null, replacing: boolean) {
    if (!worker || watched.has(worker)) return;
    watched.add(worker);
    const changed = () => {
      if (!replacing) return;
      if (worker.state === "installed") {
        state.updateWaiting = true;
        state.updateReady = false;
        state.reloadPending = false;
        notify();
      } else if (worker.state === "activated") {
        // Normally the old clients have all closed by now. Retain this path
        // for externally activated/legacy workers without reloading a live game.
        state.updateWaiting = false; state.updateReady = true; state.reloadPending = true;
        notify(); maybeReload();
      } else if (worker.state === "redundant") {
        state.updateWaiting = !!state.registration?.waiting;
        notify();
      }
    };
    worker.addEventListener("statechange", changed);
    changed();
  }
  const controlledBeforeRegistration = !!serviceWorker?.controller;
  function watchRegistration(registration: ServiceWorkerRegistrationLike) {
    if (registration === state.registration) return;
    state.registration = registration;
    state.updateWaiting = !!registration.waiting && !!serviceWorker?.controller;
    registration.addEventListener("updatefound", () => watchWorker(registration.installing, !!serviceWorker?.controller));
    // register() may resolve after updatefound. Observe the in-flight worker too.
    watchWorker(registration.installing, controlledBeforeRegistration);
    notify();
    void checkForUpdate();
  }
  async function waitForInitialActivation(registration: ServiceWorkerRegistrationLike) {
    const worker = registration.installing;
    if (serviceWorker?.controller || registration.active || !worker) return registration;
    // register() resolves before install finishes. Callers preparing Runtime
    // caches await ready and need an active worker even on the first visit.
    // Do not wait for controllerchange: we intentionally do not clients.claim().
    await new Promise<void>(resolve => {
      let timer: ReturnType<typeof setTimeout> | undefined;
      const finish = () => {
        if (timer !== undefined) clearTimeout(timer);
        worker.removeEventListener?.("statechange", changed);
        resolve();
      };
      const changed = () => {
        if (worker.state === "activated" || worker.state === "redundant") finish();
      };
      timer = setTimeout(finish, activationTimeoutMs);
      worker.addEventListener("statechange", changed);
      changed();
    });
    return registration;
  }
  const ready = secureContext && serviceWorker
    ? Promise.resolve().then(() => serviceWorker.register(workerUrl, { scope, updateViaCache: "none" }))
      .catch(async error => {
        logger?.warn?.("App Shell Service Worker unavailable; continuing without it", error);
        try { return await serviceWorker.getRegistration(scope); } catch { return null; }
      }).then(registration => {
        if (registration) watchRegistration(registration);
        return registration ? waitForInitialActivation(registration) : null;
      })
    : Promise.resolve(null);
  return Object.freeze({ ready, snapshot, checkForUpdate, maybeReload });
}
