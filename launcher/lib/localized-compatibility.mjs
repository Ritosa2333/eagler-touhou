import { readFile } from 'node:fs/promises';
import { parse as parseJavaScript } from 'acorn';
import { parse as parseHtml } from 'parse5';
import postcss from 'postcss';

const entries = JSON.parse(await readFile(new URL('../messages/compatibility.json', import.meta.url), 'utf8'));
const keys = new Map(entries.map(({ source, key }) => [source, key]));

/** Old browsers receive literal colors from the same tokens as the modern UI. */
export function compatibilityThemeHtml(source, tokens, controls) {
  const themes = {dark: {}, light: {}};
  postcss.parse(tokens).walkRules(rule => {
    for (const theme of Object.keys(themes)) {
      if (!rule.selectors.includes(`html[data-book-theme='${theme}']`)) continue;
      rule.walkDecls(declaration => {themes[theme][declaration.prop] = declaration.value;});
    }
  });
  const color = (theme, name) => {
    const value = themes[theme][`--kit-${name}`];
    if (!/^#[\da-f]{3,8}$/i.test(value ?? '')) throw new Error(`Missing literal compatibility color: ${theme}/${name}`);
    return value;
  };
  const css = Object.keys(themes).map(theme => {
    const prefix = theme === 'light' ? '.light ' : '', c = name => color(theme, name);
    return `${prefix}body{color:${c('foreground')};background:${c('background')}}
${prefix}a,${prefix}a:visited{color:${c('brand')}}
${prefix}.actions button{color:${c('primary-foreground')};background:${c('primary')}}
${prefix}.section,${prefix}.half.right{border-color:${c('border')}}
${prefix}.pass{color:${c('success')}}${prefix}.fail{color:${c('destructive')}}
${prefix}a:focus,${prefix}button:focus{outline:2px solid ${c('ring')};outline-offset:3px}`;
  }).join('\n');
  if (!/<style id="siteTheme">[\s\S]*?<\/style>/.test(source)) throw new Error('Missing compatibility theme owner');
  return source.replace(/<style id="siteTheme">[\s\S]*?<\/style>/, () => `<style id="siteTheme">\n${css}\n${controls.trim()}\n  </style>`)
    .replace(/<meta name="theme-color"[^>]*>/, () => `<meta name="theme-color" content="${color('dark', 'background')}" data-light="${color('light', 'background')}">`);
}

/** Render old-browser help at build time; no modern translation runtime is delivered to it. */
export function localizeCompatibilityHtml(source, locale, translate) {
  if (locale !== 'en' && locale !== 'ja') throw new Error('Unsupported compatibility locale: ' + locale);
  // parse5 normalizes script text. Keep its offsets and Acorn offsets in the same LF source.
  source = source.replace(/\r\n?/g, '\n');
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
              text: JSON.stringify(translate(locale, keys.get(value.value))).replaceAll('<', '\\u003c').replaceAll('\u2028', '\\u2028').replaceAll('\u2029', '\\u2029'),
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
    .replace("location.href='./?compat=continue'", `location.href='${locale}.html?compat=continue'`)
    .replace(/(<nav\b[^>]*\s)aria-label="[^"]*"/, (_match, prefix) => `${prefix}aria-label="${languageLabel}"`);
}

/** These ES5-page messages are literals. Reject parameters instead of adding a
 * second interpolator to the build tool; interactive messages use Paraglide. */
export async function loadCompatibilityTranslations() {
  const read = async file => JSON.parse(await readFile(new URL('../' + file, import.meta.url), 'utf8'));
  const manifest = await read('messages/keys.json');
  const catalogs = Object.fromEntries(await Promise.all(['en', 'ja'].map(async locale => [locale, await read(`messages/${locale}.json`)])));
  return (locale, key) => {
    const entry = manifest[key], value = catalogs[locale]?.[entry?.id];
    if (!entry || entry.params.length || typeof value !== 'string') throw new Error(`Compatibility page requires a literal message: ${locale}/${key}`);
    return value;
  };
}
