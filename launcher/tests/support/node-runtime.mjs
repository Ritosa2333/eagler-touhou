import {existsSync} from 'node:fs';
import {delimiter, resolve} from 'node:path';
import {execFileSync} from 'node:child_process';

let executable;
/** Game-owned node:test suites keep their Node runner outside frontend Vitest. */
export function nativeNodeExecutable() {
  if (executable) return executable;
  const candidates = process.versions.bun
    ? (process.env.PATH ?? '').split(delimiter).filter(Boolean).map(path => resolve(path.replace(/^"|"$/g, ''), process.platform === 'win32' ? 'node.exe' : 'node'))
    : [process.execPath];
  for (const path of new Set(candidates)) {
    if (!existsSync(path)) continue;
    try {
      const versions = JSON.parse(execFileSync(path, ['-p', 'JSON.stringify(process.versions)'], {encoding: 'utf8', windowsHide: true}));
      if (!versions.bun && Number.parseInt(versions.node, 10) >= 22) return executable = path;
    } catch { /* A missing or incompatible PATH entry is not a Node runner. */ }
  }
  throw new Error('Workspace game tests require Node.js >=22 on PATH, separately from Bun.');
}
