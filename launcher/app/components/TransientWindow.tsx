import {Button as UiButton} from '../ui/controls';
import {useLayoutEffect, useRef, useState, type ReactNode} from 'react';
import {useSurfaceMotion} from '../ui/use-surface-motion';

export interface TransientWindowProps {
  id: string;
  titleId: string;
  title: ReactNode;
  closeId: string;
  closeLabel: string;
  open: boolean;
  busy?: boolean;
  /** A reference panel sits above the standard task panel and permits copying. */
  variant?: 'standard' | 'reference';
  onClose(): void;
  children: ReactNode;
}

// Standard and reference panels share their shell in ui/windows.css.
const presentationClass = {
  standard: 'game-data-import-window',
  reference: 'game-data-link-window',
} as const;

/** Controlled, nonmodal presentation. Children remain mounted when hidden.
 * The caller owns presentation events, focus, fullscreen placement and work;
 * this shell adds no native dialog, Escape/outside dismissal or history layer.
 */
export function TransientWindow({id, titleId, title, closeId, closeLabel, open, busy,
  variant = 'standard', onClose, children}: TransientWindowProps) {
  const ref = useRef<HTMLElement>(null), [retained, setRetained] = useState(open);
  const present = open || retained;
  useLayoutEffect(() => {if (open) setRetained(true);}, [open]);
  useSurfaceMotion(ref, present ? open ? 'enter' : 'exit' : null, {onComplete() {if (!open) setRetained(false);}});
  return <section ref={ref} className={presentationClass[variant]} id={id} role="dialog" aria-modal="false" aria-labelledby={titleId} aria-busy={busy} hidden={!present} inert={!open} aria-hidden={!open}>
    <header><div><strong id={titleId}>{title}</strong></div><UiButton variant="icon" id={closeId} type="button" aria-label={closeLabel} disabled={busy} onClick={onClose}>×</UiButton></header>
    {children}
  </section>;
}
