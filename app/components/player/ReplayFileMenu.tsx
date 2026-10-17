import {useLayoutEffect, useRef} from 'react';
import {FullscreenTransient} from '../FullscreenTransient';

export interface ReplayFileAction {label: string; run(): void; destructive?: boolean}
/** One floating action menu, outside the file list's scrolling/clipping owner. */
export function ReplayFileMenu({anchor, label, actions, onClose}: {
  anchor: HTMLElement; label: string; actions: ReplayFileAction[]; onClose(): void;
}) {
  const menu = useRef<HTMLDivElement>(null), close = useRef(onClose);
  close.current = onClose;
  useLayoutEffect(() => {
    const element = menu.current;
    if (!element) return;
    const rect = anchor.getBoundingClientRect(), width = element.offsetWidth, height = element.offsetHeight;
    const view = anchor.ownerDocument.defaultView!;
    const top = rect.bottom + 6 + height <= view.innerHeight - 8 ? rect.bottom + 6 : rect.top - height - 6;
    element.style.left = `${Math.max(8, Math.min(rect.right - width, view.innerWidth - width - 8))}px`;
    element.style.top = `${Math.max(8, top)}px`;
    element.querySelector<HTMLButtonElement>('button')?.focus({preventScroll: true});
    const outside = (event: PointerEvent) => {if (!element.contains(event.target as Node) && !anchor.contains(event.target as Node)) close.current();};
    const dismiss = () => close.current();
    const scrolled = (event: Event) => {if (!element.contains(event.target as Node)) dismiss();};
    document.addEventListener('pointerdown', outside, true);
    document.addEventListener('scroll', scrolled, true);
    view.addEventListener('resize', dismiss);
    return () => {document.removeEventListener('pointerdown', outside, true); document.removeEventListener('scroll', scrolled, true); view.removeEventListener('resize', dismiss);};
  }, [anchor]);
  return <FullscreenTransient><div ref={menu} className="replay-file-menu" role="menu" aria-label={label} onKeyDown={event => {
    if (event.key === 'Escape' || event.key === 'Tab') {event.preventDefault(); onClose(); anchor.focus({preventScroll: true}); return;}
    if (!['ArrowDown', 'ArrowUp', 'Home', 'End'].includes(event.key)) return;
    event.preventDefault();
    const buttons = [...event.currentTarget.querySelectorAll<HTMLButtonElement>('button')], index = buttons.indexOf(document.activeElement as HTMLButtonElement);
    buttons[event.key === 'Home' ? 0 : event.key === 'End' ? buttons.length - 1 : (index + (event.key === 'ArrowDown' ? 1 : -1) + buttons.length) % buttons.length]?.focus();
  }}>{actions.map(action => <button key={action.label} type="button" role="menuitem" className={action.destructive ? 'destructive' : undefined} onClick={() => {onClose(); anchor.focus({preventScroll: true}); action.run();}}>{action.label}</button>)}</div></FullscreenTransient>;
}
