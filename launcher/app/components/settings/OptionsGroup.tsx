import {Collapsible} from 'radix-ui';
import {ChevronDown} from 'lucide-react';
import {Button, type ButtonProps} from '../../ui/button';
import {forwardRef, type ReactNode} from 'react';

export const OptionsGroupHeading = forwardRef<HTMLButtonElement, ButtonProps>(function OptionsGroupHeading(
  {children, className = '', ...props}, ref,
) {
  return <Button {...props} ref={ref} variant="ghost" className={`options-group-head h-auto min-h-11 w-full justify-between rounded-none border-0 px-0 py-2 text-sm font-semibold text-foreground hover:bg-transparent ${className}`.trim()}><span>{children}</span><ChevronDown className="options-group-chevron" aria-hidden="true"/></Button>;
});

export interface OptionsGroupProps {
  id: string;
  title: ReactNode;
  open: boolean;
  onOpenChange(open: boolean): void;
  bodyClassName: string;
  children: ReactNode;
}
/** The settings model owns disclosure state; content stays mounted for refs. */
export function OptionsGroup({id, title, open, onOpenChange, bodyClassName, children}: OptionsGroupProps) {
  return <Collapsible.Root className="options-group" id={id} open={open} onOpenChange={onOpenChange}>
    <Collapsible.Trigger asChild><OptionsGroupHeading>{title}</OptionsGroupHeading></Collapsible.Trigger>
    <Collapsible.Content forceMount hidden={!open} className={bodyClassName}>{children}</Collapsible.Content>
  </Collapsible.Root>;
}
