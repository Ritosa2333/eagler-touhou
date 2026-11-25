import createModule from './soew-full.mjs';
import {sha256HexSync} from './package-integrity.mjs';

const shiftJis = new TextDecoder('shift_jis', {fatal: true});
const numericKeys = new Map([
    ['Backspace',8],['Tab',9],['Enter',13],['NumpadEnter',13],['Escape',27],['Space',32],
    ['PageUp',33],['PageDown',34],['End',35],['Home',36],['ArrowLeft',37],['ArrowUp',38],
    ['ArrowRight',39],['ArrowDown',40],['Delete',46],['ContextMenu',93],['NumpadSubtract',109],
    ['ShiftLeft',160],['ShiftRight',161],['ControlLeft',162],['ControlRight',163],
    ['AltLeft',164],['AltRight',165],['Minus',189],['Backquote',192],['BracketLeft',219],['BracketRight',221],
]);
function keyCode(code) {
    if (/^Key[A-Z]$|^Digit[0-9]$/.test(code)) return code.charCodeAt(code.length - 1);
    if (/^F([1-9]|1[0-2])$/.test(code)) return 111 + Number(code.slice(1));
    if (/^Numpad[0-9]$/.test(code)) return 96 + Number(code.at(-1));
    return numericKeys.get(code);
}

async function fetchFile(url) {
    const response = await fetch(url);
    if (!response.ok) throw Error(`HTTP ${response.status}: ${url}`);
    return response;
}
const joined = (root, name) => new URL(name.split('/').map(encodeURIComponent).join('/'), root);

