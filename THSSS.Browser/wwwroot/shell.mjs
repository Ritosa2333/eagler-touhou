import { keyNameFromEvent } from "./directory-keyboard.mjs";
import { mountManagedData } from "./eagler-host.mjs";
import { createFileBridge } from "./host/storage/files.mjs";
import { createKeyOwners } from "./host/input/key-owners.mjs";
import { prepareRuntime, launchRuntime } from "./main.js";
import { createTouchBridge } from "./host/input/touch-bridge.mjs";
import { installRuntimeFileSystem } from "./runtime-files.mjs";

const protocol = "eagler-touhou/1";
const game = "thsss";
const saveRoot = "/savesthsss";
const query = new URLSearchParams(location.search);
const epoch = Number(query.get("runtimeEpoch"));
const parentWindow = window.parent;

installRuntimeFileSystem(window);

const keys = createKeyOwners((name, down) => {
  if (!runtime || !runtime.host) return;
  try {
    runtime.host.OnKey(name, down ? 1 : 0);
  } catch (err) {
    emit("error", { message: String(err && err.message || err) });
  }
});
let phase = "booting";
let starting = false;
let runtime = null;
let firstFrameSent = false;
let runtimeInfoSent = false;
let frameStats = { frames: 0, gaps: [] };
let lastFrameAt = 0;
let healthTimer = 0;

function hosted() {
  return window.parent !== window && Number.isSafeInteger(epoch) && epoch > 0;
}

function post(message) {
  if (!hosted()) return;
  parentWindow.postMessage(message, location.origin);
}

function emit(event, fields) {
  post(Object.assign({ protocol, game, epoch, event }, fields || {}));
}

function reply(message, ok, extra) {
  if (!message || message.request == null) return;
  post(Object.assign({
    protocol,
    game,
    epoch,
    request: message.request,
    ok,
  }, extra || {}));
}

function surface() {
  const host = runtime && runtime.host;
  if (!host || typeof host.InputSurface !== "function") return null;
  const value = host.InputSurface();
  if (value === 2) return "dialogue";
  if (value === 1) return "play";
  return "menu";
}

const files = createFileBridge({ getRuntime: () => runtime, reply });
const touch = createTouchBridge({
  keys,
  surface,
  options: () => (runtime && runtime.options) || {},
  shift(dx, dy) {
    const host = runtime && runtime.host;
    if (!host || typeof host.TouchShift !== "function") return false;
    host.TouchShift(dx, dy);
    return true;
  },
});

function rejectInapplicable(message) {
  reply(message, false, { error: "Command is not available for this title" });
}

async function handle(message) {
  if (!message || message.protocol !== protocol || message.game !== game) return;
  if (message.epoch !== epoch) return;
  const command = message.command;
  if (command === "configure") {
    if (phase === "launched" || starting) {
      reply(message, false, { error: "Cannot configure a running game" });
      return;
    }
    const options = message.options || {};
    const music = String(message.music || "none");
    if (music !== "none" && music !== "ogg") {
      reply(message, false, { error: "Music mode is not available" });
      return;
    }
    if (window.Module) window.Module.touhouMusicMode = music === "ogg" ? "ogg" : "none";
    if (runtime) runtime.options = options;
    reply(message, true, {});
    return;
  }
  if (command === "resources") {
    reply(message, true, {});
    return;
  }
  if (command === "launch") {
    if (phase === "launched" || starting) {
      reply(message, true, {});
      return;
    }
    try {
      await startGame();
      reply(message, true, {});
    } catch (err) {
      if (phase !== "launched") starting = false;
      reply(message, false, { error: String(err && err.message || err) });
      emit("error", { message: String(err && err.message || err) });
    }
    return;
  }
  if (command === "list" || command === "read" || command === "write" || command === "remove" || command === "sync") {
    await files.handle(message);
    return;
  }
  if (command === "keyboard") {
    const name = keyNameFromEvent(message);
    if (name) keys.setKey(name, !!message.down, "key");
    reply(message, true, {});
    return;
  }
  if (command === "keyboard-clear") {
    keys.releaseSource("key");
    reply(message, true, {});
    return;
  }
  if (command === "touch-controls") {
    touch.controls(message);
    reply(message, true, {});
    return;
  }
  if (command === "touch-cancel") {
    touch.cancel();
    reply(message, true, {});
    return;
  }
  if (command === "direct-touch") {
    touch.direct(message);
    reply(message, true, {});
    return;
  }
  if (command === "thprac-mouse" || command === "network-cancel" || command === "retry-music") {
    rejectInapplicable(message);
  }
}

