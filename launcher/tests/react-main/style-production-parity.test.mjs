/** Public CSS partition/output ownership; authored style edits are permitted. */
import {test} from 'vitest';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {resolve, basename} from 'node:path';
import {fileURLToPath} from 'node:url';
import postcss from 'postcss';
import {launcherStyleSources, buildLauncherStyleOutputs} from '../../lib/frontend-assets.mjs';
const root = fileURLToPath(new URL('../..', import.meta.url));
function rules(css) {
  const result = [];
  postcss.parse(css.replaceAll("\r\n", "\n")).walkRules(rule => {
    const context = [];
    for (let parent = rule.parent; parent.type !== 'root'; parent = parent.parent) context.unshift([parent.name, parent.params]);
    result.push(JSON.stringify([context, rule.selector, rule.nodes.filter(node => node.type === 'decl').map(node => [node.prop, node.value, !!node.important])]));
  });
  return result.sort();
}
test('CSS partition preserves every authored rule and keeps critical fonts in the shell', async () => {
  const source = await readFile(resolve(root, 'public/styles.css'), 'utf8');
  const fonts = await readFile(resolve(root, 'public/ui-fonts.css'), 'utf8');
  const styles = launcherStyleSources(source, fonts);
  assert.deepEqual(rules(styles.shell + '\n' + styles.features), rules(source), 'no authored rule may be lost or duplicated while partitioning');
  assert.match(styles.shell, /chill-round-gothic-site-medium-critical\.woff2/);
  assert.doesNotMatch(styles.shell, /chill-round-gothic-site-(?:medium|bold)\.woff2/);
  assert.doesNotMatch(styles.shell, /chill-round-gothic-site-.*-deferred\.woff2/);
});
test('stale partition markers fail instead of silently changing CSS cascade', () => {
  assert.throws(() => launcherStyleSources('body{}', ''), /split markers are stale/);
});
test('standalone pages receive parseable CSS at the three public URLs without browser JavaScript', async () => {
  const outputs = await buildLauncherStyleOutputs({project: root, outputDirectory: resolve(root, '.cache/standalone-style-proof')});
  assert.deepEqual(outputs.map(file => basename(file.path)), ['styles.css', 'features.css', 'touch-guide.css']);
  for (const file of outputs) {
    assert.ok(postcss.parse(file.text).nodes.length, basename(file.path));
    assert.doesNotMatch(file.text, /@import|<script|__reactRouterContext/);
  }
  assert.match(outputs[0].text, /chill-round-gothic-site-medium-critical\.woff2/);
  assert.match(outputs[1].text, /\.touch-direct-surface/);
});
