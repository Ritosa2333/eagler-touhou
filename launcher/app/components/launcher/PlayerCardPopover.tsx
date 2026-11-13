import {cloneElement, useEffect, useRef, useState, type MouseEvent, type PointerEvent, type ReactElement, type ReactNode} from 'react';
import {Popover, PopoverContent, PopoverTrigger} from '../../ui/popover';

export type PlayerCardTrigger = ReactElement<{
  onClick?: (event: MouseEvent) => void;
  onPointerEnter?: (event: PointerEvent) => void;
  onPointerLeave?: (event: PointerEvent) => void;
}>;

export function PlayerCardPopover({trigger, label, children}: {trigger: PlayerCardTrigger; label: string; children: ReactNode}) {
  const [mode,setMode] = useState<'hover'|'click'>();
  const timer = useRef<ReturnType<typeof setTimeout>>(undefined);
  const clear = () => clearTimeout(timer.current);
  const close = () => {clear();setMode(undefined);};
  const leave = () => {
    clear();
    timer.current = setTimeout(()=>setMode(mode=>mode === 'hover' ? undefined : mode),220);
  };
  useEffect(()=>()=>clearTimeout(timer.current),[]);
  return <Popover open={!!mode} onOpenChange={open=>{if (!open) close();}}>
    <PopoverTrigger asChild>{cloneElement(trigger,{
      onPointerEnter(event) {
        if (event.pointerType !== 'mouse') return;
        clear();
        if (!mode) timer.current = setTimeout(()=>setMode('hover'),180);
      },
      onPointerLeave:leave,
      onClick(event) {
        event.preventDefault();
        event.stopPropagation();
        clear();
        setMode(mode=>mode === 'click' ? undefined : 'click');
      },
    })}</PopoverTrigger>
    <PopoverContent className="player-profile-popover" aria-label={label}
      onOpenAutoFocus={event=>event.preventDefault()}
      onCloseAutoFocus={event=>{if (document.activeElement instanceof HTMLElement && !document.activeElement.closest('.player-profile-popover')) event.preventDefault();}} onPointerEnter={clear} onPointerLeave={leave}>
      {children}
    </PopoverContent>
  </Popover>;
}
