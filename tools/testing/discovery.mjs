import {readdir} from 'node:fs/promises';
import {relative, resolve} from 'node:path';

export function isNodeTest(file) {
  const parts = file.split('/');
  if (parts.includes('fixtures')) return false;
  const name = parts.at(-1);
  return /^test-.+\.mjs$/.test(name) || /^.+\.test\.mjs$/.test(name);
}

/** Recursively select executable Node test modules, relative to the search root. */
export async function discoverNodeTests(directory) {
  const root = resolve(directory), selected = [];
  const visit = async current => {
    for (const entry of await readdir(current, {withFileTypes:true})) {
      if (entry.isDirectory()) {
        if (entry.name !== 'fixtures') await visit(resolve(current, entry.name));
      } else if (entry.isFile()) {
        const path = resolve(current, entry.name);
        const name = relative(root, path).replaceAll('\\', '/');
        if (isNodeTest(name)) selected.push(name);
      }
    }
  };
  await visit(root);
  return selected.sort();
}
