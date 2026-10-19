#!/usr/bin/env node
import { existsSync } from "node:fs";
import { spawnSync } from "node:child_process";
import { join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const { HOST_WORKSPACE_GAMES, inspectHostWorkspace } = await import("../lib/host-workspace.mjs");
const args = Object.fromEntries(process.argv.slice(2).map(value => {
  const split = value.indexOf("=");
  if (!value.startsWith("--") || split < 3) throw new Error(`invalid argument: ${value}`);
  return [value.slice(2, split), value.slice(split + 1)];
}));
const root = resolve(args.root || project);
const postBuild = args["post-build"] === "1" || args["post-build"] === "true";
const postImport = args["post-import"] === "1" || args["post-import"] === "true";
const colorEnabled = args.json !== "1" && args.json !== "true" &&
  process.env.NO_COLOR == null && process.env.TERM !== "dumb" &&
  (process.stdout.isTTY || (process.env.FORCE_COLOR != null && process.env.FORCE_COLOR !== "0"));
const deploymentDoc = existsSync(join(root, "SELF-HOSTING-REFERENCE.md"))
  ? "SELF-HOSTING-REFERENCE.md"
  : "docs/SELF_HOSTING_REFERENCE.md";

const ansi = Object.freeze({ green: "\x1b[32m", yellow: "\x1b[33m", red: "\x1b[31m", reset: "\x1b[0m" });
function color(text, name) {
  return colorEnabled ? `${ansi[name]}${text}${ansi.reset}` : text;
}
const good = text => color(text, "green");
const warn = text => color(text, "yellow");
const bad = text => color(text, "red");

function doctorValue(value) {
  if (["OK", "READY", "CACHED", "CONFIGURED"].includes(value)) return good(value);
  if (["MISSING", "UNSUPPORTED", "NOT INSTALLED", "FAILED"].includes(value)) return bad(value);
  if (["NOT CONFIGURED", "DOWNLOAD REQUIRED", "INSTALL REQUIRED", "READY WITH WARNINGS"].includes(value)) return warn(value);
  return value;
}

function printWarning(message) {
  console.log(warn(`WARNING: ${message}`));
  if (message.startsWith("WebSocket relay is not configured")) {
    console.log("  Action: Set netplay.relay in eagler-touhou.config.json to your wss:// relay URL, then rebuild.");
    console.log(`  Docs:   ${deploymentDoc} - "WebSocket relay"`);
  } else if (message.startsWith("External import source is not configured")) {
    console.log("  Action: Set externalImportSource.url in eagler-touhou.config.json if you want the Launcher to offer an administrator-provided package download link.");
    console.log(`  Docs:   ${deploymentDoc} - "Host configuration"`);
  }
}

function run(command, argv) {
  const result = spawnSync(command, argv, { encoding: "utf8", windowsHide: true });
  return {
    available: !result.error || result.error.code !== "ENOENT",
    ok: !result.error && result.status === 0,
    stdout: String(result.stdout || "").trim(),
    stderr: String(result.stderr || "").trim(),
  };
}

function inspectBuildEnvironment() {
  const nodeMajor = Number.parseInt(process.versions.node.split(".")[0], 10);
  const privatePython = process.platform === "win32"
    ? join(root, ".cache", "python", "Scripts", "python.exe")
    : join(root, ".cache", "python", "bin", "python");
  const configuredPython = args.python || "python";
  const privateProbe = existsSync(privatePython)
    ? run(privatePython, ["-c", "import sys; print('.'.join(map(str, sys.version_info[:3]))); raise SystemExit(0 if sys.version_info.major == 3 else 1)"])
    : null;
  const pythonCommand = privateProbe?.ok ? privatePython : configuredPython;
  const pythonProbe = privateProbe?.ok
    ? privateProbe
    : run(configuredPython, ["-c", "import sys; print('.'.join(map(str, sys.version_info[:3]))); raise SystemExit(0 if sys.version_info.major == 3 else 1)"]);
  const privateReady = existsSync(privatePython) && run(privatePython, ["-c", "import fontTools, soundfile, PIL"]).ok;

  let thtk;
  if (process.platform === "win32") {
    const toolRoot = join(root, ".cache", "tools", "thtk", "12", "extracted", "thtk-bin-12");
    thtk = existsSync(join(toolRoot, "thdat.exe")) && existsSync(join(toolRoot, "thmsg.exe"))
      ? "CACHED"
      : "DOWNLOAD REQUIRED";
  } else {
    const thdat = run("thdat", ["--help"]);
    const thmsg = run("thmsg", ["--help"]);
    thtk = thdat.available && thmsg.available ? "READY" : "NOT INSTALLED";
  }

  return Object.freeze({
    node: Object.freeze({
      status: Number.isInteger(nodeMajor) && nodeMajor >= 22 ? "OK" : "UNSUPPORTED",
      version: process.version,
    }),
    python: Object.freeze({
      status: pythonProbe.ok ? "OK" : (pythonProbe.available ? "UNSUPPORTED" : "MISSING"),
      version: pythonProbe.ok ? pythonProbe.stdout : "",
      command: pythonCommand,
    }),
    pythonPackages: privateReady ? "READY" : "INSTALL REQUIRED",
    thtk,
  });
}

const buildEnvironment = inspectBuildEnvironment();
try {
  const result = await inspectHostWorkspace(root, { music: args.music || "midi,ogg" });
  if (args.json === "1" || args.json === "true") {
    console.log(JSON.stringify({ ...result, buildEnvironment }));
  } else {
    const relayConfigured = !!result.hostConfig.netplay.relay;
    const externalConfigured = !!result.hostConfig.externalImportSource.url;
    console.log("Eagler Touhou Host Check\n");
    console.log("Build environment");
    console.log(`  ${"Node.js".padEnd(24)}${doctorValue(buildEnvironment.node.status)} ${buildEnvironment.node.version}`);
    console.log(`  ${"Python".padEnd(24)}${doctorValue(buildEnvironment.python.status)}${buildEnvironment.python.version ? ` ${buildEnvironment.python.version}` : ""}`);
    console.log(`  ${"Python packages".padEnd(24)}${doctorValue(buildEnvironment.pythonPackages)}`);
    console.log(`  ${"thtk 12".padEnd(24)}${doctorValue(buildEnvironment.thtk)}`);
    console.log("\nHost input");
    for (const game of HOST_WORKSPACE_GAMES) console.log(`  ${game.toUpperCase().padEnd(24)}${good("OK")}`);
    console.log(`  ${"Runtime Release".padEnd(24)}${good("OK")}`);
    console.log("\nNetplay");
    console.log(`  ${"WebSocket relay".padEnd(24)}${doctorValue(relayConfigured ? "CONFIGURED" : "NOT CONFIGURED")}`);
    console.log(`  ${"TURN relay".padEnd(24)}SERVER-MANAGED / NOT VERIFIED`);
    console.log("\nExternal content");
    console.log(`  ${"External import source".padEnd(24)}${doctorValue(externalConfigured ? "CONFIGURED" : "NOT CONFIGURED")}`);
    let generatedOutputReady = true;
    if (postBuild || postImport) {
      const siteReady = existsSync(join(result.site, "deployment.json")) &&
        existsSync(join(result.site, "release-manifest.json")) &&
        existsSync(join(result.site, "checksums.txt"));
      generatedOutputReady = siteReady;
      console.log("\nGenerated output");
      console.log(`  ${"Static site".padEnd(24)}${doctorValue(siteReady ? "READY" : "MISSING")}`);
      if (postImport) {
        const importSiteReady = existsSync(join(result.importSite, "deployment.json")) &&
          existsSync(join(result.importSite, "release-manifest.json")) &&
          existsSync(join(result.importSite, "checksums.txt"));
        const packageStates = Object.fromEntries(HOST_WORKSPACE_GAMES.map(game => [
          game,
          existsSync(join(result.importPackages, `${game}.zip`)),
        ]));
        generatedOutputReady = generatedOutputReady && importSiteReady && Object.values(packageStates).every(Boolean);
        console.log(`  ${"Import site".padEnd(24)}${doctorValue(importSiteReady ? "READY" : "MISSING")}`);
        for (const game of HOST_WORKSPACE_GAMES) {
          console.log(`  ${`${game.toUpperCase()} Import ZIP`.padEnd(24)}${doctorValue(packageStates[game] ? "READY" : "MISSING")}`);
        }
      }
    }
    const buildBlocked = buildEnvironment.node.status !== "OK" || buildEnvironment.python.status !== "OK" ||
      buildEnvironment.thtk === "NOT INSTALLED" || !generatedOutputReady;
    const status = buildBlocked ? "FAILED" : result.warnings.length ? "READY WITH WARNINGS" : "READY";
    console.log(`\n${"Result".padEnd(26)}${doctorValue(status)}`);
    if (buildBlocked || result.warnings.length) console.log("");
    if (buildEnvironment.node.status !== "OK") {
      console.log(bad("ERROR: Node.js 22 or newer is required for self-hosting."));
    }
    if (buildEnvironment.python.status !== "OK") {
      console.log(bad("ERROR: Python 3 is required. Install Python and ensure it is available on PATH."));
    }
    if (buildEnvironment.thtk === "NOT INSTALLED") {
      console.log(bad("ERROR: thdat and thmsg from thtk 12 are required on PATH on this platform."));
    }
    if ((postBuild || postImport) && !generatedOutputReady) {
      console.log(bad("ERROR: One or more generated outputs are incomplete or missing."));
    }
    result.warnings.forEach((warning, index) => {
      if (index) console.log("");
      printWarning(warning);
    });
    if (!buildBlocked && (postBuild || postImport)) {
      const operation = postImport ? "Import" : "Build";
      const output = postImport ? result.importPackages : result.site;
      console.log("");
      console.log(result.warnings.length ? warn(`${operation} completed with warnings.`) : good(`${operation} ready.`));
      console.log(`Output: ${output}`);
    }
    if (buildBlocked) process.exitCode = 1;
  }
} catch (error) {
  if (args.json === "1" || args.json === "true") throw error;
  console.error("Eagler Touhou Host Check\n");
  console.error("Build environment");
  console.error(`  ${"Node.js".padEnd(22)}${buildEnvironment.node.status} ${buildEnvironment.node.version}`);
  console.error(`  ${"Python".padEnd(22)}${buildEnvironment.python.status}${buildEnvironment.python.version ? ` ${buildEnvironment.python.version}` : ""}`);
  console.error(`  ${"Python packages".padEnd(22)}${buildEnvironment.pythonPackages}`);
  console.error(`  ${"thtk 12".padEnd(22)}${buildEnvironment.thtk}\n`);
  console.error(`Result                  ${bad("FAILED")}`);
  console.error(bad(`ERROR: ${error.message || error}`));
  process.exitCode = 1;
}
