// SoEW core_contract.hpp input bits, not DOM key codes.
const keys = new Map([
    ['ArrowUp', 1], ['Numpad8', 1], ['ArrowDown', 2], ['Numpad2', 2],
    ['ArrowLeft', 4], ['Numpad4', 4], ['ArrowRight', 8], ['Numpad6', 8],
    ['Space', 16], ['KeyX', 32], ['Escape', 64], ['Enter', 128], ['NumpadEnter', 128],
    ['KeyQ', 256], ['ShiftLeft', 1024], ['ShiftRight', 1024],
    ['Numpad7', 4096], ['Numpad9', 8192], ['Numpad1', 16384], ['Numpad3', 32768],
]);

export class Keyboard {
    held = new Set();
    shotEdges = [];
    shot = false;

    event(code, down) {
        if (code !== 'KeyZ' && !keys.has(code)) return false;
        if (this.held.has(code) === down) return true;
        if (down) this.held.add(code);
        else this.held.delete(code);
        if (code === 'KeyZ') this.shotEdges.push(down);
        return true;
    }
    clear() {
        this.held.clear();
        this.shotEdges.length = 0;
        this.shot = false;
    }
    sample() {
        this.shot = this.shotEdges.shift() ?? this.held.has('KeyZ');
        let mask = this.shot ? 16 : 0;
        for (const code of this.held) mask |= keys.get(code) ?? 0;
        return [mask, this.shot || Boolean(mask & (16 | 32 | 128))];
    }
}
