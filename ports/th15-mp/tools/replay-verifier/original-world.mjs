// Original-provider factory extracted from the existing TH15 native world oracle.
// Executes retail gameplay only. Candidate simulation and comparison are separate.
import { readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { oracle, root, enableNativeMath, enableOriginalVectorMath } from '../../th15_web/tests/cpp/helpers.mjs';
import { recycleOriginalAllocations } from './arena.mjs';
import { sceneFlags, stageCleared } from './original-progress.mjs';
import { resourceLoader, nativePool, prepareBank } from '../../th15_web/tests/cpp/anm-oracle.mjs';
export async function createOriginalWorld(spec, externalPath) {
    const replayName = externalPath.split(/[\\/]/).pop(), stagePrefix = 'st0' + spec.stage, playerPrefix = 'pl0' + spec.character;
    const m = await oracle({ memoryEnd: 0x18000000, heapStart: 0x08000000, heapLimit: 0x17000000 }), arg = i => m.u32(m.reg('ESP') + 4 + i * 4), files = new Map(), banks = new Map(), reads = [], loads = [];
    recycleOriginalAllocations(m);
    enableNativeMath(m);
    m.replace(0x4869c0, 'verified offline dialogue glyph raster/upload boundary', () => 0);
    m.replace(0x486c00, 'verified offline spell-title glyph raster/upload boundary', () => 0, 1);
    if (process.env.TH15_WORLD_FPCW)
        m.reg("FPCW", Number(process.env.TH15_WORLD_FPCW));
    const vectorLibrary = enableOriginalVectorMath(m);
    for (const entry of m.image.imports)
        if (entry.dll === 'd3dx9_43.dll' && vectorLibrary.exports.has(entry.name))
            m.u32(entry.address, vectorLibrary.exports.get(entry.name));
    const file = name => { name = name.split(/[\\/]/).pop(); if (!files.has(name))
        files.set(name, readFileSync(name === replayName ? externalPath : resolve(root, 'reference/assets', name))); return files.get(name); };
    const alloc = n => { const p = m.allocate(n); m.view(p, n).fill(0); return p; };
    let result = {}, traceFrame = -1, rngTrace = [], cosmeticReads = [];
    if (process.env.TH15_ORACLE_COUNT_INSTRUCTIONS === '0') {
        const emu = m.cpu.emu_start.bind(m.cpu);
        m.cpu.emu_start = (a, b, t) => emu(a, b, t, 0);
    }
    if (process.env.TH15_WORLD_TRACE_RNG)
        for (const address of [0x403630, 0x4036a0])
            m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE, () => { if (process.env.TH15_WORLD_TRACE_RNG && traceFrame >= 7208 && m.reg("ECX") === 0x4e9a48)
                rngTrace.push({ frame: traceFrame, address: address.toString(16), callback: result.lastCallback?.function, returnAddress: m.u32(m.reg("ESP")).toString(16), stack: m.readWords(m.reg("ESP"), 8).map(v => v.toString(16)), seed: m.u32(0x4e9a48), calls: m.u32(0x4e9a4c) }); }, null, address, address));
    if (process.env.TH15_WORLD_ENEMIES) {
        m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE, () => { if (traceFrame < 1370 || traceFrame > 1385)
            return; const sp = m.reg('ESP'), manager = m.reg('ECX'); result.spawnTrace ??= []; result.spawnTrace.push({ frame: traceFrame, id: m.u32(manager + 0x90), name: m.string(m.u32(sp + 4), 256), request: Buffer.from(m.bytes(m.u32(sp + 8), 84)).toString('hex'), caller: m.u32(sp).toString(16) }); }, null, 0x426050, 0x426050));
        for (const address of [0x428830, 0x428930])
            m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE, () => { if (traceFrame < 1370 || traceFrame > 1385)
                return; const owner = m.reg('ECX') - (address === 0x428930 ? 0x120c : 0), id = m.u32(owner + 0x5740); if (id < 49)
                return; result.enemyUpdateTrace ??= []; result.enemyUpdateTrace.push({ frame: traceFrame, address: address.toString(16), id, flags: m.u32(owner + 0x526c), context: Buffer.from(m.bytes(owner + 0x10, 12)).toString('hex'), motion: Buffer.from(m.bytes(owner + 0x1250, 68)).toString('hex') }); }, null, address, address));
    }
    const nm = nativePool(m), loader = resourceLoader(m), textManager = alloc(0x19270), gfx = null;
    if (gfx) {
        const vt = m.u32(m.u32(0x4e77d8));
        m.u32(vt + 0xac, m.registerImport({ dll: "oracle GPU", name: "original Clear output", argc: 7, handler: () => 0 }));
    }
    m.f32(0x51bc04, 1);
    for (const [at, value] of [[0x51bc08, 128], [0x51bc0c, 16], [0x51bc18, 320], [0x51bc1c, 16], [0x51bc20, 320], [0x51bc24, 16]])
        m.i32(at, value);
    m.u32(0x4e9a58, textManager);
    m.u32(0x4ca620, 0x4e73e8);
    m.f32(0x4e73e8, 1);
    for (const [id, name] of [[0, 'text.anm'], [2, 'ascii.anm'], [5, 'front.anm'], [7, 'bullet.anm'], [8, 'effect.anm'], [9, playerPrefix + '.anm'], [spec.stage % 2 === 0 ? 3 : 4, stagePrefix + 'wl.anm'], [10, 'enemy.anm'], [11, stagePrefix + 'enm.anm'], ...(spec.stage === 7 ? [[12, 'st07enm2.anm'], [13, 'st07enm3.anm']] : []), [6, stagePrefix + 'logo.anm']]) {
        try {
            const bytes = file(name);
            banks.set(id, { name, bank: prepareBank(m, loader, name, bytes, nm, id) });
            if (id === 2)
                m.u32(textManager + 0x19258, banks.get(id).bank);
            if (id === 0)
                m.u32(0x4e7f00, banks.get(id).bank);
        }
        catch (e) {
            console.log('Preload omitted', id, name, String(e));
        }
    }
    m.i32(nm + 0x20, -1);
    m.u32(0x4e797c, 1);
    const tex = m.u32(m.u32(banks.get(0).bank + 0x124)), textureTable = m.u32(tex);
    m.u32(textureTable + 0x48, m.registerImport({ dll: "oracle GPU", name: "capture texture surface boundary", argc: 3, handler: () => 1 }));
    m.replace(0x490f2b, 'original working-directory boundary', () => 0);
    m.replace(0x402db0, 'original resource file boundary', () => { const name = m.string(m.reg('ECX'), 256), b = file(name), p = alloc(b.length); m.write(p, b); if (m.reg('EDX'))
        m.u32(m.reg('EDX'), b.length); reads.push(name); return p; }, 1);
    m.replace(0x485f70, 'prepared native ANM bank boundary', () => { const id = arg(0), name = m.string(arg(1), 256), b = banks.get(id); loads.push({ id, name }); if (!b || b.name !== name.split(/[\\/]/).pop())
        throw Error('Unprepared original ANM ' + id + ' ' + name); return b.bank; }, 2);
    m.replace(0x403490, 'original resource diagnostic', () => { throw Error('Original diagnostic ' + m.string(arg(1), 256)); });
    m.replace(0x403560, 'original text diagnostic', () => { throw Error('Original diagnostic ' + m.string(arg(0), 256)); });
    m.replace(0x490cf2, 'path format', () => { const dst = arg(0), format = m.string(arg(1), 256); let output = format; if (format.includes('%s'))
        output = format.replace('%s', m.string(arg(2), 256));
    else if (format.includes('%d'))
        output = format.replace('%d', String(arg(2))); m.write(dst, Buffer.from(output + '\0')); return output.length; });
    const scheduler = alloc(0x58), scene = alloc(0x100), graphics = alloc(0x2100), records = alloc(0x60000), overlay = alloc(0x608);
    m.u32(0x4e9a54, scheduler);
    m.call(0x401310, { ecx: scheduler });
    m.u32(0x4e9a94, scene);
    m.u32(0x4e9a88, graphics);
    m.u32(0x4e9bc8, records);
    m.u32(0x4e798c, overlay);
    m.call(0x40db10, { ecx: overlay });
    for (const at of [0x4e7990, 0x4e7994, 0x4e7998]) {
        const vm = alloc(0x608);
        m.call(0x40db10, { ecx: vm });
        m.u32(at, vm);
    }
    m.u32(0x4e7ed8, 1);
    m.u32(0x4e7794, 0x40);
    m.i32(scene + 0xb4, 1);
    m.i32(0x4e73f0, spec.stage);
    m.i32(0x4e73f4, spec.stage);
    m.i32(0x4e7404, spec.character);
    m.i32(0x4e7410, spec.difficulty);
    m.i32(0x4e7424, -1);
    m.call(0x44f6c0);
    const wave = alloc(0x1c);
    m.u32(0x4e9bd0, 0);
    m.u32(0x4e77d0 + 0x10, 0);
    m.u32(0x4e73e8, 0x3f800000);
    m.u32(0x4ca620, 0x4e73e8);
    m.u32(0x51e0a4, 0);
    for (const e of m.image.imports)
        if (e.dll === 'kernel32.dll' && e.name === 'Sleep') {
            const s = m.importMap.get(m.u32(e.address));
            s.handler = () => 0;
            s.argc = 1;
        }
    for (const [at, argc] of [[0x44db40, 0], [0x44de70, 0], [0x44d500, 0], [0x44d360, 2], [0x476360, 2]])
        m.replace(at, 'world presentation boundary ' + at.toString(16), () => 0, argc);
    const sound = alloc(4), soundTable = alloc(0x60);
    m.u32(sound, soundTable);
    m.u32(0x51d944, sound);
    m.u32(soundTable + 0x24, m.registerImport({ dll: "oracle audio", name: "original sound status output", argc: 2, handler: () => { m.u32(arg(1), 0); return 0; } }));
    m.u32(soundTable + 0x48, m.registerImport({ dll: "oracle audio", name: "original sound stop output", argc: 1, handler: () => 0 }));
    m.u32(soundTable + 0x40, m.registerImport({ dll: "oracle audio", name: "original laser pan output", argc: 2, handler: () => 0 }));
    const path = alloc(128);
    m.write(path, Buffer.from(replayName + '\0'));
    m.write(0x4e9aa0, Buffer.from(replayName + '\0'));
    if (!m.call(0x4227b0))
        throw Error('Native effect manager construction failed');
    const value = m.call(0x43bff0, { limit: 100000000 });
    for (const [fn, priority] of [[0x487b40, 9], [0x487b10, 34]]) {
        const cb = m.call(0x4017e0, { args: [fn] });
        m.u32(cb + 4, m.u32(cb + 4) | 2);
        m.u32(cb + 36, nm);
        m.call(0x401390, { args: [cb, priority] });
    }
    if (process.env.TH15_WORLD_TRACE_VISUAL) {
        for (const address of [0x403630, 0x4036a0])
            m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE, () => { if (traceFrame >= 5815 && traceFrame <= 5817 && m.reg("ECX") === 0x4e9a40)
                cosmeticReads.push({ frame: traceFrame, address: address.toString(16), callback: result.lastCallback?.function, returnAddress: m.u32(m.reg("ESP")).toString(16), seed: m.u32(0x4e9a40), calls: m.u32(0x4e9a44) }); }, null, address, address));
    }
    const callbacks = [];
    for (let node = m.u32(scheduler + 0x18); node; node = m.u32(node + 4)) {
        const cb = m.u32(node);
        callbacks.push({ address: cb, priority: m.i32(cb), enabled: m.u32(cb + 4), function: m.u32(cb + 8).toString(16), owner: m.u32(cb + 36).toString(16) });
    }
    result = { callbacks };
    if (gfx) {
        for (let node = m.u32(scheduler + 0x40); node; node = m.u32(node + 4)) {
            const cb = m.u32(node), fn = m.u32(cb + 8);
            m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE, () => { result.lastDraw = { frame: traceFrame, priority: m.i32(cb), fn: fn.toString(16), owner: m.reg("ECX").toString(16) }; }, null, fn, fn));
        }
    }
    if (process.env.TH15_WORLD_TRACE_CALLBACKS)
        for (const cb of callbacks) {
            const fn = parseInt(cb.function, 16);
            m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE, () => { result.lastCallback = { ...cb, ecx: m.reg('ECX'), stageFrame: m.i32(0x4e73fc) }; if (process.env.TH15_WORLD_TRACE_VISUAL && traceFrame >= 5815 && traceFrame <= 5817) {
                result.cosmeticCallbacks ??= [];
                result.cosmeticCallbacks.push({ frame: traceFrame, priority: cb.priority, function: cb.function, seed: m.u32(0x4e9a40), calls: m.u32(0x4e9a44) });
            } }, null, fn, fn));
        }
    if (process.env.TH15_ORACLE_NULL_VM_GUARD === '1')
        m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE, () => { if (m.reg('ECX') === 0)
            throw Error('Null native animation: ' + m.readWords(m.reg('ESP'), 48).map(v => v.toString(16)).join(',')); }, null, 0x477e10, 0x477e10));
    m.u32(0x4e7794, m.u32(0x4e7794) & ~0x40);
    return { m, tick(frame) { traceFrame = frame; const returned = m.call(0x4014f0, { ecx: scheduler, limit: 100000000 }); if (returned < 0)
            throw Error('Original calculation stopped before stage clear: ' + frame); },
        flags() { return sceneFlags(m, scene); }, completed() { return stageCleared(m, scene, spec.stage); },
        state() { const player = m.u32(0x4e9bb8); return [m.u32(0x4e73fc), m.u32(0x4e6f28), m.u32(player + 0x618), m.u32(player + 0x61c), m.u32(player + 0x16220), m.u32(0x4e740c), m.u32(0x4e742c), m.u32(0x4e7450), m.u32(0x4e7440), m.u32(0x4e745c), m.u32(0x4e9a48), m.u32(0x4e9a4c), m.u32(m.u32(0x4e9a80) + 0x18c), m.u32(m.u32(0x4e9a6c) + 0x40), m.u32(0x4e7418), m.u32(0x4e73f8)]; }, close() { m.close(); } };
}
