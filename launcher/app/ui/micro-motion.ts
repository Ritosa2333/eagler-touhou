const easing = 'cubic-bezier(.4,0,.6,1)';
export const microLayoutTiming = {duration: 180, easing};
const reorderTravel = 3;

export function prefersStillMotion(element: Element) {
  return !!element.closest('.less-motion') || window.matchMedia('(prefers-reduced-motion: reduce)').matches;
}

const iconFeedback = {
  favorite: [
    {scale: '1', easing},
    {scale: '1.035', easing},
    {scale: '1'},
  ],
  theme: [
    {opacity: .86, transform: 'rotate(-3deg)', easing},
    {opacity: 1, transform: 'rotate(0deg)'},
  ],
  refresh: [
    {rotate: '0deg', easing},
    {rotate: '6deg', easing},
    {rotate: '0deg'},
  ],
} satisfies Record<string, Keyframe[]>;

const active = new WeakMap<SVGElement, Animation>();

/** One short, interruptible response; the caller owns the actual action. */
export function playIconFeedback(button: HTMLElement, kind: keyof typeof iconFeedback) {
  const icon = button.querySelector('svg');
  if (!icon || prefersStillMotion(button)) return;
  active.get(icon)?.cancel();
  const animation = icon.animate(iconFeedback[kind], microLayoutTiming.duration);
  active.set(icon, animation);
  return animation;
}

/** Stay close to the destination even when a result moves several columns. */
export function microReorderFrames(x: number, y: number): Keyframe[] {
  const distance = Math.hypot(x, y);
  const ratio = distance > reorderTravel ? reorderTravel / distance : 1;
  return [
    {translate: `${x * ratio}px ${y * ratio}px`, ...(ratio < 1 && {opacity: .9})},
    {translate: '0px 0px', ...(ratio < 1 && {opacity: 1})},
  ];
}

export const microArrivalFrames: Keyframe[] = [
  {opacity: .86, translate: '0px 2px'},
  {opacity: 1, translate: '0px 0px'},
];
