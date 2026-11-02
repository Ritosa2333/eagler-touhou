import {createContext, forwardRef, useContext, useEffect, useRef, useState, type ComponentPropsWithoutRef, type ReactElement, type ReactNode} from 'react';
import {DropdownMenu} from 'radix-ui';
import {uiPortalHost} from './portal';
import {cn} from './utils';
import {usePopupPadding} from './use-popup-padding';
const MenuAction = createContext<((action: () => void) => void) | null>(null);

/** Menus share placement, keyboard navigation, theme scope and focus return. */
export function ActionMenu({label, trigger, children, className, minimumWidth = 200, side = 'bottom', align = 'end'}: {
  label: string; trigger: ReactElement; children: ReactNode; className?: string; minimumWidth?: number;
  side?: 'top' | 'right' | 'bottom' | 'left'; align?: 'start' | 'center' | 'end';
}) {
  const anchor = useRef<HTMLButtonElement>(null);
  const afterClose = useRef<(() => void) | null>(null);
  const [open, setOpen] = useState(false), [host, setHost] = useState<HTMLElement>(), [surface, setSurface] = useState<string>();
  const padding = usePopupPadding(open);
  useEffect(() => {
    if (!open) return;
    const close = () => setOpen(false);
    document.addEventListener('fullscreenchange', close); document.addEventListener('webkitfullscreenchange', close);
    return () => {document.removeEventListener('fullscreenchange', close);document.removeEventListener('webkitfullscreenchange', close);};
  }, [open]);
  return <MenuAction.Provider value={action=>{afterClose.current=action;}}><DropdownMenu.Root modal={false} open={open} onOpenChange={next => {
    if (next) {setHost(uiPortalHost(anchor.current));setSurface(anchor.current?.closest('[data-ui-surface]')?.getAttribute('data-ui-surface') ?? undefined);}
    setOpen(next);
  }}><DropdownMenu.Trigger asChild ref={anchor}>{trigger}</DropdownMenu.Trigger>
    <DropdownMenu.Portal container={host}><DropdownMenu.Content className={cn('ui-menu', className)} aria-label={label} data-ui-surface={surface}
      side={side} align={align} sideOffset={6} collisionPadding={padding} style={{minWidth: minimumWidth}} onCloseAutoFocus={event=>{
        const action=afterClose.current;afterClose.current=null;
        if(action){event.preventDefault();anchor.current?.focus({preventScroll:true});action();}
      }}>{children}</DropdownMenu.Content></DropdownMenu.Portal>
  </DropdownMenu.Root></MenuAction.Provider>;
}
export const ActionMenuItem = forwardRef<HTMLDivElement, ComponentPropsWithoutRef<typeof DropdownMenu.Item> & {destructive?: boolean; onAction?(): void}>(function ActionMenuItem({className, destructive, onSelect, onAction, ...props}, ref) {
  const queue=useContext(MenuAction);
  return <DropdownMenu.Item {...props} ref={ref} className={cn('ui-menu-item', className)} data-danger={destructive || undefined} onSelect={event=>{
    onSelect?.(event);
    // Focus the surviving trigger before a dialog or navigation acquires focus.
    if(!event.defaultPrevented&&onAction){if(queue)queue(onAction);else onAction();}
  }}/>;
});
