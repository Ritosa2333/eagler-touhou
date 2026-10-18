#!/usr/bin/env node
import { existsSync, mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { dirname, relative, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { PRODUCT_GAMES } from '../../common/contracts/product-catalog.mjs';

const root = resolve(dirname(fileURLToPath(import.meta.url)), '../..');
const spec = JSON.parse(readFileSync(resolve(root, 'docs/adaptation/checklist.json'), 'utf8'));
const layout = JSON.parse(readFileSync(resolve(root, 'launcher/config/workspace.json'), 'utf8'));
const builds = JSON.parse(readFileSync(resolve(root, 'launcher/config/runtime-builds.json'), 'utf8'));
const slash = path => path.replaceAll('\\', '/');
export const MANUAL_EVIDENCE = '<!-- manual-verification -->';

export function preserveManualEvidence(generated, existing = '') {
  const start=existing.indexOf(MANUAL_EVIDENCE);
  return start<0 ? generated : generated.slice(0,generated.indexOf(MANUAL_EVIDENCE))+existing.slice(start);
}

export function portDefinitions() {
  const ports = new Map();
  for (const [game, entry] of Object.entries(builds.games)) {
    for (const variant of ['normal', 'multiplayer']) {
      const definition = entry.variants[variant];
      if (!definition) continue;
      const directory = layout.repositories[definition.workspaceRepository ?? entry.workspaceRepository];
      if (!directory?.startsWith('ports/')) throw new Error(`${game}/${variant} 缺少 ports/ 源码目录。`);
      const port = ports.get(directory) ?? { directory, game, variants: [] };
      if (port.game !== game) throw new Error(`${directory} 同时归属两个作品。`);
      port.variants.push(variant);
      ports.set(directory, port);
    }
  }
  return [...ports.values()].sort((a, b) => a.directory.localeCompare(b.directory, 'en'));
}

export function notApplicable(check, game, variant) {
  const product = PRODUCT_GAMES[game];
  const build = builds.games[game].variants[variant];
  if ((check.profile === 'multiplayer' || check.profile === 'multiplayer-cooperative') && variant === 'normal') return '单人模式';
  if (check.profile === 'multiplayer-cooperative' && product.multiplayer?.gameplay !== 'cooperative') {
    return '本作多人模式不是合作玩法';
  }
  if (check.profile === 'singleplayer' && variant === 'multiplayer') return 'MP 使用多人 Replay 自一致性检查';
  if (check.profile === 'midi' && !product.musicCapabilities?.midi) return '本作使用 OGG 音乐';
  if (check.profile === 'thprac' && build.features.thprac === false) return '该构建未提供 thprac';
  if (check.profile === 'language' && !product.features.languages) return '本作使用内置原文';
  return null;
}

export function adaptationMarkdown(port, {evidence = {}} = {}) {
  const lines = [`# ${port.game}${port.variants.length === 1 && port.variants[0] === 'multiplayer' ? ' MP' : ''} 适配检查`, '',
    '[开发入口](README.md) · [检查方法](../../docs/adaptation/README.md) · [全部作品](../README.md)', '',
    `源码目录：\`${port.directory}\`。构建模式：${port.variants.map(value => `\`${value}\``).join('、')}。`, '',
    '实现情况与实际验收分别记录。源码接口和模块检查通过后记为“已实现”；完成对应操作并附执行记录后记为“已验证”。', '',
    '状态：`待核对`、`进行中`、`待验证`、`已实现`、`已验证`、`检查失败`、`不适用`。', '',
    '[自动检查结果与源码身份](../../docs/adaptation/status.json) · `npm run ports:check -- --game=' + port.game + ' --variant=both`', ''];
  for (const section of spec.sections) {
    const checks = spec.checks.filter(check => check.section === section.id);
    if (!checks.length) continue;
    lines.push(`## ${section.title}`, '', `| 编号 | 检查项 | ${port.variants.join(' | ')} | 记录 |`,
      `| --- | --- | ${port.variants.map(() => '---').join(' | ')} | --- |`);
    for (const check of checks) {
      const states = port.variants.map(variant => notApplicable(check, port.game, variant) ? '不适用' : evidence[variant]?.[check.id]?.status ?? '待核对');
      const notes = port.variants.map(variant => {
        const reason = notApplicable(check, port.game, variant);
        return reason ? `${variant}：${reason}` : evidence[variant]?.[check.id]?.note ?? '';
      }).filter(Boolean).join('；') || '—';
      lines.push(`| ${check.id} | [${check.label}](../../docs/adaptation/${section.file}#${check.id.toLowerCase()}) | ${states.join(' | ')} | ${notes} |`);
    }
    lines.push('');
  }
  lines.push(MANUAL_EVIDENCE, '', '## 运行验收记录', '',
    '按检查编号附[验证记录](../../docs/adaptation/record-template.md)，写明源码身份、模式、环境、操作与结果。自动更新保留本节内容。');
  return `${lines.join('\n').trimEnd()}\n`;
}

function main(args) {
  if (!args.length || args.includes('--help')) {
    console.log('node tools/docs/new-adaptation.mjs --game=th10\nnode tools/docs/new-adaptation.mjs --all\n\n从构建配方建立适配表。已有表格保持原样。'); return;
  }
  if (args.length !== 1 || (!args[0].startsWith('--game=') && args[0] !== '--all')) throw new Error('参数：--game=作品 或 --all');
  const game = args[0] === '--all' ? null : args[0].slice(7);
  if (game && !builds.games[game]) throw new Error(`构建配方未登记作品：${game}`);
  for (const port of portDefinitions().filter(port => !game || port.game === game)) {
    const path = resolve(root, port.directory, 'ADAPTATION.md');
    if (existsSync(path)) { console.log(`保留 ${slash(relative(root, path))}`); continue; }
    if (!existsSync(resolve(root, port.directory))) throw new Error(`先检出 ${port.directory}。`);
    mkdirSync(dirname(path), { recursive: true });
    writeFileSync(path, adaptationMarkdown(port));
    console.log(`创建 ${slash(relative(root, path))}`);
  }
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  try { main(process.argv.slice(2)); } catch (error) { console.error(error.message); process.exitCode = 1; }
}
