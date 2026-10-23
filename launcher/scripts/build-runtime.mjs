import { planRuntimeSourceBuild } from "../lib/runtime-source-plan.mjs";
import { workspaceRoot } from "../lib/workspace-layout.mjs";
import { mkdir, writeFile, readFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import {executeRuntimeSourceBuild,runtimeBuildInputIdentity} from '../lib/runtime-build-evidence.mjs';

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
const identify = () => runtimeBuildInputIdentity(plan,workspaceRoot());
plan.sources = identify();
if (dryRun) console.log(JSON.stringify(plan, null, 2));
else {
  const receipt = resolve(workspaceRoot(), "artifacts/validation/source-builds", `${plan.game}-${plan.variant}.json`);
  const runtimeReceipt = receipt.replace(/\.json$/, '.runtime.json');
  let previousReceipt,previousPlan;
  try {
    previousReceipt=JSON.parse(await readFile(runtimeReceipt,'utf8'));
    previousPlan=JSON.parse(await readFile(receipt,'utf8'));
  }
  catch (error) {if (error.code !== 'ENOENT') throw error;}
  const runtimeEvidence = await executeRuntimeSourceBuild(plan, workspaceRoot(), {previousReceipt,previousPlan});
  if (JSON.stringify(identify()) !== JSON.stringify(plan.sources)) throw new Error("Source changed during compilation; discard this build and rebuild.");
  await mkdir(dirname(receipt), { recursive: true });
  await writeFile(runtimeReceipt, JSON.stringify(runtimeEvidence, null, 2) + '\n');
  await writeFile(receipt, JSON.stringify({ schema: "eagler-touhou/source-build/1", ...plan }, null, 2) + "\n");
  console.log(`Build source receipt: ${receipt}`);
  console.log(`Runtime acceptance receipt: ${runtimeReceipt}`);
}
