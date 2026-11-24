import {isAbsolute, relative, resolve, sep} from 'node:path';

const raw = '/artifacts/reference/raw';
const draws = '/artifacts/port-draws';

export function guestPath(workspace, value) {
  const root = resolve(workspace);
  const path = String(value);
  if (isAbsolute(path) && (path === root || path.startsWith(root + sep))) {
    return '/' + relative(root, path).split(sep).join('/');
  }
  return '/' + path.replace(/^\/+/, '');
}

export function harnessPaths(workspace, argv, environment = {}) {
  const args = ['th13-harness', ...argv];
  const mode = args[1];
  const outputs = [];
  const input = (index, fallback) => {
    const value = args[index] || fallback;
    if (!value) throw Error(`${mode}: missing input at argument ${index}`);
    args[index] = guestPath(workspace, value);
  };
  const declare = guest => {
    const host = resolve(workspace, '.' + guest);
    const inside = relative(resolve(workspace), host);
    if (!inside || inside === '..' || inside.startsWith('..' + sep))
      throw Error(`${mode}: output must be inside the workspace: ${guest}`);
    outputs.push({guest, host});
  };
  const output = (index, fallback) => {
    args[index] = guestPath(workspace, args[index] || fallback);
    declare(args[index]);
  };

  switch (mode) {
    case 'stage1':
      input(2, raw);
      output(3, '/eagler-touhou/ports/th13/web/artifacts/wasi/stage1-events.jsonl');
      break;
    case 'replay':
      input(2);
      input(3, raw);
      output(4, '/eagler-touhou/ports/th13/web/artifacts/wasi/replay-events.jsonl');
      break;
    case 'draws':
      input(2, raw);
      output(3, `${draws}/port-draws.jsonl`);
      break;
    case 'draws-replay':
      input(2);
      input(3, raw);
      output(4, `${draws}/port-draws-replay.jsonl`);
      break;
    case 'title':
    case 'title-b':
      input(2, raw);
      output(3, `${draws}/title-draws${mode === 'title-b' ? '-b' : ''}.jsonl`);
      break;
  }
  // TH13_DIGEST_LOG（guest 路径）：只有 replay/draws-replay 写回归 digest 日志。
  // 纳入声明输出 → 目录自动创建 + 复用「必须重新生成且非空」校验。
  const digest = environment.TH13_DIGEST_LOG;
  if (digest && (mode === 'replay' || mode === 'draws-replay'))
    declare(guestPath(workspace, digest));
  return {args, outputs};
}

export function validateHarnessOutput(output, before, after) {
  if (!after || after.size === 0n)
    throw Error(`harness did not produce a nonempty output: ${output.host}`);
  if (before && before.ino === after.ino && before.size === after.size &&
      before.mtimeNs === after.mtimeNs && before.ctimeNs === after.ctimeNs)
    throw Error(`harness output was not regenerated: ${output.host}`);
}
