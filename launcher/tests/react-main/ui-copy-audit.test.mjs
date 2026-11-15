import {test} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {auditUiCopy} from '../../scripts/audit-ui-copy.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
test('copy audit catches dynamically capitalized game labels and permits HTTP methods and status watermarks',async()=>{
  const work=await mkdtemp(resolve(project,'.cache/copy-audit-'));
  try {
    await writeFile(resolve(work,'dynamic.ts'),'const labels=[game.toUpperCase(),plan.game.toUpperCase(),context.product.toUpperCase(),selected.gameId.toUpperCase(),gameIdForProduct(context.product).toUpperCase(),String(gameId || "game").toUpperCase()]; const protocol=method.toUpperCase(); const watermark=status.toUpperCase();');
    const findings=auditUiCopy(work);
    assert.equal(findings.length,6);
    assert.ok(findings.every(item=>item.kind==='game-id-case'));
  } finally {await rm(work,{recursive:true,force:true});}
});
test('copy audit sees rendered conditional text, accessible names and interpolated Chinese without executing source', async () => {
  const work = await mkdtemp(resolve(project, '.cache/copy-audit-'));
  try {
    await writeFile(resolve(work, 'controls.tsx'), 'throw new Error("Must not execute"); export const control = <button title={ready ? "Ready" : "等待"} aria-label="按钮">{ready ? "Start" : "开始"}{`第${step}步`}</button>;');
    const findings = auditUiCopy(work), texts = findings.map(item => item.text);
    for (const text of ['Ready', '等待', '按钮', 'Start', '开始', '第', '步']) assert.ok(texts.includes(text), text);
    assert.equal(texts.filter(text => text === '按钮').length, 1);
    assert.equal(texts.includes('Must not execute'), false);
    for (const finding of findings) {assert.ok(finding.line > 0); assert.equal(finding.file.includes('\\'), false);}
  } finally {await rm(work, {recursive: true, force: true});}
});
test('copy audit accepts message calls and brand names while keeping exceptions scoped to their actual files', async () => {
  const work = await mkdtemp(resolve(project, '.cache/copy-audit-'));
  try {
    await writeFile(resolve(work, 'probe.tsx'), '// 注释不属于文案\nconst pattern = /正则/; export const view = <><span>{t("action.close")}</span><span>GitHub</span><span>東方</span></>;');
    assert.deepEqual(auditUiCopy(work).map(item => item.text), ['東方']);
  } finally {await rm(work, {recursive: true, force: true});}
});

test('copy audit distinguishes rendered copy from CSS and non-text attribute identifiers', async () => {
  const work = await mkdtemp(resolve(project, '.cache/copy-audit-'));
  try {
    await writeFile(resolve(work, 'attributes.tsx'), 'export const view = <div className={active?"surface-one":"surface-two"} data-owner={ready?"capture-owner":"navigation-owner"} title={ready?"Visible title":"可见标题"}>{ready?"Visible body":"可见正文"}</div>;');
    assert.deepEqual(auditUiCopy(work).map(item => item.text), ['Visible title','可见标题','Visible body','可见正文']);
  } finally {await rm(work, {recursive: true, force: true});}
});

test('copy audit separates startup code and registered error templates from text rendered directly', async () => {
  const work = await mkdtemp(resolve(project, '.cache/copy-audit-'));
  try {
    await writeFile(resolve(work, 'startup.tsx'), 'const error = new Error(`${label} 加载超时，请重试`); export const view = <><script>{"window.boot()"}</script><style>{"html{color:red}"}</style><p>{`${label} 加载超时，请重试`}</p><p>Unknown visible text</p></>;');
    assert.deepEqual(auditUiCopy(work).map(item => item.text), ['加载超时，请重试','Unknown visible text']);
  } finally {await rm(work, {recursive: true, force: true});}
});
