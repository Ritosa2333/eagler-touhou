// Reuse production source resolution and Vite-compatible presentation assets.
import {readFileSync} from 'node:fs';
import {resolve,basename} from 'node:path';
import {authoredSourcesPlugin as authoredSources} from '../../lib/authored-source-resolution.mjs';
export function authoredSourcesPlugin(project) {
  const source = authoredSources(project);
  return {...source, setup(context) {
    context.onResolve({filter: /\?worker&url$/}, args => ({path: args.path, namespace: 'fixture-worker-url'}));
    context.onLoad({filter: /.*/, namespace: 'fixture-worker-url'}, () => ({contents: 'export default "https://fixture.invalid/worker.js"', loader: 'js'}));
    context.onResolve({filter: /\.md\?raw$/}, args => ({path: resolve(args.resolveDir, args.path.slice(0, -4)), namespace: 'fixture-markdown'}));
    context.onLoad({filter: /.*/, namespace: 'fixture-markdown'}, args => ({contents: readFileSync(args.path), loader: 'text'}));
    source.setup(context);
    context.onResolve({filter: /^\.\.?(?:\/|\\).*\.(?:wasm|js)\?url$/}, args => ({path: resolve(args.resolveDir, args.path.slice(0, -4)), namespace: 'fixture-code-url'}));
    context.onLoad({filter: /.*/, namespace: 'fixture-code-url'}, args => {
      readFileSync(args.path);
      return {contents: `export default ${JSON.stringify('https://fixture.invalid/'+basename(args.path))}`, loader: 'js'};
    });
    context.onResolve({filter: /\.(?:webp|png|svg)\?url$/}, args => ({path: resolve(args.resolveDir, args.path.slice(0, -4)), namespace: 'fixture-asset-url'}));
    context.onLoad({filter: /.*/, namespace: 'fixture-asset-url'}, args => ({contents: readFileSync(args.path), loader: 'dataurl'}));
  }};
}
