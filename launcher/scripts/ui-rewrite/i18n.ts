import {relative, resolve} from 'node:path';
import type {Plugin} from 'vite';
import {ensureI18nBuild, i18nBuildInputs} from '../../lib/i18n-build.mjs';

/** Use the same compiler owner as CLI checks and service publication. */
export function uiTranslations(project: string): Plugin {
  let inputs = new Set<string>();
  return {
    name: 'launcher-translations', enforce: 'pre',
    async buildStart() {
      await ensureI18nBuild({project});
      inputs = new Set(await i18nBuildInputs(project));
      for (const path of inputs) this.addWatchFile(path);
    },
    async handleHotUpdate(context) {
      const path = resolve(context.file);
      if (!inputs.has(path) && !relative(project, path).replaceAll('\\', '/').startsWith('messages/')) return;
      await ensureI18nBuild({project});
      context.server.moduleGraph.invalidateAll();
      context.server.ws.send({type: 'full-reload'});
      return [];
    },
  };
}
