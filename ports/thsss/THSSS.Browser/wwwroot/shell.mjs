import { keyNameFromEvent } from "./directory-keyboard.mjs";
import { mountManagedData } from "./eagler-host.mjs";
import { createFileBridge } from "./host/storage/files.mjs";
import { createKeyOwners } from "./host/input/key-owners.mjs";
import { createKeyboardInputBridge } from "./host/input/keyboard-input-bridge.mjs";
import { createGamepadKeys } from "./host/input/gamepad-keys.mjs";
import { prepareRuntime, launchRuntime } from "./main.js";
import { bindCanvasDirectTouch } from "./host/input/canvas-direct-touch.mjs";
import { createTouchBridge } from "./host/input/touch-bridge.mjs";
import { installRemoteResources, installRuntimeFileSystem } from "./runtime-files.mjs";
import { seedSettingIni } from "./host/storage/setting-ini.mjs";
import { mountPracticePanel } from "./host/practice/practice-panel.mjs";
import { createReplayPlayback } from "./host/replay/playback.mjs";

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
}, { sampleTaps: true });
let phase = "booting";
let starting = false;
let runtime = null;
let presentation = null;
let firstFrameSent = false;
let runtimeInfoSent = false;
const frameStats = { frames: 0, maxGap: 0 };
let lastFrameAt = 0;
let healthAt = 0;
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
const replay = createReplayPlayback({
  getRuntime: () => runtime,
  emit,
  stopLoop() {
    if (presentation) presentation.stop();
    presentation = null;
  },
});
const touch = createTouchBridge({
  keys,
  surface,
  options: () => (runtime && runtime.options) || {},
  shift(dx, dy, unlimited) {
    const host = runtime && runtime.host;
    if (!host || typeof host.TouchShift !== "function") return false;
    return host.TouchShift(dx, dy, !!unlimited) !== 0;
  },
  residual() {
    const host = runtime && runtime.host;
    if (!host || typeof host.TakeTouchResidual !== "function") return null;
    return host.TakeTouchResidual();
  },
});
const gamepad = createGamepadKeys({ keys });
const practice = mountPracticePanel({
  parent: document.body,
  sync(flags) {
    const host = runtime && runtime.host;
    if (host && typeof host.PracticeConfigure === "function") host.PracticeConfigure(flags);
  },
  warp(stage) {
    const host = runtime && runtime.host;
    if (host && typeof host.PracticeResources === "function") host.PracticeResources();
    if (host && typeof host.PracticeWarp === "function") host.PracticeWarp(stage);
  },
});

function applyCanonicalKey(event, down) {
  const name = keyNameFromEvent(event);
  if (!name) return;
  if (practice.key(name, !!down)) return;
  keys.setKey(name, !!down, "key");
}

const keyboardBridge = hosted()
  ? createKeyboardInputBridge({
      target: window,
      protocol,
      game,
      epoch,
      origin: location.origin,
      ready: () => phase === "launched",
      dispatch: applyCanonicalKey,
      reset: () => keys.releaseSource("key"),
    })
  : null;
// Joystick modes hide the launcher direct surface; capture on the canvas instead.
const canvasTouch = bindCanvasDirectTouch({
  canvas: document.getElementById("game"),
  direct: (message) => touch.direct(message),
  options: () => (runtime && runtime.options) || {},
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
    practice.configure(options);
    await installRemoteResources([message.resources, message.runtimeResources, message.sharedResources]);
    reply(message, true, {});
    return;
  }
  if (command === "resources") {
    await installRemoteResources([message.resources]);
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
  if (command === "playback") {
    try {
      reply(message, true, await replay.command(message));
    } catch (err) {
      reply(message, false, { error: String(err && err.message || err) });
    }
    return;
  }
  if (command === "keyboard" || command === "keyboard-clear") {
    // When the launcher mapping port is live, remap here. Otherwise keep the
    // original raw keyboard path for standalone / pre-bridge sessions.
    if (keyboardBridge && keyboardBridge.submit(message)) {
      reply(message, true, {});
      return;
    }
    if (command === "keyboard-clear") keys.releaseSource("key");
    else applyCanonicalKey(message, !!message.down);
    reply(message, true, {});
    return;
  }
  if (command === "touch-controls") {
    touch.controls(message);
    reply(message, true, {});
    return;
  }
  if (command === "touch-cancel") {
    canvasTouch.cancel();
    touch.cancel();
    gamepad.cancel();
    reply(message, true, {});
    return;
  }
  if (command === "direct-touch") {
    touch.direct(message);
    reply(message, true, {});
    return;
  }
  if (command === "thprac-mouse") {
    practice.pointer(message);
    reply(message, true, {});
    return;
  }
  if (command === "network-cancel" || command === "retry-music") {
    rejectInapplicable(message);
  }
}

function onLocalKey(event) {
  if (phase !== "launched") return;
  // Bound launcher mapping owns accept and reject; do not fall through to a
  // second raw owner after a rejected DOWN (its UP would never release).
  if (keyboardBridge && keyboardBridge.active()) {
    if (keyboardBridge.event(event, event.type === "keydown")) event.preventDefault();
    return;
  }
  const name = keyNameFromEvent(event);
  if (!name) return;
  event.preventDefault();
  if (event.repeat) return;
  if (practice.key(name, event.type === "keydown")) return;
  keys.setKey(name, event.type === "keydown", "local");
}

function onMessage(event) {
  if (!hosted() || event.source !== parentWindow || event.origin !== location.origin) return;
  handle(event.data || {}).catch((err) => emit("error", { message: String(err && err.message || err) }));
}

function publishHealth() {
  if (phase !== "launched") return;
  const now = performance.now();
  emit("frame-health", { fps: frameStats.frames * 1000 / (now - healthAt), maxGapMs: frameStats.maxGap });
  healthAt = now;
  frameStats.frames = 0;
  frameStats.maxGap = 0;
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
  await seedSettingIni(runtime);
  practice.configure((runtime && runtime.options) || {});
  const options = (runtime && runtime.options) || {};
  if (options.replayViewer) {
    practice.configure(Object.assign({}, options, { thpracEnabled: false, touchEnabled: false }));
    if (runtime.host && runtime.host.SetTouchInputEnabled) runtime.host.SetTouchInputEnabled(false);
  }
  presentation = await launchRuntime(runtime, {
    onExit() {
      if (healthTimer) window.clearInterval(healthTimer);
      healthTimer = 0;
      keys.releaseAll();
      replay.close();
      phase = "exited";
      emit("exit", { status: "success", code: 0 });
    },
    onBeforeFrame() {
      gamepad.presented();
      touch.presented();
    },
    onAfterStep: keys.finishStep,
    onPresented() {
      const now = performance.now();
      if (lastFrameAt) frameStats.maxGap = Math.max(frameStats.maxGap, now - lastFrameAt);
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
  healthAt = performance.now();
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
  window.addEventListener("blur", () => {
    keyboardBridge?.clear();
    keys.releaseSource("key");
    keys.releaseSource("local");
  });
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
    canvasTouch.cancel();
    touch.cancel();
    keys.releaseAll();
    lastFrameAt = 0;
  }
});
window.addEventListener("pagehide", () => {
  canvasTouch.cancel();
  keys.releaseAll();
  if (runtime && runtime.flushSaves) runtime.flushSaves();
});

boot();
