import {readdir} from 'node:fs/promises';
import {resolve, relative} from 'node:path';

export async function discoverNodeTests(directory) {
  const root = resolve(directory), selected = [];
  async function visit(current) {
    for (const entry of await readdir(current, {withFileTypes: true})) {
      const path = resolve(current, entry.name);
      if (entry.isDirectory() && entry.name !== 'fixtures') await visit(path);
      else if (entry.isFile() && /^(?:test-.+|.+\.test)\.mjs$/.test(entry.name))
        selected.push(relative(root, path).replaceAll('\\', '/'));
    }
  }
  await visit(root);
  return selected.sort();
}
