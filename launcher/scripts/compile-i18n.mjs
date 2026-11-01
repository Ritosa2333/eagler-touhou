import { fileURLToPath } from 'node:url';
import { compile } from '@inlang/paraglide-js';

await compile({
  project: fileURLToPath(new URL('../project.inlang', import.meta.url)),
  outdir: fileURLToPath(new URL('../src/paraglide', import.meta.url)),
  strategy: ['baseLocale'],
  outputStructure: 'message-modules',
  emitTsDeclarations: true,
  disableAsyncLocalStorage: true,
});
