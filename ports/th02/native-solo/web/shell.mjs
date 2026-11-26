import createModule from './soew-web.mjs';
import { RetraceClock } from './clock.mjs';
import { Keyboard } from './input.mjs';

async function fetchContent(url) {
    const response = await fetch(url);
    if (!response.ok) throw Error(`${response.status} loading ${url}`);
    return response;
}

export async function createGame(canvas, content = new URL('./content/', import.meta.url)) {
    const wasm = await createModule();
    const names = await (await fetchContent(new URL('files.json', content))).json();
    const assets = await Promise.all(names.map(async name =>
        [name, new Uint8Array(await (await fetchContent(new URL(name, content))).arrayBuffer())]));
    wasm.FS.mkdir('/content');
    for (const [name, bytes] of assets) wasm.FS.writeFile('/content/' + name, bytes);

    const context = canvas.getContext('2d', {alpha: false});
    const image = context.createImageData(canvas.width, canvas.height);
    const rgba = new Uint32Array(image.data.buffer);
    const decoder = new TextDecoder('shift_jis');
    context.font = '16px "MS Gothic", monospace';
    const check = status => {
        if (status < 0) throw Error(wasm.UTF8ToString(wasm._soew_error()));
        return status;
    };
    return {
        start(seed = 1234) { check(wasm._soew_start(seed)); },
        openReplay(bytes) {
            wasm.FS.writeFile('/input.rep', bytes);
            check(wasm._soew_open_replay());
        },
        step(mask = 0, group = false, fast = false) {
            return check(wasm._soew_step(mask, group, fast)) === 1;
        },
        draw() {
            check(wasm._soew_render());
            const start = wasm._soew_pixels() >>> 2;
            rgba.set(wasm.HEAPU32.subarray(start, start + rgba.length));
            context.putImageData(image, 0, 0);
            context.fillStyle = '#fff';
            for (let line = 0; line < 2; ++line) {
                const size = wasm._soew_dialogue_line(line);
                if (!size) continue;
                const start = wasm._soew_dialogue_bytes(line);
                context.fillText(decoder.decode(wasm.HEAPU8.subarray(start, start + size)),
                    112, 350 + line * 16, 288);
            }
        },
        get ticks() { return wasm._soew_tick_count(); },
        get digest() { return BigInt.asUintN(64, wasm._soew_digest()).toString(); },
        get playerX() { return wasm._soew_player_x(); },
        get slowdown() { return wasm._soew_slowdown(); },
        get retraceMs() { return wasm._soew_retrace_ms(); },
    };
}

export async function play(canvas) {
    const game = await createGame(canvas);
    const input = new Keyboard();
    const clock = new RetraceClock(game.retraceMs);
    let running = true, paused = false;
    const restart = () => {
        input.clear();
        clock.reset();
        game.start();
        game.draw();
        running = true;
    };
    const release = () => { input.clear(); clock.reset(); };
    restart();
    canvas.addEventListener('pointerdown', () => canvas.focus());
    window.addEventListener('keydown', event => {
        const {code, repeat} = event;
        if (!repeat && code === 'KeyR') { paused = false; restart(); }
        else if (!repeat && code === 'KeyP') { paused = !paused; release(); }
        else if (input.event(code, true)) event.preventDefault();
    });
    window.addEventListener('keyup', event => {
        if (input.event(event.code, false)) event.preventDefault();
    });
    window.addEventListener('blur', release);
    document.addEventListener('visibilitychange', release);

    function frame(now) {
        if (running && !paused && !document.hidden) {
            const before = game.ticks;
            clock.frame(now, () => game.slowdown, () => {
                running = game.step(...input.sample());
                return running;
            });
            if (game.ticks !== before) game.draw();
        }
        requestAnimationFrame(frame);
    }
    requestAnimationFrame(frame);
}
