import {createContext, useContext, useRef, useState, type ComponentPropsWithoutRef, type RefObject} from 'react';
import {Popover as Primitive} from 'radix-ui';
import {uiPortalHost} from './portal';
import {cn} from './utils';

const Context = createContext<{trigger: RefObject<HTMLButtonElement | null>; host?: HTMLElement}>({trigger: {current: null}});
export const PopoverClose = Primitive.Close;
export function Popover({onOpenChange, ...props}: ComponentPropsWithoutRef<typeof Primitive.Root>) {
  const trigger = useRef<HTMLButtonElement>(null), [host, setHost] = useState<HTMLElement>();
  return <Context value={{trigger, host}}><Primitive.Root {...props} onOpenChange={open => {if (open) setHost(uiPortalHost(trigger.current)); onOpenChange?.(open);}}/></Context>;
}
export function PopoverTrigger(props: ComponentPropsWithoutRef<typeof Primitive.Trigger>) {
  return <Primitive.Trigger {...props} ref={useContext(Context).trigger}/>;
}
export function PopoverContent({className, ...props}: ComponentPropsWithoutRef<typeof Primitive.Content>) {
  return <Primitive.Portal container={useContext(Context).host}><Primitive.Content sideOffset={6} collisionPadding={12} align="start" {...props} className={cn('ui-menu', className)}/></Primitive.Portal>;
}
