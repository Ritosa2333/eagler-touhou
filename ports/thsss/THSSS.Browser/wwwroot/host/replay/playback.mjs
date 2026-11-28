import { createLocalReplay } from "./local-replay.mjs";
import { describeReplay } from "./describe.mjs";

function storedPath(path) {
  const text = String(path || "").replace(/\\/g, "/").replace(/^\/+/, "");
  const replay = text.match(/^(?:replay|Replay)\/([^/]+)$/i);
  return replay ? "Replay/" + replay[1] : text;
}

function savedBytes(runtime, path) {
  const value = runtime && runtime.saves && runtime.saves[storedPath(path)];
  if (typeof value !== "string" || !value) return null;
  const binary = atob(value);
  const out = new Uint8Array(binary.length);
  for (let i = 0; i < binary.length; i++) out[i] = binary.charCodeAt(i);
  return out;
}

function readState(text) {
  const parsed = JSON.parse(text || "");
  if (!parsed || typeof parsed !== "object") throw Error("录像信息无效");
  return {
    frame: parsed.frame | 0,
    total: parsed.total | 0,
    ended: !!parsed.ended,
    stage: parsed.stage | 0,
    startStage: parsed.startStage | 0,
  };
}

export function createReplayPlayback({ getRuntime, emit, stopLoop }) {
  let player = null;
  function host() {
    const runtime = getRuntime();
    return runtime && runtime.host;
  }
  function ensure() {
    if (player) return player;
    player = createLocalReplay({
      async inspect(path) {
        const api = host();
        const text = api && api.InspectReplay && api.InspectReplay(path);
        if (text) {
          const description = JSON.parse(text);
          if (!description || !Array.isArray(description.stages) || !description.stages.length) throw Error("录像信息无效");
          return description;
        }
        return describeReplay(savedBytes(getRuntime(), path));
      },
      async open(path, stage) {
        const api = host();
        if (!api || !api.OpenReplay || api.OpenReplay(path, stage) !== 1) throw Error("录像原生执行失败");
      },
      step(render) {
        const api = host();
        return !!(api && api.ReplayStep && api.ReplayStep(render ? 1 : 0) === 1);
      },
      state() {
        const api = host();
        if (!api || !api.ReplayState) throw Error("录像原生执行失败");
        return readState(api.ReplayState());
      },
      stopLoop,
      async prepare() {
        const deadline = performance.now() + 15000;
        while (performance.now() < deadline) {
          const api = host();
          if (api && api.ReplayReady && api.ReplayReady()) return;
          await new Promise((resolve) => setTimeout(resolve, 30));
        }
        throw Error("Replay is not ready");
      },
      audioState(status) {
        const api = host();
        const hold = !!(status && (status.paused || status.hidden || status.seeking || status.ended));
        if (api && api.WorkspaceAudio) api.WorkspaceAudio(hold);
        return true;
      },
      onState(playback) {
        emit("runtime-info", { playback });
      },
      onError(error) {
        emit("error", { message: String(error && error.message || error) });
      },
    });
    return player;
  }
  return {
    command(message) {
      return ensure().command(message);
    },
    close() {
      if (player) player.close();
      player = null;
    },
  };
}
