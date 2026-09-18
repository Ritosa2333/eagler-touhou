/** L1 protocol-envelope behavior contract. Mutation: none. Proves unknown
 * Runtime messages are accepted only for the current protocol/game and known
 * event or response discriminants. Does not execute a Runtime shell. */
import assert from "node:assert/strict";
import {
  TOUCH_SENSITIVITY_MAX,
  TOUCH_SENSITIVITY_MIN,
  RUNTIME_CONFIGURE_OPTION_BEHAVIOR,
  RUNTIME_CONFIGURE_OPTION_KEYS,
  RUNTIME_CONFIGURE_LEGACY_OPTION_KEYS,
  RUNTIME_CONFIGURE_LEGACY_MUSIC_MODES,
  RUNTIME_PROTOCOL_LEGACY_EVENTS,
  RUNTIME_PROTOCOL_COMMANDS,
  RUNTIME_PROTOCOL_COMMAND_BEHAVIOR,
  RUNTIME_PROTOCOL_EVENTS,
  RUNTIME_PROTOCOL_EVENT_BEHAVIOR,
  RUNTIME_PROTOCOL_OPTIONAL_COMMANDS,
  RUNTIME_PROTOCOL_OPTIONAL_EVENTS,
  parseRuntimeInboundMessage,
} from "../.cache/build/browser/assets/contracts/runtime-protocol.mjs";

assert.equal(TOUCH_SENSITIVITY_MIN, 100);
assert.equal(TOUCH_SENSITIVITY_MAX, 300);
assert.deepEqual(Object.keys(RUNTIME_CONFIGURE_OPTION_BEHAVIOR), RUNTIME_CONFIGURE_OPTION_KEYS,
  "every canonical Runtime configure field must have exactly one semantic classification");
assert.equal(new Set(RUNTIME_CONFIGURE_OPTION_KEYS).size, RUNTIME_CONFIGURE_OPTION_KEYS.length);
assert.equal(new Set(RUNTIME_CONFIGURE_LEGACY_OPTION_KEYS).size, RUNTIME_CONFIGURE_LEGACY_OPTION_KEYS.length);
for (const key of RUNTIME_CONFIGURE_LEGACY_OPTION_KEYS) {
  assert.ok(!RUNTIME_CONFIGURE_OPTION_KEYS.includes(key), `${key}: legacy option must not be canonical`);
}
assert.deepEqual(RUNTIME_CONFIGURE_LEGACY_MUSIC_MODES, ["wav"]);
assert.deepEqual(RUNTIME_PROTOCOL_LEGACY_EVENTS, ["thprac-session"]);
for (const event of RUNTIME_PROTOCOL_LEGACY_EVENTS) {
  assert.ok(!RUNTIME_PROTOCOL_EVENTS.includes(event), `${event}: legacy event must not become a canonical required event`);
  assert.ok(!RUNTIME_PROTOCOL_OPTIONAL_EVENTS.includes(event), `${event}: legacy event must not remain in canonical optional vocabulary`);
  const legacyMessage = { protocol: "eagler-touhou/1", game: "th06", epoch: 7, event };
  assert.equal(parseRuntimeInboundMessage(legacyMessage, "th06")?.event, event,
    `${event}: bounded legacy read compatibility must remain parseable`);
}
for (const launcherOnly of ["magnifierEnabled", "restartButtonEnabled", "thpracTouchControlsEnabled", "frameLimit60Enabled"]) {
  assert.ok(!RUNTIME_CONFIGURE_OPTION_KEYS.includes(launcherOnly), `${launcherOnly}: Launcher preference leaked into Runtime configure vocabulary`);
}
for (const runtimeOwned of [
  "touchEnabled", "touchMovementMode", "touchSensitivity", "touchFocusMode",
  "doubleTapBombEnabled", "alwaysHitbox", "limitPresentationTo60",
  "focusHitboxEnabled", "multiplayerLocalPlayerVisibility",
]) assert.ok(RUNTIME_CONFIGURE_OPTION_KEYS.includes(runtimeOwned), `${runtimeOwned}: canonical Runtime option missing`);
for (const required of [
  "limitPresentationTo60", "touchEnabled", "touchMovementMode", "touchSensitivity",
  "touchFocusMode", "doubleTapBombEnabled", "alwaysHitbox", "oggDecodeMode",
]) assert.equal(RUNTIME_CONFIGURE_OPTION_BEHAVIOR[required].requirement, "required",
  `${required}: all-game wire obligation must be required`);
