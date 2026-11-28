import {createContext, forwardRef, useContext, useEffect, useRef, useState, type ComponentPropsWithoutRef, type ReactElement, type ReactNode} from 'react';
import {DropdownMenu} from 'radix-ui';
import {Check} from 'lucide-react';
import {uiPortalHost} from './portal';
import {cn} from './utils';
import {usePopupPadding} from './use-popup-padding';
import {listPopupPanelClass,listPopupItemClass,listPopupIndicatorClass} from './list-popup';
import './menu-motion.css';
const MenuAction = createContext<((action: () => void) => void) | null>(null);
const MenuVariant=createContext<'action'|'selection'>('action');

/** Menus share placement, keyboard navigation, theme scope and focus return. */
export function ActionMenu({label, trigger, children, className, minimumWidth = 200, side = 'bottom', align = 'end', sideOffset = 6,variant='action',openOnClick=false, open:controlledOpen, onOpenChange}: {
  label: string; trigger: ReactElement; children: ReactNode; className?: string; minimumWidth?: number;
  side?: 'top' | 'right' | 'bottom' | 'left'; align?: 'start' | 'center' | 'end'; sideOffset?: number;
  variant?: 'action'|'selection'; openOnClick?: boolean; open?:boolean; onOpenChange?(open:boolean):void;
}) {
  const anchor = useRef<HTMLButtonElement>(null);
  const afterClose = useRef<(() => void) | null>(null);
  const [internalOpen, setOpen] = useState(false), [host, setHost] = useState<HTMLElement>(), [surface, setSurface] = useState<string>();
  const open = controlledOpen ?? internalOpen;
  const padding = usePopupPadding(open);
  useEffect(() => {if (open) {setHost(uiPortalHost(anchor.current));setSurface(anchor.current?.closest('[data-ui-surface]')?.getAttribute('data-ui-surface') ?? undefined);}}, [open]);
  useEffect(() => {
    if (!open) return;
    const close = () => {setOpen(false); onOpenChange?.(false);};
    document.addEventListener('fullscreenchange', close); document.addEventListener('webkitfullscreenchange', close);
    return () => {document.removeEventListener('fullscreenchange', close);document.removeEventListener('webkitfullscreenchange', close);};
  }, [open, onOpenChange]);
  function changeOpen(next: boolean) {
    if (next) {setHost(uiPortalHost(anchor.current));setSurface(anchor.current?.closest('[data-ui-surface]')?.getAttribute('data-ui-surface') ?? undefined);}
    setOpen(next);
    onOpenChange?.(next);
  }
  return <MenuVariant value={variant}><MenuAction.Provider value={action=>{afterClose.current=action;}}><DropdownMenu.Root modal={false} open={open} onOpenChange={changeOpen}>
    <DropdownMenu.Trigger asChild ref={anchor}
      onPointerDownCapture={openOnClick ? event => event.stopPropagation() : undefined}
      onClick={openOnClick ? () => changeOpen(!open) : undefined}>{trigger}</DropdownMenu.Trigger>
    <DropdownMenu.Portal container={host}><DropdownMenu.Content className={cn(variant==='selection'?`${listPopupPanelClass} overflow-y-auto`:'ui-menu', 'ui-dropdown-motion', className)} aria-label={label} data-ui-surface={surface}
      side={side} align={align} sideOffset={sideOffset} collisionPadding={padding} style={{minWidth:`min(${minimumWidth}px, var(--radix-dropdown-menu-content-available-width, calc(100vw - 24px)))`,...(variant==='selection'?{maxHeight:'min(440px,var(--radix-dropdown-menu-content-available-height))'}:{})}} onCloseAutoFocus={event=>{
        const action=afterClose.current;afterClose.current=null;
        if(action){event.preventDefault();anchor.current?.focus({preventScroll:true});action();}
      }}>{children}</DropdownMenu.Content></DropdownMenu.Portal>
  </DropdownMenu.Root></MenuAction.Provider></MenuVariant>;
}
export const ActionMenuItem = forwardRef<HTMLDivElement, ComponentPropsWithoutRef<typeof DropdownMenu.Item> & {destructive?: boolean; onAction?(): void}>(function ActionMenuItem({className, destructive, onSelect, onAction, ...props}, ref) {
  const queue=useContext(MenuAction);
  const variant=useContext(MenuVariant);
  return <DropdownMenu.Item {...props} ref={ref} className={cn(variant==='selection'?listPopupItemClass:'ui-menu-item', className)} data-danger={destructive || undefined} onSelect={event=>{
    onSelect?.(event);
    // Focus the surviving trigger before a dialog or navigation acquires focus.
    if(!event.defaultPrevented&&onAction){if(queue)queue(onAction);else onAction();}
  }}/>;
});

export const ActionMenuRadioGroup=DropdownMenu.RadioGroup;
export const ActionMenuGroup=DropdownMenu.Group;
export const ActionMenuLabel=DropdownMenu.Label;
export const ActionMenuRadioItem=forwardRef<HTMLDivElement,ComponentPropsWithoutRef<typeof DropdownMenu.RadioItem>>(function ActionMenuRadioItem({children,className,...props},ref){
  return <DropdownMenu.RadioItem {...props} ref={ref} className={cn(listPopupItemClass,className)}>{children}<DropdownMenu.ItemIndicator className={listPopupIndicatorClass}><Check className="size-4"/></DropdownMenu.ItemIndicator></DropdownMenu.RadioItem>;
});
export const ActionMenuCheckboxItem=forwardRef<HTMLDivElement,ComponentPropsWithoutRef<typeof DropdownMenu.CheckboxItem>&{showIndicator?:boolean}>(function ActionMenuCheckboxItem({children,className,showIndicator=true,...props},ref){
  return <DropdownMenu.CheckboxItem {...props} ref={ref} className={cn(listPopupItemClass,className)}>{children}{showIndicator&&<DropdownMenu.ItemIndicator className={listPopupIndicatorClass}><Check className="size-4"/></DropdownMenu.ItemIndicator>}</DropdownMenu.CheckboxItem>;
});
