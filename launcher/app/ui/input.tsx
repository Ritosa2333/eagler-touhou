import {forwardRef, type ComponentPropsWithoutRef} from 'react';
import {cn} from './utils';

const inputClasses = 'ui-input w-full min-w-0 rounded-md border border-solid border-input bg-background font-sans text-sm text-foreground shadow-none outline-none placeholder:text-muted-foreground focus-visible:border-ring focus-visible:ring-2 focus-visible:ring-ring/25 disabled:cursor-not-allowed disabled:opacity-50 aria-invalid:border-destructive aria-invalid:ring-destructive/20';

export const Input = forwardRef<HTMLInputElement, ComponentPropsWithoutRef<'input'>>(function Input({className, ...props}, ref) {
  return <input data-slot="input" {...props} ref={ref} className={cn(inputClasses, 'h-9 px-3 py-2 pointer-coarse:min-h-11', className)}/>;
});

export const Textarea = forwardRef<HTMLTextAreaElement, ComponentPropsWithoutRef<'textarea'>>(function Textarea({className, ...props}, ref) {
  return <textarea data-slot="textarea" {...props} ref={ref} className={cn(inputClasses, 'min-h-24 resize-y px-3 py-2 leading-relaxed', className)}/>;
});
