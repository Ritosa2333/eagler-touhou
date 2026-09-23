/** L0 static contract. Preconditions: workspace Runtime Shell sources.
 * Mutation: none. Proves the shared command/event vocabulary remains present.
 * Does NOT prove message execution, browser lifecycle, payload semantics or gameplay. */
import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import {
  RUNTIME_PROTOCOL_COMMANDS,
  RUNTIME_PROTOCOL_EVENTS,
  TOUCH_SENSITIVITY_MAX,
  TOUCH_SENSITIVITY_MIN,
} from "../lib/contracts/runtime-protocol.mjs";
import { workspacePath } from "../lib/workspace-layout.mjs";

const shellSources = Object.fromEntries(Object.entries(PRODUCT_GAMES).map(([game, product]) => [game,
  product.runtimeFileLayout === "directory"
    ? game === "th09"
      ? [workspacePath(game, "th09_web", "sdl-runtime", "managed.mjs")]
      : [
        workspacePath(game, `${game}_web`, "sdl-runtime", "shell.mjs"),
        workspacePath(game, `${game}_web`, "sdl-runtime", "eagler-host.mjs"),
      ]
    : [workspacePath(game, "resources", "shell.html")],
]));

assert.deepEqual(Object.keys(shellSources), Object.keys(PRODUCT_GAMES));
for (const [game, paths] of Object.entries(shellSources)) {
  const source = (await Promise.all(paths.map(path => readFile(path, "utf8")))).join("\n");
  assert.match(source, /runtimeEpoch/, `${game}: Runtime shell must bind protocol traffic to a navigation epoch`);
  assert.match(source, /\.epoch\s*!==\s*epoch|message\.epoch\s*!==\s*epoch/,
    `${game}: Runtime shell must reject Host commands from another navigation epoch`);
  const directoryRuntime = PRODUCT_GAMES[game].runtimeFileLayout === "directory";
  if (directoryRuntime) {
    assert.ok(source.includes(`Math.max(${TOUCH_SENSITIVITY_MIN},Math.min(${TOUCH_SENSITIVITY_MAX},sensitivity))`),
      `${game}: directory Runtime touch normalization must share the ${TOUCH_SENSITIVITY_MIN}-${TOUCH_SENSITIVITY_MAX}% contract`);
    assert.ok(source.includes(`sensitivity>=${TOUCH_SENSITIVITY_MIN}&&sensitivity<=${TOUCH_SENSITIVITY_MAX}`),
      `${game}: live touch sensitivity must reject values outside the shared contract`);
  } else {
    assert.ok(source.includes(`touchSensitivity < ${TOUCH_SENSITIVITY_MIN}`) && source.includes(`touchSensitivity > ${TOUCH_SENSITIVITY_MAX}`),
      `${game}: preload Runtime touch validation must share the ${TOUCH_SENSITIVITY_MIN}-${TOUCH_SENSITIVITY_MAX}% contract`);
    const nativeOptions = await readFile(workspacePath(game, "src", "EaglerOptions.hpp"), "utf8");
    assert.ok(nativeOptions.includes(`Math.max(${TOUCH_SENSITIVITY_MIN}, value)`) && nativeOptions.includes(`Math.min(${TOUCH_SENSITIVITY_MAX},`),
      `${game}: native touch sensitivity owner must not retain a hidden range below the Launcher/Runtime contract`);
  }
  for (const command of RUNTIME_PROTOCOL_COMMANDS) {
    const pattern = new RegExp(`case\\s+["']${command.replaceAll("-", "\\-")}["']`);
    assert.ok(pattern.test(source), `${game}: shared command missing: ${command}`);
  }
  for (const event of RUNTIME_PROTOCOL_EVENTS) {
    const escaped = event.replaceAll("-", "\\-");
    const present = new RegExp(`emit\\(["']${escaped}["']`).test(source) ||
      (event === "ready" && /event:\s*["']ready["']/.test(source));
    assert.ok(present, `${game}: shared event missing: ${event}`);
  }
}
console.log(`Runtime protocol L0: PASS (${Object.keys(shellSources).length} games, ${RUNTIME_PROTOCOL_COMMANDS.length} commands, ${RUNTIME_PROTOCOL_EVENTS.length} events)`);
