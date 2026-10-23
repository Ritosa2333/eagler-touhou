import {execFileSync} from 'node:child_process';
import {createRequire} from 'node:module';
import {pathToFileURL} from 'node:url';

/** Reuse either the JS install or Python Playwright's existing bundled driver.
 * This helper never downloads a browser or adds a project dependency. */
export async function loadPlaywright({preferPython = false} = {}) {
  try {if (!preferPython) {
    const require = createRequire(new URL('../../launcher/package.json',import.meta.url));
    const imported = await import(pathToFileURL(require.resolve('playwright')).href);
    // Recent CommonJS bundles expose the driver through the default export.
    return imported.chromium ? imported : imported.default;
  }}
  catch (error) {if (!['ERR_MODULE_NOT_FOUND','MODULE_NOT_FOUND'].includes(error.code)) throw error;}
  const module = execFileSync(process.env.PYTHON || 'python', ['-c',
    "import pathlib,playwright; print(pathlib.Path(playwright.__file__).parent/'driver'/'package'/'index.mjs')"], {encoding:'utf8',windowsHide:true}).trim();
  return import(pathToFileURL(module).href);
}
