import {createContext, useContext, useRef, useState, forwardRef, type ComponentPropsWithoutRef, type RefObject} from 'react';
import {Select as Primitive} from 'radix-ui';
import {Check, ChevronDown, ChevronUp} from 'lucide-react';
import {cn} from './utils';
import {uiPortalHost} from './portal';

const Context = createContext<{trigger: RefObject<HTMLButtonElement | null>; host?: HTMLElement}>({trigger: {current: null}});
export function Select({onOpenChange, ...props}: ComponentPropsWithoutRef<typeof Primitive.Root>) {
  const trigger = useRef<HTMLButtonElement>(null), [host, setHost] = useState<HTMLElement>();
  return <Context value={{trigger, host}}><Primitive.Root {...props} onOpenChange={open => {if (open) setHost(uiPortalHost(trigger.current)); onOpenChange?.(open);}}/></Context>;
}
export const SelectValue = Primitive.Value;
export function SelectTrigger({children, className, ...props}: ComponentPropsWithoutRef<typeof Primitive.Trigger>) {
  const {trigger} = useContext(Context);
  return <Primitive.Trigger {...props} ref={trigger} data-slot="select-trigger" className={cn('ui-kit-select-trigger inline-flex h-9 items-center justify-between gap-3 rounded-md border border-solid border-input bg-background px-3 font-sans text-sm text-foreground outline-none transition-colors hover:bg-accent focus-visible:ring-2 focus-visible:ring-ring disabled:pointer-events-none disabled:opacity-50', className)}>
    {children}<Primitive.Icon asChild><ChevronDown className="size-4 text-muted-foreground"/></Primitive.Icon>
  </Primitive.Trigger>;
}
export function SelectContent({children, className, ...props}: ComponentPropsWithoutRef<typeof Primitive.Content>) {
  const {host} = useContext(Context);
  return <Primitive.Portal container={host}><Primitive.Content {...props} position="popper" sideOffset={6} collisionPadding={12} data-slot="select-content"
    className={cn('ui-kit-select-content z-[var(--ui-layer-popup)] flex min-w-[var(--radix-select-trigger-width)] flex-col overflow-hidden rounded-md border border-solid border-border bg-popover p-1 font-sans text-sm text-popover-foreground shadow-lg', className)}>
    <Primitive.ScrollUpButton className="flex justify-center py-1"><ChevronUp className="size-4"/></Primitive.ScrollUpButton>
    <Primitive.Viewport className="min-h-0 flex-1">{children}</Primitive.Viewport>
    <Primitive.ScrollDownButton className="flex justify-center py-1"><ChevronDown className="size-4"/></Primitive.ScrollDownButton>
  </Primitive.Content></Primitive.Portal>;
}
export const SelectItem = forwardRef<HTMLDivElement, ComponentPropsWithoutRef<typeof Primitive.Item>>(function SelectItem({children, className, ...props}, ref) {
  return <Primitive.Item {...props} ref={ref} data-slot="select-item" className={cn('relative flex min-h-9 cursor-default items-center rounded-sm py-2 pr-8 pl-2 outline-none select-none data-[highlighted]:bg-accent data-[disabled]:pointer-events-none data-[disabled]:opacity-50', className)}>
    <Primitive.ItemText>{children}</Primitive.ItemText><Primitive.ItemIndicator className="absolute right-2 flex"><Check className="size-4"/></Primitive.ItemIndicator>
  </Primitive.Item>;
});
