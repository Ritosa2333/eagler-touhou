import { spawn } from 'node:child_process';
import { existsSync } from 'node:fs';
import { resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { ensureClientServicesBuild } from '../lib/client-services-build.mjs';
import { reactFrontendArtifactDirectory, readReactFrontendArtifact } from '../lib/react-frontend-artifact.mjs';

const project = fileURLToPath(new URL('..', import.meta.url));
const result = await ensureClientServicesBuild({ force: process.argv.includes('--force') });
console.log(`Vitest inputs: ${result.sourceCount} TypeScript modules (${result.built ? 'compiled' : 'cached'})`);
// Repository contracts read real frontend artifacts. A fresh checkout builds
// this missing prerequisite; existing artifacts are validated and retained.
const directory = reactFrontendArtifactDirectory({ project });
if (!existsSync(directory)) {
  await new Promise((yes, no) => {
    const child = spawn(process.execPath, [resolve(project, 'node_modules/@react-router/dev/bin.cjs'), 'build'], {
      cwd: project,
      stdio: 'inherit',
    });
    child.once('error', no);
    child.once('exit', (code) => (code === 0 ? yes() : no(new Error(`Vitest frontend preparation failed (${code})`))));
  });
}
await readReactFrontendArtifact({ directory, expectedMountPath: process.env.EAGLER_REACT_MOUNT_PATH });
