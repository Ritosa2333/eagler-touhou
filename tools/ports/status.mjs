import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';

export const STATUS_SCHEMA = 'eagler-touhou/port-status/1';
export const sourceChecks = new Set(['source-contract', 'source-inputs', 'module']);
export const verificationLevels = new Set(['build','package']);
export function retainedVerification(previous, sourceDigest) {
  return previous?.sourceDigest===sourceDigest ? previous.gates.filter(gate=>verificationLevels.has(gate.level)) : [];
}
export function checkStates({port, variant, checks, gates, notApplicableFor}) {
  const states = {};
  for (const check of checks) {
    const reason = notApplicableFor(check, port.game, variant);
    if (reason) {states[check.id] = {status:'不适用', note:`${variant}：${reason}`}; continue;}
    const relevant = gates.filter(gate => gate.checks.includes(check.id));
    const failed = relevant.filter(gate => gate.status === 'failed');
    const complete = relevant.filter(gate => gate.complete !== false);
    const status = failed.length ? '检查失败' :
      complete.length && complete.every(gate => gate.status === 'passed') ?
        check.id === 'BUILD-01' || complete.some(gate=>verificationLevels.has(gate.level)) ? '已验证' : '已实现' : '待核对';
    const names = relevant.map(gate => gate.id).join('、');
    states[check.id] = {status, note:names ?
      `${variant}：[${names}](../../docs/adaptation/status.json)` :
      `${variant}：按本项步骤执行，结果写入验证记录`};
  }
  return states;
}

export function validateStatusReport(report, {expectedTargets = [], allowedTargets = expectedTargets, checkIds} = {}) {
  const issues = [];
  if (report?.schema !== STATUS_SCHEMA || !Array.isArray(report.targets)) return ['适配检查报告格式无效'];
  const seen = new Set();
  const allowed = new Set(allowedTargets.map(target=>`${target.game}/${target.variant}`));
  for (const target of report.targets) {
    const key = `${target.game}/${target.variant}`;
    if (seen.has(key)) issues.push(`适配检查目标重复：${key}`);
    seen.add(key);
    if (allowed.size && !allowed.has(key)) issues.push(`适配检查目标未登记：${key}`);
    const unavailable = target.sourceDigest === null && target.gates?.some(gate=>gate.id==='source-inputs' && gate.status==='failed');
    if (!unavailable && !/^[a-f0-9]{64}$/.test(target.sourceDigest ?? '')) issues.push(`${key} 缺少源码身份`);
    if (!Array.isArray(target.gates) || !target.gates.length) {issues.push(`${key} 缺少检查结果`); continue;}
    const gateIds = new Set();
    for (const gate of target.gates) {
      if (!gate.id || gateIds.has(gate.id)) issues.push(`${key} 检查编号缺失或重复：${gate.id}`);
      gateIds.add(gate.id);
      if (!['passed','failed'].includes(gate.status)) issues.push(`${key}/${gate.id} 检查状态无效`);
      if (!Array.isArray(gate.checks) || !gate.checks.length) issues.push(`${key}/${gate.id} 没有适配编号`);
      else if (checkIds && gate.checks.some(id=>!checkIds.has(id))) issues.push(`${key}/${gate.id} 引用未登记适配编号`);
      if (!sourceChecks.has(gate.level) && !verificationLevels.has(gate.level)) issues.push(`${key}/${gate.id} 缺少检查范围`);
      if (verificationLevels.has(gate.level) && ((gate.status==='passed' && !/^[a-f0-9]{64}$/.test(gate.evidence?.sha256 ?? '')) ||
          !Array.isArray(gate.command) || !gate.command.length || !gate.environment)) issues.push(`${key}/${gate.id} 缺少执行命令、环境或产物身份`);
      if (gate.status === 'failed' && !gate.error) issues.push(`${key}/${gate.id} 缺少失败依据`);
    }
  }
  for (const target of expectedTargets) if (!seen.has(`${target.game}/${target.variant}`)) issues.push(`适配检查漏掉 ${target.game}/${target.variant}`);
  return issues;
}

export function readStatusReport(root) {
  try {return JSON.parse(readFileSync(resolve(root, 'docs/adaptation/status.json'), 'utf8'));}
  catch (error) {if (error.code === 'ENOENT') return null; throw error;}
}
