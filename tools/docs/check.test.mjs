import assert from 'node:assert/strict';
import { mkdtempSync, mkdirSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { resolve } from 'node:path';
import test from 'node:test';
import { readFileSync } from 'node:fs';
import { adaptationMarkdown, notApplicable, portDefinitions } from './new-adaptation.mjs';
import {
  assertIndexWriteAllowed,
  checkDocumentLinks,
  documentAnchors,
  extractRequiredBehaviorContracts,
  isDocumentationPath,
  renderDocumentIndex,
  validateAdaptationMarkdown,
  validateChecklistContractMap,
  documentCategory,
  validateDocumentIndex,
} from './check.mjs';

const checklist = JSON.parse(readFileSync(new URL('../../docs/adaptation/checklist.json', import.meta.url), 'utf8'));
const capabilitySource = readFileSync(new URL('../../launcher/src/contracts/adapter-capabilities.mts', import.meta.url), 'utf8');
const requiredContracts = extractRequiredBehaviorContracts(capabilitySource);
const port = portDefinitions().find(item => item.game === 'th06');

test('license inventory includes license directories and excludes notice code and generated HTML', () => {
  for (const path of ['ports/th09/web/licenses/FreeType.txt', 'ports/th09/web/cpp/licenses/th08-reference-MIT.txt',
    'ports/th09/web/reference/MIT-TH08-codec.txt', 'ports/th06/src/thirdparty/STB_LICENSE']) {
    assert.equal(isDocumentationPath(path), true, path);
  }
  for (const path of ['launcher/tests/test-site-notice.mjs', 'launcher/tests/react-main/site-notice.checks.tsx',
    'launcher/public/content/FIRST_USE_NOTICE.html']) {
    assert.equal(isDocumentationPath(path), false, path);
  }
});

test('new mapped behavior extends the checklist without changing a checker count', () => {
  const contract = 'REQUIRED_INPUT_BEHAVIORS/new-input-behavior';
  const expanded = { ...checklist, checks: [...checklist.checks, { id: 'INPUT-NEW', contract }] };
  assert.deepEqual(validateChecklistContractMap(expanded, new Set([...requiredContracts, contract])), []);
  const omittedBuildStep = { ...checklist, checks: checklist.checks.filter(check => check.id !== 'BUILD-01') };
  assert.ok(validateChecklistContractMap(omittedBuildStep, requiredContracts).some(issue => issue.includes('BUILD-01')));
});

test('authored player links resolve at the published site root and still reject missing pages', () => {
  const root = mkdtempSync(resolve(tmpdir(), 'eagler-doc-content-'));
  try {
    mkdirSync(resolve(root, 'launcher/public'), { recursive: true });
    writeFileSync(resolve(root, 'launcher/public/faq.html'), '<h1 id="help">Help</h1>');
    assert.deepEqual(checkDocumentLinks({ docPath: 'launcher/content/FIRST_USE_NOTICE.md',
      content: '[帮助](faq.html#help)', root }), []);
    assert.equal(checkDocumentLinks({ docPath: 'launcher/content/FIRST_USE_NOTICE.md',
      content: '[丢失页面](missing.html)', root }).length, 1);
    assert.equal(checkDocumentLinks({ docPath: 'docs/example.md', content: '[错误相对路径](faq.html)', root }).length, 1);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

function rowWithStatus(markdown, id, variantColumn, status, note) {
  const lines = markdown.split('\n');
  const index = lines.findIndex(line => line.startsWith(`| ${id} |`));
  assert.notEqual(index, -1, `row ${id} exists`);
  const cells = lines[index].trim().replace(/^\|/, '').replace(/\|$/, '').split('|').map(cell => cell.trim());
  cells[variantColumn + 2] = status;
  if (note !== undefined) cells[cells.length - 1] = note;
  lines[index] = `| ${cells.join(' | ')} |`;
  return lines.join('\n');
}

test('generated adaptation table covers all 117 IDs and matches recipe columns', () => {
  const markdown = adaptationMarkdown(port);
  assert.deepEqual(validateAdaptationMarkdown(markdown, { port, checklist }), []);
});

test('cooperative multiplayer rules do not apply to TH09 versus mode', () => {
  const checks = checklist.checks.filter(check => check.id.startsWith('MP-RULE-'));
  assert.equal(checks.length, 5);
  for (const check of checks) {
    assert.equal(check.profile, 'multiplayer-cooperative');
    assert.match(notApplicable(check, 'th09', 'multiplayer'), /不是合作玩法/);
    assert.equal(notApplicable(check, 'th08', 'multiplayer'), null);
    assert.equal(notApplicable(check, 'th09', 'normal'), '单人模式');
  }
});

test('a missing checklist row fails', () => {
  const markdown = adaptationMarkdown(port).replace(/^\| BUILD-01 \|.*\n/m, '');
  assert.ok(validateAdaptationMarkdown(markdown, { port, checklist }).some(issue => issue.includes('缺少清单编号：BUILD-01')));
});

test('verified without an evidence link fails', () => {
  const markdown = rowWithStatus(adaptationMarkdown(port), 'BUILD-01', 0, '已验证', '已完成');
  assert.ok(validateAdaptationMarkdown(markdown, { port, checklist }).some(issue => issue.includes('已验证但没有证据链接')));
});

test('required high-refresh behavior cannot be marked not applicable', () => {
  const markdown = rowWithStatus(adaptationMarkdown(port), 'DISPLAY-02', 0, '不适用', '本机不测高刷新');
  assert.ok(validateAdaptationMarkdown(markdown, { port, checklist }).some(issue => issue.includes('DISPLAY-02/normal 不能标为“不适用”')));
});

test('a newly declared required behavior must be mapped by the checklist', () => {
  const extendedSource = `${capabilitySource}\nexport const REQUIRED_NEW_BEHAVIORS = Object.freeze([{ id: "new-required-contract", summary: "new rule" }]);\n`;
  const extendedContracts = extractRequiredBehaviorContracts(extendedSource);
  assert.ok(extendedContracts.has('REQUIRED_NEW_BEHAVIORS/new-required-contract'));
  assert.ok(validateChecklistContractMap(checklist, extendedContracts).some(issue => issue.includes('缺少能力契约：REQUIRED_NEW_BEHAVIORS/new-required-contract')));
});

test('behavior ID extraction ignores commented source examples', () => {
  const contracts = extractRequiredBehaviorContracts('export const REQUIRED_SAMPLE_BEHAVIORS = Object.freeze([{ id: "real" }, /* { id: "commented" } */]);');
  assert.deepEqual([...contracts], ['REQUIRED_SAMPLE_BEHAVIORS/real']);
});

test('index writes require every document to be readable from the worktree or Git index', () => {
  assert.doesNotThrow(() => assertIndexWriteAllowed([
    {path: 'README.md', content: '# Workspace'},
    {path: 'ports/th09/README.md', content: '# Indexed title'},
  ]));
  assert.throws(() => assertIndexWriteAllowed([
    {path: 'README.md', content: '# Workspace'},
    {path: 'ports/th09/README.md', content: null, unreadable: true},
  ]), /ports\/th09\/README\.md/);
});

test('license notices are indexed while numeric dumps are excluded', () => {
  for (const path of [
    'common/include/eagler/thprac/THPRAC-LICENSE.txt',
    'launcher/public/assets/fonts/OFL-Unifont.txt',
    'ports/th11-mp/THIRD-PARTY-NOTICES.txt',
  ]) {
    assert.equal(isDocumentationPath(path), true, path);
    assert.equal(documentCategory(path), '许可证与声明', path);
  }
  assert.equal(isDocumentationPath('ports/th20/scripts/recovered/anm/e00a0.anm.txt'), false);
  assert.equal(isDocumentationPath('data/2024.01.txt'), false);
});

test('broken relative Markdown and HTML links fail while sparse-indexed source links pass', () => {
  const root = mkdtempSync(resolve(tmpdir(), 'docs-check-'));
  try {
    mkdirSync(resolve(root, 'docs'), { recursive: true });
    const broken = checkDocumentLinks({
      docPath: 'docs/page.md',
      content: '[missing](missing.md)\n<a href="absent.html">broken</a>\n<img src="absent.png">',
      root,
    });
    assert.equal(broken.length, 3);

    const sparseSource = checkDocumentLinks({
      docPath: 'docs/page.md',
      content: '[source](../ports/th06/source/game.cpp)',
      root,
      knownPaths: new Set(['ports/th06/source/game.cpp']),
    });
    assert.deepEqual(sparseSource, []);

    writeFileSync(resolve(root, 'docs/target.md'), '# Existing section\n');
    const badAnchor = checkDocumentLinks({
      docPath: 'docs/page.md',
      content: '[section](target.md#missing-section)',
      root,
    });
    assert.ok(badAnchor.some(issue => issue.includes('没有锚点 #missing-section')));
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('document index lists every registry entry and heading anchors use rendered slugs', () => {
  const documents = [
    { path: 'docs/INDEX.md', title: '文档总表', category: '文档总表' },
    { path: 'ports/th06/README.md', title: 'TH06 入口', category: '作品入口' },
  ];
  const content = renderDocumentIndex(documents);
  assert.deepEqual(validateDocumentIndex(content, new Set(documents.map(document => document.path))), []);
  assert.ok(documentAnchors('# BUILD-01：Sources and build\n\n```md\n## fake-anchor\n```\n').has('build-01sources-and-build'));
  assert.ok(!documentAnchors('# Real heading\n\n```md\n## fake-anchor\n```\n').has('fake-anchor'));
  assert.ok(documentAnchors('= Guide\n\n== Runtime Setup\n', '.adoc').has('runtime-setup'));
});
