import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {existsSync, mkdirSync, readFileSync, renameSync, writeFileSync} from 'node:fs';
import {join, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {parseArgs} from 'node:util';
import {decodeJunowen, encodeJunowen} from '../replay/junowen-format.mjs';

const {positionals, values} = parseArgs({allowPositionals: true, options: {
    cards: {type: 'boolean'}, case: {type: 'string'}, 'score-profile': {type: 'string'},
    'isolated-pipe': {type: 'boolean'},
}});
const [original, observer, driver, output, stopFrame = '180'] = positionals;
const cards = values.cards;
const first = cards ? 1 : 0, end = cards ? 39 : 19;
const selected = values.case === undefined ? null : Number(values.case);
if (!output || !Number.isSafeInteger(Number(stopFrame)) || Number(stopFrame) < 1 ||
    (selected !== null && (!Number.isInteger(selected) || selected < first || selected >= end))) {
    throw Error('Usage: node scan-native.mjs <original> <observer> <driver> <output> [stop frame] [--cards] [--case ID] [--score-profile FILE] [--isolated-pipe]');
}

const template = decodeJunowen(readFileSync(new URL('samples/cpu-lunatic-reimu-marisa-no-cards.bin', import.meta.url)));
const capture = fileURLToPath(new URL('capture-native.py', import.meta.url));
const directory = resolve(output);
mkdirSync(directory, {recursive: true});
function snapshot(path) {
    const bytes = readFileSync(path);
    const hash = createHash('sha256').update(bytes).digest('hex');
    const copy = join(directory, `${hash}.dll`);
    if (!existsSync(copy)) writeFileSync(copy, bytes, {flag: 'wx'});
    return {path: copy, hash};
}
const cpu = snapshot(driver), d3d = snapshot(observer);
const scoreProfile = values['score-profile'];
const scoreHash = scoreProfile ? createHash('sha256').update(readFileSync(scoreProfile)).digest('hex') : null;

const kind = cards ? 'card' : 'character';
for (let id = selected ?? first; id < (selected === null ? end : selected + 1); ++id) {
    const run = join(directory, `${kind}-${String(id).padStart(2, '0')}`);
    mkdirSync(run, {recursive: true});
    const resultPath = join(run, 'result.json');
    if (existsSync(resultPath)) {
        const prior = JSON.parse(readFileSync(resultPath, 'utf8'));
        if (prior.status === 0 && prior.driverHash === cpu.hash && prior.observerHash === d3d.hash &&
            prior.stopFrame >= Number(stopFrame) && (prior.scoreHash ?? null) === scoreHash &&
            prior.capture && existsSync(join(prior.capture, 'capture.json'))) continue;
    }

    const fixture = join(run, 'fixture.bin');
    const header = cards
        ? {...template.header, common: template.header.common | 0xc0, p1Card: id}
        : {...template.header, p1Character: id};
    writeFileSync(fixture, encodeJunowen({header, inputs: template.inputs}));
    const result = spawnSync(process.env.TH19_PYTHON || 'python', [capture,
        '--original', original, '--observer', d3d.path, '--output-dir', run,
        '--cpu-fixture', fixture, '--cpu-driver', cpu.path, '--cpu-driver-sha256', cpu.hash,
        '--cpu-second-rng', '424182232', '--stop-at-frame', stopFrame, '--timeout', '40',
        '--cpu-start-delay', process.env.TH19_CPU_START_DELAY || '0',
        ...(values['isolated-pipe'] ? ['--isolated-pipe'] : []),
        ...(scoreProfile ? ['--score-profile', scoreProfile] : []),
    ], {encoding: 'utf8'});
    const recorded = {
        kind, id, characters: [header.p1Character, header.p2Character], cards: [header.p1Card, header.p2Card],
        driverHash: cpu.hash, observerHash: d3d.hash, scoreHash, stopFrame: Number(stopFrame),
        status: result.status, capture: /^CAPTURE=(.+)$/m.exec(result.stdout || '')?.[1]?.trim(),
        error: result.error?.message || result.stderr?.trim() || undefined,
    };
    const pendingResult = `${resultPath}.${process.pid}.tmp`;
    writeFileSync(pendingResult, JSON.stringify(recorded, null, 2) + '\n');
    renameSync(pendingResult, resultPath);
    console.log(JSON.stringify(recorded));
    if (recorded.status !== 0) process.exitCode = 1;
}
