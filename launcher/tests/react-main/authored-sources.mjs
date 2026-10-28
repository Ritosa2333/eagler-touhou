// Reuse production source resolution and Vite-compatible presentation assets.
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {authoredSourcesPlugin as authoredSources} from '../../lib/authored-source-resolution.mjs';
export function authoredSourcesPlugin(project) {
  const source = authoredSources(project);
  return {...source, setup(context) {
    source.setup(context);
    context.onResolve({filter: /\.(?:webp|png|svg)\?url$/}, args => ({path: resolve(args.resolveDir, args.path.slice(0, -4)), namespace: 'fixture-asset-url'}));
    context.onLoad({filter: /.*/, namespace: 'fixture-asset-url'}, args => ({contents: readFileSync(args.path), loader: 'dataurl'}));
  }};
}
