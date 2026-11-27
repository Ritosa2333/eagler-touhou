const easing = 'cubic-bezier(.22,.7,.2,1)';
export const microLayoutTiming = {duration: 200, easing};

export function prefersStillMotion(element: Element) {
  return !!element.closest('.less-motion') || window.matchMedia('(prefers-reduced-motion: reduce)').matches;
}

const iconFeedback = {
  favorite: [
    {scale: '1', rotate: '0deg'},
    {scale: '1.08', rotate: '-3deg', offset: .4},
    {scale: '1', rotate: '0deg'},
  ],
  theme: [
    {opacity: .7, scale: '.96', rotate: '-8deg'},
    {opacity: 1, scale: '1', rotate: '0deg'},
  ],
  refresh: [
    {rotate: '0deg'},
    {rotate: '18deg', offset: .46},
    {rotate: '0deg'},
  ],
} satisfies Record<string, Keyframe[]>;

const active = new WeakMap<SVGElement, Animation>();

/** One short, interruptible response; the caller owns the actual action. */
export function playIconFeedback(button: HTMLElement, kind: keyof typeof iconFeedback) {
  const icon = button.querySelector('svg');
  if (!icon || prefersStillMotion(button)) return;
  active.get(icon)?.cancel();
  const animation = icon.animate(iconFeedback[kind], {duration: 240, easing});
  active.set(icon, animation);
  return animation;
}

/** Stay close to the destination even when a result moves several columns. */
export function microReorderFrames(x: number, y: number): Keyframe[] {
  const distance = Math.hypot(x, y);
  const ratio = distance > 6 ? 6 / distance : 1;
  return [
    {translate: `${x * ratio}px ${y * ratio}px`, ...(ratio < 1 && {opacity: .78})},
    {translate: '0px 0px', ...(ratio < 1 && {opacity: 1})},
  ];
}

export const microArrivalFrames: Keyframe[] = [
  {opacity: .72, translate: '0px 3px'},
  {opacity: 1, translate: '0px 0px'},
];
