import {forwardRef, type ComponentPropsWithoutRef} from 'react';
import {Tabs as Primitive} from 'radix-ui';
import {cn} from './utils';

export const Tabs = Primitive.Root;
export const TabsContent = Primitive.Content;
export const TabsList = forwardRef<HTMLDivElement, ComponentPropsWithoutRef<typeof Primitive.List>>(function TabsList({className, ...props}, ref) {
  return <Primitive.List {...props} ref={ref} className={cn('ui-tabs-list', className)}/>;
});
export const TabsTrigger = forwardRef<HTMLButtonElement, ComponentPropsWithoutRef<typeof Primitive.Trigger>>(function TabsTrigger({className, ...props}, ref) {
  return <Primitive.Trigger {...props} ref={ref} className={cn('ui-tabs-trigger', className)}/>;
});
