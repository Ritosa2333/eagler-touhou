// Diagnostic only: the caller owns a stopped loop and a confirmed native world.
// Draw/readback stays synchronous; SHA work starts after GL wrappers and the
// renderer option have been restored. Timings include instrumentation and are
// not rAF, 4x CPU, or physical-device performance evidence.
const ALPHAS = [0, .25, .5, .75, 1];
const MODES = [false, true, true, false, true];
const METHODS = ['drawArrays', 'drawElements', 'drawArraysInstanced',
    'drawElementsInstanced', 'bufferData'];
const MAX_EVENTS = 20000;

function requireValue(value, message) {
    if (!value) throw new Error(message);
}
function synchronous(value, label) {
    requireValue(!value || typeof value.then !== 'function', label + ' must be synchronous');
    return value;
}
function words(value, count, label) {
    requireValue(value && value.length === count, label + ' word count');
    const result = Array.from(value);
    requireValue(result.every(x => Number.isInteger(x) && x >= 0 && x <= 0xffffffff),
        label + ' must contain u32 values');
    return result;
}
function differences(before, after) {
    return before.flatMap((value, word) =>
        value === after[word] ? [] : [{word, before: value, after: after[word]}]);
}
function delta(before, after) {
    return after.map((value, i) => (value - before[i]) >>> 0);
}
export function pixelDifference(expected, actual, width) {
    let count = 0, first = null;
    for (let i = 0; i < expected.length; ++i) {
        if (expected[i] === actual[i]) continue;
        ++count;
        if (!first) first = {byte: i, x: Math.floor(i / 4) % width,
            yFromBottom: Math.floor(i / (width * 4)), channel: i % 4,
            expected: expected[i], actual: actual[i]};
    }
    return {equal: count === 0, differingBytes: count, first};
}
function uploadExtent(args) {
    const source = args[1];
    if (typeof source === 'number') return Number.isSafeInteger(source) && source >= 0
        ? {form: 'allocation', allocatedBytes: source, sourceBytes: 0}
        : {form: 'unknown', allocatedBytes: null, sourceBytes: null};
    if (source == null) return {form: 'null', allocatedBytes: 0, sourceBytes: 0};
    const view = ArrayBuffer.isView(source);
    const raw = source instanceof ArrayBuffer ||
        (typeof SharedArrayBuffer !== 'undefined' && source instanceof SharedArrayBuffer);
    if (!view && !raw) return {form: 'unknown', allocatedBytes: null, sourceBytes: null};
    const unit = view ? (source.BYTES_PER_ELEMENT || 1) : 1;
    const offset = args[3] === undefined ? 0 : args[3];
    const length = args[4] === undefined ? 0 : args[4];
    const available = source.byteLength - offset * unit;
    const bytes = length === 0 ? available : length * unit;
    if (!Number.isSafeInteger(offset) || !Number.isSafeInteger(length) ||
        offset < 0 || length < 0 || available < 0 || bytes > available)
        return {form: 'unknown', allocatedBytes: null, sourceBytes: null};
    return {form: view ? 'view' : 'buffer', allocatedBytes: bytes, sourceBytes: bytes,
        elementBytes: unit, sourceOffset: offset, sourceLength: length};
}
function installRecorder(gl) {
    let current = null, nextId = 1;
    const ids = new WeakMap(), sizes = new WeakMap(), saved = [], errors = [];
    const id = object => {
        if (object === null) return 0;
        if (!object || (typeof object !== 'object' && typeof object !== 'function')) return null;
        if (!ids.has(object)) ids.set(object, nextId++);
        return ids.get(object);
    };
    const targets = new Map([
        [gl.ARRAY_BUFFER, ['ARRAY_BUFFER', gl.ARRAY_BUFFER_BINDING]],
        [gl.ELEMENT_ARRAY_BUFFER, ['ELEMENT_ARRAY_BUFFER', gl.ELEMENT_ARRAY_BUFFER_BINDING]],
        [gl.COPY_READ_BUFFER, ['COPY_READ_BUFFER', gl.COPY_READ_BUFFER_BINDING]],
        [gl.COPY_WRITE_BUFFER, ['COPY_WRITE_BUFFER', gl.COPY_WRITE_BUFFER_BINDING]],
        [gl.PIXEL_PACK_BUFFER, ['PIXEL_PACK_BUFFER', gl.PIXEL_PACK_BUFFER_BINDING]],
        [gl.PIXEL_UNPACK_BUFFER, ['PIXEL_UNPACK_BUFFER', gl.PIXEL_UNPACK_BUFFER_BINDING]],
        [gl.TRANSFORM_FEEDBACK_BUFFER, ['TRANSFORM_FEEDBACK_BUFFER', gl.TRANSFORM_FEEDBACK_BUFFER_BINDING]],
        [gl.UNIFORM_BUFFER, ['UNIFORM_BUFFER', gl.UNIFORM_BUFFER_BINDING]],
    ]);
    function noteError(message) { if (errors.length < 32) errors.push(message); }
    function record(name, args, receiver) {
        if (!current) return;
        const totals = current.gl.totals;
        ++totals[name];
        if (receiver !== gl) { ++totals.unknownContextCalls; return; }
        if (name === 'bufferData') {
            const known = targets.get(args[0]), extent = uploadExtent(args);
            const buffer = known ? gl.getParameter(known[1]) : null;
            const event = {target: known ? known[0] : 'unknown',
                targetValue: args[0], buffer: known ? id(buffer) : null,
                usage: args[2], ...extent};
            if (buffer && extent.allocatedBytes !== null) sizes.set(buffer, extent.allocatedBytes);
            if (extent.allocatedBytes === null) ++totals.unknownBufferSizes;
            else totals.allocatedBytes += extent.allocatedBytes;
            if (extent.sourceBytes !== null) totals.sourceBytes += extent.sourceBytes;
            if (current.gl.bufferData.length < MAX_EVENTS) current.gl.bufferData.push(event);
            else noteError('bufferData event bound exceeded');
            return;
        }
        const event = {method: name, args: Array.from(args),
            drawFramebuffer: id(gl.getParameter(gl.DRAW_FRAMEBUFFER_BINDING)),
            vao: id(gl.getParameter(gl.VERTEX_ARRAY_BINDING))};
        // Describe a matching draw shape, not an assumed game resource identity.
        // Unknown/unobserved buffer storage remains explicitly unknown.
        if ((name === 'drawArrays' || name === 'drawArraysInstanced') &&
            args[0] === gl.TRIANGLE_STRIP && args[1] === 0 && args[2] === 4) {
            const attribute = location => ({
                enabled: gl.getVertexAttrib(location, gl.VERTEX_ATTRIB_ARRAY_ENABLED),
                size: gl.getVertexAttrib(location, gl.VERTEX_ATTRIB_ARRAY_SIZE),
                type: gl.getVertexAttrib(location, gl.VERTEX_ATTRIB_ARRAY_TYPE),
                stride: gl.getVertexAttrib(location, gl.VERTEX_ATTRIB_ARRAY_STRIDE),
                offset: gl.getVertexAttribOffset(location, gl.VERTEX_ATTRIB_ARRAY_POINTER),
                buffer: gl.getVertexAttrib(location, gl.VERTEX_ATTRIB_ARRAY_BUFFER_BINDING),
            });
            const position = attribute(0), uv = attribute(2);
            const sameBuffer = position.buffer !== null && position.buffer === uv.buffer;
            const shape = !!(sameBuffer && position.enabled && uv.enabled &&
                position.size === 3 && uv.size === 2 &&
                position.type === gl.FLOAT && uv.type === gl.FLOAT &&
                position.stride === 20 && uv.stride === 20 &&
                position.offset === 0 && uv.offset === 12 &&
                !gl.getVertexAttrib(1, gl.VERTEX_ATTRIB_ARRAY_ENABLED) &&
                !gl.getVertexAttrib(3, gl.VERTEX_ATTRIB_ARRAY_ENABLED));
            event.worldUvStrip4 = {shape, sameBuffer,
                position: {...position, buffer: id(position.buffer)},
                uv: {...uv, buffer: id(uv.buffer)},
                observedBufferBytes: position.buffer && sizes.has(position.buffer)
                    ? sizes.get(position.buffer) : null};
            if (shape) ++totals[name === 'drawArrays' ? 'modelShapeNormal' : 'modelShapeInstanced'];
        }
        if (current.gl.draws.length < MAX_EVENTS) current.gl.draws.push(event);
        else noteError('draw event bound exceeded');
    }
    function restore() {
        let restored = true;
        for (const item of saved.splice(0).reverse()) {
            try {
                if (item.descriptor) Object.defineProperty(gl, item.name, item.descriptor);
                else requireValue(Reflect.deleteProperty(gl, item.name),
                    'Could not remove wrapper ' + item.name);
                requireValue(gl[item.name] === item.original, 'Method restoration mismatch ' + item.name);
            } catch (error) { restored = false;noteError(String(error.message || error)); }
        }
        return restored;
    }
    try {
        for (const name of METHODS) {
            const original = gl[name], descriptor = Object.getOwnPropertyDescriptor(gl, name);
            requireValue(typeof original === 'function', 'WebGL2 method missing: ' + name);
            const wrapper = function (...args) {
                // Always execute the original call before optional diagnostics.
                const result = Reflect.apply(original, this, args);
                try { record(name, args, this); }
                catch (error) { noteError(name + ': ' + String(error.message || error)); }
                return result;
            };
            Object.defineProperty(gl, name, {value: wrapper, configurable: true,
                writable: true, enumerable: descriptor ? descriptor.enumerable : false});
            saved.push({name, descriptor, original});
            requireValue(gl[name] === wrapper, 'Could not wrap ' + name);
        }
    } catch (error) { restore(); throw error; }
    return {setRow(row) { current = row; }, restore, errors};
}
function emptyGL() {
    return {draws: [], bufferData: [], totals: {
        drawArrays: 0, drawElements: 0, drawArraysInstanced: 0, drawElementsInstanced: 0,
        bufferData: 0, allocatedBytes: 0, sourceBytes: 0, unknownBufferSizes: 0,
        unknownContextCalls: 0, modelShapeNormal: 0, modelShapeInstanced: 0,
    }};
}
export function readDefault(gl, width, height) {
    const read = gl.getParameter(gl.READ_FRAMEBUFFER_BINDING);
    const pack = gl.getParameter(gl.PIXEL_PACK_BUFFER_BINDING);
    const stores = [gl.PACK_ALIGNMENT, gl.PACK_ROW_LENGTH, gl.PACK_SKIP_PIXELS, gl.PACK_SKIP_ROWS];
    const values = stores.map(key => gl.getParameter(key));
    let defaultReadBuffer, haveDefaultReadBuffer = false;
    try {
        gl.bindFramebuffer(gl.READ_FRAMEBUFFER, null);
        defaultReadBuffer = gl.getParameter(gl.READ_BUFFER);haveDefaultReadBuffer = true;
        gl.readBuffer(gl.BACK);gl.bindBuffer(gl.PIXEL_PACK_BUFFER, null);
        gl.pixelStorei(gl.PACK_ALIGNMENT, 1);
        for (const key of stores.slice(1)) gl.pixelStorei(key, 0);
        const pixels = new Uint8Array(width * height * 4);
        gl.readPixels(0, 0, width, height, gl.RGBA, gl.UNSIGNED_BYTE, pixels);
        return pixels;
    } finally {
        // READ_BUFFER belongs to the framebuffer currently bound for reading.
        if (haveDefaultReadBuffer) gl.readBuffer(defaultReadBuffer);
        gl.bindFramebuffer(gl.READ_FRAMEBUFFER, read);
        gl.bindBuffer(gl.PIXEL_PACK_BUFFER, pack);
        stores.forEach((key, i) => gl.pixelStorei(key, values[i]));
        requireValue(gl.getParameter(gl.READ_FRAMEBUFFER_BINDING) === read,
            'READ_FRAMEBUFFER_BINDING was not restored');
    }
}

