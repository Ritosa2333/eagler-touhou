import {relative, resolve} from 'node:path';
import type {Plugin} from 'vite';
import {ensureI18nBuild, i18nBuildInputs} from '../../lib/i18n-build.mjs';
import {buildAuthoredContent} from '../../lib/content-build.mjs';
import {helpDocuments} from '../../app/components/documents/catalog.ts';

/** Use the same compiler owner as CLI checks and service publication. */
export function uiTranslations(project: string): Plugin {
  let inputs = new Set<string>();
  const documents=new Set(helpDocuments.map(document=>resolve(project,document.source)));
  documents.add(resolve(project,'scripts/build-content-pages.mjs'));
  return {
    name: 'launcher-translations', enforce: 'pre',
    async buildStart() {
      await buildAuthoredContent(project);
      await ensureI18nBuild({project});
      inputs = new Set(await i18nBuildInputs(project));
      for (const path of inputs) this.addWatchFile(path);
      for (const path of documents) this.addWatchFile(path);
    },
    async handleHotUpdate(context) {
      const path = resolve(context.file);
      if (!inputs.has(path) && !documents.has(path) && !relative(project, path).replaceAll('\\', '/').startsWith('messages/')) return;
      if(documents.has(path))await buildAuthoredContent(project);
      await ensureI18nBuild({project});
      context.server.moduleGraph.invalidateAll();
      context.server.ws.send({type: 'full-reload'});
      return [];
    },
  };
}
