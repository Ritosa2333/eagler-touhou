#pragma once
#include <cstdint>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

// Output service only: the game retains sound IDs, commands and deduplication.
// Prerecorded effects bypass the mixed music queue without a second sequencer.
EM_JS(double, eagler_web_sfx_command,
      (int op, unsigned id, const void* pcm, unsigned frames, unsigned channels,
       unsigned rate, int format, double value), {
    // BEGIN_WEB_SFX_JS
    const ctx = Module['SDL3'] && Module['SDL3'].audioContext;
    if (op === 10) Module['eaglerSfxPaused'] = !!value;
    let pool = Module['eaglerSfxOutput'];
    if (pool && pool.context !== ctx) {
        for (const key of Array.from(pool.voices.keys())) pool.erase(key);
        delete Module['eaglerSfxOutput'];pool = null;
    }
    if (!pool) {
        if (op !== 0 || !ctx || ctx.state === 'closed') return 0;
        const voices = new Map();
        const cursor = v => {
            let p = v.offset;
            if (v.node) p += Math.max(0, ctx.currentTime - v.started) * v.speed;
            return v.loop ? p % v.buffer.duration : Math.min(p, v.buffer.duration);
        };
        const stop = v => {
            v.offset = cursor(v);
            if (v.node) {
                v.node.onended = null;
                v.node.stop();
                v.node.disconnect();
                v.node = null;
            }
            v.playing = false;
        };
        const gains = v => {
            v.left.gain.setValueAtTime(v.volume * (v.pan > 0 ? 1 - v.pan : 1), ctx.currentTime);
            v.right.gain.setValueAtTime(v.volume * (v.pan < 0 ? 1 + v.pan : 1), ctx.currentTime);
        };
        const start = v => {
            v.playing = true;
            if (Module['eaglerSfxPaused']) return;
            if (!v.loop && v.offset >= v.buffer.duration) { v.playing = false; return; }
            const node = ctx.createBufferSource();
            node.buffer = v.buffer;
            node.loop = v.loop;
            node.playbackRate.value = v.speed;
            node.connect(v.split);
            v.node = node;
            v.started = ctx.currentTime;
            node.onended = () => {
                if (v.node !== node) return;
                node.disconnect();v.node = null;v.playing = false;v.offset = v.buffer.duration;
            };
            node.start(0, v.offset);
        };
        const erase = key => {
            const v = voices.get(key);
            if (!v) return;
            stop(v);v.split.disconnect();v.left.disconnect();v.right.disconnect();v.merge.disconnect();
            voices.delete(key);
        };
        pool = Module['eaglerSfxOutput'] = {context:ctx, voices, cursor, stop, gains, start, erase};
    }
    if (op === 0) {
        pool.erase(id);
        if (!frames || (channels !== 1 && channels !== 2) || !rate) return 0;
        try {
            const buffer = ctx.createBuffer(2, frames, rate);
            const heap = format === 0 ? new Float32Array(HEAPU8.buffer, pcm, frames * channels)
                : format === 1 ? new Int16Array(HEAPU8.buffer, pcm, frames * channels)
                : new Uint8Array(HEAPU8.buffer, pcm, frames * channels);
            for (let c = 0; c < 2; ++c) {
                const out = buffer.getChannelData(c), input = Math.min(c, channels - 1);
                for (let f = 0; f < frames; ++f) {
                    const sample = heap[f * channels + input];
                    out[f] = format === 0 ? sample : format === 1 ? sample / 32768 : (sample - 128) / 128;
                }
            }
            const v = {buffer, split:ctx.createChannelSplitter(2), left:ctx.createGain(),
                right:ctx.createGain(), merge:ctx.createChannelMerger(2), volume:1, pan:0,
                offset:0, started:0, speed:1, loop:false, playing:false, node:null};
            v.split.connect(v.left,0);v.split.connect(v.right,1);
            v.left.connect(v.merge,0,0);v.right.connect(v.merge,0,1);v.merge.connect(ctx.destination);
            pool.voices.set(id,v);pool.gains(v);return 1;
        } catch (_) { return 0; }
    }
    if (op === 10) {
        for (const v of pool.voices.values()) {
            if (!v.playing) continue;
            if (value) { pool.stop(v);v.playing=true; }
            else if (!v.node) pool.start(v);
        }
        return 1;
    }
    if (op === 12) { for (const key of Array.from(pool.voices.keys())) pool.erase(key); return 1; }
    const v = pool.voices.get(id);
    if (!v) return 0;
    switch (op) {
    case 1: pool.erase(id);break;
    case 2: pool.stop(v);v.loop=!!value;pool.start(v);break;
    case 3: pool.stop(v);break;
    case 4: {const playing=v.playing;pool.stop(v);v.offset=Math.max(0,Math.min(value/rate,v.buffer.duration));if(playing)pool.start(v);break;}
    case 5: v.volume=Math.max(0,value);pool.gains(v);break;
    case 6: v.pan=Math.max(-1,Math.min(1,value));pool.gains(v);break;
    case 7: {if(v.speed===value)break;const playing=v.playing;pool.stop(v);v.speed=value;if(playing)pool.start(v);break;}
    case 8: return pool.cursor(v)*v.buffer.sampleRate;
    case 9: return +v.playing;
    case 11: return 1;
    }
    return 1;
    // END_WEB_SFX_JS
});
#endif

namespace eagler::audio::web_sfx {
inline unsigned key(const void* voice) { return static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(voice)); }
inline double command(int op, unsigned id=0, const void* pcm=nullptr, unsigned frames=0,
                      unsigned channels=0, unsigned rate=0, int format=0, double value=0) {
#ifdef __EMSCRIPTEN__
    return eagler_web_sfx_command(op,id,pcm,frames,channels,rate,format,value);
#else
    return 0;
#endif
}
inline bool upload(const void* voice,const void* pcm,unsigned frames,unsigned channels,unsigned rate,int format=0) {
    return command(0,key(voice),pcm,frames,channels,rate,format)!=0;
}
inline bool contains(const void* voice) { return command(11,key(voice))!=0; }
inline void erase(const void* voice) { command(1,key(voice)); }
inline bool play(const void* voice,bool loop=false) { return command(2,key(voice),nullptr,0,0,0,0,loop)!=0; }
inline void stop(const void* voice) { command(3,key(voice)); }
inline void seek(const void* voice,unsigned frame,unsigned rate) { command(4,key(voice),nullptr,0,0,rate,0,frame); }
inline void volume(const void* voice,float gain) { command(5,key(voice),nullptr,0,0,0,0,gain); }
inline void pan(const void* voice,float balance) { command(6,key(voice),nullptr,0,0,0,0,balance); }
inline void speed(const void* voice,float ratio) { command(7,key(voice),nullptr,0,0,0,0,ratio); }
inline unsigned cursor(const void* voice) { return static_cast<unsigned>(command(8,key(voice))); }
inline bool playing(const void* voice) { return command(9,key(voice))!=0; }
inline void suspend(bool paused) { command(10,0,nullptr,0,0,0,0,paused); }
inline void clear() { command(12); }
}
