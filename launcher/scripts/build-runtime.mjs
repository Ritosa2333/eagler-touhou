import { spawn } from "node:child_process";
import { planRuntimeSourceBuild } from "../lib/runtime-source-plan.mjs";
import { sourceIdentity } from "../lib/release-manifest.mjs";
import { workspaceRoot } from "../lib/workspace-layout.mjs";
import { mkdir, writeFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";

const options = {};
const names = new Map([["game", "game"], ["variant", "variant"], ["source", "source"], ["common", "common"],
  ["netplay-common", "netplayCommon"], ["emsdk", "emsdk"], ["emscripten-version", "emscriptenVersion"], ["python", "python"], ["jobs", "jobs"],
  ["cmake", "cmake"], ["ninja", "ninja"], ["assets", "assets"], ["font", "font"], ["imgui", "imgui"],
  ["runtime-extension", "runtimeExtension"], ["build-directory", "buildDirectory"], ["thcrap", "thcrap"], ["thprac", "thprac"]]);
let dryRun = false;
for (const arg of process.argv.slice(2)) {
  if (arg === "--dry-run") { dryRun = true; continue; }
  const match = arg.match(/^--([^=]+)=(.+)$/);
  const key = match && names.get(match[1]);
  if (!key || Object.hasOwn(options, key)) throw new Error(`invalid or duplicate argument: ${arg}`);
  if (["thcrap", "thprac"].includes(key)) {
    if (!["on", "off"].includes(match[2])) throw new Error(`${match[1]} must be on or off`);
    options[key] = match[2] === "on";
  } else options[key] = match[2];
}
if (!options.game) throw new Error("usage: node scripts/build-runtime.mjs --game=thXX [--variant=normal|multiplayer] [--emsdk=PATH] [--dry-run]");
const plan = await planRuntimeSourceBuild(options);
const identify = async () => ({
  source: await sourceIdentity(plan.source),
  common: await sourceIdentity(plan.common),
  netplayCommon: await sourceIdentity(plan.environment.EAGLER_COMMON_NETPLAY_ROOT),
});
plan.sources = await identify();
if (dryRun) console.log(JSON.stringify(plan, null, 2));
else {
  for (const step of plan.steps) await new Promise((accept, reject) => {
    const child = spawn(step.command, step.args, { cwd: step.cwd, shell: false, windowsHide: true,
      stdio: "inherit", env: { ...process.env, ...plan.environment } });
    child.once("error", reject);
    child.once("exit", code => code === 0 ? accept() : reject(new Error(`${plan.game}/${plan.variant} build failed (${code})`)));
  });
  if (JSON.stringify(await identify()) !== JSON.stringify(plan.sources)) throw new Error("Source changed during compilation; discard this build and rebuild.");
  const receipt = resolve(workspaceRoot(), "artifacts/validation/source-builds", `${plan.game}-${plan.variant}.json`);
  await mkdir(dirname(receipt), { recursive: true });
  await writeFile(receipt, JSON.stringify({ schema: "eagler-touhou/source-build/1", ...plan }, null, 2) + "\n");
  console.log(`Build source receipt: ${receipt}`);
}
