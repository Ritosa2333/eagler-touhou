import {cloneElement, useEffect, useRef, useState, type MouseEvent, type PointerEvent, type ReactElement, type ReactNode} from 'react';
import {Popover, PopoverContent, PopoverTrigger} from '../../ui/popover';

export type PlayerCardTrigger = ReactElement<{
  onClick?: (event: MouseEvent) => void;
  onPointerEnter?: (event: PointerEvent) => void;
  onPointerLeave?: (event: PointerEvent) => void;
}>;

const sideOffset = 6;
const collisionPadding = 12;
const fullCardHeight = 200;
type OpenCard = {
  mode: 'hover' | 'click';
  side: 'top' | 'bottom';
  maxHeight: number;
};

function openCard(trigger: Element, mode: OpenCard['mode']): OpenCard {
  const rect = trigger.getBoundingClientRect();
  const viewport = window.visualViewport;
  const viewportTop = viewport?.offsetTop ?? 0;
  const viewportBottom = viewportTop + (viewport?.height ?? document.documentElement.clientHeight);
  const above = rect.top - viewportTop - sideOffset - collisionPadding;
  const below = viewportBottom - rect.bottom - sideOffset - collisionPadding;
  const side = below >= fullCardHeight || below >= above ? 'bottom' : 'top';
  return {mode, side, maxHeight: Math.max(0, Math.floor(side === 'bottom' ? below : above))};
}

export function PlayerCardPopover({trigger, label, children}: {trigger: PlayerCardTrigger; label: string; children: ReactNode}) {
  const [card,setCard] = useState<OpenCard>();
  const timer = useRef<ReturnType<typeof setTimeout>>(undefined);
  const clear = () => clearTimeout(timer.current);
  const close = () => {clear();setCard(undefined);};
  const leave = () => {
    clear();
    timer.current = setTimeout(()=>setCard(card=>card?.mode === 'hover' ? undefined : card),220);
  };
  useEffect(()=>()=>clearTimeout(timer.current),[]);
  return <Popover open={!!card} onOpenChange={open=>{if (!open) close();}}>
    <PopoverTrigger asChild>{cloneElement(trigger,{
      onPointerEnter(event) {
        if (event.pointerType !== 'mouse') return;
        clear();
        if (card) return;
        const anchor = event.currentTarget;
        timer.current = setTimeout(()=>setCard(openCard(anchor, 'hover')),180);
      },
      onPointerLeave:leave,
      onClick(event) {
        event.preventDefault();
        event.stopPropagation();
        clear();
        if (card?.mode === 'click') close();
        else setCard(card ? {...card, mode:'click'} : openCard(event.currentTarget, 'click'));
      },
    })}</PopoverTrigger>
    <PopoverContent className="player-profile-popover" aria-label={label}
      side={card?.side} sideOffset={sideOffset} collisionPadding={collisionPadding} style={{maxHeight:card?.maxHeight}}
      onOpenAutoFocus={event=>event.preventDefault()}
      onCloseAutoFocus={event=>{if (document.activeElement instanceof HTMLElement && !document.activeElement.closest('.player-profile-popover')) event.preventDefault();}} onPointerEnter={clear} onPointerLeave={leave}>
      {children}
    </PopoverContent>
  </Popover>;
}
