import {readFileSync, readdirSync} from 'node:fs';
import {relative, resolve, sep} from 'node:path';
import {createHash} from 'node:crypto';

export const printingRuntimeInputs = process.argv.includes('--print-inputs');
export function headerInputs(...directories) {
  return directories.flatMap(directory => readdirSync(directory, {withFileTypes: true}).flatMap(entry => {
    const path = resolve(directory, entry.name);
    return entry.isDirectory() ? headerInputs(path) : /\.(?:h|hpp|inc)$/.test(entry.name) ? [path] : [];
  }));
}
export function printRuntimeInputs({workspace, root, sources, headers, flags}) {
  if (!printingRuntimeInputs) return;
  const name = path => relative(workspace, resolve(root, path)).split(sep).join('/');
  const units = [...new Set(sources.map(name))].sort();
  const inputs = [...new Set([...units, ...headers.map(name)])].sort();
  const files = Object.fromEntries(inputs.map(path => {
    const bytes = readFileSync(resolve(workspace, path));
    return [path, {bytes: bytes.length, sha256: createHash('sha256').update(bytes).digest('hex')}];
  }));
  console.log(JSON.stringify({schema: 'eagler-touhou/runtime-inputs/1',
    directory: workspace.split(sep).at(-1), translationUnits: units, inputs: files,
    compilerFlags: flags}, null, 2));
  process.exit(0);
}
