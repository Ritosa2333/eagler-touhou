import {spawn} from 'node:child_process';
import {fileURLToPath} from 'node:url';
import {ensureI18nBuild} from '../lib/i18n-build.mjs';

if (process.argv.slice(2).some(argument => argument !== '--full')) throw new Error('usage: node scripts/prepare-tests.mjs [--full]');
await ensureI18nBuild();
// The unit path needs messages. Publication and repository gates can explicitly
// request the complete compiler/frontend preparation.
if (process.argv.includes('--full')) {
  const script = fileURLToPath(new URL('./build-launcher.mjs', import.meta.url));
  await new Promise((resolve, reject) => {
    const child = spawn(process.execPath, [script], {stdio: 'inherit'});
    child.once('error', reject);
    child.once('exit', code => code === 0 ? resolve() : reject(new Error(`Full test preparation failed (${code})`)));
  });
}
