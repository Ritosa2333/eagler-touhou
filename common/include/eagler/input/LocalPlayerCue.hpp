#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace eagler::presentation {
// Publish the local player's rendered position in the game's 640x480 viewport.
// This is presentation only: no input bits, replay records or saved state.
inline void local_player_cue(unsigned seat, float x, float y) {
#ifdef __EMSCRIPTEN__
    EM_ASM({
        if (typeof document === "undefined" || Module.eaglerOptions?.netplaySpectator || Module.eaglerOptions?.replayViewer) return;
        let cue = globalThis.__eaglerLocalPlayerCue;
        if (!cue) {
            cue = globalThis.__eaglerLocalPlayerCue = Object.create(null);
            cue.until = 0;
            cue.held = false;
            const reset = () => {
                cue.held = false;
                cue.until = 0;
                if (cue.label) cue.label.hidden = true;
            };
            addEventListener("keydown", event => {
                if (event.code === "KeyI" && !event.repeat && !event.isComposing &&
                    !event.ctrlKey && !event.altKey && !event.metaKey &&
                    !event.target?.closest?.("input, textarea, [contenteditable]")) {
                    cue.held = true;
                    cue.until = performance.now() + 1200;
                }
            }, true);
            addEventListener("keyup", event => {
                if (event.code === "KeyI") cue.held = false;
            }, true);
            addEventListener("blur", reset);
            document.addEventListener("visibilitychange", reset);
        }
        const now = performance.now();
        if (document.hidden || (!cue.held && now >= cue.until)) {
            if (cue.label) cue.label.hidden = true;
            return;
        }
        const canvas = Module.canvas || document.querySelector("canvas");
        if (!canvas) return;
        const box = canvas.getBoundingClientRect();
        const scale = Math.min(box.width / 640, box.height / 480);
        if (!scale || !Number.isFinite($1) || !Number.isFinite($2)) return;
        if (!cue.label) {
            cue.label = document.createElement("span");
            cue.label.setAttribute("aria-hidden", "true");
            cue.label.style.cssText = "position:fixed;z-index:10;pointer-events:none;transform:translate(-50%,-100%);font:700 12px/1.2 system-ui,sans-serif;color:#fff4ba;background:#16131de8;padding:2px 5px;border-radius:3px;white-space:nowrap;";
            document.body.append(cue.label);
        }
        const text = "Player " + ($0 + 1);
        if (cue.label.textContent !== text) cue.label.textContent = text;
        cue.label.style.left = box.left + (box.width - 640 * scale) / 2 + $1 * scale + "px";
        cue.label.style.top = box.top + (box.height - 480 * scale) / 2 + Math.max(18, $2 - 14) * scale + "px";
        cue.label.hidden = false;
        cue.lastDraw = now;
        if (!cue.timer) {
            const hideWhenInactive = () => {
                const time = performance.now();
                if (time - cue.lastDraw >= 150 || (!cue.held && time >= cue.until)) {
                    cue.label.hidden = true;
                    cue.timer = 0;
                } else cue.timer = setTimeout(hideWhenInactive, 150);
            };
            cue.timer = setTimeout(hideWhenInactive, 150);
        }
    }, seat, x, y);
#else
    (void)seat;
    (void)x;
    (void)y;
#endif
}
}
