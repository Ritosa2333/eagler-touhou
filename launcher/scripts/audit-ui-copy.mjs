import { readdirSync, readFileSync } from 'node:fs';
import { relative, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { parse } from '@babel/parser';

const project = fileURLToPath(new URL('..', import.meta.url));
const allowed = new Map([
  ['EAGLER☯TOUHOU', 'Brand'],
  ['EAGLER', 'Brand'],
  ['TOUHOU', 'Brand'],
  ['GitHub', 'Service name'],
  ['ms', 'SI unit'],
  ['f', 'Frame unit'],
  ['MP', 'Product suffix'],
  ['→ P', 'Player-seat prefix'],
  ['vs', 'Match notation'],
  ['EAGLER TOUHOU', 'Brand'],
  ['東方', 'Native game artwork title'],
  ['赣ICP备2025074288号-1', 'Statutory registration identifier'],
  ['联机大厅東方紅魔郷妖々夢永夜抄花映塚風神録', 'Font coverage probe'],
  ['本局已经开始；已保留旁观席，将在下一局生效', 'Server protocol classifier; display uses a translated message'],
  ['R', 'Keyboard label'],
  ['Z', 'Keyboard label'],
  ['X', 'Keyboard label'],
  ['C', 'Keyboard label'],
  ['C / V', 'Keyboard labels'],
  ['Shift', 'Keyboard label'],
  ['Esc', 'Keyboard label'],
  ['ESC', 'Keyboard label'],
  ['Ctrl', 'Keyboard label'],
  ['Backspace', 'Keyboard label'],
  ['Tab', 'Keyboard label'],
  ...Array.from({ length: 12 }, (_, i) => [`F${i + 1}`, 'Keyboard label']),
  ['WASD', 'Keyboard preset'],
  ['B', 'Bomb key'],
  ['BOMB', 'Native game control label'],
  ['thprac', 'Project name'],
  ['midi', 'Audio format'],
  ['MIDI', 'Audio format'],
  ['P', 'Player seat prefix'],
]);
const errors = JSON.parse(readFileSync(resolve(project, 'messages/errors.json'), 'utf8'));
const errorSources = new Set(errors.filter((entry) => !entry.source.includes('{')).map((entry) => entry.source));
const containsCopy = (text) => /[\p{L}]/u.test(text) && !allowed.has(text) && !/^P\d+$/.test(text);

export function auditUiCopy(root = resolve(project, 'app')) {
  const findings = [];
  function walk(path) {
    for (const entry of readdirSync(path, { withFileTypes: true })) {
      const file = resolve(path, entry.name);
      if (entry.isDirectory()) {
        walk(file);
        continue;
      }
      if (!/\.(?:ts|tsx)$/.test(file) || /(?:utility-icons|masthead-icons)\.ts$/.test(file)) continue;
      const source = parse(readFileSync(file, 'utf8'), { sourceType: 'module', plugins: ['typescript', 'jsx'] });
      const report = (node, text, kind) =>
        findings.push({ file: relative(project, file), line: node.loc.start.line, kind, text });
      function visit(node) {
        if (node.type === 'JSXText') {
          const value = node.value.replace(/\s+/g, ' ').trim();
          if (containsCopy(value)) report(node, value, 'jsx-text');
        } else if (
          node.type === 'JSXAttribute' &&
          ['aria-label', 'title', 'placeholder', 'alt', 'label'].includes(node.name.name) &&
          node.value?.type === 'StringLiteral'
        ) {
          const value = node.value.value.trim();
          if (containsCopy(value)) report(node, value, 'jsx-attribute');
        } else if (
          node.type === 'StringLiteral' &&
          /\p{Script=Han}/u.test(node.value) &&
          !allowed.has(node.value) &&
          !errorSources.has(node.value)
        ) {
          // Regex protocol classifiers and comments are not StringLiteral nodes.
          report(node, node.value, 'han-literal');
        }
        for (const value of Object.values(node)) {
          if (Array.isArray(value)) {
            for (const child of value) if (child?.type) visit(child);
          } else if (value && typeof value === 'object' && typeof value.type === 'string') visit(value);
        }
      }
      visit(source);
    }
  }
  walk(root);
  return findings;
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  const findings = auditUiCopy();
  console.log(JSON.stringify({ status: findings.length ? 'FAIL' : 'PASS', findings }, null, 2));
  if (findings.length) process.exitCode = 1;
}
