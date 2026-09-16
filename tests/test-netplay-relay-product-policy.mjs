import assert from "node:assert/strict";
import { spawn } from "node:child_process";
import net from "node:net";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";

import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";

const root = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const relayPath = resolve(root, "server/netplay-relay.mjs");
const multiplayerGames = Object.entries(PRODUCT_GAMES)
  .filter(([, product]) => !!product.multiplayer)
  .map(([game]) => game);

function freePort() {
  return new Promise((resolvePort, reject) => {
    const server = net.createServer();
    server.once("error", reject);
    server.listen(0, "127.0.0.1", () => {
      const address = server.address();
      server.close(error => {
        if (error) reject(error);
        else resolvePort(address.port);
      });
    });
  });
}

function waitListening(child) {
  return new Promise((resolveReady, reject) => {
    const timer = setTimeout(() => reject(new Error("relay start timeout")), 5000);
    const onData = chunk => {
      if (!String(chunk).includes("listening")) return;
      clearTimeout(timer);
      child.stdout.off("data", onData);
      resolveReady();
    };
    child.stdout.on("data", onData);
    child.once("exit", code => {
      clearTimeout(timer);
      reject(new Error(`relay exited before listen: ${code}`));
    });
  });
}

function nextJson(socket) {
  return new Promise((resolveMessage, reject) => {
    const onMessage = event => {
      cleanup();
      try { resolveMessage(JSON.parse(String(event.data))); }
      catch (error) { reject(error); }
    };
    const onError = () => { cleanup(); reject(new Error("lobby socket failed")); };
    const cleanup = () => {
      socket.removeEventListener("message", onMessage);
      socket.removeEventListener("error", onError);
    };
    socket.addEventListener("message", onMessage);
    socket.addEventListener("error", onError);
  });
}

async function openLobby(port, room, clientId) {
  const socket = new WebSocket(`ws://127.0.0.1:${port}/?room=${room}&lobby=${clientId}`);
  const first = nextJson(socket);
  await new Promise((resolveOpen, reject) => {
    socket.addEventListener("open", resolveOpen, { once: true });
    socket.addEventListener("error", () => reject(new Error("lobby open failed")), { once: true });
  });
  const initial = await first;
  assert.equal(initial.type, "state");
  return socket;
}

async function sendAndReceive(socket, message) {
  const response = nextJson(socket);
  socket.send(JSON.stringify(message));
  return response;
}

async function verifyProduct(port, game) {
  const multiplayer = PRODUCT_GAMES[game].multiplayer;
  assert.ok(multiplayer, `${game} must declare multiplayer policy`);
  const product = `${game}mp`;
  const room = `${product}-policy${Date.now().toString(36)}`;
  const socket = await openLobby(port, room, `${product}_client`);
  try {
    const invalid = await sendAndReceive(socket, {
      type: "take-seat", seat: 0, loadout: multiplayer.loadouts.length, ready: false,
    });
    assert.equal(invalid.type, "error");
    assert.match(invalid.error, /loadout/);

    const seated = await sendAndReceive(socket, {
      type: "take-seat", seat: 0, loadout: multiplayer.loadouts.length - 1, ready: false,
    });
    assert.equal(seated.type, "state");
    assert.equal(seated.room.seats[0].loadout, multiplayer.loadouts.length - 1);

    const settings = await sendAndReceive(socket, {
      type: "settings", playerCount: multiplayer.playerCounts[0], difficulty: multiplayer.difficulties.length,
    });
    assert.equal(settings.type, "state");
    assert.equal(settings.room.difficulty, multiplayer.difficulties.length - 1);
  } finally {
    socket.close(1000);
  }
}

async function verifyGenericRoom(port) {
  const socket = await openLobby(port, `genericpolicy${Date.now().toString(36)}`, "generic_client");
  try {
    const seated = await sendAndReceive(socket, { type: "take-seat", seat: 0, loadout: 5, ready: false });
    assert.equal(seated.type, "state");
    assert.equal(seated.room.seats[0].loadout, 5);
    const settings = await sendAndReceive(socket, { type: "settings", playerCount: 2, difficulty: 5 });
    assert.equal(settings.type, "state");
    assert.equal(settings.room.difficulty, 5);
  } finally {
    socket.close(1000);
  }
}

const port = await freePort();
const relayEnv = {
  ...process.env,
  EAGLER_NETPLAY_RELAY_HOST: "127.0.0.1",
  EAGLER_NETPLAY_RELAY_PORT: String(port),
  EAGLER_NETPLAY_STUN_URLS: "",
};
for (const legacy of ["TH07_RELAY_HOST", "TH07_RELAY_PORT", "TH07_STUN_URLS"])
  delete relayEnv[legacy];
const relay = spawn(process.execPath, [relayPath], {
  cwd: root,
  env: relayEnv,
  stdio: ["ignore", "pipe", "pipe"],
});

try {
  await waitListening(relay);
  for (const game of multiplayerGames) await verifyProduct(port, game);
  await verifyGenericRoom(port);
} finally {
  relay.kill();
}

console.log(JSON.stringify({
  netplayRelayProductPolicy: "PASS",
  products: multiplayerGames.map(game => `${game}mp`),
  policyOwner: "product-catalog",
  genericRooms: "product-neutral",
  configuration: "EAGLER_NETPLAY_*",
}));
