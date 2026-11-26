// Retain PC-98 retraces even on displays with a different refresh rate.
export class RetraceClock {
    constructor(period) {
        this.period = period;
        this.reset();
    }
    reset() {
        this.last = undefined;
        this.remaining = 0;
        this.retraces = 0;
    }
    frame(now, slowdown, tick) {
        const elapsed = this.last === undefined ? this.period : now - this.last;
        this.last = now;
        if (elapsed >= 4 * this.period) this.remaining = this.period;
        else this.remaining += elapsed;
        while (this.remaining >= this.period) {
            this.remaining -= this.period;
            if (++this.retraces < slowdown()) continue;
            this.retraces = 0;
            if (!tick()) break;
        }
    }
}
