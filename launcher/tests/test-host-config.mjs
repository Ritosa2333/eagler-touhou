import assert from "node:assert/strict";
import { mkdtemp, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import {
  HOST_CONFIG_SCHEMA,
  hostConfigWarnings,
  readHostConfig,
  validateHostConfig,
} from "../lib/host-config.mjs";

const empty = validateHostConfig({
  schema: HOST_CONFIG_SCHEMA,
  netplay: { relay: "" },
  externalImportSource: { url: "", hint: "" },
});
assert.equal(empty.netplay.relay, "");
assert.equal(hostConfigWarnings(empty).length, 2);

const configured = validateHostConfig({
  schema: HOST_CONFIG_SCHEMA,
  netplay: { relay: "wss://relay.example.com/eagler-netplay/" },
  externalImportSource: { url: "https://downloads.example.com/packages", hint: "code 1234" },
});
assert.equal(configured.netplay.relay, "wss://relay.example.com/eagler-netplay/");
assert.equal(configured.externalImportSource.url, "https://downloads.example.com/packages");
assert.equal(hostConfigWarnings(configured).length, 1, "TURN remains a server-managed warning until actively verified");

assert.throws(() => validateHostConfig({ schema: HOST_CONFIG_SCHEMA, netplay: { relay: "https://example.com" } }), /ws:\/\/ or wss:\/\//);
assert.throws(() => validateHostConfig({ schema: HOST_CONFIG_SCHEMA, externalImportSource: { url: "http://example.com" } }), /https:\/\//);

const root = await mkdtemp(join(tmpdir(), "eagler-host-config-"));
try {
const missing = await readHostConfig(join(root, "missing.json"));
assert.equal(missing.present, false);
const file = join(root, "eagler-touhou.config.json");
await writeFile(file, JSON.stringify(configured));
assert.equal((await readHostConfig(file)).present, true);

} finally {
  await rm(root, { recursive: true, force: true });
}

console.log(JSON.stringify({ hostConfig: "PASS" }));
