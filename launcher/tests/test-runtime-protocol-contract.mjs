/** L0 static contract. Preconditions: workspace Runtime Shell sources.
 * Mutation: none. Proves the shared command/event vocabulary remains present.
 * Does NOT prove message execution, browser lifecycle, payload semantics or gameplay. */
import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import {
  RUNTIME_PROTOCOL_COMMANDS,
  RUNTIME_PROTOCOL_COMMAND_BEHAVIOR,
  RUNTIME_PROTOCOL_EVENTS,
  TOUCH_SENSITIVITY_MAX,
  TOUCH_SENSITIVITY_MIN,
} from "../lib/contracts/runtime-protocol.mjs";
import { loadRuntimeBuildConfig } from "../lib/runtime-build-profiles.mjs";
import { workspacePath } from "../lib/workspace-layout.mjs";

const builds = await loadRuntimeBuildConfig();
const variants = Object.entries(builds.games).flatMap(([game, entry]) =>
  Object.keys(entry.variants)
    .filter(variant => variant === "normal" || variant === "multiplayer")
    .map(variant => ({ game, variant, entry, product: PRODUCT_GAMES[game] })));

assert.deepEqual(Object.keys(builds.games).sort(), Object.keys(PRODUCT_GAMES).sort(),
  "Runtime build registration must cover every Product Catalog game");
assert.equal(variants.length, Object.values(PRODUCT_GAMES).reduce((total, product) =>
  total + 1 + Number(!!product.multiplayerRuntime), 0),
"protocol checks must cover each registered normal and multiplayer Runtime variant");

async function shellPaths({ game, variant, entry, product }) {
  const repository = entry.variants[variant].workspaceRepository ?? entry.workspaceRepository;
  if (product.runtimeFileLayout !== "directory") {
    return [workspacePath(repository, "resources", "shell.html")];
  }

  // TH09's Eagler packager publishes managed.mjs as the Runtime shell.mjs.
  // Other directory products publish their declared shell.mjs source directly.
  const shellSource = game === "th09" ? "managed.mjs" : "shell.mjs";
  const paths = [workspacePath(repository, "web", "sdl-runtime", shellSource)];
  if (product.runtimeAssets.includes("eagler-host.mjs")) {
    paths.push(workspacePath(repository, "web", "sdl-runtime", "eagler-host.mjs"));
  }
  return paths;
}

for (const target of variants) {
  const { game, variant, product } = target;
  const label = `${game}/${variant}`;
  const paths = await shellPaths(target);
  const source = (await Promise.all(paths.map(path => readFile(path, "utf8")))).join("\n");
  assert.match(source, /runtimeEpoch/, `${label}: Runtime shell must bind protocol traffic to a navigation epoch`);
  assert.match(source, /\.epoch\s*!==\s*epoch|message\.epoch\s*!==\s*epoch/,
    `${label}: Runtime shell must reject Host commands from another navigation epoch`);
  const directoryRuntime = product.runtimeFileLayout === "directory";
  if (directoryRuntime) {
    assert.ok(source.includes(`Math.max(${TOUCH_SENSITIVITY_MIN},Math.min(${TOUCH_SENSITIVITY_MAX},`),
      `${label}: directory Runtime touch normalization must share the ${TOUCH_SENSITIVITY_MIN}-${TOUCH_SENSITIVITY_MAX}% contract`);
    assert.ok(new RegExp(`(?:sensitivity|touchSensitivity)>=${TOUCH_SENSITIVITY_MIN}&&[\\w.]+<=${TOUCH_SENSITIVITY_MAX}`).test(source),
      `${label}: live touch sensitivity must reject values outside the shared contract`);
  } else {
    assert.ok(source.includes(`touchSensitivity < ${TOUCH_SENSITIVITY_MIN}`) && source.includes(`touchSensitivity > ${TOUCH_SENSITIVITY_MAX}`),
      `${label}: preload Runtime touch validation must share the ${TOUCH_SENSITIVITY_MIN}-${TOUCH_SENSITIVITY_MAX}% contract`);
    const repository = target.entry.variants[variant].workspaceRepository ?? target.entry.workspaceRepository;
    const nativeOptions = await readFile(workspacePath(repository, "src", "EaglerOptions.hpp"), "utf8");
    assert.ok(nativeOptions.includes(`Math.max(${TOUCH_SENSITIVITY_MIN}, value)`) && nativeOptions.includes(`Math.min(${TOUCH_SENSITIVITY_MAX},`),
      `${label}: native touch sensitivity owner must not retain a hidden range below the Launcher/Runtime contract`);
  }
  for (const command of RUNTIME_PROTOCOL_COMMANDS) {
    const games = RUNTIME_PROTOCOL_COMMAND_BEHAVIOR[command].games;
    if (games && !games.includes(game)) continue;
    const pattern = new RegExp(`case\\s+["']${command.replaceAll("-", "\\-")}['"]`);
    assert.ok(pattern.test(source), `${label}: shared command missing: ${command}`);
  }
  for (const event of RUNTIME_PROTOCOL_EVENTS) {
    const escaped = event.replaceAll("-", "\\-");
    const present = new RegExp(`emit\\(["']${escaped}['"]`).test(source) ||
      (event === "ready" && /event:\s*["']ready['"]/.test(source));
    assert.ok(present, `${label}: shared event missing: ${event}`);
  }
}
console.log(`Runtime protocol L0: PASS (${variants.length} variants, ${Object.keys(builds.games).length} games, ${RUNTIME_PROTOCOL_COMMANDS.length} commands, ${RUNTIME_PROTOCOL_EVENTS.length} events)`);
