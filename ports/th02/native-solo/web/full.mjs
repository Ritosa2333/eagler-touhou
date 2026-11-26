import createModule from './soew-full.mjs';
import {glyphCells} from './glyph-cells.mjs';

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

export async function playFull(canvas, content = new URL('./content/', import.meta.url), {
    hosted = false, beforeFrame, onExit, localCoop, confirmedCoop,
} = {}) {
    const screen = canvas.getContext('2d', {alpha: false});
    const framebuffer = new OffscreenCanvas(640,400);
    const framePixels = framebuffer.getContext('2d',{alpha:false});
    const textSurface = document.createElement('canvas');
    const textContext = textSurface.getContext('2d', {willReadFrequently: true});
    const picker = document.createElement('input');
    picker.type = 'file';
    picker.accept = '.rep';
    let wasm, cells, lastError, image, stopped = false, timer, frames = 0, ticks = 0;
    const listeners = new AbortController();
    const listen = (target,type,fn,options) => target.addEventListener(type,fn,{...options,signal:listeners.signal});
    let audio, audioPaused = false, audioEnd = 0;
    let album, albumTrack = null, albumLoad, albumNode;
    let albumVolume = 0, albumGain;
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
    function drawScreen() {
        const scale = window.devicePixelRatio;
        const rect = canvas.getBoundingClientRect();
        const w = Math.round(rect.width*scale), h = Math.round(rect.height*scale);
        if (canvas.width !== w) canvas.width = w;
        if (canvas.height !== h) canvas.height = h;
        screen.imageSmoothingEnabled = false;
        screen.drawImage(framebuffer,0,0,w,h);
    }
    const resize = new ResizeObserver(drawScreen);
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
            albumGain = audio.createGain();
            albumGain.gain.value = albumVolume;
            albumGain.connect(audio.destination);
            for (const [header, samples] of waiting) scheduleAudio(header, samples);
            waiting.clear();
            if (albumTrack) playAlbumTrack();
        }
        if (!audioPaused && audio.state !== 'running') void audio.resume();
    }
    function stopAlbumTrack() {
        albumTrack = null;
        albumLoad?.abort();
        albumNode?.stop();
        albumNode?.disconnect();
        albumNode = null;
    }
    async function playAlbumTrack() {
        const track = albumTrack;
        const load = new AbortController();
        albumLoad = load;
        try {
            const response = await fetch(joined(content, track.url), {signal:load.signal});
            if (!response.ok) throw Error(`HTTP ${response.status}: ${track.url}`);
            const buffer = await audio.decodeAudioData(await response.arrayBuffer());
            if (load.signal.aborted) return;
            const node = audio.createBufferSource();
            node.buffer = buffer;
            node.loop = true;
            node.loopStart = track.loopStart;
            node.loopEnd = Math.min(track.loopEnd, buffer.duration);
            node.connect(albumGain);
            node.start();
            albumNode = node;
        } catch (error) {
            if (!load.signal.aborted) console.error(error);
        }
    }
    const host = {
        present(pointer,w,h) {
            if (!image || image.width !== w || image.height !== h) {
                framebuffer.width = w;
                framebuffer.height = h;
                image = framePixels.createImageData(w,h);
                canvas.style.setProperty('--aspect',w/h);
            }
            const rgba = new Uint32Array(image.data.buffer);
            const pixels = wasm.HEAPU32.subarray(pointer >>> 2, (pointer >>> 2) + w*h);
            for (let i = 0; i < pixels.length; i++) {
                const p = pixels[i];
                rgba[i] = 0xff000000 | ((p & 255) << 16) | (p & 0xff00) | ((p >>> 16) & 255);
            }
            framePixels.putImageData(image,0,0);
            drawScreen();
            ++frames;
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
        hasCell(cp,face) { return cells.has(cp,face); },
        paintCells(pointer,w,h,text,n,x,y,color,face) {
            cells.paint(wasm.HEAPU32.subarray(pointer>>>2),wasm.HEAPU32.subarray(text>>>2,(text>>>2)+n),
                w,h,x,y,color,face);
        },
        title(pointer) { document.title = wide(pointer); },
        fullscreen(yes) {
            if (yes && !document.fullscreenElement) void canvas.requestFullscreen().catch(() => {});
            if (!yes && document.fullscreenElement) void document.exitFullscreen().catch(() => {});
        },
        openReplay() { picker.click(); },
        exit() { stop(); onExit?.(); },
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
            stopAlbumTrack();
            waiting.clear();
            for (const node of scheduled) {node.onended = null; node.stop(); node.disconnect();}
            scheduled.clear();
            audioEnd = 0;
        },
        akyuTrack(track) {
            stopAlbumTrack();
            albumTrack = album[track];
            if (audio) void playAlbumTrack();
        },
        akyuGain(gain) {
            albumVolume = gain;
            if (albumGain) albumGain.gain.value = gain;
        },
    };
    function stop() {
        if (stopped) return;
        stopped = true;
        clearTimeout(timer);
        resize.disconnect();
        listeners.abort();
        host.audioReset();
        if (audio) void audio.close();
    }
    const [module, names, atlas] = await Promise.all([
        createModule({host,printErr: message => {lastError = message; console.error(message);}}),
        fetchFile(joined(content,'full-files.json')).then(res => res.json()),
        fetchFile(joined(new URL('.', import.meta.url),'soew-glyphs.bin')).then(res => res.arrayBuffer()),
    ]);
    wasm = module;
    cells = glyphCells(atlas);
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
        if (name === 'akyu.pack') album = JSON.parse(new TextDecoder().decode(bytes));
        wasm.FS.mkdirTree(path.slice(0,path.lastIndexOf('/')));
        wasm.FS.writeFile(path,bytes);
    })]);
    if (wasm._soew_full_start() !== 0) throw Error(lastError ?? 'SoEW startup failed');
    if (localCoop && !wasm._soew_full_start_coop(...localCoop)) throw Error(lastError ?? 'SoEW local co-op startup failed');
    if (confirmedCoop && !wasm._soew_full_start_confirmed_coop(...confirmedCoop.rules,confirmedCoop.seat)) {
        stop();
        throw Error(lastError ?? 'SoEW confirmed co-op startup failed');
    }

    listen(picker,'change', async () => {
        const file = picker.files[0];
        if (!file) return;
        wasm.FS.writeFile('/soew/import.rep',new Uint8Array(await file.arrayBuffer()));
        wasm._soew_full_import_replay();
        picker.value = '';
        canvas.focus();
    });
    const focus = () => wasm._soew_host_focus(!document.hidden && document.hasFocus());
    listen(window,'focus',focus);
    listen(window,'blur',focus);
    listen(document,'visibilitychange',focus);
    listen(document,'fullscreenchange',() =>
        wasm._soew_full_fullscreen_changed(document.fullscreenElement === canvas));
    function deliverKey(code,down,event = {}) {
        const vk = keyCode(code);
        if (stopped || confirmedCoop || vk === undefined) return;
        const glyph = event.key ?? '';
        const scalar = !event.ctrlKey && !event.altKey && [...glyph].length === 1 ? glyph.codePointAt(0) : 0;
        wasm._soew_host_key(vk,down,Boolean(event.repeat),scalar);
    }
    function key(event,down) {
        if (stopped || event.metaKey || (event.ctrlKey && event.code === 'KeyV')) return;
        if (keyCode(event.code) === undefined) return;
        event.preventDefault();
        if (down) unlockAudio();
        deliverKey(event.code,down,event);
    }
    if (!hosted) {
        listen(window,'keydown',event => key(event,true));
        listen(window,'keyup',event => key(event,false));
    }
    listen(window,'paste',event => {
        if (confirmedCoop) return;
        const chars = [...event.clipboardData.getData('text')].map(c => c.codePointAt(0));
        const pointer = wasm._malloc((chars.length+1)*4);
        wasm.HEAPU32.set([...chars,0],pointer >>> 2);
        wasm._soew_host_paste(pointer);
        wasm._free(pointer);
        event.preventDefault();
    });
    function pointer(event,message,wparam=0) {
        if (confirmedCoop) return;
        const rect = canvas.getBoundingClientRect();
        const x = Math.floor((event.clientX-rect.left)*framebuffer.width/rect.width);
        const y = Math.floor((event.clientY-rect.top)*framebuffer.height/rect.height);
        wasm._soew_host_pointer(message,x,y,wparam);
    }
    listen(canvas,'contextmenu',event => event.preventDefault());
    listen(canvas,'pointerdown',event => {
        canvas.focus();
        canvas.setPointerCapture(event.pointerId);
        unlockAudio();
        pointer(event,[513,519,516,523,523][event.button],event.button >= 3 ? event.button-2 : 0);
    });
    listen(canvas,'pointermove',event => pointer(event,512));
    listen(canvas,'pointerup',event => {
        if (event.button === 0) pointer(event,514);
    });
    listen(canvas,'lostpointercapture',event => pointer(event,533));
    listen(canvas,'wheel',event => {
        event.preventDefault();
        pointer(event,522,-120*Math.sign(event.deltaY));
    },{passive:false});

    const player = {key:deliverKey, unlock:unlockAudio, stop, sync:() => sync(false),
        coopInputs:(...seats)=>wasm._soew_full_coop_inputs(...seats),
        health:() => ({frames,gameFrame:localCoop||confirmedCoop?wasm._soew_full_coop_frame():ticks,audio:{state:audio?.state ?? 'suspended'}})};
    if (confirmedCoop) player.confirmed = {
        step(frame, masks) {
            const result = wasm._soew_full_confirmed_step(frame,...masks);
            if (!result) throw Error(lastError ?? 'SoEW refused the confirmed frame');
            return result === 2;
        },
        hash: () => wasm._soew_full_confirmed_hash() >>> 0,
    };
    function frame() {
        if (stopped) return;
        beforeFrame?.(player);
        const wait = wasm._soew_host_frame();
        ++ticks;
        if (!stopped) timer = setTimeout(frame,Math.ceil(wait));
    }
    resize.observe(canvas);
    focus();
    frame();
    return player;
}
