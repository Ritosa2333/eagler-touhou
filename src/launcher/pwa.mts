// Optional installed-app UX. No storage/SW promise is on the Launcher boot path.
interface InstallPromptEvent extends Event {
  prompt(): Promise<unknown>;
}
type StorageLike = Partial<Pick<StorageManager, "estimate" | "persist" | "persisted">>;
export async function settleWithin<T>(operation: () => T | PromiseLike<T>, fallback: T, timeout = 5000): Promise<T> {
  let timer: ReturnType<typeof setTimeout> | undefined;
  try {
    return await Promise.race([
      Promise.resolve().then(operation).catch(() => fallback),
      new Promise<T>(resolve => { timer = setTimeout(() => resolve(fallback), timeout); }),
    ]);
  } finally { if (timer !== undefined) clearTimeout(timer); }
}
function browserStorage(): StorageLike | undefined {
  try { return globalThis.navigator?.storage; } catch { return undefined; }
}
export async function readStorageStatus(storage = browserStorage()) {
  const [persistent, estimate] = await Promise.all([
    settleWithin<boolean | null>(() => storage?.persisted?.() ?? null, null),
    settleWithin<StorageEstimate | null>(() => storage?.estimate?.() ?? null, null),
  ]);
  return { persistent, usage: estimate?.usage ?? null, quota: estimate?.quota ?? null };
}
export function requestPersistentStorage(storage = browserStorage()): Promise<boolean | null> {
  // Invoke synchronously within the click's user activation, before any await.
  try {
    const result = storage?.persist?.();
    return result ? settleWithin<boolean | null>(() => result, null) : Promise.resolve(null);
  } catch { return Promise.resolve(null); }
}
const messages = {
  en: {
    open: "Install / offline", title: "Install and offline use", close: "Close", install: "Install app",
    installed: "Installed app", manual: "Use your browser's Install app menu. On iPhone/iPad, use Share → Add to Home Screen; in macOS Safari, use File → Add to Dock. Installation options depend on the browser.",
    boundary: "Installing the app does not install games. Offline play requires a cached launcher, the matching Runtime and all selected game/music/font packages. Multiplayer and downloads require a network.",
    separate: "An installed app may use separate storage from the browser tab. Open the app online and install/import your games there; export important saves before changing browser, profile or app.",
    waiting: "An update is ready. Close ALL windows and tabs of this site/app, then reopen. Refreshing one window does not apply it. Running games will not be interrupted.",
    shell: "Launcher cache is ready. Cached Runtime groups: ", unknown: "Offline readiness has not been confirmed. Open online and allow preparation to finish.",
    protect: "Request persistent storage", protected: "Persistent storage granted. Keep backups: manual clearing can still delete data.",
    bestEffort: "Storage is best-effort and may be evicted. Export important saves regularly.", storageUnknown: "Storage protection is unavailable or has not been confirmed. Keep backups.",
    size: "Estimated local storage: ", unavailable: "Installation was not completed. You can use the browser's installation menu.", none: "none",
  },
  zh: {
    open: "安装与离线", title: "安装与离线使用", close: "关闭", install: "安装应用",
    installed: "已安装应用", manual: "可使用浏览器菜单中的“安装应用”。iPhone / iPad 使用“分享 → 添加到主屏幕”；macOS Safari 使用“文件 → 添加到程序坞”。安装入口因浏览器而异。",
    boundary: "安装应用不等于安装游戏。离线游玩需要启动器缓存、对应的运行组件，以及全部已选择的游戏、音乐和字体包。联机和下载需要网络。",
    separate: "应用窗口可能与浏览器标签页使用独立存储。请先联网打开应用，在其中安装或导入游戏；切换浏览器、配置文件或应用前，请导出重要存档。",
    waiting: "更新已准备好。请关闭本站／应用的全部窗口和标签页，再重新打开。仅刷新一个窗口不会应用更新，正在运行的游戏不会被中断。",
    shell: "启动器已缓存。已缓存的运行组件组：", unknown: "尚未确认离线准备完成。请联网打开并等待准备完成。",
    protect: "申请保护本地存储", protected: "已获准持久存储。仍需备份：手动清理数据依然会删除存档。",
    bestEffort: "当前存储可能被浏览器回收，请定期导出重要存档。", storageUnknown: "存储保护不可用或尚未确认，请保留存档备份。",
    size: "本地存储估算：", unavailable: "未完成安装，可继续使用浏览器的安装菜单。", none: "无",
  },
};
function initPwa() {
  const mount = document.getElementById("mastheadMenuPanel");
  if (!mount || document.getElementById("pwaOpen")) return;
  const base = new URL("./", document.querySelector<HTMLLinkElement>('link[rel="manifest"]')?.href || location.href);
  const style = document.createElement("link");
  style.rel = "stylesheet"; style.href = new URL("pwa.css", base).href; document.head.append(style);
  if (!document.querySelector('link[rel="apple-touch-icon"]')) {
    const icon = document.createElement("link");
    icon.rel = "apple-touch-icon"; icon.sizes.value = "180x180";
    icon.href = new URL("assets/pwa/apple-touch-icon.png", base).href; document.head.append(icon);
  }
  const open = document.createElement("button");
  open.type = "button"; open.id = "pwaOpen"; open.className = "masthead-menu-item";
  open.setAttribute("aria-haspopup", "dialog"); open.setAttribute("aria-controls", "pwaDialog"); mount.append(open);
  const dialog = document.createElement("dialog");
  dialog.id = "pwaDialog"; dialog.className = "pwa-dialog";
  dialog.setAttribute("aria-labelledby", "pwaTitle");
  dialog.innerHTML = `<h2 id="pwaTitle" data-pwa-text="title"></h2>
    <p data-pwa-text="boundary"></p><p data-pwa-text="separate"></p>
    <p id="pwaManual" data-pwa-text="manual"></p>
    <p id="pwaUpdate" data-pwa-text="waiting" hidden></p>
    <p id="pwaShell" role="status"></p><p id="pwaStorage" role="status"></p><p id="pwaSize"></p>
    <div class="pwa-actions"><button id="pwaInstall" type="button"></button>
    <button id="pwaPersist" type="button" data-pwa-text="protect"></button>
    <button id="pwaClose" type="button" data-pwa-text="close"></button></div>`;
  document.body.append(dialog);
  const node = <T extends HTMLElement>(id: string) => dialog.querySelector<T>(`#${id}`)!;
  const install = node<HTMLButtonElement>("pwaInstall");
  const protect = node<HTMLButtonElement>("pwaPersist");
  const locale = () => document.documentElement.lang.toLowerCase().startsWith("en") ? messages.en : messages.zh;
  let prompt: InstallPromptEvent | null = null;
  let installed = false;
  let refreshId = 0;
  let persistent: boolean | null = null;
  function inApp() {
    try {
      return installed || matchMedia("(display-mode: standalone)").matches ||
        (navigator as Navigator & { standalone?: boolean }).standalone === true;
    } catch { return installed; }
  }
  function render() {
    const text = locale();
    open.textContent = text.open;
    for (const element of dialog.querySelectorAll<HTMLElement>("[data-pwa-text]")) {
      const key = element.dataset.pwaText as keyof typeof text;
      element.textContent = text[key];
    }
    install.textContent = inApp() ? text.installed : text.install;
    install.disabled = inApp(); install.hidden = !prompt && !inApp();
    node("pwaManual").hidden = !!prompt || inApp();
    node("pwaStorage").textContent = persistent === true ? text.protected : persistent === false ? text.bestEffort : text.storageUnknown;
  }
  window.addEventListener("beforeinstallprompt", event => {
    event.preventDefault(); prompt = event as InstallPromptEvent; render();
  });
  window.addEventListener("appinstalled", () => { installed = true; prompt = null; render(); });
  install.addEventListener("click", () => {
    const pending = prompt;
    if (!pending) return;
    prompt = null; render(); // one-use event; a dismissal does not trigger a loop
    try { void Promise.resolve(pending.prompt()).catch(() => { node("pwaManual").textContent = locale().unavailable; }); }
    catch { node("pwaManual").textContent = locale().unavailable; }
  });
  protect.addEventListener("click", () => {
    protect.disabled = true;
    void requestPersistentStorage().then(value => { persistent = value; render(); })
      .finally(() => { protect.disabled = false; });
  });
  async function refresh() {
    const id = ++refreshId;
    node("pwaShell").textContent = locale().unknown;
    node("pwaUpdate").hidden = true;
    void readStorageStatus().then(value => {
      if (id !== refreshId) return;
      persistent = value.persistent; render();
      node("pwaSize").textContent = value.usage === null ? "" :
        `${locale().size}${(value.usage / 1048576).toFixed(1)} MiB` +
        (value.quota === null ? "" : ` / ${(value.quota / 1048576).toFixed(1)} MiB`);
    });
    const registration = await settleWithin<ServiceWorkerRegistration | null>(async () =>
      await navigator.serviceWorker?.getRegistration(base.href) ?? null, null);
    if (id !== refreshId || !registration) return;
    node("pwaUpdate").hidden = !registration.waiting;
    const worker = registration.active;
    if (!worker) return;
    const channel = new MessageChannel();
    type Status = { ok?: boolean; shellReady?: boolean; runtimeGroups?: string[] };
    try {
      const status = await settleWithin<Status | null>(() => new Promise(resolve => {
        channel.port1.onmessage = event => resolve(event.data as Status);
        worker.postMessage({ type: "GET_APP_SHELL_STATUS" }, [channel.port2]);
      }), null);
      if (id === refreshId && status?.ok && status.shellReady) {
        node("pwaShell").textContent = locale().shell + (status.runtimeGroups?.join(", ") || locale().none);
      }
    } finally { channel.port1.close(); channel.port2.close(); }
  }
  open.addEventListener("click", () => {
    render();
    if (typeof dialog.showModal === "function") dialog.showModal(); else dialog.setAttribute("open", "");
    void refresh().catch(() => { node("pwaShell").textContent = locale().unknown; });
  });
  node("pwaClose").addEventListener("click", () => {
    if (typeof dialog.close === "function") dialog.close(); else dialog.removeAttribute("open");
    open.focus();
  });
  new MutationObserver(render).observe(document.documentElement, { attributes: true, attributeFilter: ["lang", "data-ui-locale"] });
  render();
}
if (typeof document !== "undefined") {
  // Optional PWA enhancement must never trip the Launcher's emergency boot path.
  try { initPwa(); } catch (error) { console.warn("PWA controls unavailable; continuing with the Launcher", error); }
}
