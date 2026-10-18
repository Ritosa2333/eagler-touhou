#!/usr/bin/env node
import { existsSync, readFileSync, statSync, writeFileSync } from 'node:fs';
import { dirname, extname, posix, relative, resolve, sep } from 'node:path';
import { spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import { adaptationMarkdown, notApplicable, portDefinitions } from './new-adaptation.mjs';
import {checkStates, readStatusReport, validateStatusReport} from '../ports/status.mjs';

const PROJECT_ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '../..');
const INDEX_PATH = 'docs/INDEX.md';
const CHECKLIST_PATH = 'docs/adaptation/checklist.json';
const CAPABILITIES_PATH = 'launcher/src/contracts/adapter-capabilities.mts';
const REQUIRED_MANUAL_CHECKS = [
  ...Array.from({ length: 6 }, (_, index) => `BUILD-${String(index + 1).padStart(2, '0')}`),
  ...Array.from({ length: 6 }, (_, index) => `RUNTIME-${String(index + 1).padStart(2, '0')}`),
  ...Array.from({ length: 5 }, (_, index) => `MP-RULE-${String(index + 1).padStart(2, '0')}`),
];
const STATES = new Set(['待核对', '进行中', '待验证', '已实现', '已验证', '检查失败', '不适用']);
const DOCUMENT_EXTENSIONS = new Set(['.md', '.markdown', '.mdx', '.rst', '.adoc', '.asciidoc']);
const CATEGORY_ORDER = ['项目入口', '文档总表', '适配指南', '作品入口', '作品适配表', '作品源码文档', '公共库', 'Launcher', 'Relay 服务', 'TURN 服务', 'Telemetry 服务', '服务', '开发工具', '架构说明', '生成报告', '迁移记录', '历史记录', '许可证与声明', '协作约定', 'CI 与协作模板', '项目文档', '其他说明'];
const INSTRUCTION_TEXT_NAMES = /^(?:agents?|workspace|license|licence|copying|notice|readme|contribut(?:ing|ors?)|install(?:ation)?|setup|build|development|instructions?|guide|manual|documentation|security|support|usage|faq|changelog|changes|history|authors?|acknowledg(?:e)?ments?)(?:[._ -].*)?\.txt$/i;
const LICENSE_TEXT_NAME = /(?:^|[._ -])(?:license|licence|copying|notices?|ofl)(?:[._ -]|$)/i;
const NUMERIC_DUMP_NAME = /^(?:\d+(?:[._-]\d+)*|[a-f0-9]{32,})\.txt$/i;

function slash(path) { return path.replaceAll('\\', '/'); }

function git(root, args, options = {}) {
  const result = spawnSync('git', args, {
    cwd: root,
    encoding: options.encoding ?? 'utf8',
    maxBuffer: options.maxBuffer ?? 64 * 1024 * 1024,
  });
  if (result.error) throw result.error;
  return result;
}

function nulSeparated(output) {
  return output.split('\0').filter(Boolean).map(slash);
}

