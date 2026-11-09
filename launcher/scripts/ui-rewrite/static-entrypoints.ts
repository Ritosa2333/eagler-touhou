import {mkdir, readFile, readdir, rename, rmdir} from 'node:fs/promises';
import {dirname, resolve} from 'node:path';
import {REACT_DOCUMENTS, UI_REVIEW_DOCUMENT} from '../../lib/frontend-static-manifest.mjs';

/** RR prerenders the flat launcher document addresses as directories. Preserve
 * those original .html addresses without broad server rewrites. */
export async function finalizeOriginalEntrypoints(clientDirectory: string, mountPath = '/', includeUiReview = false) {
  const documents = includeUiReview ? [...REACT_DOCUMENTS, UI_REVIEW_DOCUMENT] : REACT_DOCUMENTS;
  for (const {path, file} of documents) {
    if (path === '/') continue;
    // Supported basename prerenders include the mount in their output path.
    // Flatten the file in the mount-local publication without changing HTML.
    const directory = resolve(clientDirectory, '.' + mountPath, path.slice(1)), source = resolve(directory, 'index.html');
    const target = resolve(clientDirectory, file);
    const html = await readFile(source, 'utf8');
    if (!html.includes('window.__reactRouterContext =') || !html.includes('window.__reactRouterRouteModules =')) throw new Error(`Missing hydration scripts for ${file}`);
    if (source === target) continue;
    const entries = await readdir(directory);
    const childPaths = documents.filter(document => document.path.startsWith(path + '/'));
    if (entries.some(entry => entry !== 'index.html' && !childPaths.some(document => document.path === `${path}/${entry}`))) {
      throw new Error(`Unexpected prerender contents for ${file}`);
    }
    const temporary = resolve(clientDirectory, `${file.replaceAll('/', '_')}.prerender-tmp`);
    await rename(source, temporary);
    if (entries.length === 1) await rmdir(directory);
    await mkdir(dirname(target), {recursive: true});
    await rename(temporary, target);
  }
}
