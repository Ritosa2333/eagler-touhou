import { execFileSync } from 'node:child_process';
import { existsSync } from 'node:fs';
import { delimiter, resolve } from 'node:path';

const NODE_INFO_SCRIPT =
  'process.stdout.write(JSON.stringify({version:process.versions.node,bun:process.versions.bun??null}))';
let cachedNodeExecutable;

function inspectNodeExecutable(path) {
  const executable = resolve(path);
  if (!existsSync(executable)) return null;
  try {
    const info = JSON.parse(
      execFileSync(executable, ['-e', NODE_INFO_SCRIPT], {
        encoding: 'utf8',
        timeout: 5_000,
        windowsHide: true,
      }),
    );
    const major = Number.parseInt(String(info.version).split('.')[0], 10);
    if (info.bun || !Number.isInteger(major) || major < 22) return null;
    return executable;
  } catch {
    return null;
  }
}

/** Resolve a real Node executable for native build helpers, never Bun's node shim. */
export function nodeHostExecutable() {
  if (cachedNodeExecutable) return cachedNodeExecutable;

  const configured = process.env.EAGLER_BUILD_LOCK_NODE?.trim();
  if (configured) {
    cachedNodeExecutable = inspectNodeExecutable(configured);
    if (!cachedNodeExecutable) {
      throw new Error(`EAGLER_BUILD_LOCK_NODE must point to a real Node.js >=22 executable: ${configured}`);
    }
    return cachedNodeExecutable;
  }

  const candidates = process.versions.bun
    ? (process.env.PATH ?? '')
        .split(delimiter)
        .filter(Boolean)
        .map((directory) =>
          resolve(directory.replace(/^"|"$/g, ''), process.platform === 'win32' ? 'node.exe' : 'node'),
        )
    : [process.execPath];
  for (const candidate of new Set(candidates)) {
    const executable = inspectNodeExecutable(candidate);
    if (executable) {
      cachedNodeExecutable = executable;
      return executable;
    }
  }
  throw new Error(
    'Bun build locks require a real Node.js >=22 executable on PATH; set EAGLER_BUILD_LOCK_NODE to its path.',
  );
}
