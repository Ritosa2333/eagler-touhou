import {readFile} from 'node:fs/promises';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';
import {assertSupportedBun, ensureBunDependencies} from '../host/lib/bun-environment.mjs';

assertSupportedBun();
const project = fileURLToPath(new URL('..', import.meta.url));
const manifest = JSON.parse(await readFile(resolve(project, 'package.json'), 'utf8'));
await ensureBunDependencies(project, Object.keys({...manifest.dependencies, ...manifest.devDependencies}).sort());
console.log(`Locked frontend dependencies verified with Bun ${process.versions.bun}`);
