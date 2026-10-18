import {execFileSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';

/** Reuse either the JS install or Python Playwright's existing bundled driver.
 * This helper never downloads a browser or adds a project dependency. */
export async function loadPlaywright({preferPython = false} = {}) {
  try {if (!preferPython) return await import('playwright');}
  catch (error) {if (error.code !== 'ERR_MODULE_NOT_FOUND') throw error;}
  const module = execFileSync(process.env.PYTHON || 'python', ['-c',
    "import pathlib,playwright; print(pathlib.Path(playwright.__file__).parent/'driver'/'package'/'index.mjs')"], {encoding:'utf8'}).trim();
  return import(pathToFileURL(module).href);
}
