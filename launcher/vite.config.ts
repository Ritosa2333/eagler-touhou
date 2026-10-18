import {defineConfig} from 'vite';
import {reactRouter} from '@react-router/dev/vite';
import {originalBootstrap} from './scripts/ui-rewrite/original-bootstrap.ts';
import {mainAssets} from './scripts/ui-rewrite/main-assets.ts';
import {authoredSources} from './scripts/ui-rewrite/authored-sources.ts';
import {reactDeploymentConfig} from './scripts/ui-rewrite/deployment-config.ts';
import {relative} from 'node:path';
import {fileURLToPath} from 'node:url';
const deployment = reactDeploymentConfig();
const sourceRoot = fileURLToPath(new URL('.', import.meta.url));
export default defineConfig({base: deployment.mountPath,
  // Generated fixtures are not authored UI inputs. Watching them also holds
  // directory handles that block publication/refresh renames on Windows.
  server: {watch: {ignored: path => /^(?:\.cache|artifacts)(?:\/|$)/.test(relative(sourceRoot, path).replaceAll('\\', '/'))}},
  // The build's preview fetch and listener share one explicit loopback family.
  preview: {host: '127.0.0.1'},
  // Keep generated JavaScript within the browser floor admitted by bootstrap.
  build: {manifest: true, target: 'chrome108'},
  define: {__REACT_APP_SHELL_DEPLOYMENT__: JSON.stringify(deployment.appShell)},
  plugins: [originalBootstrap(), mainAssets(), authoredSources(), reactRouter()], publicDir: false});
