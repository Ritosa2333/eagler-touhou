import {readFile, appendFile} from 'node:fs/promises';
import {execFileSync} from 'node:child_process';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';

export function affectedComponents(files, workspace, builds) {
  const runtimes = [...new Set(Object.entries(workspace.repositories)
    .filter(([key, value]) => value.startsWith('ports/')).map(([, value]) => value))];
  const services=Object.values(workspace.repositories).filter(value=>value.startsWith('services/'));
  const all = ['launcher', workspace.repositories.common, ...runtimes, ...services];
  if (files === null) return all.sort();
  const affected = new Set(['launcher']);
  if (files.some(file => file.startsWith('common/') || file.startsWith('tools/') ||
      file.startsWith('.github/') || file.startsWith('launcher/config/'))) return all.sort();
  for (const directory of runtimes) if (files.some(file => file === directory || file.startsWith(directory + '/'))) affected.add(directory);
  for (const directory of services) if (files.some(file => file === directory || file.startsWith(directory + '/'))) affected.add(directory);
  // Ordinary and MP fixes require semantic review together, even when the
  // source branches were imported into two directories.
  for (const entry of Object.values(builds.games)) {
    const owners = [...new Set(Object.values(entry.variants).map(variant =>
      workspace.repositories[variant.workspaceRepository ?? entry.workspaceRepository]))];
    if (owners.some(owner => affected.has(owner))) owners.forEach(owner => affected.add(owner));
  }
  return [...affected].sort();
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  const root = resolve(import.meta.dirname, '../..');
  const workspace = JSON.parse(await readFile(resolve(root, 'launcher/config/workspace.json'), 'utf8'));
  const builds = JSON.parse(await readFile(resolve(root, 'launcher/config/runtime-builds.json'), 'utf8'));
  const base = process.env.EAGLER_CI_BASE || '';
  const files = /^[a-f0-9]{40,64}$/i.test(base) && !/^0+$/.test(base)
    ? execFileSync('git', ['diff', '--name-only', '--no-renames', '-z', base, 'HEAD', '--'], {cwd: root, encoding: 'utf8'}).split('\0').filter(Boolean)
    : null;
  const affected = affectedComponents(files, workspace, builds);
  const result = JSON.stringify(affected);
  if (process.env.GITHUB_OUTPUT) {
    const checkout = [...new Set(['tools', 'launcher/config', 'common', ...affected])].join('\n');
    await appendFile(process.env.GITHUB_OUTPUT, `affected=${result}\ncheckout<<EAGLER_SCOPE\n${checkout}\nEAGLER_SCOPE\n`);
  }
  console.log(result);
}
