// Exact-input lockstep for the native TH03 lifecycle, including character select.
// One outstanding frame bounds memory and prevents peers advancing on guesses.
export class NativeLockstep {
  constructor({seat, send, step, digest, onError = () => {}}) {
    if (![0,1].includes(seat) || typeof send !== 'function' || typeof step !== 'function') throw Error('Invalid lockstep configuration');
    Object.assign(this, {seat, send, step, digest, onError});
    this.frame = 0; this.local = null; this.remote = new Map(); this.closed = false;
    this.digests = new Map(); this.remoteDigests = new Map();
  }
  submit(input) {
    if (this.closed || this.local !== null) return false;
    this.validate(input);
    this.local = input;
    let proof;
    if(this.digest && this.frame > 0 && this.frame % 120 === 0) {
      proof=this.digest();this.digests.set(this.frame,proof);this.compare(this.frame);
    }
    this.send(JSON.stringify({type:'native-input',frame:this.frame,input,...(proof===undefined?{}:{proof})}));
    this.advance(); return true;
  }
  receive(text) {
    if (this.closed) return;
    try {
      const msg = JSON.parse(text);
      if (msg.type !== 'native-input' || !Number.isSafeInteger(msg.frame) || msg.frame < 0 || msg.frame > this.frame + 1) throw Error('Invalid remote frame');
      this.validate(msg.input);
      if (msg.frame < this.frame) return;
      if (this.digest && msg.frame > 0 && msg.frame % 120 === 0) {
        if (!Number.isInteger(msg.proof) || msg.proof < 0 || msg.proof > 0xffffffff) throw Error('Invalid state proof');
        this.remoteDigests.set(msg.frame,msg.proof);this.compare(msg.frame);
      } else if (msg.proof !== undefined) throw Error('Unexpected state proof');
      if (this.remote.has(msg.frame)) {
        if (this.remote.get(msg.frame) !== msg.input) throw Error('Conflicting remote input');
        return;
      }
      this.remote.set(msg.frame,msg.input); this.advance();
    } catch (error) { this.closed = true; this.onError(error); }
  }
  validate(input) { if (!Number.isSafeInteger(input) || input < 0 || input > 2**45-1) throw Error('Invalid player input'); }
  compare(frame) {
    if(!this.digests.has(frame)||!this.remoteDigests.has(frame))return;
    if(this.digests.get(frame)!==this.remoteDigests.get(frame))throw Error(`双方游戏状态不同步（第 ${frame} 帧）`);
    this.digests.delete(frame);this.remoteDigests.delete(frame);
  }
  advance() {
    if (this.closed || this.local === null || !this.remote.has(this.frame)) return;
    const remote = this.remote.get(this.frame), local = this.local;
    this.remote.delete(this.frame); this.local = null;
    const inputs = this.seat === 0 ? [local,remote] : [remote,local];
    this.step(inputs,this.frame++);
  }
  close() { this.closed = true; this.remote.clear(); this.digests.clear(); this.remoteDigests.clear(); this.local = null; }
}
