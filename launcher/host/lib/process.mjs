import { spawn } from "node:child_process";

export function run(command, args = [], { cwd, env, capture = false, allowFailure = false } = {}) {
  return new Promise((resolve, reject) => {
    const child = spawn(command, args, {
      cwd,
      env: env ? { ...process.env, ...env } : process.env,
      windowsHide: true,
      stdio: capture ? ["ignore", "pipe", "pipe"] : "inherit",
    });
    const stdout = [];
    const stderr = [];
    if (capture) {
      child.stdout.on("data", chunk => stdout.push(chunk));
      child.stderr.on("data", chunk => stderr.push(chunk));
    }
    child.once("error", error => reject(error));
    child.once("close", code => {
      const result = {
        code: code ?? 1,
        stdout: capture ? Buffer.concat(stdout).toString("utf8").trim() : "",
        stderr: capture ? Buffer.concat(stderr).toString("utf8").trim() : "",
      };
      if (result.code === 0 || allowFailure) {
        resolve(result);
        return;
      }
      const detail = result.stderr || result.stdout;
      reject(new Error(`${command} exited with code ${result.code}${detail ? `: ${detail}` : ""}`));
    });
  });
}

export async function commandExists(command) {
  const probe = process.platform === "win32"
    ? await run("where", [command], { capture: true, allowFailure: true })
    : await run("sh", ["-c", `command -v -- "$1"`, "sh", command], { capture: true, allowFailure: true });
  return probe.code === 0 && !!probe.stdout;
}