export async function playFull(canvas, content = new URL('./content/', import.meta.url)) {
    const screen = canvas.getContext('2d', {alpha: false});
    const textSurface = document.createElement('canvas');
    const textContext = textSurface.getContext('2d', {willReadFrequently: true});
    const picker = document.createElement('input');
    picker.type = 'file';
    picker.accept = '.rep';
    let wasm, lastError, image, stopped = false, timer;
    let audio, audioPaused = false, audioEnd = 0;
    const waiting = new Map(), scheduled = new Set();
    const masks = new Map();

    function wide(pointer, length) {
        const words = wasm.HEAPU32;
        let out = '', i = pointer >>> 2;
        const end = length === undefined ? words.length : i + length;
        for (; i < end && (length !== undefined || words[i]); ++i) out += String.fromCodePoint(words[i]);
        return out;
    }
    function sync(populate) {
        return new Promise((done, fail) => wasm.FS.syncfs(populate, err => err ? fail(err) : done()));
    }
    function fontStyle(face,size,bold) {
        const family = face ? `"${face}", "Yu Gothic", "Microsoft YaHei", monospace` : 'sans-serif';
        return `${bold ? 'bold ' : ''}${size}px ${family}`;
    }
    function raster(value, face, size, bold, w, h, flags, x, y) {
        const key = JSON.stringify([value,face,size,bold,w,h,flags,x,y]);
        if (masks.has(key)) return masks.get(key);
        textSurface.width = w;
        textSurface.height = h;
        textContext.font = fontStyle(face,size,bold);
        textContext.fillStyle = '#fff';
        textContext.textAlign = flags & 2 ? 'right' : 'left';
        textContext.textBaseline = 'middle';
        if (flags & 32768 && textContext.measureText(value).width > w) {
            const letters = [...value];
            while (letters.length && textContext.measureText(letters.join('') + '…').width > w) letters.pop();
            value = letters.join('') + '…';
        }
        textContext.fillText(value, flags & 2 ? w : x,
            (flags & 8 ? h - size / 2 : flags & 4 ? h / 2 : y + size / 2));
        const rgba = textContext.getImageData(0,0,w,h).data;
        const ink = [];
        for (let i = 0; i < w * h; ++i) if (rgba[4*i+3] >= 96) ink.push(i);
        if (masks.size >= 2048) masks.delete(masks.keys().next().value);
        masks.set(key, ink);
        return ink;
    }
    function scheduleAudio(header, samples) {
        const count = samples.length / 2;
        const buffer = audio.createBuffer(2, count, 48000);
        const left = buffer.getChannelData(0), right = buffer.getChannelData(1);
        for (let i = 0; i < count; i++) {
            left[i] = samples[i*2] / 32768;
            right[i] = samples[i*2+1] / 32768;
        }
        const node = audio.createBufferSource();
        node.buffer = buffer;
        node.connect(audio.destination);
        scheduled.add(node);
        node.onended = () => {
            scheduled.delete(node);
            wasm.HEAPU32[(header+8) >>> 2] = 1;
        };
        const at = Math.max(audioEnd, audio.currentTime + 0.005);
        node.start(at);
        audioEnd = at + buffer.duration;
    }
    function unlockAudio() {
        if (!audio) {
            audio = new AudioContext({latencyHint:'interactive'});
            for (const [header, samples] of waiting) scheduleAudio(header, samples);
            waiting.clear();
        }
        if (!audioPaused && audio.state !== 'running') void audio.resume();
    }
    const host = {
        present(pointer,w,h) {
            if (canvas.width !== w) canvas.width = w;
            if (canvas.height !== h) canvas.height = h;
            if (!image || image.width !== w || image.height !== h) {
                image = screen.createImageData(w,h);
                canvas.style.setProperty('--aspect',w/h);
            }
            const rgba = new Uint32Array(image.data.buffer);
            const pixels = wasm.HEAPU32.subarray(pointer >>> 2, (pointer >>> 2) + w*h);
            for (let i = 0; i < pixels.length; i++) {
                const p = pixels[i];
                rgba[i] = 0xff000000 | ((p & 255) << 16) | (p & 0xff00) | ((p >>> 16) & 255);
            }
            screen.putImageData(image,0,0);
        },
        text(pointer,w,h,x,y,left,top,right,bottom,s,n,face,size,bold,color,flags,opaque,background) {
            if (n === 0) return;
            const value = wide(s,n), family = wide(face);
            if (opaque) {
                // DrawTextW fills the text run, not the full alignment rectangle.
                textContext.font = fontStyle(family,size,bold);
                const width = Math.ceil(textContext.measureText(value).width);
                if (flags & 2) left = Math.max(left,right-width);
                else right = Math.min(right,x+width);
                if (flags & 8) top = Math.max(top,bottom-size);
                else bottom = Math.min(bottom,y+size);
            }
            const cw = right-left, ch = bottom-top;
            if (cw <= 0 || ch <= 0) return;
            const pixels = wasm.HEAPU32, start = pointer >>> 2;
            if (opaque) for (let row = Math.max(top,0); row < Math.min(bottom,h); row++)
                pixels.fill(background,start+row*w+Math.max(left,0),start+row*w+Math.min(right,w));
            const ink = raster(value,family,size,bold,cw,ch,flags,x-left,y-top);
            for (const i of ink) {
                const row = top + Math.floor(i/cw), col = left + i%cw;
                if (row >= 0 && row < h && col >= 0 && col < w) pixels[start+row*w+col] = color;
            }
        },
        glyph(pointer,n) {
            const value = wide(pointer,n);
            return /^\s$/u.test(value) || raster(value,'MS Gothic',16,true,16,16,0,0,0).length > 0;
        },
        title(pointer) { document.title = wide(pointer); },
        fullscreen(yes) {
            if (yes && !document.fullscreenElement) void canvas.requestFullscreen().catch(() => {});
            if (!yes && document.fullscreenElement) void document.exitFullscreen().catch(() => {});
        },
        openReplay() { picker.click(); },
        exit() { stopped = true; clearTimeout(timer); },
        decode(pointer,n,out,count) {
            let value;
            try { value = [...shiftJis.decode(wasm.HEAPU8.subarray(pointer,pointer+n))]; }
            catch { return 0; }
            if (out) {
                if (count < value.length) return 0;
                wasm.HEAPU32.set(value.map(c => c.codePointAt(0)),out >>> 2);
            }
            return value.length;
        },
        digest(pointer,n,out) {
            const hex = sha256HexSync(wasm.HEAPU8.subarray(pointer,pointer+n));
            for (let i = 0; i < 32; i++) wasm.HEAPU8[out+i] = Number.parseInt(hex.slice(i*2,i*2+2),16);
            return 0;
        },
        audioWrite(header,pointer,n) {
            const pcm = wasm.HEAP16.slice(pointer >>> 1,(pointer+n) >>> 1);
            if (audio) scheduleAudio(header,pcm);
            else waiting.set(header,pcm);
        },
        audioPause(yes) {
            audioPaused = Boolean(yes);
            if (audio) void (audioPaused ? audio.suspend() : audio.resume());
        },
        audioReset() {
            waiting.clear();
            for (const node of scheduled) {node.onended = null; node.stop(); node.disconnect();}
            scheduled.clear();
            audioEnd = 0;
        },
    };
    const [module, names] = await Promise.all([
        createModule({host,printErr: message => {lastError = message; console.error(message);}}),
        fetchFile(new URL('full-files.json', content)).then(res => res.json()),
    ]);
    wasm = module;
    wasm.FS.mkdirTree('/soew/content');
    for (const dir of ['save','rep']) {
        const path = '/soew/' + dir;
        wasm.FS.mkdir(path);
        wasm.FS.mount(wasm.IDBFS,{autoPersist:true},path);
    }
    wasm.FS.writeFile('/soew/portable.ini','');
    await Promise.all([sync(true), ...names.map(async name => {
        const path = '/soew/content/' + name;
        const bytes = new Uint8Array(await (await fetchFile(joined(content,name))).arrayBuffer());
        wasm.FS.mkdirTree(path.slice(0,path.lastIndexOf('/')));
        wasm.FS.writeFile(path,bytes);
    })]);
    if (wasm._soew_full_start() !== 0) throw Error(lastError ?? 'SoEW startup failed');

    picker.addEventListener('change', async () => {
        const file = picker.files[0];
        if (!file) return;
        wasm.FS.writeFile('/soew/import.rep',new Uint8Array(await file.arrayBuffer()));
        wasm._soew_full_import_replay();
        picker.value = '';
        canvas.focus();
    });
    const focus = () => wasm._soew_host_focus(!document.hidden && document.hasFocus());
    window.addEventListener('focus',focus);
    window.addEventListener('blur',focus);
    document.addEventListener('visibilitychange',focus);
    document.addEventListener('fullscreenchange',() =>
        wasm._soew_full_fullscreen_changed(document.fullscreenElement === canvas));
    function key(event,down) {
        if (stopped || event.metaKey || (event.ctrlKey && event.code === 'KeyV')) return;
        const code = keyCode(event.code);
        if (code === undefined) return;
        event.preventDefault();
        if (down) unlockAudio();
        const scalar = !event.ctrlKey && !event.altKey && [...event.key].length === 1 ? event.key.codePointAt(0) : 0;
        wasm._soew_host_key(code,down,event.repeat,scalar);
    }
    window.addEventListener('keydown',event => key(event,true));
    window.addEventListener('keyup',event => key(event,false));
    window.addEventListener('paste',event => {
        const chars = [...event.clipboardData.getData('text')].map(c => c.codePointAt(0));
        const pointer = wasm._malloc((chars.length+1)*4);
        wasm.HEAPU32.set([...chars,0],pointer >>> 2);
        wasm._soew_host_paste(pointer);
        wasm._free(pointer);
        event.preventDefault();
    });
    function pointer(event,message,wparam=0) {
        const rect = canvas.getBoundingClientRect();
        const x = Math.floor((event.clientX-rect.left)*canvas.width/rect.width);
        const y = Math.floor((event.clientY-rect.top)*canvas.height/rect.height);
        wasm._soew_host_pointer(message,x,y,wparam);
    }
    canvas.addEventListener('contextmenu',event => event.preventDefault());
    canvas.addEventListener('pointerdown',event => {
        canvas.focus();
        canvas.setPointerCapture(event.pointerId);
        unlockAudio();
        pointer(event,[513,519,516,523,523][event.button],event.button >= 3 ? event.button-2 : 0);
    });
    canvas.addEventListener('pointermove',event => pointer(event,512));
    canvas.addEventListener('pointerup',event => {
        if (event.button === 0) pointer(event,514);
    });
    canvas.addEventListener('lostpointercapture',event => pointer(event,533));
    canvas.addEventListener('wheel',event => {
        event.preventDefault();
        pointer(event,522,-120*Math.sign(event.deltaY));
    },{passive:false});

    function frame() {
        if (stopped) return;
        const wait = wasm._soew_host_frame();
        if (!stopped) timer = setTimeout(frame,Math.ceil(wait));
    }
    focus();
    frame();
}