assert.equal(RUNTIME_CONFIGURE_OPTION_BEHAVIOR.focusHitboxEnabled.requirement, "optional");
assert.equal(RUNTIME_CONFIGURE_OPTION_BEHAVIOR.multiplayerLocalPlayerVisibility.requirement, "profile-required");
assert.equal(RUNTIME_CONFIGURE_OPTION_BEHAVIOR.netplayUrl.requirement, "profile-required");
assert.equal(RUNTIME_CONFIGURE_OPTION_BEHAVIOR.debugHarness.requirement, "diagnostic");
assert.ok(RUNTIME_PROTOCOL_COMMANDS.includes("configure"));
assert.ok(RUNTIME_PROTOCOL_OPTIONAL_COMMANDS.includes("retry-music"));
assert.ok(RUNTIME_PROTOCOL_EVENTS.includes("audio-health"));
assert.ok(RUNTIME_PROTOCOL_OPTIONAL_EVENTS.includes("notice"));
assert.deepEqual(
  Object.keys(RUNTIME_PROTOCOL_COMMAND_BEHAVIOR).sort(),
  [...RUNTIME_PROTOCOL_COMMANDS, ...RUNTIME_PROTOCOL_OPTIONAL_COMMANDS].sort(),
);
assert.equal(RUNTIME_PROTOCOL_COMMAND_BEHAVIOR.configure.phase, "prelaunch");
assert.equal(RUNTIME_PROTOCOL_COMMAND_BEHAVIOR.configure.response, "required");
assert.equal(RUNTIME_PROTOCOL_COMMAND_BEHAVIOR.configure.requirement, "required");
assert.equal(RUNTIME_PROTOCOL_COMMAND_BEHAVIOR.launch.response, "required");
assert.equal(RUNTIME_PROTOCOL_COMMAND_BEHAVIOR["touch-controls"].response, "optional");
assert.equal(RUNTIME_PROTOCOL_COMMAND_BEHAVIOR["touch-controls"].requirement, "required",
  "fire-and-forget touch delivery must not be confused with optional touch support");
assert.equal(RUNTIME_PROTOCOL_COMMAND_BEHAVIOR["direct-touch"].phase, "live");
assert.equal(RUNTIME_PROTOCOL_COMMAND_BEHAVIOR["retry-music"].requirement, "conditional");
assert.match(RUNTIME_PROTOCOL_COMMAND_BEHAVIOR["retry-music"].when, /music transfer/i);
assert.equal(RUNTIME_PROTOCOL_COMMAND_BEHAVIOR["thprac-mouse"].requirement, "profile-required");
assert.deepEqual(
  Object.keys(RUNTIME_PROTOCOL_EVENT_BEHAVIOR).sort(),
  [...RUNTIME_PROTOCOL_EVENTS, ...RUNTIME_PROTOCOL_OPTIONAL_EVENTS].sort(),
);
for (const event of ["ready", "first-frame", "runtime-info", "frame-health", "audio-health", "exit", "error"]) {
  assert.equal(RUNTIME_PROTOCOL_EVENT_BEHAVIOR[event].requirement, "required", `${event}: formal Runtime lifecycle/health event must be required`);
}
assert.equal(RUNTIME_PROTOCOL_EVENT_BEHAVIOR["first-frame"].cadence, "per-launch");
assert.equal(RUNTIME_PROTOCOL_EVENT_BEHAVIOR["frame-health"].cadence, "periodic");
assert.equal(RUNTIME_PROTOCOL_EVENT_BEHAVIOR["audio-health"].cadence, "periodic");

const ready = { protocol: "eagler-touhou/1", game: "th06", epoch: 7, event: "ready", saveRoot: "/savesth06" };
assert.deepEqual(parseRuntimeInboundMessage(ready, "th06", 7), ready);
assert.equal(parseRuntimeInboundMessage(ready, "th06", 8), null,
  "a queued message from a previous iframe navigation must not enter the current Runtime session");

const response = { protocol: "eagler-touhou/1", game: "th07", epoch: 11, request: "r1", ok: true, files: [] };
assert.deepEqual(parseRuntimeInboundMessage(response, "th07", 11), response);

const notice = { protocol: "eagler-touhou/1", game: "th08", epoch: 13, event: "notice", message: "Runtime notice" };
assert.deepEqual(parseRuntimeInboundMessage(notice, "th08", 13), notice);

for (const invalid of [
  null,
  [],
  { ...ready, protocol: "other/1" },
  { ...ready, game: "th07" },
  { ...ready, epoch: 0 },
  { ...ready, epoch: Number.NaN },
  { protocol: "eagler-touhou/1", game: "th06", event: "ready" },
  { ...ready, event: "unknown-event" },
  { protocol: "eagler-touhou/1", game: "th06", epoch: 7, request: "r1", ok: "yes" },
]) assert.equal(parseRuntimeInboundMessage(invalid, "th06"), null);

console.log(JSON.stringify({ runtimeProtocolModel: "PASS", boundary: "game-and-navigation-epoch" }));
