import { existsSync } from "node:fs";
import { rm } from "node:fs/promises";
import { resolve } from "node:path";
import { run } from "./process.mjs";

function environmentPython(environment) {
  return process.platform === "win32"
    ? resolve(environment, "Scripts", "python.exe")
    : resolve(environment, "bin", "python");
}

async function importsWork(python) {
  if (!existsSync(python)) return false;
  const result = await run(python, ["-c", "import fontTools, soundfile, PIL"], {
    capture: true,
    allowFailure: true,
  });
  return result.code === 0;
}

async function isPython3(python) {
  if (!python) return false;
  if ((python.includes("/") || python.includes("\\")) && !existsSync(python)) return false;
  const result = await run(python, [
    "-c", "import sys; print(sys.version.split()[0]); raise SystemExit(0 if sys.version_info.major == 3 else 1)",
  ], { capture: true, allowFailure: true }).catch(() => ({ code: 1 }));
  return result.code === 0;
}

export async function ensurePythonEnvironment({ projectRoot, hostRoot, python = "python" }) {
  const environment = resolve(hostRoot, ".cache", "python");
  const privatePython = environmentPython(environment);
  if (await importsWork(privatePython)) return privatePython;

  if (existsSync(privatePython) && await isPython3(privatePython)) {
    console.log("[Build] Repairing Python build dependencies in the existing private environment");
  } else if (!await isPython3(python)) {
    throw new Error(`Python 3 is required for self-host asset preparation. Configured interpreter: ${python}`);
  } else {
    await rm(environment, { recursive: true, force: true });
    console.log(`[Build] Creating Python environment: ${environment}`);
    await run(python, ["-m", "venv", environment]);
  }

  console.log("[Build] Preparing Python build dependencies");
  await run(privatePython, [
    "-m", "pip", "install", "--disable-pip-version-check",
    "-r", resolve(projectRoot, "host", "requirements.txt"),
  ]);
  if (!await importsWork(privatePython)) {
    throw new Error("The private Python environment cannot import fontTools, soundfile and PIL after installation");
  }
  return privatePython;
}
