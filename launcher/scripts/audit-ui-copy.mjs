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
  ['Discord', 'Service name'],
  ['https://', 'URL scheme example'],
  ['name@example.com', 'Reserved example email address'],
  ['th', 'Product number prefix'],
  ['ms', 'SI unit'],
  ['s', 'SI unit'],
  ['f', 'Frame unit'],
  ['MP', 'Product suffix'],
  ['| MP', 'Product suffix'],
  ['→ P', 'Player-seat prefix'],
  ['vs', 'Match notation'],
  ['EAGLER TOUHOU', 'Brand'],
  ['赣ICP备2025074288号-1', 'Statutory registration identifier'],
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
const contextualAllowed = new Map([
  ['app/components/launcher/CareerShowcase.tsx', new Map(['F', 'E / N / H / L', 'Easy / Normal / Hard / Lunatic', 'X / P', 'Extra / Phantasm', '1CC', 'NM / NB / NN', 'F / NF', 'U', 'e1 / e2'].map(text => [text, 'Original chart notation; the adjacent explanation is localized']))],
  ['app/components/player/TouchHelp.tsx', new Map(['CONTINUE?', 'YES', 'NO', 'INPUT'].map(text => [text, 'Native game menu labels reproduced by the touch demonstration']))],
  ['app/components/settings/TouchControlPreview.tsx', new Map([['Tracker', 'Native practice tool name']])],
  ['app/components/room/RoomWorkspace.tsx', new Map([['Normal', 'Canonical game difficulty name']])],
  ['app/components/launcher/OptionsPanel.tsx', new Map([['MULTIPLAYER', 'Decorative multiplayer artwork label']])],
  ['app/components/launcher/LibraryCards.tsx', new Map([['東方', 'Native artwork title prefix'], ...['MULTIPLAYER', 'Multiplayer'].map(text => [text, 'Decorative multiplayer artwork label'])])],
  ['app/services/startup.ts', new Map([['联机大厅東方紅魔郷妖々夢永夜抄花映塚風神録', 'Font coverage probe']])],
  ['app/services/multiplayer-room.ts', new Map([['本局已经开始；已保留旁观席，将在下一局生效', 'Server protocol classifier; translated at display boundary']])],
]);
const errors = JSON.parse(readFileSync(resolve(project, 'messages/errors.json'), 'utf8'));
const errorSources = new Set(errors.filter((entry) => !entry.source.includes('{')).map((entry) => entry.source));
const errorTemplates = new Set(errors.filter(entry => entry.source.includes('{')).map(entry => entry.source.replace(/\{\w+\}/g, '{}')));
// These sources compose the isolated, explicitly Chinese maintainer handbook.
// Production controls and all other routes stay in the copy audit.
const authoredReferenceSources = new Map([
  ['app/routes/ui-review.tsx', 'Chinese UI handbook; its root declares catalogLocale through lang and LocaleProvider'],
  ['app/ui/catalog.ts', 'Chinese handbook data; catalogLocale is declared with the data'],
  ['app/ui/page-catalog.ts', 'Chinese handbook page inventory rendered inside the declared catalogLocale'],
  ['app/ui/page-inventory.tsx', 'Chinese handbook inventory controls inside the declared catalogLocale'],
  ['app/ui/room-example.tsx', 'Chinese handbook demo data and controls; production room views stay audited'],
]);
const containsCopy = (text) => /[\p{L}]/u.test(text) && !allowed.has(text) && !/^P\d+$/.test(text);

export function auditUiCopy(root = resolve(project, 'app')) {
  const findings = [];
  const reported = new Set();
  function walk(path) {
    for (const entry of readdirSync(path, { withFileTypes: true })) {
      const file = resolve(path, entry.name);
      if (entry.isDirectory()) {
        walk(file);
        continue;
      }
      if (!/\.(?:ts|tsx)$/.test(file)) continue;
      const filename = relative(project, file).replaceAll('\\', '/');
      if (authoredReferenceSources.has(filename)) continue;
      const source = parse(readFileSync(file, 'utf8'), { sourceType: 'module', plugins: ['typescript', 'jsx'] });
      const report = (node, text, kind) => {
        if (contextualAllowed.get(filename)?.has(text)) return;
        const identity = filename + ':' + node.start + ':' + text;
        if (reported.has(identity)) return;
        reported.add(identity);
        findings.push({file: filename, line: node.loc.start.line, kind, text});
      };
      function displayed(node, kind) {
        if (!node) return;
        if (node.type === 'StringLiteral') {
          const value = node.value.trim();
          if (containsCopy(value)) report(node, value, kind);
        } else if (node.type === 'ConditionalExpression') {
          displayed(node.consequent, kind); displayed(node.alternate, kind);
        } else if (node.type === 'LogicalExpression') displayed(node.right, kind);
        else if (node.type === 'BinaryExpression' && node.operator === '+') {displayed(node.left, kind); displayed(node.right, kind);}
        else if (node.type === 'TemplateLiteral') for (const part of node.quasis) {
          const value = (part.value.cooked ?? part.value.raw).trim();
          if (containsCopy(value)) report(part, value, kind);
        }
      }
      function visit(node, parent) {
        if (node.type === 'JSXElement' && ['style', 'script'].includes(node.openingElement.name.name)) return;
        if (node.type === 'TemplateLiteral' && errorTemplates.has(node.quasis.map(part => part.value.cooked ?? part.value.raw).join('{}'))) return;
        if (node.type === 'JSXText') {
          const value = node.value.replace(/\s+/g, ' ').trim();
          if (containsCopy(value)) report(node, value, 'jsx-text');
        } else if (
          node.type === 'JSXAttribute' &&
          ['aria-label', 'title', 'placeholder', 'alt', 'label'].includes(node.name.name) &&
          node.value
        ) {
          displayed(node.value.type === 'JSXExpressionContainer' ? node.value.expression : node.value, 'jsx-attribute');
        } else if (node.type === 'JSXExpressionContainer' && parent?.type !== 'JSXAttribute') {
          displayed(node.expression, 'jsx-expression');
        } else if (node.type === 'TemplateElement' && /\p{Script=Han}/u.test(node.value.cooked ?? node.value.raw)) {
          const value = (node.value.cooked ?? node.value.raw).trim();
          if (containsCopy(value)) report(node, value, 'han-template');
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
            for (const child of value) if (child?.type) visit(child, node);
          } else if (value && typeof value === 'object' && typeof value.type === 'string') visit(value, node);
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