function onLocalKey(event) {
  if (phase !== "launched") return;
  const name = keyNameFromEvent(event);
  if (!name) return;
  event.preventDefault();
  if (event.repeat) return;
  keys.setKey(name, event.type === "keydown", "local");
}

function onMessage(event) {
  if (!hosted() || event.source !== parentWindow || event.origin !== location.origin) return;
  handle(event.data || {}).catch((err) => emit("error", { message: String(err && err.message || err) }));
}

function publishHealth() {
  if (phase !== "launched") return;
  const gaps = frameStats.gaps.splice(0);
  const maxGap = gaps.length ? Math.max.apply(null, gaps) : 0;
  emit("frame-health", { fps: frameStats.frames, maxGapMs: maxGap });
  frameStats.frames = 0;
  const audio = window.__thsssAudio || {};
  emit("audio-health", {
    backend: audio.backend || "script",
    underruns: audio.underruns || 0,
    minQueuedMs: audio.minQueuedMs == null ? null : audio.minQueuedMs,
  });
}

async function startGame() {
  if (phase === "launched" || starting) return;
  starting = true;
  await launchRuntime(runtime, {
    onExit() {
      if (healthTimer) window.clearInterval(healthTimer);
      healthTimer = 0;
      keys.releaseAll();
      phase = "exited";
      emit("exit", { status: "success", code: 0 });
    },
    onBeforeFrame() {
      touch.presented();
    },
    onPresented() {
      const now = performance.now();
      if (lastFrameAt) frameStats.gaps.push(now - lastFrameAt);
      lastFrameAt = now;
      frameStats.frames += 1;
      if (!firstFrameSent) {
        firstFrameSent = true;
        emit("first-frame", {});
      }
      if (!runtimeInfoSent) {
        runtimeInfoSent = true;
        emit("runtime-info", { renderer: "webgl2", architecture: "nativeaot-wasm" });
      }
    },
  });
  if (phase === "exited") return;
  phase = "launched";
  healthTimer = window.setInterval(publishHealth, 1000);
}

async function loadManagedArchive() {
  const deadline = performance.now() + 5000;
  while (performance.now() < deadline) {
    if (typeof parentWindow.__eaglerPrepareManagedRuntimeDataV1 === "function") {
      const mounted = await mountManagedData(parentWindow, query, game);
      if (!mounted || !mounted.buffer || mounted.buffer.byteLength <= 0) {
        throw new Error("Managed game data is empty");
      }
      globalThis.__thsssArchive = new Uint8Array(mounted.buffer);
      return;
    }
    await new Promise((resolve) => setTimeout(resolve, 30));
  }
  throw new Error("Managed game data is unavailable");
}

async function boot() {
  if (!hosted()) {
    const prepared = await prepareRuntime({ captureKeys: true });
    await launchRuntime(prepared);
    return;
  }
  window.addEventListener("message", onMessage);
  window.__eaglerDirectInputBridge = {
    schema: "eagler-touhou/direct-input/1",
    protocol,
    game,
    epoch,
    origin: location.origin,
    submit(message) {
      if (!message || message.epoch !== epoch || message.protocol !== protocol || message.game !== game) return false;
      handle(message).catch((err) => emit("error", { message: String(err && err.message || err) }));
      return true;
    },
  };
  window.addEventListener("keydown", onLocalKey);
  window.addEventListener("keyup", onLocalKey);
  try {
    runtime = await prepareRuntime({ captureKeys: false });
    await loadManagedArchive();
    phase = "ready";
    parent.postMessage({ protocol, game, epoch, event: "ready", saveRoot }, location.origin);
  } catch (err) {
    emit("error", { message: String(err && err.message || err) });
  }
}

document.addEventListener("visibilitychange", () => {
  if (document.hidden) {
    touch.cancel();
    keys.releaseAll();
  }
});
window.addEventListener("pagehide", () => {
  keys.releaseAll();
  if (runtime && runtime.flushSaves) runtime.flushSaves();
});

boot();
