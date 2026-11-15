import {forwardRef, type ComponentPropsWithoutRef} from 'react';
import {cn} from './utils';
import {Input} from './input';

export function InputGroup({className, ...props}: ComponentPropsWithoutRef<'div'>) {
  return <div {...props} data-slot="input-group" className={cn('ui-kit-input-group flex min-w-0 items-center gap-2 rounded-md border border-solid border-input bg-background px-3 text-muted-foreground transition-shadow focus-within:border-ring focus-within:ring-2 focus-within:ring-ring/25 [&>svg]:size-4 [&>svg]:shrink-0', className)}/>;
}
export const InputGroupInput = forwardRef<HTMLInputElement, ComponentPropsWithoutRef<'input'>>(function InputGroupInput({className, ...props}, ref) {
  return <Input {...props} ref={ref} data-slot="input-group-control" className={cn('ui-kit-input h-10 flex-1 rounded-none border-0 bg-transparent p-0 shadow-none focus-visible:border-0 focus-visible:ring-0 disabled:opacity-100', className)}/>;
});