export async function verifyModelQuad(api) {
    const report = {schema: 1, passed: false,
        scope: 'Stopped-loop diagnostic Draw/default-framebuffer equivalence',
        scopeLimits: 'Caller supplies a confirmed native world; no world/input mutation or tick. Instrumented Draw/readback timings are not rAF or CPU-throttle performance evidence.',
        alphas: ALPHAS, sequence: MODES.map(x => x ? 'on' : 'off'),
        failures: [], glErrors: [], samples: [], coverage: {}};
    let gl, recorder, initialCanonical, initialStats, nativeTime;
    const pendingHashes = [], hashes = new Map();
    const fail = (where, error) => report.failures.push({where, message: String(error.message || error)});
    const canonical = () => {
        const value = words(synchronous(api.canonical(), 'canonical'), 44, 'canonical');
        requireValue(value[0] === 2, 'Canonical schema 2 required');return value;
    };
    const stats = () => {
        const value = words(synchronous(api.modelQuadStats(), 'modelQuadStats'), 8, 'modelQuadStats');
        requireValue(value[0] === 1 && value[1] <= 1, 'Model quad stats schema/enabled');
        requireValue(value[7] === 0 || value[7] === 80, 'Resident buffer must be bounded to 80 bytes');
        return value;
    };
    const graphics = () => {
        const pointer = api.core.sdl_stats();
        return Array.from(new Uint32Array(api.core.memory.buffer, pointer, 17));
    };
    const checkGL = where => {
        const codes = [];
        for (let i = 0; i < 32; ++i) {
            const code = gl.getError();if (code === gl.NO_ERROR) break;codes.push(code);
        }
        if (codes.length) report.glErrors.push({where, codes});
        requireValue(!codes.length && !gl.isContextLost(), 'GL error/context loss at ' + where);
    };
    try {
        for (const name of ['canonical', 'presentationDraw', 'modelQuadResidency', 'modelQuadStats'])
            requireValue(typeof api?.[name] === 'function', 'API missing ' + name);
        requireValue(api.core?.mp_fixture_presentation_draw &&
            typeof api.core.sdl_stats === 'function', 'Diagnostic presentation fixture/core required');
        initialCanonical = canonical();initialStats = stats();
        report.canonicalBefore = initialCanonical;report.statsBefore = initialStats;
        nativeTime = typeof api.core.sdl_loop_time === 'function' ? api.core.sdl_loop_time() : null;
        const canvas = document.getElementById('screen');
        requireValue(canvas, 'Smoke canvas #screen required');
        gl = canvas.getContext('webgl2');requireValue(gl, 'Existing WebGL2 renderer required');
        checkGL('entry');
        const width = gl.drawingBufferWidth, height = gl.drawingBufferHeight;
        requireValue(width > 0 && height > 0 && width * height * 4 <= 16 * 1024 * 1024,
            'Bounded drawable framebuffer required');
        report.framebuffer = {width, height, format: 'RGBA8', origin: 'bottom-left'};
        recorder = installRecorder(gl);
        for (const alpha of ALPHAS) {
            let reference = null;
            for (let phase = 0; phase < MODES.length; ++phase) {
                const enabled = MODES[phase];
                const row = {alpha, phase, enabled, canonicalBefore: canonical(),
                    statsBefore: stats(), gl: emptyGL()};
                report.samples.push(row);recorder.setRow(row);
                requireValue(!differences(initialCanonical, row.canonicalBefore).length,
                    'Canonical changed before alpha ' + alpha + ' phase ' + phase);
                const graphicsBefore = graphics();
                synchronous(api.modelQuadResidency(enabled), 'modelQuadResidency');
                requireValue(stats()[1] === Number(enabled), 'Renderer option did not change as requested');
                checkGL('before Draw ' + alpha + '/' + phase);
                const started = performance.now();
                const drawn = synchronous(api.presentationDraw(alpha), 'presentationDraw');
                row.drawCallWallMs = performance.now() - started;
                // No await/canvas capture: preserve the just-presented default
                // framebuffer before the browser may clear its drawing buffer.
                const readStarted = performance.now(), pixels = readDefault(gl, width, height);
                row.readbackWallMs = performance.now() - readStarted;
                row.canonicalAfter = canonical();row.statsAfter = stats();
                row.statsDeltaWords = delta(row.statsBefore, row.statsAfter);
                const d = row.statsDeltaWords;
                row.counterDelta = {eligible: d[2], uploads: d[3], hits: d[4],
                    uploadedBytes: d[5], reusedBytes: d[6]};
                row.counterInvariant = d[2] === d[3] + d[4] &&
                    d[5] === d[3] * 80 && d[6] === d[4] * 80;
                row.canonicalDifferences = differences(row.canonicalBefore, row.canonicalAfter);
                row.graphicsDelta = delta(graphicsBefore, graphics());
                row.committedPresentations = row.graphicsDelta[5];
                row.pixelComparison = reference ? pixelDifference(reference, pixels, width)
                    : {equal: true, differingBytes: 0, first: null};
                if (!reference) reference = pixels;
                pendingHashes.push({row, pixels: row.pixelComparison.equal ? reference : pixels});
                checkGL('after readback ' + alpha + '/' + phase);
                if (!row.pixelComparison.equal) fail('pixels',
                    'Same-alpha bytes differ at ' + alpha + '/' + phase);
                requireValue(drawn === true || drawn === 1, 'Presentation fixture rejected Draw');
                requireValue(row.committedPresentations === 1,
                    'Expected one newly committed default framebuffer, not a deferred/old image');
                requireValue(row.counterInvariant, '80-byte counter invariant failed');
                requireValue(!row.canonicalDifferences.length, 'Draw changed full 44-word canonical');
                requireValue(!recorder.errors.length, 'GL recorder failure: ' + recorder.errors.join('; '));
                requireValue(gl.drawingBufferWidth === width && gl.drawingBufferHeight === height,
                    'Framebuffer resized during oracle');
                recorder.setRow(null);
            }
        }
    } catch (error) { fail('oracle', error); }
    finally {
        if (recorder) recorder.setRow(null);
        if (initialStats) {
            try {
                synchronous(api.modelQuadResidency(initialStats[1] !== 0), 'restore modelQuadResidency');
                report.statsAfterRestore = stats();
                report.optionRestored = report.statsAfterRestore[1] === initialStats[1];
                requireValue(report.optionRestored, 'Renderer option restore failed');
            } catch (error) { fail('option restore', error); }
        }
        if (recorder) {
            report.wrappersRestored = recorder.restore();
            report.wrapperErrors = recorder.errors.slice();
            if (recorder.errors.length) fail('GL wrappers', recorder.errors.join('; '));
        }
        if (gl) {
            try {
                checkGL('cleanup');
            } catch (error) { fail('GL cleanup', error); }
        }
        if (initialCanonical) {
            try {
                report.canonicalAfterRestore = canonical();
                requireValue(!differences(initialCanonical, report.canonicalAfterRestore).length,
                    'Full canonical changed across oracle/cleanup');
                report.nativeTimeBefore = nativeTime;
                report.nativeTimeAfter = nativeTime === null ? null : api.core.sdl_loop_time();
                requireValue(nativeTime === report.nativeTimeAfter, 'Native loop clock advanced');
            } catch (error) { fail('native cleanup', error); }
        }
    }
    // Pixel equality above is a byte comparison. Equal buffers may share the
    // same SHA calculation only after that full comparison has already passed.
    try {
        for (const item of pendingHashes) {
            if (!hashes.has(item.pixels)) hashes.set(item.pixels,
                crypto.subtle.digest('SHA-256', item.pixels).then(hash =>
                    Array.from(new Uint8Array(hash), x => x.toString(16).padStart(2, '0')).join('')));
        }
        await Promise.all(pendingHashes.map(async item => {
            item.row.pixelSha256 = await hashes.get(item.pixels);
        }));
        if (initialCanonical) {
            report.canonicalAtReturn = canonical();
            requireValue(!differences(initialCanonical, report.canonicalAtReturn).length,
                'Canonical changed while producing report');
            if (nativeTime !== null)
                requireValue(api.core.sdl_loop_time() === nativeTime, 'Native loop clock advanced during report');
        }
    } catch (error) { fail('report hashes/native stability', error); }
    const enabledRows = report.samples.filter(row => row.enabled && row.counterDelta);
    const total = key => enabledRows.reduce((sum, row) => sum + row.counterDelta[key], 0);
    const shapeTotal = (key, enabled) => report.samples.filter(row => row.enabled === enabled)
        .reduce((sum, row) => sum + row.gl.totals[key], 0);
    report.coverage = {enabledEligible: total('eligible'), enabledUploads: total('uploads'),
        enabledHits: total('hits'), enabledUploadedBytes: total('uploadedBytes'),
        enabledReusedBytes: total('reusedBytes'),
        modelShapeNormal: {off: shapeTotal('modelShapeNormal', false), on: shapeTotal('modelShapeNormal', true)},
        modelShapeInstanced: {off: shapeTotal('modelShapeInstanced', false), on: shapeTotal('modelShapeInstanced', true)},
        note: 'Draw shapes are observed GL attributes, not assumed resource identity; null buffer sizes were not observed. A zero count means that path was not covered.'};
    if (!report.coverage.enabledEligible || !report.coverage.enabledHits)
        fail('coverage', 'Enabled path requires actual eligible submissions and at least one cache hit');
    if (!enabledRows.some(row => row.gl.totals.drawArrays || row.gl.totals.drawElements ||
        row.gl.totals.drawArraysInstanced || row.gl.totals.drawElementsInstanced))
        fail('coverage', 'No actual GL draw call was captured while enabled');
    if (report.samples.length !== ALPHAS.length * MODES.length)
        fail('coverage', 'Full alpha/mode sequence did not complete');
    report.passed = report.failures.length === 0;
    return report;
}
