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

export function harnessPaths(workspace, argv) {
  const args = ['th13-harness', ...argv];
  const mode = args[1];
  const outputs = [];
  const input = (index, fallback) => {
    const value = args[index] || fallback;
    if (!value) throw Error(`${mode}: missing input at argument ${index}`);
    args[index] = guestPath(workspace, value);
  };
  const output = (index, fallback) => {
    args[index] = guestPath(workspace, args[index] || fallback);
    const host = resolve(workspace, '.' + args[index]);
    const inside = relative(resolve(workspace), host);
    if (!inside || inside === '..' || inside.startsWith('..' + sep))
      throw Error(`${mode}: output must be inside the workspace: ${args[index]}`);
    outputs.push({guest: args[index], host});
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
  return {args, outputs};
}

export function validateHarnessOutput(output, before, after) {
  if (!after || after.size === 0n)
    throw Error(`harness did not produce a nonempty output: ${output.host}`);
  if (before && before.ino === after.ino && before.size === after.size &&
      before.mtimeNs === after.mtimeNs && before.ctimeNs === after.ctimeNs)
    throw Error(`harness output was not regenerated: ${output.host}`);
}