export function isDocumentationPath(path) {
  const name = path.split('/').at(-1) ?? '';
  const extension = extname(name).toLowerCase();
  if (DOCUMENT_EXTENSIONS.has(extension)) return true;
  // A notice controller or a generated notice page is code/output, not prose.
  if (!['', '.txt', '.license', '.licence', '.eagler-common'].includes(extension)) return false;
  if (LICENSE_TEXT_NAME.test(name) || /(?:^|\/)licen[sc]es?\//i.test(path)) return true;
  if (/^(?:STB_LICENSE|COPYING|LICENSE|LICENCE)$/.test(name) || /^MIT-.*\.txt$/i.test(name)) return true;
  if (extension !== '.txt' || NUMERIC_DUMP_NAME.test(name)) return false;
  return INSTRUCTION_TEXT_NAMES.test(name) || path.startsWith('docs/');
}

export function extractRequiredBehaviorContracts(source) {
  const contracts = new Set();
  const declarations = [...source.matchAll(/\bexport\s+const\s+(REQUIRED_[A-Z0-9_]*_BEHAVIORS)\s*=\s*Object\.freeze\s*\(\s*\[/g)];
  for (const declaration of declarations) {
    const arrayStart = declaration.index + declaration[0].lastIndexOf('[');
    const arrayEnd = matchingArrayEnd(source, arrayStart);
    if (arrayEnd < 0) throw new Error(`无法解析 ${declaration[1]} 的行为表`);
    const body = removeJavaScriptComments(source.slice(arrayStart + 1, arrayEnd));
    const ids = stringPropertyValues(body, 'id');
    if (!ids.length) throw new Error(`${declaration[1]} 没有行为 ID`);
    for (const id of ids) {
      const contract = `${declaration[1]}/${id}`;
      if (contracts.has(contract)) throw new Error(`重复的能力契约：${contract}`);
      contracts.add(contract);
    }
  }
  if (!declarations.length) throw new Error('未找到 REQUIRED_*_BEHAVIORS 能力表');
  return contracts;
}

function matchingArrayEnd(source, start) {
  let depth = 0;
  let quote = null;
  let lineComment = false;
  let blockComment = false;
  let escaped = false;
  for (let i = start; i < source.length; i++) {
    const char = source[i];
    const next = source[i + 1];
    if (lineComment) {
      if (char === '\n') lineComment = false;
      continue;
    }
    if (blockComment) {
      if (char === '*' && next === '/') { blockComment = false; i++; }
      continue;
    }
    if (quote) {
      if (escaped) { escaped = false; continue; }
      if (char === '\\') { escaped = true; continue; }
      if (char === quote) quote = null;
      continue;
    }
    if (char === '/' && next === '/') { lineComment = true; i++; continue; }
    if (char === '/' && next === '*') { blockComment = true; i++; continue; }
    if (char === '"' || char === "'" || char === '`') { quote = char; continue; }
    if (char === '[') depth++;
    else if (char === ']' && --depth === 0) return i;
  }
  return -1;
}

function removeJavaScriptComments(source) {
  let output = '';
  let quote = null;
  let lineComment = false;
  let blockComment = false;
  let escaped = false;
  for (let i = 0; i < source.length; i++) {
    const char = source[i], next = source[i + 1];
    if (lineComment) {
      if (char === '\n') { lineComment = false; output += '\n'; }
      else output += ' ';
      continue;
    }
    if (blockComment) {
      if (char === '*' && next === '/') { output += '  '; i++; blockComment = false; }
      else output += char === '\n' ? '\n' : ' ';
      continue;
    }
    if (quote) {
      output += char;
      if (escaped) { escaped = false; continue; }
      if (char === '\\') { escaped = true; continue; }
      if (char === quote) quote = null;
      continue;
    }
    if (char === '/' && next === '/') { output += '  '; i++; lineComment = true; continue; }
    if (char === '/' && next === '*') { output += '  '; i++; blockComment = true; continue; }
    if (char === '"' || char === "'" || char === '`') quote = char;
    output += char;
  }
  return output;
}

function stringPropertyValues(source, property) {
  const values = [];
  for (let i = 0; i < source.length;) {
    const char = source[i];
    if (char === '"' || char === "'" || char === '`') {
      const quote = char;
      let escaped = false;
      i++;
      while (i < source.length) {
        if (escaped) { escaped = false; i++; continue; }
        if (source[i] === '\\') { escaped = true; i++; continue; }
        if (source[i] === quote) { i++; break; }
        i++;
      }
      continue;
    }
    if (/[A-Za-z_$]/.test(char)) {
      const start = i++;
      while (i < source.length && /[A-Za-z0-9_$]/.test(source[i])) i++;
      if (source.slice(start, i) !== property) continue;
      let cursor = i;
      while (/\s/.test(source[cursor] ?? '')) cursor++;
      if (source[cursor++] !== ':') continue;
      while (/\s/.test(source[cursor] ?? '')) cursor++;
      const quote = source[cursor];
      if (quote !== '"' && quote !== "'") continue;
      const valueStart = ++cursor;
      let escaped = false;
      while (cursor < source.length) {
        if (escaped) { escaped = false; cursor++; continue; }
        if (source[cursor] === '\\') { escaped = true; cursor++; continue; }
        if (source[cursor] === quote) break;
        cursor++;
      }
      if (cursor < source.length) values.push(source.slice(valueStart, cursor));
      i = cursor + 1;
      continue;
    }
    i++;
  }
  return values;
}

export function validateChecklistContractMap(checklist, requiredContracts, expectedCount) {
  const issues = [];
  const checks = Array.isArray(checklist?.checks) ? checklist.checks : [];
  if (expectedCount !== undefined && checks.length !== expectedCount) {
    issues.push(`适配清单应有 ${expectedCount} 项，实际为 ${checks.length} 项。`);
  }
  const ids = new Set();
  const mapped = new Set();
  for (const check of checks) {
    if (!check?.id || ids.has(check.id)) issues.push(`适配清单编号缺失或重复：${check?.id ?? '(空)'}`);
    ids.add(check?.id);
    if (!check?.contract) continue;
    if (mapped.has(check.contract)) issues.push(`清单重复映射能力契约：${check.contract}`);
    mapped.add(check.contract);
  }
  for (const contract of [...requiredContracts].sort()) {
    if (!mapped.has(contract)) issues.push(`适配清单缺少能力契约：${contract}`);
  }
  for (const contract of [...mapped].sort()) {
    if (!requiredContracts.has(contract)) issues.push(`适配清单引用未知能力契约：${contract}`);
  }
  if (expectedCount === undefined) {
    for (const id of REQUIRED_MANUAL_CHECKS) {
      if (!ids.has(id)) issues.push(`适配清单缺少基础流程项：${id}`);
    }
  }
  return issues;
}

function cellsFromTableLine(line) {
  if (!/^\s*\|/.test(line)) return null;
  const cells = line.trim().replace(/^\|/, '').replace(/\|$/, '').split('|');
  return cells.map(cell => cell.trim());
}

function isTableSeparator(line) {
  const cells = cellsFromTableLine(line);
  return Boolean(cells?.length && cells.every(cell => /^:?-{3,}:?$/.test(cell)));
}

function parseAdaptationTables(markdown) {
  const lines = markdown.split(/\r?\n/);
  const tables = [];
  for (let i = 0; i < lines.length; i++) {
    const headers = cellsFromTableLine(lines[i]);
    if (!headers || headers[0] !== '编号' || headers[1] !== '检查项') continue;
    const table = { line: i + 1, headers, rows: [] };
    if (!isTableSeparator(lines[i + 1] ?? '')) {
      table.separatorMissing = true;
    } else {
      i += 2;
      for (; i < lines.length; i++) {
        const cells = cellsFromTableLine(lines[i]);
        if (!cells) { i--; break; }
        if (isTableSeparator(lines[i])) continue;
        table.rows.push({ line: i + 1, cells });
      }
    }
    tables.push(table);
  }
  return tables;
}

function usefulReason(text) {
  const normalized = text.replace(/[`*_]/g, '').trim();
  return normalized.length > 0 && !/^(?:[—–-]|n\/?a)$/i.test(normalized);
}

export function validateAdaptationMarkdown(markdown, { port, checklist, notApplicableFor = notApplicable, evidence }) {
  const issues = [];
  const tables = parseAdaptationTables(markdown);
  const expectedVariants = port.variants;
  const expectedById = new Map(checklist.checks.map(check => [check.id, check]));
  const seen = new Set();
  if (!tables.length) return [`${port.directory}/ADAPTATION.md 缺少检查表。`];

  for (const table of tables) {
    const expectedHeaders = [...expectedVariants, '记录'];
    const actualHeaders = table.headers.slice(2);
    if (table.separatorMissing) issues.push(`${port.directory}/ADAPTATION.md:${table.line} 缺少表格分隔行。`);
    if (actualHeaders.length !== expectedHeaders.length || actualHeaders.some((header, index) => header !== expectedHeaders[index])) {
      issues.push(`${port.directory}/ADAPTATION.md:${table.line} 表头应为 ${expectedHeaders.join(' | ')}，实际为 ${actualHeaders.join(' | ')}。`);
    }
    for (const row of table.rows) {
      const id = row.cells[0];
      if (row.cells.length !== table.headers.length) {
        issues.push(`${port.directory}/ADAPTATION.md:${row.line} ${id || '(空编号)'} 列数与表头不一致。`);
        continue;
      }
      const check = expectedById.get(id);
      if (!check) {
        issues.push(`${port.directory}/ADAPTATION.md:${row.line} 存在未知编号：${id || '(空)'}`);
        continue;
      }
      if (seen.has(id)) issues.push(`${port.directory}/ADAPTATION.md:${row.line} 编号重复：${id}`);
      seen.add(id);
      const note = row.cells.at(-1) ?? '';
      for (let column = 0; column < expectedVariants.length; column++) {
        const variant = expectedVariants[column];
        const status = row.cells[column + 2];
        if (evidence && (['已实现','检查失败'].includes(status) || id==='BUILD-01') && evidence[variant]?.[id]?.status !== status)
          issues.push(`${port.directory}/ADAPTATION.md:${row.line} ${id}/${variant} 与执行结果不一致。`);
        if (!STATES.has(status)) {
          issues.push(`${port.directory}/ADAPTATION.md:${row.line} ${id}/${variant} 状态无效：${status}`);
          continue;
        }
        const reason = notApplicableFor(check, port.game, variant);
        if (reason && status !== '不适用') {
          issues.push(`${port.directory}/ADAPTATION.md:${row.line} ${id}/${variant} 应标为“不适用”：${reason}`);
        }
        if (status === '不适用') {
          if (reason) {
            if (!note.includes(reason)) issues.push(`${port.directory}/ADAPTATION.md:${row.line} ${id}/${variant} 需记录配方原因“${reason}”。`);
          } else if (check.contract) {
            issues.push(`${port.directory}/ADAPTATION.md:${row.line} 必需能力 ${id}/${variant} 不能标为“不适用”。`);
          } else if (!usefulReason(note)) {
            issues.push(`${port.directory}/ADAPTATION.md:${row.line} ${id}/${variant} 标为“不适用”时必须填写原因。`);
          }
        }
        if (status === '已验证' && extractDocumentLinks(note, '.md').length === 0) {
          issues.push(`${port.directory}/ADAPTATION.md:${row.line} ${id}/${variant} 已验证但没有证据链接。`);
        }
      }
    }
  }

  for (const id of expectedById.keys()) {
    if (!seen.has(id)) issues.push(`${port.directory}/ADAPTATION.md 缺少清单编号：${id}`);
  }
  return issues;
}

function blankLines(value) { return value.replace(/[^\n]/g, ' '); }

function stripCodeAndComments(source, extension, stripInline = true) {
  const lines = source.split(/\r?\n/);
  let fence = null;
  let inComment = false;
  let rstIndent = null;
  const output = [];
  for (const line of lines) {
    const trimmed = line.trimStart();
    if (fence) {
      output.push('');
      if (new RegExp(`^\\s{0,3}${fence[0]}{${fence.length},}\\s*$`).test(line)) fence = null;
      continue;
    }
    if (extension === '.md' || extension === '.markdown' || extension === '.mdx') {
      const match = line.match(/^\s{0,3}(`{3,}|~{3,})/);
      if (match) { fence = match[1]; output.push(''); continue; }
    }
    if (extension === '.adoc' || extension === '.asciidoc') {
      if (/^\s*----\s*$/.test(line)) { fence = '----'; output.push(''); continue; }
    }
    if (inComment) {
      if (line.includes('-->')) inComment = false;
      output.push('');
      continue;
    }
    const commentAt = line.indexOf('<!--');
    if (commentAt >= 0) {
      if (line.indexOf('-->', commentAt + 4) < 0) inComment = true;
      output.push(line.slice(0, commentAt));
      continue;
    }
    if (extension === '.rst') {
      const indent = line.match(/^(\s*)\S/)?.[1].length ?? 0;
      if (rstIndent !== null) {
        if (!trimmed || indent > rstIndent) { output.push(''); continue; }
        rstIndent = null;
      }
      if (/::\s*$/.test(line)) rstIndent = indent;
    }
    output.push(line);
  }
  let cleaned = output.join('\n');
  if (stripInline) cleaned = cleaned.replace(/(`+)(?!`)([^\n]*?)\1/g, (_match, _ticks, inner) => blankLines(inner));
  return cleaned;
}

function markdownDestination(raw) {
  let value = raw.trim();
  if (value.startsWith('<') && value.includes('>')) value = value.slice(1, value.indexOf('>'));
  else value = value.replace(/\s+(?:"[^"]*"|'[^']*')\s*$/, '').trim();
  return value.replace(/\\([\\()])/g, '$1');
}

function markdownInlineLinks(source) {
  const links = [];
  for (let i = 0; i < source.length; i++) {
    if (source[i] !== '[') continue;
    const labelStart = i + 1;
    let cursor = labelStart;
    let bracketDepth = 1;
    let escaped = false;
    for (; cursor < source.length; cursor++) {
      const char = source[cursor];
      if (escaped) { escaped = false; continue; }
      if (char === '\\') { escaped = true; continue; }
      if (char === '[') bracketDepth++;
      if (char === ']' && --bracketDepth === 0) break;
    }
    if (cursor >= source.length || source[cursor + 1] !== '(') continue;
    const label = source.slice(labelStart, cursor);
    let destinationStart = cursor + 2;
    let depth = 1;
    let inAngle = false;
    let quote = null;
    escaped = false;
    cursor = destinationStart;
    for (; cursor < source.length; cursor++) {
      const char = source[cursor];
      if (escaped) { escaped = false; continue; }
      if (char === '\\') { escaped = true; continue; }
      if (quote) { if (char === quote) quote = null; continue; }
      if (inAngle) { if (char === '>') inAngle = false; continue; }
      if (char === '"' || char === "'") { quote = char; continue; }
      if (char === '<') { inAngle = true; continue; }
      if (char === '(') depth++;
      if (char === ')' && --depth === 0) break;
    }
    if (cursor >= source.length) continue;
    links.push({ target: markdownDestination(source.slice(destinationStart, cursor)), label, index: i });
    i = cursor;
  }
  return links;
}

export function extractDocumentLinks(source, extension = '.md') {
  const cleaned = stripCodeAndComments(source, extension.toLowerCase());
  const links = [];
  const add = (target, label, index, kind) => {
    if (!target) return;
    links.push({ target: target.trim(), label: (label ?? '').trim(), line: 1 + cleaned.slice(0, index).split('\n').length - 1, kind });
  };

  for (const link of markdownInlineLinks(cleaned)) add(link.target, link.label, link.index, 'markdown');
  for (const match of cleaned.matchAll(/^\s{0,3}\[([^\]]+)\]:\s*(\S+)(?:\s+.*)?$/gm)) {
    add(markdownDestination(match[2]), match[1], match.index, 'markdown-reference');
  }
  for (const match of cleaned.matchAll(/<a\b[^>]*\bhref\s*=\s*(?:"([^"]*)"|'([^']*)'|([^\s>]+))[^>]*>([\s\S]*?)<\/a\s*>/gi)) {
    const label = (match[4] ?? '').replace(/<[^>]*>/g, '').trim();
    add(match[1] ?? match[2] ?? match[3], label, match.index, 'html-anchor');
  }
  for (const match of cleaned.matchAll(/\b(?:href|src)\s*=\s*(?:"([^"]*)"|'([^']*)'|([^\s>]+))/gi)) {
    add(match[1] ?? match[2] ?? match[3], '', match.index, 'html');
  }
  if (extension.toLowerCase() === '.rst') {
    for (const match of cleaned.matchAll(/^\s*\.\.\s+image::\s*(\S+)/gm)) add(match[1], '', match.index, 'rst-image');
    for (const match of cleaned.matchAll(/^\s*\.\.\s+(?:include|download)::\s*(\S+)/gm)) add(match[1], '', match.index, 'rst-include');
    for (const match of cleaned.matchAll(/^\s*\.\.\s+_[^:]+:\s*(\S+)/gm)) add(match[1], '', match.index, 'rst-target');
    for (const match of cleaned.matchAll(/`([^`<>]+?)\s*<([^>]+)>`_/g)) add(match[2], match[1], match.index, 'rst-inline');
    for (const match of cleaned.matchAll(/:doc:`([^`]+)`/g)) add(match[1], '', match.index, 'rst-doc');
    const rstLines = cleaned.split('\n');
    let offset = 0;
    for (let i = 0; i < rstLines.length; i++) {
      const directive = rstLines[i].match(/^(\s*)\.\.\s+toctree::\s*$/);
      if (directive) {
        const baseIndent = directive[1].length;
        let lineOffset = offset + rstLines[i].length + 1;
        for (let j = i + 1; j < rstLines.length; j++) {
          const line = rstLines[j];
          const trimmed = line.trim();
          const indent = line.match(/^\s*/)?.[0].length ?? 0;
          if (!trimmed) { lineOffset += line.length + 1; continue; }
          if (indent <= baseIndent) break;
          if (!trimmed.startsWith(':')) add(trimmed, '', lineOffset, 'rst-toctree');
          lineOffset += line.length + 1;
        }
      }
      offset += rstLines[i].length + 1;
    }
  }
  if (extension.toLowerCase() === '.adoc' || extension.toLowerCase() === '.asciidoc') {
    for (const match of cleaned.matchAll(/\b(?:link|image)::?([^\s\[]+)\[[^\]]*\]/g)) add(match[1], '', match.index, 'asciidoc');
    for (const match of cleaned.matchAll(/\binclude::([^\s\[]+)\[[^\]]*\]/g)) add(match[1], '', match.index, 'asciidoc-include');
    for (const match of cleaned.matchAll(/<<([^,>]+)(?:,([^>]*))?>>/g)) add(match[1], match[2] ?? '', match.index, 'asciidoc-xref');
    for (const match of cleaned.matchAll(/\bxref:([^\s\[]+)\[[^\]]*\]/g)) add(match[1], '', match.index, 'asciidoc-xref');
  }

  const deduplicated = new Map();
  for (const link of links) {
    const key = `${link.target}\0${link.line}`;
    const current = deduplicated.get(key);
    if (!current || (!current.label && link.label)) deduplicated.set(key, link);
  }
  return [...deduplicated.values()];
}

function externalTarget(target) {
  return /^(?:[a-z][a-z0-9+.-]*:|\/\/)/i.test(target);
}

function decodeUrlPart(value) {
  try { return decodeURIComponent(value); }
  catch { return null; }
}

function targetParts(target) {
  const hash = target.indexOf('#');
  const rawPath = hash >= 0 ? target.slice(0, hash) : target;
  const fragment = hash >= 0 ? target.slice(hash + 1) : '';
  const query = rawPath.indexOf('?');
  const path = query >= 0 ? rawPath.slice(0, query) : rawPath;
  const decodedPath = decodeUrlPart(path);
  const decodedFragment = decodeUrlPart(fragment);
  return { path: decodedPath, fragment: decodedFragment, invalidEncoding: decodedPath === null || decodedFragment === null };
}

function headingText(line) {
  return line.replace(/^\s{0,3}#{1,6}\s+/, '').replace(/\s+#+\s*$/, '').trim();
}

function slugHeading(value) {
  return value
    .replace(/!?\[([^\]]*)\]\([^)]*\)/g, '$1')
    .replace(/`([^`]*)`/g, '$1')
    .replace(/<[^>]*>/g, '')
    .replace(/&(?:amp|lt|gt|quot|#39);/gi, ' ')
    .trim().toLowerCase()
    .replace(/[^\p{L}\p{N}_\-\s]/gu, '')
    .replace(/\s+/g, '-')
    .replace(/-+/g, '-');
}

export function documentAnchors(source, extension = '.md') {
  source = stripCodeAndComments(source, extension, false);
  const anchors = new Set();
  const explicit = [...source.matchAll(/\b(?:id|name)\s*=\s*["']([^"']+)["']/gi)];
  for (const match of explicit) anchors.add(match[1]);
  for (const match of source.matchAll(/^\s*\.\.\s+_([^:]+):/gm)) anchors.add(match[1].trim());
  for (const match of source.matchAll(/\[\[([^,\]]+)(?:,[^\]]*)?\]\]/g)) anchors.add(match[1].trim());
  for (const match of source.matchAll(/\{#([^}]+)\}/g)) anchors.add(match[1].trim());
  for (const match of source.matchAll(/^\s{0,3}#{1,6}\s+(.+?)\s*#*\s*$/gm)) {
    const base = slugHeading(headingText(match[0]));
    if (!base) continue;
    let slug = base;
    let suffix = 0;
    while (anchors.has(slug)) slug = `${base}-${++suffix}`;
    anchors.add(slug);
  }
  if (extension === '.adoc' || extension === '.asciidoc') {
    for (const match of source.matchAll(/^\s{0,3}={1,6}\s+(.+?)\s*$/gm)) {
      const slug = slugHeading(match[1]);
      if (slug) anchors.add(slug);
    }
  }
  const lines = source.split(/\r?\n/);
  for (let i = 0; i < lines.length - 1; i++) {
    if (!lines[i].trim() || !/^\s*(?:=+|-+|~+|\^+|"+|#+|\*+|\++|`+|:+|'+){3,}\s*$/.test(lines[i + 1])) continue;
    const base = slugHeading(lines[i].trim());
    if (base) anchors.add(base);
  }
  for (const match of source.matchAll(/^\s*\[#([^\]]+)\]\s*$/gm)) anchors.add(match[1]);
  return anchors;
}

function localRelativePath(docPath, linkPath) {
  const normalized = slash(linkPath);
  if (normalized.startsWith('/')) return { external: true };
  const destination = posix.normalize(posix.join(posix.dirname(docPath), normalized));
  if (destination === '..' || destination.startsWith('../')) return { error: '链接越出仓库根目录' };
  return { path: destination };
}

export function checkDocumentLinks({ docPath, content, root = PROJECT_ROOT, knownPaths = new Set(), readIndexedFile }) {
  const issues = [];
  const ext = extname(docPath).toLowerCase();
  const siteContent = docPath === 'launcher/NOTICE.txt' || docPath === 'launcher/docs/FAQ.md'
    || /^launcher\/content\/[^/]+\.md$/.test(docPath);
  // Authored player content is rendered at the published site's root. Other
  // plain-text license files are indexed verbatim and have no Markdown links.
  if (!DOCUMENT_EXTENSIONS.has(ext) && !siteContent) return issues;
  const linkBase = siteContent ? 'launcher/public/index.html' : docPath;
  const links = extractDocumentLinks(content, ext);
  for (const link of links) {
    const target = link.target;
    if (!target || externalTarget(target) || target.startsWith('#')) {
      if (target.startsWith('#') && target.length > 1 && !documentAnchors(content, ext).has(decodeUrlPart(target.slice(1)))) {
        issues.push(`${docPath}:${link.line} 找不到本页锚点 #${target.slice(1)}。`);
      }
      continue;
    }
    const parts = targetParts(target);
    if (parts.invalidEncoding) {
      issues.push(`${docPath}:${link.line} 链接包含无效百分号编码：${target}`);
      continue;
    }
    if (!parts.path) {
      if (parts.fragment && !documentAnchors(content, ext).has(parts.fragment)) issues.push(`${docPath}:${link.line} 找不到本页锚点 #${parts.fragment}。`);
      continue;
    }
    const resolved = localRelativePath(linkBase, parts.path);
    if (resolved.external) continue;
    if (resolved.error) {
      issues.push(`${docPath}:${link.line} ${resolved.error}：${target}`);
      continue;
    }
    const baseDestination = resolved.path;
    let inferredExtensions = ['.md', '.markdown', '.mdx', '.rst', '.adoc', '.asciidoc', '.html', '.htm'];
    if (link.kind.startsWith('rst')) inferredExtensions = ['.rst', '.md', '.markdown', '.html'];
    if (link.kind.startsWith('asciidoc')) inferredExtensions = ['.adoc', '.asciidoc', '.md', '.html'];
    const candidates = extname(baseDestination)
      ? [baseDestination]
      : [baseDestination, ...inferredExtensions.map(extension => `${baseDestination}${extension}`),
        ...['README.md', 'README.rst', 'README.adoc'].map(name => posix.join(baseDestination, name))];
    const destination = candidates.find(candidate => {
      const absoluteCandidate = resolve(root, candidate);
      return existsSync(absoluteCandidate) || knownPaths.has(candidate) || [...knownPaths].some(path => path.startsWith(`${candidate.replace(/\/$/, '')}/`));
    }) ?? baseDestination;
    const absolute = resolve(root, destination);
    if (absolute !== root && !absolute.startsWith(root + sep)) {
      issues.push(`${docPath}:${link.line} 链接越出仓库根目录：${target}`);
      continue;
    }
    const indexedDirectory = [...knownPaths].some(path => path.startsWith(`${destination.replace(/\/$/, '')}/`));
    const exists = existsSync(absolute) || knownPaths.has(destination) || indexedDirectory;
    if (!exists) {
      issues.push(`${docPath}:${link.line} 链接目标不存在：${target}`);
      continue;
    }
    if (!parts.fragment || /^L\d+(?:-L\d+)?$/i.test(parts.fragment)) continue;
    const targetExtension = extname(destination).toLowerCase();
    if (![...DOCUMENT_EXTENSIONS, '.html', '.htm', '.svg'].includes(targetExtension)) continue;
    const targetContent = readDocument(destination, root, knownPaths, readIndexedFile);
    if (targetContent === null) continue;
    if (!documentAnchors(targetContent, targetExtension).has(parts.fragment)) {
      issues.push(`${docPath}:${link.line} ${destination} 没有锚点 #${parts.fragment}。`);
    }
  }
  return issues;
}

function readDocument(path, root, knownPaths, readIndexedFile) {
  const absolute = resolve(root, path);
  try {
    if (existsSync(absolute) && statSync(absolute).isFile()) return readFileSync(absolute, 'utf8');
  } catch { /* Fall through to the index for sparse checkout. */ }
  if (knownPaths.has(path) && readIndexedFile) return readIndexedFile(path);
  return null;
}

export function documentTitle(path, content) {
  const extension = extname(path).toLowerCase();
  const lines = content.replace(/^\uFEFF/, '').split(/\r?\n/);
  let frontMatter = false;
  const usable = [];
  for (const line of lines) {
    if (line.trim() === '---' && !usable.length) { frontMatter = !frontMatter; continue; }
    if (frontMatter) continue;
    usable.push(line);
  }
  for (let i = 0; i < usable.length; i++) {
    const line = usable[i].trim();
    if (!line) continue;
    const markdown = line.match(/^#{1,6}\s+(.+?)\s*#*$/);
    if (markdown) return markdown[1].replace(/[`*_]/g, '').trim();
    const asciidoc = line.match(/^=\s+(.+)$/);
    if (asciidoc) return asciidoc[1].trim();
    if (extension === '.rst' && i + 1 < usable.length && /^\s*(?:=+|-+|~+|\^+|"+|#+|\*+|\++|`+|:+|'+){3,}\s*$/.test(usable[i + 1])) return line;
    if (extension === '.txt' && line.length <= 120 && !/^\s*(?:SPDX-License-Identifier:|Copyright\b)/i.test(line)) return line;
    if (line.startsWith('#')) return line.replace(/^#+\s*/, '').trim();
    break;
  }
  const basename = path.split('/').at(-1) ?? path;
  if (/^readme\./i.test(basename)) {
    const parent = path.split('/').at(-2);
    return parent ? `${parent} README` : '项目 README';
  }
  return basename.replace(/\.[^.]+$/, '').replaceAll('-', ' ');
}

export function documentCategory(path) {
  const name = path.split('/').at(-1) ?? path;
  if (LICENSE_TEXT_NAME.test(name) || /(?:^|\/)licen[sc]es?\//i.test(path) || /^(?:AUTHORS|ACKNOWLEDGEMENTS?)(?:\.|$)/i.test(name)) return '许可证与声明';
  if (/^AGENTS\.md$/i.test(name)) return '协作约定';
  if (/^CHANGELOG\.txt$/i.test(name)) return '历史记录';
  if (path === INDEX_PATH) return '文档总表';
  if (path.startsWith('docs/adaptation/')) return '适配指南';
  if (path.startsWith('docs/history/')) return '历史记录';
  if (path.startsWith('docs/migration/')) return '迁移记录';
  if (path.startsWith('docs/architecture/')) return '架构说明';
  if (path.startsWith('ports/') && name.toUpperCase() === 'ADAPTATION.MD') return '作品适配表';
  if (/^ports\/[^/]+\/README(?:\.[^.]+)?\.md$/i.test(path) || path === 'ports/README.md') return '作品入口';
  if (path.startsWith('ports/')) return '作品源码文档';
  if (path.startsWith('common/')) return '公共库';
  if (path.startsWith('launcher/')) return 'Launcher';
  if (path.startsWith('services/relay/')) return 'Relay 服务';
  if (path.startsWith('services/turn/')) return 'TURN 服务';
  if (path.startsWith('services/telemetry/')) return 'Telemetry 服务';
  if (path.startsWith('services/')) return '服务';
  if (path.startsWith('tools/')) return '开发工具';
  if (path.startsWith('.github/')) return 'CI 与协作模板';
  if (['README.md', 'README.en.md', 'CONTRIBUTING.md', 'WORKSPACE.md'].includes(path)) return '项目入口';
  if (path.startsWith('docs/')) return '项目文档';
  return '其他说明';
}

export function renderDocumentIndex(documents, { root = PROJECT_ROOT } = {}) {
  const categoryRank = new Map(CATEGORY_ORDER.map((category, index) => [category, index]));
  const compareText = (left, right) => left < right ? -1 : left > right ? 1 : 0;
  const sorted = [...documents].sort((a, b) =>
    (categoryRank.get(a.category) ?? CATEGORY_ORDER.length) - (categoryRank.get(b.category) ?? CATEGORY_ORDER.length) || compareText(a.path, b.path));
  const lines = [
    '# 文档总表',
    '',
    `共 ${sorted.length} 份可读文档，包含贡献入口、适配说明、服务文档、生成报告、许可证和历史记录。由 \`npm run docs:index\` 更新。`,
    '',
    '项目说明按当前目录重写；许可证保留原文，生成报告由各自工具更新。原有 498 份 Markdown 的逐篇去向见[旧文档去向](history/INDEX.md)。',
    '',
    '| 类别 | 路径 | 标题 |',
    '| --- | --- | --- |',
  ];
  for (const document of sorted) {
    const destination = slash(relative(resolve(root, 'docs'), resolve(root, document.path))) || 'INDEX.md';
    const title = document.title.replaceAll('|', '｜').replace(/\s+/g, ' ').trim();
    lines.push(`| ${document.category} | [${document.path}](${destination}) | ${title} |`);
  }
  return `${lines.join('\n')}\n`;
}

export function validateDocumentIndex(content, expectedPaths) {
  const issues = [];
  const listed = [];
  let foundHeader = false;
  for (const line of content.split(/\r?\n/)) {
    const cells = cellsFromTableLine(line);
    if (!cells || isTableSeparator(line)) continue;
    if (cells[0] === '类别' && cells[1] === '路径' && cells[2] === '标题') { foundHeader = true; continue; }
    const match = cells[1]?.match(/^\[([^\]]+)\]\([^)]+\)$/);
    if (!match) {
      issues.push(`docs/INDEX.md 文档路径单元格格式无效：${cells[1] ?? ''}`);
      continue;
    }
    if (cells.length !== 3) issues.push(`docs/INDEX.md ${match[1]} 表格列数应为 3，实际为 ${cells.length}。`);
    const path = match[1];
    if (!cells[0] || !cells[2]) issues.push(`docs/INDEX.md ${path} 缺少类别或标题。`);
    listed.push(path);
  }
  if (!foundHeader) issues.push('docs/INDEX.md 缺少“类别 | 路径 | 标题”表头。');
  const counts = new Map();
  for (const path of listed) counts.set(path, (counts.get(path) ?? 0) + 1);
  for (const path of expectedPaths) {
    if (!counts.has(path)) issues.push(`docs/INDEX.md 漏列文档：${path}`);
  }
  for (const path of counts.keys()) {
    if (!expectedPaths.has(path)) issues.push(`docs/INDEX.md 列出不存在或非文档文件：${path}`);
    if (counts.get(path) > 1) issues.push(`docs/INDEX.md 重复列出文档：${path}`);
  }
  return issues;
}

function repositoryState(root) {
  const listedResult = git(root, ['ls-files', '--cached', '--others', '--exclude-standard', '-z'], { encoding: 'buffer' });
  if (listedResult.status !== 0) throw new Error(listedResult.stderr.toString('utf8').trim() || '无法读取 Git 文档清单');
  const deletedResult = git(root, ['ls-files', '--deleted', '-z'], { encoding: 'buffer' });
  if (deletedResult.status !== 0) throw new Error(deletedResult.stderr.toString('utf8').trim() || '无法读取 Git 删除清单');
  const allPaths = new Set(nulSeparated(listedResult.stdout.toString('utf8')));
  const deleted = new Set(nulSeparated(deletedResult.stdout.toString('utf8')));
  for (const path of deleted) allPaths.delete(path);
  const sparseResult = git(root, ['config', '--bool', 'core.sparseCheckout']);
  const sparse = sparseResult.status === 0 && sparseResult.stdout.trim() === 'true';
  return { allPaths, sparse };
}

function readIndexedFile(root, path) {
  const result = git(root, ['show', `:${path}`], { encoding: 'buffer', maxBuffer: 32 * 1024 * 1024 });
  if (result.status !== 0) return null;
  return result.stdout.toString('utf8');
}

function readRepositoryFile(root, path, knownPaths) {
  const absolute = resolve(root, path);
  if (existsSync(absolute) && statSync(absolute).isFile()) return readFileSync(absolute, 'utf8');
  if (knownPaths.has(path)) return readIndexedFile(root, path);
  return null;
}

function collectDocuments(root, allPaths, { includeIndex = true } = {}) {
  const paths = new Set([...allPaths].filter(isDocumentationPath));
  if (includeIndex) paths.add(INDEX_PATH);
  const documents = [];
  for (const path of paths) {
    const content = readRepositoryFile(root, path, allPaths);
    if (content === null) {
      if (path === INDEX_PATH && !allPaths.has(path)) continue;
      documents.push({ path, content, title: documentTitle(path, ''), category: documentCategory(path), unreadable: true });
      continue;
    }
    if (isNumericalDump(path, content)) continue;
    documents.push({ path, content, title: documentTitle(path, content), category: documentCategory(path) });
  }
  return documents;
}

function isNumericalDump(path, content) {
  if (extname(path).toLowerCase() !== '.txt') return false;
  const lines = content.split(/\r?\n/).map(line => line.trim()).filter(Boolean);
  if (lines.length < 8) return false;
  const numeric = lines.filter(line => /^[\d\s,.;:+\-eExXa-fA-F%()]+$/.test(line)).length;
  return numeric / lines.length >= 0.9;
}

function checklistData(root) {
  const checklist = JSON.parse(readFileSync(resolve(root, CHECKLIST_PATH), 'utf8'));
  const source = readFileSync(resolve(root, CAPABILITIES_PATH), 'utf8');
  return { checklist, requiredContracts: extractRequiredBehaviorContracts(source) };
}

function adaptationChecks(root, allPaths, checklist) {
  const issues = [];
  const ports = portDefinitions();
  const report = readStatusReport(root);
  const expectedTargets = ports.flatMap(port=>port.variants.map(variant=>({game:port.game,variant})));
  issues.push(...validateStatusReport(report,{expectedTargets,checkIds:new Set(checklist.checks.map(check=>check.id))}));
  for (const port of ports) {
    const evidence = Object.fromEntries(port.variants.map(variant=>{
      const record=report?.targets?.find(target=>target.game===port.game && target.variant===variant);
      return [variant,checkStates({port,variant,checks:checklist.checks,gates:record?.gates ?? [],notApplicableFor:notApplicable})];
    }));
    for (const filename of ['README.md', 'ADAPTATION.md']) {
      const path = `${port.directory}/${filename}`;
      const content = readRepositoryFile(root, path, allPaths);
      if (content === null) {
        issues.push(`${path} 不在工作区或 Git index 中。`);
        continue;
      }
      if (filename === 'README.md') {
        const hasVisibleAdaptationLink = extractDocumentLinks(content, '.md').some(link => {
          if (link.line > 15) return false;
          if (!/(?:适配|adaptation)/i.test(link.label)) return false;
          const parts = targetParts(link.target);
          if (parts.invalidEncoding || !parts.path) return false;
          const resolved = posix.normalize(posix.join(posix.dirname(path), parts.path));
          return resolved === `${port.directory}/ADAPTATION.md`;
        });
        if (!hasVisibleAdaptationLink) issues.push(`${path} 顶部缺少指向 ADAPTATION.md 的明显适配链接。`);
      } else {
        issues.push(...validateAdaptationMarkdown(content, { port, checklist, evidence }));
      }
    }
  }
  return issues;
}

function addDocumentLinkChecks(root, allPaths, documents) {
  const issues = [];
  const readIndexed = path => readIndexedFile(root, path);
  for (const document of documents) {
    if (document.content === null) {
      issues.push(`${document.path} 已登记但工作区与 Git index 均无法读取。`);
      continue;
    }
    issues.push(...checkDocumentLinks({
      docPath: document.path,
      content: document.content,
      root,
      knownPaths: allPaths,
      readIndexedFile: readIndexed,
    }));
  }
  return issues;
}

export function assertIndexWriteAllowed(documents) {
  const unreadable = documents.filter(document => document.unreadable || document.content === null);
  if (unreadable.length) throw new Error(`生成总表前需要读取这些文档：${unreadable.map(document => document.path).join('、')}。请检查工作区或 Git 索引中的文件。`);
}

function writeIndex(root, documents) {
  assertIndexWriteAllowed(documents);
  const path = resolve(root, INDEX_PATH);
  const rows = documents.map(({ path: documentPath, title, category }) => ({ path: documentPath, title, category }));
  if (!rows.some(document => document.path === INDEX_PATH)) rows.push({ path: INDEX_PATH, title: '文档总表', category: '文档总表' });
  writeFileSync(path, renderDocumentIndex(rows, { root }), 'utf8');
}

export function runChecks({ root = PROJECT_ROOT, writeIndex: shouldWriteIndex = false } = {}) {
  const issues = [];
  const state = repositoryState(root);
  const { checklist, requiredContracts } = checklistData(root);
  issues.push(...validateChecklistContractMap(checklist, requiredContracts));

  if (shouldWriteIndex) {
    const documents = collectDocuments(root, state.allPaths);
    writeIndex(root, documents);
  }

  issues.push(...adaptationChecks(root, state.allPaths, checklist));
  const documents = collectDocuments(root, state.allPaths);
  issues.push(...addDocumentLinkChecks(root, state.allPaths, documents));
  const indexContent = readRepositoryFile(root, INDEX_PATH, state.allPaths);
  if (indexContent === null) {
    issues.push('缺少 docs/INDEX.md；运行 node tools/docs/check.mjs --write-index 生成。');
  } else {
    issues.push(...validateDocumentIndex(indexContent, new Set(documents.map(document => document.path))));
  }
  return { issues, documentCount: documents.length, sparse: state.sparse, portCount: portDefinitions().length };
}

function usage() {
  return [
    'node tools/docs/check.mjs',
    'node tools/docs/check.mjs --write-index',
    '',
    '统一检查每个 Runtime 构建目录的 README / ADAPTATION 表、能力契约覆盖、全仓文档链接与 docs/INDEX.md。',
  ].join('\n');
}

function main(args) {
  if (args.includes('--help') || args.includes('-h')) { console.log(usage()); return; }
  if (args.some(arg => arg !== '--write-index') || args.filter(arg => arg === '--write-index').length > 1) {
    throw new Error(`参数无效。\n${usage()}`);
  }
  const result = runChecks({ writeIndex: args.includes('--write-index') });
  if (result.issues.length) {
    for (const issue of result.issues) console.error(`✗ ${issue}`);
    console.error(`文档检查失败：${result.issues.length} 项；扫描 ${result.documentCount} 份文档、${result.portCount} 个源码目录。`);
    process.exitCode = 1;
  } else {
    console.log(`文档检查通过：扫描 ${result.documentCount} 份文档、${result.portCount} 个源码目录。`);
  }
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  try { main(process.argv.slice(2)); }
  catch (error) { console.error(error.message); process.exitCode = 1; }
}
