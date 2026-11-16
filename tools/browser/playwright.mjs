import {execFileSync} from 'node:child_process';
import {createRequire} from 'node:module';
import {pathToFileURL} from 'node:url';

/** Reuse either the JS install or Python Playwright's existing bundled driver.
 * This helper never downloads a browser or adds a project dependency. */
export async function loadPlaywright({preferPython = false} = {}) {
  if (!preferPython) {
    const require = createRequire(new URL('../../launcher/package.json',import.meta.url));
    // `playwright` is the full package; the th13 shot image installs
    // `playwright-core` only (system chromium via CHROMIUM_PATH). Both expose
    // the same chromium launcher API.
    for (const name of ['playwright', 'playwright-core']) {
      try {
        const loaded = await import(pathToFileURL(require.resolve(name)).href);
        // CJS entrypoints surface through `default`; ESM through the namespace.
        return loaded.chromium ? loaded : loaded.default ?? loaded;
      } catch (error) {
        if (!['ERR_MODULE_NOT_FOUND','MODULE_NOT_FOUND'].includes(error.code)) throw error;
      }
    }
  }
  const module = execFileSync(process.env.PYTHON || 'python', ['-c',
    "import pathlib,playwright; print(pathlib.Path(playwright.__file__).parent/'driver'/'package'/'index.mjs')"], {encoding:'utf8',windowsHide:true}).trim();
  return import(pathToFileURL(module).href);
}
