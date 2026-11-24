import {Collapsible} from 'radix-ui';
import {ChevronDown, type LucideIcon} from 'lucide-react';
import {Button, type ButtonProps} from '../../ui/button';
import {forwardRef, type ReactNode} from 'react';

export const OptionsGroupHeading = forwardRef<HTMLButtonElement, ButtonProps & {icon?: LucideIcon}>(function OptionsGroupHeading(
  {children, icon: Icon, className = '', ...props}, ref,
) {
  return <Button {...props} ref={ref} variant="ghost" className={`options-group-head h-auto min-h-11 w-full justify-between rounded-none border-0 px-0 py-2 text-sm font-semibold text-foreground hover:bg-transparent ${className}`.trim()}>
    <span className="inline-flex min-w-0 items-center gap-2.5">{Icon && <Icon aria-hidden="true"/>}{children}</span>
    <ChevronDown className="options-group-chevron" aria-hidden="true"/>
  </Button>;
});

export interface OptionsGroupProps {
  id: string;
  title: ReactNode;
  icon?: LucideIcon;
  open: boolean;
  onOpenChange(open: boolean): void;
  animated?: boolean;
  bodyClassName: string;
  children: ReactNode;
}
/** The settings model owns disclosure state; content stays mounted for refs. */
export function OptionsGroup({id, title, icon, open, onOpenChange, animated = false, bodyClassName, children}: OptionsGroupProps) {
  return <Collapsible.Root className="options-group" id={id} open={open} onOpenChange={onOpenChange}>
    <Collapsible.Trigger asChild><OptionsGroupHeading icon={icon}>{title}</OptionsGroupHeading></Collapsible.Trigger>
    {animated ? <Collapsible.Content className="settings-disclosure-content" inert={!open}>
      <div className={bodyClassName}>{children}</div>
    </Collapsible.Content> : <Collapsible.Content forceMount hidden={!open} className={bodyClassName}>{children}</Collapsible.Content>}
  </Collapsible.Root>;
}
