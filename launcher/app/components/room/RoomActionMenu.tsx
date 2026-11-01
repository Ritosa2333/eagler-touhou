import { type ReactNode, useLayoutEffect, useRef, useState } from 'react';
import { useSurfaceMotion } from '../../lib/surface-motion';
import { FullscreenTransient } from '../FullscreenTransient';
import { Button } from '../ui/button';

export interface RoomActionMenuItem {
  id: string;
  label: string;
  disabled?: boolean;
  danger?: boolean;
  select(): void;
}

/** Shared popup placement, keyboard focus and motion for room actions. */
export function RoomActionMenu({
  enabled,
  label,
  triggerContent,
  triggerClassName,
  menuClassName,
  items,
  minimumWidth = 260,
  align = 'start',
}: {
  enabled: boolean;
  label: string;
  triggerContent: ReactNode;
  triggerClassName: string;
  menuClassName: string;
  items: readonly RoomActionMenuItem[];
  minimumWidth?: number;
  align?: 'start' | 'end';
}) {
  const trigger = useRef<HTMLButtonElement>(null),
    menu = useRef<HTMLDivElement>(null);
  const [phase, setPhase] = useState<'closed' | 'open' | 'closing'>('closed');
  useSurfaceMotion(menu, phase === 'open', 'menu');
  const close = (restore = true) => {
    if (restore) trigger.current?.focus({ preventScroll: true });
    setPhase(
      matchMedia('(prefers-reduced-motion: reduce)').matches || trigger.current?.closest('.less-motion')
        ? 'closed'
        : 'closing',
    );
  };
  const latestClose = useRef(close);
  latestClose.current = close;
  useLayoutEffect(() => {
    if (phase !== 'closing') return;
    const timer = window.setTimeout(() => setPhase('closed'), 160);
    return () => window.clearTimeout(timer);
  }, [phase]);
  useLayoutEffect(() => {
    const element = menu.current,
      anchor = trigger.current;
    if (phase !== 'open' || !element || !anchor) return;
    const rect = anchor.getBoundingClientRect(),
      view = anchor.ownerDocument.defaultView!;
    const above = Math.max(0, rect.top - 16),
      below = Math.max(0, view.innerHeight - rect.bottom - 16);
    const upwards = above >= below,
      width = Math.min(Math.max(rect.width, minimumWidth), view.innerWidth - 24);
    element.style.width = `${width}px`;
    element.style.maxHeight = `${Math.min(330, upwards ? above : below)}px`;
    const left = align === 'end' ? rect.right - width : rect.left;
    element.style.left = `${Math.max(12, Math.min(left, view.innerWidth - width - 12))}px`;
    element.style.top = `${upwards ? rect.top - element.offsetHeight - 8 : rect.bottom + 8}px`;
    element.style.transformOrigin = upwards ? 'bottom center' : 'top center';
    element.querySelector<HTMLButtonElement>('button:not(:disabled)')?.focus({ preventScroll: true });
    const outside = (event: PointerEvent) => {
      if (!element.contains(event.target as Node) && !anchor.contains(event.target as Node)) latestClose.current(false);
    };
    const scroll = (event: Event) => {
      if (!element.contains(event.target as Node)) latestClose.current(false);
    };
    const resize = () => latestClose.current(false);
    document.addEventListener('pointerdown', outside, true);
    document.addEventListener('scroll', scroll, true);
    view.addEventListener('resize', resize);
    return () => {
      document.removeEventListener('pointerdown', outside, true);
      document.removeEventListener('scroll', scroll, true);
      view.removeEventListener('resize', resize);
    };
  }, [phase, minimumWidth, align]);
  useLayoutEffect(() => {
    if (!enabled) setPhase('closed');
  }, [enabled]);
  return (
    <>
      <Button
        variant="legacy"
        size="legacy"
        ref={trigger}
        className={triggerClassName}
        type="button"
        title={label}
        aria-label={label}
        aria-haspopup="menu"
        aria-expanded={phase === 'open'}
        disabled={!enabled}
        onClick={() => (phase === 'closed' ? setPhase('open') : close())}
      >
        {triggerContent}
      </Button>
      {phase !== 'closed' && (
        <FullscreenTransient>
          <div
            ref={menu}
            className={`room-action-menu ${menuClassName}`}
            data-closing={phase === 'closing' || undefined}
            role="menu"
            aria-label={label}
            onKeyDown={(event) => {
              if (event.key === 'Escape') {
                event.preventDefault();
                close();
              } else if (event.key === 'Tab') close(false);
              else if (['ArrowDown', 'ArrowUp', 'Home', 'End'].includes(event.key)) {
                event.preventDefault();
                const buttons = [...event.currentTarget.querySelectorAll<HTMLButtonElement>('button:not(:disabled)')],
                  index = buttons.indexOf(document.activeElement as HTMLButtonElement);
                buttons[
                  event.key === 'Home'
                    ? 0
                    : event.key === 'End'
                      ? buttons.length - 1
                      : (index + (event.key === 'ArrowDown' ? 1 : -1) + buttons.length) % buttons.length
                ]?.focus();
              }
            }}
          >
            {items.map((item) => (
              <Button
                variant="legacy"
                size="legacy"
                key={item.id}
                type="button"
                role="menuitem"
                data-phrase={item.id}
                data-danger={item.danger || undefined}
                disabled={item.disabled}
                onClick={() => {
                  close();
                  item.select();
                }}
              >
                {item.label}
              </Button>
            ))}
          </div>
        </FullscreenTransient>
      )}
    </>
  );
}
