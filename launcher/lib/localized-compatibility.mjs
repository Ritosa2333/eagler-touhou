import { readFile } from 'node:fs/promises';
import { parse as parseJavaScript } from 'acorn';
import { parse as parseHtml } from 'parse5';

const entries = JSON.parse(await readFile(new URL('../messages/compatibility.json', import.meta.url), 'utf8'));
const keys = new Map(entries.map(({ source, key }) => [source, key]));

/** Render old-browser help at build time; no modern translation runtime is delivered to it. */
export function localizeCompatibilityHtml(source, locale, translate) {
  const replacements = [];
  function translateText(value) {
    const text = value.trim(),
      key = keys.get(text) ?? keys.get(value);
    if (!key) return value;
    const result = translate(locale, key);
    return value === text
      ? result
      : value.slice(0, value.indexOf(text)) + result + value.slice(value.indexOf(text) + text.length);
  }
  function visit(node, ignored = false) {
    if (node.nodeName === 'script') {
      for (const child of node.childNodes ?? []) {
        if (child.nodeName !== '#text') continue;
        const ast = parseJavaScript(child.value, { ecmaVersion: 5 });
        function walk(value) {
          if (!value || typeof value !== 'object') return;
          if (value.type === 'Literal' && typeof value.value === 'string' && keys.has(value.value)) {
            replacements.push({
              start: child.sourceCodeLocation.startOffset + value.start,
              end: child.sourceCodeLocation.startOffset + value.end,
              text: JSON.stringify(translate(locale, keys.get(value.value))).replaceAll('<', '\\u003c'),
            });
          }
          for (const childNode of Object.values(value)) {
            if (Array.isArray(childNode)) childNode.forEach(walk);
            else if (childNode && typeof childNode === 'object') walk(childNode);
          }
        }
        walk(ast);
      }
      return;
    }
    if (node.nodeName === '#text' && !ignored) {
      const text = translateText(node.value);
      if (text !== node.value && node.sourceCodeLocation)
        replacements.push({
          start: node.sourceCodeLocation.startOffset,
          end: node.sourceCodeLocation.endOffset,
          text: text.replaceAll('&', '&amp;').replaceAll('<', '&lt;').replaceAll('>', '&gt;'),
        });
    }
    for (const child of node.childNodes ?? []) visit(child, ignored || node.nodeName === 'style');
  }
  visit(parseHtml(source, { sourceCodeLocationInfo: true }));
  let result = source;
  for (const { start, end, text } of replacements.sort((a, b) => b.start - a.start))
    result = result.slice(0, start) + text + result.slice(end);
  const languageLabel = translate(locale, 'ui.language')
    .replaceAll('&', '&amp;')
    .replaceAll('"', '&quot;')
    .replaceAll('<', '&lt;')
    .replaceAll('>', '&gt;');
  return result
    .replace('<html lang="zh-CN">', `<html lang="${locale}">`)
    .replace(/(<nav\s+)aria-label="[^"]*"/, (_match, prefix) => `${prefix}aria-label="${languageLabel}"`);
}
