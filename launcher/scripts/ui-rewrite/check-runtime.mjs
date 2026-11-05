import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {assertSupportedBun} from '../../host/lib/bun-environment.mjs';

/** Source acceptance uses native erasable TypeScript. Packaged self-host tools
 * consume compiled artifacts and do not require this test-only runtime feature. */
export function assertUiRewriteRuntime(version = process.versions.node) {
  if (process.versions.bun && version === process.versions.node) return assertSupportedBun();
  const match = /^(\d+)\.(\d+)\.\d+(?:-|$)/.exec(version);
  const major = Number(match?.[1]), minor = Number(match?.[2]);
  if (!(major >= 24 || major === 23 && minor >= 6 || major === 22 && minor >= 18)) {
    throw new Error(`The React test/fixture lane requires Node.js 22.18+, 23.6+, or 24+; found ${version}. Node.js 24 is the verified source-build environment.`);
  }
}

/** Both the CLI and every worker must use the selected frontend runtime. */
export function assertFrontendRuntime(expected = process.env.EAGLER_FRONTEND_BUN_VERSION) {
  assertSupportedBun();
  if (expected && process.versions.bun !== expected)
    throw new Error(`Frontend worker Bun ${process.versions.bun} differs from CLI Bun ${expected}`);
  return process.versions.bun;
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) assertUiRewriteRuntime();
