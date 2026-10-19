#!/usr/bin/env node
import { resolveRuntimeBuild } from "../lib/runtime-build-profiles.mjs";

const args = Object.fromEntries(process.argv.slice(2).map(value => {
  const index = value.indexOf("=");
  if (!value.startsWith("--") || index < 3) throw new Error(`invalid argument: ${value}`);
  return [value.slice(2, index), value.slice(index + 1)];
}));

function optionalBoolean(name) {
  if (!(name in args)) return undefined;
  if (args[name] === "true" || args[name] === "on" || args[name] === "ON") return true;
  if (args[name] === "false" || args[name] === "off" || args[name] === "OFF") return false;
  throw new Error(`--${name} must be true/false or on/off`);
}

if (!args.game) throw new Error("usage: resolve-runtime-build.mjs --game=GAME [--variant=normal] [--thcrap=on|off] [--thprac=on|off] [--asset-root=PATH] [--runtime-extension=PATH]");
const plan = await resolveRuntimeBuild({
  game: args.game,
  variant: args.variant || "normal",
  thcrap: optionalBoolean("thcrap"),
  thprac: optionalBoolean("thprac"),
  assetRoot: args["asset-root"] || null,
  runtimeExtension: Object.hasOwn(args, "runtime-extension") ? args["runtime-extension"] : null,
});
process.stdout.write(`${JSON.stringify(plan)}\n`);

