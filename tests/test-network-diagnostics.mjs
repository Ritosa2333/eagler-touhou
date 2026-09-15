import assert from "node:assert/strict";
import { WebSocket, WebSocketServer } from "ws";
import {
  legacyDiagnosticRelayUrl,
  probeRelay,
  turnServerLatencyFromLoopback,
} from "../.cache/build/browser/assets/launcher/network-diagnostics.mjs";

assert.equal(turnServerLatencyFromLoopback(86), 43,
  "same-browser relay-to-relay RTT contains two client-to-TURN round trips");

const fallback = new URL(legacyDiagnosticRelayUrl(
  "wss://relay.example.test/netplay/?deployment=blue&diagnostic=1",
  "probe-123",
));
assert.equal(fallback.searchParams.get("deployment"), "blue");
assert.equal(fallback.searchParams.get("diagnostic"), null);
assert.equal(fallback.searchParams.get("signal"), "1");
assert.equal(fallback.searchParams.get("player"), "0");
assert.equal(fallback.searchParams.get("players"), "2");

const server = new WebSocketServer({ host: "127.0.0.1", port: 0 });
await new Promise((resolve, reject) => {
  server.once("listening", resolve);
  server.once("error", reject);
});
let dedicatedAttempts = 0;
let signalingAttempts = 0;
server.on("connection", (socket, request) => {
  const url = new URL(request.url || "/", "ws://127.0.0.1");
  if (url.searchParams.get("diagnostic") === "1") {
    dedicatedAttempts++;
    socket.close(1008, "invalid room or player");
    return;
  }
  if (url.searchParams.get("signal") === "1") {
    signalingAttempts++;
    socket.send(JSON.stringify({
      type: "peers",
      iceServers: [{ urls: ["turn:relay.example.test:3478"], username: "ephemeral", credential: "test" }],
    }));
  }
});

const previousWebSocket = globalThis.WebSocket;
globalThis.WebSocket = WebSocket;
try {
  const address = server.address();
  assert.equal(typeof address, "object");
  // The repository gate runs many CPU-heavy checks concurrently. Give the
  // loopback socket enough scheduling headroom to prove the dedicated probe
  // before exercising the legacy fallback; production already uses a longer
  // bounded timeout.
  const result = await probeRelay(`ws://127.0.0.1:${address.port}/?diagnostic=1`, 5000);
  assert.equal(dedicatedAttempts, 1);
  assert.equal(signalingAttempts, 1);
  assert.equal(result.iceServers[0].username, "ephemeral");
  assert.ok(result.latencyMs >= 1);
} finally {
  globalThis.WebSocket = previousWebSocket;
  await new Promise(resolve => server.close(resolve));
}

console.log(JSON.stringify({ networkDiagnostics: "PASS", legacyRelayFallback: true }));
