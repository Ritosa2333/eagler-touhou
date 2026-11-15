import {existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

const defaultProject = fileURLToPath(new URL('..', import.meta.url));
const pending = new Map();
export const i18nCompilerOptions = Object.freeze({
  strategy: ['baseLocale'], outputStructure: 'locale-modules', emitTsDeclarations: true,
  disableAsyncLocalStorage: true, emitReadme: false, emitGitIgnore: false, emitPrettierIgnore: false,
});

// Compiler code is a source-checkout tool. Prebuilt Host packages load this
// small facade without resolving maintainer-only dependencies.
const sourceCompiler = () => import(new URL('../scripts/build-i18n-messages.mjs', import.meta.url).href);
export async function i18nBuildInputs(project = defaultProject) {
  return (await sourceCompiler()).i18nBuildInputs(project);
}
export function ensureI18nBuild(options = {}) {
  const project = resolve(options.project ?? defaultProject);
  if (!existsSync(resolve(project, 'project.inlang/settings.json'))) return Promise.resolve({built: false, mode: 'prebuilt'});
  if (pending.has(project)) return pending.get(project);
  const work = sourceCompiler().then(compiler => compiler.ensureI18nBuild({...options, project}))
    .finally(() => {if (pending.get(project) === work) pending.delete(project);});
  pending.set(project, work);
  return work;
}
