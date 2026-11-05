if (!process.argv[2]) throw new Error("usage: node tools/maintainer/verify-public-targeted-relay.mjs <relay-ws-url>");
const base = new URL(process.argv[2]);
const nonce = `target-${Date.now().toString(36)}${Math.random().toString(36).slice(2, 7)}`;
base.searchParams.set("room", nonce);
base.searchParams.set("run", "1");
base.searchParams.set("players", "3");

const sockets = [];

function open(player, signal = false) {
  return new Promise((resolve, reject) => {
    const url = new URL(base);
    url.searchParams.set("player", String(player));
    if (signal) url.searchParams.set("signal", "1");
    const socket = new WebSocket(url);
    socket.binaryType = "arraybuffer";
    sockets.push(socket);
    const timer = setTimeout(() => reject(new Error(`open timeout: P${player + 1}`)), 60_000);
    socket.addEventListener("open", () => { clearTimeout(timer); resolve(socket); }, { once: true });
    socket.addEventListener("error", () => { clearTimeout(timer); reject(new Error(`socket error: ${url.pathname}`)); }, { once: true });
  });
}

function route(socket) {
  return new Promise((resolve, reject) => {
    const timer = setTimeout(() => reject(new Error("route timeout")), 60_000);
    const onMessage = event => {
      if (typeof event.data !== "string") return;
      let message;
      try { message = JSON.parse(event.data); } catch { return; }
      if (message.type !== "route") return;
      clearTimeout(timer);
      socket.removeEventListener("message", onMessage);
      resolve(message.mode);
    };
    socket.addEventListener("message", onMessage);
  });
}

function nextBinary(socket, timeoutMs = 3000) {
  return new Promise((resolve, reject) => {
    const timer = setTimeout(() => {
      socket.removeEventListener("message", onMessage);
      reject(new Error("binary timeout"));
    }, timeoutMs);
    const onMessage = event => {
      if (typeof event.data === "string") return;
      clearTimeout(timer);
      socket.removeEventListener("message", onMessage);
      resolve(new Uint8Array(event.data));
    };
    socket.addEventListener("message", onMessage);
  });
}

function expectBinary(socket, expected, timeoutMs = 60_000) {
  return new Promise((resolve, reject) => {
    const timer = setTimeout(() => {
      socket.removeEventListener("message", onMessage);
      reject(new Error("binary barrier timeout"));
    }, timeoutMs);
    const onMessage = event => {
      if (typeof event.data === "string") return;
      const payload = [...new Uint8Array(event.data)];
      clearTimeout(timer);
      socket.removeEventListener("message", onMessage);
      if (payload.join(",") !== expected.join(",")) {
        reject(new Error(`non-target peer received unexpected payload: ${payload.join(",")}`));
        return;
      }
      resolve(payload);
    };
    socket.addEventListener("message", onMessage);
  });
}

try {
  const relay0 = await open(0);
  const relay1 = await open(1);
  const relay2 = await open(2);
  const routes = [route(relay0), route(relay1), route(relay2)];
  await open(0, true); // Incomplete signaling deliberately forces whole-room relay.
  const modes = await Promise.all(routes);
  if (modes.some(mode => mode !== "relay")) throw new Error(`unexpected route: ${modes.join(",")}`);

  const targetPayload = nextBinary(relay2, 60_000);
  const barrierBytes = [0x55, 0x66, 0x77, 0x88];
  const nonTargetBarrier = expectBinary(relay1, barrierBytes);

  relay0.send(new Uint8Array([0xe7, 2, 0x11, 0x22, 0x33, 0x44]));
  const payload = [...await targetPayload];
  relay0.send(new Uint8Array([0xe7, 1, ...barrierBytes]));
  const barrier = await nonTargetBarrier;

  if (payload.join(",") !== "17,34,51,68") throw new Error(`payload mismatch: ${payload.join(",")}`);
  console.log(JSON.stringify({ pass: true, modes, target: 2, payload, nonTargetBarrier: barrier }));
} finally {
  for (const socket of sockets) try { socket.close(); } catch {}
}
