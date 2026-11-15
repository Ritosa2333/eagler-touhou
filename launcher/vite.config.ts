import {defineConfig, loadEnv, type Plugin, type ViteDevServer, type PreviewServer} from 'vite';
import {reactRouter} from '@react-router/dev/vite';
import tailwindcss from '@tailwindcss/vite';
import {originalBootstrap} from './scripts/ui-rewrite/original-bootstrap.ts';
import {mainAssets} from './scripts/ui-rewrite/main-assets.ts';
import {authoredSources} from './scripts/ui-rewrite/authored-sources.ts';
import {uiTranslations} from './scripts/ui-rewrite/i18n.ts';
import {documentHistory} from './scripts/ui-rewrite/document-history.ts';
import {reactDeploymentConfig} from './scripts/ui-rewrite/deployment-config.ts';
import {relative, resolve} from 'node:path';
import {readFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
const deployment = reactDeploymentConfig();
const sourceRoot = fileURLToPath(new URL('.', import.meta.url));
// Local UI acceptance can use an existing Host without replacing Vite's UI.
const uiEnvironment = loadEnv('development', sourceRoot, 'EAGLER_UI_');
const uiServer = process.env.EAGLER_UI_SERVER_ORIGIN || uiEnvironment.EAGLER_UI_SERVER_ORIGIN;
const accountServer = process.env.EAGLER_UI_API_ORIGIN || uiEnvironment.EAGLER_UI_API_ORIGIN;
const uiSiteManifest = process.env.EAGLER_UI_SITE_MANIFEST || uiEnvironment.EAGLER_UI_SITE_MANIFEST;
const serverProxy = uiServer ? Object.fromEntries([
  '^/(?:site-manifest|host-manifest|runtime-manifest|release-catalog)\\.json(?:\\?|$)',
  '/runtime/', '/netplay-stats/', '/thcrap/', '^/th[0-9]+\\.(?:package\\.json|data)(?:\\?|$)',
].map(path => [path, {target: uiServer, changeOrigin: true}])) : undefined;

/** Local inspection tools stay outside the published application graph. */
function layoutWatch(): Plugin {
  const files: Record<string,[string,string]> = {
    '':['tests/visual/layout-watch.html','text/html'],
    'layout-watch.css':['tests/visual/layout-watch.css','text/css'],
    'layout-watch.mjs':['tests/visual/layout-watch.mjs','text/javascript'],
    'layout-watch-core.mjs':['tests/visual/layout-watch-core.mjs','text/javascript'],
    'probe.mjs':['tests/browser/ui-layout-probe.mjs','text/javascript'],
  };
  const install=(server: ViteDevServer|PreviewServer)=>{
    server.middlewares.use((request,response,next)=>{
      const path=request.url?.split('?')[0]??'',prefix=deployment.mountPath+'__ui-audit/';
      if(!path.startsWith(prefix)){next();return;}
      const file=files[path.slice(prefix.length)];
      if(!file){response.statusCode=404;response.end('Not found');return;}
      try {
        const text=readFileSync(resolve(sourceRoot,file[0]),'utf8').replace('__APP_BASE__',deployment.mountPath);
        response.setHeader('Content-Type',file[1]+'; charset=utf-8');response.setHeader('Cache-Control','no-store');response.end(text);
      } catch(error){next(error);}
    });
  };
  return {name:'local-layout-watch',configureServer:install,configurePreviewServer:install};
}
export default defineConfig({base: deployment.mountPath,
  ssr:{noExternal:['fumadocs-core','@fumadocs/base-ui']},
  optimizeDeps:{exclude:['fumadocs-core','@fumadocs/base-ui']},
  // Generated fixtures are not authored UI inputs. Watching them also holds
  // directory handles that block publication/refresh renames on Windows.
  server: {proxy: {...serverProxy, ...(accountServer ? {'/api/': {target: accountServer, changeOrigin: true}} : {})}, watch: {ignored: path => /^(?:\.cache|artifacts|src\/paraglide)(?:\/|$)/.test(relative(sourceRoot, path).replaceAll('\\', '/'))}},
  // The build's preview fetch and listener share one explicit loopback family.
  preview: {host: '127.0.0.1'},
  // Keep generated JavaScript within the browser floor admitted by bootstrap.
  // Lightning CSS merges independent transforms and drops optical offsets.
  // https://github.com/parcel-bundler/lightningcss/issues/426
  build: {manifest: true, target: 'chrome108', cssMinify: 'esbuild'},
  define: {__REACT_APP_SHELL_DEPLOYMENT__: JSON.stringify(deployment.appShell)},
  plugins: [uiTranslations(sourceRoot), documentHistory(sourceRoot), layoutWatch(), tailwindcss({optimize: false}), {name: 'local-ui-site-manifest', configureServer(server) {
    if (!uiSiteManifest) return;
    server.middlewares.use((req, res, next) => {
      if (req.url?.split('?')[0] !== deployment.mountPath + 'site-manifest.json') {next();return;}
      try {res.setHeader('Content-Type', 'application/json');res.setHeader('Cache-Control', 'no-store');res.end(readFileSync(resolve(sourceRoot, uiSiteManifest)));}
      catch (error) {next(error);}
    });
  }}, originalBootstrap(), mainAssets(), authoredSources(), reactRouter()], publicDir: false});
