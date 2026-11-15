/** Build declarations and synthetic document metadata only; no SW operations. */
import {test} from 'vitest';
import assert from 'node:assert/strict';
import {readReactAppShellDeployment, validateReactAppShellDeployment, normalizeReactMountPath} from '../../app/services/app-shell-deployment.ts';
import {reactDeploymentConfig} from '../../scripts/ui-rewrite/deployment-config.ts';
import {reactAppShellMatchesSite} from '../../lib/react-app-shell-deployment.mjs';
import {assertUiRewriteRuntime} from '../../scripts/ui-rewrite/check-runtime.mjs';
const config = {schema: 'eagler-touhou/react-app-shell/1', origin: 'https://isolated.invalid', mountPath: '/review/'};
const document = (value, count = 1) => ({querySelectorAll: () => Array.from({length: count}, () => ({getAttribute: () => JSON.stringify(value)}))});
test('optional test/fixture runtime bound does not claim all Node22 releases', () => {
  for (const version of ['22.18.0', '23.6.0', '24.0.0', '24.19.0']) assert.doesNotThrow(() => assertUiRewriteRuntime(version));
  for (const version of ['20.19.0', '22.0.0', '22.17.0', '23.5.0']) assert.throws(() => assertUiRewriteRuntime(version), /22\.18\+.*23\.6\+.*24\+.*Node\.js 24 is the verified/);
});
test('default and mount-only builds enable offline loading on their own origin', () => {
  for (const mountPath of ['/', '/review/']) {
    const {appShell} = reactDeploymentConfig({EAGLER_REACT_MOUNT_PATH: mountPath});
    assert.deepEqual(appShell, {...config, origin: 'same-origin', mountPath});
    for (const origin of ['https://touhou.vip', 'https://beta.touhou.vip', 'http://localhost:8130']) {
      assert.deepEqual(readReactAppShellDeployment(document(appShell), {href: origin + mountPath + 'index.html'}),
        {workerUrl: origin + mountPath + 'app-shell-sw.js', scope: origin + mountPath,
          previousWorkerUrls: [origin + mountPath + 'app-shell-sw-fontfix-1fefcaa790f0.js']});
    }
    for (const origin of ['http://remote.invalid', 'file://']) assert.equal(readReactAppShellDeployment(document(appShell), {href: origin + mountPath}), undefined);
    assert.equal(readReactAppShellDeployment(document(appShell), {href: 'https://site.invalid' + mountPath + 'nested/'}), undefined);
    assert.equal(reactAppShellMatchesSite(appShell, null), true);
    assert.equal(reactAppShellMatchesSite(appShell, new URL('https://site.invalid' + mountPath)), true);
    assert.equal(reactAppShellMatchesSite(appShell, new URL('https://site.invalid/other/')), false);
  }
  assert.equal(reactDeploymentConfig({}).appShell.origin, 'same-origin');
  assert.equal(reactDeploymentConfig({EAGLER_REACT_APP_SHELL: 'disabled'}).appShell, null);
  assert.throws(() => reactDeploymentConfig({EAGLER_REACT_APP_SHELL: 'disabled', EAGLER_REACT_APP_SHELL_ORIGIN: config.origin}), /Disabled/);
  assert.throws(() => reactDeploymentConfig({EAGLER_REACT_APP_SHELL: 'typo'}), /must be/);
  assert.equal(readReactAppShellDeployment(document(null, 0), {href: 'https://isolated.invalid/review/'}), undefined);
});
test('explicit immutable origin and mount produce only the selected worker/scope', () => {
  for (const page of ['', 'index.html', 'en.html', 'lobby.html']) assert.deepEqual(readReactAppShellDeployment(document(config), {href: config.origin + config.mountPath + page}), {workerUrl: 'https://isolated.invalid/review/app-shell-sw.js', scope: 'https://isolated.invalid/review/', previousWorkerUrls: ['https://isolated.invalid/review/app-shell-sw-fontfix-1fefcaa790f0.js']});
});
test('copied artifacts, malformed or duplicate metadata cannot register elsewhere', () => {
  for (const href of ['https://isolated.invalid/', 'https://production.invalid/review/', 'https://isolated.invalid/review-child/', 'https://isolated.invalid/review/nested/']) assert.equal(readReactAppShellDeployment(document(config), {href}), undefined);
  assert.equal(readReactAppShellDeployment(document(config, 2), {href: config.origin + config.mountPath}), undefined);
  assert.equal(readReactAppShellDeployment(document({}), {href: config.origin + config.mountPath}), undefined);
});
test('remote root and nonsecure hosts are rejected; exact loopback fixtures allowed', () => {
  assert.throws(() => validateReactAppShellDeployment({...config, mountPath: '/'}), /non-root/);
  assert.throws(() => validateReactAppShellDeployment({...config, origin: 'http://isolated.invalid'}), /secure/);
  assert.throws(() => validateReactAppShellDeployment({...config, origin: config.origin + '/'}), /exact/);
  const local = {...config, origin: 'http://127.0.0.1:4321', mountPath: '/'};
  assert.deepEqual(validateReactAppShellDeployment(local), local);
  assert.equal(readReactAppShellDeployment(document(local), {href: 'http://127.0.0.1:4322/'}), undefined);
});
test('dedicated secure origin can explicitly cache its root without registering on another origin', () => {
  const isolated = {...config, mountPath: '/', isolatedOrigin: true};
  assert.deepEqual(validateReactAppShellDeployment(isolated), isolated);
  const build = reactDeploymentConfig({EAGLER_REACT_APP_SHELL: 'isolated', EAGLER_REACT_APP_SHELL_ORIGIN: config.origin, EAGLER_REACT_APP_SHELL_ISOLATED_ORIGIN: 'true'});
  assert.deepEqual(build.appShell, isolated);
  assert.equal(readReactAppShellDeployment(document(isolated), {href: 'https://production.invalid/'}), undefined);
  assert.deepEqual(readReactAppShellDeployment(document(isolated), {href: config.origin + '/'}), {workerUrl: config.origin + '/app-shell-sw.js', scope: config.origin + '/', previousWorkerUrls: [config.origin + '/app-shell-sw-fontfix-1fefcaa790f0.js']});
});
test('scope traversal, queries, encoded paths and missing explicit opt-in fail closed', () => {
  for (const path of ['//', '/review', '/review/../', '/%2f/', '/review/?x', '/review/#x', '/review.with-dot/']) assert.throws(() => normalizeReactMountPath(path));
  assert.deepEqual(reactDeploymentConfig({EAGLER_REACT_APP_SHELL_ORIGIN: config.origin, EAGLER_REACT_MOUNT_PATH: config.mountPath}).appShell, config);
  assert.throws(() => reactDeploymentConfig({EAGLER_REACT_APP_SHELL: 'isolated'}), /requires/);
  assert.deepEqual(reactDeploymentConfig({EAGLER_REACT_APP_SHELL: 'isolated', EAGLER_REACT_APP_SHELL_ORIGIN: config.origin, EAGLER_REACT_MOUNT_PATH: config.mountPath}).appShell, config);
});
