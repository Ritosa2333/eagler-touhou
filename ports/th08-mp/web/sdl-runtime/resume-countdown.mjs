// osu!lazer-style synchronized resume countdown overlay (DelayedResumeOverlay
// visual language: two circles, a sweeping progress ring and a popping digit).
// The native world owns the countdown; this is presentation-only and follows
// the canonical frames remaining exported by the SDL runtime.
// Must stay in sync with resume_countdown_frames in web/cpp/game/UiMenus.cpp.
const TOTAL_FRAMES = 180, FRAMES_PER_DIGIT = 60;
const OUT_QUINT = 'cubic-bezier(.23,1,.32,1)';
const STYLES = `
#th08-resume{position:fixed;inset:0;display:grid;place-items:center;pointer-events:none;z-index:40;opacity:0;transition:opacity .3s ${OUT_QUINT}}
#th08-resume.active{opacity:1}
#th08-resume .dim{position:absolute;inset:0;background:radial-gradient(circle at 50% 50%,rgba(16,10,24,.28),rgba(8,5,12,.55));opacity:0;transition:opacity .3s ${OUT_QUINT}}
#th08-resume.active .dim{opacity:1}
#th08-resume .outer{position:relative;width:var(--rc-unit);height:var(--rc-unit);border-radius:50%;background:rgba(26,18,36,.92);box-shadow:0 18px 60px rgba(0,0,0,.45),inset 0 0 0 1px rgba(190,140,255,.08);transform:scale(0);animation:rc-outer-in .2s ${OUT_QUINT} forwards}
#th08-resume .inner{position:absolute;inset:12.5%;border-radius:50%;background:rgba(48,32,72,.95);box-shadow:inset 0 0 24px rgba(0,0,0,.35);transform:scale(0);animation:rc-inner-in .4s cubic-bezier(.16,1.3,.3,1) forwards}
#th08-resume .inner.flash{animation:rc-inner-in .4s cubic-bezier(.16,1.3,.3,1) forwards,rc-flash .4s ${OUT_QUINT}}
#th08-resume .ring{position:absolute;inset:0;width:100%;height:100%;transform:rotate(-90deg)}
#th08-resume .ring circle{fill:none;stroke-width:7;stroke-linecap:round}
#th08-resume .ring .track{stroke:rgba(255,255,255,.05)}
#th08-resume .ring .bar{stroke:#b07ce8;filter:drop-shadow(0 0 6px rgba(160,111,214,.55))}
#th08-resume .number{position:absolute;inset:0;display:grid;place-items:center;font:300 calc(var(--rc-unit)*.34)/1 Torus,'Poppins','Nunito',ui-rounded,system-ui,sans-serif;color:#f4edff;text-shadow:0 2px 18px rgba(120,70,190,.35);transform-origin:50% 50%}
#th08-resume .number.pop{animation:rc-digit .2s ${OUT_QUINT}}
#th08-resume .outer.pulse{animation:rc-outer-in .2s ${OUT_QUINT} forwards,rc-pulse .3s ${OUT_QUINT}}
#th08-resume.closing .ring{transition:transform .3s ${OUT_QUINT},opacity .3s ${OUT_QUINT};transform:rotate(-90deg) scale(2);opacity:0}
#th08-resume.closing{opacity:0}
@keyframes rc-outer-in{from{transform:scale(0)}to{transform:scale(1)}}
@keyframes rc-inner-in{from{transform:scale(0)}to{transform:scale(1)}}
@keyframes rc-digit{from{transform:scale(.25)}to{transform:scale(1)}}
@keyframes rc-pulse{0%{transform:scale(1)}33%{transform:scale(1.05)}100%{transform:scale(1)}}
@keyframes rc-flash{from{background:rgba(122,84,178,.95)}to{background:rgba(48,32,72,.95)}}
`;
// SVG circle in a 200x200 viewBox; the visible bar length follows the
// canonical frames remaining so every endpoint sweeps in lockstep.
const R = 96.5, CIRCUMFERENCE = 2 * Math.PI * R;
function playTicks(context, final) {
  if (!context) return;
  try {
    const t = context.currentTime, osc = context.createOscillator(), gain = context.createGain();
    osc.type = 'sine'; osc.frequency.value = final ? 392 : 784;
    gain.gain.setValueAtTime(0.0001, t);
    gain.gain.exponentialRampToValueAtTime(0.16, t + 0.008);
    gain.gain.exponentialRampToValueAtTime(0.0001, t + (final ? 0.32 : 0.11));
    osc.connect(gain).connect(context.destination);
    osc.start(t); osc.stop(t + (final ? 0.36 : 0.14));
  } catch {}
}
export function createResumeCountdown({remaining, audio, canvas = () => document.querySelector('canvas')}) {
  let root = null, number = null, bar = null, outer = null, inner = null;
  let active = false, digitShown = 0, closeTimer = 0;
  const ensure = () => {
    if (root) return root;
    const style = document.createElement('style');
    style.textContent = STYLES;
    document.head.append(style);
    root = document.createElement('div');
    root.id = 'th08-resume';
    root.innerHTML = `<div class="dim"></div><div class="outer"><div class="inner">
<svg class="ring" viewBox="0 0 200 200"><circle class="track" cx="100" cy="100" r="${R}"></circle><circle class="bar" cx="100" cy="100" r="${R}"></circle></svg>
<div class="number"></div></div></div>`;
    document.body.append(root);
    outer = root.querySelector('.outer'); inner = root.querySelector('.inner');
    number = root.querySelector('.number'); bar = root.querySelector('.bar');
    bar.style.strokeDasharray = CIRCUMFERENCE.toFixed(2);
    return root;
  };
  const resize = () => {
    const box = canvas()?.getBoundingClientRect();
    if (box?.height) root.style.setProperty('--rc-unit', Math.round(box.height * 0.26) + 'px');
  };
  const reopen = () => {
    clearTimeout(closeTimer); root.classList.remove('closing', 'active');
    root.style.opacity = '';
    resize();
    // Restart the pop-in transitions from a clean slate.
    const clone = root.cloneNode(true);
    root.replaceWith(clone); root = clone;
    outer = clone.querySelector('.outer'); inner = clone.querySelector('.inner');
    number = clone.querySelector('.number'); bar = clone.querySelector('.bar');
    bar.style.strokeDasharray = CIRCUMFERENCE.toFixed(2);
    root.getBoundingClientRect(); root.classList.add('active');
  };
  const hide = () => {
    if (!active) return;
    active = false; digitShown = 0;
    playTicks(audio(), true);
    root.style.opacity = '';
    root.classList.add('closing');
    closeTimer = setTimeout(() => {root?.classList.remove('active', 'closing');}, 320);
  };
  // Teardown path for a session that dies mid-countdown: no completion beep,
  // no closing sweep — just clear the overlay so it cannot freeze on screen.
  const dismiss = () => {
    if (!active) return;
    active = false; digitShown = 0;
    clearTimeout(closeTimer);
    root.style.opacity = '';
    root.classList.remove('active', 'closing');
  };
  function pump() {
    const frames = Number(remaining?.() || 0);
    if (frames <= 0) {if (active) hide(); return;}
    if (!active) {ensure(); reopen(); active = true;}
    if (!root.style.getPropertyValue('--rc-unit')) resize();
    const digit = Math.max(1, Math.ceil(frames / FRAMES_PER_DIGIT));
    // lazer fades the whole overlay toward transparent as the count drops.
    root.style.opacity = (0.2 + 0.8 * digit / 3).toFixed(3);
    bar.style.strokeDashoffset = (CIRCUMFERENCE * frames / TOTAL_FRAMES).toFixed(2);
    if (digit !== digitShown) {
      digitShown = digit;
      number.textContent = String(digit);
      number.classList.remove('pop'); void number.offsetWidth; number.classList.add('pop');
      outer.classList.remove('pulse'); void outer.offsetWidth; outer.classList.add('pulse');
      inner.classList.remove('flash'); void inner.offsetWidth; inner.classList.add('flash');
      playTicks(audio(), false);
    }
  };
  pump.dismiss = dismiss;
  return pump;
}
