import assert from "node:assert/strict";

import {
  resolveEffectiveMusicMode,
  resolveMusicAvailability,
  resolveMusicMode,
} from "../.cache/build/browser/assets/launcher/music-availability.mjs";

const cases = [
  [{ requested: "none", localOgg: true }, "none"],
  [{ requested: "ogg-full", audio: false, localOgg: true }, "none"],
  [{ requested: "midi", explicit: true, localOgg: true }, "midi"],
  [{ requested: "midi", localOgg: true }, "ogg-stream"],
  [{ requested: "midi", remoteOgg: true, preferOgg: false }, "midi"],
  [{ requested: "ogg-full", explicit: true, remoteOgg: true }, "ogg-full"],
  [{ requested: "ogg-full", explicit: true }, "midi"],
  [{ requested: "ogg-full", explicit: true, midi: false }, "none"],
  [{ requested: "midi", explicit: true, midi: false, localOgg: true }, "ogg-stream"],
  [{ requested: "bogus", midi: false, remoteOgg: true }, "ogg-stream"],
];
for (const [input, expected] of cases) {
  assert.equal(resolveMusicMode(input), expected, JSON.stringify(input));
}

let combinations = 0;
for (const requested of ["ogg-stream", "ogg-full", "midi", "none", "bogus"])
for (const explicit of [false, true])
for (const audio of [false, true])
for (const midi of [false, true])
for (const localOgg of [false, true])
for (const remoteOgg of [false, true])
for (const preferOgg of [false, true]) {
  const input = { requested, explicit, audio, midi, localOgg, remoteOgg, preferOgg };
  const mode = resolveMusicMode(input);
  if (!audio || requested === "none") assert.equal(mode, "none");
  if (mode === "midi") assert.ok(audio && midi);
  if (mode.startsWith("ogg-")) assert.ok(audio && (localOgg || remoteOgg));
  if (audio && explicit && requested === "midi" && midi) assert.equal(mode, "midi");
  if (audio && explicit && requested.startsWith("ogg-") && (localOgg || remoteOgg)) assert.equal(mode, requested);
  combinations++;
}

const files = {};
const installed = { revision: "r1", oggFileIds: ["track1", "track2"], files };
const effective = overrides => resolveEffectiveMusicMode({
  requested: "ogg-full",
  explicit: true,
  audio: true,
  midiAvailable: true,
  importServer: false,
  remoteOggAdvertised: true,
  remoteRevision: null,
  installed,
  ...overrides,
});

assert.equal(effective({}), "midi");
files.track1 = { objectId: "a" };
assert.equal(effective({ remoteRevision: "r1" }), "ogg-full");
assert.equal(effective({ importServer: true, remoteRevision: "r1" }), "midi");
files.track2 = { objectId: "b" };
assert.equal(effective({ importServer: true }), "ogg-full");
assert.equal(effective({ importServer: true, remoteOggAdvertised: false }), "ogg-full",
  "fully installed local OGG must not depend on Host-published OGG/WAV capability");
assert.deepEqual(resolveMusicAvailability({
  audio: true,
  midiAvailable: true,
  importServer: true,
  remoteOggAdvertised: false,
  installed,
}), {
  audio: true,
  midi: true,
  localOgg: true,
  remoteOgg: false,
  ogg: true,
}, "local Package OGG must be exposed to both single-player and multiplayer UI");

const progressiveInstalled = {
  revision: "r1",
  oggFileIds: ["track1", "track2", "track3", "track4"],
  files: {
    track1: { objectId: "a" },
    track2: { objectId: "b" },
  },
};
assert.deepEqual(resolveMusicAvailability({
  audio: true,
  midiAvailable: true,
  importServer: true,
  remoteOggAdvertised: false,
  installed: progressiveInstalled,
}), {
  audio: true,
  midi: true,
  localOgg: true,
  remoteOgg: false,
  ogg: true,
}, "the first two persisted OGG tracks are the local startup barrier; later progressive tracks may still be missing");
assert.equal(resolveEffectiveMusicMode({
  requested: "ogg-stream",
  explicit: true,
  audio: true,
  midiAvailable: true,
  importServer: true,
  remoteOggAdvertised: false,
  installed: progressiveInstalled,
}), "ogg-stream", "leaving during progressive OGG installation must not change the effective music option to MIDI/none");
delete progressiveInstalled.files.track2;
assert.equal(resolveEffectiveMusicMode({
  requested: "ogg-stream",
  explicit: true,
  audio: true,
  midiAvailable: true,
  importServer: true,
  remoteOggAdvertised: false,
  installed: progressiveInstalled,
}), "midi", "one persisted startup track is not enough to claim local OGG launch readiness");

assert.equal(effective({ requested: "none" }), "none");
assert.equal(effective({ remoteRevision: "other", installed: { ...installed, files: {} } }), "midi");
assert.equal(effective({
  requested: "ogg-stream",
  explicit: true,
  remoteOggAdvertised: false,
  remoteRevision: "r1",
  installed: { ...installed, files: {} },
}), "ogg-stream", "matching Release Catalog revision must preserve remote OGG capability even when Host Manifest does not duplicate music.ogg");
assert.equal(effective({
  requested: "midi",
  explicit: false,
  remoteRevision: "r1",
  installed: { ...installed, files: { track1: { objectId: "a" } } },
}), "midi", "a partial install may use matching remote OGG when explicitly requested but must not force an implicit MIDI choice back to OGG");
assert.equal(effective({ midiAvailable: false, remoteRevision: "r1", installed: { ...installed, files: {} } }), "ogg-full",
  "matching remote OGG remains available even when MIDI is unsupported");
assert.equal(effective({
  midiAvailable: false,
  remoteRevision: "other",
  installed: { ...installed, files: {} },
}), "none", "without local OGG, MIDI, or a matching remote generation there is no music capability");
assert.equal(resolveEffectiveMusicMode({
  requested: "ogg-stream",
  explicit: true,
  audio: true,
  midiAvailable: true,
  importServer: false,
  remoteOggAdvertised: true,
  installed: null,
}), "ogg-stream");

console.log(JSON.stringify({
  musicAvailability: "PASS",
  combinations,
  preference: "fallback-does-not-overwrite",
  remoteGeneration: "revision-bound",
}));
