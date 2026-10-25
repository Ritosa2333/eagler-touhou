#!/usr/bin/env node
import { existsSync, mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { dirname, relative, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { PRODUCT_GAMES } from '../../common/contracts/product-catalog.mjs';
import assert from 'node:assert/strict';

const root = resolve(dirname(fileURLToPath(import.meta.url)), '../..');
const spec = JSON.parse(readFileSync(resolve(root, 'docs/adaptation/checklist.json'), 'utf8'));
const layout = JSON.parse(readFileSync(resolve(root, 'launcher/config/workspace.json'), 'utf8'));
const builds = JSON.parse(readFileSync(resolve(root, 'launcher/config/runtime-builds.json'), 'utf8'));
const slash = path => path.replaceAll('\\', '/');
export const MANUAL_EVIDENCE = '<!-- manual-verification -->';

// A published review snapshot describes its recorded runs, independently of
// today's source checks. Raw browser reports and private inputs stay local.
export function readVerificationSnapshot(port, repositoryRoot = root) {
  const path = resolve(repositoryRoot, port.directory, 'docs/verification/adaptation.json');
  if (!existsSync(path)) return null;
  const snapshot = JSON.parse(readFileSync(path, 'utf8'));
  assert.equal(snapshot.schema, 'eagler-touhou/adaptation-review/1');
  assert.equal(snapshot.game, port.game);
  assert.match(snapshot.date, /^\d{4}-\d{2}-\d{2}$/);
  assert.match(snapshot.reviewedAtCommit, /^[a-f0-9]{40}$/);
  assert.equal(snapshot.record, 'docs/verification/adaptation.md');
  assert.ok(existsSync(resolve(repositoryRoot, port.directory, snapshot.record)), 'Missing readable adaptation review');
  assert.ok(snapshot.variants && Object.keys(snapshot.variants).length, 'Review must select at least one variant');
  const evidence = new Set();
  for (const item of snapshot.evidence) {
    assert.ok(item.id && !evidence.has(item.id), 'Duplicate review evidence');
    evidence.add(item.id);
    assert.match(item.sha256, /^[a-f0-9]{64}$/);
    assert.ok(Number.isSafeInteger(item.bytes) && item.bytes > 0);
    assert.ok(item.scope?.trim() && item.report?.trim());
    assert.ok(!/^(?:[A-Za-z]:|[\\/])/.test(item.report) && !item.report.split(/[\\/]/).includes('..'), 'Review report identifiers must be relative');
  }
  const statuses = new Set(['已验证', '部分验证', '检查失败', '本轮未验', '待材料', '暂缓验证', '不适用']);
  const checks = new Map(spec.checks.map(check => [check.id, check]));
  for (const [variant, rows] of Object.entries(snapshot.variants)) {
    assert.ok(port.variants.includes(variant), 'Review belongs to another variant');
    assert.equal(Object.keys(rows).length, checks.size, 'Review must account for every checklist item');
    for (const [id, row] of Object.entries(rows)) {
      assert.ok(checks.has(id), `Unknown review check ${id}`);
      assert.ok(statuses.has(row.status) && row.detail?.trim(), `Invalid review row ${id}`);
      assert.equal(row.status === '不适用', !!notApplicable(checks.get(id), port.game, variant), `Review applicability differs for ${id}`);
      assert.ok(Array.isArray(row.evidence) && row.evidence.every(ref => evidence.has(ref)), `Missing review evidence for ${id}`);
      assert.equal(row.complete, row.status === '已验证', `Review completeness differs for ${id}`);
      if (['已验证', '部分验证', '检查失败'].includes(row.status)) assert.ok(row.evidence.length, `Review needs evidence for ${id}`);
    }
  }
  return snapshot;
}

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

export function adaptationMarkdown(port, {evidence = {}, verification = null} = {}) {
  const lines = [`# ${port.game}${port.variants.length === 1 && port.variants[0] === 'multiplayer' ? ' MP' : ''} 适配检查`, '',
    '[开发入口](README.md) | [检查方法](../../docs/adaptation/README.md) | [全部作品](../README.md)', '',
    `源码目录：\`${port.directory}\`。构建模式：${port.variants.map(value => `\`${value}\``).join('、')}。`, '',
    '实现情况与实际验收分别记录。源码接口和模块检查通过后记为“已实现”；完成对应操作并附执行记录后记为“已验证”。', '',
    '状态：`待核对`、`进行中`、`待验证`、`已实现`、`已验证`、`检查失败`、`不适用`。', '',
    '[自动检查结果与源码身份](../../docs/adaptation/status.json) | `npm run ports:check -- --game=' + port.game + ' --variant=both`', ''];
  if (verification) lines.push(
    `运行验收快照：${verification.date}。[逐项结果、范围与证据](docs/verification/adaptation.md) | [结构化记录](docs/verification/adaptation.json)。`, '',
    '源码检查列记录接口、模块与构建检查；验收记录列记录各次运行所绑定的源码和产物。后续版本按相应身份复验。部分验证的已测行为与剩余范围见逐项记录。', '');
  for (const section of spec.sections) {
    const checks = spec.checks.filter(check => check.section === section.id);
    if (!checks.length) continue;
    const columns = port.variants.flatMap(variant => verification ? [`${variant} 源码检查`, `${variant} 验收记录`] : [variant]);
    lines.push(`## ${section.title}`, '', `| 编号 | 检查项 | ${columns.join(' | ')} | 记录 |`,
      `| --- | --- | ${columns.map(() => '---').join(' | ')} | --- |`);
    for (const check of checks) {
      const states = port.variants.flatMap(variant => {
        const state = notApplicable(check, port.game, variant) ? '不适用' : evidence[variant]?.[check.id]?.status ?? '待核对';
        if (!verification) return [state];
        const row = verification.variants[variant]?.[check.id];
        return [state, row ? `[${row.status}](docs/verification/adaptation.md#${check.id.toLowerCase()})` : '本轮未验'];
      });
      const notes = port.variants.map(variant => {
        const reason = notApplicable(check, port.game, variant);
        const note = reason ? `${variant}：${reason}` : evidence[variant]?.[check.id]?.note ?? '';
        return verification ? note.replace('按本项步骤执行，结果写入验证记录', '此项尚未登记源码检查结果') : note;
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
