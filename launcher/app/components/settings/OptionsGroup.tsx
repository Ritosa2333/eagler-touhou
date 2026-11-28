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
  id?: string;
  title: ReactNode;
  icon?: LucideIcon;
  open?: boolean;
  onOpenChange?(open: boolean): void;
  className?: string;
  triggerId?: string;
  bodyId?: string;
  bodyClassName: string;
  children: ReactNode;
}
/** Omit disclosure state for sections on a dedicated settings page. */
export function OptionsGroup({id, title, icon: Icon, open, onOpenChange, className, triggerId, bodyId, bodyClassName, children}: OptionsGroupProps) {
  const classes = className ? `options-group ${className}` : 'options-group';
  if (open === undefined) return <section className={classes} id={id}>
    <h3 className="options-group-head options-group-title"><span>{Icon && <Icon aria-hidden="true"/>}{title}</span></h3>
    <div id={bodyId} className={`options-group-body ${bodyClassName}`}>{children}</div>
  </section>;
  return <Collapsible.Root className={classes} id={id} open={open} onOpenChange={onOpenChange}>
    <Collapsible.Trigger asChild><OptionsGroupHeading id={triggerId} {...(bodyId && {'aria-controls':bodyId})} icon={Icon}>{title}</OptionsGroupHeading></Collapsible.Trigger>
    <Collapsible.Content {...(bodyId && {id:bodyId})} forceMount hidden={!open} className={bodyClassName}>{children}</Collapsible.Content>
  </Collapsible.Root>;
}
