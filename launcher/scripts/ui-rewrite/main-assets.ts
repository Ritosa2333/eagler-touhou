import {readFileSync, cpSync, existsSync, mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {execFileSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
import type {Plugin} from 'vite';
import {launcherStyleSources, buildLauncherStyleOutputs, buildRuntimeProgramWorkerSource, buildDocumentStyles, buildHelpPageOutputs} from '../../lib/frontend-assets.mjs';
import {writeFileAtomic} from '../../lib/atomic-file.mjs';
import {STATIC_FRONTEND_PACKAGE_FILES} from '../../lib/frontend-static-manifest.mjs';
import {resolvePublicOrRepositorySource} from '../../lib/public-source.mjs';
import {authoredSourcesPlugin} from '../../lib/authored-source-resolution.mjs';
import {build} from 'esbuild';
import {reactDeploymentConfig} from './deployment-config.ts';
const root = fileURLToPath(new URL('../../', import.meta.url));
// Original lobby selectors stopped at the launcher iframe's document. Settings,
// Player and their detached overlays now retain that ownership explicitly. The
// exclusion adds no specificity to controls that still belong to the lobby.
const outsideLauncherDocument = ':not(:where([data-launcher-document],[data-launcher-document] *))';
export function mainAssets(): Plugin {
  const deployment = reactDeploymentConfig();
  let generatedFiles: string[] = [];
  let wroteAssets = false;
  const output = resolve(root, deployment.outputDirectory, 'client');
  return {
    configureServer(server) {
      server.middlewares.use(async (req, res, next) => {
        const pathname = req.url?.split('?')[0];
        const asset = pathname?.startsWith(deployment.mountPath) ? pathname.slice(deployment.mountPath.length) : '';
        // publicDir is disabled: serve the same explicit static asset inventory
        // in development that closeBundle copies into the production artifact.
        if (['about.html', 'faq.html', 'migrate.html', 'compatibility.html', 'compatibility.en.html', 'compatibility.ja.html'].includes(asset)) {
          res.setHeader('Content-Type','text/html; charset=utf-8');res.setHeader('Cache-Control','no-cache');res.end(readFileSync(resolve(root,'public',asset)));return;
        }
        if (asset === 'about.css') {
          try {const [file] = await buildDocumentStyles({project: root});res.setHeader('Content-Type','text/css');res.setHeader('Cache-Control','no-cache');res.end(file.contents);}
          catch (error) {next(error);}
          return;
        }
        if (asset === 'site-help.mjs') {
          try {const [file] = await buildHelpPageOutputs({project: root});res.setHeader('Content-Type','application/javascript');res.setHeader('Cache-Control','no-cache');res.end(file.contents);}
          catch (error) {next(error);}
          return;
        }
        if ((asset.startsWith('assets/') || asset.startsWith('content/')) && STATIC_FRONTEND_PACKAGE_FILES.includes(asset)) {
          try {
            const source = resolvePublicOrRepositorySource(asset);
            res.setHeader('Content-Type', asset.endsWith('.html') ? 'text/html; charset=utf-8' : asset.endsWith('.mjs') ? 'application/javascript' : asset.endsWith('.svg') ? 'image/svg+xml' : asset.endsWith('.png') ? 'image/png' : asset.endsWith('.webp') ? 'image/webp' : asset.endsWith('.ico') ? 'image/x-icon' : 'application/octet-stream');
            res.setHeader('Cache-Control', 'no-cache');
            res.end(readFileSync(source));
          } catch (error) {next(error);}
          return;
        }
        if (req.url?.split('?')[0] !== deployment.mountPath + 'runtime-program-sw.js') {next();return;}
        try {res.setHeader('Content-Type', 'application/javascript');res.setHeader('Cache-Control', 'no-cache');
          res.end(await buildRuntimeProgramWorkerSource({project:root}));}
        catch (error) {next(error);}
      });
    },
    name: 'main-visual-assets', enforce: 'pre',
    load(id) {
      const sourceId = id.split('?')[0].replaceAll('\\', '/');
      if (sourceId === resolve(root, 'public/styles.css').replaceAll('\\', '/')) {
        const source = readFileSync(resolve(root, 'public/styles.css'), 'utf8');
        const fonts = readFileSync(resolve(root, 'public/ui-fonts.css'), 'utf8');
        const styles = launcherStyleSources(source, fonts);
        // Match main's styles.css then features.css cascade and font subsets.
        return styles.shell + '\n' + styles.features;
      }
      if (sourceId !== resolve(root, 'public/lobby.css').replaceAll('\\', '/')) return;
      return readFileSync(resolve(root, 'public/lobby.css'), 'utf8')
        .replace('.lobby-page [hidden]', `.lobby-page [hidden]${outsideLauncherDocument}`)
        .replace('.lobby-page :is(.lobby-button,input,select)', `.lobby-page :is(.lobby-button,input,select)${outsideLauncherDocument}`)
        .replace('.lobby-page button,.lobby-page a{', `.lobby-page button${outsideLauncherDocument},.lobby-page a${outsideLauncherDocument}{`)
        .replace('.lobby-page :is(button,a,input,select):focus-visible', `.lobby-page :is(button,a,input,select)${outsideLauncherDocument}:focus-visible`)
        .replace('.lobby-page.less-motion *{', `.lobby-page.less-motion *${outsideLauncherDocument}{`)
        .replace('.lobby-page *{', `.lobby-page *${outsideLauncherDocument}{`);
    },
    buildStart() {
      if (this.environment.config.consumer !== 'client') return;
      // Use main's compiler so Markdown-only edits cannot publish stale copy.
      execFileSync(process.execPath, [resolve(root, 'scripts/build-content-pages.mjs'), '--quiet'], {cwd: root, stdio: 'pipe'});
    },
    writeBundle(_options, bundle) {
      if (this.environment.config.consumer !== 'client') return;
      if (!Object.values(bundle).some(output => output.type === 'chunk' && output.isEntry)) return;
      generatedFiles = Object.keys(bundle).filter(path => !path.endsWith('.map') && !path.startsWith('.'));
    },
    async closeBundle() {
      // React Router also instantiates Vite for configuration and SSR cleanup.
      // Only the instance that actually wrote the client graph owns this step;
      // a later config-loader close must never overwrite the finalized graph.
      if (this.environment.config.consumer !== 'client' || !generatedFiles.length || wroteAssets) return;
      wroteAssets = true;
      mkdirSync(output, {recursive: true});
      // Consume the same public-file contract as main. Route documents and
      // generated CSS belong to their respective builders, not raw copying.
      const generated = new Set(['index.html', 'en.html', 'ja.html', 'lobby.html', 'profile.html', 'login.html', 'ui-review.html', 'styles.css', 'about.css', 'features.css', 'touch-guide.css', 'runtime-program-sw.js', 'site-help.mjs']);
      for (const path of STATIC_FRONTEND_PACKAGE_FILES) {
        if (generated.has(path) || path.startsWith('legacy/')) continue;
        const source = resolvePublicOrRepositorySource(path);
        if (!existsSync(source)) throw new Error(`Missing canonical public asset: ${path}`);
        mkdirSync(resolve(output, path, '..'), {recursive: true});
        cpSync(source, resolve(output, path));
      }
      // Stable read-compatibility URLs remain available without pulling the
      // retired DOM/history owners or Node build facades into the browser.
      const compatibility = await build({
        entryPoints: Object.fromEntries(STATIC_FRONTEND_PACKAGE_FILES.filter(path => path.startsWith('legacy/')).map(path => [path.replace(/\.mjs$/, ''), resolvePublicOrRepositorySource(path)])),
        outbase: root, outdir: output, bundle: true, splitting: true, format: 'esm',
        platform: 'browser', target: 'es2022', minify: true, write: false,
        outExtension: {'.js': '.mjs'}, chunkNames: 'legacy/chunks/[name]-[hash]',
        plugins: [authoredSourcesPlugin(root)], logLevel: 'warning',
      });
      for (const file of compatibility.outputFiles) await writeFileAtomic(file.path, file.contents);
      // Standalone original pages keep their original styles.css URL. They do
      // not load the Framework document's hashed stylesheet or application JS.
      for (const file of await buildLauncherStyleOutputs({project: root, outputDirectory: output})) {
        await writeFileAtomic(file.path, file.contents);
      }
      for (const file of await buildHelpPageOutputs({project: root, outputDirectory: output})) await writeFileAtomic(file.path, file.contents);
    },
  };
}
